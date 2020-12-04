/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS L2 Table Manipulation API routines.
 *
 * The L2Xm pseudo-table is an aggregate structure supported by
 * hardware.  When an entry is read from it, it is constructed from the
 * L2X_BASEm, L2X_VALIDm, L2X_HITm, and L2X_STATICm tables.
 * NOTE: The L2X pseudo-table is read-only.
 *
 * A low-level L2 shadow table is optionally kept in soc->arlShadow by
 * using a callback to get all inserts/deletes from the l2xmsg task.  It
 * can be disabled by setting the l2xmsg_avl property to 0.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ptable.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/tucana.h>
#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/triumph3.h>
#endif
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/tomahawk3.h>
#endif

#define DEFAULT_L2DELETE_CHUNKS		(64)	/* 16k entries / 64 = 256 */
            
#ifdef BCM_XGS_SWITCH_SUPPORT

/*
 * While the L2 table is frozen, the L2Xm lock is held.
 *
 * All tasks must obtain the L2Xm lock before modifying the CML bits or
 * age timer registers.
 */

typedef struct l2_freeze_s {
    int                 frozen;
    int                 save_age_sec;
    int                 save_age_ena;
    int                 new_age_set;
    int                 hw_frozen;
} l2_freeze_t;

typedef struct cml_freeze_s {
    int                 frozen;
    int                 *save_cml; 
    int                 *save_cml_move; 
    int                 *save_vp_cml;
    int                 *save_vp_cml_move; 
    SHR_BITDCL          *vp_bitmap;         /* Managed in BCM layer */
#ifdef BCM_TRIDENT2_SUPPORT
    sal_mutex_t         cml_freeze_mutex;
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    soc_l2x_cml_cb_fn   cml_func;  /*callback function to set lport_tab in TD3*/
#endif
} cml_freeze_t;

l2_freeze_t l2_freeze_state[SOC_MAX_NUM_DEVICES];
cml_freeze_t cml_freeze_state[SOC_MAX_NUM_DEVICES];
static int l2_freeze_mode[SOC_MAX_NUM_DEVICES];

#ifdef BCM_TRIDENT2_SUPPORT
#define SOC_CML_FREEZE_LOCK(unit) \
    sal_mutex_take(cml_freeze_state[unit].cml_freeze_mutex, sal_mutex_FOREVER)

#define SOC_CML_FREEZE_UNLOCK(unit) \
    sal_mutex_give(cml_freeze_state[unit].cml_freeze_mutex)
#endif

#ifdef PLISIM
#ifdef _KATANA_DEBUG /* BCM_KATANA_SUPPORT */
STATIC uint32 port_age_enable=FALSE;
STATIC uint32 port_age_flags=0;
STATIC _bcm_l2_replace_t *port_age_rep_st=NULL;
#define SOC_L2X_TGID_TRUNK_INDICATOR(unit) \
        SOC_IS_RAVEN(unit) ? 0x40 : 0x20
#define SOC_L2X_TGID_PORT_TRUNK_MASK           0x1f
#define SOC_L2X_TGID_PORT_TRUNK_MASK_HI        0x60
#endif
#endif

/*
 * Function:
 *	soc_l2x_entry_compare_key
 * Purpose:
 *	Comparison function for AVL shadow table operations.  Compares
 *	key portion of entry only.
 * Parameters:
 *	user_data - used to pass StrataSwitch unit #
 *	datum1 - first L2X entry to compare
 *	datum2 - second L2X entry to compare
 * Returns:
 *	datum1 <=> datum2
 */

int
soc_l2x_entry_compare_key(void *user_data,
                          shr_avl_datum_t *datum1,
                          shr_avl_datum_t *datum2)
{
    int		unit = PTR_TO_INT(user_data);

    return soc_mem_compare_key(unit, L2Xm, datum1, datum2);
}

/*
 * Function:
 *	soc_l2x_entry_compare_all
 * Purpose:
 *	Comparison function for AVL shadow table operations.  Compares
 *	entire key+data of entry, but key is still most significant.
 * Parameters:
 *	user_data - used to pass StrataSwitch unit #
 *	datum1 - first L2X entry to compare
 *	datum2 - second L2X entry to compare
 * Returns:
 *	datum1 <=> datum2
 */

int
soc_l2x_entry_compare_all(void *user_data,
                          shr_avl_datum_t *datum1,
                          shr_avl_datum_t *datum2)
{
    int		unit = PTR_TO_INT(user_data);
    int		i;

    i = soc_mem_compare_key(unit, L2Xm, datum1, datum2);

    if (i == 0) {
        i = sal_memcmp(datum1, datum2, sizeof (l2x_entry_t));
    }

    return i;
}

/*
 * Function:
 *	soc_l2x_entry_dump
 * Purpose:
 *	Debug dump function for AVL shadow table operations.
 * Parameters:
 *	user_data - used to pass StrataSwitch unit #
 *	datum - L2X entry to dump
 *	extra_data - unused
 * Returns:
 *	SOC_E_XXX
 */

int
soc_l2x_entry_dump(void *user_data, shr_avl_datum_t *datum, void *extra_data)
{
    int		unit = PTR_TO_INT(user_data);

    COMPILER_REFERENCE(extra_data);

    soc_mem_entry_dump(unit, L2Xm, (l2x_entry_t *) datum, BSL_LSS_CLI);
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2x_shadow_callback
 * Purpose:
 *	Internal callback routine for updating an AVL tree shadow table
 * Parameters:
 *	unit - StrataSwitch unit number.
 *	entry_del - Entry to be deleted or updated, NULL if none.
 *	entry_add - Entry to be inserted or updated, NULL if none.
 *	fn_data - unused.
 * Notes:
 *	Used only if L2X shadow table is enabled.
 */

STATIC void
soc_l2x_shadow_callback(int unit,
                        int flags,
                        l2x_entry_t *entry_del,
                        l2x_entry_t *entry_add,
                        void *fn_data)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    if (flags & (SOC_L2X_ENTRY_DUMMY | SOC_L2X_ENTRY_NO_ACTION |
                 SOC_L2X_ENTRY_OVERFLOW)) {
        return;
    }

    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);

    if (entry_del != NULL) {
        shr_avl_delete(soc->arlShadow, soc_l2x_entry_compare_key,
                       (shr_avl_datum_t *)entry_del);
    }

    if (entry_add != NULL) {
        shr_avl_insert(soc->arlShadow, soc_l2x_entry_compare_key,
                       (shr_avl_datum_t *)entry_add);
    }

    sal_mutex_give(soc->arlShadowMutex);
}

/*
 * Function:
 *	_soc_l2x_cml_struct_free
 * Purpose:
 *	Frees cml_freeze_state structure for a given unit 
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	    NONE 
 */

void
_soc_l2x_cml_struct_free(int unit)
{
    if (NULL != cml_freeze_state[unit].save_cml) {
        sal_free(cml_freeze_state[unit].save_cml);
        cml_freeze_state[unit].save_cml = NULL;
    }
    if (NULL != cml_freeze_state[unit].save_cml_move) {
        sal_free(cml_freeze_state[unit].save_cml_move);
        cml_freeze_state[unit].save_cml_move = NULL;
    }
    if (NULL != cml_freeze_state[unit].save_vp_cml) {
        sal_free(cml_freeze_state[unit].save_vp_cml);
        cml_freeze_state[unit].save_vp_cml = NULL;
    }
    if (NULL != cml_freeze_state[unit].save_vp_cml_move) {
        sal_free(cml_freeze_state[unit].save_vp_cml_move);
        cml_freeze_state[unit].save_vp_cml_move = NULL;
    }

    /* We DO NOT free the vp_bitmap here because it is a copy
     * of a BCM-layer pointer for SOC reference.
     * We do mark it NULL for safety.
     */
    cml_freeze_state[unit].vp_bitmap = NULL;
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        cml_freeze_state[unit].cml_func = NULL;
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        if (cml_freeze_state[unit].cml_freeze_mutex != NULL) {
            sal_mutex_destroy(cml_freeze_state[unit].cml_freeze_mutex);
            cml_freeze_state[unit].cml_freeze_mutex = NULL;
        }
    }
#endif
    return;
}

/*
 * Function:
 *	_soc_l2x_cml_struct_alloc
 * Purpose:
 *	Allocates cml_freeze_state structure for a given unit 
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_NONE - Allocated succesfully
 *  SOC_E_MEMORY - Allocation failed
 */

int
_soc_l2x_cml_struct_alloc(int unit)
{
    int cml_alloc_size, num_ports = 0;
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        num_ports = (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS) ?
                    SOC_MAX_NUM_PP_PORTS : SOC_MAX_NUM_PORTS;
    } else {
        num_ports = SOC_MAX_NUM_PORTS;
    }

    cml_alloc_size = sizeof(int) * num_ports;
    
    cml_freeze_state[unit].save_cml = (int *)sal_alloc(cml_alloc_size, "CML");
    if (NULL == cml_freeze_state[unit].save_cml) {
        goto e_memory;
    }
    sal_memset(cml_freeze_state[unit].save_cml, 0, cml_alloc_size);

    cml_freeze_state[unit].save_cml_move = 
        (int *)sal_alloc(cml_alloc_size, "CML MOVE");
    if (NULL == cml_freeze_state[unit].save_cml_move) {
        goto e_memory;
    }
    sal_memset(cml_freeze_state[unit].save_cml_move, 0, cml_alloc_size);

    if (SOC_MEM_IS_VALID(unit, SOURCE_VPm)) {
        cml_alloc_size = sizeof(int) * soc_mem_index_count(unit,SOURCE_VPm); 
        cml_freeze_state[unit].save_vp_cml = 
            (int *)sal_alloc(cml_alloc_size, "VP CML");
        if (NULL == cml_freeze_state[unit].save_vp_cml) {
            goto e_memory;
        }
        sal_memset(cml_freeze_state[unit].save_vp_cml, 0, cml_alloc_size);

        cml_freeze_state[unit].save_vp_cml_move = 
            (int *)sal_alloc(cml_alloc_size, "VP CML MOVE");
        if (NULL == cml_freeze_state[unit].save_vp_cml_move) {
            goto e_memory;
        }
        sal_memset(cml_freeze_state[unit].save_vp_cml_move, 0, cml_alloc_size);
    }
    /* We DO NOT initialize the vp_bitmap here because it is a copy
     * of a BCM-layer pointer for SOC reference.
     * We do mark it NULL for safety until the BCM layer passes
     * a valid pointer.
     */
    cml_freeze_state[unit].vp_bitmap = NULL;
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        cml_freeze_state[unit].cml_func = NULL;
    }
#endif/*BCM_TRIDENT3_SUPPORT*/

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        if ((cml_freeze_state[unit].cml_freeze_mutex =
            sal_mutex_create("soc_cml_freeze_lock")) == NULL) {
            return (SOC_E_MEMORY);
        }
    }
#endif
    return SOC_E_NONE;

e_memory:
    _soc_l2x_cml_struct_free(unit);

    return SOC_E_MEMORY;

}

/*
 * Function:
 *	soc_l2x_cml_vp_bitmap_set
 * Purpose:
 *	Record pointer to the in-use virtual port bitmap managed by
 *      the BCM layer.
 * Parameters:
 *	unit - StrataSwitch unit number.
 *      vp_bitmap - pointer to BCM-layer VP use bitmap.
 * Returns:
 *	Nothing
 */

