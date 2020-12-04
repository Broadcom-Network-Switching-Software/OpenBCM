/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *  sat.c
 *
 * Purpose:
 *  SAT implementation for SABER2 family of devices.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>
#include <soc/feature.h>
#include <bcm_int/esw_dispatch.h>
#include <shared/idxres_fl.h>
#include <shared/idxres_afl.h>
#include <shared/hash_tbl.h>
#include <bcm_int/esw/trunk.h>

#include <bcm/sat.h>
#include <bcm_int/esw/sat.h>

#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#include <bcm_int/esw/saber2.h>

#include <bcm_int/esw/katana2.h>


/* * * * * * * * * * * * * * * * * * *
 *            SAT MACROS             *
 */
/*
 * Macro:
 *     _BCM_SAT_IS_INIT (internal)
 * Purpose:
 *     Check that the unit is valid and confirm that the sat functions
 *     are initialized.
 * Parameters:
 *     unit - BCM device number
 * Notes:
 *     Results in return(BCM_E_UNIT), return(BCM_E_UNAVAIL), or
 *     return(BCM_E_INIT) if fails.
 */
#define _BCM_SAT_IS_INIT(unit)                                               \
            do {                                                             \
                if (!soc_feature(unit, soc_feature_sat)) {                   \
                    return (BCM_E_UNAVAIL);                                  \
                }                                                            \
                if (_sb2_sat_control[unit] == NULL) {                        \
                    LOG_ERROR(BSL_LS_BCM_SAT,                                \
                    (BSL_META_U(unit,                                        \
                    "SAT(unit %d) Error: Module not initialized\n"),         \
                            unit));                                          \
                    return (BCM_E_INIT);                                     \
                }                                                            \
            } while (0)

/*
 *Macro:
 *     _BCM_SAT_LOCK
 * Purpose:
 *     Acquire the SAT control mutex Lock
 * Parameters:
 *     control - Pointer to SAT control structure.
 */
#define _BCM_SAT_LOCK(control) \
            sal_mutex_take((control)->sat_lock, sal_mutex_FOREVER)

/*
 * Macro:
 *     _BCM_SAT_UNLOCK
 * Purpose:
 *     Release the SAT control mutex Lock
 * Parameters:
 *     control - Pointer to SAT control structure.
 */
#define _BCM_SAT_UNLOCK(control) \
            sal_mutex_give((control)->sat_lock);
/*
 * Macro:
 *     _BCM_SAT_HASH_DATA_CLEAR
 * Purpose:
 *      Clear hash data memory occupied by one endpoint.
 * Parameters:
 *     _ptr_    - Pointer to endpoint hash data memory. 
 */
#define _BCM_SAT_HASH_DATA_CLEAR(_ptr_) \
            sal_memset(_ptr_, 0, sizeof(_bcm_sat_hash_data_t));


/*
 * Macro:
 *     _BCM_SAT_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_SAT_ALLOC(_ptr_,_ptype_,_size_,_descr_)                     \
            do {                                                         \
                if (NULL == (_ptr_)) {                                   \
                   (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                }                                                        \
                if((_ptr_) != NULL) {                                    \
                    sal_memset((_ptr_), 0, (_size_));                    \
                }  else {                                                \
                    LOG_ERROR(BSL_LS_BCM_SAT,                            \
                        (BSL_META_U(unit,                                \
                              "SAT Error: Allocation failure %s\n"),     \
                            (_descr_)));                                 \
                }                                                        \
            } while (0)


/*
 * Macro:
 *     _BCM_SAT_MEP_INDEX_VALIDATE
 * Purpose:
 *     Validate UPMEP/DOWNMEP SAT Endpoint ID value.
 * Parameters:
 *     _ep_ - Endpoint ID value.
 */
#define _BCM_SAT_MEP_INDEX_VALIDATE(mep_count,_ep_)                       \
            do {                                                          \
                if ((_ep_) < 0 || (_ep_) >= mep_count) {                  \
                    LOG_ERROR(BSL_LS_BCM_SAT,                             \
                             (BSL_META_U(unit,                            \
                             "SAT(unit %d) Error: Invalid  MEP Index"     \
                             " = %d.\n"), unit, _ep_));                   \
                    return (BCM_E_PARAM);                                 \
                }                                                         \
            } while (0);


/*
 * Macro:
 *     _BCM_SB2_SAT_MOD_PORT_TO_GLP
 * Purpose:
 *     Construct hadware GLP value from module ID, port ID and Trunk ID value.
 * Parameters:
 *     _modid_ - Module ID.
 *     _port_  - Port ID.
 *     _trunk_ - Trunk (1 - TRUE/ 0- FALSE).
 *     _tgid_  - Trunk ID.
 */
#define _BCM_SB2_SAT_MOD_PORT_TO_GLP(_u_, _m_, _p_, _t_, _tgid_, _glp_)     \
    do {                                                                    \
        if ((_tgid_) != -1) {                                               \
            (_glp_) = (((0x1 & (_t_)) << SOC_TRUNK_BIT_POS(_u_))            \
                | ((soc_mem_index_count((_u_), TRUNK_GROUPm) - 1)           \
                & (_tgid_)));                                               \
        } else {                                                            \
            (_glp_) = (((0x1 & (_t_)) << SOC_TRUNK_BIT_POS(_u_))            \
                | ((SOC_MODID_MAX(_u_) & (_m_))                             \
                << (_shr_popcount((unsigned int) SOC_PORT_ADDR_MAX(_u_)))   \
                | (SOC_PORT_ADDR_MAX(_u_) & (_p_))));                       \
        }                                                                   \
        LOG_DEBUG(BSL_LS_BCM_SAT,                                           \
                  (BSL_META_U(unit,                                         \
                     "u:%d m:%d p:%d t:%d tgid:%d glp:%x\n"),               \
                     _u_, _m_, _p_, _t_, _tgid_, _glp_));                   \
    } while (0)



/*
 * Macro:
 *     _BCM_SAT_KEY_PACK
 * Purpose:
 *     Pack the hash table look up key fields.
 * Parameters:
 *     _dest_ - Hash key buffer.
 *     _src_  - Hash key field to be packed.
 *     _size_ - Hash key field size in bytes.
 */
#define _BCM_SAT_KEY_PACK(_dest_,_src_,_size_)            \
            do {                                          \
                sal_memcpy((_dest_), (_src_), (_size_));  \
                (_dest_) += (_size_);                     \
            } while (0)

/*
 * Macro:
 *     _BCM_SAT_CPU_OLP_HDR_SET
 * Purpose:
 *     Enable/Disable adding OLP header to SAT packet forwarded to CPU port
 * Parameters:
 *     unit - BCM device number
 *     olp_hdr - Enable/Disable OLP header for SAT packet forwarded to CPU port
 * Notes:
 */
#define _BCM_SAT_CPU_OLP_HDR_SET(unit, olp_hdr) \
      do {                                          \
          SOC_IF_ERROR_RETURN(WRITE_EGR_OAM_RX_MODE_FOR_CPUr(unit,olp_hdr)); \
      } while (0)

/*
 * Macro:
 *     _BCM_SAT_GLP_XXX
 * Purpose:
 *     Get components of generic logical port value.
 * Parameters:
 *     _glp_ - Generic logical port.
 */
#define _BCM_SAT_GLP_TRUNK_BIT_GET(_u_, _glp_)        \
        (0x1 & ((_glp_) >> SOC_TRUNK_BIT_POS(_u_)))
#define _BCM_SAT_GLP_TRUNK_ID_GET(_u_, _glp_)         \
        ((soc_mem_index_count((_u_), TRUNK_GROUPm)    \
                                   - 1) & (_glp_))
#define _BCM_SAT_GLP_MODULE_ID_GET(_u_, _glp_)        \
        (SOC_MODID_MAX(_u_) & ((_glp_) >>             \
        (_shr_popcount((unsigned int)                 \
        SOC_PORT_ADDR_MAX(_u_)))))
#define _BCM_SAT_GLP_PORT_GET(_u_, _glp_)             \
        (SOC_PORT_ADDR_MAX(_u_) & (_glp_))

/*
 * Macro: _SAT_HASH_KEY_SIZE
 *
 * Purpose:
 *      SAT endpoint hash table key size.
 */

/* Outer VLAN ID + Inner VLAN ID + MAC DA + MAC SA + Gport + Ether type */
#define _SAT_HASH_KEY_SIZE (2 + 2 + 6 + 6 + 4 + 2 )

/* Define the port used in OLP-XGS communication */
#define _BCM_SAT_OLP_COMMUNICATION_PORT  0x7f

/* Define the VLAN ID mask */
#define _BCM_SAT_VLAN_MASK          0xfff

/* Define the priority mask  */
#define _BCM_SAT_VLAN_PRIO_MASK     0x07

/* Define the priority bit field location  */
#define _BCM_SAT_VLAN_PRI_BIT       13

/* Define the CFi bit field location  */
#define _BCM_SAT_VLAN_CFI_BIT       12

/* Define generic enable field */
#define _BCM_SAT_ENABLE             0x1

/* Define generic disable field */
#define _BCM_SAT_DISABLE            0x0

#define _BCM_SAT_SRC_GPORT          0x0001
#define _BCM_SAT_DEST_GPORT         0x0002

/* SAT Entry TCAM VALID bit to be set to 3 for valid entry */
#define BCM_SAT_TCAM_ENTRY_VALID    3

/*
 * Macro:
 *     _BCM_SAT_VLAN_PRI_SET
 * Purpose:
 *     Set priority to VLAN tag 
 * Parameters:
 *     vlan - VLAN tag
 *     pri  - prioirty 
 *     mask - Priority field mask bits 
 * Notes:
 */
#define _BCM_SAT_VLAN_PRI_SET(vlan, pri, mask) \
                (vlan |= ((pri & mask ) << _BCM_SAT_VLAN_PRI_BIT))

/*
 * Macro:
 *     _BCM_SAT_VLAN_PRI_GET
 * Purpose:
 *     Get priority from VLAN tag 
 * Parameters:
 *     vlan - VLAN tag
 *     mask - Priority field mask bits 
 * Notes:
 */
#define _BCM_SAT_VLAN_PRI_GET(vlan, mask) \
            ((vlan & (mask << _BCM_SAT_VLAN_PRI_BIT)) >> _BCM_SAT_VLAN_PRI_BIT)

/*
 * Macro:
 *     _BCM_SAT_VLAN_CFI_SET
 * Purpose:
 *     Set CFI field to VLAN tag 
 * Parameters:
 *     vlan - VLAN tag
 *     cfi  - CFI field 
 *     mask - CFI field mask bits 
 * Notes:
 */
#define _BCM_SAT_VLAN_CFI_SET(vlan, cfi, mask) \
                (vlan |= ((cfi & mask ) << _BCM_SAT_VLAN_CFI_BIT))

/*
 * Macro:
 *     _BCM_SAT_VLAN_CFI_GET
 * Purpose:
 *     Get CFI field from VLAN tag 
 * Parameters:
 *     vlan - VLAN tag
 *     mask - CFI field mask bits 
 * Notes:
 */
#define _BCM_SAT_VLAN_CFI_GET(vlan, mask) \
            ((vlan & (mask << _BCM_SAT_VLAN_CFI_BIT)) >> _BCM_SAT_VLAN_CFI_BIT)

typedef uint8 _bcm_sat_hash_key_t[_SAT_HASH_KEY_SIZE];
			
static _bcm_sat_control_t *_sb2_sat_control[BCM_MAX_NUM_UNITS];

STATIC int
_bcm_sb2_sat_control_get(int unit, _bcm_sat_control_t **satc);

STATIC void
_bcm_sb2_sat_ep_hash_key_construct(int unit,
                                   _bcm_sat_control_t *satc,
                                   bcm_sat_endpoint_info_t *ep_info,
                                   _bcm_sat_hash_key_t *key);

void
_bcm_sb2_sat_endpoint_cleanup(int unit, int upmep, 
                              _bcm_sat_hash_key_t  hash_key,
                              _bcm_sat_hash_data_t *hash_data);

STATIC int
_bcm_sb2_sat_endpoint_destroy_all(int unit, uint32  flags);

STATIC int
_bcm_sb2_sat_mep_hw_set(int unit, bcm_sat_endpoint_info_t *ep_info_p,
                        uint16 sglp, uint16 dglp);

STATIC int
_bcm_sb2_sat_endpoint_hw_delete(int unit, _bcm_sat_hash_data_t *h_data_p);

#if defined(BCM_WARM_BOOT_SUPPORT)
STATIC int
_bcm_sb2_sat_wb_upmep_endpoints_recover(int unit);

STATIC int
_bcm_sb2_sat_wb_downmep_endpoints_recover(int unit);
#endif

/*
 * Function:
 *     _bcm_sb2_sat_control_free
 * Purpose:
 *     Free SAT control structure resources allocated by this unit.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     satc -  (IN) Pointer to SAT control structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_control_free(int unit, _bcm_sat_control_t *satc)
{
    int status = 0;
    _sb2_sat_control[unit] = NULL;

    if (NULL == satc) {
        /* Module already un-initialized. */
        return (BCM_E_NONE);
    }

    /* Free protection mutex. */
    if (NULL != satc->sat_lock) {
        sal_mutex_destroy(satc->sat_lock);
    }

    /* Free UP-MEP hash data storage memory. */
    if (NULL != satc->upsamp_hash_data) {
        sal_free(satc->upsamp_hash_data);
    }

    /* Destory UP-MEP endpoint hash table. */
    if (NULL != satc->upsamp_htbl) {
        status = shr_htb_destroy(&satc->upsamp_htbl, NULL);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit, "Freeing UP-MEP samp_htbl failed\n")));
        }
    }

    /* Free Down-MEP hash data storage memory. */
    if (NULL != satc->downsamp_hash_data) {
        sal_free(satc->downsamp_hash_data);
    }

    /* Destory Down-MEP endpoint hash table. */
    if (NULL != satc->downsamp_htbl) {
        status = shr_htb_destroy(&satc->downsamp_htbl, NULL);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                                  "Freeing Down-MEP samp_htbl failed\n")));
        }
    }

    /* Destroy UP MEP endpoint indices list. */
    if (NULL != satc->upsamp_mep_pool) {
        shr_idxres_list_destroy(satc->upsamp_mep_pool);
        satc->upsamp_mep_pool = NULL;
    }

    /* Destroy DOWN MEP endpoint indices list. */
    if (NULL != satc->downsamp_mep_pool) {
        shr_idxres_list_destroy(satc->downsamp_mep_pool);
        satc->downsamp_mep_pool = NULL;
    }

    /* Free SAT control structure memory. */
    sal_free(satc);

    return (BCM_E_NONE);
}


typedef struct _bcm_sat_olp_hdr_type_map_s {
   uint32  mem_index;
   uint32  hdr_type;
   uint32  hdr_subtype; 
} _bcm_sat_olp_hdr_type_map_t;

/*
 * Function:
 *     _bcm_sb2_sat_olp_header_type_mapping_set
 * Purpose:
 *    Set default olp header type mapping 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 * MEP_TYPE      COMPRESSED_HDR_TYPE       HDR_TYPE          HDR_SUBTYPE
 * ==========================================================================
 *  Downmep/IFP      3'b000                8'd1(L2_HDR)      8'd0(NULL)
 *  Downmep/IFP      3'b001                8'd0(OAM_HDR)     8'd2(CCM/BHH-CCM)
 *  Downmep/IFP      3'b010                8'd0(OAM_HDR)     8'd3(BFD)
 *  Downmep/IFP      3'b011                8'd0(OAM_HDR)     8'd4(LM/DM)
 *  Downmep/IFP      5'b01111              8'd1(L2_HDR)      8'd15(SAT)
 *  Upmep            3'b000                N/A               N/A
 *  Upmep            3'b001                8'd0(OAM_HDR)     8'd5(CCM)
 *  Upmep            3'b010                N/A               N/A
 *  Upmep            3'b011                8'd0(OAM_HDR)     8'd6(LM/DM)
 *  Upmep            5'b01111              8'd1(L2_HDR)      8'd21(SAT)
 */
