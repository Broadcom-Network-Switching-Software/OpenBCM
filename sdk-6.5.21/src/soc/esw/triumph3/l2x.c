/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS TR3 L2 Table Manipulation API routines.
 *
 * A low-level L2 shadow table is optionally kept in soc->arlShadow by
 * using a callback to get all inserts/deletes from the l2xmsg task.  It
 * can be disabled by setting the l2xmsg_avl property to 0.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ptable.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/ism_hash.h>
#include <soc/triumph3.h>
#include <soc/tcam/tcamtype1.h>
#include <soc/er_tcam.h>

#ifdef BCM_XGS_SWITCH_SUPPORT

#define _SOC_ALL_L2X_MEM_LOCK(unit) \
        SOC_L2X_MEM_LOCK(unit); \
        if (soc_feature(unit, soc_feature_esm_support)) { \
            SOC_EXT_L2X_MEM_LOCK(unit); \
        }

#define _SOC_ALL_L2X_MEM_UNLOCK(unit) \
        SOC_L2X_MEM_UNLOCK(unit); \
        if (soc_feature(unit, soc_feature_esm_support)) { \
            SOC_EXT_L2X_MEM_UNLOCK(unit); \
        }

#define _SOC_IF_ERROR_ALL_L2X_UNLOCK_RETURN(rv) \
        if (!SOC_SUCCESS(rv)) { \
            SOC_L2X_MEM_UNLOCK(unit); \
            if (soc_feature(unit, soc_feature_esm_support)) { \
                SOC_EXT_L2X_MEM_UNLOCK(unit); \
            } \
            LOG_ERROR(BSL_LS_SOC_L2, \
                      (BSL_META_U(unit, \
                                  "rv: %d\n"), rv)); \
            return rv; \
        }

#define _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, label) \
        if (!SOC_SUCCESS(rv)) { \
            SOC_L2X_MEM_UNLOCK(unit); \
            if (soc_feature(unit, soc_feature_esm_support)) { \
                SOC_EXT_L2X_MEM_UNLOCK(unit); \
            } \
            LOG_ERROR(BSL_LS_SOC_L2, \
                      (BSL_META_U(unit, \
                                  "rv: %d\n"), rv)); \
            goto label; \
        }

/*
 * While the L2 table is frozen, the L2Xm lock is held.
 *
 * All tasks must obtain the L2Xm lock before modifying the CML bits or
 * age timer registers.
 */

typedef struct l2_freeze_s {
    int frozen;
    int save_age_sec;
    int save_age_ena;
    sal_mutex_t l2_freeze_mutex;
} l2_freeze_t;

#define SOC_L2_FREEZE_LOCK(unit) \
    sal_mutex_take(tr3_l2_freeze_state[unit].l2_freeze_mutex, sal_mutex_FOREVER)
#define SOC_L2_FREEZE_UNLOCK(unit) \
    sal_mutex_give(tr3_l2_freeze_state[unit].l2_freeze_mutex)

typedef enum {
    _SOC_AGE_DEFAULT,
    _SOC_AGE_ON_HITSA_ONLY
} _soc_age_criteria_t;

typedef enum {
    RETRY_DEFAULT,
    RETRY_DELETE,
    RETRY_AGE
} retry_action;


l2_freeze_t tr3_l2_freeze_state[SOC_MAX_NUM_DEVICES];

#define _SOC_TR3_L2_TABLE_DMA_CHUNK_SIZE 1024
#define _SOC_TR3_VALID_MAX_AGE_INTERVAL  (2147)
#define _SOC_TR3_L2AGE_USEC_IN_ONESEC    (1000000)

/*
 * Function:
 *	soc_tr3_l2_entry_compare_key
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
soc_tr3_l2_entry_compare_key(void *user_data,
                             shr_avl_datum_t *datum1,
                             shr_avl_datum_t *datum2)
{
    int		unit = PTR_TO_INT(user_data);

    return _soc_mem_cmp_tr3_l2x(unit, (void *)datum1, (void *)datum2);
}

int
soc_tr3_ext_l2_entry_compare_key(void *user_data,
                                 shr_avl_datum_t *datum1,
                                 shr_avl_datum_t *datum2)
{
    int		unit = PTR_TO_INT(user_data);

    return _soc_mem_cmp_tr3_ext_l2x(unit, (void *)datum1, (void *)datum2);
}

/*
 * Function:
 *	soc_tr3_l2_entry_dump
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
soc_tr3_l2_entry_dump(void *user_data, shr_avl_datum_t *datum, void *extra_data)
{
    uint32 val;
    int	unit = PTR_TO_INT(user_data);
    soc_mem_t mem_type = L2_ENTRY_1m;

    COMPILER_REFERENCE(extra_data);

    val = soc_mem_field32_get(unit, L2_ENTRY_1m, (l2_entry_1_entry_t *)datum, KEY_TYPEf);
    if ((val == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) || 
        (val == SOC_MEM_KEY_L2_ENTRY_2_L2_VFI) ||
        (val == SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS)) {
        mem_type = L2_ENTRY_2m;
    }
    
    if (mem_type == L2_ENTRY_1m) {
        soc_mem_entry_dump(unit, L2_ENTRY_1m, (l2x_entry_t *)datum, BSL_LSS_CLI);
    } else {
        soc_mem_entry_dump(unit, L2_ENTRY_2m, (l2x_entry_t *)datum, BSL_LSS_CLI);
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return SOC_E_NONE;
}

int
soc_tr3_ext_l2_1_entry_dump(void *user_data, shr_avl_datum_t *datum, void *extra_data)
{
    int	unit = PTR_TO_INT(user_data);

    COMPILER_REFERENCE(extra_data);

    soc_mem_entry_dump(unit, EXT_L2_ENTRY_1m, (l2x_entry_t *)datum, BSL_LSS_CLI);
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return SOC_E_NONE;
}

int
soc_tr3_ext_l2_2_entry_dump(void *user_data, shr_avl_datum_t *datum, void *extra_data)
{
    int	unit = PTR_TO_INT(user_data);

    COMPILER_REFERENCE(extra_data);

    soc_mem_entry_dump(unit, EXT_L2_ENTRY_2m, (l2x_entry_t *)datum, BSL_LSS_CLI);
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_tr3_l2_shadow_callback
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
soc_tr3_l2_shadow_callback(int unit, uint32 flags, soc_mem_t mem,
                           l2_combo_entry_t *entry_del,
                           l2_combo_entry_t *entry_add,
                           void *fn_data)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
    if ((mem == L2_ENTRY_1m) || (mem == L2_ENTRY_2m)) {
        if (entry_del != NULL) {
            shr_avl_delete(soc->arlShadow, soc_tr3_l2_entry_compare_key,
                           (shr_avl_datum_t *)entry_del);
        }
        if (entry_add != NULL) {
            shr_avl_insert(soc->arlShadow, soc_tr3_l2_entry_compare_key,
                           (shr_avl_datum_t *)entry_add);
        }
    } else if (soc_feature(unit, soc_feature_esm_support)) {
        if (mem == EXT_L2_ENTRY_1m) {
            if (entry_del != NULL) {
                shr_avl_delete(soc->arlShadow_ext1, soc_tr3_ext_l2_entry_compare_key,
                               (shr_avl_datum_t *)entry_del);
            }
            if (entry_add != NULL) {
                shr_avl_insert(soc->arlShadow_ext1, soc_tr3_ext_l2_entry_compare_key,
                               (shr_avl_datum_t *)entry_add);
            }
        } else if (mem == EXT_L2_ENTRY_2m) {
            if (entry_del != NULL) {
                shr_avl_delete(soc->arlShadow_ext2, soc_tr3_ext_l2_entry_compare_key,
                               (shr_avl_datum_t *)entry_del);
            }
            if (entry_add != NULL) {
                shr_avl_insert(soc->arlShadow_ext2, soc_tr3_ext_l2_entry_compare_key,
                               (shr_avl_datum_t *)entry_add);
            }
        }
    }
    sal_mutex_give(soc->arlShadowMutex);
}

/*
 * Function:
 *	soc_tr3_l2_detach
 * Purpose:
 *	Deallocate L2X subsystem resources
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 */