void
soc_l2x_cml_vp_bitmap_set(int unit, SHR_BITDCL *vp_bitmap)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* On Tomahawk3, do not store the bitmap since VP memory is not supported */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return;
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
    {
        cml_freeze_state[unit].vp_bitmap = vp_bitmap;
    }
}

/*
 * Function:
 *	soc_l2x_attach
 * Purpose:
 *	Allocate L2X subsystem resources
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	The L2X tree shadow table is optional and its allocation
 *	is controlled using a property.
 */

int
soc_l2x_attach(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    soc->l2x_age_timeout = soc_property_get(unit, spn_ARL_CLEAN_TIMEOUT_USEC,
                                            15000000);
    soc->l2x_mode = soc_l2x_mode_cfg_get(unit);

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_l2_attach(unit);
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* FIFO mode is not supported, since h/w to support it does not exist
         * on TH3. So this property setting is ignored
         */
        soc->l2x_mode = L2MODE_POLL;
        return soc_th3_l2x_attach(unit);
    }
#endif

    (void)soc_l2x_detach(unit);

    if (soc_property_get(unit, spn_L2XMSG_AVL, TRUE)) {
        int		datum_bytes, datum_max;

        datum_bytes = sizeof (l2x_entry_t);
        datum_max = soc_mem_index_count(unit, L2Xm);

        if (shr_avl_create(&soc->arlShadow,
                           INT_TO_PTR(unit),
                           datum_bytes,
                           datum_max) < 0) {
            return SOC_E_MEMORY;
        }

        if ((soc->arlShadowMutex = sal_mutex_create("asMutex")) == NULL) {
            (void)soc_l2x_detach(unit);
            return SOC_E_MEMORY;
        }

        soc_l2x_register(unit, soc_l2x_shadow_callback, NULL);
    }

    /* Reset l2 freeze structure. */
    sal_memset(l2_freeze_state + unit, 0, sizeof(l2_freeze_t));

    /* Allocate cml freeze structure */
    return _soc_l2x_cml_struct_alloc(unit);
}

/*
 * Function:
 *	soc_l2x_detach
 * Purpose:
 *	Deallocate L2X subsystem resources
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 */

int
soc_l2x_detach(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_l2_detach(unit);
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return soc_th3_l2x_detach(unit);
    }
#endif

    soc_l2x_unregister(unit, soc_l2x_shadow_callback, NULL);

    /* Free cml_freeze structure  */
    _soc_l2x_cml_struct_free(unit);

    if (soc->arlShadow != NULL) {
        shr_avl_destroy(soc->arlShadow);
        soc->arlShadow = NULL;
    }

    if (soc->arlShadowMutex != NULL) {
        sal_mutex_destroy(soc->arlShadowMutex);
        soc->arlShadowMutex = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2x_init
 * Purpose:
 *	Initialize L2 table subsystem.
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_xxx
 * Notes:
 *	This routine must not do anything time consuming since it is
 *	called by soc_init().
 */

int
soc_l2x_init(int unit)
{
    /* The L2 table is cleared in soc_misc_init(). */
    return SOC_E_NONE;
}

#if defined(BCM_XGS12_SWITCH_SUPPORT)
/*
 * Function:
 *	soc_l2x_lookup
 * Purpose:
 *	Send an L2 lookup message over the S-Channel and receive the
 *	response.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	key - L2X entry to look up; only MAC+VLAN fields are relevant
 *	result - L2X entry to receive entire found entry
 *	index_ptr (OUT) - If found, receives table index where found
 * Returns:
 *	SOC_E_INTERNAL if retries exceeded or other internal error
 *	SOC_E_NOT_FOUND if the entry is not found.
 *	SOC_E_NONE (0) on success (entry found):
 * Notes:
 *	The S-Channel response contains either a matching L2 entry,
 *	or an entry with a key of all f's if not found.  It is okay
 *	if the result pointer is the same as the key pointer.
 *	Retries the lookup in cases where the particular chip requires it.
 */

int
soc_l2x_lookup(int unit,
               l2x_entry_t *key, l2x_entry_t *result,
               int *index_ptr)
{
    schan_msg_t 	schan_msg;
    int                 src_blk;
    int                 opcode;

    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    soc_schan_header_cmd_set(unit, &schan_msg.header, ARL_LOOKUP_CMD_MSG,
                             0, src_blk, 0, 8, 0, 0);  
    schan_msg.arllkup.address = soc_mem_addr(unit, L2Xm, 0, ARL_BLOCK(unit), 0);

    /* Fill in packet data */

    sal_memcpy(schan_msg.arllkup.data, key, sizeof (schan_msg.arllkup.data));

    /*
     * Write onto S-Channel "ARL lookup" command packet consisting of
     * header word + address + two words of ARL key, and read back header
     * word + 2 words of a special lookup result format.
     */

    SOC_IF_ERROR_RETURN(soc_schan_op(unit, &schan_msg, 4, 3, 1));

    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                NULL, NULL, NULL);
    if (opcode != READ_MEMORY_ACK_MSG) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_l2x_lookup: "
                              "invalid S-Channel reply, expected READ_MEMORY_ACK:\n")));
        soc_schan_dump(unit, &schan_msg, 3);
        return SOC_E_INTERNAL;
    }

    /*
     * Fill in result entry from read data.
     * Note that the lookup does not return the key, so it must be copied.
     *
     * Format of S-Channel response:
     *		readresp.header : Memory Read Ack S-channel header
     *		readresp.data[0]: { 4'h0, hit, static, l2_table_data }
     *		readresp.data[1]: { 18'h0, index_into_L2_table[13:0] }
     */

    if (schan_msg.readresp.data[0] == 0xffffffff &&
        schan_msg.readresp.data[1] == 0xffffffff) {
        *index_ptr = -1;
        return SOC_E_NOT_FOUND;
    }

    /* MACADDR<31:0> */
    result->entry_data[0] = key->entry_data[0];

    /* CPU, COS<2:0>, VLAN<12:0>, MACADDR<47:32> */
    result->entry_data[1] = ((schan_msg.readresp.data[0] << 28) |
                             (key->entry_data[1] & 0x0fffffff));

    /* ZERO<6:0>, VALID=1, HIT, STATIC .. DST_DISCARD */
    result->entry_data[2] = schan_msg.readresp.data[0] >> 4;
    result->entry_data[2] |= 1 << (88-64);

    *index_ptr = schan_msg.readresp.data[1];

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "L2 entry lookup: ")));
        soc_mem_entry_dump(unit, L2Xm, result, BSL_INFO|BSL_LS_SOC_SOCMEM);
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             " (index=%d)\n"), *index_ptr));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	_soc_l2x_hit_clear
 * Purpose:
 *	Clear the hit bit on a specified L2 entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number
 *	entry - Entry to operate on
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_NOT_FOUND - no such address
 *	SOC_E_XXX - other
 * Notes:
 *	The proper way to clear hit bits is to use soc_l2x_insert()
 *	with the hit bit clear.  This is just a helper routine for
 *	a 5690 problem.
 *
 *	5690 cannot clear hit bits atomically, hence there is a race
 *	condition where if other hit bit(s) got set in the same bucket
 *	between our read and write operations, they could also be
 *	cleared.
 */

int
_soc_l2x_hit_clear(int unit, l2x_entry_t *entry)
{
    l2x_entry_t		result;
    l2x_hit_entry_t	hit_bits;
    int			index, bucket, bit, r;

    if ((r = soc_l2x_lookup(unit, entry, &result, &index)) < 0) {
        return r;
    }

    bucket = index / SOC_L2X_BUCKET_SIZE;
    bit = index % SOC_L2X_BUCKET_SIZE;

    /* Read/modify/write bucket hit bits */

    soc_mem_lock(unit, L2X_HITm);

    if ((r = soc_mem_read(unit, L2X_HITm, MEM_BLOCK_ANY,
                          bucket, &hit_bits)) < 0) {
        soc_mem_unlock(unit, L2X_HITm);
        return r;
    }

    hit_bits.entry_data[0] &= ~(1 << bit);

    if ((r = soc_mem_write(unit, L2X_HITm, MEM_BLOCK_ANY,
                           bucket, &hit_bits)) < 0) {
        soc_mem_unlock(unit, L2X_HITm);
        return r;
    }

    soc_mem_unlock(unit, L2X_HITm);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2x_insert
 * Purpose:
 *	Insert an entry into the L2X hash table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	entry - L2X entry to insert
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_FULL - hash bucket full
 * Notes:
 *	Uses hardware insertion; sends an L2 INSERT message over the
 *	S-Channel.  The hardware needs the L2Xm entry type.
 *	This affects the L2_ENTRYm table, L2_VALIDm table, L2_STATICm
 *	table, and the L2_HITm table.
 */

int
soc_l2x_insert(int unit, l2x_entry_t *entry)
{
    l2x_entry_t         valid_entry;
    schan_msg_t 	schan_msg;
    int			rv;
    int                 src_blk;

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "Insert table[L2X]: ")));
        soc_mem_entry_dump(unit, L2Xm, entry, BSL_INFO|BSL_LS_SOC_SOCMEM);
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "\n")));
    }

    memcpy(&valid_entry, entry, sizeof(l2x_entry_t));
    soc_L2Xm_field32_set(unit, &valid_entry, VALID_BITf, 1);

#define SOC_L2X_MEM_BYTES 11
    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    soc_schan_header_cmd_set(unit, &schan_msg.header, ARL_INSERT_CMD_MSG,
                             0, src_blk, 0, SOC_L2X_MEM_BYETS, 0, 0);  
#undef SOC_L2X_MEM_BYTES

    /* Fill in ARL packet data */

    sal_memcpy(schan_msg.arlins.data, &valid_entry, 
               sizeof (schan_msg.arlins.data));

    /* Lock ARL memory to avoid modifying L2 table while frozen */

    soc_mem_lock(unit, L2Xm);

    /* Execute S-Channel operation (header word + 3 DWORDs) */

    rv = soc_schan_op(unit, &schan_msg, 4, 0, 1);
    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                "soc_schan_op: operation failed: %s\n"), soc_errmsg(rv)));
    }

    if (rv == SOC_E_FAIL) {
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "Insert table[L2X]: hash bucket full\n")));
        rv = SOC_E_FULL;
    }

    soc_mem_unlock(unit, L2Xm);

    /* 5690 workaround; see _soc_l2x_hit_clear() for details */

    if (rv >= 0 &&
        soc_feature(unit, soc_feature_l2x_ins_sets_hit) &&
        !soc_L2Xm_field32_get(unit, entry, HIT_BITf)) {
        rv = _soc_l2x_hit_clear(unit, entry);
    }

    return rv;
}

/*
 * Function:
 *	soc_l2x_delete
 * Purpose:
 *	Delete an entry from the L2X hash table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	entry - L2X entry to delete
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_FULL - hash bucket full
 * Notes:
 *	Uses hardware deletion; sends an L2 DELETE message over the
 *	S-Channel.  The hardware needs the L2Xm entry type.
 *	Only the L2_VALIDm table is affected.
 */

int
soc_l2x_delete(int unit, l2x_entry_t *entry)
{
    schan_msg_t 	schan_msg;
    int			rv;
    int                 src_blk;

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "Delete table[L2X]: ")));
        soc_mem_entry_dump(unit, L2Xm, entry, BSL_INFO|BSL_LS_SOC_SOCMEM);
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "\n")));
    }