STATIC int
_bcm_sb2_sat_olp_header_type_mapping_set(int unit) 
{
    int rv = BCM_E_NONE;
    int index = 0;
    int entry_mem_size;    /* Size of table entry. */
    int entry_count = 0;

    egr_olp_header_type_mapping_entry_t *entry_buf;
    egr_olp_header_type_mapping_entry_t *entry;
    _bcm_sat_olp_hdr_type_map_t olp_hdr_type;

    _bcm_sat_olp_hdr_type_map_t olp_hdr_type_map[] = {
        /* SAT DownMEP index */
        { BCM_SAT_DOWNMEP_OLP_HDR_COMPRESSED,  1, 0xf},
        /* SAT UpMEP index (5th bit set to 1 for upmep) */
        { (BCM_SAT_UPMEP_OLP_HDR_COMPRESSED | 0x20), 1, 0x15},
        { 0xff }
    };

    entry_count = soc_mem_index_count(unit, EGR_OLP_HEADER_TYPE_MAPPINGm);
    entry_mem_size = sizeof(egr_olp_header_type_mapping_entry_t);

    /* Allocate buffer to store the DMAed table entries. */
    entry_buf = soc_cm_salloc(unit, entry_mem_size * entry_count,
                              "olp header type mapping buffer");
    if (NULL == entry_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(entry_buf, 0, sizeof(entry_mem_size) * entry_count);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, MEM_BLOCK_ALL,
                            0, (entry_count-1), entry_buf);
    if (BCM_FAILURE(rv)) {
        if (entry_buf) {
            soc_cm_sfree(unit, entry_buf);
        }
        return rv;
    }

    for (index = 0; ;index++) { 
        olp_hdr_type = olp_hdr_type_map[index];
        if(olp_hdr_type.mem_index == 0xff) {
            /* End of table */
            break;
        }
        if (olp_hdr_type.mem_index >= entry_count) {
            soc_cm_sfree(unit, entry_buf);
            return BCM_E_INTERNAL;
        }
        entry = soc_mem_table_idx_to_pointer (unit, 
                                         EGR_OLP_HEADER_TYPE_MAPPINGm, 
                                         egr_olp_header_type_mapping_entry_t *,
                                         entry_buf, olp_hdr_type.mem_index);
        soc_mem_field_set(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, 
                          (uint32 *)entry, HDR_TYPEf, 
                          &(olp_hdr_type.hdr_type));
        soc_mem_field_set(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, 
                          (uint32 *)entry, HDR_SUBTYPEf, 
                          &(olp_hdr_type.hdr_subtype));
    } 
    rv = soc_mem_write_range(unit, EGR_OLP_HEADER_TYPE_MAPPINGm, 
                            MEM_BLOCK_ALL, 0, (entry_count - 1), entry_buf); 
    soc_cm_sfree(unit, entry_buf);
    return rv;
}


/*
 * Function:
 *     _bcm_sb2_sat_olp_magic_port_set
 * Purpose:
 *     Set Magic port used in OLP-XGS communication 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_olp_magic_port_set(int unit)
{
    uint64 rval, set_val;
    int    modid;

    COMPILER_64_ZERO(rval); 

    /* configure modid and the magic port */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
    SOC_IF_ERROR_RETURN(READ_IARB_OLP_CONFIG_1r(unit, &rval));

    COMPILER_64_SET(set_val, 0, modid);
    soc_reg64_field_set(unit, IARB_OLP_CONFIG_1r, &rval, MY_MODIDf, set_val);
    COMPILER_64_SET(set_val, 0, _BCM_SAT_OLP_COMMUNICATION_PORT);
    soc_reg64_field_set(unit, IARB_OLP_CONFIG_1r, &rval, MY_PORT_NUMf, set_val);

    SOC_IF_ERROR_RETURN(WRITE_IARB_OLP_CONFIG_1r(unit, rval));
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_sb2_sat_olp_magic_port_clear
 * Purpose:
 *     Clear Magic port used in OLP-XGS communication 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_olp_magic_port_clear(int unit)
{
    uint64 rval, set_val;

    COMPILER_64_ZERO(rval); 

    /* clear modid and the magic port */
    SOC_IF_ERROR_RETURN(READ_IARB_OLP_CONFIG_1r(unit, &rval));

    COMPILER_64_SET(set_val, 0, 0);
    soc_reg64_field_set(unit, IARB_OLP_CONFIG_1r, &rval, MY_MODIDf, set_val);
    soc_reg64_field_set(unit, IARB_OLP_CONFIG_1r, &rval, MY_PORT_NUMf, set_val);

    SOC_IF_ERROR_RETURN(WRITE_IARB_OLP_CONFIG_1r(unit, rval));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_sb2_sat_hg_olp_mode_set
 * Purpose:
 *    Enable/Disable OLP handling on HG ports 
 * Parameters:
 *     unit - (IN) BCM device number
 *     olp_mode - (IN) OLP enable mode
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_hg_olp_mode_set(int unit, uint32 olp_mode)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    iarb_ing_physical_port_entry_t entry;

    BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
    PBMP_ITER(pbmp, port) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PHYSICAL_PORTm, 
                                         MEM_BLOCK_ANY, port, &entry));
        if (IS_HG_PORT(unit, port)) {
            soc_IARB_ING_PHYSICAL_PORTm_field32_set(unit, &entry, 
                                                    OLP_ENABLEf, olp_mode);
        } else {
            soc_IARB_ING_PHYSICAL_PORTm_field32_set(unit, &entry, 
                                                    OLP_ENABLEf, 0);
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, IARB_ING_PHYSICAL_PORTm, 
                                              MEM_BLOCK_ALL, port, &entry));
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_sb2_sat_endpoint_count_init
 * Purpose:
 *     Retrieves and initializes endpoint count information for this device.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     satc -  (IN) Pointer to device SAT control structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_endpoint_count_init(int unit, _bcm_sat_control_t *satc)
{

    /* Input parameter check. */
    if (NULL == satc) {
        return (BCM_E_PARAM);
    }

    /*
     * Get endpoint hardware table index count values and
     * initialize device SAT control structure members variables.
     */
    satc->upsamp_ep_count   = soc_mem_index_count(unit, EGR_SAT_SAMP_TCAMm);
    satc->downsamp_ep_count = soc_mem_index_count(unit, ING_SAT_SAMP_TCAMm);

    LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: Total No. UP SAMP endpoint Count = %d.\n"),
                unit, satc->upsamp_ep_count));

    LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: Total No. DOWN SAMP endpoint Count = %d.\n"),
                unit, satc->downsamp_ep_count));

    return (BCM_E_NONE);
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *     _bcm_sb2_sat_reinit
 * Purpose:
 *     Reconstruct SAT module software state during warmboot.
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_reinit(int unit)
{
    int        rv = BCM_E_NONE;     /* Operation return status. */

    LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: SAT warm boot recovery.....\n"),unit));

    /* Recover SAT UP MEP endpoints */
    rv = _bcm_sb2_sat_wb_upmep_endpoints_recover(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: UP MEP Endpoint recovery failed  - %s.\n"),
                  unit, bcm_errmsg(rv)));
        return (rv);
    }

    /* Recover SAT DOWN MEP endpoints */
    rv = _bcm_sb2_sat_wb_downmep_endpoints_recover(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: DOWN MEP Endpoint recovery failed  - %s.\n"),
                  unit, bcm_errmsg(rv)));
        return (rv);
    }
    
    return (rv);
}
#endif

/*
 * Function: bcm_sb2_sat_init
 *
 * Purpose:
 *     Initialize SAT module.
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - SAT feature not supported on this device.
 *     BCM_E_MEMORY  - Allocation failure
 *     BCM_E_XXX     - Error code from bcm_XX_sat_init()
 *     BCM_E_NONE    - Success
 */
int
bcm_sb2_sat_init(int unit)
{
    _bcm_sat_control_t *satc = NULL; /* SAT control structure.     */
    int             rv = BCM_E_NONE; /* Operation return value.    */
    uint32          size;            /* Size of memory allocation. */
    bcm_port_t      port = _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER;
    egr_enable_entry_t egr_enable_entry = {{0}};
    soc_info_t *si = &SOC_INFO(unit);
    int             oamp_enable;

    /* Ensure that the unit has SAT support. */
    if (!soc_feature(unit, soc_feature_sat)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: SAT feature not supported \n"), unit));
        return (BCM_E_UNAVAIL);
    }

    /* Detach first if the module has been previously initialized. */
    if (NULL != _sb2_sat_control[unit]) {
        _sb2_sat_control[unit]->init = FALSE;
        rv = bcm_sb2_sat_detach(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                      "SAT(unit %d) Error: SAT Module deinit - %s.\n"),
                      unit, bcm_errmsg(rv)));
            return (rv);
        }
    }

    /* Allocate SAT control memory for this unit. */
    _BCM_SAT_ALLOC(satc, _bcm_sat_control_t, 
                   sizeof (_bcm_sat_control_t),"SAT control");

    if (NULL == satc) {
        return (BCM_E_MEMORY);
    }

    /* Get number of endpoints supported by this unit. */
    rv = _bcm_sb2_sat_endpoint_count_init(unit, satc);
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }

    /* Mem_1: Allocate UP-MEP hash data memory */
    size = sizeof(_bcm_sat_hash_data_t) * satc->upsamp_ep_count;
    _BCM_SAT_ALLOC(satc->upsamp_hash_data, _bcm_sat_hash_data_t, 
                   size, "UP MEP Hash data");

    if (NULL == satc->upsamp_hash_data) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (BCM_E_MEMORY);
    }

	
    /* Mem_2: Create UP-MEP hash table. */
    rv = shr_htb_create(&satc->upsamp_htbl, satc->upsamp_ep_count,
                        sizeof(_bcm_sat_hash_key_t), "SAT UP-MEP Hash");
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }

    /* Mem_3: Allocate Down-MEP hash data memory */
     size = sizeof(_bcm_sat_hash_data_t) * satc->downsamp_ep_count;
     _BCM_SAT_ALLOC(satc->downsamp_hash_data, _bcm_sat_hash_data_t, 
                    size, "Down MEP Hash data");

     if (NULL == satc->downsamp_hash_data) {
         _bcm_sb2_sat_control_free(unit, satc);
         return (BCM_E_MEMORY);
     }
    

    /* Mem_4: Create Down-MEP hash table. */
    rv = shr_htb_create(&satc->downsamp_htbl, satc->downsamp_ep_count,
                        sizeof(_bcm_sat_hash_key_t), "SAT Down-MEP Hash");
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }
	
	/* Mem_5: Create UP MEP endpoint index list. */
    rv = shr_idxres_list_create(&satc->upsamp_mep_pool, 0, 
                                satc->upsamp_ep_count - 1, 0, 
                                satc->upsamp_ep_count -1,
                                "upsamp_mep_pool");         
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }

    /* Mem_6: Create DOWN MEP endpoint index list. */
    rv = shr_idxres_list_create(&satc->downsamp_mep_pool, 0, 
                            satc->downsamp_ep_count - 1, 0, 
                            satc->downsamp_ep_count -1, "downsamp_mep pool");
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }
	
    /* Create protection mutex. */
    satc->sat_lock = sal_mutex_create("sat_control.lock");
    if (NULL == satc->sat_lock) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (BCM_E_MEMORY);
    }

    /* Set up the unit SAT control structure. */
    _sb2_sat_control[unit] = satc;

    /* WARM Boot reint support for SAT   */
    #if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_sb2_sat_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_sb2_sat_control_free(unit, satc);
            return (rv);
        }
    } 
    #endif

    /* Enable OLP handling on HG ports */
    rv = _bcm_sb2_sat_hg_olp_mode_set(unit, _BCM_SAT_ENABLE);
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }

    /* Set default olp header type mapping */
    rv = _bcm_sb2_sat_olp_header_type_mapping_set(unit); 
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }

    /* Set Magic port used in OLP-XGS communication */
    rv = _bcm_sb2_sat_olp_magic_port_set(unit); 
    if (BCM_FAILURE(rv)) {
        _bcm_sb2_sat_control_free(unit, satc);
        return (rv);
    }

    /* Set OLP header add for SAT packet forwarded to CPU port */
    _BCM_SAT_CPU_OLP_HDR_SET(unit, _BCM_SAT_ENABLE);

    if(SOC_WARM_BOOT(unit)) {
        (void)bcm_sb2_sat_oamp_enable_get(unit, &oamp_enable);
    }

    if((!SOC_WARM_BOOT(unit) &&
            (soc_property_get(unit, spn_SAT_ENABLE, FALSE) == FALSE)) ||
      (SOC_WARM_BOOT(unit) && (oamp_enable == FALSE))) {
        /* SAT initialization complete. */
        _sb2_sat_control[unit]->init = TRUE;
        return (BCM_E_NONE);
    }

    /* Enable OAMP and egress */
    SOC_IF_ERROR_CLEAN_RETURN((bcm_sb2_sat_oamp_enable_set(unit, TRUE)),
            _bcm_sb2_sat_control_free(unit, satc));
    /* Keep port in FWD state w.r.t OAMP engine */
    if(!SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_stp_set(unit, port, 
                    BCM_STG_STP_FORWARD));
    }
    sal_memset(&egr_enable_entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, &egr_enable_entry, PRT_ENABLEf, 1);
    SOC_IF_ERROR_CLEAN_RETURN((WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port,
                &egr_enable_entry)),_bcm_sb2_sat_control_free(unit, satc));

    /* detach phy for the sat port */
    SOC_IF_ERROR_CLEAN_RETURN((soc_phyctrl_detach(unit, port)),
            _bcm_sb2_sat_control_free(unit, satc));
    /* Remove the port from ethernet pbmps */
    if (SOC_PBMP_MEMBER(si->ge.bitmap, port)) {
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
        SOC_PBMP_PORT_ADD(si->ge.disabled_bitmap, port);
    } else {
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        SOC_PBMP_PORT_ADD(si->xe.disabled_bitmap, port);
    }
    SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
    SOC_PBMP_PORT_ADD(si->ether.disabled_bitmap, port);

    SOC_PBMP_PORT_REMOVE(si->port.bitmap, port);
    SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, port);

    SOC_PBMP_PORT_ADD(si->intp.bitmap, port);
    SOC_PBMP_PORT_REMOVE(si->intp.disabled_bitmap, port);

    soc_katana2_pbmp_all_resync(unit) ;
    soc_esw_dport_init(unit);

    /* SAT initialization complete. */
    _sb2_sat_control[unit]->init = TRUE;
    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_sb2_sat_detach
 * Purpose:
 *     Shut down SAT subsystem
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_sb2_sat_detach(int unit)
{
    _bcm_sat_control_t *satc;  /* Pointer to SAT control structure. */
    int rv = BCM_E_NONE;

    /* Get the device SAT module handle. */
    satc = _sb2_sat_control[unit];

    if (NULL == satc) {
        /* Module already uninitialized. */
        return (BCM_E_NONE);
    }

    if (NULL != satc->sat_lock) {
        _BCM_SAT_LOCK(satc);
    }

    /* Disable OLP handling on HG ports */
    rv = _bcm_sb2_sat_hg_olp_mode_set(unit, _BCM_SAT_DISABLE);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error:Disable OLP handling on HG ports %s.\n"),
                  unit, bcm_errmsg(rv)));
    }

    /* Clear Magic port used in OLP-XGS communication */
    rv = _bcm_sb2_sat_olp_magic_port_clear(unit); 
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
        "SAT(unit %d) Error:Clear Magic port used in OLP-XGS communication %s.\n"),
                  unit, bcm_errmsg(rv)));
    }

    /* Disable OLP header add for SAT packet forwarded to CPU port */
   _BCM_SAT_CPU_OLP_HDR_SET(unit, _BCM_SAT_DISABLE);


	/* Destroy all UP-MEP endpoints and free the resources. */
    rv = bcm_sb2_sat_endpoint_destroy_all(unit, BCM_SAT_ENDPOINT_UPMEP);

     if (BCM_FAILURE(rv)) {
        /* Endpoint destroy failed. */
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error:UP-MEP Endpoint destory  %s.\n"),
                  unit, bcm_errmsg(rv)));
    }

   	/* Destroy all Down-MEP endpoints and free the resources. */
    rv = _bcm_sb2_sat_endpoint_destroy_all(unit, BCM_SAT_ENDPOINT_DOWNMEP);

     if (BCM_FAILURE(rv)) {
        /* Endpoint destroy failed. */
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error:Down-MEP Endpoint destroy %s.\n"),
                  unit, bcm_errmsg(rv)));
    }

     /* Release the protection mutex. */
    _BCM_SAT_UNLOCK(satc);

    /* Free SAT module allocated resources. */
    _bcm_sb2_sat_control_free(unit, satc);

    return (BCM_E_NONE);
}