int
soc_tr3_l2_detach(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    soc_l2_entry_unregister(unit, soc_tr3_l2_shadow_callback, NULL);

    /* Free cml_freeze structure  */
    _soc_l2x_cml_struct_free(unit);

    if (soc->arlShadow != NULL) {
        shr_avl_destroy(soc->arlShadow);
        soc->arlShadow = NULL;
    }    
    if (soc_feature(unit, soc_feature_esm_support)) {
        if (soc->arlShadow_ext1 != NULL) {
            shr_avl_destroy(soc->arlShadow_ext1);
            soc->arlShadow_ext1 = NULL;
        }
        if (soc->arlShadow_ext2 != NULL) {
            shr_avl_destroy(soc->arlShadow_ext2);
            soc->arlShadow_ext2 = NULL;
        }
    }

    if (soc->arlShadowMutex != NULL) {
        sal_mutex_destroy(soc->arlShadowMutex);
        soc->arlShadowMutex = NULL;
    }

    if (tr3_l2_freeze_state[unit].l2_freeze_mutex != NULL) {
        sal_mutex_destroy(tr3_l2_freeze_state[unit].l2_freeze_mutex);
        tr3_l2_freeze_state[unit].l2_freeze_mutex = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_tr3_l2_attach
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
soc_tr3_l2_attach(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions = NULL;
    
    (void)soc_tr3_l2_detach(unit);

    tcam_info = soc->tcam_info;
    if (tcam_info) {
        partitions = tcam_info->partitions;
    }
    if (soc_property_get(unit, spn_L2XMSG_AVL, TRUE)) {
        int datum_bytes, datum_max;
        
        datum_bytes = sizeof(l2_entry_1_entry_t);
        datum_max = tcam_info ? partitions[TCAM_PARTITION_FWD_L2].num_entries :
                    soc_mem_index_count(unit, L2_ENTRY_1m);
        if (shr_avl_create(&soc->arlShadow,
                           INT_TO_PTR(unit),
                           datum_bytes,
                           datum_max) < 0) {
            return SOC_E_MEMORY;
        }
        if (soc_feature(unit, soc_feature_esm_support)) {
            datum_bytes = sizeof(ext_l2_entry_1_entry_t);
            datum_max = tcam_info ? 
                            partitions[TCAM_PARTITION_FWD_L2].num_entries : 
                            soc_mem_index_count(unit, EXT_L2_ENTRY_1m);
            if (shr_avl_create(&soc->arlShadow_ext1,
                               INT_TO_PTR(unit),
                               datum_bytes,
                               datum_max) < 0) {
                (void)soc_tr3_l2_detach(unit);
                return SOC_E_MEMORY;
            }
            datum_bytes = sizeof(ext_l2_entry_2_entry_t);
            datum_max = tcam_info ? 
                        partitions[TCAM_PARTITION_FWD_L2_WIDE].num_entries : 
                        soc_mem_index_count(unit, EXT_L2_ENTRY_2m);
            if (shr_avl_create(&soc->arlShadow_ext2,
                               INT_TO_PTR(unit),
                               datum_bytes,
                               datum_max) < 0) {
                (void)soc_tr3_l2_detach(unit);
                return SOC_E_MEMORY;
            }
        }
        if ((soc->arlShadowMutex = sal_mutex_create("asMutex")) == NULL) {
            (void)soc_tr3_l2_detach(unit);
            return SOC_E_MEMORY;
        }

        soc_l2_entry_register(unit, soc_tr3_l2_shadow_callback, NULL);
    }

    /* Reset l2 freeze structure. */
    sal_memset(&tr3_l2_freeze_state[unit], 0, sizeof(l2_freeze_t));
    if ((tr3_l2_freeze_state[unit].l2_freeze_mutex
             = sal_mutex_create("tr3_l2_freeze_mutex")) == NULL) {
        (void)soc_tr3_l2_detach(unit);
        return SOC_E_MEMORY;
    }

    /* Init cml state */
    SOC_IF_ERROR_RETURN(_soc_l2x_cml_struct_alloc(unit));
	
    return SOC_E_NONE;
}

/*
 * Function:
 *	_soc_tr3_l2_port_age
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
soc_tr3_l2_port_age(int unit, soc_reg_t reg0, soc_reg_t reg1)
{
    static soc_field_t fields[2] = { STARTf, COMPLETEf };
    static uint32      values[2] = { 1, 0 };
    soc_timeout_t      to;
    int                rv;
    int                timeout_usec;
    int                reg0_complete, reg1_complete;
    uint32             rval;
    int                mode;

    if (reg0 == INVALIDr && reg1 == INVALIDr) {
        return SOC_E_NONE;
    }

    timeout_usec = SOC_CONTROL(unit)->l2x_age_timeout;
    mode = SOC_CONTROL(unit)->l2x_mode;

    if (mode != L2MODE_FIFO) {
        /* Disable learning etc. */ 
        SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_save(unit));
    }

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

    SOC_CONTROL(unit)->l2x_ppa_in_progress = TRUE;
    soc_timeout_init(&to, timeout_usec, 0);
    for (;;) {
        if (!reg0_complete) {
            rv = soc_reg32_get(unit, reg0, REG_PORT_ANY, 0, &rval);
            if (SOC_SUCCESS(rv)) {
                reg0_complete = soc_reg_field_get(unit, reg0, rval, COMPLETEf);
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
        sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 1000);
    }

 done:
    SOC_CONTROL(unit)->l2x_ppa_in_progress = FALSE;
    if (mode != L2MODE_FIFO) {
        SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_restore(unit));
    }
    return rv;
}

/*
 * Function:
 *	    soc_tr3_l2_is_frozen
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
soc_tr3_l2_is_frozen(int unit, int *frozen)
{
    l2_freeze_t	*f_l2 = &tr3_l2_freeze_state[unit];

    *frozen = (f_l2->frozen > 0) ? TRUE : FALSE ;

    return (SOC_E_NONE);
}

/*
 * Function:
 *	    soc_tr3_l2_freeze
 * Purpose:
 *   	Temporarily quiesce L2 table from all activity (learning, aging)
 * Parameters:
 *	   unit - (IN) BCM device number. 
 * Returns:
 *	   SOC_E_XXX
 * Notes:
 *	Leaves L2Xm locked until corresponding thaw.
 *	PORT_TABm is locked in order to lockout bcm_port calls
 *	bcm_port calls will callout to soc_arl_frozen_cml_set/get
 */

int
soc_tr3_l2_freeze(int unit)
{
    l2_freeze_t *f_l2 = &tr3_l2_freeze_state[unit];
    int         rv = SOC_E_NONE;

    SOC_L2_FREEZE_LOCK(unit);
    /* Check if already frozen. */
    SOC_L2X_MEM_LOCK(unit);
    if (f_l2->frozen > 0) {
        /* Keep count - do nothing. */
        f_l2->frozen++;
        SOC_L2X_MEM_UNLOCK(unit);
        return (SOC_E_NONE);
    }
    SOC_L2X_MEM_UNLOCK(unit);
    /* Disable learning etc. */ 
    rv = _soc_l2x_frozen_cml_save(unit);
    if (SOC_FAILURE(rv)) {
        SOC_L2_FREEZE_UNLOCK(unit);
        return rv;
    }

    /*
     * Disable learning and aging.
     */
    /* Read l2 aging interval. */
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &f_l2->save_age_sec,
                                                &f_l2->save_age_ena);
    if (SOC_FAILURE(rv)) {
        _soc_l2x_frozen_cml_restore(unit);
        SOC_L2_FREEZE_UNLOCK(unit);
        return rv;
    }
    /* If l2 aging is on - disable it. */
    if (f_l2->save_age_ena) {
        rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, 
                                                    f_l2->save_age_sec, 0);
        if (SOC_FAILURE(rv)) {
            _soc_l2x_frozen_cml_restore(unit);
            SOC_L2_FREEZE_UNLOCK(unit);
            return rv;
        }
    }
    SOC_L2X_MEM_LOCK(unit);
    /* Increment  l2 frozen indicator. */
    f_l2->frozen++;

    SOC_L2_FREEZE_UNLOCK(unit);
    return (SOC_E_NONE);
}


/*
 * Function:
 *   	soc_tr3_l2_thaw
 * Purpose:
 *	    Restore normal L2 activity.
 * Parameters:
 *	    unit - (IN) BCM device number.
 * Returns:
 *	    SOC_E_XXX
 */

int
soc_tr3_l2_thaw(int unit)
{
    l2_freeze_t *f_l2 = &tr3_l2_freeze_state[unit];
    int         l2rv, cmlrv;

    /* Sanity check to protect from thaw without freeze. */
    if (f_l2->frozen == 0 ) {
        assert(0);
    }

    /* In case of nested freeze/thaw just decrement reference counter. */
    SOC_L2X_MEM_LOCK(unit);
    if (f_l2->frozen > 1) {
        f_l2->frozen--;
        SOC_L2X_MEM_UNLOCK(unit);
        return (SOC_E_NONE);
    }
    SOC_L2X_MEM_UNLOCK(unit);
    /*
     * Last thaw restore L2 learning and aging.
     */
    l2rv = SOC_E_NONE;
    if (f_l2->save_age_ena) {
        l2rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, f_l2->save_age_sec,
                                                      1);
    }
    /* L2 freeze reference counter decrement. */
    f_l2->frozen--;
    SOC_L2X_MEM_UNLOCK(unit);

    /* Restore port learning mode. */
    cmlrv = _soc_l2x_frozen_cml_restore(unit);
    if (SOC_FAILURE(l2rv)) {
        return l2rv;
    }
    return cmlrv;
}