#define SOC_L2X_MEM_KEY_BYTES 8
    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    soc_schan_header_cmd_set(unit, &schan_msg.header, ARL_DELETE_CMD_MSG,
                             0, src_blk, 0, SOC_L2X_MEM_KEY_BYTES, 0, 0);  
#undef SOC_L2X_MEM_KEY_BYTES

    /* Fill in packet data */

    sal_memcpy(schan_msg.arldel.data, entry, sizeof (schan_msg.arldel.data));

    /* Lock ARL memory to avoid modifying L2 table while frozen */

    soc_mem_lock(unit, L2Xm);

    /* Execute S-Channel operation (header word + 2 DWORDs) */
    rv = soc_schan_op(unit, &schan_msg, 3, 0, 1);
    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                "soc_schan_op: operation failed: %s\n"), soc_errmsg(rv)));
    }

    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *	soc_l2x_delete_all
 * Purpose:
 *	Clear the L2 table by invalidating entries.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	static_too - if TRUE, delete static and non-static entries;
 *		     if FALSE, delete only non-static entries
 * Returns:
 *	SOC_E_XXX
 */

int
soc_l2x_delete_all(int unit, int static_too)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			index_min, index_max, index;
    l2x_valid_entry_t	valid_bits;
    l2x_static_entry_t	static_bits;
    int			rv = SOC_E_NONE;

    soc_mem_lock(unit, L2Xm);

    sal_memset(&valid_bits, 0, sizeof (valid_bits));

    index_min = soc_mem_index_min(unit, L2X_VALIDm);
    index_max = soc_mem_index_max(unit, L2X_VALIDm);

    for (index = index_min; index <= index_max; index++) {
        if (!static_too) {
            if ((rv = soc_mem_read(unit, L2X_VALIDm, MEM_BLOCK_ANY,
                                   index, &valid_bits)) < 0) {
                goto done;
            }

            if ((rv = soc_mem_read(unit, L2X_STATICm, MEM_BLOCK_ANY,
                                   index, &static_bits)) < 0) {
                goto done;
            }

            valid_bits.entry_data[0] &= static_bits.entry_data[0];
        } /* else the valid bits will always be 0 */

        if ((rv = soc_mem_write(unit, L2X_VALIDm, MEM_BLOCK_ALL,
                                index, &valid_bits)) < 0) {
            goto done;
        }
    }

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }

 done:

    soc_mem_unlock(unit, L2Xm);

    return rv;
}
#endif /* BCM_XGS12_SWITCH_SUPPORT */
#ifdef PLISIM
#ifdef _KATANA_DEBUG /* BCM_KATANA_SUPPORT */
void _bcm_kt_enable_port_age_simulation(uint32 flags, _bcm_l2_replace_t *rep_st)
{
	port_age_enable=TRUE;
	port_age_flags=flags;
	port_age_rep_st=rep_st;
	return ;
}
STATIC int perform_port_age_simulation(int unit)
{
    	uint32 		sync_op;
        soc_field_t     valid_bit = VALID_BITf;
	int             static_bit_val;
	int             pending_bit_val;
    	int             port_val, mod_val;
	vlan_id_t       vlan_val;
	int 		vfi_val;
	int             tgid_hi=0,tgid_lo=0;
	int 		tgid_val=0, tid_val=0;
	int		trunk128;
	uint32		tid;


    	/* Indexes to iterate over memories, chunks and entries */
	int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
	int             buf_size, chunksize, chnk_end;
	/* Buffer to store chunk of L2 table we currently work on */
	uint32          *l2_tbl_chnk;
	uint32          *l2_entry;
	int             rv = SOC_E_NONE;

	if ( port_age_enable==FALSE) {
        LOG_CLI((BSL_META_U(unit,
                            "INFO:PortAging simulation not enabled yet \n")));	
		return SOC_E_NONE;
	}
	port_age_enable=FALSE;
    LOG_CLI((BSL_META_U(unit,
                        "Performing PortAging simulation... STATIC %d \n"),
             port_age_flags & BCM_L2_REPLACE_MATCH_STATIC));

	if (SOC_IS_FBX(unit)) {
        	valid_bit = VALIDf;
    	}
	tid = port_age_rep_st->match_trunk;
	if (!(port_age_flags & BCM_L2_REPLACE_DELETE) || (port_age_flags & _BCM_L2_REPLACE_DELETE_ALL)) {
       		return SOC_E_NONE;
	}
	switch (port_age_flags & (BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN)) {
		case BCM_L2_REPLACE_MATCH_DEST:
			if (port_age_rep_st->isTrunk) {
				sync_op = SOC_L2X_TRUNK_DEL;
			} else {
				sync_op = SOC_L2X_PORTMOD_DEL;
			}
			break;
		case BCM_L2_REPLACE_MATCH_VLAN:
			sync_op = SOC_L2X_VLAN_DEL;
			break;
		case BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_VLAN:
			if (port_age_rep_st->isTrunk) {
				sync_op = SOC_L2X_TRUNK_VLAN_DEL;
			} else {
				sync_op = SOC_L2X_PORTMOD_VLAN_DEL;
			}
			break;
		default:
            LOG_CLI((BSL_META_U(unit,
                                "Unknown PortAging operation !!! \n")));	
			return SOC_E_PARAM;
	}
    	if (!soc_mem_index_count(unit, L2Xm)) {
        	return SOC_E_NONE;
	}
    	chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS, L2_MEM_CHUNKS_DEFAULT);
	buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
	l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 traverse");
	if (NULL == l2_tbl_chnk) {
		return SOC_E_MEMORY;
	}
	mem_idx_max = soc_mem_index_max(unit, L2Xm);
	for (chnk_idx = soc_mem_index_min(unit, L2Xm); chnk_idx <= mem_idx_max; chnk_idx += chunksize) {
		sal_memset((void *)l2_tbl_chnk, 0, buf_size);
		chnk_idx_max = ((chnk_idx + chunksize) < mem_idx_max) ?  (chnk_idx + chunksize - 1) : mem_idx_max;
		rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY, chnk_idx, chnk_idx_max, l2_tbl_chnk);
		if (SOC_FAILURE(rv)) {
			break;
		}
		chnk_end = (chnk_idx_max - chnk_idx);
		for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
			l2_entry = soc_mem_table_idx_to_pointer(unit, L2Xm, uint32 *, l2_tbl_chnk, ent_idx);
			if (!soc_mem_field32_get(unit,L2Xm, l2_entry,valid_bit)) {
				continue;
            		}
		        trunk128 = soc_feature(unit, soc_feature_trunk_extended);
			if (!trunk128)
			{
				tid &= SOC_L2X_TGID_PORT_TRUNK_MASK;
				tid |= SOC_L2X_TGID_TRUNK_INDICATOR(unit);
			}
            		if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                		port_val = soc_mem_field32_get(unit, L2Xm, l2_entry, PORT_NUMf);
				if (trunk128)
				{
					tid_val =  soc_mem_field32_get(unit, L2Xm, l2_entry, TGIDf);
				}
            		} else {
                		port_val = soc_mem_field32_get(unit, L2Xm, l2_entry, TGID_PORTf);
				if (trunk128)
				{
					tgid_val = soc_mem_field32_get(unit, L2Xm, l2_entry, MODULE_IDf);
			               	tgid_hi = (tid & SOC_L2X_TGID_PORT_TRUNK_MASK_HI) >> SOC_TRUNK_BIT_POS(unit);
					tgid_lo = tid & SOC_L2X_TGID_PORT_TRUNK_MASK;
					tgid_lo |= SOC_L2X_TGID_TRUNK_INDICATOR(unit);
				}
            		}
			mod_val 		= soc_mem_field32_get(unit,L2Xm,l2_entry,MODULE_IDf);
			static_bit_val 	= soc_mem_field32_get(unit,L2Xm,l2_entry,STATIC_BITf);
			pending_bit_val 	= soc_mem_field32_get(unit,L2Xm,l2_entry,PENDINGf);
			vlan_val 	= soc_mem_field32_get(unit,L2Xm,l2_entry,VLAN_IDf);
			vfi_val 		= soc_mem_field32_get(unit, L2Xm,l2_entry, VFIf);
			switch(sync_op)
			{
				case SOC_L2X_PORTMOD_DEL:
                			if (soc_mem_field32_get(unit, L2Xm, l2_entry, Tf)) {
                    				continue;
                			}
					if ((port_val != port_age_rep_st->match_port) || (mod_val != port_age_rep_st->match_module)) {
						continue;
					}
					break;
				case SOC_L2X_VLAN_DEL:
               				if (soc_mem_field32_get(unit, L2Xm, l2_entry, Tf)) {
               					continue;
               				}
					if (vlan_val != port_age_rep_st->match_vid) {
						continue;
					}
					break;
				case SOC_L2X_VFI_DEL:
                    LOG_CLI((BSL_META_U(unit,
                                        "NOT IMPLEMENTED!\n")));	
					continue;
				case SOC_L2X_PORTMOD_VLAN_DEL:
               				if (soc_mem_field32_get(unit, L2Xm, l2_entry, Tf)) {
               					continue;
               				}
					if ((vlan_val != port_age_rep_st->match_vid) || (port_val != port_age_rep_st->match_port) || (mod_val != port_age_rep_st->match_module)) {
						continue;
					}
					break;
				case SOC_L2X_TRUNK_DEL:
				case SOC_L2X_TRUNK_VLAN_DEL:
					if(trunk128) {
						LOG_CLI((BSL_META_U(unit,
                                                                    "Trunk128...\n")));
						if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
				                    	if (!soc_mem_field32_get(unit, L2Xm, l2_entry, Tf)) {
								continue;
							}
							if (tid_val != port_age_rep_st->match_trunk) {
								continue;
							}
							}else {
								if ((tgid_lo != port_val) ||(tgid_hi != tgid_val)) {
									continue;
								}
							}
							if (sync_op == SOC_L2X_TRUNK_VLAN_DEL) {
								if(vlan_val != port_age_rep_st->match_vid) {
									continue;
								}
							}
						} else {
							if (port_val != port_age_rep_st->match_trunk) {
								continue;
							}
							if (sync_op == SOC_L2X_TRUNK_VLAN_DEL) {
								if(vlan_val != port_age_rep_st->match_vid) {
									continue;
								}
							}
						}
						break;
				default:
					continue;
			}
			/* Check for static entry deletion */
			if ( (port_age_flags & BCM_L2_REPLACE_MATCH_STATIC) != BCM_L2_REPLACE_MATCH_STATIC) {
				if (static_bit_val == 1) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Skipped STATIC L2 Entry Deletion:"
                                        "port_val:%d mod_id : %d static_bit_val : %d "
                                        "pending_bit_val %d vlan_id : %d vfi_id : %d "
                                        "\n"),
                             port_val, mod_val, static_bit_val,
                             pending_bit_val, vlan_val, vfi_val));
					continue;
				}
			}
			if( (port_age_flags & BCM_L2_REPLACE_PENDING) == BCM_L2_REPLACE_PENDING) {
				if (pending_bit_val != 1) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Skipped Non Pending L2 Entry Deletion:"
                                        "port_val:%d mod_id : %d static_bit_val : %d "
                                        "pending_bit_val %d vlan_id : %d vfi_id : %d "
                                        "\n"),
                             port_val, mod_val, static_bit_val,
                             pending_bit_val, vlan_val, vfi_val));
					continue;
				}
			}
			/* soc_mem_field32_set(unit,L2Xm, l2_entry,valid_bit,0); */
			rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, l2_entry);
       			if (SOC_FAILURE(rv)) {
				break;
       			}
                LOG_CLI((BSL_META_U(unit,
                                    "Deleted L2 Entry:port_val:%d mod_id : %d "
                                    "static_bit_val : %d pending_bit_val : %d "
                                    "vlan_id : %d vfi_id : %d \n"),
                         port_val, mod_val, static_bit_val,
                         pending_bit_val, vlan_val, vfi_val));
        	}
	}
	soc_cm_sfree(unit, l2_tbl_chnk);
	return SOC_E_NONE;
}
#endif
#endif