#define KEY_PRINT 0 

#if defined(KEY_PRINT)
/*
 * Function:
 *     _bcm_sat_hash_key_print
 * Purpose:
 *     Print the contents of a hash key buffer. 
 * Parameters: 
 *     unit      - (IN) BCM device number
 *     hash_key - (IN) Pointer to hash key buffer.
 * Returns:
 *     None
 */
STATIC void
_bcm_sat_hash_key_print(int unit,_bcm_sat_hash_key_t *hash_key)
{
    int i;
    LOG_CLI((BSL_META_U(unit, 
                        "SAT HASH KEY:")));
    for(i = 0; i < _SAT_HASH_KEY_SIZE; i++) {
        LOG_CLI((BSL_META_U(unit,
                            ":%u"), *(hash_key[i])));
    }
    LOG_CLI((BSL_META_U(unit, 
                        "\n")));
}
#endif


/*
 * Function:
 *     _bcm_sb2_sat_ep_hash_key_construct
 * Purpose:
 *     Construct hash table key for a given endpoint information.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     satc    - (IN) Pointer to SAT control structure.
 *     ep_info - (IN) Pointer to endpoint information structure.
 *     key     - (IN/OUT) Pointer to hash key buffer.
 * Returns:
 *     None
 */
STATIC void
_bcm_sb2_sat_ep_hash_key_construct(int unit,
                                   _bcm_sat_control_t *satc,
                                   bcm_sat_endpoint_info_t *ep_info,
                                   _bcm_sat_hash_key_t *key)
{
    uint8  *loc = *key;

    sal_memset(key, 0, sizeof(_bcm_sat_hash_key_t));

    if (NULL != ep_info) {

        if (ep_info->flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN) {
             _BCM_SAT_KEY_PACK(loc, &ep_info->outer_vlan, 
                               sizeof(ep_info->outer_vlan));
        }

        if (ep_info->flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN) {
             _BCM_SAT_KEY_PACK(loc, &ep_info->inner_vlan, 
                                sizeof(ep_info->inner_vlan));		
        }
		
        _BCM_SAT_KEY_PACK(loc, &ep_info->dst_mac_address, 
                          sizeof(ep_info->dst_mac_address));

        _BCM_SAT_KEY_PACK(loc, &ep_info->src_mac_address, 
                          sizeof(ep_info->src_mac_address));

        _BCM_SAT_KEY_PACK(loc, &ep_info->src_gport, sizeof(ep_info->src_gport));

        _BCM_SAT_KEY_PACK(loc, &ep_info->ether_type, sizeof(ep_info->ether_type));

    }

    /* End address should not exceed size of _bcm_sat_hash_key_t. */
    assert ((int) (loc - *key) <= sizeof(_bcm_sat_hash_key_t));
}

/*
 * Function:
 *     _bcm_sb2_sat_control_get
 * Purpose:
 *     Lookup a SAT control config from a bcm device id.
 * Parameters:
 *     unit -  (IN)BCM unit number.
 *     satc -  (OUT) SAT control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_control_get(int unit, _bcm_sat_control_t **satc)
{
    if (NULL == satc) {
        return (BCM_E_PARAM);
    }

    /* Ensure sat module is initialized. */
    _BCM_SAT_IS_INIT(unit);

    *satc = _sb2_sat_control[unit];

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_sb2_sat_endpoint_gport_resolve
 * Purpose:
 *     Resolve an endpoint GPORT value to GLP value.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     ep_info_p  - (IN/OUT) Pointer to endpoint information.
 *     flags      - (IN) Flag indicates gport type source/dest to resolve. 
 *     glp        - (IN/OUT) Pointer to generic logical port value.
 *     trunk_id   - (IN/OUT) Pointer to trunk id
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_endpoint_gport_resolve(int unit,
                                    bcm_sat_endpoint_info_t *ep_info_p,
                                    uint32 flags,
                                    uint32 *glp,
                                    bcm_trunk_t *trunk_id)
{
    bcm_module_t       module_id;            /* Module ID           */
    bcm_port_t         port_id;              /* Port ID.            */
    bcm_gport_t        gport;                /* Endpoint source/dest gport */
    int                local_id;             /* Hardware ID.        */
    int                member_count = 0;     /* Trunk Member count. */
    int                rv = BCM_E_NONE;      /* Return status.      */
    uint8              glp_valid = 0;        /* Logical port valid. */
    int                local_member_count = 0;
    int                is_local = 0;
	
    if(flags & _BCM_SAT_SRC_GPORT) {
        gport = ep_info_p->src_gport;
    }
    else if(flags & _BCM_SAT_DEST_GPORT) {
        gport = ep_info_p->dest_gport;
    }
    else {
        return BCM_E_PARAM;
    }


    /* Get Trunk ID or (Modid + Port) value from Gport */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &module_id,
                                &port_id, trunk_id, &local_id));

    /*
     * If Gport is Trunk type, _bcm_esw_gport_resolve()
     * sets trunk_id. Using Trunk ID, get Dst Modid and Port value.
     */
    if (BCM_GPORT_IS_TRUNK(gport)) {

        if (BCM_TRUNK_INVALID == *trunk_id)  {
            /* Has to be a valid Trunk. */
            return (BCM_E_PARAM);
        }

        /* Construct Hw GLP value. */
        _BCM_SB2_SAT_MOD_PORT_TO_GLP(unit, module_id, port_id, 1, 
                                     *trunk_id, *glp);

        /* Get a member of the trunk belonging to this module */
        if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, *trunk_id, 1,
                                        &port_id,
                                        &local_member_count))) {
        }

        /* Get count of ports in this trunk. */
        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, *trunk_id, NULL, 0, NULL, &member_count));
        if (0 == member_count) {
            /* No members have been added to the trunk group yet */
            return BCM_E_PARAM;
        }

        /* Construct Hw GLP value. */
        _BCM_SB2_SAT_MOD_PORT_TO_GLP(unit, module_id, port_id, 0, -1, *glp);

        glp_valid = 1;
    }

    /*
     * Application can resolve the trunk and pass the designated
     * port as Gport value. Check if the Gport belongs to a trunk.
     */
    if ((BCM_TRUNK_INVALID == (*trunk_id))
        && (BCM_GPORT_IS_MODPORT(gport)
        || BCM_GPORT_IS_LOCAL(gport)
        || BCM_GPORT_IS_SUBPORT_PORT(gport)
        || BCM_GPORT_IS_SUBPORT_GROUP(gport))) {

        /* When Gport is ModPort. Subport or Port type, _bcm_esw_gport_resolve()
         * returns Modid and Port value. Use these values to make the DGLP
         * value.
         */
        _BCM_SB2_SAT_MOD_PORT_TO_GLP(unit, module_id, port_id, 0, -1, *glp);
		

        /* Use the Modid, Port value and determine if the port
         * belongs to a Trunk.
         */
        rv = bcm_esw_trunk_find(unit, module_id, port_id, trunk_id);
        if (BCM_SUCCESS(rv)) {
            /*
             * Port is member of a valid trunk.
             * Now create the SGLP value from Trunk ID.
             */
            /* Get a member of the trunk belonging to this module */
            if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, *trunk_id, 1,
                                        &port_id,
                                        &local_member_count))) {

                _BCM_SB2_SAT_MOD_PORT_TO_GLP(unit, module_id, port_id, 1, 
                                             *trunk_id, *glp);
            }
        }
        glp_valid = 1;
    }
    if (SOC_GPORT_IS_MIM_PORT(gport)) {
        rv = _bcm_esw_modid_is_local(unit, module_id, &is_local);
        if(BCM_SUCCESS(rv) && (is_local)) {  
            _BCM_SB2_SAT_MOD_PORT_TO_GLP(unit, module_id, port_id, 0, 
                                             -1, *glp);
            glp_valid = 1;
        }
    } 

    /*
     * At this point, glp should be valid.
     * Gport types other than TRUNK, MODPORT or LOCAL are not valid.
     */
    if (0 == glp_valid) {
        return (BCM_E_PORT);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_sb2_sat_endpoint_old_hash_key_get
 * Purpose:
 *      Get an SAT endpoint object old hash key
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      flags - (IN) Endpoint flags for type of endpoint to get
 *      hash_key  - (OUT) Pointer to endpoint hash key value.
 * Returns:
 *      BCM_E_XXX
 * Notes: Internal function this API assumes that caller has aquired SAT lock 
 *        before invoking this function.
 */
static int 
_bcm_sb2_sat_endpoint_old_hash_key_get(int unit, 
                                       bcm_sat_endpoint_t endpoint,
                                       uint32 flags,
                                       _bcm_sat_hash_key_t *hash_key)
{
    _bcm_sat_control_t    *satc;      /* Pointer to SAT control structure. */
    _bcm_sat_hash_data_t  *h_data_p;  /* Pointer to endpoint data.         */
    bcm_sat_endpoint_info_t endpoint_info;

    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));


    if (flags & BCM_SAT_ENDPOINT_UPMEP) {
         _BCM_SAT_MEP_INDEX_VALIDATE(satc->upsamp_ep_count,endpoint);

         /* Get the hash data pointer. */
         h_data_p = &satc->upsamp_hash_data[endpoint];
	}
	else if (flags & BCM_SAT_ENDPOINT_DOWNMEP) {
         _BCM_SAT_MEP_INDEX_VALIDATE(satc->downsamp_ep_count,endpoint);

         /* Get the hash data pointer. */
         h_data_p = &satc->downsamp_hash_data[endpoint];
	}
    else {
        return BCM_E_PARAM;
    }

    if (!h_data_p->in_use)
    {
        return BCM_E_NOT_FOUND;
    }

    /* Initialize endpoint info structure. */
    bcm_sat_endpoint_info_t_init(&endpoint_info);

    /* Set up endpoint information for key construction. */
    endpoint_info.ep_id = h_data_p->ep_info.ep_id;
    endpoint_info.outer_vlan = h_data_p->ep_info.outer_vlan;
    endpoint_info.inner_vlan = h_data_p->ep_info.inner_vlan;
    sal_memcpy(endpoint_info.dst_mac_address, 
               h_data_p->ep_info.dst_mac_address,
               sizeof(bcm_mac_t));
    sal_memcpy(endpoint_info.src_mac_address, 
               h_data_p->ep_info.src_mac_address,
               sizeof(bcm_mac_t));
    endpoint_info.ether_type = h_data_p->ep_info.ether_type;
    endpoint_info.flags = h_data_p->ep_info.flags;
    endpoint_info.src_gport = h_data_p->ep_info.src_gport;

    endpoint_info.action_flags = h_data_p->ep_info.action_flags;
    endpoint_info.pkt_pri = h_data_p->ep_info.pkt_pri;
    endpoint_info.dest_gport = h_data_p->ep_info.dest_gport;
    endpoint_info.timestamp_format = h_data_p->ep_info.timestamp_format;
    endpoint_info.session_id = h_data_p->ep_info.session_id;

    /* Calculate the hash key for given enpoint input parameters. */
    _bcm_sb2_sat_ep_hash_key_construct(unit, satc, &endpoint_info, hash_key);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_sb2_sat_endpoint_params_validate
 * Purpose:
 *     Validate an endpoint parameters.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     satc      - (IN) Pointer to SAT control structure.
 *     hash_key  - (IN) Pointer to endpoint hash key value.
 *     ep_info_p - (IN) Pointer to endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_endpoint_params_validate(int unit,
                                      _bcm_sat_control_t *satc,
                                      _bcm_sat_hash_key_t *hash_key,
                                      bcm_sat_endpoint_info_t *ep_info_p)
{
    int                     rv = BCM_E_NONE;  /* Operation return status. */ 
    _bcm_sat_hash_data_t    h_stored_data;
    shr_htb_hash_table_t    samp_htbl = NULL; /* SAT endpoint hash table. */
    _bcm_sat_hash_key_t     old_hash_key;     /* Hash Key buffer.         */

    LOG_DEBUG(BSL_LS_BCM_SAT, 
              (BSL_META_U(unit,
                 "SAT(unit %d) Info: _bcm_sb2_sat_endpoint_params_validate.\n"),
                 unit));

    /* BCM_SAT_ENDPOINT_DOWNMEP and BCM_SAT_ENDPOINT_UPMEP flags are
     * mutually exclusive, an endpoint can be either UP-MEP or Down-MEP 
     * only.
     */
    if ((ep_info_p->flags & BCM_SAT_ENDPOINT_DOWNMEP) &&
       (ep_info_p->flags & BCM_SAT_ENDPOINT_UPMEP)) {
        return (BCM_E_PARAM);
    }

    /* Check for invalid flags (use last flag value for check) */
    if ( ep_info_p->flags >= (BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN << 1)){
        return (BCM_E_PARAM);
    }

    /* Check for invalid action flags (use last action flag for check) */
    if ( ep_info_p->action_flags >= (BCM_SAT_ACTION_SAMPLE_TIMESTAMP << 1)){
        return (BCM_E_PARAM);
    }

    /* Same SAMAC and DAMAC are not valid */
    if (MAC_ADDR_EQUAL(ep_info_p->dst_mac_address, 
                       ep_info_p->src_mac_address)){
        return (BCM_E_PARAM);
    }

    /* Check for invalid timestamp field */
    if ( ep_info_p->timestamp_format > bcmSATTimestampFormatNTP ) {
        return (BCM_E_PARAM);
    }

    /* For replace operation, endpoint ID is required. */
    if ((ep_info_p->flags & BCM_SAT_ENDPOINT_REPLACE)
        && !(ep_info_p->flags & BCM_SAT_ENDPOINT_WITH_ID)) {
        return (BCM_E_PARAM);
    }

    /* Validate inner VLAN */
    /* coverity[result_independent_of_operands] */
    if (ep_info_p->flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN) {
        VLAN_CHK_ID(unit, (ep_info_p->inner_vlan & _BCM_SAT_VLAN_MASK));

        /* Validate inner VLAN priority */
	/* coverity[result_independent_of_operands] */
        VLAN_CHK_PRIO(unit,
         _BCM_SAT_VLAN_PRI_GET(ep_info_p->inner_vlan, _BCM_SAT_VLAN_PRIO_MASK));
    }

    /* Validate outer VLAN */
    /* coverity[result_independent_of_operands] */
    if (ep_info_p->flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN) {
        VLAN_CHK_ID(unit, (ep_info_p->outer_vlan & _BCM_SAT_VLAN_MASK));

        /* Validate outer VLAN priority */
	/* coverity[result_independent_of_operands] */
        VLAN_CHK_PRIO(unit,
         _BCM_SAT_VLAN_PRI_GET(ep_info_p->outer_vlan, _BCM_SAT_VLAN_PRIO_MASK));
    }

    /* Store samp_htbl appropriately for UP-MEP endpoint. */
    if(ep_info_p->flags & BCM_SAT_ENDPOINT_UPMEP ){
         samp_htbl = satc->upsamp_htbl;
    }

    /* Store samp_htbl appropriately for Down-MEP endpoint. */
    if(ep_info_p->flags & BCM_SAT_ENDPOINT_DOWNMEP ){
         samp_htbl = satc->downsamp_htbl;
    }

    if (samp_htbl == NULL) {
        return (BCM_E_INTERNAL);
    }

    if (ep_info_p->flags & BCM_SAT_ENDPOINT_WITH_ID) {

        /* Validate UP-MEP endpoint index value. */
        if(ep_info_p->flags & BCM_SAT_ENDPOINT_UPMEP ){
           _BCM_SAT_MEP_INDEX_VALIDATE(satc->upsamp_ep_count,
                                       ep_info_p->ep_id); 
        }

        /* Validate DOWN-MEP endpoint index value. */
        else if(ep_info_p->flags & BCM_SAT_ENDPOINT_DOWNMEP ){
            _BCM_SAT_MEP_INDEX_VALIDATE(satc->downsamp_ep_count,
                                        ep_info_p->ep_id);
        }

        /* Modify the hash key */
        rv = _bcm_sb2_sat_endpoint_old_hash_key_get(unit, 
                                                    ep_info_p->ep_id,
                                                    ep_info_p->flags,
                                                    &old_hash_key);
        if (rv != BCM_E_NONE) {
            sal_memcpy(&old_hash_key, hash_key, sizeof(_bcm_sat_hash_key_t));
        }
    }
    else {
        sal_memcpy(&old_hash_key, hash_key, sizeof(_bcm_sat_hash_key_t));
    }

#if defined(KEY_PRINT)
    /* coverity[overrun-buffer-val] */
    _bcm_sat_hash_key_print(unit, &old_hash_key);
#endif

    /*
     * Lookup using hash key value.
     * Last param value '0' specifies keep the match entry.
     * Value '1' would mean remove the entry from the table.
     * Mtched Params:
     *      .
     */
     rv = shr_htb_find(samp_htbl, old_hash_key,
                  (shr_htb_data_t *)&h_stored_data, 0);

    if (BCM_SUCCESS(rv)
        && !(ep_info_p->flags & BCM_SAT_ENDPOINT_REPLACE)) {

        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: Endpoint ID=%d in use %s.\n"),
                  unit, ep_info_p->ep_id, bcm_errmsg(BCM_E_EXISTS)));

        /* Endpoint must not be in use expect for Replace operation. */
        return (BCM_E_EXISTS);

    } else {

        LOG_DEBUG(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                     "SAT(unit %d) Info: Endpoint ID=%d Available. %s.\n"),
                     unit, ep_info_p->ep_id, bcm_errmsg(rv)));

    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_sb2_sat_endpoint_cleanup
 * Purpose:
 *     Free all the counters and the indexes allocated on endpoint create
 *     failure  
 * Parameters:
 *     unit - (IN) BCM device number
 *     upmep- (IN) UpMep/DownMep
 *     hash_key (IN)
 *     hash_data (IN) Pointer to endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