static int _soc_tr3_l2_bulk_age_iter[SOC_MAX_NUM_DEVICES] = {0};

/*
 * Function:
 *  _soc_tr3_l2_do_age
 * Purpose:
 *      set up match criteria and execute bulk
 *      operations on l2 table
 * Parameters:
 *  unit - unit number.
 *  flags- to specify match criteria
 *  retry_loc-to store the memory if it returns a time_out
 * Returns:
 *  none
 */

STATIC void
_soc_tr3_l2_do_age(int unit, uint32 flags, int8 *retry_loc) {
    uint32 rval;
    int field_len, rv;
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t stime, etime;
    l2_bulk_entry_t l2_bulk;
    l2_entry_1_entry_t l2_entry_1;
    l2_entry_2_entry_t l2_entry_2;
    ext_l2_entry_1_entry_t ext_l2_entry_1;
    ext_l2_entry_2_entry_t ext_l2_entry_2;
    int age_on_hitsa_only = flags & _SOC_AGE_ON_HITSA_ONLY;

    stime = sal_time_usecs();

    _SOC_ALL_L2X_MEM_LOCK(unit);
    if (soc_mem_index_count(unit, L2_ENTRY_1m) == 0) {
        goto skip_age_int_l2;
    }
    if (*retry_loc > 0) {
        goto skip_age_int_l2_1;
    }
    *retry_loc = 0;
    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memset(&l2_entry_1, 0, sizeof(l2_entry_1));

    /* First work on L2_ENTRY_1 */
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, VALIDf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, WIDEf, 1);

    field_len = soc_mem_field_length(unit, L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, STATIC_BITf, 1);
    if (age_on_hitsa_only) {
        soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, HITSAf, 1);
    }
    sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
    /* Set match mask */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, WIDEf, 0);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf,
                        SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, STATIC_BITf, 0);
    if (age_on_hitsa_only) {
        soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, HITSAf, 0);
    }
    sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
    /* Set match data */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 1);
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    }
    if (age_on_hitsa_only) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 3);
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf,
                      _SOC_TR3_L2_BULK_BURST_MIN);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf,
                      soc_mem_index_count(unit, L2_ENTRY_1m));
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 0);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    /* Age entries for SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE */
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    if (rv == SOC_E_TIMEOUT) {
        goto skip_age_ext_l2_2;
    }
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &l2_entry_1, KEY_TYPEf,
                        SOC_MEM_KEY_L2_ENTRY_1_L2_VFI);
    sal_memcpy(&l2_bulk, &l2_entry_1, sizeof(l2_entry_1));
    /* Set match data */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    /* Age entries for SOC_MEM_KEY_L2_ENTRY_1_L2_VFI */
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    if (rv == SOC_E_TIMEOUT) {
        goto skip_age_ext_l2_2;
    }

    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    *retry_loc = -1;

skip_age_int_l2_1:
     if (*retry_loc > 1) {
         goto skip_age_int_l2;
     }
     *retry_loc = 1;
     sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
     sal_memset(&l2_entry_2, 0, sizeof(l2_entry_2));
     /* Then work on L2_ENTRY_2 */
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, VALID_0f, 1);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, VALID_1f, 1);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, WIDE_0f, 1);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, WIDE_1f, 1);
     field_len = soc_mem_field_length(unit, L2_ENTRY_2m, KEY_TYPE_0f);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f,
                         (1 << field_len) - 1);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f,
                         (1 << field_len) - 1);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_0f, 1);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_1f, 1);
     if (age_on_hitsa_only) {
         soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, HITSAf, 1);
     }
     sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
     /* Set match mask */
     rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
     _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f,
                         SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f,
                         SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_0f, 0);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, STATIC_BIT_1f, 0);
     if (age_on_hitsa_only) {
         soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, HITSAf, 0);
     }
     sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
     /* Set match data */
     rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
     _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
     rv = READ_L2_BULK_CONTROLr(unit, &rval);
     _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
     if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
         soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 1);
     } else {
         soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
     }
     if (age_on_hitsa_only) {
         soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
     } else {
         soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 3);
     }
     soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf,
                       _SOC_TR3_L2_BULK_BURST_MIN);
     soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
     soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf,
                       soc_mem_index_count(unit, L2_ENTRY_2m));
     rv = WRITE_L2_BULK_CONTROLr(unit, rval);
     _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
     /* Age entries for SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE */
     rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
     if (rv == SOC_E_TIMEOUT) {
         goto skip_age_ext_l2_2;
     }
     _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_0f,
                         SOC_MEM_KEY_L2_ENTRY_2_L2_VFI);
     soc_mem_field32_set(unit, L2_ENTRY_2m, &l2_entry_2, KEY_TYPE_1f,
                         SOC_MEM_KEY_L2_ENTRY_2_L2_VFI);
     sal_memcpy(&l2_bulk, &l2_entry_2, sizeof(l2_entry_2));
     /* Set match data */
     rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
     _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
     /* Age entries for SOC_MEM_KEY_L2_ENTRY_2_L2_VFI */
     rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
     if (rv == SOC_E_TIMEOUT) {
         goto skip_age_ext_l2_2;
     }
     _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);

     *retry_loc = -1;

skip_age_int_l2:
    if (!soc_feature(unit, soc_feature_esm_support)) {
        goto skip_age_ext_l2_2;
    }
    if (soc_mem_index_count(unit, EXT_L2_ENTRY_1m) == 0) {
        goto skip_age_ext_l2_1;
    }
    if (*retry_loc > 2) {
        goto skip_age_ext_l2_1;
    }
    *retry_loc = 2;
    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memset(&ext_l2_entry_1, 0, sizeof(ext_l2_entry_1));
    /* First work on EXT_L2_ENTRY_1 */
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, FREEf, 1);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, STATIC_BITf, 1);
    if (age_on_hitsa_only) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, HITSAf, 1);
    }
    sal_memcpy(&l2_bulk, &ext_l2_entry_1, sizeof(ext_l2_entry_1));

    /* Set match mask */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, FREEf, 0);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, STATIC_BITf, 0);
    if (age_on_hitsa_only) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_1m, &ext_l2_entry_1, HITSAf, 0);
    }
    sal_memcpy(&l2_bulk, &ext_l2_entry_1, sizeof(ext_l2_entry_1));
    /* Set match data */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 1);
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    }
    if (age_on_hitsa_only) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 3);
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf,
                      _SOC_TR3_L2_BULK_BURST_MIN);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf,
                  SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 1);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    /* Age entries */
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    if (rv == SOC_E_TIMEOUT) {
        goto skip_age_ext_l2_2;
    }
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    /* Issue dummy op */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    if (rv == SOC_E_TIMEOUT) {
        goto skip_age_ext_l2_2;
    }
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);

    *retry_loc = -1;

skip_age_ext_l2_1:
    if (soc_mem_index_count(unit, EXT_L2_ENTRY_2m) == 0) {
        goto skip_age_ext_l2_2;
    }
    if (*retry_loc >= 0 && *retry_loc < 3) {
        goto skip_age_ext_l2_2;
    }
    *retry_loc = 3;
    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memset(&ext_l2_entry_2, 0, sizeof(ext_l2_entry_2));

    /* Then work on EXT_L2_ENTRY_2 */
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, FREEf, 1);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, STATIC_BITf, 1);
    if (age_on_hitsa_only) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, HITSAf, 1);
    }
    sal_memcpy(&l2_bulk, &ext_l2_entry_2, sizeof(ext_l2_entry_2));
    /* Set match mask */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 1, &l2_bulk);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, FREEf, 0);
    soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, STATIC_BITf, 0);
    if (age_on_hitsa_only) {
        soc_mem_field32_set(unit, EXT_L2_ENTRY_2m, &ext_l2_entry_2, HITSAf, 0);
    }
    sal_memcpy(&l2_bulk, &ext_l2_entry_2, sizeof(ext_l2_entry_2));
    /* Set match data */
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, 0, &l2_bulk);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = READ_L2_BULK_CONTROLr(unit, &rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 1);
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    }
    if (age_on_hitsa_only) {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    } else {
        soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 3);
    }
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf,
                      _SOC_TR3_L2_BULK_BURST_MIN);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf,
                  SOC_MEM_TR3_EXT_L2_MAX_ENTRIES);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    /* Age entries */
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    if (rv == SOC_E_TIMEOUT) {
        goto skip_age_ext_l2_2;
    }
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    /* Issue dummy op */
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 1);
    rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    if (rv == SOC_E_TIMEOUT) {
        goto skip_age_ext_l2_2;
    }
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);
    rv = WRITE_ETU_EXT_L2_BULK_INFOr(unit, 0);
    _SOC_IF_ERROR_ALL_L2X_UNLOCK_JUMP(rv, cleanup_exit);

    *retry_loc = -1;