#ifdef BCM_TRIDENT2_SUPPORT
STATIC int 
_soc_l2x_td2_convert_delete_to_replace(int unit) 
{
    uint32 rval;  
    SOC_IF_ERROR_RETURN(READ_L2_BULK_CONTROLr(unit, &rval));  
    if (soc_reg_field_get(unit, L2_BULK_CONTROLr, rval, 
                            ACTIONf) == 1) {
        l2_bulk_replace_mask_entry_t repl_mask;
        l2_bulk_replace_data_entry_t repl_data;
        sal_memset(&repl_mask, 0xFF, sizeof(repl_mask));
        sal_memset(&repl_data, 0, sizeof(repl_data));
        SOC_IF_ERROR_RETURN
            (WRITE_L2_BULK_REPLACE_MASKm(unit, MEM_BLOCK_ALL, 0, &repl_mask));
        SOC_IF_ERROR_RETURN
            (WRITE_L2_BULK_REPLACE_DATAm(unit, MEM_BLOCK_ALL, 0, &repl_data));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY, 
                                    ACTIONf, 2));  
    }  
    return SOC_E_NONE;
}

STATIC int 
_soc_l2x_td2_frozen_cml_restore(int unit) 
{
    uint32       rval;
    cml_freeze_t *f_cml = &cml_freeze_state[unit]; /* Cml freeze state.*/


    SOC_CML_FREEZE_LOCK(unit);
    /* If going out of freeze restore values to HW. */
    if (f_cml->frozen == 1) {
        SOC_IF_ERROR_RETURN(
            READ_ING_MISC_CONFIG2r(unit, &rval));
        soc_reg_field_set(unit, ING_MISC_CONFIG2r, &rval, 
                                CML_OVERRIDE_ENABLE_NEWf, 0);
        soc_reg_field_set(unit, ING_MISC_CONFIG2r, &rval, 
                                CML_OVERRIDE_ENABLE_MOVEf, 0);
        SOC_IF_ERROR_RETURN(
            WRITE_ING_MISC_CONFIG2r(unit, rval));
    } 

    /* Regardless of status, decrement frozen count. */
    f_cml->frozen--;
    SOC_CML_FREEZE_UNLOCK(unit);
    return SOC_E_NONE;
}

STATIC int 
_soc_l2x_td2_frozen_cml_save(int unit) 
{
    uint32       rval;
    cml_freeze_t *f_cml = &cml_freeze_state[unit]; /* Cml freeze state.*/

    SOC_CML_FREEZE_LOCK(unit);
    /* Freezing l2 - disable learning. */
    if (f_cml->frozen == 0) {
        SOC_IF_ERROR_RETURN(
            READ_ING_MISC_CONFIG2r(unit, &rval));
        soc_reg_field_set(unit, ING_MISC_CONFIG2r, &rval, 
                                CML_OVERRIDE_ENABLE_NEWf, 1);
        soc_reg_field_set(unit, ING_MISC_CONFIG2r, &rval, 
                                CML_OVERRIDE_ENABLE_MOVEf, 1);
        SOC_IF_ERROR_RETURN(
            WRITE_ING_MISC_CONFIG2r(unit, rval));
    }

    /* Icrement CML frozen indicator. */
    f_cml->frozen++;
    SOC_CML_FREEZE_UNLOCK(unit);
    return SOC_E_NONE;
}

int 
soc_l2x_cml_override_is_enabled(int unit) 
{
    uint32       rval = 0;
    uint32       enable_newf = 0, enable_movef = 0;
    int          rv = SOC_E_NONE;

    rv = READ_ING_MISC_CONFIG2r(unit, &rval);
    if (rv != SOC_E_NONE) {
        return 0;
    }
    enable_newf = soc_reg_field_get(unit, ING_MISC_CONFIG2r,
                                        rval, CML_OVERRIDE_ENABLE_NEWf);

    enable_movef = soc_reg_field_get(unit, ING_MISC_CONFIG2r,
                                         rval, CML_OVERRIDE_ENABLE_MOVEf);

    rval = (enable_newf == 0 && enable_movef == 0)? 0:1;
    return rval;
}

#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      soc_l2x_wb_scache_size_get
 * Purpose:
 *      Returns required scache size for L2 part
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      req_scache_size      - (OUT) Request scache size
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int
soc_l2x_wb_scache_size_get(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    alloc_size += sizeof(l2_freeze_mode);
    *req_scache_size += alloc_size;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_l2x_scache_sync
 * Purpose:
 *      Syncs L2 part warmboot state to scache
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      scache_ptr           - (IN)  Scache pointer.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int
soc_l2x_scache_sync(int unit, uint8 **scache_ptr)
{
    int *iptr = NULL;

    iptr = (int *)(*scache_ptr);
    *iptr = l2_freeze_mode[unit];
    iptr++;
    *scache_ptr = (uint8 *)iptr;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_l2x_reinit
 * Purpose:
 *      Recovers L2 part warmboot state from scache
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      scache_ptr           - (IN)  Scache pointer.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int
soc_l2x_reinit(int unit, uint8 **scache_ptr)
{
    int *iptr = NULL;

    iptr = (int *)(*scache_ptr);
    l2_freeze_mode[unit] = *iptr;
    iptr++;
    *scache_ptr = (uint8 *)iptr;

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      soc_l2x_freeze_mode_is_global_override
 * Description:
 *      Check if SDK is using global override to freeze L2X.
 * Parameters:
 *      unit      - (IN) Device number
 * Return Value:
 *      TRUE / FALSE
 */
int
soc_l2x_freeze_mode_is_global_override(int unit)
{
    return (l2_freeze_mode[unit] == SOC_L2X_FREEZE_MODE_GLOBAL_OVERRIDE)? \
            TRUE:FALSE;
}

/*
 * Function:
 *      soc_l2x_freeze_mode_set
 * Description:
 *      Record configured freeze mode.
 *      Mode cannot be changed during frozen. Because freeze/thaw should
 *      be executed with the same mode.
 * Parameters:
 *      unit      - (IN) Device number
 *      mode      - (IN) Mode to use
 * Return Value:
 *      SOC_E_XXX
 */
int
soc_l2x_freeze_mode_set(int unit, int mode)
{
    int frozen = 0;
    int cur_mode = 0;

    SOC_IF_ERROR_RETURN(soc_l2x_freeze_mode_get(unit, &cur_mode));
    if (mode == cur_mode) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_l2x_is_frozen(unit, SOC_L2X_FROZEN, &frozen));
    if (frozen) {
        return SOC_E_BUSY;
    } else {
        l2_freeze_mode[unit] = mode;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_l2x_freeze_mode_get
 * Description:
 *      Retrieve configured freeze mode..
 * Parameters:
 *      unit      - (IN)  Device number
 *      mode      - (OUT) Mode in use
 * Return Value:
 *      SOC_E_XXX
 */
int
soc_l2x_freeze_mode_get(int unit, int *mode)
{
    if (mode == NULL) {
        return SOC_E_PARAM;
    }
    *mode = l2_freeze_mode[unit];

    return SOC_E_NONE;
}

/*
 * Function:
 *	_soc_l2x_port_age
 * Purpose:
 *	Use HW port/VLAN "aging" to delete selected L2 entries.
 * Parameters:
 *	unit	     - StrataSwitch unit #
 *      age_reg0     - port aging register.
 *	age_reg1     - optioanl 2nd port aging register.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	Hardware deletion is used.
 *	The chip requires that ARL aging be disabled during this
 *	operation.  An unavoidable side effect is that the hardware
 *	aging timer gets restarted whenever this routine is called.
 */
int
soc_l2x_port_age(int unit, soc_reg_t reg0, soc_reg_t reg1)
{
    static soc_field_t  fields[2] = { STARTf, COMPLETEf };
    static uint32       values[2] = { 1, 0 };
    soc_timeout_t	to;
    int			rv;
    int			timeout_usec;
    int                 reg0_complete, reg1_complete;
    uint32              rval;
    int                 mode;
#ifdef BCM_TRIDENT2_SUPPORT
    soc_control_t *soc = SOC_CONTROL(unit);
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    uint64              rval64;
#endif
    int                 min_polls = 0;

    if (reg0 == INVALIDr && reg1 == INVALIDr) {
        return SOC_E_NONE;
    }

    timeout_usec = SOC_CONTROL(unit)->l2x_age_timeout;
    mode = SOC_CONTROL(unit)->l2x_mode;

    if (mode != L2MODE_FIFO) {
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_save(unit));
        } else {
            SOC_IF_ERROR_RETURN(soc_l2x_freeze(unit));
        }
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)) {
        /* if action == 1, convert action to 2 and replace data to be 0 */
        /* TD3TBD, does TD3 need this? */
        rv = _soc_l2x_td2_convert_delete_to_replace(unit);
        if (SOC_FAILURE(rv)) {
            goto exit;
        }
    }
#endif

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_l2_hw_aging_bug) &&
        sal_thread_self() == soc->l2x_age_pid) {
        TABLE_DMA_LOCK(unit);
        SOC_SBUSDMA_DM_LOCK(unit);
        SCHAN_LOCK(unit);
    }
#endif

    reg0_complete = TRUE;
    if (reg0 != INVALIDr) {
        rv = soc_reg_fields32_modify(unit, reg0, REG_PORT_ANY, 2, fields,
                                     values);
        if (SOC_FAILURE(rv)) {
            goto done;
        }
        reg0_complete = FALSE;
    }

    reg1_complete = TRUE;
    if (reg1 != INVALIDr) {
        rv = soc_reg_fields32_modify(unit, reg1, REG_PORT_ANY, 2, fields,
                                     values);
        if (SOC_FAILURE(rv)) {
            goto done;
        }
        reg1_complete = FALSE;
    }
    if (soc_feature(unit, soc_feature_l2x_bulk_improvement)) {
       min_polls = soc_property_get(unit, spn_L2X_BULK_DELETE_MIN_POLL_NUM, 0);
    }
    SOC_CONTROL(unit)->l2x_ppa_in_progress = TRUE;
    soc_timeout_init(&to, timeout_usec, min_polls);
    for (;;) {
        if (!reg0_complete) {
#ifdef BCM_HURRICANE3_SUPPORT
            if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) && 
                reg0 == PER_PORT_AGE_CONTROL_64r) {
                rv = soc_reg64_get(unit, reg0, REG_PORT_ANY, 0, &rval64);
                if (SOC_SUCCESS(rv)) {
                    reg0_complete = soc_reg64_field32_get(unit, reg0, rval64, COMPLETEf);
                }
            } else 