void
_bcm_sb2_sat_endpoint_cleanup(int unit, int upmep, 
                              _bcm_sat_hash_key_t  hash_key,
                              _bcm_sat_hash_data_t *hash_data)
{
    _bcm_sat_control_t   *satc;            /* Pointer to control structure.  */
    int                  rv = BCM_E_NONE;
    shr_htb_hash_table_t    samp_htbl;     /* SAT endpoint hash table.       */

    rv = _bcm_sb2_sat_control_get(unit, &satc);
    if (BCM_FAILURE(rv)) {
        return;
    }

    if (upmep) {
        rv = _bcm_sb2_sat_endpoint_hw_delete(unit, hash_data);
        if (BCM_FAILURE(rv)) {
            LOG_DEBUG(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                         "SAT(unit %d) Error: UP-MEP clear failed EP=%d %s.\n"),
                         unit, hash_data->ep_info.ep_id, bcm_errmsg(rv)));
        }
        samp_htbl = satc->upsamp_htbl;
	    shr_idxres_list_free(satc->upsamp_mep_pool, hash_data->ep_info.ep_id);
    } else {
        rv = _bcm_sb2_sat_endpoint_hw_delete(unit, hash_data);
        if (BCM_FAILURE(rv)) {
            LOG_DEBUG(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                         "SAT(unit %d) Error: Down-MEP clear failed EP=%d %s.\n"),
                         unit, hash_data->ep_info.ep_id, bcm_errmsg(rv)));
        }
        samp_htbl = satc->downsamp_htbl;
	    shr_idxres_list_free(satc->downsamp_mep_pool, hash_data->ep_info.ep_id);
    }
 
    /* Return entry to hash data entry to free pool. */
    shr_htb_find(samp_htbl, hash_key, (shr_htb_data_t *)hash_data, 1);

    /* Clear contents of hash data element. */
    _BCM_SAT_HASH_DATA_CLEAR(hash_data);
}


/*
 * Function:
 *      bcm_sb2_sat_endpoint_get
 * Purpose:
 *      Get an SAT endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      flags - (IN) Endpoint flags for type of endpoint to get
 *      endpoint_info - (OUT) Pointer to an SAT endpoint structure to receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_sb2_sat_endpoint_get(int unit, 
                         bcm_sat_endpoint_t endpoint,
                         uint32 flags,
                         bcm_sat_endpoint_info_t *endpoint_info)
{
    _bcm_sat_control_t    *satc;      /* Pointer to SAT control structure. */
    _bcm_sat_hash_data_t  *h_data_p;  /* Pointer to endpoint data.         */

    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));


    if (flags & BCM_SAT_ENDPOINT_UPMEP) {
         _BCM_SAT_MEP_INDEX_VALIDATE(satc->upsamp_ep_count, endpoint);

         /* Get the hash data pointer. */
         h_data_p = &satc->upsamp_hash_data[endpoint];
	}
	else {
         _BCM_SAT_MEP_INDEX_VALIDATE(satc->downsamp_ep_count, endpoint);

         /* Get the hash data pointer. */
         h_data_p = &satc->downsamp_hash_data[endpoint];
	}

    _BCM_SAT_LOCK(satc);

    if (!h_data_p->in_use)
    {
        if (NULL != satc->sat_lock) {
            _BCM_SAT_UNLOCK(satc);
        }
        return BCM_E_NOT_FOUND;
    }

    /* Initialize endpoint info structure. */
    bcm_sat_endpoint_info_t_init(endpoint_info);

    /* Set up endpoint information for key construction. */
    endpoint_info->ep_id = h_data_p->ep_info.ep_id;
    endpoint_info->outer_vlan = h_data_p->ep_info.outer_vlan;
    endpoint_info->inner_vlan = h_data_p->ep_info.inner_vlan;
    sal_memcpy(endpoint_info->dst_mac_address, 
               h_data_p->ep_info.dst_mac_address,
               sizeof(bcm_mac_t));
    sal_memcpy(endpoint_info->src_mac_address, 
               h_data_p->ep_info.src_mac_address,
               sizeof(bcm_mac_t));
    endpoint_info->ether_type = h_data_p->ep_info.ether_type;
    endpoint_info->flags = h_data_p->ep_info.flags;
    endpoint_info->src_gport = h_data_p->ep_info.src_gport;

    endpoint_info->action_flags = h_data_p->ep_info.action_flags;
    endpoint_info->pkt_pri = h_data_p->ep_info.pkt_pri;
    endpoint_info->dest_gport = h_data_p->ep_info.dest_gport;
    endpoint_info->timestamp_format = h_data_p->ep_info.timestamp_format;
    endpoint_info->session_id = h_data_p->ep_info.session_id;

    _BCM_SAT_UNLOCK(satc);

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_sb2_sat_endpoint_destroy
 * Purpose:
 *     Delete an endpoint and free all its allocated resources.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     ep_id  - (IN) Endpoint ID value.
 *     flags  - (IN) Endpoint flags.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_endpoint_destroy(int unit,
                              bcm_sat_endpoint_t ep_id, 
                              unsigned int flags)
{
    _bcm_sat_control_t      *satc;      /* Pointer to SAT control structure. */
    _bcm_sat_hash_data_t    *h_data_p;  /* Pointer to endpoint data.         */
    _bcm_sat_hash_key_t     hash_key;   /* Hash key buffer for lookup.       */
    bcm_sat_endpoint_info_t ep_info;    /* Endpoint information.             */
    shr_idxres_list_handle_t  mep_pool; /* SAT MEP index pool                */
    shr_htb_hash_table_t    samp_htbl;  /* SAT endpoint hash table.          */
    int                     rv;         /* Operation return status.          */

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    if (flags & BCM_SAT_ENDPOINT_UPMEP) {

	     mep_pool = satc->upsamp_mep_pool;
         samp_htbl = satc->upsamp_htbl;

         /* Get the hash data pointer. */
         h_data_p = &satc->upsamp_hash_data[ep_id];

         if(h_data_p->in_use != 1) {
             return BCM_E_NOT_FOUND;
         }

         /* Delete entry from hardware */
         rv = _bcm_sb2_sat_endpoint_hw_delete(unit, h_data_p);
         if (BCM_FAILURE(rv)) {
             LOG_DEBUG(BSL_LS_BCM_SAT, 
                       (BSL_META_U(unit,
                          "SAT(unit %d) Error: UP-MEP clear failed EP=%d %s.\n"),
                          unit, h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
             return (rv);
         }
	}
	else if (flags & BCM_SAT_ENDPOINT_DOWNMEP) {
	     mep_pool = satc->downsamp_mep_pool;
         samp_htbl = satc->downsamp_htbl;

         /* Get the hash data pointer. */
         h_data_p = &satc->downsamp_hash_data[ep_id];

         if(h_data_p->in_use != 1) {
             return BCM_E_NOT_FOUND;
         }

         /* Delete entry from hardware */
         rv = _bcm_sb2_sat_endpoint_hw_delete(unit, h_data_p);
         if (BCM_FAILURE(rv)) {
             LOG_DEBUG(BSL_LS_BCM_SAT, 
                       (BSL_META_U(unit,
                          "SAT(unit %d) Error: Down-MEP clear failed EP=%d %s.\n"),
                          unit, h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
             return (rv);
         }
	}
    else  {
         return BCM_E_PARAM;
    }

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(mep_pool, ep_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: Endpoint EP=%d %s.\n"),
                  unit, ep_id, bcm_errmsg(rv)));
        return (rv);
    }

    /* Initialize endpoint info structure. */
    bcm_sat_endpoint_info_t_init(&ep_info);

    /* Set up endpoint information for key construction. */
    ep_info.ep_id = h_data_p->ep_info.ep_id;
    ep_info.outer_vlan = h_data_p->ep_info.outer_vlan;
    ep_info.inner_vlan = h_data_p->ep_info.inner_vlan;
    sal_memcpy(ep_info.dst_mac_address, h_data_p->ep_info.dst_mac_address,
               sizeof(bcm_mac_t));
    sal_memcpy(ep_info.src_mac_address, h_data_p->ep_info.src_mac_address,
               sizeof(bcm_mac_t));
    ep_info.ether_type = h_data_p->ep_info.ether_type;
    ep_info.flags = h_data_p->ep_info.flags;
    ep_info.src_gport = h_data_p->ep_info.src_gport;

    /* Construct hash key for lookup + delete operation. */
    _bcm_sb2_sat_ep_hash_key_construct(unit, satc, &ep_info, &hash_key);

    /* Remove entry from hash table. */
    BCM_IF_ERROR_RETURN(shr_htb_find(samp_htbl, hash_key,
                                     (shr_htb_data_t *)h_data_p,
                                     1));

    /* Return ID back to free MEP ID pool.*/
    BCM_IF_ERROR_RETURN(shr_idxres_list_free(mep_pool, ep_id));

    /* Clear the hash data memory previously occupied by this endpoint. */
    _BCM_SAT_HASH_DATA_CLEAR(h_data_p);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_sb2_sat_endpoint_create
 * Purpose:
 *     Create or replace an SAT endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_sb2_sat_endpoint_create(int unit, bcm_sat_endpoint_info_t *endpoint_info)
{
    _bcm_sat_hash_data_t *hash_data = NULL; /* Endpoint hash data pointer.  */
    _bcm_sat_hash_key_t  hash_key;          /* Hash Key buffer.             */
    int                  ep_req_index;      /* Requested endpoint index.    */
    int                  rv;                /* Operation return status.     */
    _bcm_sat_control_t   *satc;             /* Pointer to SAT control       */
                                            /* structure.  
											*/
    shr_idxres_list_handle_t  mep_pool;     /* SAT MEP index pool */
    shr_htb_hash_table_t      samp_htbl;    /* SAT endpoint hash table.      */
    uint32               sglp = 0;          /* Source global logical port.  */
    uint32               dglp = 0;          /* Dest global logical port.    */
    bcm_trunk_t          trunk_id = BCM_TRUNK_INVALID;
    int                  up_mep = 0;        /* Endpoint is an upMep         */

    /* Validate input parameter. */
    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_SAT, 
              (BSL_META_U(unit,
            "SAT(unit %d) Info: bcm_sb2_sat_endpoint_create Endpoint ID=%d.\n"),
                 unit, endpoint_info->ep_id));

    /* Get SAT Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    _BCM_SAT_LOCK(satc);


   /* Calculate the hash key for given enpoint input parameters. */
    _bcm_sb2_sat_ep_hash_key_construct(unit, satc, endpoint_info, &hash_key);

#if defined(KEY_PRINT)
    /* coverity[overrun-buffer-val] */
    _bcm_sat_hash_key_print(unit, &hash_key);
#endif

    /* Validate endpoint input parameters. */
    rv = _bcm_sb2_sat_endpoint_params_validate(unit, satc, &hash_key,
                                               endpoint_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: (EP=%d) - %s.\n"),
                  unit, endpoint_info->ep_id, bcm_errmsg(rv)));
        _BCM_SAT_UNLOCK(satc);
        return (rv);
    }


    /* Resolve given endpoint gport value to Source GLP value. */
    rv = _bcm_sb2_sat_endpoint_gport_resolve(unit, endpoint_info,
                                             _BCM_SAT_SRC_GPORT, 
                                             &sglp, &trunk_id);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: Source Gport resolve (EP=%d) - %s.\n"),
                  unit, endpoint_info->ep_id, bcm_errmsg(rv)));
        _BCM_SAT_UNLOCK(satc);
        return (rv);
    }

    if( endpoint_info->action_flags & BCM_SAT_ACTION_FWD_ACTION_REDIRECT) {
        /* Resolve given endpoint gport value to Dest GLP value. */
        rv = _bcm_sb2_sat_endpoint_gport_resolve(unit, endpoint_info, 
                _BCM_SAT_DEST_GPORT,
                &dglp, &trunk_id);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                    (BSL_META_U(unit,
                                "SAT(unit %d) Error: Dest Gport resolve (EP=%d) - %s.\n"),
                     unit, endpoint_info->ep_id, bcm_errmsg(rv)));
            _BCM_SAT_UNLOCK(satc);
            return (rv);
        }
    }
    /* Replace an existing endpoint. */
    if (endpoint_info->flags & BCM_SAT_ENDPOINT_REPLACE) {
        /*
         * For replace operation, first delete the existing endpoint
         * and recreate a new endpoint with new parameters.
         */
        rv = _bcm_sb2_sat_endpoint_destroy(unit, endpoint_info->ep_id, 
                                           endpoint_info->flags);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                      "SAT(unit %d) Error: Endpoint destroy (EP=%d) - %s.\n"),
                      unit, endpoint_info->ep_id, bcm_errmsg(rv)));
            _BCM_SAT_UNLOCK(satc);
            return (rv);
        }
    }

	/* Get appropriate index pool handle for the MEP */
	if (endpoint_info->flags & BCM_SAT_ENDPOINT_UPMEP) {
	     mep_pool = satc->upsamp_mep_pool;
         up_mep = 1;
	}
	else  {
	     mep_pool = satc->downsamp_mep_pool;
         up_mep = 0;
	}
	
    /* Create a new endpoint with the requested ID. */
    if (endpoint_info->flags & BCM_SAT_ENDPOINT_WITH_ID) {
        ep_req_index = endpoint_info->ep_id;
        rv = shr_idxres_list_reserve(mep_pool, ep_req_index,
                                     ep_req_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                "SAT(unit %d) Error: Endpoint (EP=%d) reserve failed - %s.\n"),
                      unit, endpoint_info->ep_id, bcm_errmsg(rv)));
            _BCM_SAT_UNLOCK(satc);
            return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
        }
    } else {
        /* Allocate the next available endpoint index. */
        rv = shr_idxres_list_alloc(mep_pool,
                                   (shr_idxres_element_t *)&ep_req_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                "SAT(unit %d) Error: Endpoint (EP=%d) alloc failed - %s.\n"),
                      unit, endpoint_info->ep_id, bcm_errmsg(rv)));
            _BCM_SAT_UNLOCK(satc);
            return (rv);
        }
        /* Set the allocated endpoint id value. */
        endpoint_info->ep_id =  ep_req_index;
    }
	
    /* Get appropriate hash data pool handle for the MEP */
	if (up_mep) {
	     /* Get the hash data pointer where the data is to be stored. */
         hash_data = &satc->upsamp_hash_data[ep_req_index];
         samp_htbl = satc->upsamp_htbl;
	}
	else  {
	     /* Get the hash data pointer where the data is to be stored. */
         hash_data = &satc->downsamp_hash_data[ep_req_index];
         samp_htbl = satc->downsamp_htbl;
	}

    /* Clear the hash data element contents before storing the values. */
    _BCM_SAT_HASH_DATA_CLEAR(hash_data);
    hash_data->in_use = 1;
    hash_data->ep_info.ep_id = endpoint_info->ep_id;
    if (endpoint_info->flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN) {
        hash_data->ep_info.outer_vlan = endpoint_info->outer_vlan;
    }
    if (endpoint_info->flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN) {
        hash_data->ep_info.inner_vlan = endpoint_info->inner_vlan;
    }
    sal_memcpy(hash_data->ep_info.dst_mac_address, 
               endpoint_info->dst_mac_address,
               sizeof(bcm_mac_t));
    sal_memcpy(hash_data->ep_info.src_mac_address, 
               endpoint_info->src_mac_address,
               sizeof(bcm_mac_t));
    hash_data->ep_info.src_gport = endpoint_info->src_gport;
    hash_data->sglp   = (sglp & 0xFFFFFFFF);
    hash_data->ep_info.ether_type = endpoint_info->ether_type;
    hash_data->ep_info.flags = endpoint_info->flags;


    /* TCAM data */
    hash_data->ep_info.action_flags = endpoint_info->action_flags;
    hash_data->ep_info.dest_gport = endpoint_info->dest_gport;
    hash_data->dglp   = (dglp & 0xFFFFFFFF);
    hash_data->ep_info.pkt_pri = endpoint_info->pkt_pri;
    hash_data->ep_info.timestamp_format = endpoint_info->timestamp_format;
    hash_data->ep_info.session_id = endpoint_info->session_id;

    /* Insert entry into hash table */
    rv = shr_htb_insert(samp_htbl, hash_key, hash_data);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: Hash table insert failed EP=%d %s.\n"),
                  unit, endpoint_info->ep_id, bcm_errmsg(rv)));
        _bcm_sb2_sat_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
        _BCM_SAT_UNLOCK(satc);
        return (rv);
    }
    
	/* Configure endpoint info to hardware. */
    rv = _bcm_sb2_sat_mep_hw_set(unit, endpoint_info, sglp, dglp);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                            "SAT(unit %d) Error: %s hw set failed EP=%d %s.\n"),
                 unit, (up_mep ? "UP-MEP" : "Down-MEP"), endpoint_info->ep_id, bcm_errmsg(rv)));
        _bcm_sb2_sat_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
        _BCM_SAT_UNLOCK(satc);
        return (rv);
    }

    _BCM_SAT_UNLOCK(satc);
    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_sb2_sat_endpoint_destroy
 * Purpose:
 *     Destroy an SAT endpoint object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     endpoint - (IN) Endpoint ID to destroy.
 *     flags    - (IN) Endpoint flags.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_sb2_sat_endpoint_destroy(int unit, 
                             bcm_sat_endpoint_t endpoint, 
                             uint32 flags)
{
    _bcm_sat_control_t *satc; /* Pointer to SAT control structure. */
    int rv = BCM_E_NONE;      /* Operation return status.          */

    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    _BCM_SAT_LOCK(satc);

    rv = _bcm_sb2_sat_endpoint_destroy(unit, endpoint, flags);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: Endpoint destroy EP=%d failed %s.\n"),
                  unit, endpoint, bcm_errmsg(rv)));
    }

    _BCM_SAT_UNLOCK(satc);
    return (rv);
}