skip_age_ext_l2_2:
    _SOC_ALL_L2X_MEM_UNLOCK(unit);
    etime = sal_time_usecs();
    LOG_VERBOSE(BSL_LS_SOC_ARL,
                (BSL_META_U(unit,
                            "l2_bulk_age_thread: unit=%d: done in %d usec\n"),
                 unit, SAL_USECS_SUB(etime, stime)));

    return;

cleanup_exit:
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "l2_bulk_age_thread: exiting\n")));
    (void)sal_sem_take(soc->l2x_age_notify, 1);
    soc->l2x_age_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Function:
 *  _soc_tr3_l2age_sem_wait
 * Purpose:
 *  Wait in soc->l2x_age_notify semaphore for specified interval
 * Parameters:
 *  wait_time -  amount of time to be waited on semaphore
 * Returns:
 *  usec - time spent in semaphore 
 */


STATIC
sal_usecs_t _soc_tr3_l2age_sem_wait(void * unit_vp, sal_usecs_t wait_time)
{
    int unit = PTR_TO_INT(unit_vp);
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t timebefore_agewait = 0, timeafter_agewait = 0, retval = 0;

    timebefore_agewait  = sal_time_usecs();
    (void)sal_sem_take(soc->l2x_age_notify, wait_time);
    timeafter_agewait = sal_time_usecs();
    if (timeafter_agewait > timebefore_agewait) {
        retval = timeafter_agewait - timebefore_agewait;
    }
    else {
        retval = ((0xFFFFFFFF - timebefore_agewait) + timeafter_agewait);
    }

    return (retval);
}


/*
 * Function:
 *  _soc_tr3_l2_bulk_age
 * Purpose:
 *      l2 bulk age thread
 * Parameters:
 *  unit - unit number.
 * Returns:
 *  none
 */
STATIC void
_soc_tr3_l2_bulk_age(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    int c, m, r, iter = 0;
    uint32 flags = 0;
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval;
    sal_usecs_t timebefore_agewait = 0, timeafter_agewait = 0;
    sal_usecs_t actual_wait_time= 0, wait_interval = 0;
    int skip_tr3_l2age_semops = 0;
    int8 retry_loc = -1;  /* To store the memory if bulk action times out */

    /* In case a bulk operation times out, we need to store which action timed
       out in order to retry. This variable stores the failed action -
       age or delete. If action succeds this variable is again set back to
       RETRY_DEFAULT */
    int retry_action = RETRY_DEFAULT;

    timebefore_agewait  = sal_time_usecs();
    while((interval = soc->l2x_age_interval) != 0) {
        if (!iter) {
            goto age_delay;
        }

run_tr3_l2bulkage_ops:
        LOG_VERBOSE(BSL_LS_SOC_ARL,
                    (BSL_META_U(unit,
                                "l2_bulk_age_thread: "
                                "Process iters(total:%d, this run:%d\n"),
                     ++_soc_tr3_l2_bulk_age_iter[unit], iter));

        timebefore_agewait  = sal_time_usecs();
        soc->l2x_agetime_adjust_usec = 0;
        soc->l2x_agetime_curr_timeblk = 0;
        soc->l2x_agetime_curr_timeblk_usec = 0;


    /* If aging should be based on HITSA only, we need to delete all entries
       HITSA = 0 and HITDA = 1 first, and then perform an age action. Age action
       deletes entries with HITSA = 0 and HITDA = 0, and clears all hit bits */
    if (soc->l2x_age_hitsa_only) {
        flags |= _SOC_AGE_ON_HITSA_ONLY;
        if (retry_action == RETRY_DEFAULT || retry_action == RETRY_DELETE) {
            _soc_tr3_l2_do_age(unit, flags, &retry_loc);

            if (retry_loc != -1) {
                /* Bulk delete failed. Need to retry this action */
                retry_action = RETRY_DELETE;
                goto age_delay;
            } else {
                /* Bulk delete succeeded */
                retry_action = RETRY_DEFAULT;
            }
        }
    }

    if (retry_action == RETRY_DEFAULT || retry_action == RETRY_AGE) {
        _soc_tr3_l2_do_age(unit, 0, &retry_loc);

        if (retry_loc != -1) {
            /* Bulk age failed. Need to retry this action */
            retry_action = RETRY_AGE;
            goto age_delay;
        } else {
            /* Bulk age succeeded */
            retry_action = RETRY_DEFAULT;
        }
    }

    /* age delay processing normally wait in l2x_age_notify semaphore for the
     * specified interval before running bulk operation, now l2 bulk operation
     * API like delete_by_vlan/port will stop and start the age thread.
     * Here during exit, aging thread keeps track of amount of time spent on
     * semaphore in current cycle.  Now when aging thread restarted, it waits
     * (aging interval - time spent on previuos cycle). Lets say, aging interval
     * is 50 sec, aging thread run for 20 sec. At that time, delete by vlan API
     * gets called, it stops and restarts the aging thread. This time aging 
     * thread will wait for (50 sec - 20 sec) =  30 sec before running bulk ops
     *
     * _SOC_TR3_VALID_MAX_AGE_INTERVAL is 2147 sec. Now sem_take takes usec
     * integer vlaue. Max integer vlaue that can be adjusted in 31 bit is
     * 0x7FFFFFFF which approximates to (2147 * 1000000) usec. If age interval
     * is greater then 2147 sec, it is splitted to number of 2147 time blocks
     * and reminder part. For example, if age time is 2150 sec, there will be 
     * (2150/2147 = 1) , one 2147 time block and (2150%2147= 3) , three sec 
     * reminder part. Thread will wait first 2147 sec and then 3 sec before 
     * running bulk operation.
     *
     * if age time is less that 2147 sec, soc->l2x_agetime_adjust_usec keeps
     * track of amount of time spent on semaphore in current cycle. It  is 
     * accumulative in nature , resetted before running bulk ops.
     *
     * if age time is greater then 2147 sec, soc->l2x_agetime_curr_timeblk 
     * keeps track of current time block index and 
     * soc->l2x_agetime_curr_timeblk_usec keeps track of amount of time 
     * spent of sem. during current cycle. soc->l2x_agetime_adjust_usec keeps
     * track of reminder part of age time. All the variable are accumulative 
     * in nature and resetted before running bulk ops.
     */
age_delay:
        /* If this flow is due to a force-bulk-op, goto exit, no ageing 
        necessary */
        if (skip_tr3_l2age_semops) {
            goto post_skip_l2bulkage_semops;
        }
        if (interval > _SOC_TR3_VALID_MAX_AGE_INTERVAL) {
            int aging_off = 0;

            /* age interval greater than 2147 processing. */

            /* Cumpute the number of '_SOC_TR3_VALID_MAX_AGE_INTERVAL' blocks
            in the current interval */
            m = interval / _SOC_TR3_VALID_MAX_AGE_INTERVAL;

            /* Gather the reminder of the interval that is < than a block
            boundary */
            r = interval % _SOC_TR3_VALID_MAX_AGE_INTERVAL;

            /* Loop thru the # of blocks waiting. If we have already waited on
            the sem earlier, the 'l2x_agetime_curr_timeblk' will keep track of
            how long, so start there */
            for (c = soc->l2x_agetime_curr_timeblk; c < m; c++) {
                /* Compute the mac usec's per '_SOC_TR3_VALID_MAX_AGE_INTERVAL'
                block */
                wait_interval = _SOC_TR3_VALID_MAX_AGE_INTERVAL
                                * _SOC_TR3_L2AGE_USEC_IN_ONESEC;
                /* Make sure 'wait_interval' is > than the accumulated time */
                if (soc->l2x_agetime_curr_timeblk_usec < wait_interval) {
                    /* Wait for the duration - any accumulated time */
                    actual_wait_time = _soc_tr3_l2age_sem_wait(unit_vp,
                               wait_interval - soc->l2x_agetime_curr_timeblk_usec);
                    /* Take into account how long we waited */
                    soc->l2x_agetime_curr_timeblk_usec += actual_wait_time;
                    /* If the user is doing a bulk-op, 'soc->l2x_age_interval'
                    would have gone to 0, if so, turn off aging and break */
                    if (!soc->l2x_age_interval) {
                        aging_off =1;
                        break;
                    } else if (interval != soc->l2x_age_interval) {
                        interval = soc->l2x_age_interval;
                        soc->l2x_agetime_curr_timeblk_usec = 0;
                        goto age_delay;
                    }
                }
                /* In case of the user changes the age_interval while we are 
                waiting on sem, the sem_wait exits sooner hence need to turn 
                off aging, which will run bulk-op, init all vars and go back
                to sem-wait */
                if (wait_interval > soc->l2x_agetime_curr_timeblk_usec) {
                    aging_off = 1;
                    break;
                }
                /* Increment how many blocks we have traversed thus far */
                soc->l2x_agetime_curr_timeblk++;
                soc->l2x_agetime_curr_timeblk_usec = 0;
            }
            /* Wait on the sem for the reminder of the interval that is < than a
            block */
            if (!aging_off) {
                if ( (r * _SOC_TR3_L2AGE_USEC_IN_ONESEC) >
                            soc->l2x_agetime_adjust_usec ) {
                    wait_interval = (r*_SOC_TR3_L2AGE_USEC_IN_ONESEC) -
                                    soc->l2x_agetime_adjust_usec;
                    soc->l2x_agetime_adjust_usec +=
                                _soc_tr3_l2age_sem_wait(unit_vp, wait_interval);
                    if (soc->l2x_age_interval && (interval != soc->l2x_age_interval)) {
                        interval = soc->l2x_age_interval;
                        goto age_delay;
                    }
                }
            }
        } else {
            /* age time interval less then 2147 sec processing */
            if ((interval * _SOC_TR3_L2AGE_USEC_IN_ONESEC) >
                                soc->l2x_agetime_adjust_usec) {
                (void)sal_sem_take(soc->l2x_age_notify,
                                  (interval * _SOC_TR3_L2AGE_USEC_IN_ONESEC)
                                  - soc->l2x_agetime_adjust_usec);
            }
            if (soc->l2x_age_interval && (interval != soc->l2x_age_interval)) {
                interval = soc->l2x_age_interval;
                goto age_delay;
            }
            /* After the current pass, note the time after age_wait */
            timeafter_agewait = sal_time_usecs();
        }
        iter++;
    }

    /* age thread exit processing */
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "l2_bulk_age_thread: exiting\n")));
    /* Check if the previous accumulated time exceeds the max interval? */
    if (soc->l2x_prev_age_timeout <= _SOC_TR3_VALID_MAX_AGE_INTERVAL) {
        if (!timeafter_agewait) {
            /* If time_after is not set yet, do it now. This happends when
            thread is not schedduled but is exiting */
            timeafter_agewait = sal_time_usecs();
        }
        /* Normal case, where after is less than before*/
        if (timeafter_agewait >= timebefore_agewait) {
            soc->l2x_agetime_adjust_usec += 
                    (timeafter_agewait - timebefore_agewait);
        }
        else {
            /* This is a wrap around case, where after is less than before */
            soc->l2x_agetime_adjust_usec +=
                    ((0xFFFFFFFF - timebefore_agewait) + timeafter_agewait);
        }

        /* Check for a special case where accumulated time exceeds the wait 
        interval In such a case we need to force a bulk-op, before exiting */
        if ((soc->l2x_prev_age_timeout) && (soc->l2x_agetime_adjust_usec >=
                (soc->l2x_prev_age_timeout * _SOC_TR3_L2AGE_USEC_IN_ONESEC))) {
            skip_tr3_l2age_semops = 1;
            /* Goto the force bulk op section */
            goto run_tr3_l2bulkage_ops;
        }
    }