#endif
            {
                rv = soc_reg32_get(unit, reg0, REG_PORT_ANY, 0, &rval);
                if (SOC_SUCCESS(rv)) {
                    reg0_complete = soc_reg_field_get(unit, reg0, rval, COMPLETEf);
                }
            }
        }
        if (!reg1_complete) {
            rv = soc_reg32_get(unit, reg1, REG_PORT_ANY, 0, &rval);
            if (SOC_SUCCESS(rv)) {
                reg1_complete = soc_reg_field_get(unit, reg1, rval, COMPLETEf);
            }
        }

        if (reg0_complete && reg1_complete) {
            rv = SOC_E_NONE;
            break;
        }

        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            break;
        }
    }

 done:
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_l2_hw_aging_bug) &&
        sal_thread_self() == soc->l2x_age_pid) {
        SCHAN_UNLOCK(unit);
        SOC_SBUSDMA_DM_UNLOCK(unit);
        TABLE_DMA_UNLOCK(unit);
    }

 exit:
#endif
    SOC_CONTROL(unit)->l2x_ppa_in_progress = FALSE;
    if (mode != L2MODE_FIFO) {
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_restore(unit));
        } else {
            SOC_IF_ERROR_RETURN(soc_l2x_thaw(unit));
        }
    }
#ifdef PLISIM
#ifdef _KATANA_DEBUG /* BCM_KATANA_SUPPORT */
    if (SAL_BOOT_PLISIM) {
        if ((rv == SOC_E_NONE) && (SOC_IS_KATANA(unit))) {
            perform_port_age_simulation(unit);
        }
    }
#endif
#endif
    return rv;
}

#ifdef BCM_TRIDENT3_SUPPORT
int
soc_l2x_cml_register(int unit, soc_l2x_cml_cb_fn fn)
{
    cml_freeze_t  *f_cml = &cml_freeze_state[unit];
    f_cml->cml_func = fn;
    return SOC_E_NONE;
}
#endif/*BCM_TRIDENT3_SUPPORT*/
/*
 * Function:
 *  	_soc_l2x_frozen_cml_restore
 * Purpose:
 *  	Helper function to restore port learning mode.
 * Parameters:
 *   	unit - (IN) BCM device number.
 * Returns:
 *	    SOC_E_XXX
 */
int 
_soc_l2x_frozen_cml_restore(int unit)
{
    cml_freeze_t		*f_cml = &cml_freeze_state[unit]; /* Cml freeze state.*/
    soc_port_t		    port;                /* Device port iterator.         */ 
    port_tab_entry_t    pent;                /* Port table hw entry buffer.   */
    int                 cml;                 /* Learn mode for specific port. */ 
    int                 rv = SOC_E_NONE;     /* Operation return status.      */
    soc_pbmp_t          pbmp;

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit) &&
        soc_l2x_freeze_mode_is_global_override(unit)) {
        return _soc_l2x_td2_frozen_cml_restore(unit);
    }
#endif

    SOC_PBMP_CLEAR(pbmp);
    /* Take protection mutex. */ 
    if (SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        soc_mem_lock(unit, PORT_TABm);
    }
    if (SOC_MEM_IS_VALID(unit, SOURCE_VPm)) {
        soc_mem_lock(unit, SOURCE_VPm);
    }

    /* If going out of freeze restore sw preserved values to HW. */
    if (f_cml->frozen == 1) {
        SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &pbmp);
        }
#endif
        PBMP_ITER(pbmp, port) {
            /* Read port table entry. */
            if (!SOC_IS_TRIDENT3X(unit)) {
                rv = READ_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ANY, port, &pent);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
            /* Get port current learning mode. */
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               /*mem write is not allowed during WB*/
               if (SOC_WARM_BOOT(unit)) {
                   break;
               }
               if (f_cml->cml_func == NULL) {
                   rv = SOC_E_FAIL;
                   break;
               }
               rv = f_cml->cml_func(unit, port, FALSE,
                                    &f_cml->save_cml[port],
                                    &f_cml->save_cml_move[port]);
               if (SOC_FAILURE(rv)) {
                    break;
               }
            } else
#endif /*BCM_TRIDENT3_SUPPORT*/
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                int cml_change = FALSE;
                cml = soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_NEWf);
                if (cml != f_cml->save_cml[port]) {
                    soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_NEWf,
                                              f_cml->save_cml[port]);
                    cml_change = TRUE;
                }
                cml = soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_MOVEf);
                if (cml != f_cml->save_cml_move[port]) {
                    soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_MOVEf,
                                              f_cml->save_cml_move[port]);
                    cml_change = TRUE;
                }
                if (cml_change) {
                    rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port,
                                                 &pent);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                }
            } else
#endif /* BCM_TRX_SUPPORT */
            {
                cml = soc_PORT_TABm_field32_get(unit, &pent, CMLf);
                /* Update mode if necessary. */
                if (cml != f_cml->save_cml[port]) {
                    soc_PORT_TABm_field32_set(unit, &pent, CMLf, 
                                              f_cml->save_cml[port]);
                    rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port, 
                                                &pent);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                }
            }
        } 
        /* Restore CML value and on Virtual ports */
        if (SOC_SUCCESS(rv) && SOC_MEM_IS_VALID(unit, SOURCE_VPm) &&
            (NULL != f_cml->vp_bitmap)) {
            int  index, index_min, index_max, buf_index, count;
            void *svp, *buf;

            index_min = soc_mem_index_min(unit, SOURCE_VPm);
            index_max = soc_mem_index_max(unit, SOURCE_VPm);
            index_min += 1; /* Always skip entry 0 of SVP table */

            if (!shr_bitop_range_null(f_cml->vp_bitmap, index_min,
                                      (index_max - index_min + 1))) {
                buf = soc_cm_salloc(unit,
                                    SOC_MEM_TABLE_BYTES(unit, SOURCE_VPm),
                                    "source_vp for cml restore");
                if (buf == NULL) {
                    rv = SOC_E_MEMORY;
                }

                if (SOC_SUCCESS(rv)) {
                rv = soc_mem_read_range(unit, SOURCE_VPm, MEM_BLOCK_ANY,
                                        index_min, index_max, buf);
                }
                if (SOC_SUCCESS(rv)) {
                    count = 0;
                    for (index = index_min; index < index_max; index++) {
                        if (!SHR_BITGET(f_cml->vp_bitmap, index)) {
                            /* VP not in use */
                            continue;
                        }
                        buf_index = index - index_min;
                        svp = soc_mem_table_idx_to_pointer
                            (unit, SOURCE_VPm, void *, buf, buf_index);
                        if (soc_mem_field32_get(unit, SOURCE_VPm, svp,
                                                ENTRY_TYPEf) == 0) {
                            continue;
                        }
                        soc_mem_field32_set
                            (unit, SOURCE_VPm, svp, CML_FLAGS_MOVEf,
                             f_cml->save_vp_cml_move[buf_index]);
                        soc_mem_field32_set
                            (unit, SOURCE_VPm, svp, CML_FLAGS_NEWf,
                             f_cml->save_vp_cml[buf_index]);
                        count++;
                    }
                    if (count > 0) {
                        rv = soc_mem_write_range(unit, SOURCE_VPm,
                                                 MEM_BLOCK_ANY, index_min,
                                                 index_max, buf);
                    }
                }
                if (buf != NULL) {
                    soc_cm_sfree(unit, buf);
                } /* Else not using any VPs, don't process the SVP table. */
            }
        }
    } 

    /* Regardless of status, decrement frozen count. */
    f_cml->frozen--;

    /* Release protection mutex. */ 
    if (SOC_MEM_IS_VALID(unit, SOURCE_VPm)) {
        soc_mem_unlock(unit, SOURCE_VPm);
    }
    if (SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        soc_mem_unlock(unit, PORT_TABm);
    }
    return (rv);
}


/*
 * Function:
 *  	_soc_l2x_frozen_cml_save
 * Purpose:
 *  	Helper function to preserve port learning mode in sw structure.
 * Parameters:
 *   	unit - (IN) BCM device number.
 * Returns:
 *	    SOC_E_XXX
 */
int
_soc_l2x_frozen_cml_save(int unit)
{
    cml_freeze_t		*f_cml = &cml_freeze_state[unit]; /* Cml freeze state.*/
    soc_port_t		    port;                /* Device port iterator.         */
    port_tab_entry_t    pent;                /* Port table hw entry buffer.   */
    int                 rv = SOC_E_NONE;     /* Operation return status.      */
    soc_pbmp_t          pbmp;

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit) &&
        soc_l2x_freeze_mode_is_global_override(unit)) {
        return _soc_l2x_td2_frozen_cml_save(unit);
    }
#endif

    SOC_PBMP_CLEAR(pbmp);

    /* Take protection mutex. */ 
    if (SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        soc_mem_lock(unit, PORT_TABm);
    }
    if (SOC_MEM_IS_VALID(unit, SOURCE_VPm)) {
        soc_mem_lock(unit, SOURCE_VPm);
    }

    /* Freezing l2 save l2 learn mode and disable learning. */
    if (f_cml->frozen == 0) {
        SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &pbmp);
        }
#endif
        PBMP_ITER(pbmp, port) {
            /* Read port table entry. */
            if (!SOC_IS_TRIDENT3X(unit)) {
                rv = READ_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ANY, port, &pent);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
#ifdef BCM_TRIDENT3_SUPPORT
           if (SOC_IS_TRIDENT3X(unit)) {
               /*mem write is not allowed during WB*/
               if (SOC_WARM_BOOT(unit)) {
                   break;
               }
               if (f_cml->cml_func == NULL) {
                   rv = SOC_E_FAIL;
                   break;
               }
               rv = f_cml->cml_func(unit, port, TRUE,
                                    &f_cml->save_cml[port],
                                    &f_cml->save_cml_move[port]);
               if (SOC_FAILURE(rv)) {
                    break;
               }
            } else
#endif /*BCM_TRIDENT3_SUPPORT*/
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                f_cml->save_cml[port] =
                    soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_NEWf);
                f_cml->save_cml_move[port] =
                    soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_MOVEf);

                /* Set bits to the equivilant of DROP and COPY TO CPU */
                soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_NEWf,
                                          f_cml->save_cml[port] & 0x03);
                soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_MOVEf,
                                          f_cml->save_cml_move[port] & 0x03);
                rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port, &pent);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            } else