/*
 * Function:
 *     _bcm_sb2_sat_endpoint_destroy_all
 * Purpose:
 *     Delete all endpoints for given endpoint type and free all
 *     its allocated resources.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     flags  - (IN) Endpoint flags.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_endpoint_destroy_all(int unit, uint32  flags)
{
    _bcm_sat_control_t      *satc;      /* Pointer to SAT control structure. */
    bcm_sat_endpoint_t      ep_id;      /* Endpoint ID                       */  
    unsigned int            ep_count;   /* Endpoint count                    */
    int                     rv = BCM_E_NONE;/* Operation return status.      */

     /* Get SAT device control structure. */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    if (flags & BCM_SAT_ENDPOINT_UPMEP) {
	    ep_count = satc->upsamp_ep_count;
	}
	else if (flags & BCM_SAT_ENDPOINT_DOWNMEP) {
        ep_count = satc->downsamp_ep_count;
	}
    else {
        rv = BCM_E_PARAM;
        return rv;
    }

    for(ep_id = 0; ep_id<ep_count; ep_id++) {

        /* Check endpoint destroy status. */
        rv = _bcm_sb2_sat_endpoint_destroy(unit, ep_id, flags);

         if ((BCM_FAILURE(rv)) && (rv != BCM_E_NOT_FOUND)) {
            /* Endpoint destroy failed. */
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                      "SAT(unit %d) Error: Endpoint EP=%d %s.\n"),
                      unit, ep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_sb2_sat_endpoint_destroy_all
 * Purpose:
 *     Delete all endpoints for given endpoint type and free all
 *     its allocated resources.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     flags  - (IN) Endpoint flags.
 * Retruns:
 *     BCM_E_XXX
 */
int
bcm_sb2_sat_endpoint_destroy_all(int unit, uint32  flags)
{
    _bcm_sat_control_t      *satc;      /* Pointer to SAT control structure. */
    int                     rv = BCM_E_NONE;/* Operation return status.      */

     /* Get SAT device control structure. */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    _BCM_SAT_LOCK(satc);

    /* Check endpoint destroy status. */
    rv = _bcm_sb2_sat_endpoint_destroy_all(unit, flags);

     if (BCM_FAILURE(rv)) {
        /* Endpoint destroy failed. */
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error: Endpoint destory all %s.\n"),
                  unit, bcm_errmsg(rv)));
        _BCM_SAT_UNLOCK(satc);
        return (rv);
    }

     _BCM_SAT_UNLOCK(satc);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_sb2_sat_endpoint_traverse
 * Purpose:
 *     Traverse the set of SAT endpoints associated with the
 *     specified endpoint type, calling a specified callback for each one
 * Parameters:
 *     unit      - (IN) BCM device number
 *     flags     - (IN) The SAT endpoint type whose endpoints should be traversed
 *     cb        - (IN) A pointer to the callback function to call for each SAT
 *                      endpoint in the specified endpoint type
 *     user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_sb2_sat_endpoint_traverse(int unit, uint32 flags,
                              bcm_sat_endpoint_traverse_cb cb,
                              void *user_data)
{
    _bcm_sat_control_t      *satc;      /* Pointer to SAT control structure. */
    bcm_sat_endpoint_t      ep_id;      /* Endpoint ID                       */  
    unsigned int            ep_count;   /* Endpoint count                    */
    bcm_sat_endpoint_info_t ep_info;    /* Endpoint information              */ 
    _bcm_sat_hash_data_t    *h_data_p;  /* Endpoint count                    */
    _bcm_sat_hash_data_t    *hash_data; 
    int                     rv = BCM_E_NONE ; /* Operation return status.    */



    /* Validate input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Get SAT device control structure. */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));


    _BCM_SAT_LOCK(satc);

    if (flags & BCM_SAT_ENDPOINT_UPMEP) {
	     ep_count = satc->upsamp_ep_count;
         hash_data = satc->upsamp_hash_data;
	}
	else if (flags & BCM_SAT_ENDPOINT_DOWNMEP) {
         ep_count = satc->downsamp_ep_count;
         hash_data = satc->downsamp_hash_data;
	}
    else {
        _BCM_SAT_UNLOCK(satc);
        return (BCM_E_PARAM);
    }

    if (NULL == hash_data) {
        LOG_ERROR(BSL_LS_BCM_SAT, 
                  (BSL_META_U(unit,
                  "SAT(unit %d) Error:  endpoints data access failed %s.\n"),
                  unit, bcm_errmsg(BCM_E_INTERNAL)));
        _BCM_SAT_UNLOCK(satc);
        return (BCM_E_INTERNAL);
    }

    for(ep_id = 0; ep_id<ep_count; ep_id++) {

        h_data_p = &hash_data[ep_id];

        if (h_data_p->in_use != 1) {
            LOG_VERBOSE(BSL_LS_BCM_SAT, 
                        (BSL_META_U(unit,
                        "SAT(unit %d) Info:  endpoint does not exist %s.\n"),
                        unit, bcm_errmsg(BCM_E_INTERNAL)));
            continue;
        }

        bcm_sat_endpoint_info_t_init(&ep_info);

        /* Release the SAT control mutex Lock since it is acquired
         * in bcm_sb2_sat_endpoint_get
         */
         _BCM_SAT_UNLOCK(satc);

        rv = bcm_sb2_sat_endpoint_get(unit, ep_id, flags, &ep_info);

        /* Require the SAT control mutex Lock */
        _BCM_SAT_LOCK(satc);

        if (BCM_FAILURE(rv)) {
            _BCM_SAT_UNLOCK(satc);
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                      "SAT(unit %d) Error: EP=%d info get failed %s.\n"),
                      unit, h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
            return (rv);
        }

        rv = cb(unit, &ep_info, user_data);
        if (BCM_FAILURE(rv)) {
            _BCM_SAT_UNLOCK(satc);
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                      "SAT(unit %d) Error: EP=%d callback failed - %s.\n"),
                      unit, h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    _BCM_SAT_UNLOCK(satc);
    return (rv);
}
 

/*
 * Function:
 *     _bcm_sb2_sat_resolve_dglp
 * Purpose:
 *   Add an dglp profile entry 
 * Parameters:
 *     unit        - (IN) BCM device number
 *     dglp        - (IN) DGLP value
 *     olp_enable  - (OUT) Pointer to field indicating whether olp is enabled on
 *                         the port. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_sb2_sat_resolve_dglp(int unit, uint32 dglp, int *olp_enable) 
{ 
    int rv = BCM_E_NONE;
    egr_olp_dgpp_config_entry_t *buf;
    egr_olp_dgpp_config_entry_t *entry;
    int         index_max = 0;
    int         index = 0;
    uint32      configured_dglp = 0;
    int         entry_mem_size = 0;

    /* Check whether this DGLP is already configured as  OLP port */ 
    entry_mem_size = sizeof(egr_olp_dgpp_config_entry_t);
	
    /* Allocate buffer to store the DMAed table entries. */
    index_max = soc_mem_index_max(unit, EGR_OLP_DGPP_CONFIGm);
    buf = soc_cm_salloc(unit, entry_mem_size * (index_max + 1),
                              "OLP dglp config table entry buffer");
    if (NULL == buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(buf, 0, sizeof(entry_mem_size) * (index_max + 1));

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, EGR_OLP_DGPP_CONFIGm, MEM_BLOCK_ALL,
                            0, index_max, buf);
    if (BCM_FAILURE(rv)) {
        if (buf) {
            soc_cm_sfree(unit, buf);
        }
        return rv;
    }

    /* Iterate over the table entries. */
    for (index = 0; index <= index_max; index++) {
        entry = soc_mem_table_idx_to_pointer
                    (unit, EGR_OLP_DGPP_CONFIGm, egr_olp_dgpp_config_entry_t *,
                     buf, index);

        soc_mem_field_get(unit, EGR_OLP_DGPP_CONFIGm, 
                          (uint32 *)entry, DGLPf, &configured_dglp);
        if (dglp == configured_dglp) {
            *olp_enable = 1;
            break;
        }
    }
	
    if (buf) {
        soc_cm_sfree(unit, buf);
    }

    return (BCM_E_NONE);
} 

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *      _bcm_sb2_sat_wb_upmep_endpoints_recover 
 * Purpose:
 *     Recover SAT UPMEP endpoint configuration.
 * Parameters:
 *     unit        - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_wb_upmep_endpoints_recover(int unit)
{
    _bcm_gport_dest_t       gport_dest;   /* Gport specification.             */
    _bcm_sat_control_t      *satc;        /* Pointer to control structure.    */
    uint32                  index;        
    uint16                  sglp;         /* Source generic logical port.     */
    uint16                  dglp;         /* Destination generic logical port.*/
    uint16                  vlan_valid;   /* Vlan valid flag.                 */
    uint16                  vlan_pri;     /* Priority.                        */
    uint16                  vlan_cfi;     /* CFI field.                       */
    uint16                  mask;         /* mask field.                      */
    uint32                  fwd_action;   /* SAT packet forward action.       */
    uint32                  time_stamp;   /* Time stamp field.                */
    uint32                  copy_to_cpu;  /* Copy to CPU field.               */
    bcm_sat_endpoint_info_t hw_ep_info; /* Endpoint information read from HW table. */

    egr_sat_samp_tcam_entry_t      entry; /* UP-MEP SAMP TCAM  entry.   */
	egr_sat_samp_data_entry_t      dentry; /* UP-MEP SAMP Data entry.   */
	int  rv = BCM_E_NONE; /* Operation return status.     */


	LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: _bcm_sb2_sat_upmep_endpoints_recover.\n"),
                unit));
	
    sal_memset(&entry, 0, sizeof(egr_sat_samp_tcam_entry_t));
    sal_memset(&dentry, 0, sizeof(egr_sat_samp_data_entry_t));
	
    /*
     * Get SAT control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    _BCM_SAT_LOCK(satc);

    for (index = 0; index < satc->upsamp_ep_count; index++) {
        sal_memset(&hw_ep_info,0,sizeof(bcm_sat_endpoint_info_t));

        /* Get the UP MEP TCAM table entry. */
		rv = READ_EGR_SAT_SAMP_TCAMm(unit, MEM_BLOCK_ANY, index, &entry);
		if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                    "SAT(unit %d) Error: UP MEP TCAM table read (index=%d) failed "\
                    "- %s.\n"), unit, index, bcm_errmsg(rv)));
            goto cleanup;
        }
		
		/* Get the UP MEP DATA table entry. */
		rv = READ_EGR_SAT_SAMP_DATAm(unit, MEM_BLOCK_ANY, index, &dentry);
		if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                      "SAT(unit %d) Error: UP MEP DATA table read (index=%d) failed "\
                    "- %s.\n"), unit, index, bcm_errmsg(rv)));
            goto cleanup;
        }
        

        if (soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, &entry, VALIDf)) {

            hw_ep_info.ep_id = index;

            hw_ep_info.flags |= BCM_SAT_ENDPOINT_UPMEP;
            hw_ep_info.flags |= BCM_SAT_ENDPOINT_WITH_ID;

            /* Get ether type field for the UP-MEP SAMP entry. */
            hw_ep_info.ether_type = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit,
                                                            &entry, 
                                                            ETHERTYPEf);

            /* Get source port field for the UP-MEP SAMP entry. */
            sglp = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, &entry, GLPf);

            _bcm_gport_dest_t_init(&gport_dest);
            if(_BCM_SAT_GLP_TRUNK_BIT_GET(unit, sglp)) {
                gport_dest.tgid = _BCM_SAT_GLP_TRUNK_ID_GET(unit, sglp);
                gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                gport_dest.modid = _BCM_SAT_GLP_MODULE_ID_GET(unit, sglp);
                gport_dest.port = _BCM_SAT_GLP_PORT_GET(unit, sglp);
            }
            rv = _bcm_esw_gport_construct(unit, &gport_dest, 
                                          &hw_ep_info.src_gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SAT, 
                          (BSL_META_U(unit,
                          "SAT(unit %d) Error: Gport construct failed"\
                        " - %s.\n"), unit, bcm_errmsg(rv)));
                goto cleanup;
            }


            /* Get source MAC address field for the UP-MEP SAMP entry. */
            soc_mem_mac_addr_get(unit, EGR_SAT_SAMP_TCAMm, &entry, 
                                 MACSAf, hw_ep_info.src_mac_address);
            	
            /* Get destination MAC address field for the UP-MEP SAMP entry. */
            soc_mem_mac_addr_get(unit, EGR_SAT_SAMP_TCAMm, &entry, 
                                 MACDAf, hw_ep_info.dst_mac_address);

            /* Get the outer TAG valid field  */
            vlan_valid = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, 
                                                            OTAG_VALIDf); 

            if (vlan_valid) {
                hw_ep_info.flags |= BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN; 
            }

            hw_ep_info.outer_vlan = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, 
                                                      &entry, OVIDf);
                 
            vlan_pri = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, &entry, OPRIf);
            mask = (1 << soc_mem_field_length(unit, EGR_SAT_SAMP_TCAMm, 
                                              OPRI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.outer_vlan,vlan_pri,mask);

            vlan_cfi = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, &entry, OCFIf);
            mask = (1 << soc_mem_field_length(unit, EGR_SAT_SAMP_TCAMm, 
                                              OCFI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.outer_vlan,vlan_cfi,mask);

            /* Get the inner TAG valid field  */
            vlan_valid = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, 
                                                            ITAG_VALIDf); 
            if (vlan_valid) {
                hw_ep_info.flags |= BCM_SAT_ENDPOINT_MATCH_INNER_VLAN; 
            }

            hw_ep_info.inner_vlan = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, 
                                                      &entry, IVIDf);
            vlan_pri = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, &entry, IPRIf);
            mask = (1 << soc_mem_field_length(unit, EGR_SAT_SAMP_TCAMm, 
                                              IPRI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.inner_vlan,vlan_pri,mask);

            vlan_cfi = soc_EGR_SAT_SAMP_TCAMm_field32_get(unit, &entry, ICFIf);
            mask = (1 << soc_mem_field_length(unit, EGR_SAT_SAMP_TCAMm, 
                                              ICFI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.inner_vlan,vlan_cfi,mask);


            /* Retrieve SAT TCAM Data fields */

            /* Get the Copy-to-CPU field  */
            copy_to_cpu = soc_EGR_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                             COPYTO_CPUf);
        	
        	if (copy_to_cpu) {
        	    hw_ep_info.action_flags |= BCM_SAT_ACTION_COPY_TO_CPU;
        	}
        						 
            /* Get destination port */
            dglp = soc_EGR_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                      DESTINATIONf);

            _bcm_gport_dest_t_init(&gport_dest);
            if(_BCM_SAT_GLP_TRUNK_BIT_GET(unit, dglp)) {
                gport_dest.tgid = _BCM_SAT_GLP_TRUNK_ID_GET(unit, dglp);
                gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                gport_dest.modid = _BCM_SAT_GLP_MODULE_ID_GET(unit, dglp);
                gport_dest.port = _BCM_SAT_GLP_PORT_GET(unit, dglp);
            }
            rv = _bcm_esw_gport_construct(unit, &gport_dest, 
                                          &hw_ep_info.dest_gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SAT, 
                          (BSL_META_U(unit,
                          "SAT(unit %d) Error: Gport construct failed"\
                        " - %s.\n"), unit, bcm_errmsg(rv)));
                goto cleanup;
            }

        	/* Get forward action field */
            fwd_action = soc_EGR_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                            FORWARD_ACTIONf);
        	
        	/* Forward action for the SAT packet is to drop */
            if (fwd_action == 0 ) {
        		  hw_ep_info.action_flags |= BCM_SAT_ACTION_FWD_ACTION_DROP;
            }
            
        	/* Forward action for the SAT packet is redirect to DGLP port */
            else if (fwd_action == 1 ) {
                  hw_ep_info.action_flags |= BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
            }
        	/* Forward action for the SAT packet is perform LLF */
        	else if (fwd_action == 2 ) {
                  hw_ep_info.action_flags |= BCM_SAT_ACTION_FWD_ACTION_LLF;
            }

            /* Get Sample time stamp status into OLP header field */
            time_stamp = soc_EGR_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                            SAMPLE_TIMESTAMPf); 
        	if (time_stamp) {
                hw_ep_info.action_flags |= BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
        	}

            /* Get INT_PRI */
            hw_ep_info.pkt_pri = soc_EGR_SAT_SAMP_DATAm_field32_get(unit,
                                                                    &dentry, 
                                                                    INT_PRIf);
        	
            /* Get Session ID field  */
            hw_ep_info.session_id = soc_EGR_SAT_SAMP_DATAm_field32_get(unit,
                                            &dentry, SAT_SESSION_IDf);

            /* Get Time stamp type field  */
            hw_ep_info.timestamp_format = soc_EGR_SAT_SAMP_DATAm_field32_get(
                                            unit,&dentry, 
                                            TIMESTAMP_TYPEf);

            /* Release Lock here as lock is aquired in  bcm_sat_endpoint_create */
            _BCM_SAT_UNLOCK(satc);

            rv = bcm_sb2_sat_endpoint_create(unit,&hw_ep_info);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SAT, 
                          (BSL_META_U(unit,
             "SAT(unit %d) Error: UP MEP Endpoint table insert failed EP=%d %s.\n"),
                          unit, hw_ep_info.ep_id, bcm_errmsg(rv)));
                goto cleanup;
                return (rv);
            }else {
                LOG_VERBOSE(BSL_LS_BCM_SAT, 
                            (BSL_META_U(unit,
                            "SAT(unit %d) Info: UP MEP Hash Tbl (EP=%d)"\
                          " inserted  - %s.\n"), unit, hw_ep_info.ep_id,
                          bcm_errmsg(rv)));
            }

            /* Aquire Lock as lock is released in bcm_sat_endpoint_create */
            _BCM_SAT_LOCK(satc);
        }
    }

    _BCM_SAT_UNLOCK(satc);
    LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: _bcm_sb2_sat_wb_upmep_endpoints_recover"
              " - done.\n"), unit));
    return (rv);