post_skip_l2bulkage_semops:
    (void)sal_sem_take(soc->l2x_age_notify, 1);
    soc->l2x_age_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);

}
                                                                     
/*
 * Function:
 * 	soc_tr3_l2_bulk_age_start
 * Purpose:
 *   	Start l2 bulk age thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */
int
soc_tr3_l2_bulk_age_start(int unit, int interval)
{
    int cfg_interval;
    soc_control_t *soc = SOC_CONTROL(unit);

    cfg_interval = soc_property_get(unit, spn_L2_SW_AGING_INTERVAL, 
                                    SAL_BOOT_QUICKTURN ? 30 : 10);
    SOC_CONTROL_LOCK(unit);
    soc->l2x_age_interval = interval ? interval : cfg_interval;
    sal_snprintf(soc->l2x_age_name, sizeof (soc->l2x_age_name),
                 "bcmL2age.%d", unit);
    soc->l2x_age_pid = sal_thread_create(soc->l2x_age_name, SAL_THREAD_STKSZ,
                                         soc_property_get(unit, spn_L2AGE_THREAD_PRI, 50),
                                         _soc_tr3_l2_bulk_age, INT_TO_PTR(unit));
    if (soc->l2x_age_pid == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "bcm_esw_l2_init: Could not start L2 bulk age thread\n")));
        SOC_CONTROL_UNLOCK(unit);
        return SOC_E_MEMORY;
    }
    SOC_CONTROL_UNLOCK(unit);
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_tr3_l2_bulk_age_stop
 * Purpose:
 *   	Stop l2 bulk age thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */
int
soc_tr3_l2_bulk_age_stop(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = SOC_E_NONE;
    soc_timeout_t to;

    SOC_CONTROL_LOCK(unit);
    soc->l2x_age_interval = 0;  /* Request exit */
    SOC_CONTROL_UNLOCK(unit);

    if (soc->l2x_age_pid && (soc->l2x_age_pid != SAL_THREAD_ERROR)) {
        /* Wake up thread so it will check the exit flag */
        sal_sem_give(soc->l2x_age_notify);

        /* Give thread a few seconds to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 300 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 60 * 1000000, 0);
        }

        while (soc->l2x_age_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_L2,
                          (BSL_META_U(unit,
                                      "thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }
    return rv;
}

/**************************************************
 * @function soc_tr3_l2_entry_limit_count_update
 *          
 * 
 * @purpose Read L2 table from hardware, calculate 
 *          and restore port/trunk and vlan/vfi 
 *          mac counts. 
 * 
 * @param  unit   @b{(input)}  unit number
 *
 * @returns BCM_E_NONE
 * @returns BCM_E_FAIL
 * 
 * @comments This rountine gets called for an 
 *           L2 table SER event, if any of the
 *           mac limits(system, port, vlan) are
 *           enabled on the device.  
 * 
 * @end
 */

int 
soc_tr3_l2_entry_limit_count_update(int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    uint32 *v_entry;     /* vlan or vfi count entry */
    uint32 *p_entry;     /* port or trunk count entry */
    uint32 *l2;          /* l2 entry */
    int32 v_index = -1;  /* vlan index */
    int32 p_index = -1;  /* port index */
    uint32 s_count = 0;  /* system mac count */
    uint32 p_count = 0;  /* port or trunk mac count */
    uint32 v_count = 0;  /* vlan or vfi mac count */

    /* l2 table */     
    int index_min, index_max; 
    int entry_dw, entry_sz;
    int chnk_idx, chnk_idx_max, table_chnk_sz, idx;
    int chunk_size = _SOC_TR3_L2_TABLE_DMA_CHUNK_SIZE;

    /* port or trunk mac count table */
    int ptm_index_min, ptm_index_max; 
    int ptm_table_sz, ptm_entry_dw, ptm_entry_sz;

    /* vlan or vfi mac count table */
    int vvm_index_min, vvm_index_max; 
    int vvm_table_sz, vvm_entry_dw, vvm_entry_sz;

    uint32 *buf = NULL;
    uint32 *ptm_buf = NULL; 
    uint32 *vvm_buf = NULL;

    uint32 dest_type, key_type;
    uint32 is_valid, is_limit_counted = 0;
    uint32 is_free;
    soc_mem_t mem;
    int rv;

    p_index = -1;
    v_index = -1;


    /* If MAC learn limit not enabled do nothing */
    SOC_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    if (!soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, 
        rval, ENABLE_INTERNAL_L2_ENTRYf)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "\nMAC limits not enabled.\n"))); 
        return SOC_E_NONE;
    }


    /* 
     * Mac limits are enabled.
     * 1. Read (DMA) L2 table to recalculate mac count
     * 2. Iterate through all the entries.
     * 3. if the entry is limit_counted
     * Check if KEY_TYPE is BRIDGE, VLAN or VFI 
     * based on DEST_TYPE determine if Trunk or ModPort
     * Get port index into port_or_trunk_mac_count table
     * Get vlan/vfi index into vlan_or_vfi_mac_count table
     * update the port, vlan/vfi, system mac count.
     * Write(slam) port_or_trunk_mac count and 
     * vlan_or_vfi_mac_count tables.
     */

    mem = L2_ENTRY_1m;
    _SOC_ALL_L2X_MEM_LOCK(unit);

    /*
     * Allocate memory for port/trunk mac count table to store 
     * the updated count.
     */
    ptm_entry_dw = soc_mem_entry_words(unit, PORT_OR_TRUNK_MAC_COUNTm);
    ptm_entry_sz = ptm_entry_dw * sizeof(uint32); 
    ptm_index_min = soc_mem_index_min(unit, PORT_OR_TRUNK_MAC_COUNTm);
    ptm_index_max = soc_mem_index_max(unit,PORT_OR_TRUNK_MAC_COUNTm);
    ptm_table_sz = (ptm_index_max - ptm_index_min + 1) * ptm_entry_sz;
    
    ptm_buf = soc_cm_salloc(unit, ptm_table_sz, "ptm_tmp");

    if (ptm_buf == NULL) {
        _SOC_ALL_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_tr3_l2_entry_limit_count_update: "
                              "Memory allocation failed for port mac count\n")));
        return SOC_E_MEMORY; 
    }

    sal_memset(ptm_buf, 0, ptm_table_sz);

    /*
     * Allocate memory for vlan/vfi mac count table to store 
     * the updated count.
     */
    vvm_entry_dw = soc_mem_entry_words(unit, VLAN_OR_VFI_MAC_COUNTm);
    vvm_entry_sz = vvm_entry_dw * sizeof(uint32); 
    vvm_index_min = soc_mem_index_min(unit, VLAN_OR_VFI_MAC_COUNTm);
    vvm_index_max = soc_mem_index_max(unit,VLAN_OR_VFI_MAC_COUNTm);
    vvm_table_sz = (vvm_index_max - vvm_index_min + 1) * vvm_entry_sz;
    
    vvm_buf = soc_cm_salloc(unit, vvm_table_sz, "vvm_tmp");

    if (vvm_buf == NULL) {
        soc_cm_sfree(unit, ptm_buf);
        _SOC_ALL_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_tr3_l2_entry_limit_count_update: "
                              "Memory allocation failed for vlan mac count\n")));
        return SOC_E_MEMORY; 
    }

    sal_memset(vvm_buf, 0, vvm_table_sz);

    /* Create a copy L2_ENTRY_1m table for calculating mac count */
    entry_dw = soc_mem_entry_words(unit, mem);
    entry_sz = entry_dw * sizeof(uint32); 
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    table_chnk_sz = chunk_size * entry_sz;
    
    buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_tmp");

    if (buf == NULL) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        _SOC_ALL_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_tr3_l2_entry_limit_count_update: "
                              "Memory allocation failed for %s\n"), 
                   SOC_MEM_NAME(unit, mem)));
        return SOC_E_MEMORY; 
    }

    for (chnk_idx = index_min; chnk_idx <= index_max; chnk_idx += chunk_size) {

         sal_memset(buf, 0, table_chnk_sz);

         chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                (chnk_idx + chunk_size - 1) : index_max;

         /* DMA L2 Table */
         rv =  soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, chnk_idx, 
                                  chnk_idx_max, buf);
         if (rv < 0) {
             soc_cm_sfree(unit, buf);
             soc_cm_sfree(unit, ptm_buf);
             soc_cm_sfree(unit, vvm_buf);
             _SOC_ALL_L2X_MEM_UNLOCK(unit);
             LOG_ERROR(BSL_LS_SOC_L2,
                       (BSL_META_U(unit,
                                   "DMA failed: %s, mac limts not synced!\n"),
                                   soc_errmsg(rv)));
             return SOC_E_FAIL;
         }                          

         /* Iter through all l2 entries in the chunk */
         for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
              l2 =  (buf + (idx * entry_dw));
         
              /* check if entry is valid */
              is_valid = soc_mem_field32_get(unit, mem, l2, VALIDf); 
              if (!is_valid) {
                  /* not valid entry skip it */
                  continue;
              } 

              /* check if entry is limit counted */
              if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
                  is_limit_counted = soc_mem_field32_get(unit,mem, l2, 
                                                         LIMIT_COUNTEDf);
                  if (!is_limit_counted) {
                      /* entry not limit counted skip it */
                      continue;
                  }
              }

              /*
               * Get the key type of the entries
               * Process entries with only key_types
               * 1. SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE
               * 2. SOC_MEM_KEY_L2_ENTRY_1_L2_VFI
               */
              key_type = soc_mem_field32_get(unit, mem, l2, KEY_TYPEf); 

              if ((key_type != SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE) &&
                  (key_type != SOC_MEM_KEY_L2_ENTRY_1_L2_VFI)) {
                  continue; 
              } 

              dest_type = soc_mem_field32_get(unit, mem, l2, L2__DEST_TYPEf);

              switch (dest_type) {
                  case 0: /* mod port */
                      rv = soc_mem_read(unit, PORT_TABm, 
                                                       MEM_BLOCK_ANY, 0,
                                                       &entry);
                      if (SOC_FAILURE(rv)) {
                          soc_cm_sfree(unit, buf);
                          soc_cm_sfree(unit, ptm_buf);
                          soc_cm_sfree(unit, vvm_buf);
                          _SOC_ALL_L2X_MEM_UNLOCK(unit);
                          return rv;
                      }

	              if (soc_mem_field32_get(unit, PORT_TABm, 
                                              &entry, MY_MODIDf) ==
	                  soc_mem_field32_get(unit, mem, l2, L2__MODULE_IDf)) {
                          p_index = soc_mem_field32_get(unit, mem, 
                                                        l2, PORT_NUMf) + 
	            	  soc_mem_index_count(unit, TRUNK_GROUPm) ;
	              }
                      break;
                  case 1: /* trunk */
                      p_index = soc_mem_field32_get(unit, mem, l2, L2__TGIDf);
                      break;
                  default:
                      /* if VFI based key then break else continue? */
                      break;
              }

              /*
               * based on key_type get vlan or vfi
               * to index into VLAN_OR_VFI_MAC_COUNTm
               */
              if (key_type == SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE) {
                  v_index = soc_mem_field32_get(unit, mem, l2, L2__VLAN_IDf);
              } else {
                  v_index = soc_mem_field32_get(unit, mem, l2, L2__VFIf) +
                               soc_mem_index_count(unit, VLAN_TABm);
              } 
  

             /* 
              * read and update vlan or vfi mac count
              * in buffer also update the sys mac count.
              */
             v_count = 0;
             if (v_index >= 0) {
                 v_entry =  (vvm_buf + (v_index * vvm_entry_dw)); 
                 v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                          v_entry, COUNTf);
                 s_count++;
                 v_count++;
                 soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm, 
                                     v_entry, COUNTf, v_count);
             }
    
             /* 
              * read and update port or trunk mac count
              * in buffer.
              */
             p_count = 0;
             if (p_index >= 0) {
                 p_entry = ptm_buf + (p_index * ptm_entry_dw); 
                 p_count = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                               p_entry, COUNTf);
                 p_count++;
                 soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                     p_entry, COUNTf, p_count);
             }
    
         } /* End for index */
    } /* End for chnk_idx */

    /* Free memory */
    soc_cm_sfree(unit, buf);

    /* Now create a copy L2_ENTRY_2m table for calculating mac count */
    mem = L2_ENTRY_2m;
    entry_dw = soc_mem_entry_words(unit, mem);
    entry_sz = entry_dw * sizeof(uint32); 
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    table_chnk_sz = chunk_size * entry_sz;
    
    buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_tmp");

    if (buf == NULL) {
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        _SOC_ALL_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "soc_tr3_l2_entry_limit_count_update: "
                              "Memory allocation failed for %s\n"), 
                   SOC_MEM_NAME(unit, mem)));
        return SOC_E_MEMORY; 
    }

    for (chnk_idx = index_min; chnk_idx <= index_max; chnk_idx += chunk_size) {

         sal_memset(buf, 0, table_chnk_sz);

         chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                (chnk_idx + chunk_size - 1) : index_max;

         /* DMA L2 Table */
         rv =  soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, chnk_idx, 
                                  chnk_idx_max, buf);
         if (rv < 0) {
             soc_cm_sfree(unit, buf);
             soc_cm_sfree(unit, ptm_buf);
             soc_cm_sfree(unit, vvm_buf);
             _SOC_ALL_L2X_MEM_UNLOCK(unit);
             LOG_ERROR(BSL_LS_SOC_L2,
                       (BSL_META_U(unit,
                                   "DMA failed: %s, mac limts not synced!\n"),
                                   soc_errmsg(rv)));
             return SOC_E_FAIL;
         }                          

         /* Iter through all l2 entries in the chunk */
         for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
              l2 =  (buf + (idx * entry_dw));
         
              /* check if entry is valid */
              if (!soc_mem_field32_get(unit, mem, l2, VALID_0f)) {
                  /* not valid entry skip it */
                  continue;
              } 

              /* check if entry is limit counted */
              if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
                  is_limit_counted = soc_mem_field32_get(unit,mem, l2, 
                                                         LIMIT_COUNTEDf);
                  if (!is_limit_counted) {
                      /* entry not limit counted skip it */
                      continue;
                  }
              }

              /*
               * Get the key type of the entries
               * Process entries with only key_types
               * 1. SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE
               * 2. SOC_MEM_KEY_L2_ENTRY_2_L2_VFI
               */
              key_type = soc_mem_field32_get(unit, L2_ENTRY_1m, l2, KEY_TYPEf);

              if ((key_type != SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) &&
                  (key_type != SOC_MEM_KEY_L2_ENTRY_2_L2_VFI)) {
                  continue; 
              } 

              dest_type = soc_mem_field32_get(unit, mem, l2, L2__DEST_TYPEf);

              switch (dest_type) {
                  case 0: /* mod port */
                      rv = soc_mem_read(unit, PORT_TABm, 
                                                       MEM_BLOCK_ANY, 0,
                                                       &entry);
                      if (SOC_FAILURE(rv)) {
                          soc_cm_sfree(unit, buf);
                          soc_cm_sfree(unit, ptm_buf);
                          soc_cm_sfree(unit, vvm_buf);
                          _SOC_ALL_L2X_MEM_UNLOCK(unit);
                          return rv;
                      }
	              if (soc_mem_field32_get(unit, PORT_TABm, 
                                              &entry, MY_MODIDf) ==
	                  soc_mem_field32_get(unit, mem, l2,  L2__MODULE_IDf)) {
                          p_index = soc_mem_field32_get(unit, mem, 
                                                        l2, L2__PORT_NUMf) + 
	            	            soc_mem_index_count(unit, TRUNK_GROUPm) ;
	              }
                      break;
                  case 1: /* trunk */
                      p_index = soc_mem_field32_get(unit, mem, l2, L2__TGIDf);
                      break;
                  default:
                      /* if VFI based key then break else continue? */
                      break;
              }

              /*
               * based on key_type get vlan or vfi
               * to index into VLAN_OR_VFI_MAC_COUNTm
               */
              if (key_type == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) {
                  v_index = soc_mem_field32_get(unit, mem, l2, L2__VLAN_IDf);
              } else {
                  v_index = soc_mem_field32_get(unit, mem, l2, L2__VFIf) +
                               soc_mem_index_count(unit, VLAN_TABm);
              } 
  

             /* 
              * read and update vlan or vfi mac count
              * in buffer also update the sys mac count.
              */
             v_count = 0;
             if (v_index >= 0) {
                 v_entry =  (vvm_buf + (v_index * vvm_entry_dw)); 
                 v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                          v_entry, COUNTf);
                 s_count++;
                 v_count++;
                 soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm, 
                                     v_entry, COUNTf, v_count);
             }
    
             /* 
              * read and update port or trunk mac count
              * in buffer.
              */
             p_count = 0;
             if (p_index >= 0) {
                 p_entry = ptm_buf + (p_index * ptm_entry_dw); 
                 p_count = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                               p_entry, COUNTf);
                 p_count++;
                 soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                     p_entry, COUNTf, p_count);
             }
    
         } /* End for index */
    } /* End for chnk_idx */

    soc_cm_sfree(unit, buf);

    /* Work on EXT_L2_TABLE */
    if (soc_feature(unit, soc_feature_esm_support)) {
        /* Create a copy EXT_L2_ENTRY_1m table for calculating mac count */
        mem = EXT_L2_ENTRY_1m;   
        entry_dw = soc_mem_entry_words(unit, mem);
        entry_sz = entry_dw * sizeof(uint32); 
        index_min = soc_mem_index_min(unit, mem);
        index_max = soc_mem_index_max(unit, mem);
        table_chnk_sz = chunk_size * entry_sz;
    
        buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_tmp");

        if (buf == NULL) {
            soc_cm_sfree(unit, ptm_buf);
            soc_cm_sfree(unit, vvm_buf);
            _SOC_ALL_L2X_MEM_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "soc_tr3_l2_entry_limit_count_update: "
                                  "Memory allocation failed for %s\n"), 
                       SOC_MEM_NAME(unit, mem)));
            return SOC_E_MEMORY; 
        }

        for (chnk_idx = index_min; chnk_idx <= index_max; 
                                   chnk_idx += chunk_size) {

             sal_memset(buf, 0, table_chnk_sz);

             chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                    (chnk_idx + chunk_size - 1) : index_max;

             /* DMA L2 Table */
             rv =  soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, chnk_idx, 
                                      chnk_idx_max, buf);
             if (rv < 0) {
                 soc_cm_sfree(unit, buf);
                 soc_cm_sfree(unit, ptm_buf);
                 soc_cm_sfree(unit, vvm_buf);
                 _SOC_ALL_L2X_MEM_UNLOCK(unit);
                 LOG_ERROR(BSL_LS_SOC_L2,
                           (BSL_META_U(unit,
                                       "DMA failed: %s, mac limts not synced!\n"),
                                       soc_errmsg(rv)));
                 return SOC_E_FAIL;
             }                          

             /* Iter through all l2 entries in the chunk */
             for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
                  l2 =  (buf + (idx * entry_dw));
         
                  /* check if entry is valid */
                  is_free = soc_mem_field32_get(unit, mem, l2, FREEf); 
                  if (is_free) {
                      /* not valid entry skip it */
                      continue;
                  } 

                  /* check if entry is limit counted */
                  if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
                      is_limit_counted = soc_mem_field32_get(unit,mem, l2, 
                                                             LIMIT_COUNTEDf);
                      if (!is_limit_counted) {
                          /* entry not limit counted skip it */
                          continue;
                      }
                  }
    
                  /*
                   * Get the key type of the entries
                   * Process entries with only key_types
                   * 1. SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE
                   * 2. SOC_MEM_KEY_L2_ENTRY_1_L2_VFI
                   */
                  key_type = soc_mem_field32_get(unit, mem, l2, KEY_TYPEf); 

                  if ((key_type != SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE) &&
                      (key_type != SOC_MEM_KEY_L2_ENTRY_1_L2_VFI)) {
                      continue; 
                  } 

                  dest_type = soc_mem_field32_get(unit, mem, l2, DEST_TYPEf);

                  switch (dest_type) {
                      case 0: /* mod port */
                          rv = soc_mem_read(unit, PORT_TABm, 
                                                           MEM_BLOCK_ANY, 0,
                                                           &entry);
                          if (SOC_FAILURE(rv)) {
                              soc_cm_sfree(unit, buf);
                              soc_cm_sfree(unit, ptm_buf);
                              soc_cm_sfree(unit, vvm_buf);
                              _SOC_ALL_L2X_MEM_UNLOCK(unit);
                              return rv;
                          }
	                  if (soc_mem_field32_get(unit, PORT_TABm, 
                                                  &entry, MY_MODIDf) ==
	                      soc_mem_field32_get(unit, mem, l2, MODULE_IDf)) {
                              p_index = soc_mem_field32_get(unit, mem, 
                                                            l2, PORT_NUMf) + 
	            	                soc_mem_index_count(unit, TRUNK_GROUPm) ;
	                  }
                          break;
                      case 1: /* trunk */
                          p_index = soc_mem_field32_get(unit, mem, 
                                                        l2, TGIDf);
                          break;
                      default:
                          /* if VFI based key then break else continue? */
                          break;
                  }
    
                  /*
                   * based on key_type get vlan or vfi
                   * to index into VLAN_OR_VFI_MAC_COUNTm
                   */
                  if (key_type == SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE) {
                      v_index = soc_mem_field32_get(unit, mem, l2, VLAN_IDf);
                  } else {
                      v_index = soc_mem_field32_get(unit, mem, l2, VFIf) +
                                   soc_mem_index_count(unit, VLAN_TABm);
                  } 
  

                 /* 
                  * read and update vlan or vfi mac count
                  * in buffer also update the sys mac count.
                  */
                 v_count = 0;
                 if (v_index >= 0) {
                     v_entry =  (vvm_buf + (v_index * vvm_entry_dw)); 
                     v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                              v_entry, COUNTf);
                     s_count++;
                     v_count++;
                     soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm, 
                                         v_entry, COUNTf, v_count);
                 }
    
                 /* 
                  * read and update port or trunk mac count
                  * in buffer.
                  */
                 p_count = 0;
                 if (p_index >= 0) {
                     p_entry = ptm_buf + (p_index * ptm_entry_dw); 
                     p_count = soc_mem_field32_get(unit, 
                                                   PORT_OR_TRUNK_MAC_COUNTm,
                                                   p_entry, COUNTf);
                     p_count++;
                     soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                         p_entry, COUNTf, p_count);
                 }
    
             } /* End for index */
        } /* End for chnk_idx */
    
        /* Free memory */
        soc_cm_sfree(unit, buf);

        /* Create a copy EXT_L2_ENTRY_2m table for calculating mac count */
        mem = EXT_L2_ENTRY_2m;   
        entry_dw = soc_mem_entry_words(unit, mem);
        entry_sz = entry_dw * sizeof(uint32); 
        index_min = soc_mem_index_min(unit, mem);
        index_max = soc_mem_index_max(unit, mem);
        table_chnk_sz = chunk_size * entry_sz;
    
        buf = soc_cm_salloc(unit, table_chnk_sz, "l2x_tmp");

        if (buf == NULL) {
            soc_cm_sfree(unit, ptm_buf);
            soc_cm_sfree(unit, vvm_buf);
            _SOC_ALL_L2X_MEM_UNLOCK(unit);
            LOG_ERROR(BSL_LS_SOC_L2,
                      (BSL_META_U(unit,
                                  "soc_tr3_l2_entry_limit_count_update: "
                                  "Memory allocation failed for %s\n"), 
                       SOC_MEM_NAME(unit, mem)));
            return SOC_E_MEMORY; 
        }

        for (chnk_idx = index_min; chnk_idx <= index_max; chnk_idx += chunk_size) {

             sal_memset(buf, 0, table_chnk_sz);

             chnk_idx_max = ((chnk_idx + chunk_size) <= index_max) ?
                    (chnk_idx + chunk_size - 1) : index_max;

             /* DMA L2 Table */
             rv =  soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, chnk_idx, 
                                      chnk_idx_max, buf);
             if (rv < 0) {
                 soc_cm_sfree(unit, buf);
                 soc_cm_sfree(unit, ptm_buf);
                 soc_cm_sfree(unit, vvm_buf);
                 _SOC_ALL_L2X_MEM_UNLOCK(unit);
                 LOG_ERROR(BSL_LS_SOC_L2,
                           (BSL_META_U(unit,
                                       "DMA failed: %s, mac limts not synced!\n"),
                                       soc_errmsg(rv)));
                 return SOC_E_FAIL;
             }                          

             /* Iter through all l2 entries in the chunk */
             for (idx = 0; idx <= (chnk_idx_max - chnk_idx); idx++) {
                  l2 =  (buf + (idx * entry_dw));
         
                  /* check if entry is valid */
                  is_free = soc_mem_field32_get(unit, mem, l2, FREEf); 
                  if (is_free) {
                      /* not valid entry skip it */
                      continue;
                  } 

                  /* check if entry is limit counted */
                  if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
                      is_limit_counted = soc_mem_field32_get(unit,mem, l2, 
                                                             LIMIT_COUNTEDf);
                      if (!is_limit_counted) {
                          /* entry not limit counted skip it */
                          continue;
                      }
                  }
    
                  /*
                   * Get the key type of the entries
                   * Process entries with only key_types
                   * 1. SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE
                   * 2. SOC_MEM_KEY_L2_ENTRY_2_L2_VFI
                   */
                  key_type = soc_mem_field32_get(unit, mem, l2, KEY_TYPEf); 

                  if ((key_type != SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) &&
                      (key_type != SOC_MEM_KEY_L2_ENTRY_2_L2_VFI)) {
                      continue; 
                  } 

                  dest_type = soc_mem_field32_get(unit, mem, l2, DEST_TYPEf);

                  switch (dest_type) {
                      case 0: /* mod port */
                          rv = soc_mem_read(unit, PORT_TABm, 
                                                           MEM_BLOCK_ANY, 0,
                                                           &entry);
                          if (SOC_FAILURE(rv)) {
                              soc_cm_sfree(unit, buf);
                              soc_cm_sfree(unit, ptm_buf);
                              soc_cm_sfree(unit, vvm_buf);
                              _SOC_ALL_L2X_MEM_UNLOCK(unit);
                              return rv;
                          }
	                  if (soc_mem_field32_get(unit, PORT_TABm, 
                                                  &entry, MY_MODIDf) ==
	                      soc_mem_field32_get(unit, mem, l2, MODULE_IDf)) {
                              p_index = soc_mem_field32_get(unit, mem, 
                                                            l2, PORT_NUMf) + 
	            	                soc_mem_index_count(unit, TRUNK_GROUPm) ;
	                  }
                          break;
                      case 1: /* trunk */
                          p_index = soc_mem_field32_get(unit, mem, 
                                                        l2, TGIDf);
                          break;
                      default:
                          /* if VFI based key then break else continue? */
                          break;
                  }
    
                  /*
                   * based on key_type get vlan or vfi
                   * to index into VLAN_OR_VFI_MAC_COUNTm
                   */
                  if (key_type == SOC_MEM_KEY_L2_ENTRY_2_L2_BRIDGE) {
                      v_index = soc_mem_field32_get(unit, mem, l2, VLAN_IDf);
                  } else {
                      v_index = soc_mem_field32_get(unit, mem, l2, VFIf) +
                                   soc_mem_index_count(unit, VLAN_TABm);
                  } 
  

                 /* 
                  * read and update vlan or vfi mac count
                  * in buffer also update the sys mac count.
                  */
                 v_count = 0;
                 if (v_index >= 0) {
                     v_entry =  (vvm_buf + (v_index * vvm_entry_dw)); 
                     v_count = soc_mem_field32_get(unit, VLAN_OR_VFI_MAC_COUNTm,
                                              v_entry, COUNTf);
                     s_count++;
                     v_count++;
                     soc_mem_field32_set(unit, VLAN_OR_VFI_MAC_COUNTm, 
                                         v_entry, COUNTf, v_count);
                 }
    
                 /* 
                  * read and update port or trunk mac count
                  * in buffer.
                  */
                 p_count = 0;
                 if (p_index >= 0) {
                     p_entry = ptm_buf + (p_index * ptm_entry_dw); 
                     p_count = soc_mem_field32_get(unit, 
                                                   PORT_OR_TRUNK_MAC_COUNTm,
                                                   p_entry, COUNTf);
                     p_count++;
                     soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm,
                                         p_entry, COUNTf, p_count);
                 }
    
             } /* End for index */
        } /* End for chnk_idx */
    } /* End if soc_feature_esm_support */

    /* Write the tables to hardware */
     /* coverity[stack_use_overflow] */
     rv = soc_mem_write_range(unit, PORT_OR_TRUNK_MAC_COUNTm, MEM_BLOCK_ANY, 
                              ptm_index_min, ptm_index_max, (void *)ptm_buf);

    if (rv < 0) {
        soc_cm_sfree(unit, buf);
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        _SOC_ALL_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "PORT_OR_TRUNK_MAC_COUNT write failed: "
                              "%s, mac limts not synced!\n"),
                   soc_errmsg(rv)));
        return SOC_E_FAIL;
    }

    rv = soc_mem_write_range(unit, VLAN_OR_VFI_MAC_COUNTm, MEM_BLOCK_ANY, 
                             vvm_index_min, vvm_index_max, (void *)vvm_buf);

    if (rv < 0) {
        soc_cm_sfree(unit, buf);
        soc_cm_sfree(unit, ptm_buf);
        soc_cm_sfree(unit, vvm_buf);
        _SOC_ALL_L2X_MEM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                              "VLAN_OR_VFI_MAC_COUNT write failed: "
                              "%s, mac limts not synced!\n"),
                   soc_errmsg(rv)));
        return SOC_E_FAIL;
    }
     
    /* Update system count */
    soc_reg_field_set(unit, SYS_MAC_COUNTr, &rval, COUNTf, s_count);
    rv = WRITE_SYS_MAC_COUNTr(unit, rval);
    
    /* Free memory */
    soc_cm_sfree(unit, buf);
    soc_cm_sfree(unit, ptm_buf);
    soc_cm_sfree(unit, vvm_buf);

    _SOC_ALL_L2X_MEM_UNLOCK(unit);
    return rv;
}

#endif /* BCM_XGS_SWITCH_SUPPORT */