#endif /* BCM_TRX_SUPPORT */
            {
                /* Save current learning mode in sw structure. */
                f_cml->save_cml[port] = soc_PORT_TABm_field32_get(unit, &pent, CMLf);

                /* Disable L2 learning on the port. */
                if ((f_cml->save_cml[port] == PVP_CML_SWITCH) ||
                    (f_cml->save_cml[port] == PVP_CML_CPU_SWITCH)) {
                    soc_PORT_TABm_field32_set(unit, &pent, CMLf, PVP_CML_FORWARD);
                    rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port, &pent);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                } 
            }
        }

        /* Saving CML value and disable learning on Virtual ports */
        if (SOC_SUCCESS(rv) && SOC_MEM_IS_VALID(unit, SOURCE_VPm) &&
            (NULL != f_cml->vp_bitmap)) {
            int  index, index_min, index_max, buf_index, count;
            void *svp, *buf;

            index_min = soc_mem_index_min(unit, SOURCE_VPm);
            index_max = soc_mem_index_max(unit, SOURCE_VPm);
            index_min += 1; /* Always skip entry 0 of SVP table */

            if (!shr_bitop_range_null(f_cml->vp_bitmap, index_min,
                                      (index_max - index_min + 1))) {
                buf = soc_cm_salloc(unit,
                                    SOC_MEM_TABLE_BYTES(unit, SOURCE_VPm),
                                    "source_vp for cml restore");
                if (buf == NULL) {
                    rv = SOC_E_MEMORY;
                }

                if (SOC_SUCCESS(rv)) {
                    rv = soc_mem_read_range(unit, SOURCE_VPm, MEM_BLOCK_ANY,
                                            index_min, index_max, buf);
                }
                if (SOC_SUCCESS(rv)) {
                    count = 0;
                    for (index = index_min; index < index_max; index++) {
                        if (!SHR_BITGET(f_cml->vp_bitmap, index)) {
                            /* VP not in use */
                            continue;
                        }
                        buf_index = index - index_min;
                        svp = soc_mem_table_idx_to_pointer
                            (unit, SOURCE_VPm, void *, buf, buf_index);
                        if (soc_mem_field32_get(unit, SOURCE_VPm, svp,
                                                ENTRY_TYPEf) == 0) {
                            continue;
                        }
                        f_cml->save_vp_cml_move[buf_index] =
                            soc_mem_field32_get(unit, SOURCE_VPm, svp,
                                                CML_FLAGS_MOVEf);
                        f_cml->save_vp_cml[buf_index] =
                            soc_mem_field32_get(unit, SOURCE_VPm, svp,
                                                CML_FLAGS_NEWf);
                        /* Set bits to the equivilant of DROP and COPY TO CPU */
                        soc_mem_field32_set(unit, SOURCE_VPm, svp,
                                            CML_FLAGS_MOVEf,
                                            f_cml->save_vp_cml_move[buf_index] &
                                            0x03);
                        soc_mem_field32_set(unit, SOURCE_VPm, svp,
                                            CML_FLAGS_NEWf,
                                            f_cml->save_vp_cml[buf_index] &
                                            0x03);
                        count++;
                    }
                    if (count > 0) {
                        rv = soc_mem_write_range(unit, SOURCE_VPm,
                                                 MEM_BLOCK_ANY, index_min,
                                                 index_max, buf);
                    }
                }
                if (buf != NULL) {
                    soc_cm_sfree(unit, buf);
                }
            } /* Else not using any VPs, don't process the SVP table. */
        }
    } 

    /* Increase CML frozen indicator. */
    if (SOC_SUCCESS(rv)) {
        f_cml->frozen++;
    }
    if (SOC_MEM_IS_VALID(unit, SOURCE_VPm)) {
        soc_mem_unlock(unit, SOURCE_VPm); /* UNLOCK Virtual port */
    }
    if (SOC_MEM_IS_VALID(unit, PORT_TABm)) {
        soc_mem_unlock(unit, PORT_TABm);	/* PORT_UNLOCK */
    }
    return(rv);  
}

/*
 * Function:
 *	    soc_l2x_is_frozen
 * Purpose:
 *   	Provides indication if L3 table is in frozen state
 * Parameters:
 *	   unit - (IN) BCM device number.
 *     frozen - (OUT) TRUE if L2x is frozen, FLASE otherwise
 * Returns:
 *	   SOC_E_NONE
 * Notes:
 *	Does not change the frozen status of L2x table only reads it.
 */

int
soc_l2x_is_frozen(int unit, int frozen_type, int *frozen)
{
    l2_freeze_t		    *f_l2 = &l2_freeze_state[unit];

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_l2_is_frozen(unit, frozen);
    }
#endif
    if (frozen_type == SOC_L2X_FROZEN_WITH_LOCK) {
        *frozen = (f_l2->frozen > 0) ? TRUE : FALSE ;
    } else if (frozen_type == SOC_L2X_FROZEN_WITHOUT_LOCK) {
        *frozen = (f_l2->hw_frozen > 0) ? TRUE : FALSE ;
    } else {
        *frozen = (f_l2->frozen > 0 || f_l2->hw_frozen > 0) ? TRUE : FALSE ;
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *	    soc_age_timer_cache_set
 * Purpose:
 *   	set value of l2_freeze_state[unit].save_age_ena
 *    and l2_freeze_state[unit].save_age_sec
 * Parameters:
 *	   unit - (IN) BCM device number.
 * Returns:
 *	   NONE
 */
void
soc_age_timer_cache_set(int unit, int age_seconds, int enabled)
{
    l2_freeze_t		    *f_l2 = &l2_freeze_state[unit];

    f_l2->save_age_sec = age_seconds;
    f_l2->save_age_ena = enabled;
    f_l2->new_age_set = 1;
    return;
}

/*
 * Function:
 *	    soc_age_timer_cache_get
 * Purpose:
 *   	get value of l2_freeze_state[unit].save_age_ena
 *    and l2_freeze_state[unit].save_age_sec
 * Parameters:
 *	   unit - (IN) BCM device number.
 * Returns:
 *	   NONE
 */
void
soc_age_timer_cache_get(int unit, int *age_seconds, int *enabled)
{
    l2_freeze_t		    *f_l2 = &l2_freeze_state[unit];

    *age_seconds = f_l2->save_age_sec;
    *enabled = f_l2->save_age_ena;
    return;
}

/*
 * Function:
 *	    soc_l2x_selective_freeze
 * Purpose:
 *   	Temporarily quiesce L2 table from H/W activity (learning, aging)
 *    Does not quiesce S/W activity
 * Parameters:
 *	   unit - (IN) BCM device number. 
 * freeze_mode -(IN) 1: Only H/W is frozen
 * Returns:
 *	   SOC_E_XXX
 */

/*soc_l2x_selective_freeze matches with soc_l2x_selective_thaw */
int
soc_l2x_selective_freeze(int unit, int freeze_mode)
{
    l2_freeze_t		    *f_l2 = &l2_freeze_state[unit];
    int                 rv = SOC_E_NONE;
    int                 age_sec = 0;
    int                 age_ena = 0;

    /* Check if already frozen. */
    SOC_L2X_MEM_LOCK(unit);
    if (f_l2->frozen > 0 || f_l2->hw_frozen > 0) {
        /* Keep count - do nothing. */
        if (freeze_mode == SOC_L2X_FROZEN_WITH_LOCK) {
            f_l2->frozen++;
        } else {
            f_l2->hw_frozen++;
        }
        SOC_L2X_MEM_UNLOCK(unit);
        return (SOC_E_NONE);
    }

    /*
     * Increase l2 frozen indicator in the beginning to
     * avoid _soc_l2x_frozen_cml_save() being called twice
     * in reentrance unexpectedly.
     */
    if (freeze_mode == SOC_L2X_FROZEN_WITH_LOCK) {
        f_l2->frozen++;
    } else {
        f_l2->hw_frozen++;
    }
    SOC_L2X_MEM_UNLOCK(unit);

    /* Preserve ports learning mode & disable learning. */
    rv = _soc_l2x_frozen_cml_save(unit);
    if (SOC_FAILURE(rv)) {
        /* Decrease l2 frozen indicator. */
        if (freeze_mode == SOC_L2X_FROZEN_WITH_LOCK) {
            f_l2->frozen--;
        } else {
            f_l2->hw_frozen--;
        }
        return rv;
    }

    /*
     * Lock l2x, disable learning and aging.
     */
    SOC_L2X_MEM_LOCK(unit);

    /* Read l2 aging interval. */
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit,
                                            &age_sec,
                                            &age_ena);
    if (SOC_FAILURE(rv)) {
        /* Decrease l2 frozen indicator. */
        if (freeze_mode == SOC_L2X_FROZEN_WITH_LOCK) {
            f_l2->frozen--;
        } else {
            f_l2->hw_frozen--;
        }
        SOC_L2X_MEM_UNLOCK(unit);
        _soc_l2x_frozen_cml_restore(unit);
        return rv;
    }
    /* If use bcm_esw_l2_age_timer_set to set new age seconde, we use new age_sec. */
    if (!f_l2->new_age_set) {
        f_l2->save_age_sec = age_sec;
        f_l2->save_age_ena = age_ena;
    }
    /* If l2 aging is on - disable it. */
    /* If use bcm_esw_l2_age_timer_set to set new age seconde as 0  - save it. */
    if (f_l2->save_age_ena || f_l2->new_age_set) {
        rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit,
                                                f_l2->save_age_sec, 0);
        if (SOC_FAILURE(rv)) {
            /* Decrease l2 frozen indicator. */
            if (freeze_mode == SOC_L2X_FROZEN_WITH_LOCK) {
                f_l2->frozen--;
            } else {
                f_l2->hw_frozen--;
            }
            SOC_L2X_MEM_UNLOCK(unit);
            _soc_l2x_frozen_cml_restore(unit);
            return rv;
        }
        f_l2->new_age_set = 0;
    }

    SOC_L2X_MEM_UNLOCK(unit);
    return (SOC_E_NONE);
}
/*
 * Function:
 *	    soc_l2x_freeze
 * Purpose:
 *   	Temporarily quiesce L2 table from all activity (learning, aging)
 *    Include H/W and S/W activity
 * Parameters:
 *	   unit - (IN) BCM device number. 
 * Returns:
 *	   SOC_E_XXX
 */
int
soc_l2x_freeze(int unit)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_l2_freeze(unit);
    }
#endif

    SOC_L2X_MEM_LOCK(unit);

    SOC_IF_ERROR_RETURN(soc_l2x_selective_freeze(unit, SOC_L2X_FROZEN_WITH_LOCK));


    return (SOC_E_NONE);
}

/*
 * Function:
 *   	soc_l2x_selective_thaw
 * Purpose:
 *	    Restore normal L2 H/W activity.
 * Parameters:
 *	    unit -   (IN) BCM device number.
 *  thaw_mode - Only H/W is thawed
 * Returns:
 *	    SOC_E_XXX
 */