cleanup:

    _BCM_SAT_UNLOCK(satc);

    LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: _bcm_sb2_sat_wb_upmep_endpoints_recover"\
              " - error_done.\n"), unit));
    return (rv);
}

/*
 * Function:
 *      _bcm_sb2_sat_wb_downmep_endpoints_recover 
 * Purpose:
 *     Recover SAT DOWNMEP endpoint configuration.
 * Parameters:
 *     unit        - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_wb_downmep_endpoints_recover(int unit)
{
    _bcm_gport_dest_t       gport_dest;   /* Gport specification.             */
    _bcm_sat_control_t      *satc;        /* Pointer to control structure.    */
    uint32                  index;        
    uint16                  sglp;         /* Source generic logical port.     */
    uint16                  dglp;         /* Destination generic logical port.*/
    uint16                  vlan_valid;   /* Vlan valid flag.                 */
    uint16                  vlan_pri;     /* Priority.                        */
    uint16                  vlan_cfi;     /* CFI field.                       */
    uint16                  mask;         /* mask field.                      */
    uint32                  fwd_action;   /* SAT packet forward action.       */
    uint32                  time_stamp;   /* Time stamp field.                */
    uint32                  copy_to_cpu;  /* Copy to CPU field.               */
    bcm_sat_endpoint_info_t hw_ep_info; /* Endpoint information read from HW table. */

    egr_sat_samp_tcam_entry_t      entry; /* DOWN-MEP SAMP TCAM  entry.   */
	egr_sat_samp_data_entry_t      dentry; /* DOWN-MEP SAMP Data entry.   */
	int  rv = BCM_E_NONE; /* Operation return status.     */


	LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: _bcm_sb2_sat_downmep_endpoints_recover.\n"),
                unit));
	
    sal_memset(&entry, 0, sizeof(egr_sat_samp_tcam_entry_t));
    sal_memset(&dentry, 0, sizeof(egr_sat_samp_data_entry_t));
	
    /*
     * Get SAT control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    _BCM_SAT_LOCK(satc);

    for (index = 0; index < satc->downsamp_ep_count; index++) {
        sal_memset(&hw_ep_info,0,sizeof(bcm_sat_endpoint_info_t));

        /* Get the DOWN MEP TCAM table entry. */
		rv = READ_ING_SAT_SAMP_TCAMm(unit, MEM_BLOCK_ANY, index, &entry);
		if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                  "SAT(unit %d) Error: DOWN MEP TCAM table read (index=%d) failed "\
                    "- %s.\n"), unit, index, bcm_errmsg(rv)));
            goto cleanup;
        }
		
		/* Get the DOWN MEP DATA table entry. */
		rv = READ_ING_SAT_SAMP_DATAm(unit, MEM_BLOCK_ANY, index, &dentry);
		if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                  "SAT(unit %d) Error: DOWN MEP DATA table read (index=%d) failed "\
                    "- %s.\n"), unit, index, bcm_errmsg(rv)));
            goto cleanup;
        }
        

        if (soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, VALIDf)) {

            hw_ep_info.ep_id = index;

            hw_ep_info.flags |= BCM_SAT_ENDPOINT_DOWNMEP;
            hw_ep_info.flags |= BCM_SAT_ENDPOINT_WITH_ID;
            

            /* Get ether type field for the DOWN-MEP SAMP entry. */
            hw_ep_info.ether_type = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, 
                                                            &entry, 
                                                            ETHERTYPEf);

            /* Get source port field for the DOWN-MEP SAMP entry. */
            sglp = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, GLPf);

            _bcm_gport_dest_t_init(&gport_dest);
            if(_BCM_SAT_GLP_TRUNK_BIT_GET(unit, sglp)) {
                gport_dest.tgid = _BCM_SAT_GLP_TRUNK_ID_GET(unit, sglp);
                gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                gport_dest.modid = _BCM_SAT_GLP_MODULE_ID_GET(unit, sglp);
                gport_dest.port = _BCM_SAT_GLP_PORT_GET(unit, sglp);
            }
            rv = _bcm_esw_gport_construct(unit, &gport_dest, 
                                          &hw_ep_info.src_gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SAT, 
                          (BSL_META_U(unit,
                          "SAT(unit %d) Error: Gport construct failed"\
                        " - %s.\n"), unit, bcm_errmsg(rv)));
                goto cleanup;
            }

            /* Get source MAC address field for the DOWN-MEP SAMP entry. */
            soc_mem_mac_addr_get(unit, ING_SAT_SAMP_TCAMm, &entry, 
                                 MACSAf, hw_ep_info.src_mac_address);
            	
            /* Get destination MAC address field for the DOWN-MEP SAMP entry. */
            soc_mem_mac_addr_get(unit, ING_SAT_SAMP_TCAMm, &entry, 
                                 MACDAf, hw_ep_info.dst_mac_address);

            /* Get the outer TAG valid field  */
            vlan_valid = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, 
                                                            OTAG_VALIDf); 

            if (vlan_valid) {
                hw_ep_info.flags |= BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN; 
            }

            hw_ep_info.outer_vlan = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, 
                                                      &entry, OVIDf);
                 
            vlan_pri = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, OPRIf);
            mask = (1 << soc_mem_field_length(unit, ING_SAT_SAMP_TCAMm, 
                                              OPRI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.outer_vlan,vlan_pri,mask);

            vlan_cfi = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, OCFIf);
            mask = (1 << soc_mem_field_length(unit, ING_SAT_SAMP_TCAMm, 
                                              OCFI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.outer_vlan,vlan_cfi,mask);

            /* Get the inner TAG valid field  */
            vlan_valid = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, 
                                                            ITAG_VALIDf); 
            if (vlan_valid) {
                hw_ep_info.flags |= BCM_SAT_ENDPOINT_MATCH_INNER_VLAN; 
            }

            hw_ep_info.inner_vlan = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, 
                                                      &entry, IVIDf);
            vlan_pri = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, IPRIf);
            mask = (1 << soc_mem_field_length(unit, ING_SAT_SAMP_TCAMm, 
                                              IPRI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.inner_vlan,vlan_pri,mask);

            vlan_cfi = soc_ING_SAT_SAMP_TCAMm_field32_get(unit, &entry, ICFIf);
            mask = (1 << soc_mem_field_length(unit, ING_SAT_SAMP_TCAMm, 
                                              ICFI_MASKf)) - 1;
            _BCM_SAT_VLAN_PRI_SET(hw_ep_info.inner_vlan,vlan_cfi,mask);


            /* Retrieve SAT TCAM Data fields */

            /* Get the Copy-to-CPU field  */
            copy_to_cpu = soc_ING_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                             COPYTO_CPUf);
        	
        	if (copy_to_cpu) {
        	    hw_ep_info.action_flags |= BCM_SAT_ACTION_COPY_TO_CPU;
        	}
        						 
            /* Get destination port */
            dglp = soc_ING_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                      DESTINATIONf);

            _bcm_gport_dest_t_init(&gport_dest);
            if(_BCM_SAT_GLP_TRUNK_BIT_GET(unit, dglp)) {
                gport_dest.tgid = _BCM_SAT_GLP_TRUNK_ID_GET(unit, dglp);
                gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                gport_dest.modid = _BCM_SAT_GLP_MODULE_ID_GET(unit, dglp);
                gport_dest.port = _BCM_SAT_GLP_PORT_GET(unit, dglp);
            }
            rv = _bcm_esw_gport_construct(unit, &gport_dest, 
                                          &hw_ep_info.dest_gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SAT, 
                          (BSL_META_U(unit,
                          "SAT(unit %d) Error: Gport construct failed"\
                        " - %s.\n"), unit, bcm_errmsg(rv)));
                goto cleanup;
            }

        	/* Get forward action field */
            fwd_action = soc_ING_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                            FORWARD_ACTIONf);
        	
        	/* Forward action for the SAT packet is to drop */
            if (fwd_action == 0 ) {
        		  hw_ep_info.action_flags |= BCM_SAT_ACTION_FWD_ACTION_DROP;
            }
            
        	/* Forward action for the SAT packet is redirect to DGLP port */
            else if (fwd_action == 1 ) {
                  hw_ep_info.action_flags |= BCM_SAT_ACTION_FWD_ACTION_REDIRECT;
            }
        	/* Forward action for the SAT packet is perform LLF */
        	else if (fwd_action == 2 ) {
                  hw_ep_info.action_flags |= BCM_SAT_ACTION_FWD_ACTION_LLF;
            }

            /* Get Sample time stamp status into OLP header field */
            time_stamp = soc_ING_SAT_SAMP_DATAm_field32_get(unit, &dentry,
                                                            SAMPLE_TIMESTAMPf); 
        	if (time_stamp) {
                hw_ep_info.action_flags |= BCM_SAT_ACTION_SAMPLE_TIMESTAMP;
        	}

            /* Get INT_PRI */
            hw_ep_info.pkt_pri = soc_ING_SAT_SAMP_DATAm_field32_get(unit,
                                                                    &dentry, 
                                                                    INT_PRIf);
        	
            /* Get Session ID field  */
            hw_ep_info.session_id = soc_ING_SAT_SAMP_DATAm_field32_get(unit,
                                            &dentry, SAT_SESSION_IDf);

            /* Get Time stamp type field  */
            hw_ep_info.timestamp_format = soc_ING_SAT_SAMP_DATAm_field32_get(
                                            unit,&dentry, 
                                            TIMESTAMP_TYPEf);

            /* Release Lock here as lock is aquired in  bcm_sat_endpoint_create */
            _BCM_SAT_UNLOCK(satc);

            rv = bcm_sb2_sat_endpoint_create(unit,&hw_ep_info);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SAT, 
                          (BSL_META_U(unit,
            "SAT(unit %d) Error: DownMEP Endpoint table insert failed EP=%d %s.\n"),
                          unit, hw_ep_info.ep_id, bcm_errmsg(rv)));
                goto cleanup;
                return (rv);
            }else {
                LOG_VERBOSE(BSL_LS_BCM_SAT, 
                            (BSL_META_U(unit,
                            "SAT(unit %d) Info: DOWN MEP Hash Tbl (EP=%d)"\
                          " inserted  - %s.\n"), unit, hw_ep_info.ep_id,
                          bcm_errmsg(rv)));
            }

            /* Aquire Lock as lock is released in bcm_sat_endpoint_create */
            _BCM_SAT_LOCK(satc);
        }
    }

    _BCM_SAT_UNLOCK(satc);

    LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: _bcm_sb2_sat_wb_downmep_endpoints_recover"
              " - done.\n"), unit));
    return (rv);

cleanup:
    _BCM_SAT_UNLOCK(satc);
    LOG_VERBOSE(BSL_LS_BCM_SAT, 
                (BSL_META_U(unit,
                "SAT(unit %d) Info: _bcm_sb2_sat_wb_downmep_endpoints_recover"\
              " - error_done.\n"), unit));
    return (rv);
}
#endif