/*soc_l2x_selective_freeze matches with soc_l2x_selective_thaw */
int
soc_l2x_selective_thaw(int unit, int thaw_mode)
{
    l2_freeze_t		    *f_l2 = &l2_freeze_state[unit];
    int                 l2rv, cmlrv;

    /* Sanity check to protect from thaw without freeze. */
    SOC_L2X_MEM_LOCK(unit);
    if (thaw_mode == SOC_L2X_FROZEN_WITH_LOCK) {
        if (f_l2->frozen == 0) {
            SOC_L2X_MEM_UNLOCK(unit);
            assert(0);
            return (SOC_E_UNAVAIL);
        }
        /* In case of nested freeze/thaw just decrement reference counter. */
        if (f_l2->frozen > 1 ||
            (f_l2->frozen == 1 && f_l2->hw_frozen > 0)) {
            f_l2->frozen--;
            SOC_L2X_MEM_UNLOCK(unit);
            return (SOC_E_NONE);
        }
    } else {
        if (f_l2->hw_frozen == 0) {
            SOC_L2X_MEM_UNLOCK(unit);
            return (SOC_E_NONE);
        }
        /* In case of nested freeze/thaw just decrement reference counter. */
        if (f_l2->hw_frozen > 1 ||
            (f_l2->hw_frozen == 1 && f_l2->frozen > 0)) {
            f_l2->hw_frozen--;
            SOC_L2X_MEM_UNLOCK(unit);
            return (SOC_E_NONE);
        }
    }
    /*
     * Last thaw restore L2 learning and aging.
     */
    l2rv = SOC_E_NONE;
    /* If use bcm_esw_l2_age_timer_set to set new age seconde as 0  - save it. */
    if (f_l2->save_age_ena || f_l2->new_age_set) {
        l2rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit,
                                                  f_l2->save_age_sec,
                                                  f_l2->save_age_ena);
        f_l2->new_age_set = 0;
    }

    /* L2 freeze reference counter decrement. */
    if (thaw_mode == SOC_L2X_FROZEN_WITH_LOCK) {
        f_l2->frozen--;
    } else {
        f_l2->hw_frozen--;
    }

    SOC_L2X_MEM_UNLOCK(unit);

    /* Restore port learning mode. */
    cmlrv = _soc_l2x_frozen_cml_restore(unit);

    if (SOC_FAILURE(l2rv)) {
        return l2rv;
    }
    return cmlrv;
}

/*
 * Function:
 *   	soc_l2x_thaw
 * Purpose:
 *	    Restore normal L2 activity.
 * Parameters:
 *	    unit - (IN) BCM device number.
 * Returns:
 *	    SOC_E_XXX
 */

int
soc_l2x_thaw(int unit)
{

    int rv;
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return soc_tr3_l2_thaw(unit);
    }
#endif

    rv = soc_l2x_selective_thaw(unit, SOC_L2X_FROZEN_WITH_LOCK);
    SOC_L2X_MEM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *	soc_l2x_frozen_cml_set
 * Purpose:
 *	Update the saved Cpu Managed Learning mode for a port
 *	if the device is frozen.
 * Parameters:
 *	unit		device number
 *	port		port number
 *	cml		Cpu-Managed-Learning mode to update
 *	cml_move        Station move Cpu-Managed-Learning mode to update
 *	repl_cml	CML mode to use in port table update
 *	repl_cml_move   Station move CML mode to use in port table update
 * Returns:
 *	SOC_E_NONE	frozen cml is updated
 *	SOC_E_FAIL	unit is not frozen
 *
 *  NOTE: 
 *     PORT_TAB must be locked when calling this routine.
 */
int
soc_l2x_frozen_cml_set(int unit, soc_port_t port, int cml, int cml_move, int *repl_cml, int *repl_cml_move)
{
    cml_freeze_t	*f_cml = &cml_freeze_state[unit];

    if (SOC_IS_TD2_TT2(unit) &&
        soc_l2x_freeze_mode_is_global_override(unit)) {
        return SOC_E_UNAVAIL;
    }

    if (f_cml->frozen) {
        f_cml->save_cml[port] = cml;
        f_cml->save_cml_move[port] = cml_move;
        if (repl_cml != NULL) {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRX(unit)) {
                /* Since L2 is frozen, bits 4, 3 should be clear, they would
                   reflect the actual value after thaw */
                *repl_cml = (cml & 0x3);
            } else
#endif
            {
                *repl_cml = PVP_CML_FORWARD;
            }
        }
        if (repl_cml_move != NULL) {
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRX(unit)) {
                /* Since L2 is frozen, bits 4, 3 should be clear, they would
                   reflect the actual value after thaw */
                *repl_cml_move = (cml_move & 0x3);
            } else
#endif
            {
                *repl_cml_move = PVP_CML_FORWARD;
            }
        }

        return SOC_E_NONE;
    }
    return SOC_E_FAIL;
}

/*
 * Function:
 *	soc_l2x_frozen_cml_get
 * Purpose:
 *	Get the saved Cpu Managed Learning mode for a port
 *	if the device is frozen.
 * Parameters:
 *	unit		device number
 *	port		port number
 *	cml		(OUT) saved Cpu-Managed-Learning mode
 *	cml_move        (OUT) saved Station move Cpu-Managed-Learning mode
 * Returns:
 *	SOC_E_NONE	frozen cml is returned
 *	SOC_E_FAIL	unit is not frozen
 *
 *  NOTE: 
 *     PORT_TAB must be locked when calling this routine.
 */
int
soc_l2x_frozen_cml_get(int unit, soc_port_t port, int *cml, int *cml_move)
{
    cml_freeze_t	*f_cml = &cml_freeze_state[unit];

    if (SOC_IS_TD2_TT2(unit) &&
        soc_l2x_freeze_mode_is_global_override(unit)) {
        return SOC_E_UNAVAIL;
    }

    if (f_cml->frozen) {
        if (cml) {
        *cml = f_cml->save_cml[port];
        }
        if (cml_move) {
            *cml_move = f_cml->save_cml_move[port];
        }
        
        return SOC_E_NONE;
    }
    return SOC_E_FAIL;
}


#ifdef BCM_FIREBOLT_SUPPORT
static int
soc_fb_l2x_entries(int unit)
{
    int         index_min, index_max, index, total, rv;
    uint32      *ent;
    uint32      *buf = NULL;
    int         validf = -1;


    index_min = soc_mem_index_min(unit, L2Xm);
    index_max = soc_mem_index_max(unit, L2Xm);
    buf = soc_cm_salloc(unit,
                        SOC_MEM_TABLE_BYTES(unit, L2Xm),
                        "l2x_entries");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                            index_min, index_max, buf);

    if (rv < 0) {
        soc_cm_sfree(unit, buf);
        return rv;
    }

    total = 0;
    ent = buf;

    if (soc_feature(unit, soc_feature_base_valid)) {
        validf = BASE_VALIDf;
    } else {
        validf = VALIDf;
    }

    for (index = 0; index < (index_max - index_min); index++) {
        if (soc_L2Xm_field32_get(unit, ent, validf)) {
            total++;
        }
        ent += soc_mem_entry_words(unit, L2Xm);
    }

    soc_cm_sfree(unit, buf);

    return total;
}
#endif /* BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *	soc_l2x_entries
 * Purpose:
 *	Return number of valid entries in L2 table.
 * Parameters:
 *	unit - StrataSwitch unit #
 * Returns:
 *	If >= 0, number of entries in L2 table
 *	If < 0, SOC_E_XXX
 * Notes:
 *	Somewhat slow; has to read whole table.
 *	New improved: Now with DMA power.
 */

int
soc_l2x_entries(int unit)
{
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return soc_fb_l2x_entries(unit);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    return 0;
}

#ifdef BCM_FIREBOLT_SUPPORT
/*
 * Function:
 *      soc_fb_l2x_bank_lookup
 * Purpose:
 *      Send an L2 lookup message over the S-Channel and receive the
 *      response.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      banks - For dual hashing, which halves are selected (inverted)
 *      key - L2X entry to look up; only MAC+VLAN fields are relevant
 *      result - L2X entry to receive entire found entry
 *      index_ptr (OUT) - If found, receives table index where found
 * Returns:
 *      SOC_E_INTERNAL if retries exceeded or other internal error
 *      SOC_E_NOT_FOUND if the entry is not found.
 *      SOC_E_NONE (0) on success (entry found):
 * Notes:
 *      The S-Channel response contains either a matching L2 entry,
 *      or an entry with a key of all f's if not found.  It is okay
 *      if the result pointer is the same as the key pointer.
 *      Retries the lookup in cases where the particular chip requires it.
 */

int
soc_fb_l2x_bank_lookup(int unit, uint8 banks,
               l2x_entry_t *key, l2x_entry_t *result,
               int *index_ptr)
{
    schan_msg_t    schan_msg;
    int entry_dw = soc_mem_entry_words(unit, L2Xm);
    int nbits;
    int rv;
    int                 dst_blk, src_blk, data_byte_len;
    uint32              bank_ignore_mask;
    int                 opcode, nack;

    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    dst_blk = SOC_BLOCK2SCH(unit, IPIPE_BLOCK(unit));
    bank_ignore_mask = banks & 0x3;
    data_byte_len = entry_dw * 4;
    soc_schan_header_cmd_set(unit, &schan_msg.header, L2_LOOKUP_CMD_MSG,
                             dst_blk, src_blk, 0, data_byte_len, 0,
                             bank_ignore_mask);  

    /* Fill in entry data */

    sal_memcpy(schan_msg.l2x2.data, key, sizeof (schan_msg.l2x2.data));

    /*
     * Write onto S-Channel "L2 lookup" command packet consisting of
     * header word + three words of L2 key, and read back header
     * word + 4 words of lookup result. (index of the extry + contents of
     * the entry)
     */

    rv = soc_schan_op(unit, &schan_msg, entry_dw + 1, entry_dw + 2, 1);

    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                NULL, NULL, &nack);
    if (opcode != L2_LOOKUP_ACK_MSG) {
        LOG_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                "soc_schan_op: operation failed: %s(%d)\n"), soc_errmsg(rv), rv));
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_fb_l2x_bank_lookup: "
                              "invalid S-Channel reply, expected L2_LOOKUP_ACK_MSG:\n")));
        soc_schan_dump(unit, &schan_msg, entry_dw + 2);
        return SOC_E_INTERNAL;
    }

    /*
     * Fill in result entry from read data.
     *
     * Format of S-Channel response:
     *          readresp.header : L2 lookup Ack S-channel header
     *          readresp.data[0]: index of the entry in the L2Xm Table
     *          readresp.data[1-3]: entry contents
     * =============================================================
     * | PERR_PBM | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * =============================================================
     */
    nbits = soc_mem_entry_bits(unit, L2Xm) % 32; /* Data Bits in last word */

    if ((nack != 0) || (rv == SOC_E_FAIL)) {
        *index_ptr = -1;
        if (soc_feature(unit, soc_feature_l2x_parity)) {
            int op_fail_pos;
            op_fail_pos = SOC_L2X_OP_FAIL_POS(unit);
            if ((schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff) {
                uint32 index = (schan_msg.readresp.data[2] >> nbits) &
                               ((1 << (32 - nbits)) - 1);
                index |= (schan_msg.readresp.data[3] << (32 - nbits)) &
                         soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "Lookup table[L2Xm]: Parity Error Index %d Bucket Bitmap 0x%08x\n"),
                           index,
                           (schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff ));
                return SOC_E_INTERNAL;
            }
        }
        return SOC_E_NOT_FOUND;
    }

    /* MACADDR<31:0> */
    result->entry_data[0] = schan_msg.readresp.data[0];

    /* CPU, PRI<2:0>, VLAN<12:0>, MACADDR<47:32> */
    result->entry_data[1] = schan_msg.readresp.data[1];

    /* HIT SA, HIT DA, VALID, MIRROR, RPE, STATIC_BIT, RESERVED,
     * MAC_BLOCK_INDEX, L3 MODULE_ID, PORT_TGID, SCP, SRC_DISCARD,
     * DST_DISCARD
     */
    result->entry_data[2] = schan_msg.readresp.data[2] & ((1 << nbits) - 1);

    *index_ptr = (schan_msg.readresp.data[2] >> nbits) &
                 ((1 << (32 - nbits)) - 1);
    *index_ptr |= (schan_msg.readresp.data[3] << (32 - nbits));
    *index_ptr &= soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "L2 entry lookup: ")));
        soc_mem_entry_dump(unit, L2Xm, result, BSL_INFO|BSL_LS_SOC_SOCMEM);
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             " (index=%d)\n"), *index_ptr));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fb_l2x_bank_insert
 * Purpose:
 *      Insert an entry into the L2X hash table.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      banks - For dual hashing, which halves are selected (inverted)
 *      entry - L2X entry to insert
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_FULL - hash bucket full
 *      SOC_E_BUSY - modfifo full
 * Notes:
 *      Uses hardware insertion; sends an L2 INSERT message over the
 *      S-Channel.  The hardware needs the L2Xm entry type.
 */