/*
 * Function:
 *     _bcm_sb2_sat_mep_hw_set
 * Purpose:
 *     Configure hardware tables for an UP-MEP/DOWN-MEP endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to UP-MEP/DOWN_MEP endpoint information.
 *     sglp      - (IN) SGLP port for UP-MEP/DOWN_MEP endpoint. 
 *     dglp      - (IN) DGLP port for UP-MEP/DOWN_MEP endpoint. 
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_mep_hw_set(int unit, bcm_sat_endpoint_info_t *ep_info_p,
                        uint16 sglp, uint16 dglp)
{
    _bcm_sat_hash_data_t *hash_data; /* Pointer to endpoint hash data. */
    ing_sat_samp_tcam_entry_t      ing_entry; /* DOWNMEP SAMP TCAM  entry.   */
	ing_sat_samp_data_entry_t      ing_dentry; /* DOWNMEP SAMP Data entry.   */
    egr_sat_samp_tcam_entry_t      egr_entry; /* UPMEP SAMP TCAM  entry.   */
    egr_sat_samp_data_entry_t      egr_dentry; /* UPMEP SAMP Data entry.   */
    egr_sat_samp_data_1_entry_t      egr_d_1entry; /* UPMEP SAMP Data_1 entry.   */
    void   *ptr_d_1entry; /* MEP SAMP Data entry.   */
    void   *ptr_entry; /*  MEP SAMP TCAM  entry.   */
    void   *ptr_dentry; /* MEP SAMP Data entry.   */
    _bcm_sat_control_t   *satc;        /* Pointer to control structure.  */
    uint8     fwd_action = 0;
    bcm_mac_t mac_mask;
    uint32    mask_field;	
    int       olp_enable = 0;
    uint8     vlan_pri = 0;
    uint8     vlan_cfi = 0;
	soc_mem_t tcam_mem;
    soc_mem_t data_mem;
    int       rv = BCM_E_NONE;/* Operation return status.      */


    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_sb2_sat_control_get(unit, &satc));

    if (ep_info_p->flags & BCM_SAT_ENDPOINT_UPMEP) {
        ptr_entry  = &egr_entry;
        ptr_dentry = &egr_dentry;
        tcam_mem = EGR_SAT_SAMP_TCAMm;
        data_mem = EGR_SAT_SAMP_DATAm;

        /* Get the stored endpoint information from hash table. */
        hash_data = &satc->upsamp_hash_data[ep_info_p->ep_id];
        if (0 == hash_data->in_use) {
            return(BCM_E_INTERNAL);
        }
        sal_memset(ptr_entry, 0, sizeof(egr_sat_samp_tcam_entry_t));
        sal_memset(ptr_dentry, 0, sizeof(egr_sat_samp_data_entry_t));
    }
    else if (ep_info_p->flags & BCM_SAT_ENDPOINT_DOWNMEP) {
        ptr_entry  = &ing_entry;
        ptr_dentry = &ing_dentry;
        tcam_mem = ING_SAT_SAMP_TCAMm;
        data_mem = ING_SAT_SAMP_DATAm;

        /* Get the stored endpoint information from hash table. */
        hash_data = &satc->downsamp_hash_data[ep_info_p->ep_id];
        if (0 == hash_data->in_use) {
            return(BCM_E_INTERNAL);
        }
        sal_memset(ptr_entry, 0, sizeof(ing_sat_samp_tcam_entry_t));
        sal_memset(ptr_dentry, 0, sizeof(ing_sat_samp_data_entry_t));
    }
    else 
    {
        return(BCM_E_PARAM);
    }

    /* Set ether type field for the MEP SAMP entry. */
    soc_mem_field32_set(unit, tcam_mem, ptr_entry, 
                        ETHERTYPEf, ep_info_p->ether_type);
									   
	/* Set ether type field mask for the MEP SAMP entry. */
	mask_field = (1 << soc_mem_field_length(unit, tcam_mem, ETHERTYPE_MASKf)) - 1;
		
    soc_mem_field32_set(unit, tcam_mem, ptr_entry, ETHERTYPE_MASKf, mask_field);

    /* Set source port field for the MEP SAMP entry. */
    soc_mem_field32_set(unit, tcam_mem, ptr_entry, GLPf, sglp);
	
	/* Set source port field mask for the MEP SAMP entry. */
    mask_field = (1 << soc_mem_field_length(unit, tcam_mem, GLP_MASKf)) - 1;
		
    soc_mem_field32_set(unit, tcam_mem, ptr_entry, GLP_MASKf, mask_field);

	
    /* Set source MAC address field for the MEP SAMP entry. */
    soc_mem_mac_addr_set(unit, tcam_mem, ptr_entry, 
                         MACSAf, ep_info_p->src_mac_address);
	
    /* Set source MAC address field mask for the MEP SAMP entry. */
    sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
    soc_mem_mac_addr_set(unit, tcam_mem, ptr_entry, 
                         MACSA_MASKf, mac_mask);


    /* Set destination MAC address field for the MEP SAMP entry. */
    soc_mem_mac_addr_set(unit, tcam_mem, ptr_entry, 
                         MACDAf, ep_info_p->dst_mac_address);

    /* Set destination MAC address field mask for the MEP SAMP entry. */
    sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
    soc_mem_mac_addr_set(unit, tcam_mem, ptr_entry, 
                         MACDA_MASKf, mac_mask);
	
	
    /* Configure ITAG fields to hardware only if user flag to match
     * inner VLAN is configured.
     */
    if (ep_info_p->flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN) {

        /* Set the inner VLAN field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, IVIDf)) - 1;

        soc_mem_field32_set(unit, tcam_mem, ptr_entry, IVIDf, 
                             (ep_info_p->inner_vlan & mask_field));

        /* Set the inner VLAN mask field  */
        soc_mem_field32_set(unit, tcam_mem, ptr_entry, IVID_MASKf, mask_field);

        /* Set the inner TAG valid field  */
        soc_mem_field32_set(unit, tcam_mem, ptr_entry, ITAG_VALIDf, 
            ((ep_info_p->flags & BCM_SAT_ENDPOINT_MATCH_INNER_VLAN) ? 1 : 0)); 

        /* Set the inner TAG valid mask field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, 
                                                ITAG_VALID_MASKf)) - 1;
            
        soc_mem_field32_set(unit, tcam_mem, ptr_entry, 
                            ITAG_VALID_MASKf, mask_field);

        /* Set the inner VLAN priority field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, IPRI_MASKf)) - 1;

        vlan_pri = _BCM_SAT_VLAN_PRI_GET(ep_info_p->inner_vlan, mask_field);

        if (vlan_pri) {
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, IPRIf, vlan_pri);

            /* Set the inner VLAN priority mask field  */
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, 
                                IPRI_MASKf, mask_field);
        }


        /* Set the inner VLAN CFI field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, ICFI_MASKf)) - 1;

        vlan_cfi = _BCM_SAT_VLAN_CFI_GET(ep_info_p->inner_vlan, mask_field);

        if (vlan_cfi) {
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, ICFIf, vlan_cfi);

            /* Set the inner VLAN CFI mask field  */
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, 
                                ICFI_MASKf, mask_field);
        }
    }

    /* Configure OTAG valid and OTAG valid mask fields to hardware only if
     * user flag to match outer VLAN is configured.
     */
    if (ep_info_p->flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN) {
        
        /* Set the outer VLAN valid field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, OVIDf)) - 1;
        soc_mem_field32_set(unit, tcam_mem, ptr_entry, OVIDf, 
                             (ep_info_p->outer_vlan & mask_field));

        /* Set the outer VLAN mask field  */
        soc_mem_field32_set(unit, tcam_mem, ptr_entry,
                            OVID_MASKf, mask_field);
       
        /* Set the outer TAG valid field  */
        soc_mem_field32_set(unit, tcam_mem, ptr_entry, OTAG_VALIDf, 
                ((ep_info_p->flags & BCM_SAT_ENDPOINT_MATCH_OUTER_VLAN) ? 1 : 0));

        /* Set the outer TAG valid mask field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, 
                                                OTAG_VALID_MASKf)) - 1;
            
        soc_mem_field32_set(unit, tcam_mem, ptr_entry, 
                            OTAG_VALID_MASKf, mask_field);

        /* Set the outer VLAN priority field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, OPRI_MASKf)) - 1;

        vlan_pri = _BCM_SAT_VLAN_PRI_GET(ep_info_p->outer_vlan, mask_field);

        if (vlan_pri) {
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, OPRIf, vlan_pri);

            /* Set the outer VLAN priority mask field  */
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, 
                                OPRI_MASKf, mask_field);
        }

        /* Set the outer VLAN CFI field  */
        mask_field = (1 << soc_mem_field_length(unit, tcam_mem, OCFI_MASKf)) - 1;
        vlan_cfi = _BCM_SAT_VLAN_CFI_GET(ep_info_p->outer_vlan, mask_field);

        if (vlan_cfi) {
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, OCFIf, vlan_cfi);

            /* Set the outer VLAN CFI mask field  */
            soc_mem_field32_set(unit, tcam_mem, ptr_entry, 
                                OCFI_MASKf, mask_field);
        }
    }


    /* Set the VALID field  */ 
    soc_mem_field32_set(unit, tcam_mem, ptr_entry, VALIDf, 
                        BCM_SAT_TCAM_ENTRY_VALID);
    if (ep_info_p->action_flags & BCM_SAT_ACTION_FWD_ACTION_REDIRECT) {   	
        /* Store Egress SAT SAMP DATA fields */	
        rv = _bcm_sb2_sat_resolve_dglp(unit, dglp, &olp_enable); 
        if (BCM_FAILURE(rv)) {
            /* DGLP resolve failed */;
            LOG_ERROR(BSL_LS_BCM_SAT, 
                    (BSL_META_U(unit,
                                "SAT(unit %d) Error: DGLP resolve for Endpoint EP=%d %s.\n"),
                     unit, ep_info_p->ep_id, bcm_errmsg(rv)));
        }
    }
    /* Set the OLP header add field  */
    soc_mem_field32_set(unit, data_mem, ptr_dentry, ADD_OLP_HDRf,olp_enable); 

	/* Set the Copy-to-CPU field  */
    soc_mem_field32_set(unit, data_mem, ptr_dentry, COPYTO_CPUf, 
           ((ep_info_p->action_flags & BCM_SAT_ACTION_COPY_TO_CPU ) ? 1 : 0));
						 
    /* Store destination port */
    soc_mem_field32_set(unit, data_mem, ptr_dentry, DESTINATIONf, dglp);
	
	/* Forward action for the SAT packet is to drop */
    if(ep_info_p->action_flags & BCM_SAT_ACTION_FWD_ACTION_DROP ) {
          fwd_action = 0;
    }	
	/* Forward action for the SAT packet is redirect to DGLP port */
    else if(ep_info_p->action_flags & BCM_SAT_ACTION_FWD_ACTION_REDIRECT ) {
          fwd_action = 1;
    }	
	/* Forward action for the SAT packet is perform */
	else if(ep_info_p->action_flags & BCM_SAT_ACTION_FWD_ACTION_LLF ) {
          fwd_action = 2;
    }
	
	/* Store forward action field */
    soc_mem_field32_set(unit, data_mem, ptr_dentry, 
                                       FORWARD_ACTIONf,fwd_action);

	/* Store INT_PRI */
    mask_field = (1 << soc_mem_field_length(unit, data_mem, 
                                            INT_PRIf)) - 1;
    soc_mem_field32_set(unit, data_mem, ptr_dentry, INT_PRIf, 
                                       ep_info_p->pkt_pri & mask_field);
	
    /* Sample time stamp into the OLP header field  */
    soc_mem_field32_set(unit, data_mem, ptr_dentry, SAMPLE_TIMESTAMPf, 
       ((ep_info_p->action_flags & BCM_SAT_ACTION_SAMPLE_TIMESTAMP ) ? 1 : 0));
			

	/* Set OLP header type compressed field OLP_HDR_TYPE_COMPRESSEDf */ 
    if (ep_info_p->flags & BCM_SAT_ENDPOINT_DOWNMEP) {
        mask_field = (1 << soc_mem_field_length(unit, data_mem, 
                                                OLP_HDR_TYPE_COMPRESSEDf)) - 1;
        soc_mem_field32_set(unit, data_mem, ptr_dentry, OLP_HDR_TYPE_COMPRESSEDf, 
                            BCM_SAT_DOWNMEP_OLP_HDR_COMPRESSED & mask_field);			
    }
    else if (ep_info_p->flags & BCM_SAT_ENDPOINT_UPMEP) {
        mask_field = (1 << soc_mem_field_length(unit, data_mem, 
                                                OLP_HDR_TYPE_COMPRESSEDf)) - 1;
        soc_mem_field32_set(unit, data_mem, ptr_dentry, OLP_HDR_TYPE_COMPRESSEDf, 
                            BCM_SAT_UPMEP_OLP_HDR_COMPRESSED & mask_field);			
    }

	
    /* Store Session ID field  */
    soc_mem_field32_set(unit, data_mem, ptr_dentry, 
                                       SAT_SESSION_IDf,ep_info_p->session_id);

    /* Set Time stamp type field  */
    soc_mem_field32_set(unit, data_mem, ptr_dentry, TIMESTAMP_TYPEf, 
                                       ep_info_p->timestamp_format);
	

    if (ep_info_p->flags & BCM_SAT_ENDPOINT_UPMEP) {
        /* Write entry to hardware EGR_SAT_SAMP_TCAM table. */
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_SAT_SAMP_TCAMm(unit, MEM_BLOCK_ALL, 
            ep_info_p->ep_id, ptr_entry));

        /* Write entry to hardware EGR_SAT_SAMP_TCAM table. */
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_SAT_SAMP_DATAm(unit, MEM_BLOCK_ALL, 
            ep_info_p->ep_id, ptr_dentry));
    }
    else  if (ep_info_p->flags & BCM_SAT_ENDPOINT_DOWNMEP) {
        /* Write entry to hardware ING_SAT_SAMP_TCAM table. */
        SOC_IF_ERROR_RETURN
            (WRITE_ING_SAT_SAMP_TCAMm(unit, MEM_BLOCK_ALL, 
            ep_info_p->ep_id, ptr_entry));

        /* Write entry to hardware ING_SAT_SAMP_TCAM table. */
        SOC_IF_ERROR_RETURN
            (WRITE_ING_SAT_SAMP_DATAm(unit, MEM_BLOCK_ALL, 
            ep_info_p->ep_id, ptr_dentry));
    }
    else
    {
        return (BCM_E_PARAM);
    }
    
    /* For SAT-UPSAMP processing EGR_SAT_SAMP_DATA_1 Contains OLP-HDR related items.
     * This is a shadow table of the original EGR_SAT_SAMP_DATA table and
     *  stores only a subset of fields.Looked up only in Pass-B */
    if (ep_info_p->flags & BCM_SAT_ENDPOINT_UPMEP) {

        data_mem = EGR_SAT_SAMP_DATA_1m;
        ptr_d_1entry = &egr_d_1entry;

        /* Store Session ID field  */
        soc_mem_field32_set(unit, data_mem, ptr_d_1entry, 
                                       SAT_SESSION_IDf,ep_info_p->session_id);

        /* Set Time stamp type field  */
        soc_mem_field32_set(unit, data_mem, ptr_d_1entry, TIMESTAMP_TYPEf, 
                                       ep_info_p->timestamp_format);
        /* Sample time stamp into the OLP header field  */
        soc_mem_field32_set(unit, data_mem, ptr_d_1entry, SAMPLE_TIMESTAMPf, 
            ((ep_info_p->action_flags & BCM_SAT_ACTION_SAMPLE_TIMESTAMP ) ? 1 : 0));

        /* Set OLP header type compressed field OLP_HDR_TYPE_COMPRESSEDf */ 
        mask_field = (1 << soc_mem_field_length(unit, data_mem, 
                                                OLP_HDR_TYPE_COMPRESSEDf)) - 1;
        soc_mem_field32_set(unit, data_mem, ptr_d_1entry, OLP_HDR_TYPE_COMPRESSEDf, 
                            BCM_SAT_UPMEP_OLP_HDR_COMPRESSED & mask_field);			

        /* Set the OLP header add field  */
        soc_mem_field32_set(unit, data_mem, ptr_d_1entry, ADD_OLP_HDRf,olp_enable); 

        /* Write entry to hardware EGR_SAT_SAMP_DATA_1 table. */
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_SAT_SAMP_DATA_1m(unit, MEM_BLOCK_ALL, 
            ep_info_p->ep_id, ptr_d_1entry));

   }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_sb2_sat_endpoint_hw_delete
 * Purpose:
 *     Delete a UP-MEP/DOWNMEP endpoint from hardware table.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p  - (IN) Pointer to endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_sb2_sat_endpoint_hw_delete(int unit, _bcm_sat_hash_data_t *h_data_p)
{

    ing_sat_samp_tcam_entry_t      ing_entry; /* DOWNMEP SAMP TCAM  entry.   */
    ing_sat_samp_data_entry_t      ing_dentry; /* DOWNMEP SAMP Data entry.   */

    egr_sat_samp_tcam_entry_t      egr_entry; /* UPMEP SAMP TCAM  entry.   */
    egr_sat_samp_data_entry_t      egr_dentry; /* UPMEP SAMP Data entry.   */
    egr_sat_samp_data_1_entry_t      egr_d_1entry; /* UPMEP SAMP Data_1 entry.   */

	int  rv = BCM_E_NONE; /* Operation return status.     */

	if (h_data_p->ep_info.flags & BCM_SAT_ENDPOINT_UPMEP) {

        /* Clear UP-MEP table entry for this endpoint index. */
        sal_memset(&egr_entry, 0, sizeof(egr_sat_samp_tcam_entry_t));
        
        rv = WRITE_EGR_SAT_SAMP_TCAMm(unit, MEM_BLOCK_ALL, 
                                      h_data_p->ep_info.ep_id, &egr_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                  "SAT(unit %d) Error: EGR SAT SAMP TCAM write index=%x (EP=%d)"\
                  " - %s.\n"), unit, h_data_p->ep_info.ep_id,
                  h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
            return (rv);
        }
        
        sal_memset(&egr_dentry, 0, sizeof(egr_sat_samp_data_entry_t));
        rv = WRITE_EGR_SAT_SAMP_DATAm(unit, MEM_BLOCK_ALL, 
                                      h_data_p->ep_info.ep_id, &egr_dentry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
             "SAT(unit %d) Error: EGR SAT SAMP Data table write index=%x (EP=%d)"\
                      " - %s.\n"), unit, h_data_p->ep_info.ep_id,
                      h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
            return (rv);
        }
        /* Need to delete the EGR_SAT_SAMP_DATA_1 also */
        sal_memset(&egr_d_1entry, 0, sizeof(egr_sat_samp_data_1_entry_t));
        rv = WRITE_EGR_SAT_SAMP_DATA_1m(unit, MEM_BLOCK_ALL, 
                                        h_data_p->ep_info.ep_id, &egr_d_1entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
             "SAT(unit %d) Error: EGR SAT SAMP Data_1 table write index=%x (EP=%d)"\
                      " - %s.\n"), unit, h_data_p->ep_info.ep_id,
                      h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
            return (rv);
        }

    }
    
    else if (h_data_p->ep_info.flags & BCM_SAT_ENDPOINT_DOWNMEP) {
        
        /* Clear DOWN-MEP table entry for this endpoint index. */
        sal_memset(&ing_entry, 0, sizeof(egr_sat_samp_tcam_entry_t));
        
        rv = WRITE_ING_SAT_SAMP_TCAMm(unit, MEM_BLOCK_ALL, 
                                      h_data_p->ep_info.ep_id, &ing_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
                  "SAT(unit %d) Error: ING SAT SAMP TCAM write index=%x (EP=%d)"\
                  " - %s.\n"), unit, h_data_p->ep_info.ep_id,
                  h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
            return (rv);
        }
        
        sal_memset(&ing_dentry, 0, sizeof(egr_sat_samp_data_entry_t));
        rv = WRITE_ING_SAT_SAMP_DATAm(unit, MEM_BLOCK_ALL, 
                                      h_data_p->ep_info.ep_id, &ing_dentry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT, 
                      (BSL_META_U(unit,
             "SAT(unit %d) Error: ING SAT SAMP Data table write index=%x (EP=%d)"\
                      " - %s.\n"), unit, h_data_p->ep_info.ep_id,
                      h_data_p->ep_info.ep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }
    else
    {
        return (BCM_E_PARAM);
    }
	
	return rv;
}

/*
 * Function:
 *     bcm_sb2_sat_oamp_enable_get
 * Purpose:
 *     Get OAMP enable status
 * Parameters:
 *     unit          - (IN) BCM device number
 *     enable        - (OUT) status.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_sb2_sat_oamp_enable_get(int unit, int *enable)
{
    int     rv = BCM_E_NONE;
    uint32  rval;

    BCM_IF_ERROR_RETURN(READ_OAMP_ENABLEr(unit, &rval));
    *enable = soc_reg_field_get(unit, OAMP_ENABLEr, rval, ENABLEf);

    return rv;
}

/*
 * Function:
 *     bcm_sb2_sat_oamp_enable_set
 * Purpose:
 *     Enable or disable OAMP function
 * Parameters:
 *     unit          - (IN) BCM device number
 *     enable        - (IN) zero - disable; Not zero - enable.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_sb2_sat_oamp_enable_set(int unit, int enable)
{
    int rv = SOC_E_NONE;
    uint32 reg_val;
    uint32 fld_val;
    uint32 mem_entry[SOC_MAX_MEM_WORDS];
    uint32 fld_val_above_64[SOC_MAX_MEM_WORDS];
    int rd_ptr, wr_ptr;
    soc_timeout_t to;
    int timeout_usec;

    if (enable) {
        fld_val = 1;
        rv = READ_OAMP_ENABLEr(unit, &reg_val);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        soc_reg_field_set(unit, OAMP_ENABLEr, &reg_val, ENABLEf, fld_val);
        rv = WRITE_OAMP_ENABLEr(unit, reg_val);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        /* 
         * Setting OLP RX header to DUNE RX header mapping
 */
        rv = READ_EGR_OAMP_CONTROLr(unit, &reg_val);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }
        fld_val = 1;
        soc_reg_field_set(unit, EGR_OAMP_CONTROLr, &reg_val, OAMP_ENCAP_ENABLEf, fld_val);
        fld_val = 0;
        soc_reg_field_set(unit, EGR_OAMP_CONTROLr, &reg_val, TOD_SAMPLE_MODEf, fld_val);
        fld_val = _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER;
        soc_reg_field_set(unit, EGR_OAMP_CONTROLr, &reg_val, OAMP_PHY_PORT_NUMBERf, fld_val);
        fld_val = 0xf;
        soc_reg_field_set(unit, EGR_OAMP_CONTROLr, &reg_val, DOWN_SAT_SUBTYPEf, fld_val);
        fld_val = 0x15;
        soc_reg_field_set(unit, EGR_OAMP_CONTROLr, &reg_val, UP_SAT_SUBTYPEf, fld_val);
        rv = WRITE_EGR_OAMP_CONTROLr(unit, reg_val);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        /* The register has 1 field, the value should be 0x00000000000f000000000000000000170000000000000000000000000000 when SAT is enabled */
        sal_memset(fld_val_above_64, 0, sizeof(fld_val_above_64));
        fld_val_above_64[0] = 0x0;
        fld_val_above_64[1] = 0x0;
        fld_val_above_64[2] = 0x0;
        fld_val_above_64[3] = 0x170000;
        fld_val_above_64[4] = 0x0;
        fld_val_above_64[5] = 0x0;
        fld_val_above_64[6] = 0xf;
        fld_val_above_64[7] = 0x0;
        soc_mem_field_set(unit, EGR_DNX_HEADERm, mem_entry, HDR_DEFAULT_VALUEf, fld_val_above_64); 
        rv = WRITE_EGR_DNX_HEADERm(unit, MEM_BLOCK_ALL, 0, mem_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        /* 
         * Setting OAMP programmable editor 
         * Every SAT packet sent by the SAT engine goes through the programmable editor
         */
        rv = READ_OAMP_PE_INSTr(unit, &reg_val);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        fld_val = 0;
        soc_reg_field_set(unit, OAMP_PE_INSTr, &reg_val, PE_DEFAULT_INSTRUCTIONf, fld_val);
        fld_val = 0x1000;
        soc_reg_field_set(unit, OAMP_PE_INSTr, &reg_val, MAX_INSTR_COUNTf, fld_val);

        rv = WRITE_OAMP_PE_INSTr(unit, reg_val);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        rv = READ_OAMP_PE_0_PROG_TCAMm(unit, MEM_BLOCK_ALL, 0, mem_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        fld_val = 0x0;
        soc_mem_field32_set(unit, OAMP_PE_0_PROG_TCAMm, mem_entry, DATf, fld_val); 
        soc_mem_field32_set(unit, OAMP_PE_0_PROG_TCAMm, mem_entry, KEYf, fld_val); 
        fld_val = 0x7fffff;
        soc_mem_field32_set(unit, OAMP_PE_0_PROG_TCAMm, mem_entry, MASKf, fld_val); 
        fld_val = 0x1;
        soc_mem_field32_set(unit, OAMP_PE_0_PROG_TCAMm, mem_entry, VALIDf, fld_val); 

        rv = WRITE_OAMP_PE_0_PROG_TCAMm(unit, MEM_BLOCK_ALL, 0, mem_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        rv = READ_OAMP_PE_1_PROG_TCAMm(unit, MEM_BLOCK_ALL, 0, mem_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        fld_val = 0x0;
        soc_mem_field32_set(unit, OAMP_PE_1_PROG_TCAMm, mem_entry, DATf, fld_val); 
        soc_mem_field32_set(unit, OAMP_PE_1_PROG_TCAMm, mem_entry, KEYf, fld_val); 
        fld_val = 0x7fffff;
        soc_mem_field32_set(unit, OAMP_PE_1_PROG_TCAMm, mem_entry, MASKf, fld_val); 
        fld_val = 0x1;
        soc_mem_field32_set(unit, OAMP_PE_1_PROG_TCAMm, mem_entry, VALIDf, fld_val); 

        rv = WRITE_OAMP_PE_1_PROG_TCAMm(unit, MEM_BLOCK_ALL, 0, mem_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        rv = READ_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, 0, mem_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }

        fld_val_above_64[0] = 0x7e56a377;
        fld_val_above_64[1] = 0x280008ae;
        fld_val_above_64[2] = 0x7;
        soc_mem_field_set(unit, OAMP_PE_PROGRAMm, mem_entry, PROG_DATAf, fld_val_above_64); 

        rv = WRITE_OAMP_PE_PROGRAMm(unit, MEM_BLOCK_ALL, 0, mem_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rv)));
            return rv;
        }
    } else {
        /* OAMP disable */
        /* 1. Disable redirection to OAMP and DNX header encap */
        SOC_IF_ERROR_RETURN(READ_EGR_OAMP_CONTROLr(unit, &reg_val));
        soc_reg_field_set(unit, EGR_OAMP_CONTROLr, &reg_val, OAMP_ENCAP_ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_EGR_OAMP_CONTROLr(unit, reg_val));

        /* 2. Wait for SAT TX and RX flush */
        if (SAL_BOOT_SIMULATION) {
            timeout_usec = 10000000; /* Simulation - 10sec */
        } else {
            timeout_usec = 50000;
        }

        soc_timeout_init(&to, timeout_usec, 0);
        do {
            SOC_IF_ERROR_RETURN(READ_IARB_CELL_DEBUGr(unit, &reg_val));
            rd_ptr = soc_reg_field_get(unit, IARB_CELL_DEBUGr, reg_val,
                    FIFO_RD_PTRf);
            wr_ptr = soc_reg_field_get(unit, IARB_CELL_DEBUGr, reg_val,
                    FIFO_WR_PTRf);
            if (rd_ptr == wr_ptr) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_BCM_SAT,
                        (BSL_META_U(unit,"SAT TX Flush Failed\n")));
                return SOC_E_TIMEOUT;
            }
        }while (TRUE);

        soc_timeout_init(&to, timeout_usec, 0);
        do {
            SOC_IF_ERROR_RETURN(READ_EGR_EDBIL_STATUSr(unit, &reg_val));
            rd_ptr = soc_reg_field_get(unit, EGR_EDBIL_STATUSr, reg_val,
                    READ_PTRf);
            wr_ptr = soc_reg_field_get(unit, EGR_EDBIL_STATUSr, reg_val,
                    WR_PTRf);

            /* Check : READ_PTR[4:2] == WR_PTR && READ_PTR[0:1] == 0 */
            if ((((rd_ptr & 0x1C) >> 2) == wr_ptr) && ((rd_ptr & 0x3) == 0)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_BCM_SAT,
                        (BSL_META_U(unit,"SAT RX Flush Failed\n")));
                return SOC_E_TIMEOUT;
            }
        }while (TRUE);

        /* 3. Disable OAMP */
        SOC_IF_ERROR_RETURN(READ_OAMP_ENABLEr(unit, &reg_val));
        soc_reg_field_set(unit, OAMP_ENABLEr, &reg_val, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OAMP_ENABLEr(unit, reg_val));
    }
    /* 
     * Setting OAMP programmable editor 
     * Every SAT packet sent by the SAT engine goes through the programmable editor
     */

    return (rv);
}

/*
 * Function:
 *     bcm_sb2_sat_tod_set
 * Purpose:
 *     Set RX timestamp format
 * Parameters:
 *     unit          - (IN) BCM device number
 *     ts_format     - (IN) NTP - bcmSATTimestampFormatNTP; 1588 - bcmSATTimestampFormatIEEE1588v1.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_sb2_sat_ts_format_set(int unit, int ts_format)
{
    int rv = SOC_E_NONE;
    uint32 reg_val;
    uint32 fld_val;

    /*
     * TOD_SAMPLE_MODE - value "1" (PTP) or "2" (NTP). 
     */

    fld_val = (ts_format == bcmSATTimestampFormatNTP ? 2 : 1);
    rv = READ_EGR_OAMP_CONTROLr(unit, &reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    soc_reg_field_set(unit, EGR_OAMP_CONTROLr, &reg_val, TOD_SAMPLE_MODEf, fld_val);
    rv = WRITE_EGR_OAMP_CONTROLr(unit, reg_val);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rv)));
        return rv;
    }

    return (rv);
}

/*
 * Function:
 *     bcm_sb2_sat_ctf_report_process
 * Purpose:
 *     Process CTF report message and call registered function
 * Parameters:
 *     unit   - (IN) BCM device number
 *     cb     - (IN) Registered callback function
 *     cookie - (IN) User data
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_sb2_sat_ctf_report_process(int unit, bcm_sat_event_cb cb, void *cookie)
{
    uint32 msg_start_bit = 0, msg_word = 0;
    uint32 empty_fifo_value = 0xffffffff;
    uint32 field_32;
    soc_reg_above_64_val_t data;
    soc_reg_above_64_val_t msg;
    bcm_sat_report_event_data_t report;
    bcm_sat_event_type_t type = bcmSATEventReport;

    /* Process report message */
    BCM_IF_ERROR_RETURN(READ_OAMP_STAT_INTERRUPT_MESSAGEr(unit, data));
    soc_reg_above_64_field_get(unit, OAMP_STAT_INTERRUPT_MESSAGEr, data,
                               STAT_INTERRUPT_MESSAGEf, msg);
    while (msg[msg_word] != empty_fifo_value) {
        msg_start_bit = msg_word * 32;
        field_32 = 0;
        SHR_BITCOPY_RANGE(&field_32, 0, msg, msg_start_bit, 5);
        report.ctf_id = field_32;
        field_32 = 0;
        SHR_BITCOPY_RANGE(&field_32, 0, msg, msg_start_bit+14, 1);
        if (field_32) {
            field_32 = 0;
            SHR_BITCOPY_RANGE(&field_32, 0, msg, msg_start_bit+32, 32);
            report.delay = field_32;
            report.delay_valid = 1;
        } else {
            report.delay_valid = 0;
        }
        field_32 = 0;
        SHR_BITCOPY_RANGE(&field_32, 0, msg, msg_start_bit+15, 1);
        if (field_32) {
            field_32 = 0;
            if (report.delay_valid) {
                SHR_BITCOPY_RANGE(&field_32, 0, msg, msg_start_bit+64, 32);
            } else {
                SHR_BITCOPY_RANGE(&field_32, 0, msg, msg_start_bit+32, 32);
            }
            report.sequence_number = field_32;
            report.sequence_number_valid = 1;
        } else {
            report.sequence_number_valid = 0;
        }
        field_32 = 0;
        SHR_BITCOPY_RANGE(&field_32, 0, msg, msg_start_bit+16, 4);
        msg_word += field_32;

        if (cb != NULL) {
            (void)cb(unit, type, (void *)&report, cookie);
        }
    }

    return BCM_E_NONE;
}

int bcm_sb2_sat_init_check(int unit)
{
    /* Check whether the SAT module is initialized */
    _BCM_SAT_IS_INIT(unit);

    return SOC_E_NONE;

}

#endif /* BCM_SABER2_SUPPORT */