int
soc_fb_l2x_bank_insert(int unit, uint8 banks, l2x_entry_t *entry)
{
    schan_msg_t 	schan_msg;
    int			rv;
    int                 op_fail_pos;
    int entry_dw = soc_mem_entry_words(unit, L2Xm);
    int nbits;
    int                 dst_blk, src_blk, data_byte_len;
    uint32              bank_ignore_mask;
    int                 opcode, nack;

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "Insert table[L2_ENTRY]: ")));
        soc_mem_entry_dump(unit, L2Xm, entry, BSL_INFO|BSL_LS_SOC_SOCMEM);
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "\n")));
    }

    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    dst_blk = SOC_BLOCK2SCH(unit, IPIPE_BLOCK(unit));
    bank_ignore_mask = banks & 0x3;
    data_byte_len = entry_dw * 4;
    soc_schan_header_cmd_set(unit, &schan_msg.header, ARL_INSERT_CMD_MSG,
                             dst_blk, src_blk, 0, data_byte_len, 0,
                             bank_ignore_mask);  

    /* Fill in ARL packet data */

    sal_memcpy(schan_msg.l2x2.data, entry, sizeof (schan_msg.l2x2.data));

    /* Execute S-Channel operation (header word + 3 DWORDs) */

    rv = soc_schan_op(unit, &schan_msg, entry_dw + 1, entry_dw + 2, 1);
    if (SOC_FAILURE(rv)) {
        LOG_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                "soc_schan_op: operation failed: %s\n"), soc_errmsg(rv)));
    }

    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                NULL, NULL, &nack);
    if (opcode != ARL_INSERT_DONE_MSG) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_fb_l2x_bank_insert: "
                              "invalid S-Channel reply, expected L2_INSERT_ACK_MSG:\n")));
        soc_schan_dump(unit, &schan_msg, 1);
        return SOC_E_INTERNAL;
    }

    /*
     * =============================================================
     * | PERR_PBM | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * =============================================================
     */
    nbits = soc_mem_entry_bits(unit, L2Xm) % 32; /* Data Bits in last word */
    op_fail_pos = SOC_L2X_OP_FAIL_POS(unit);

    /* bit-106 OP_FAIL; bit-107 ModFifo Full */
    if ((nack != 0) || (rv == SOC_E_FAIL)) {
        if (schan_msg.readresp.data[3] & (1 << op_fail_pos)) {
            LOG_INFO(BSL_LS_SOC_SOCMEM,
                     (BSL_META_U(unit,
                                 "Insert table[L2Xm]: hash bucket full\n")));
            rv = SOC_E_FULL;
        } else if (schan_msg.readresp.data[3] & (1 << (op_fail_pos + 1))) {
            LOG_INFO(BSL_LS_SOC_SOCMEM,
                     (BSL_META_U(unit,
                                 "Insert table[L2Xm]: Modfifo full\n")));
            rv = SOC_E_BUSY;
        } else if (soc_feature(unit, soc_feature_l2x_parity) &&
                   ((schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff)) {
            uint32 index = (schan_msg.readresp.data[2] >> nbits) &
                           ((1 << (32 - nbits)) - 1);
            index |= (schan_msg.readresp.data[3] << (32 - nbits)) &
                     soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "Insert table[L2Xm]: Parity Error Index %d Bucket Bitmap 0x%08x\n"),
                       index,
                       (schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff ));
            rv = SOC_E_INTERNAL;
        } else {
            rv = SOC_E_FAIL;
        }
    }

    return rv;
}

/*
 * Function:
 *	soc_fb_l2x_bank_delete
 * Purpose:
 *	Delete an entry from the L2X hash table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *      banks - For dual hashing, which halves are selected (inverted)
 *	entry - L2X entry to delete
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_NOT_FOUND - hash bucket full
 *      SOC_E_BUSY - modfifo full
 * Notes:
 *	Uses hardware deletion; sends an L2 DELETE message over the
 *	S-Channel.  The hardware needs the L2Xm entry type.
 */

int
soc_fb_l2x_bank_delete(int unit, uint8 banks, l2x_entry_t *entry)
{
    schan_msg_t 	schan_msg;
    int			rv;
    int                 op_fail_pos;
    int                 nbits;
    int entry_dw = soc_mem_entry_words(unit, L2Xm);
    int                 dst_blk, src_blk, data_byte_len;
    uint32              bank_ignore_mask;
    int                 opcode, nack;

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "Delete table[L2Xm]: ")));
        soc_mem_entry_dump(unit, L2Xm, entry, BSL_INFO|BSL_LS_SOC_SOCMEM);
        LOG_INFO(BSL_LS_SOC_SOCMEM,
                 (BSL_META_U(unit,
                             "\n")));
    }

    schan_msg_clear(&schan_msg);
    src_blk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    dst_blk = SOC_BLOCK2SCH(unit, IPIPE_BLOCK(unit));
    bank_ignore_mask = banks & 0x3;
    data_byte_len = entry_dw * 4;
    soc_schan_header_cmd_set(unit, &schan_msg.header, ARL_DELETE_CMD_MSG,
                             dst_blk, src_blk, 0, data_byte_len, 0,
                             bank_ignore_mask);  

    /* Fill in packet data */

    sal_memcpy(schan_msg.l2x2.data, entry, sizeof (schan_msg.l2x2.data));

    /* Execute S-Channel operation (header word + 2 DWORDs) */
    rv = soc_schan_op(unit, &schan_msg, entry_dw + 1, entry_dw + 2, 1);

    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                NULL, NULL, &nack);
    if (opcode != ARL_DELETE_DONE_MSG) {
        LOG_WARN(BSL_LS_SOC_SCHAN,
            (BSL_META_U(unit,
                "soc_schan_op: operation failed: %s(%d)\n"), soc_errmsg(rv), rv));
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_fb_l2x_bank_delete: "
                              "invalid S-Channel reply, expected L2_DELETE_ACK_MSG:\n")));
        soc_schan_dump(unit, &schan_msg, 1);
        return SOC_E_INTERNAL;
    }

    /*
     * =============================================================
     * | PERR_PBM | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * =============================================================
     */
    nbits = soc_mem_entry_bits(unit, L2Xm) % 32; /* Data Bits in last word */
    op_fail_pos = SOC_L2X_OP_FAIL_POS(unit);

    /* bit-106 OP_FAIL; bit-107 ModFifo Full */
    if ((nack != 0) || (rv == SOC_E_FAIL)) {
        if (schan_msg.readresp.data[3] & (1 << op_fail_pos)) {
            LOG_INFO(BSL_LS_SOC_SOCMEM,
                     (BSL_META_U(unit,
                                 "Delete table[L2Xm]: Not found\n")));
            rv = SOC_E_NOT_FOUND;
        } else if (schan_msg.readresp.data[3] & (1 << (op_fail_pos + 1))) {
            LOG_INFO(BSL_LS_SOC_SOCMEM,
                     (BSL_META_U(unit,
                                 "Delete table[L2Xm]: Modfifo full\n")));
            rv = SOC_E_BUSY;
        } else if (soc_feature(unit, soc_feature_l2x_parity) &&
                   ((schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff)) {
            uint32 index = (schan_msg.readresp.data[2] >> nbits) &
                           ((1 << (32 - nbits)) - 1);
            index |= (schan_msg.readresp.data[3] << (32 - nbits)) &
                     soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "Delete table[L2Xm]: Parity Error Index %d Bucket Bitmap 0x%08x\n"),
                       index,
                       (schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff ));
            rv = SOC_E_INTERNAL;
        } else {
            rv = SOC_E_FAIL;
        }
    }

    return rv;
}

/*
 * Original non-bank versions of the FB L2X table op functions
 */

int
soc_fb_l2x_insert(int unit, l2x_entry_t *entry)
{
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_dual_hash)) {
        return
            _soc_mem_dual_hash_insert(unit, L2Xm, COPYNO_ALL, (void *)entry,
                          NULL, SOC_DUAL_HASH_MOVE_MAX_L2X(unit));
    } else
#endif
    {
        return soc_fb_l2x_bank_insert(unit, 0, entry);
    }
}

int
soc_fb_l2x_delete(int unit, l2x_entry_t *entry)
{
    return soc_fb_l2x_bank_delete(unit, 0, entry);
}

int
soc_fb_l2x_lookup(int unit, l2x_entry_t *key,
                  l2x_entry_t *result, int *index_ptr)
{
    return soc_fb_l2x_bank_lookup(unit, 0, key, result, index_ptr);
}

/*
 * Function:
 *	soc_fb_l2x_delete_all
 * Purpose:
 *	Clear the L2 table by invalidating entries.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	static_too - if TRUE, delete static and non-static entries;
 *		     if FALSE, delete only non-static entries
 * Returns:
 *	SOC_E_XXX
 */

int
soc_fb_l2x_delete_all(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			    index_min, index_max, index, mem_max;
    l2x_entry_t     *l2x_entry;
    int			rv = SOC_E_NONE;
    int             *buffer = NULL;
    int             mem_size, idx;

    index_min = soc_mem_index_min(unit, L2_ENTRY_ONLYm);
    mem_max = soc_mem_index_max(unit, L2_ENTRY_ONLYm);
    mem_size =  DEFAULT_L2DELETE_CHUNKS * sizeof(l2x_entry_t);

    buffer = soc_cm_salloc(unit, mem_size, "L2X_delete");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += DEFAULT_L2DELETE_CHUNKS) {
        index_max = idx + DEFAULT_L2DELETE_CHUNKS - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ANY,
                                     idx, index_max, buffer)) < 0 ) {
            break;
        }
        for (index = 0; index < index_max - idx + 1; index++) {
            l2x_entry =
                soc_mem_table_idx_to_pointer(unit, L2_ENTRY_ONLYm,
                                             l2x_entry_t *, buffer, index);
                sal_memcpy(l2x_entry, soc_mem_entry_null(unit, L2_ENTRY_ONLYm),
                           sizeof(l2x_entry_t));
        }
        if ((rv = soc_mem_write_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL, 
                                     idx, index_max, buffer)) < 0) {
            break;
        }
    }
    soc_mem_unlock(unit, L2Xm);

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }
    soc_cm_sfree(unit, buffer);
    

    return rv;
}

#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * Function:
 *	soc_th3_l2x_reset_freeze_state
 * Purpose:
 *	This function clears aging related information during attach sequence
 * Parameters:
 *  unit - StrataSwitch unit number
 * Returns:
 *	No value
 */
void soc_th3_l2x_reset_freeze_state(int unit)
{
    sal_memset(l2_freeze_state + unit, 0, sizeof(l2_freeze_t));
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

#endif /* BCM_XGS_SWITCH_SUPPORT */
