 /* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *  oam.c
 *
 * Purpose:
 *  OAM implementation for Triumph3 family of devices.
 */
#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/hash.h>
#include <soc/l3x.h>
#include <soc/katana2.h>
#include <soc/ism_hash.h>

#include <bcm/l3.h>
#include <bcm/oam.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/subport.h>

#if defined(INCLUDE_BHH)
#include <bcm_int/esw/bhh.h>
#include <bcm_int/esw/bhh_sdk_pack.h>
#include <soc/uc.h>
#include <soc/shared/oam_pm_shared.h>
#include <soc/shared/oam_pm_pack.h>
#endif /* INCLUDE_BHH */
/*
 * Device OAM control structure.
 */
_bcm_oam_control_t *_kt2_oam_control[SOC_MAX_NUM_DEVICES];

/* * * * * * * * * * * * * * * * * * *
 *            OAM MACROS             *
 */
/*
 * Macro:
 *     _BCM_OAM_IS_INIT (internal)
 * Purpose:
 *     Check that the unit is valid and confirm that the oam functions
 *     are initialized.
 * Parameters:
 *     unit - BCM device number
 * Notes:
 *     Results in return(BCM_E_UNIT), return(BCM_E_UNAVAIL), or
 *     return(BCM_E_INIT) if fails.
 */
#define _BCM_OAM_IS_INIT(unit)                                               \
            do {                                                             \
                if (!soc_feature(unit, soc_feature_oam)) {                   \
                    return (BCM_E_UNAVAIL);                                  \
                }                                                            \
                if (_kt2_oam_control[unit] == NULL) {                        \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META_U(unit, \
                                          "OAM Error: Module not initialized\n"))); \
                    return (BCM_E_INIT);                                     \
                }                                                            \
            } while (0)

/*
 *Macro:
 *     _BCM_OAM_LOCK
 * Purpose:
 *     Lock take the OAM control mutex
 * Parameters:
 *     control - Pointer to OAM control structure.
 */
#define _BCM_OAM_LOCK(control) \
            sal_mutex_take((control)->oc_lock, sal_mutex_FOREVER)

/*
 * Macro:
 *     _BCM_OAM_UNLOCK
 * Purpose:
 *     Lock take the OAM control mutex
 * Parameters:
 *     control - Pointer to OAM control structure.
 */
#define _BCM_OAM_UNLOCK(control) \
            sal_mutex_give((control)->oc_lock);
/*
 * Macro:
 *     _BCM_OAM_HASH_DATA_CLEAR
 * Purpose:
 *      Clear hash data memory occupied by one endpoint.
 * Parameters:
 *     _ptr_    - Pointer to endpoint hash data memory. 
 */
#define _BCM_OAM_HASH_DATA_CLEAR(_ptr_) \
            sal_memset(_ptr_, 0, sizeof(_bcm_oam_hash_data_t));

/*
 * Macro:
 *     _BCM_OAM_HASH_DATA_HW_IDX_INIT
 * Purpose:
 *     Initialize hardware indices to invalid index for an endpoint hash data.
 * Parameters:
 *     _ptr_    - Pointer to endpoint hash data memory. 
 */
#define _BCM_OAM_HASH_DATA_HW_IDX_INIT(_ptr_)                             \
            do {                                                          \
                    (_ptr_)->group_index = (_BCM_OAM_INVALID_INDEX);      \
                    (_ptr_)->remote_index = (_BCM_OAM_INVALID_INDEX);     \
                    (_ptr_)->profile_index = (_BCM_OAM_INVALID_INDEX);    \
                    (_ptr_)->pri_map_index = (_BCM_OAM_INVALID_INDEX);    \
                    (_ptr_)->lm_counter_index = (_BCM_OAM_INVALID_INDEX); \
                    (_ptr_)->local_tx_index = (_BCM_OAM_INVALID_INDEX);   \
                    (_ptr_)->local_rx_index = (_BCM_OAM_INVALID_INDEX);   \
            } while (0)
/*
 * Macro:
 *     _BCM_OAM_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_OAM_ALLOC(_ptr_,_ptype_,_size_,_descr_)                     \
            do {                                                         \
                if (NULL == (_ptr_)) {                                   \
                   (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                }                                                        \
                if((_ptr_) != NULL) {                                    \
                    sal_memset((_ptr_), 0, (_size_));                    \
                }  else {                                                \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META("OAM Error: Allocation failure %s\n"), \
                               (_descr_)));                              \
                }                                                        \
            } while (0)

/*
 * Macro:
 *     _BCM_OAM_GROUP_INDEX_VALIDATE
 * Purpose:
 *     Validate OAM Group ID value.
 * Parameters:
 *     _group_ - Group ID value.
 */
#define _BCM_OAM_GROUP_INDEX_VALIDATE(_group_)                               \
            do {                                                             \
                if ((_group_) < 0 || (_group_) >= oc->group_count) {         \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META("OAM Error: Invalid Group ID = %d.\n"), \
                               _group_));                              \
                    return (BCM_E_PARAM);                                    \
                }                                                            \
            } while (0);

/*
 * Macro:
 *     _BCM_OAM_EP_INDEX_VALIDATE
 * Purpose:
 *     Validate OAM Endpoint ID value.
 * Parameters:
 *     _ep_ - Endpoint ID value.
 */
#define _BCM_OAM_EP_INDEX_VALIDATE(_ep_)                               \
            do {                                                       \
                if ((_ep_) < 0 || (_ep_) >= oc->ep_count) {            \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META("OAM Error: Invalid Endpoint ID" \
                               " = %d.\n"), _ep_));              \
                    return (BCM_E_PARAM);                              \
                }                                                      \
            } while (0);


/*
 * Macro:
 *     _BCM_OAM_RMEP_INDEX_VALIDATE
 * Purpose:
 *     Validate OAM Endpoint ID value.
 * Parameters:
 *     _ep_ - Endpoint ID value.
 */
#define _BCM_OAM_RMEP_INDEX_VALIDATE(_ep_)                              \
            do {                                                        \
                if ((_ep_) < 0 || (_ep_) >= oc->rmep_count) {           \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META("OAM Error: Invalid RMEP Index" \
                               " = %d.\n"), _ep_));               \
                    return (BCM_E_PARAM);                               \
                }                                                       \
            } while (0);


/*
 * Macro:
 *     _BCM_OAM_KEY_PACK
 * Purpose:
 *     Pack the hash table look up key fields.
 * Parameters:
 *     _dest_ - Hash key buffer.
 *     _src_  - Hash key field to be packed.
 *     _size_ - Hash key field size in bytes.
 */
#define _BCM_OAM_KEY_PACK(_dest_,_src_,_size_)            \
            do {                                          \
                sal_memcpy((_dest_), (_src_), (_size_));  \
                (_dest_) += (_size_);                     \
            } while (0)
/*
 * Macro:
 *     _BCM_KT2_OAM_MOD_PORT_TO_GLP
 * Purpose:
 *     Construct hadware GLP value from module ID, port ID and Trunk ID value.
 * Parameters:
 *     _modid_ - Module ID.
 *     _port_  - Port ID.
 *     _trunk_ - Trunk (1 - TRUE/0 - FALSE).
 *     _tgid_  - Trunk ID.
 */
#define _BCM_KT2_OAM_MOD_PORT_TO_GLP(_u_, _m_, _p_, _t_, _tgid_, _glp_)     \
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
        LOG_DEBUG(BSL_LS_BCM_OAM,                                           \
                  (BSL_META("u:%d m:%d p:%d t:%d tgid:%d glp:%x\n"),        \
                   _u_, _m_, _p_, _t_, _tgid_, _glp_));                     \
    } while (0)

/*
 * Macro:
 *     _BCM_OAM_GLP_XXX
 * Purpose:
 *     Get components of generic logical port value.
 * Parameters:
 *     _glp_ - Generic logical port.
 */
#define _BCM_OAM_GLP_TRUNK_BIT_GET(_glp_) (0x1 & ((_glp_) >> 15))
#define _BCM_OAM_GLP_TRUNK_ID_GET(_glp_)  (0xFF & (_glp_))
#define _BCM_OAM_GLP_MODULE_ID_GET(_glp_) (0xFF & ((_glp_) >> 7))
#define _BCM_OAM_GLP_PORT_GET(_glp_)      (0x7F & (_glp_))

/*
 * Macro:
 *     _BCM_OAM_EP_LEVEL_XXX
 * Purpose:
 *     Maintenance domain level bit count and level max value.   
 * Parameters:
 *     _glp_ - Generic logical port.
 */
#define _BCM_OAM_EP_LEVEL_COUNT (1 << (_BCM_OAM_EP_LEVEL_BIT_COUNT))
#define _BCM_OAM_EP_LEVEL_MAX (_BCM_OAM_EP_LEVEL_COUNT - 1)
#define BHH_EP_TYPE(ep) ( (ep->type == bcmOAMEndpointTypeBHHMPLS) ||        \
                          (ep->type == bcmOAMEndpointTypeBHHMPLSVccv) || \
                          (ep->type == bcmOAMEndpointTypeBhhSection)||\
                          (ep->type == bcmOAMEndpointTypeBhhSectionInnervlan)||\
                          (ep->type == bcmOAMEndpointTypeBhhSectionOuterVlan)||\
                          (ep->type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)\
                        )

#define BHH_EP_MPLS_SECTION_TYPE(ep) (\
                                     (ep->type == bcmOAMEndpointTypeBhhSection)||\
                                     (ep->type == bcmOAMEndpointTypeBhhSectionInnervlan)||\
                                     (ep->type == bcmOAMEndpointTypeBhhSectionOuterVlan)||\
                                     (ep->type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)\
                                    )


#define ETH_TYPE(type) (type == bcmOAMEndpointTypeEthernet)

/* Checks hash_data pointer's int_flags field */
#define _BCM_OAM_EP_IS_VP_TYPE(h_data_p) (h_data_p->int_flags &\
                                     _BCM_OAM_ENDPOINT_IS_VP_BASED)
#define _BCM_OAM_EP_IS_MIP(info) (info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE)

/* The rmep entry sync info for wb recovery */
typedef struct _kt2_oam_rmep_entry_sync_info_s {
    bcm_oam_endpoint_t  ep_id;      /* Endpoint ID                   */
    uint8               level;      /* This is equivalent to the Maintenance
                                       Domain Level (MDL) in 802.1ag. */
    bcm_vlan_t          vlan;       /* The VLAN associated with this
                                       endpoint */
    bcm_gport_t         gport;      /* The gport associated with this
                                       endpoint */
} _kt2_oam_rmep_entry_sync_info_t;

/*
 * Macro:
 *     BCM_WB_XXX
 * Purpose:
 *    OAM module scache version information.
 * Parameters:
 *    (major number, minor number)
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1      SOC_SCACHE_VERSION(1,1)
/* This new version incorporates new MA_INDEX allocation scheme.
 * Recovery of Endpoints allocated in new scheme will be
 * done for this version or higher only */
#define BCM_WB_VERSION_1_2      SOC_SCACHE_VERSION(1,2)

/* This new version saves the SW endpoint ids for OAM. 
 * Recovery of Endpoints allocated in new scheme will be
 * done for this version or higher only */

#define BCM_WB_VERSION_1_3      SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4      SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5      SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6      SOC_SCACHE_VERSION(1,6)
#define BCM_WB_DEFAULT_VERSION  BCM_WB_VERSION_1_6
#endif

#define _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_TBL_SIZE \
        (sizeof(bcm_oam_endpoint_t) * (oc->ma_idx_count + oc->egr_ma_idx_count))

#define _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_DOWNMEP_IDX(ma_base_index, ma_offset)\
        (ma_base_index + ma_offset)
#define _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_UPMEP_IDX(ma_base_index, ma_offset)\
        (oc->ma_idx_count + ma_base_index + ma_offset)

#define _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_IDX(ma_base_index, ma_offset, is_upmep)\
        ((is_upmep)?\
        _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_UPMEP_IDX(ma_base_index, ma_offset)\
        :_BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_DOWNMEP_IDX(ma_base_index, ma_offset))

/* Macros for endpoint id locations for different endpoint types. */
#define _BCM_KT2_OAM_ENDPOINT_ID_SCACHE_SIZE \
        (sizeof(bcm_oam_endpoint_t) * (oc->rmep_count + oc->lmep_count + oc->ma_idx_count + oc->egr_ma_idx_count))

#define _BCM_KT2_OAM_RX_DOWNMEP_SCACHE_LOCATION(mep_index) \
                (sizeof(bcm_oam_endpoint_t) * mep_index)
#define _BCM_KT2_OAM_RX_UPMEP_SCACHE_LOCATION(mep_index) \
                (sizeof(bcm_oam_endpoint_t) * (oc->ma_idx_count + mep_index))
#define _BCM_KT2_OAM_RMEP_SCACHE_LOCATION(rmep_index) \
                (sizeof(bcm_oam_endpoint_t) * (oc->ma_idx_count + oc->egr_ma_idx_count + rmep_index))
#define _BCM_KT2_OAM_TX_MEP_SCACHE_LOCATION(mep_index) \
                (sizeof(bcm_oam_endpoint_t) * (oc->ma_idx_count + oc->egr_ma_idx_count + oc->rmep_count + mep_index))
#define _BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_SIZE \
                (sizeof(_kt2_oam_rmep_entry_sync_info_t) * (oc->rmep_count))
#define _BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_LOCATION(rmep_index) \
                (sizeof(_kt2_oam_rmep_entry_sync_info_t) * ( rmep_index))
/* Vlans will be programmed in ING/EGR MP_GROUP table depending on MEP_TYPE */
#define _BCM_KT2_ETH_OAM_MP_GROUP_KEY_DOMAIN_SPECIFIC    0
/* Inner & Outer Vlan will be always programmed in ING/EGR MP_GROUP table */
#define _BCM_KT2_ETH_OAM_MP_GROUP_KEY_DOMAIN_INDEPENDANT 1

/* Defined for OAM opcode group 1 and 2 */
#define _BCM_OAM_OTHER_OPCODE_GROUP_1    0x1
#define _BCM_OAM_OTHER_OPCODE_GROUP_2    0x2
/* Define the port used in OLP-XGS communication */
#define _BCM_OAM_OLP_COMMUNICATION_PORT  0x7f

/*
 * Device OAM control structure.
 */
_bcm_oam_control_t *_kt2_oam_control[SOC_MAX_NUM_DEVICES];

#if defined (INCLUDE_BHH)

typedef struct _bcm_oam_pm_profile_int_info_s {
    int id_status;
    bcm_oam_pm_profile_info_t pm_profile;
}_bcm_oam_pm_profile_int_info_t;
typedef struct _bcm_oam_pm_profile_control_s {
    _bcm_oam_pm_profile_int_info_t profile_info[_BCM_OAM_MAX_PM_PROFILES];
}_bcm_oam_pm_profile_control_t;


_bcm_oam_pm_profile_control_t *kt2_pm_profile_control[SOC_MAX_NUM_DEVICES];

#define _BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, id)          (pmc->profile_info[id].id_status == 1)
#define _BCM_KT2_PM_CTRL_PROFILE_PTR(pmc, id)             (&(pmc->profile_info[id].pm_profile))
#define _BCM_KT2_SET_PM_CTRL_PROFILE_IN_USE(pmc, id)      (pmc->profile_info[id].id_status = 1)
#define _BCM_KT2_SET_PM_CTRL_PROFILE_NOT_IN_USE(pmc, id)  (pmc->profile_info[id].id_status = 0)

#define _BCM_KT2_PM_PROFILE_REPLACE_FLAG_SET(profile) (profile->flags & BCM_OAM_PM_PROFILE_REPLACE)

#define _BCM_KT2_PM_PROFILE_WITH_ID_FLAG_SET(profile) (profile->flags & BCM_OAM_PM_PROFILE_WITH_ID)

#define _BCM_KT2_PM_PROFILE_ID_VALID(profile_id) \
    ((profile_id >= 0) && (profile_id < _BCM_OAM_MAX_PM_PROFILES))

#define _BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc) \
                            (oc->pm_bhh_lmdm_data_collection_mode != \
                                _BCM_OAM_PM_COLLECTION_MODE_NONE)
#define _BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_RAW_DATA(oc) \
                            (oc->pm_bhh_lmdm_data_collection_mode == \
                                _BCM_OAM_PM_COLLECTION_MODE_RAW_DATA)
#define _BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc) \
                            (oc->pm_bhh_lmdm_data_collection_mode == \
                                _BCM_OAM_PM_COLLECTION_MODE_PROCESSED_STATS)
static int kt2_oam_mpls_tp_ach_channel_type = SHR_BHH_ACH_CHANNEL_TYPE;
#else
#define _BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc) 0
#define _BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_RAW_DATA(oc) 0
#define _BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc) 0
#endif


typedef struct _oam_tpid_s {
    uint32                       tpid;
    uint32                       ref_count;
} _oam_tpid_t;

#define BCM_MAX_TPID_ENTRIES 4
#define BCM_MAX_INNER_TPID_ENTRIES 1


#define BCM_OAM_TPID_TYPE_OUTER   0
#define BCM_OAM_TPID_TYPE_INNER   1
#define BCM_OAM_TPID_TYPE_SUBPORT 2
#define BCM_OAM_TPID_VALUE_ZERO   0
#define BCM_OAM_DEFAULT_TPID      0x8100
#define BCM_OAM_TPID_9100         0x9100
#define BCM_OAM_TPID_88A8         0x88A8
#define DGLP_LAG_ID_INDICATOR_SHIFT_BITS   15
#define DGLP_MODULE_ID_SHIFT_BITS          7
#define _BCM_OAM_OPCODE_TYPE_CCM           0x1
#define _BCM_OAM_OPCODE_TYPE_LBR           0x2
#define _BCM_OAM_OPCODE_TYPE_LBM           0x3
#define _BCM_OAM_OPCODE_TYPE_LTR           0x4
#define _BCM_OAM_OPCODE_TYPE_LTM           0x5
#define _BCM_OAM_OPCODE_TYPE_NON_CFM_FIRST 0x6
#define _BCM_OAM_OPCODE_TYPE_NON_CFM_LAST  0xFF
#define _BCM_OAM_OPCODE_TYPE_CFM_MASK      0x3E
#define _BCM_OAM_OPCODE_TYPE_NON_CFM_MASK  0xFFFFFFFF
#define _BCM_OAM_DGLP1_PROFILE_PTR         0x1
#define _BCM_OAM_DGLP2_PROFILE_PTR         0x2
#define _BCM_OAM_LOW_MDL_DROP_PACKET       0x2
#define _BCM_OAM_FWD_AS_DATA               0x0
#define _BCM_OAM_DROP_REDIRECT_T0_DGLP1    0x2
#define _BCM_OAM_DROP_REDIRECT_T0_DGLP2    0x3
#define _BCM_OAM_SERVICE_PRI_MAX_OFFSET    0x7

STATIC _oam_tpid_t
           (*_oam_outer_tpid_tab[BCM_MAX_NUM_UNITS])[BCM_MAX_TPID_ENTRIES];

STATIC _oam_tpid_t
           (*_oam_inner_tpid_tab[BCM_MAX_NUM_UNITS])[BCM_MAX_INNER_TPID_ENTRIES];

STATIC _oam_tpid_t
           (*_oam_subport_tpid_tab[BCM_MAX_NUM_UNITS])[BCM_MAX_TPID_ENTRIES];

STATIC sal_mutex_t _kt2_outer_tpid_lock[BCM_MAX_NUM_UNITS];
STATIC sal_mutex_t _kt2_inner_tpid_lock[BCM_MAX_NUM_UNITS];
STATIC sal_mutex_t _kt2_subport_tpid_lock[BCM_MAX_NUM_UNITS];

/* OAM TPID registers and default values */
STATIC int outer_tpid[4] = { OUTER_TPID_0r, OUTER_TPID_1r, 
                             OUTER_TPID_2r, OUTER_TPID_3r };

STATIC int default_outer_tpid[4] = { BCM_OAM_DEFAULT_TPID, BCM_OAM_TPID_9100, 
                                  BCM_OAM_TPID_88A8, BCM_OAM_TPID_VALUE_ZERO};

STATIC int subport_tpid[4] = { SUBPORT_TAG_TPID_0r, SUBPORT_TAG_TPID_1r, 
                               SUBPORT_TAG_TPID_2r, SUBPORT_TAG_TPID_3r };

#define KT2_OAM_OUTER_TPID_TAB(unit) _oam_outer_tpid_tab[unit]
#define KT2_OAM_INNER_TPID_TAB(unit) _oam_inner_tpid_tab[unit]
#define KT2_OAM_SUBPORT_TPID_TAB(unit) _oam_subport_tpid_tab[unit]

#define KT2_OAM_OUTER_TPID_ENTRY(unit, index) \
    ((*_oam_outer_tpid_tab[unit])[index].tpid)

#define KT2_OAM_INNER_TPID_ENTRY(unit, index) \
    ((*_oam_inner_tpid_tab[unit])[index].tpid)

#define KT2_OAM_SUBPORT_TPID_ENTRY(unit, index) \
    ((*_oam_subport_tpid_tab[unit])[index].tpid)

#define KT2_OAM_OUTER_TPID_REF_COUNT(unit, index) \
    ((*_oam_outer_tpid_tab[unit])[index].ref_count)

#define KT2_OAM_INNER_TPID_REF_COUNT(unit, index) \
    ((*_oam_inner_tpid_tab[unit])[index].ref_count)

#define KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, index) \
    ((*_oam_subport_tpid_tab[unit])[index].ref_count)

#define KT2_OAM_OUTER_TPID_TAB_INIT_CHECK(unit) \
    if (KT2_OAM_OUTER_TPID_TAB(unit) == NULL) { return BCM_E_INIT; }

#define KT2_OAM_INNER_TPID_TAB_INIT_CHECK(unit) \
    if (KT2_OAM_INNER_TPID_TAB(unit) == NULL) { return BCM_E_INIT; }

#define KT2_OAM_SUBPORT_TPID_TAB_INIT_CHECK(unit) \
    if (KT2_OAM_SUBPORT_TPID_TAB(unit) == NULL) { return BCM_E_INIT; }

#define BCM_KT2_OUTER_TPID_MUTEX(_u_) _kt2_outer_tpid_lock[_u_]
#define BCM_KT2_INNER_TPID_MUTEX(_u_) _kt2_inner_tpid_lock[_u_]
#define BCM_KT2_SUBPORT_TPID_MUTEX(_u_) _kt2_subport_tpid_lock[_u_]

#define BCM_KT2_OUTER_TPID_LOCK(_u_)   \
         ((_kt2_outer_tpid_lock[_u_]) ? \
         sal_mutex_take(_kt2_outer_tpid_lock[_u_], sal_mutex_FOREVER) :  \
         (BCM_E_INTERNAL))

#define BCM_KT2_INNER_TPID_LOCK(_u_)   \
         ((_kt2_inner_tpid_lock[_u_]) ? \
         sal_mutex_take(_kt2_inner_tpid_lock[_u_], sal_mutex_FOREVER) :  \
         (BCM_E_INTERNAL))

#define BCM_KT2_SUBPORT_TPID_LOCK(_u_)   \
         ((_kt2_subport_tpid_lock[_u_]) ? \
         sal_mutex_take(_kt2_subport_tpid_lock[_u_], sal_mutex_FOREVER) :  \
         (BCM_E_INTERNAL))

#define BCM_KT2_OUTER_TPID_UNLOCK(_u_)  \
         ((_kt2_outer_tpid_lock[_u_]) ? \
         sal_mutex_give(_kt2_outer_tpid_lock[_u_]) : \
         (BCM_E_INTERNAL))

#define BCM_KT2_INNER_TPID_UNLOCK(_u_)  \
         ((_kt2_inner_tpid_lock[_u_]) ? \
         sal_mutex_give(_kt2_inner_tpid_lock[_u_]) : \
         (BCM_E_INTERNAL))

#define BCM_KT2_SUBPORT_TPID_UNLOCK(_u_)  \
         ((_kt2_subport_tpid_lock[_u_]) ? \
         sal_mutex_give(_kt2_subport_tpid_lock[_u_]) : \
         (BCM_E_INTERNAL))


/*
 * ==========================================================================
 *  Downmep/IFP      3'b000                8'd1(L2_HDR)      8'd0(NULL)
 *  Downmep/IFP      3'b001                8'd0(OAM_HDR)     8'd2(CCM/BHH-CCM)
 *  Downmep/IFP      3'b010                8'd0(OAM_HDR)     8'd3(BFD)
 *  Downmep/IFP      3'b011                8'd0(OAM_HDR)     8'd4(LM/DM)
 *  Upmep            3'b000                N/A               N/A
 *  Upmep            3'b001                8'd0(OAM_HDR)     8'd5(CCM)
 *  Upmep            3'b010                N/A               N/A
 *  Upmep            3'b011                8'd0(OAM_HDR)     8'd6(LM/DM)
 */
#define _BCM_KT2_OLP_HDR_SUBTYPE_CCM                   0x2
#define _BCM_KT2_OLP_HDR_SUBTYPE_BFD                   0x3
#define _BCM_KT2_OLP_HDR_SUBTYPE_LM_DM                 0x4
#define _BCM_KT2_OLP_HDR_SUBTYPE_ETH_OAM_UPMEP_CCM     0x5
#define _BCM_KT2_OLP_HDR_SUBTYPE_ETH_OAM_UPMEP_LM_DM   0x6

#define _BCM_KT2_NO_OF_BITS_COMP_HDR 3
#define _BCM_KT2_UP_MEP_OLP_HDR_TYPE_COMPRESSED_GET(comp_hdr) \
             ((1 << _BCM_KT2_NO_OF_BITS_COMP_HDR) | comp_hdr)


#define _BCM_KT2_CCM_OLP_HDR_TYPE_COMPRESSED                  0x1
#define _BCM_KT2_BFD_OLP_HDR_TYPE_COMPRESSED                  0x2
#define _BCM_KT2_LM_DM_OLP_HDR_TYPE_COMPRESSED                0x3

#define _BCM_KT2_ETH_OAM_UP_MEP_CCM_OLP_HDR_TYPE_COMPRESSED           \
    _BCM_KT2_UP_MEP_OLP_HDR_TYPE_COMPRESSED_GET(                      \
                               _BCM_KT2_CCM_OLP_HDR_TYPE_COMPRESSED)

#define _BCM_KT2_ETH_OAM_UP_MEP_LM_DM_OLP_HDR_TYPE_COMPRESSED         \
    _BCM_KT2_UP_MEP_OLP_HDR_TYPE_COMPRESSED_GET(                      \
                             _BCM_KT2_LM_DM_OLP_HDR_TYPE_COMPRESSED)

#define _BCM_KT2_OLP_OAM_HDR  0
#define _BCM_KT2_OLP_L2_HDR   1

/* No. of counter pool that can be programmed per endpoint */
#define _KT2_OAM_COUNTER_SIZE 2

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *         OAM function prototypes               *
 */

int _bcm_kt2_oam_convert_action_to_opcode_entry(
                                        int unit,
                                        bcm_oam_endpoint_action_t *action, 
                                        bcm_oam_endpoint_t ep_id,
                                        void *profile,
                                        uint8  *opcode_profile_changed); 
STATIC int
_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(int unit,
                       _bcm_oam_hash_data_t *h_data_p,
                              int *num_ma_idx_entries,
                       shr_idxres_list_handle_t *pool);
STATIC int
_bcm_kt2_oam_ma_idx_pool_create(_bcm_oam_control_t *oc);
STATIC int
_bcm_kt2_oam_ma_idx_pool_destroy(_bcm_oam_control_t *oc);

STATIC int
_bcm_kt2_oam_control_get(int unit, _bcm_oam_control_t **oc);

STATIC INLINE bcm_oam_endpoint_t
_bcm_kt2_oam_ma_idx_to_ep_id_mapping_get (_bcm_oam_control_t   *oc, int ma_idx);



/* * * * * * * * * * * * * * * * * * * * * * * * *
 *         OAM global data initialization      *
 */
#if defined(INCLUDE_BHH)

#define BHH_COSQ_INVALID          0xFFFF

/*
 * Macro:
 *     _BCM_OAM_BHH_IS_VALID (internal)
 * Purpose:
 *     Check that the BHH feature is available on this unit
 * Parameters:
 *     unit - BCM device number
 * Notes:
 *     Results in return(BCM_E_UNAVAIL),
 */
#define _BCM_OAM_BHH_IS_VALID(unit)                                          \
            do {                                                             \
                if (!soc_feature(unit, soc_feature_bhh)) {                   \
                    return (BCM_E_UNAVAIL);                                  \
                }                                                            \
            } while (0)

#define BCM_OAM_BHH_GET_UKERNEL_EP(ep) \
        (ep - _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET)

#define BCM_OAM_BHH_GET_SDK_EP(ep) \
        (ep + _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET)

#define BCM_OAM_BHH_VALIDATE_EP(_ep_) \
            do {                                                       \
                if (((_ep_) < _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET) ||         \
                    ((_ep_) >= (_BCM_OAM_BHH_KT2_ENDPOINT_OFFSET           \
                                         + oc->bhh_endpoint_count))) { \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META_U(unit, \
                                          "OAM Error: Invalid Endpoint ID" \
                                          " = %d.\n"), _ep_));   \
                    _BCM_OAM_UNLOCK(oc);                               \
                    return (BCM_E_PARAM);                              \
                }                                                      \
            } while (0);

#define BCM_OAM_BHH_ABS(x)   (((x) < 0) ? (-(x)) : (x))


STATIC int
_bcm_kt2_oam_bhh_session_hw_delete(int unit, _bcm_oam_hash_data_t *h_data_p);
STATIC int
_bcm_kt2_oam_bhh_msg_send_receive(int unit, uint8 s_subclass,
                                    uint16 s_len, uint32 s_data,
                                    uint8 r_subclass, uint16 *r_len);
STATIC int
_bcm_kt2_oam_bhh_hw_init(int unit);
STATIC void
_bcm_kt2_oam_bhh_callback_thread(void *param);
STATIC int
_bcm_kt2_oam_bhh_event_mask_set(int unit);
STATIC int
bcm_kt2_oam_bhh_endpoint_create(int unit,
                                bcm_oam_endpoint_info_t *endpoint_info,
                                _bcm_oam_hash_key_t  *hash_key);
STATIC int
_bcm_kt2_oam_bhh_sec_mep_alloc_counter(int unit, shr_idxres_element_t *ctr_index);

STATIC int
_bcm_kt2_oam_pm_msg_send_receive(int unit, uint8 s_class, uint8 s_subclass,
                                    uint16 s_len, uint32 s_data,
                                    uint8 r_subclass, uint16 *r_len);

STATIC void _bcm_kt2_oam_pm_event_msg_handle(_bcm_oam_control_t *oc,
                                             mos_msg_data_t *event_msg);
int _bcm_kt2_oam_pm_init(int unit);
#endif  /* INCLUDE_BHH */

STATIC int
bcm_kt2_oam_hw_ccm_tx_ctr_update(int unit,
        bcm_oam_endpoint_info_t *ep_info);
/*
 * Katana2 device OAM CCM intervals array initialization..
 */
STATIC uint32 _kt2_ccm_intervals[] =
{
    BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED,
    BCM_OAM_ENDPOINT_CCM_PERIOD_3MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_100MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_1S,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10S,
    BCM_OAM_ENDPOINT_CCM_PERIOD_1M,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10M,
    _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED
};

/*   
 * OAM hardware interrupts to software events mapping array initialization.
 * _kt2_oam_interrupts[] =
 *     {Interrupt status register, Remote MEP index field, MA index field,
 *          CCM_INTERRUPT_CONTROLr - Interrupt status Field,
 *          OAM event type}. 
 */
STATIC _bcm_oam_interrupt_t _kt2_oam_interrupts[] =
{
    /* 1. Port down interrupt. */
    {ANY_RMEP_TLV_PORT_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_PORT_DOWN_INT_ENABLEf,
        bcmOAMEventEndpointPortDown},

    /* 2. Port up interrupt. */
    {ANY_RMEP_TLV_PORT_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_PORT_UP_INT_ENABLEf,
        bcmOAMEventEndpointPortUp},

    /* 3. Interface down interrupt. */
    {ANY_RMEP_TLV_INTERFACE_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UP_TO_DOWN_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceDown},

    /* 4. Interface up interrupt. */
    {ANY_RMEP_TLV_INTERFACE_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_DOWN_TO_UP_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceUp},

    /* 5. Interface TLV Testing to Up interrupt. */
    {ANY_RMEP_TLV_INTERFACE_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_TESTING_TO_UP_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceTestingToUp},

    /* 6. Interface TLV Unknown to Up interrupt. */
    {ANY_RMEP_TLV_INTERFACE_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UNKNOWN_TO_UP_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceUnknownToUp},

    /* 7. Interface TLV Dormant to Up interrupt. */
    {ANY_RMEP_TLV_INTERFACE_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_DORMANT_TO_UP_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceDormantToUp},

    /* 8. Interface TLV Not present to Up interrupt. */
    {ANY_RMEP_TLV_INTERFACE_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_NOTPRESENT_TO_UP_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceNotPresentToUp},

    /* 9. Interface Link Layer Down to Up interrupt. */
    {ANY_RMEP_TLV_INTERFACE_UP_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_LLDOWN_TO_UP_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceLLDownToUp},

    /* 10. Interface up to testing transition interrupt. */
    {ANY_RMEP_TLV_INTERFACE_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UP_TO_TESTING_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceTesting},

    /* 11. Interface up to unknown transition interrupt. */
    {ANY_RMEP_TLV_INTERFACE_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UP_TO_UNKNOWN_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceUnkonwn},

    /* 11. Interface up to dormant transition interrupt. */
    {ANY_RMEP_TLV_INTERFACE_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UP_TO_DORMANT_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceDormant},

    /* 11. Interface up to not present transition interrupt. */
    {ANY_RMEP_TLV_INTERFACE_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UP_TO_NOTPRESENT_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceNotPresent},

    /* 11. Interface up to Link Layer Down transition interrupt. */
    {ANY_RMEP_TLV_INTERFACE_DOWN_STATUSr, FIRST_RMEP_INDEXf, INVALIDf,
        ANY_RMEP_TLV_INTERFACE_UP_TO_LLDOWN_TRANSITION_INT_ENABLEf,
        bcmOAMEventEndpointInterfaceLLDown},

    /* 12. Low MDL or unexpected MAID interrupt. */
    {XCON_CCM_DEFECT_STATUSr, INVALIDf, FIRST_MA_INDEXf,
        XCON_CCM_DEFECT_INT_ENABLEf,
        bcmOAMEventGroupCCMxcon},

    /*
     * 13. Remote MEP lookup failed or CCM interval mismatch during Remote MEP
     *    lookup interrupt.
     */
    {ERROR_CCM_DEFECT_STATUSr, INVALIDf, FIRST_MA_INDEXf,
        ERROR_CCM_DEFECT_INT_ENABLEf,
        bcmOAMEventGroupCCMError},

    /*
     * 14. Some Remote defect indicator interrupt - aggregated health of remote
     *    MEPs.
     */
    {SOME_RDI_DEFECT_STATUSr, FIRST_RMEP_INDEXf, FIRST_MA_INDEXf,
        SOME_RDI_DEFECT_INT_ENABLEf,
        bcmOAMEventGroupRemote},

    /* 15. Aggregate health of remote MEP state machines interrupt. */
    {SOME_RMEP_CCM_DEFECT_STATUSr, FIRST_RMEP_INDEXf, FIRST_MA_INDEXf,
        SOME_RMEP_CCM_DEFECT_INT_ENABLEf,
        bcmOAMEventGroupCCMTimeout},

    /* Invalid Interrupt - Always Last */
    {INVALIDr, INVALIDf, 0, bcmOAMEventCount}
};

/*
 * 0AM Group faults array initialization.
 */
STATIC _bcm_oam_fault_t _kt2_oam_group_faults[] =
{
    {CURRENT_XCON_CCM_DEFECTf, STICKY_XCON_CCM_DEFECTf,
        BCM_OAM_GROUP_FAULT_CCM_XCON, 0x08},

    {CURRENT_ERROR_CCM_DEFECTf, STICKY_ERROR_CCM_DEFECTf,
        BCM_OAM_GROUP_FAULT_CCM_ERROR, 0x04},

    {CURRENT_SOME_RMEP_CCM_DEFECTf, STICKY_SOME_RMEP_CCM_DEFECTf,
        BCM_OAM_GROUP_FAULT_CCM_TIMEOUT, 0x02},

    {CURRENT_SOME_RDI_DEFECTf, STICKY_SOME_RDI_DEFECTf,
        BCM_OAM_GROUP_FAULT_REMOTE, 0x01},

    {0, 0, 0, 0}
};

/*
 * 0AM Endpoint faults array initialization.
 */
STATIC _bcm_oam_fault_t _kt2_oam_endpoint_faults[] =
{
    {CURRENT_RMEP_PORT_STATUS_DEFECTf,
        STICKY_RMEP_PORT_STATUS_DEFECTf,
        BCM_OAM_ENDPOINT_FAULT_PORT_DOWN, 0x08},

    {CURRENT_RMEP_INTERFACE_STATUS_DEFECTf,
        STICKY_RMEP_INTERFACE_STATUS_DEFECTf,
        BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN, 0x04},

    {CURRENT_RMEP_CCM_DEFECTf,
        STICKY_RMEP_CCM_DEFECTf,
        BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT, 0x20},

    {CURRENT_RMEP_LAST_RDIf,
        STICKY_RMEP_LAST_RDIf,
        BCM_OAM_ENDPOINT_FAULT_REMOTE, 0x10},

    {0, 0, 0, 0}
};

typedef struct _bcm_kt2_oam_intr_en_fields_s {
    soc_field_t field;
    uint32      value;
} _bcm_kt2_oam_intr_en_fields_t;
 
STATIC _bcm_kt2_oam_intr_en_fields_t _kt2_oam_intr_en_fields[bcmOAMEventCount] =
{
    /*
     * Note:
     * The order of hardware field names in the below initialization
     * code must match the event enum order in bcm_oam_event_type_t.
     */
    { ANY_RMEP_TLV_PORT_DOWN_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_PORT_UP_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_UP_TO_DOWN_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_DOWN_TO_UP_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_TESTING_TO_UP_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_UNKNOWN_TO_UP_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_DORMANT_TO_UP_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_NOTPRESENT_TO_UP_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_LLDOWN_TO_UP_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_UP_TO_TESTING_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_UP_TO_UNKNOWN_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_UP_TO_DORMANT_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_UP_TO_NOTPRESENT_TRANSITION_INT_ENABLEf, 1},
    { ANY_RMEP_TLV_INTERFACE_UP_TO_LLDOWN_TRANSITION_INT_ENABLEf, 1},
    { XCON_CCM_DEFECT_INT_ENABLEf, 1},
    { ERROR_CCM_DEFECT_INT_ENABLEf, 1},
    { SOME_RDI_DEFECT_INT_ENABLEf, 1},
    { SOME_RMEP_CCM_DEFECT_INT_ENABLEf, 1},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0},
    { INVALIDf, 0}
};

/* LM counter related fields - Ingress */
STATIC mep_ctr_info_t mep_ctr_info[] = {     
    { LMEP__CTR1_VALIDf, LMEP__CTR1_BASE_PTRf,            
      LMEP__CTR1_MEP_TYPEf, LMEP__CTR1_MEP_MDLf,                      
      LMEP__CTR1_SERVICE_PRI_MAP_PROFILE_PTRf 
    },
    { LMEP__CTR2_VALIDf, LMEP__CTR2_BASE_PTRf,
      LMEP__CTR2_MEP_TYPEf, LMEP__CTR2_MEP_MDLf,
      LMEP__CTR2_SERVICE_PRI_MAP_PROFILE_PTRf 
    }
};

/* LM counter related fields - Egress */
STATIC mep_ctr_info_t egr_mep_ctr_info[] = {     
    { CTR1_VALIDf, CTR1_BASE_PTRf, 
      CTR1_MEP_TYPEf, CTR1_MEP_MDLf,                      
      CTR1_SERVICE_PRI_MAP_PROFILE_PTRf 
    },
    { CTR2_VALIDf, CTR2_BASE_PTRf, 
      CTR2_MEP_TYPEf, CTR2_MEP_MDLf,
      CTR2_SERVICE_PRI_MAP_PROFILE_PTRf
    } 
};

typedef struct _bcm_kt2_oam_olp_hdr_type_map_s {
    bcm_field_olp_header_type_t     field_olp_hdr_type;
    uint8                           mem_index;
    uint8                           subtype;
    uint8                           reuse;
} _bcm_kt2_oam_olp_hdr_type_map_t;

STATIC _bcm_kt2_oam_olp_hdr_type_map_t kt2_olp_hdr_type_mapping[] = {
/* Down MEP CCM */
{bcmFieldOlpHeaderTypeEthOamCcm, _BCM_KT2_CCM_OLP_HDR_TYPE_COMPRESSED,
                                 _BCM_KT2_OLP_HDR_SUBTYPE_CCM, 0},

{bcmFieldOlpHeaderTypeBhhOamCcm, _BCM_KT2_CCM_OLP_HDR_TYPE_COMPRESSED,
                                 _BCM_KT2_OLP_HDR_SUBTYPE_CCM, 1},

/* BFD */
{bcmFieldOlpHeaderTypeBfdOam,  _BCM_KT2_BFD_OLP_HDR_TYPE_COMPRESSED,
                               _BCM_KT2_OLP_HDR_SUBTYPE_BFD, 0},

/* Down MEP LM/DM */
{bcmFieldOlpHeaderTypeEthOamLmDm, _BCM_KT2_LM_DM_OLP_HDR_TYPE_COMPRESSED,
                                  _BCM_KT2_OLP_HDR_SUBTYPE_LM_DM, 0},

{bcmFieldOlpHeaderTypeEthOamLm, _BCM_KT2_LM_DM_OLP_HDR_TYPE_COMPRESSED,
                                _BCM_KT2_OLP_HDR_SUBTYPE_LM_DM, 1},

{bcmFieldOlpHeaderTypeEthOamDm, _BCM_KT2_LM_DM_OLP_HDR_TYPE_COMPRESSED,
                                _BCM_KT2_OLP_HDR_SUBTYPE_LM_DM, 1},

{bcmFieldOlpHeaderTypeBhhOamLm, _BCM_KT2_LM_DM_OLP_HDR_TYPE_COMPRESSED,
                                _BCM_KT2_OLP_HDR_SUBTYPE_LM_DM, 1},

{bcmFieldOlpHeaderTypeBhhOamDm, _BCM_KT2_LM_DM_OLP_HDR_TYPE_COMPRESSED,
                                _BCM_KT2_OLP_HDR_SUBTYPE_LM_DM, 1},

/*
 * UpMEP header types are not programmable by FP, but OAM pipeline lookup
 * can assign UpMEP subtypes.
 */

/* UpMEP CCM*/
{bcmFieldOlpHeaderTypeEthOamUpMepCcm, _BCM_KT2_ETH_OAM_UP_MEP_CCM_OLP_HDR_TYPE_COMPRESSED,
                                      _BCM_KT2_OLP_HDR_SUBTYPE_ETH_OAM_UPMEP_CCM, 0},

{bcmFieldOlpHeaderTypeEthOamUpMepLm, _BCM_KT2_ETH_OAM_UP_MEP_LM_DM_OLP_HDR_TYPE_COMPRESSED,
                                     _BCM_KT2_OLP_HDR_SUBTYPE_ETH_OAM_UPMEP_LM_DM, 0},

{bcmFieldOlpHeaderTypeEthOamUpMepDm, _BCM_KT2_ETH_OAM_UP_MEP_LM_DM_OLP_HDR_TYPE_COMPRESSED,
                                     _BCM_KT2_OLP_HDR_SUBTYPE_ETH_OAM_UPMEP_LM_DM, 1},
};

static uint8 kt2_olp_hdr_type_count = sizeof(kt2_olp_hdr_type_mapping) /
                                           sizeof(kt2_olp_hdr_type_mapping[0]);


/* * * * * * * * * * * * * * * * * * * * * * * * *
 *            Static local functions             *
 */
/*
 * Function:
 *      _bcm_kt2_outer_tpid_init
 * Purpose:
 *      Allocate and initialize memory to cache oam outer tpid entries.
 *      Initialize lock for cached tpid entries.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_kt2_outer_tpid_init(int unit) {
    int               index;
    int               alloc_size;
    uint32            reg32;
    int               rv = BCM_E_NONE;

    /* Allocate memory to cache OUTER_TPID entries.  */
    alloc_size = sizeof(_oam_tpid_t) * BCM_MAX_TPID_ENTRIES;
    if (KT2_OAM_OUTER_TPID_TAB(unit) == NULL) {
        KT2_OAM_OUTER_TPID_TAB(unit) = sal_alloc(alloc_size,
                                         "Cached OAM Outer TPIDs");
        if (KT2_OAM_OUTER_TPID_TAB(unit) == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(KT2_OAM_OUTER_TPID_TAB(unit), 0, alloc_size);

    /* Cache outer TPID table */
    for (index = 0; index < BCM_MAX_TPID_ENTRIES; index++) {
        rv = soc_reg32_get(unit, outer_tpid[index], REG_PORT_ANY, 0, &reg32);
        if (BCM_FAILURE(rv)) {
            sal_free(KT2_OAM_OUTER_TPID_TAB(unit));
            KT2_OAM_OUTER_TPID_TAB(unit) = NULL;
        }
        KT2_OAM_OUTER_TPID_ENTRY(unit, index) = soc_reg_field_get(unit, 
                                          outer_tpid[index], reg32, TPIDf);
    }

    if (NULL == BCM_KT2_OUTER_TPID_MUTEX(unit)) {
        /* Create protection mutex. */
        BCM_KT2_OUTER_TPID_MUTEX(unit) = sal_mutex_create("oamouter_tpid_lock");
        if (NULL == BCM_KT2_OUTER_TPID_MUTEX(unit)) {
            return (BCM_E_MEMORY);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_kt2_inner_tpid_init
 * Purpose:
 *      Allocate and initialize memory to cache oam inner tpid entries.
 *      Initialize lock for cached tpid entries.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_kt2_inner_tpid_init(int unit) {
    int               alloc_size;
    uint32            reg32;
    int               rv = BCM_E_NONE;

    /* Allocate memory to cache INNER_TPID entries.  */
    alloc_size = sizeof(_oam_tpid_t) * BCM_MAX_INNER_TPID_ENTRIES;
    if (KT2_OAM_INNER_TPID_TAB(unit) == NULL) {
        KT2_OAM_INNER_TPID_TAB(unit) = sal_alloc(alloc_size,
                                         "Cached OAM Inner TPIDs");
        if (KT2_OAM_INNER_TPID_TAB(unit) == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(KT2_OAM_INNER_TPID_TAB(unit), 0, alloc_size);

    /* Cache Inner TPID table */
    rv = soc_reg32_get(unit, INNER_TPIDr, REG_PORT_ANY, 0, &reg32);
    if (BCM_FAILURE(rv)) {
        sal_free(KT2_OAM_INNER_TPID_TAB(unit));
        KT2_OAM_INNER_TPID_TAB(unit) = NULL;
    }
    /* Inner TPID has only 1 entry. So, index is always 0 */
    KT2_OAM_INNER_TPID_ENTRY(unit, 0) = soc_reg_field_get(unit, INNER_TPIDr, 
                                                          reg32, TPIDf);

    if (NULL == BCM_KT2_INNER_TPID_MUTEX(unit)) {
        /* Create protection mutex. */
        BCM_KT2_INNER_TPID_MUTEX(unit) = sal_mutex_create("oam inner_tpid_lock");
        if (NULL == BCM_KT2_INNER_TPID_MUTEX(unit)) {
            return (BCM_E_MEMORY);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_kt2_subport_tpid_init
 * Purpose:
 *      Allocate and initialize memory to cache oam subport tpid entries.
 *      Initialize lock for cached tpid entries.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_kt2_subport_tpid_init(int unit) {
    int               index;
    int               alloc_size;
    uint32            reg32;
    int               rv = BCM_E_NONE;

    /* Allocate memory to cache OUTER_TPID entries.  */
    alloc_size = sizeof(_oam_tpid_t) * BCM_MAX_TPID_ENTRIES;
    if (KT2_OAM_SUBPORT_TPID_TAB(unit) == NULL) {
        KT2_OAM_SUBPORT_TPID_TAB(unit) = sal_alloc(alloc_size,
                                         "Cached OAM Subport TPIDs");
        if (KT2_OAM_SUBPORT_TPID_TAB(unit) == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(KT2_OAM_SUBPORT_TPID_TAB(unit), 0, alloc_size);

    /* Cache Subport TPID table */
    for (index = 0; index < BCM_MAX_TPID_ENTRIES; index++) {
        rv = soc_reg32_get(unit, subport_tpid[index], REG_PORT_ANY, 0, &reg32);
        if (BCM_FAILURE(rv)) {
            sal_free(KT2_OAM_SUBPORT_TPID_TAB(unit));
            KT2_OAM_SUBPORT_TPID_TAB(unit) = NULL;
        }
        KT2_OAM_SUBPORT_TPID_ENTRY(unit, index) = soc_reg_field_get(unit, 
                                          subport_tpid[index], reg32, TPIDf);
    }

    if (NULL == BCM_KT2_SUBPORT_TPID_MUTEX(unit)) {
        /* Create protection mutex. */
        BCM_KT2_SUBPORT_TPID_MUTEX(unit) = sal_mutex_create("oam subport_tpid_lock");
        if (NULL == BCM_KT2_SUBPORT_TPID_MUTEX(unit)) {
            return (BCM_E_MEMORY);
        }
    }
    return (BCM_E_NONE);
}
/*
 * Function :
 *    _bcm_kt2_oam_tpid_get
 * 
 * Purpose  :
 *    Get tpid value for tpid entry index in the HW. 
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     tpid_type -(IN) - outer/inner/subport
 *     index - (IN) Entry index.
 *     value - (OUT) TPID value 
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_kt2_oam_tpid_get(int unit, int tpid_type, int index, int *value)
{
    int  rv = BCM_E_NONE;
    uint32 rval = 0;

    if (tpid_type == BCM_OAM_TPID_TYPE_OUTER) {
        if (index >= BCM_MAX_TPID_ENTRIES)  {
            return (BCM_E_PARAM); 
        }
        rv = soc_reg32_get(unit, outer_tpid[index], REG_PORT_ANY, 0, &rval);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PARAM); 
        }
        *value = soc_reg_field_get(unit, outer_tpid[index], rval, TPIDf);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_INNER) {
        if (index >= BCM_MAX_INNER_TPID_ENTRIES)  {
            return (BCM_E_PARAM); 
        }
        rv = soc_reg32_get(unit, INNER_TPIDr, REG_PORT_ANY, 0, &rval);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PARAM); 
        }
        *value = soc_reg_field_get(unit, INNER_TPIDr, rval, TPIDf);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_SUBPORT) {
        if (index >= BCM_MAX_TPID_ENTRIES)  {
            return (BCM_E_PARAM); 
        }
        rv = soc_reg32_get(unit, subport_tpid[index], REG_PORT_ANY, 0, &rval);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PARAM); 
        }
        *value = soc_reg_field_get(unit, subport_tpid[index], rval, TPIDf);
    } else {
        return (BCM_E_PARAM); 
    } 
    return (rv);
}

/*
 * Function :
 *    _bcm_kt2_oam_tpid_set
 * 
 * Purpose  :
 *    Set tpid value for tpid entry index in the HW. 
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     tpid_type -(IN) - outer/inner/subport
 *     index - (IN) Entry index.
 *     value - (IN) Value to be set as tpid 
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_kt2_oam_tpid_set(int unit, int tpid_type, int index, int value)
{
    int  rv = BCM_E_NONE;
    uint32 rval = 0;

    if (tpid_type == BCM_OAM_TPID_TYPE_OUTER) {
        if (index >= BCM_MAX_TPID_ENTRIES)  {
            return (BCM_E_PARAM); 
        }
        soc_reg_field_set(unit, outer_tpid[index], &rval, TPIDf, value);
        rv = soc_reg32_set(unit, outer_tpid[index], REG_PORT_ANY, 0, rval);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PARAM); 
        }
    } else if (tpid_type == BCM_OAM_TPID_TYPE_INNER) {
        if (index >= BCM_MAX_INNER_TPID_ENTRIES)  {
            return (BCM_E_PARAM); 
        }
        soc_reg_field_set(unit, INNER_TPIDr, &rval, TPIDf, value);
        rv = soc_reg32_set(unit, INNER_TPIDr, REG_PORT_ANY, 0, rval);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PARAM); 
        }
    } else if (tpid_type == BCM_OAM_TPID_TYPE_SUBPORT) {
        if (index >= BCM_MAX_TPID_ENTRIES)  {
            return (BCM_E_PARAM); 
        }
        soc_reg_field_set(unit, subport_tpid[index], &rval, TPIDf, value);
        rv = soc_reg32_set(unit, subport_tpid[index], REG_PORT_ANY, 0, rval);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PARAM); 
        }
    } else {
        return (BCM_E_PARAM); 
    } 
    return (rv);
}

/*
 * Function :
 *    _bcm_kt2_oam_tpid_entry_get
 * 
 * Purpose  :
 *    Get tpid value for tpid entry index . 
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     tpid  - (OUT) TPID value.
 *     index - (IN) Entry index.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_kt2_oam_tpid_entry_get(int unit, uint16 *tpid, int index, int tpid_type) 
{
    if (tpid_type == BCM_OAM_TPID_TYPE_OUTER) {
        if ((index < 0) || (index >= BCM_MAX_TPID_ENTRIES) ||
            (KT2_OAM_OUTER_TPID_REF_COUNT(unit, index) <= 0)) {
            return (BCM_E_PARAM);
        }
        *tpid = KT2_OAM_OUTER_TPID_ENTRY(unit, index);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_INNER) {
        if ((index < 0) || (index >= BCM_MAX_INNER_TPID_ENTRIES) ||
            (KT2_OAM_INNER_TPID_REF_COUNT(unit, index) <= 0)) {
            return (BCM_E_PARAM);
        }
        *tpid = KT2_OAM_INNER_TPID_ENTRY(unit, index);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_SUBPORT) {
        if ((index < 0) || (index >= BCM_MAX_TPID_ENTRIES) ||
            (KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, index) <= 0)) {
            return (BCM_E_PARAM);
        }
        *tpid = KT2_OAM_SUBPORT_TPID_ENTRY(unit, index);
    } else {
        return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function :
 *    _bcm_kt2_oam_tpid_entry_delete
 * 
 * Purpose  :
 *    Delete tpid entry by index.
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     index - (IN) Entry index.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_kt2_tpid_entry_delete(int unit, int index, int tpid_type) 
{
    int rv = BCM_E_NONE; 
    if (tpid_type == BCM_OAM_TPID_TYPE_OUTER) {
        BCM_KT2_OUTER_TPID_LOCK(unit);
        if ((index < 0) || (index >= BCM_MAX_TPID_ENTRIES) ||
            (KT2_OAM_OUTER_TPID_REF_COUNT(unit, index) <= 0)) {
            rv = BCM_E_PARAM;
            BCM_KT2_OUTER_TPID_UNLOCK(unit);
            return rv;
        }
        KT2_OAM_OUTER_TPID_REF_COUNT(unit, index)--;
        if (0 == KT2_OAM_OUTER_TPID_REF_COUNT(unit, index)) {
            KT2_OAM_OUTER_TPID_ENTRY(unit, index) = default_outer_tpid[index];
            rv = _bcm_kt2_oam_tpid_set(unit, tpid_type, index, 
                                             default_outer_tpid[index]);
        }
        BCM_KT2_OUTER_TPID_UNLOCK(unit);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_INNER) {
        BCM_KT2_INNER_TPID_LOCK(unit);
        if ((index < 0) || (index >= BCM_MAX_INNER_TPID_ENTRIES) ||
            (KT2_OAM_INNER_TPID_REF_COUNT(unit, index) <= 0)) {
            rv = BCM_E_PARAM;
            BCM_KT2_INNER_TPID_UNLOCK(unit);
            return rv;
        }
        KT2_OAM_INNER_TPID_REF_COUNT(unit, index)--;
        if (0 == KT2_OAM_INNER_TPID_REF_COUNT(unit, index)) {
            KT2_OAM_INNER_TPID_ENTRY(unit, index) = BCM_OAM_DEFAULT_TPID;
            rv = _bcm_kt2_oam_tpid_set(unit, tpid_type, index, 
                                             BCM_OAM_DEFAULT_TPID);
        }
        BCM_KT2_INNER_TPID_UNLOCK(unit);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_SUBPORT) {
        BCM_KT2_SUBPORT_TPID_LOCK(unit);
        if ((index < 0) || (index >= BCM_MAX_TPID_ENTRIES) ||
            (KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, index) <= 0)) {
            rv = BCM_E_PARAM;
            BCM_KT2_SUBPORT_TPID_UNLOCK(unit);
            return rv;
        }
        KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, index)--;
        if (0 == KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, index)) {
            KT2_OAM_SUBPORT_TPID_ENTRY(unit, index) = BCM_OAM_DEFAULT_TPID;
            rv = _bcm_kt2_oam_tpid_set(unit, tpid_type, index, 
                                             BCM_OAM_DEFAULT_TPID);
        }
        BCM_KT2_SUBPORT_TPID_UNLOCK(unit);
    } else {
        return (BCM_E_PARAM);
    }
    return (rv);
}


/*
 * Function :
 *    _bcm_kt2_tpid_lkup
 * 
 * Purpose  :
 *    Get tpid entry index for specific tpid value.
 *
 * Parameters :
 *     unit  - (IN) BCM device number.
 *     tpid  - (IN) TPID value.
 *     tpid_type - (IN) Type of the tpid - inner/outer/subport
 *     index - (OUT) Entry index.
 *
 * Return :
 *    BCM_E_XXX
 */
int
_bcm_kt2_tpid_lkup(int unit, uint16 tpid, int tpid_type,  int *index)
{
    int i;
    if (tpid_type == BCM_OAM_TPID_TYPE_OUTER) { 
        for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
            if (KT2_OAM_OUTER_TPID_ENTRY(unit, i) == tpid) {
                *index = i;
                return BCM_E_NONE;
            }
        }
    } else if (tpid_type == BCM_OAM_TPID_TYPE_INNER) { 
        for (i = 0; i < BCM_MAX_INNER_TPID_ENTRIES; i++) {
            if (KT2_OAM_INNER_TPID_ENTRY(unit, i) == tpid) {
                *index = i;
                return BCM_E_NONE;
            }
        }
    } else if (tpid_type == BCM_OAM_TPID_TYPE_SUBPORT) { 
        for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
            if (KT2_OAM_SUBPORT_TPID_ENTRY(unit, i) == tpid) {
                *index = i;
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_kt2_tpid_entry_add 
 * Purpose:
 *      Add a new TPID entry.
 *      Allocate and initialize memory to cache tpid entries.
 *      Initialize lock for cached tpid entries.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      tpid       - (IN) TPID to be added.
 *      tpid_type  - (IN) Type of the tpid - inner/outer/subport
 *      index      - (OUT) Index where the the new TPID is added.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the same TPID already exists, simply increase the
 * reference count of the cached entry. Otherwise, add the entry
 * to the cached table and write the new entry to hardware.
 * Only four distinct TPID values are currently supported for
 * outer and subport TPID and only one value is supported for
 * inner tpid.
 */

int
_bcm_kt2_tpid_entry_add(int unit, uint16 tpid, int tpid_type, int *index)
{
    int rv = BCM_E_NONE;
    int i, free_index;

    free_index = -1;
    if (tpid_type == BCM_OAM_TPID_TYPE_OUTER) { 
        BCM_KT2_OUTER_TPID_LOCK(unit);
        /* Search for an existing entry.  */
        for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
            if (KT2_OAM_OUTER_TPID_ENTRY(unit, i) == tpid) {
                KT2_OAM_OUTER_TPID_REF_COUNT(unit, i)++;
                *index = i;
                BCM_KT2_OUTER_TPID_UNLOCK(unit);
                return rv;
            }
        }
        for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
            if (KT2_OAM_OUTER_TPID_REF_COUNT(unit, i) == 0) {
                free_index = i;
                break;
            }
        } 
        if (free_index < 0) {
            rv = BCM_E_RESOURCE;
            BCM_KT2_OUTER_TPID_UNLOCK(unit);
            return rv;
        }
        /* Insert the new configuration into tpid table as free
           entry is available  */
        rv = _bcm_kt2_oam_tpid_set(unit, tpid_type, free_index, tpid);
        if (BCM_FAILURE(rv)) {
            BCM_KT2_OUTER_TPID_UNLOCK(unit);
            return rv;
        }
        KT2_OAM_OUTER_TPID_ENTRY(unit, free_index) = tpid;
        KT2_OAM_OUTER_TPID_REF_COUNT(unit, free_index)++;
        *index = free_index;
        BCM_KT2_OUTER_TPID_UNLOCK(unit);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_INNER) { 
        BCM_KT2_INNER_TPID_LOCK(unit);
        /* Search for an existing entry  */
        for (i = 0; i < BCM_MAX_INNER_TPID_ENTRIES; i++) {
            if (KT2_OAM_INNER_TPID_ENTRY(unit, i) == tpid) {
                KT2_OAM_INNER_TPID_REF_COUNT(unit, i)++;
                *index = i;
                BCM_KT2_INNER_TPID_UNLOCK(unit);
                return rv;
            }
        }
        for (i = 0; i < BCM_MAX_INNER_TPID_ENTRIES; i++) {
            if (KT2_OAM_INNER_TPID_REF_COUNT(unit, i) == 0) {
                free_index = i;
                break;
            }
        } 
        if (free_index < 0) {
            rv = BCM_E_RESOURCE;
            BCM_KT2_INNER_TPID_UNLOCK(unit);
            return rv;
        }
        /* Insert the new configuration into tpid table as free
           entry is available  */
        rv = _bcm_kt2_oam_tpid_set(unit, tpid_type, free_index, tpid);
        if (BCM_FAILURE(rv)) {
            BCM_KT2_INNER_TPID_UNLOCK(unit);
            return rv;
        }
        KT2_OAM_INNER_TPID_ENTRY(unit, free_index) = tpid;
        KT2_OAM_INNER_TPID_REF_COUNT(unit, free_index)++;
        *index = free_index;
        BCM_KT2_INNER_TPID_UNLOCK(unit);
    } else if (tpid_type == BCM_OAM_TPID_TYPE_SUBPORT) { 
        BCM_KT2_SUBPORT_TPID_LOCK(unit);
        /* Search for an existing entry */
        for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
            if (KT2_OAM_SUBPORT_TPID_ENTRY(unit, i) == tpid) {
                KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, i)++;
                *index = i;
                BCM_KT2_SUBPORT_TPID_UNLOCK(unit);
                return rv;
            }
        }
        for (i = 0; i < BCM_MAX_TPID_ENTRIES; i++) {
            if (KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, i) == 0) {
                free_index = i;
                break;
            }
        } 
        if (free_index < 0) {
            rv = BCM_E_RESOURCE;
            BCM_KT2_SUBPORT_TPID_UNLOCK(unit);
            return rv;
        }
        /* Insert the new configuration into tpid table as free
           entry is available.  */
        rv = _bcm_kt2_oam_tpid_set(unit, tpid_type, free_index, tpid);
        if (BCM_FAILURE(rv)) {
            BCM_KT2_SUBPORT_TPID_UNLOCK(unit);
            return rv;
        }
        KT2_OAM_SUBPORT_TPID_ENTRY(unit, free_index) = tpid;
        KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, free_index)++;
        *index = free_index;
        BCM_KT2_SUBPORT_TPID_UNLOCK(unit);
    } else {
        rv = BCM_E_PARAM;
    }
    return rv;    
}

/*
 * Function:
 *     _bcm_kt2_oam_ccm_msecs_to_hw_encode
 * Purpose:
 *     Quanitze CCM interval from msecs to hardware encoding.
 * Parameters:
 *     period -  (IN) CCM interval in milli seconds.
 * Retruns:
 *     Hardware encoding for the specified CCM interval value.
 */
STATIC int
_bcm_kt2_oam_ccm_msecs_to_hw_encode(int period)
{
    int q_period = 0; /* Quantized CCM period value. */

    if (0 == period) {
        return (q_period);
    }

    /* Find closest supported period */
    for (q_period = 1; _kt2_ccm_intervals[q_period]
            != _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED; ++q_period) {
        if (period < _kt2_ccm_intervals[q_period]) {
            break;
        }
    }

    if (_kt2_ccm_intervals[q_period]
        == _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED) {
        /* Use the highest defined value */
        --q_period;
    } else {
        if ((period - _kt2_ccm_intervals[q_period - 1])
                < (_kt2_ccm_intervals[q_period] - period)) {
            /* Closer to the lower value */
            --q_period;
        }
    }

    return q_period;
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *     _bcm_kt2_oam_ccm_hw_encode_to_msecs
 * Purpose:
 *     Get CCM interval in msecs for a given hardware encoded value.
 * Parameters:
 *     encode -  (IN) CCM interval hardware encoding.
 * Retruns:
 *     CCM interval in msecs.
 */
STATIC int
_bcm_kt2_oam_ccm_hw_encode_to_msecs(int encode)
{
    return (_kt2_ccm_intervals[encode]);
}
#endif

/*
 * Function:
 *     _bcm_kt2_oam_opcode_profile_entry_set
 * Purpose:
 *     Program the OAM opcode control profile fields.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     mem   - (IN) Opcode profile memory - ingress/egress
 *     flags - (IN) Bitmap of opcode control settings.
 *     entry - (IN/OUT) Pointer to opcode control profile table entry buffer.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_opcode_profile_entry_set(int unit, _bcm_oam_hash_data_t *h_data_p,
                                      soc_mem_t mem,
                                      uint32 flags,
                                      void *entry)
{
    uint32 ep_opcode;        /* Endpoint opcode flag bits. */
    uint32 opcode_count = 0; /* Number of bits set.        */
    int    bp;               /* bit position.              */
    int    rv = BCM_E_NONE;  /* Opreation return status.   */
    
    /* Validate opcode flag bits. */
    if (flags & ~(_BCM_KT2_OAM_OPCODE_MASK)) {
        return (BCM_E_PARAM);
    }

    /* Get number of valid opcodes supported. */
    opcode_count = _shr_popcount(_BCM_KT2_OAM_OPCODE_MASK);

    /*
     * Iterate over opcode flag bits and set corresponding fields
     * in entry buffer.
     */
    for (bp = 0; bp < opcode_count; bp++) {
        ep_opcode = (flags & (1 << bp));

        switch (ep_opcode) {
            case BCM_OAM_OPCODE_CCM_COPY_TO_CPU:
                soc_mem_field32_set(unit, mem, entry, CCM_COPYTO_CPUf, 1);
                break;

            case BCM_OAM_OPCODE_CCM_IN_HW:
                soc_mem_field32_set(unit, mem, entry, CCM_PROCESS_IN_HWf, 1);
                break;

            case BCM_OAM_OPCODE_CCM_DROP:
                soc_mem_field32_set(unit, mem, entry, CCM_DROPf, 1);
                break;

            case BCM_OAM_OPCODE_LBM_IN_HW:
                soc_mem_field32_set(unit, mem, entry, LBM_ACTIONf, 1);
                break;

            case BCM_OAM_OPCODE_LBM_UC_COPY_TO_CPU:
                soc_mem_field32_set(unit, mem, entry, LBM_UC_COPYTO_CPUf, 1);
                break;

            case BCM_OAM_OPCODE_LBM_UC_DROP:
                soc_mem_field32_set(unit, mem, entry, LBM_UC_DROPf, 1);
                break;

            case BCM_OAM_OPCODE_LBM_MC_DROP:
                soc_mem_field32_set(unit, mem, entry, LBM_MC_DROPf, 1);
                break;

            case BCM_OAM_OPCODE_LBM_MC_COPY_TO_CPU:
                soc_mem_field32_set(unit, mem, entry, LBM_MC_COPYTO_CPUf, 1);
                break;

            case BCM_OAM_OPCODE_LBR_COPY_TO_CPU:
                soc_mem_field32_set(unit, mem, entry, LBR_COPYTO_CPUf, 1);
                break;

            case BCM_OAM_OPCODE_LBR_DROP:
                soc_mem_field32_set(unit, mem, entry, LBR_DROPf, 1);
                break;

            case BCM_OAM_OPCODE_LTM_COPY_TO_CPU:
                soc_mem_field32_set(unit, mem, entry, LTM_COPYTO_CPUf, 1);
                break;

            case BCM_OAM_OPCODE_LTM_DROP:
                soc_mem_field32_set(unit, mem, entry, LTM_DROPf, 1);
                break;

            case BCM_OAM_OPCODE_LTR_COPY_TO_CPU:
                soc_mem_field32_set(unit, mem, entry, LTR_COPYTO_CPUf, 1);
                break;

            case BCM_OAM_OPCODE_LTR_DROP:
                soc_mem_field32_set(unit, mem, entry, LTR_DROPf, 1);
                break;

            case BCM_OAM_OPCODE_LMEP_PKT_FWD:
                soc_mem_field32_set(unit, mem, entry, FWD_LMEP_PKTf, 1);
                break;

            case BCM_OAM_OPCODE_OTHER_COPY_TO_CPU:
                soc_mem_field32_set(unit, mem, entry, 
                        OTHER_OPCODE_GROUP_1_UC_MY_STATION_MISS_COPYTO_CPUf, 1);
                soc_mem_field32_set (unit, mem, entry, 
                        OTHER_OPCODE_GROUP_1_UC_MY_STATION_HIT_COPYTO_CPUf, 1);
                soc_mem_field32_set (unit, mem, entry, 
                        OTHER_OPCODE_GROUP_1_LOW_MDL_COPYTO_CPUf, 1);
                soc_mem_field32_set (unit, mem, entry, 
                        OTHER_OPCODE_GROUP_1_MC_COPYTO_CPUf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                        OTHER_OPCODE_GROUP_2_UC_MY_STATION_MISS_COPYTO_CPUf, 1);
                soc_mem_field32_set (unit, mem, entry, 
                        OTHER_OPCODE_GROUP_2_UC_MY_STATION_HIT_COPYTO_CPUf, 1);
                soc_mem_field32_set (unit, mem, entry, 
                        OTHER_OPCODE_GROUP_2_LOW_MDL_COPYTO_CPUf, 1);
                soc_mem_field32_set (unit, mem, entry, 
                        OTHER_OPCODE_GROUP_2_MC_COPYTO_CPUf, 1);
                break;

            case BCM_OAM_OPCODE_OTHER_DROP:
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_1_UC_MY_STATION_MISS_ACTIONf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_1_UC_MY_STATION_HIT_ACTIONf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_1_LOW_MDL_ACTIONf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_1_MC_ACTIONf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_2_UC_MY_STATION_MISS_ACTIONf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_2_UC_MY_STATION_HIT_ACTIONf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_2_LOW_MDL_ACTIONf, 1);
                soc_mem_field32_set(unit, mem, entry, 
                           OTHER_OPCODE_GROUP_2_MC_ACTIONf, 1);
                break;

            default:
                break;
        }
    }
    if (_BCM_OAM_EP_IS_MIP(h_data_p)) {
        soc_mem_field32_set(unit, mem, entry, LOW_MDL_CCM_FWD_ACTIONf, 0);
        soc_mem_field32_set(unit, mem, entry, LOW_MDL_LB_LT_DROPf, 0);
        soc_mem_field32_set(unit, mem, entry, LB_LT_UC_MY_STATION_MISS_DROPf, 0);
    } else {
        /* Set the opcode action values of the newly introduced CFM opcode actions
           to values that match with the legacy behavior for these opcodes */
        /* LOW MDL CCM / LB/LT drop and LB/LT my station miss drop */
        soc_mem_field32_set(unit, mem, entry, LOW_MDL_CCM_FWD_ACTIONf, 2);
        soc_mem_field32_set(unit, mem, entry, LOW_MDL_LB_LT_DROPf, 1);
        soc_mem_field32_set(unit, mem, entry, LB_LT_UC_MY_STATION_MISS_DROPf, 1);
    }
    return rv;
}


/*
 * Function:
 *     _bcm_kt2_oam_opcode_profile_entry_init
 * Purpose:
 *     Setup default OAM opcode control profile settings for MEP.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     mem   - (IN) Opcode profile memory - ingress/egress
 *     entry - (IN/OUT) Pointer to opcode control profile table entry buffer.
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_oam_opcode_profile_entry_init(int unit, soc_mem_t mem,
                                       void *entry)
{
    uint32 opcode; /* Opcode flag bits.        */
    int    rv;     /* Operation return status. */

    _bcm_oam_hash_data_t h_data;
    sal_memset(&h_data, 0, sizeof(_bcm_oam_hash_data_t)); 

    h_data.type = bcmOAMEndpointTypeEthernet;

    opcode =  (BCM_OAM_OPCODE_CCM_COPY_TO_CPU |
                    BCM_OAM_OPCODE_CCM_DROP |
                    BCM_OAM_OPCODE_LBM_UC_COPY_TO_CPU |
                    BCM_OAM_OPCODE_LBM_UC_DROP |
                    BCM_OAM_OPCODE_LBM_MC_COPY_TO_CPU |
                    BCM_OAM_OPCODE_LBM_MC_DROP |
                    BCM_OAM_OPCODE_LBR_COPY_TO_CPU |
                    BCM_OAM_OPCODE_LBR_DROP |
                    BCM_OAM_OPCODE_LTM_COPY_TO_CPU |
                    BCM_OAM_OPCODE_LTM_DROP |
                    BCM_OAM_OPCODE_LTR_COPY_TO_CPU |
                    BCM_OAM_OPCODE_LTR_DROP |
                    BCM_OAM_OPCODE_OTHER_COPY_TO_CPU |
                    BCM_OAM_OPCODE_OTHER_DROP 
                    );

    rv = _bcm_kt2_oam_opcode_profile_entry_set(unit, &h_data, mem, opcode, entry);
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_mip_opcode_profile_entry_init
 * Purpose:
 *     Setup default OAM opcode control profile settings for MIP.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     mem   - (IN) Opcode profile memory - ingress/egress
 *     entry - (IN/OUT) Pointer to opcode control profile table entry buffer.
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_oam_mip_opcode_profile_entry_init(int unit, soc_mem_t mem,
                                       void *entry)
{
    uint32 opcode; /* Opcode flag bits.        */
    int    rv;     /* Operation return status. */

    _bcm_oam_hash_data_t h_data;
    sal_memset(&h_data, 0, sizeof(_bcm_oam_hash_data_t));

    h_data.flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;
    h_data.type = bcmOAMEndpointTypeEthernet;
    opcode =  (BCM_OAM_OPCODE_LBM_UC_COPY_TO_CPU |
            BCM_OAM_OPCODE_LBM_UC_DROP |
            BCM_OAM_OPCODE_LBM_MC_COPY_TO_CPU |
            BCM_OAM_OPCODE_LBM_MC_DROP |
            BCM_OAM_OPCODE_LTM_COPY_TO_CPU |
            BCM_OAM_OPCODE_LTM_DROP);
    
    rv = _bcm_kt2_oam_opcode_profile_entry_set(unit, &h_data, mem, opcode, entry);
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_opcode_group_init
 * Purpose:
 *     Init ingress and egress OAM opcode groups. 
 * Parameters:
 *     unit  - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_oam_opcode_group_init(int unit)
{
    int                       rv = BCM_E_NONE; 
    ingress_oam_opcode_group_entry_t    *opcode_grp_buf;
    ingress_oam_opcode_group_entry_t    *opcode_grp_entry;
    int                       entry_mem_size;    /* Size of table entry. */ 
    int                       index_max = 0;
    int                       index = 0;
    bcm_oam_endpoint_action_t action;
    BCM_OAM_ACTION_CLEAR_ALL(action);
    BCM_OAM_OPCODE_CLEAR_ALL(action);

   /* Set ingress and egress OAM opcode group */
    entry_mem_size = sizeof(ingress_oam_opcode_group_entry_t);
    /* Allocate buffer to store the DMAed table entries. */
    index_max = soc_mem_index_max(unit, INGRESS_OAM_OPCODE_GROUPm);
    opcode_grp_buf = soc_cm_salloc(unit, entry_mem_size * (index_max + 1),
                              "Opcode group entry buffer");
    if (NULL == opcode_grp_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(opcode_grp_buf, 0, sizeof(entry_mem_size) * (index_max + 1));

    for (index = 0; index <= index_max; index++) {
        opcode_grp_entry = soc_mem_table_idx_to_pointer
                    (unit, INGRESS_OAM_OPCODE_GROUPm, 
                     ingress_oam_opcode_group_entry_t *,
                     opcode_grp_buf, index);
        if (index <= _BCM_OAM_OPCODE_TYPE_LTM) {
            /* For CFM packets group valid = 0 */
            soc_mem_field32_set(unit, INGRESS_OAM_OPCODE_GROUPm, 
                          (uint32 *)opcode_grp_entry, OPCODE_GROUP_VALIDf, 0);
        } else {
            /* For Non-CFM packets group valid = 1 and group is opcode group 1*/
            soc_mem_field32_set(unit, INGRESS_OAM_OPCODE_GROUPm, 
                          (uint32 *)opcode_grp_entry, OPCODE_GROUP_VALIDf, 1);
            soc_mem_field32_set(unit, INGRESS_OAM_OPCODE_GROUPm, 
                          (uint32 *)opcode_grp_entry, OPCODE_GROUPf, 0);
        }
    }
    rv = soc_mem_write_range(unit, INGRESS_OAM_OPCODE_GROUPm,
                            MEM_BLOCK_ALL, 0, index_max, opcode_grp_buf);
    if (BCM_FAILURE(rv)) {
        if (opcode_grp_buf) {
            soc_cm_sfree(unit, opcode_grp_buf);
        }
        return rv;
    }
    rv = soc_mem_write_range(unit, EGR_OAM_OPCODE_GROUPm,
                            MEM_BLOCK_ALL, 0, index_max, opcode_grp_buf); 
    if (BCM_FAILURE(rv)) {
        if (opcode_grp_buf) {
            soc_cm_sfree(unit, opcode_grp_buf);
        }
        return rv;
    }
    if (opcode_grp_buf) {
         soc_cm_sfree(unit, opcode_grp_buf);
    }
    return (rv);
}

STATIC void
_bcm_kt2_oam_get_passive_mdl_from_active_mdl(int unit, uint8 active_mdl, 
                                             int ma_base_index,
                                             uint8 *passive_mdl,
                                             int is_upmep)
{
    uint32               mdl_value[1] = { 0 };
    int ma_offset = 0;
    int mdl = 0;
    bcm_oam_endpoint_t ep_id_at_mdl = _BCM_OAM_INVALID_INDEX;
    _bcm_oam_control_t  *oc = NULL;
    _bcm_oam_hash_data_t *h_data_p = NULL;

    /* coverity[check_return] */
    _bcm_kt2_oam_control_get(unit, &oc);

    if (oc == NULL) {
        /* Not expected */
        return;
    }
 
    /* Initialize passive_mdl to 0 */ 
    *passive_mdl = 0; 
    mdl_value[0] = active_mdl; 

    for (mdl = 0; mdl <= 7; mdl++) {
        if (SHR_BITGET(mdl_value, mdl)) {
            ep_id_at_mdl = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_get(oc, 
                            _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_IDX
                            (ma_base_index, ma_offset, is_upmep)); 
            if (ep_id_at_mdl != _BCM_OAM_INVALID_INDEX) { 
                h_data_p = &(oc->oam_hash_data[ep_id_at_mdl]);
                /* Set the passive bitmap only if it is not a MIP */
                if (!_BCM_OAM_EP_IS_MIP(h_data_p)) {
                    *passive_mdl = (1 << mdl) | ((1 << mdl) -1);
                }
            } else {
                /* Log an error. Ideally should not happen. */
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: EP ID mapping get failed (MA_IDX=%d) -"
                                    "\n"), unit, ma_base_index+ma_offset));
            }
            ma_offset++;
        }
    }
}



/*
 * Function:
 *     _bcm_kt2_oam_ep_hash_key_construct
 * Purpose:
 *     Construct hash table key for a given endpoint information.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     oc      - (IN) Pointer to OAM control structure.
 *     ep_info - (IN) Pointer to endpoint information structure.
 *     key     - (IN/OUT) Pointer to hash key buffer.
 * Retruns:
 *     None
 */
STATIC void
_bcm_kt2_oam_ep_hash_key_construct(int unit,
                               _bcm_oam_control_t *oc,
                               bcm_oam_endpoint_info_t *ep_info,
                               _bcm_oam_hash_key_t *key)
{
    uint8  *loc = *key;
    uint32 direction = 0;

    sal_memset(key, 0, sizeof(_bcm_oam_hash_key_t));

    if (NULL != ep_info) {
        if ((ep_info->flags & BCM_OAM_ENDPOINT_UP_FACING)) {
            direction = 1;
        }

        _BCM_OAM_KEY_PACK(loc, &ep_info->group, sizeof(ep_info->group));

        _BCM_OAM_KEY_PACK(loc, &ep_info->name, sizeof(ep_info->name));

        _BCM_OAM_KEY_PACK(loc, &ep_info->gport, sizeof(ep_info->gport));

        _BCM_OAM_KEY_PACK(loc, &ep_info->level, sizeof(ep_info->level));

        _BCM_OAM_KEY_PACK(loc, &ep_info->vlan, sizeof(ep_info->vlan));

        _BCM_OAM_KEY_PACK(loc, &ep_info->inner_vlan, 
                                          sizeof(ep_info->inner_vlan));
        _BCM_OAM_KEY_PACK(loc, &direction, sizeof(direction));
    }

    /* End address should not exceed size of _bcm_oam_hash_key_t. */
    assert ((int) (loc - *key) <= sizeof(_bcm_oam_hash_key_t));
}

/*
 * Function:
 *     _bcm_oam_egr_lmep_key_construct
 * Purpose:
 *     Construct egress MP group table lookup key for a given endpoint.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     h_data_p - (IN) Pointer to endpoint hash data memory.
 *     egr_mp_grp_key_p - (IN/OUT) Pointer to entry buffer.
 * Retruns:
 *     None
 */
STATIC void
_bcm_oam_egr_lmep_key_construct(int unit,
                            const _bcm_oam_hash_data_t *h_data_p,
                            egr_mp_group_entry_t *egr_mp_grp_key_p)
{
    _bcm_oam_control_t  *oc = NULL;

    /* coverity[check_return] */
    _bcm_kt2_oam_control_get(unit, &oc);

    /* Set the valid bit. */
    soc_EGR_MP_GROUPm_field32_set
        (unit, egr_mp_grp_key_p, VALIDf, 1);
    /* Set the search key type. */
    soc_EGR_MP_GROUPm_field32_set
        (unit, egr_mp_grp_key_p, KEY_TYPEf, h_data_p->oam_domain);

    if (h_data_p->oam_domain != _BCM_OAM_DOMAIN_BHH) {

        /* Set port type status in the search key. */
        if (_BCM_OAM_EP_IS_VP_TYPE(h_data_p)) {

            /* DGLP field contains virtual port. */
            soc_EGR_MP_GROUPm_field32_set
                (unit, egr_mp_grp_key_p, DGLPf, h_data_p->vp);

        } else {
            if (oc->eth_oam_mp_group_vlan_key ==
                              _BCM_KT2_ETH_OAM_MP_GROUP_KEY_DOMAIN_INDEPENDANT) {
                /* In earlier versions of the SDK, both inner & outer vlan was
                 * always programmed in the MP_GROUP irrespective of the MEP
                 * type, this has been changed to program only CVLAN for CVLAN
                 * MEPs and SVLAN for SVLAN MEPs. To avoid breaking warm-upgrade
                 * the old scheme is still supported.
                 */
                soc_EGR_MP_GROUPm_field32_set(unit, egr_mp_grp_key_p,
                                              SVIDf, h_data_p->vlan);

                soc_EGR_MP_GROUPm_field32_set(unit, egr_mp_grp_key_p,
                                              CVIDf, h_data_p->inner_vlan);
            } else {
                if (h_data_p->oam_domain != _BCM_OAM_DOMAIN_CVLAN) {
                    /* SVLAN/C+SVLAN */
                    soc_EGR_MP_GROUPm_field32_set(unit, egr_mp_grp_key_p,
                                                  SVIDf, h_data_p->vlan);
                }

                if (h_data_p->oam_domain != _BCM_OAM_DOMAIN_SVLAN) {
                    /* CLVAN/C+SVLAN */
                    soc_EGR_MP_GROUPm_field32_set(unit, egr_mp_grp_key_p,
                                                  CVIDf, h_data_p->inner_vlan);
                }
            }
            /* DGLP contains generic logical port. */
            soc_EGR_MP_GROUPm_field32_set(unit, egr_mp_grp_key_p,
                                          DGLPf, h_data_p->dglp);
        }
    } 
#if defined(INCLUDE_BHH)
    else {

        /* BHH Domain, Key = Next Hop Index 
           BHH API have been using L3_EGRESS type in 
           bcm_oam_endpoint_info_t.intf field. 
           However for supporting LM/DM on Katana2 this should be DVP_EGRESS type,
           only CCM will work with L3_EGRESS type, retaining the old type for 
           backward compatibility */
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, h_data_p->egress_if)) {
            soc_EGR_MP_GROUPm_field32_set (unit, egr_mp_grp_key_p, NHIf, 
                h_data_p->egress_if - BCM_XGS3_EGRESS_IDX_MIN(unit));
        } else {
            if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, h_data_p->egress_if)) {
                soc_EGR_MP_GROUPm_field32_set (unit, egr_mp_grp_key_p, NHIf, 
                    h_data_p->egress_if - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            }
        }
    }
#endif

}

/*
 * Function:
 *     _bcm_kt2_oam_lmep_key_construct
 * Purpose:
 *     Construct LMEP view lookup key for a given endpoint.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     h_data_p - (IN) Pointer to endpoint hash data memory.
 *     l3_key_p - (IN/OUT) Pointer to entry buffer.
 * Retruns:
 *     None
 */
STATIC void
_bcm_kt2_oam_lmep_key_construct(int unit,
                            const _bcm_oam_hash_data_t *h_data_p,
                            l3_entry_1_entry_t *l3_key_p)
{
    _bcm_oam_control_t  *oc = NULL;

    /* coverity[check_return] */
    _bcm_kt2_oam_control_get(unit, &oc);

    /* Set the search key type. */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set
        (unit, l3_key_p, KEY_TYPEf, SOC_MEM_KEY_L3_ENTRY_LMEP);

    /* Set the search key sub type. */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set
        (unit, l3_key_p, LMEP__OAM_LMEP_KEY_SUBTYPEf, 
                                   h_data_p->oam_domain);
    
    if (h_data_p->oam_domain != _BCM_OAM_DOMAIN_BHH) {

        /* Set port type status in the search key. */
        if (_BCM_OAM_EP_IS_VP_TYPE(h_data_p)) {
            /* SGLP field contains virtual port. */
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set
                (unit, l3_key_p, LMEP__SGLPf, h_data_p->vp);

        } else {
            if (oc->eth_oam_mp_group_vlan_key ==
                              _BCM_KT2_ETH_OAM_MP_GROUP_KEY_DOMAIN_INDEPENDANT) {
                /* In earlier versions of the SDK, both inner & outer vlan was
                 * always programmed in the MP_GROUP irrespective of the MEP
                 * type, this has been changed to program only CVLAN for CVLAN
                 * MEPs and SVLAN for SVLAN MEPs. To avoid breaking warm-upgrade
                 * the old scheme is still supported.
                 */
                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, LMEP__SVIDf,
                                                       h_data_p->vlan);

                soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, LMEP__CVIDf,
                                                       h_data_p->inner_vlan);
            } else {
                if (h_data_p->oam_domain != _BCM_OAM_DOMAIN_CVLAN) {
                    /* SVLAN/C+SVLAN */
                    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p,
                                                           LMEP__SVIDf,
                                                           h_data_p->vlan);
                }

                if (h_data_p->oam_domain != _BCM_OAM_DOMAIN_SVLAN) {
                    /* CLVAN/C+SVLAN */
                    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p,
                                                           LMEP__CVIDf,
                                                           h_data_p->inner_vlan);
                }
            }
            /* SGLP contains generic logical port. */
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, l3_key_p, LMEP__SGLPf,
                                                   h_data_p->sglp);

        }
    } 
#if defined(INCLUDE_BHH)
    else {
 
            /* If the label is not global, set port */
            if (bcm_tr_mpls_port_independent_range(unit, h_data_p->label, 
                 BCM_GPORT_INVALID) != BCM_E_NONE) {
                /* SGLP contains generic logical port. */
                soc_L3_ENTRY_IPV4_UNICASTm_field32_set
                    (unit, l3_key_p, LMEP__SGLPf, h_data_p->sglp);
            } else {
                /* SGLP contains generic logical port. */
                soc_L3_ENTRY_IPV4_UNICASTm_field32_set
                    (unit, l3_key_p, LMEP__SGLPf, 0xFFFF);
                
            }

            /* Set incoming MPLS Label */
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set
                (unit, l3_key_p, LMEP__MPLS_LABELf, h_data_p->label);

    }
#endif
}

/*
 * Function:
 *     _bcm_kt2_oam_rmep_key_construct
 * Purpose:
 *     Construct RMEP view lookup key for a given endpoint.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     h_data_p - (IN) Pointer to endpoint hash data memory.
 *     l3_key_p - (IN/OUT) Pointer to entry buffer.
 * Retruns:
 *     None
 */
STATIC void
_bcm_kt2_oam_rmep_key_construct(int unit,
                            const _bcm_oam_hash_data_t *h_data_p,
                            l3_entry_1_entry_t *l3_key_p)
{

    /* Set the search key type. */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set
        (unit, l3_key_p, KEY_TYPEf, SOC_MEM_KEY_L3_ENTRY_RMEP);

    /* Set endpoint name. */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set
        (unit, l3_key_p, RMEP__MEPIDf, h_data_p->name);

    /* Set MA_PTR */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set
        (unit, l3_key_p, RMEP__MA_PTRf, h_data_p->group_index);

}

#if defined(KEY_PRINT)
/*
 * Function:
 *     _bcm_oam_hash_key_print
 * Purpose:
 *     Print the contents of a hash key buffer. 
 * Parameters:
 *     hash_key - (IN) Pointer to hash key buffer.
 * Retruns:
 *     None
 */
STATIC void
_bcm_oam_hash_key_print(_bcm_oam_hash_key_t *hash_key)
{
    int i;
    LOG_CLI((BSL_META_U(unit,
                        "HASH KEY:")));
    for(i = 0; i < _OAM_HASH_KEY_SIZE; i++) {
        LOG_CLI((BSL_META_U(unit,
                            ":%u"), *(hash_key[i])));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
}
#endif

/*
 * Function:
 *     _bcm_kt2_oam_control_get
 * Purpose:
 *     Lookup a OAM control config from a bcm device id.
 * Parameters:
 *     unit -  (IN)BCM unit number.
 *     oc   -  (OUT) OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_control_get(int unit, _bcm_oam_control_t **oc)
{
    if (NULL == oc) {
        return (BCM_E_PARAM);
    }

    /* Ensure oam module is initialized. */
    _BCM_OAM_IS_INIT(unit);

    *oc = _kt2_oam_control[unit];

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_group_endpoint_count_init
 * Purpose:
 *     Retrieves and initializes endpoint count information for this device.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to device OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_group_endpoint_count_init(int unit, _bcm_oam_control_t *oc)
{
    /* Input parameter check. */
    if (NULL == oc) {
        return (BCM_E_PARAM);
    }

    /*
     * Get endpoint hardware table index count values and
     * initialize device OAM control structure members variables.
     */
    oc->rmep_count = soc_mem_index_count(unit, RMEPm);
    oc->lmep_count = soc_mem_index_count(unit, LMEPm);
    oc->ma_idx_count = soc_mem_index_count(unit, MA_INDEXm);
    oc->egr_ma_idx_count = soc_mem_index_count(unit, EGR_MA_INDEXm);

    /* Max number of endpoints supported by the device. */
    oc->ep_count = (oc->rmep_count + oc->lmep_count + oc->ma_idx_count + 
                                                   oc->egr_ma_idx_count);

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM Info: Total No. endpoint Count = %d.\n"),
                 oc->ep_count));

    /* Max number of MA Groups supported by device. */
    oc->group_count = soc_mem_index_count(unit, MA_STATEm);
    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM Info: Total No. Group Count = %d.\n"),
                 oc->group_count));

    oc->lm_counter_cnt = soc_mem_index_count(unit, OAM_LM_COUNTERS_0m);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_ccm_rx_timeout_enable
 * Purpose:
 *     Enable CCM Timer operations for endpoint state table.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     state - (IN) Enable/Disable.
 * Retruns:
 *     BCM_E_XXX
 * Note:
 *     RMEP_MA_STATE_REFRESH_INDEXr - CPU access for debug only.
 */
STATIC int
_bcm_kt2_oam_ccm_rx_timeout_set(int unit, uint8 state)
{
    int     rv;       /* Opreation return status.   */
    uint32  rval = 0; /* Register value.            */

    /* Enable timer instructions to RMEP/MA_STATE Table. */
    soc_reg_field_set(unit, OAM_TIMER_CONTROLr, &rval,
                      TIMER_ENABLEf, state ? 1 : 0);

    /* Set Clock granularity to 250us ticks - 1. */
    soc_reg_field_set(unit, OAM_TIMER_CONTROLr, &rval,
                      CLK_GRANf, 1);

    rv = WRITE_OAM_TIMER_CONTROLr(unit, rval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Timer enable - Failed.\n")));
        return (rv);
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_ccm_tx_config_enable
 * Purpose:
 *     Enable transmission of OAM PDUs on local endpoint.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     state - (IN) Enable/Disable
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_ccm_tx_config_set(int unit, uint8 state)
{
    int     rv;       /* Opreation return status.   */
    uint32  rval = 0; /* Register value.            */

    /* Enable OAM LMEP Tx. */
    soc_reg_field_set(unit, OAM_TX_CONTROLr, &rval,
                      TX_ENABLEf, state ? 1 : 0);

    /* Enable CMIC buffer. */
    soc_reg_field_set(unit, OAM_TX_CONTROLr, &rval,
                      CMIC_BUF_ENABLEf, state ? 1 : 0);

    rv = WRITE_OAM_TX_CONTROLr(unit, rval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Tx config enable - Failed.\n")));
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_misc_config
 * Purpose:
 *     Miscellaneous OAM configurations:
 *         1. Enable IFP lookup on the CPU port. 
 * Parameters:
 *     unit -  (IN) BCM unit number.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_misc_config(int unit)
{
    int     rv;       /* Opreation return status.   */

    /*
     * Enable ingress FP for CPU port so LM/DM packets sent from CPU
     * can be processed.
     */
    rv = bcm_esw_port_control_set(unit, CMIC_PORT(unit),
                                  bcmPortControlFilterIngress, 1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: bcm_esw_port_control_set"
                               " - Failed.\n")));
        return (rv);
    }
    return (rv);

}

/*
 * Function:
 *     _bcm_kt2_oam_profile_tables_init
 * Purpose:
 *     Create ingress and egress service priority mapping profile table and 
 *     setup a default profile. Create Ingress and egress OAM opcode control 
 *     profile table, dglp profile 
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_profile_tables_init(int unit, _bcm_oam_control_t *oc)
{
    int         rv = BCM_E_NONE;/* Opreation return status.   */
    soc_mem_t   mem;           /* Profiled table memory.      */
    int         entry_words;   /* Profile table word size.    */
    int         pri;           /* Priority                    */
    void        *entries[1];   /* Profile entry. */
    uint32      profile_index; /* Profile table index. */
    ing_service_pri_map_entry_t pri_ent[BCM_OAM_INTPRI_MAX]; /* ing profile */
                                                             /* entry */
    egr_service_pri_map_entry_t egr_pri_ent[BCM_OAM_INTPRI_MAX]; /*egr profile*/
                                                             /* entry */
    bcm_module_t         modid;
    uint32      dglp = 0;
    egr_oam_dglp_profile_entry_t egr_dglp_profile_entry;
    ing_oam_dglp_profile_entry_t ing_dglp_profile_entry;
    oam_opcode_control_profile_entry_t opcode_entry; /* Opcode control 
                                                             profile entry.  */
    egr_oam_opcode_control_profile_entry_t egr_opcode_entry;   

    /* Ingress Service Priority Map profile table initialization. */
    soc_profile_mem_t_init(&oc->ing_service_pri_map);

    entry_words = (sizeof(ing_service_pri_map_entry_t) / sizeof(uint32));

    mem = ING_SERVICE_PRI_MAPm;
    rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                &oc->ing_service_pri_map);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: service map profile - Failed.\n")));
        return (rv);
    }
    if (!SOC_WARM_BOOT(unit)) {
        /*
         * Initialize ingress priority map profile table.
         * All priorities priorities map to priority:'0'.
         */
        for (pri = 0; pri < BCM_OAM_INTPRI_MAX; pri++)
        {
            /* Clear ingress service pri map profile entry. */
            sal_memcpy(&pri_ent[pri], soc_mem_entry_null(unit, mem),
                   soc_mem_entry_words(unit, mem) * sizeof(uint32));

            if (SOC_MEM_FIELD_VALID(unit, mem, OFFSET_VALIDf)) {
            soc_mem_field32_set(unit, mem, &pri_ent[pri], OFFSET_VALIDf, 1);
            }
        }

        
        entries[0] = &pri_ent;
        rv = soc_profile_mem_add(unit, &oc->ing_service_pri_map,
                             (void *)entries, BCM_OAM_INTPRI_MAX,
                             &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: service map init - Failed.\n")));
            return (rv);
        }
    }

    /* Egress Service Priority Map profile table initialization. */
    soc_profile_mem_t_init(&oc->egr_service_pri_map);

    entry_words = (sizeof(egr_service_pri_map_entry_t) / sizeof(uint32));

    mem = EGR_SERVICE_PRI_MAPm;
    rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                &oc->egr_service_pri_map);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Egr service map profile - Failed.\n")));
        return (rv);
    }

    if (!SOC_WARM_BOOT(unit)) {

        /*
         * Initialize Egress priority map profile table.
         * All priorities priorities map to priority:'0'.
         */
        for (pri = 0; pri < BCM_OAM_INTPRI_MAX; pri++)
        {
            /* Clear ingress service pri map profile entry. */
            sal_memcpy(&egr_pri_ent[pri], soc_mem_entry_null(unit, mem),
                   soc_mem_entry_words(unit, mem) * sizeof(uint32));

            if (SOC_MEM_FIELD_VALID(unit, mem, OFFSET_VALIDf)) {
                soc_mem_field32_set(unit, mem, 
                                    &egr_pri_ent[pri], OFFSET_VALIDf, 1);
            }
        }

        
        entries[0] = &egr_pri_ent;
        rv = soc_profile_mem_add(unit, &oc->egr_service_pri_map,
                             (void *)entries, BCM_OAM_INTPRI_MAX,
                             &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Egress service map init - Failed.\n")));
            return (rv);
        }
    }

    /* OAM Opcode Control profile table initialization. */
    soc_profile_mem_t_init(&oc->oam_opcode_control_profile);

    entry_words = sizeof(oam_opcode_control_profile_entry_t)
                         / sizeof(uint32);

    mem = OAM_OPCODE_CONTROL_PROFILEm;
    rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                &oc->oam_opcode_control_profile);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: opcode control profile - Failed.\n")));
        return (rv);
    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Create default opcode control profile */
        sal_memset(&opcode_entry, 0, sizeof(oam_opcode_control_profile_entry_t));
        rv = _bcm_kt2_oam_opcode_profile_entry_init(unit, mem, &opcode_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Opcode profile init failed "
                                   "  %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
        /* Add entry to profile table.  */
        entries[0] = &opcode_entry;
        rv = soc_profile_mem_add(unit, &oc->oam_opcode_control_profile,
                             (void *)entries, 1, &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Opcode profile table is full - %s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        }
    }

    /* Egress OAM Opcode Control profile table initialization. */
    soc_profile_mem_t_init(&oc->egr_oam_opcode_control_profile);
    entry_words = sizeof(egr_oam_opcode_control_profile_entry_t)
                         / sizeof(uint32);
    mem = EGR_OAM_OPCODE_CONTROL_PROFILEm;
    rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                &oc->egr_oam_opcode_control_profile);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Egress opcode control profile- Failed.\n")));
        return (rv);
    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Create default opcode control profile */
        sal_memset(&egr_opcode_entry, 0, 
                             sizeof(egr_oam_opcode_control_profile_entry_t));
        rv = _bcm_kt2_oam_opcode_profile_entry_init(unit, mem, &egr_opcode_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Opcode profile init failed "
                                   "  %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
        /* Add entry to profile table.  */
        entries[0] = &egr_opcode_entry;
        rv = soc_profile_mem_add(unit, &oc->egr_oam_opcode_control_profile,
                             (void *)entries, 1, &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Egr Opcode profile table is full - %s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        }
    }

    /* Ingress OAM dglp profile table initialisation */
    soc_profile_mem_t_init(&oc->ing_oam_dglp_profile);

    entry_words = sizeof(ing_oam_dglp_profile_entry_t) / sizeof(uint32);

    mem = ING_OAM_DGLP_PROFILEm;
    rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                &oc->ing_oam_dglp_profile);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: ING OAM DGLP profile create - Failed.\n")));
        return (rv);
    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Add default profile */
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        /* put local CPU as default dglp */
        dglp = CMIC_PORT(unit);
        dglp |= ((modid << DGLP_MODULE_ID_SHIFT_BITS)); 

        soc_mem_field32_set(unit, ING_OAM_DGLP_PROFILEm, 
                        &ing_dglp_profile_entry, DGLPf, dglp);
        entries[0] = &ing_dglp_profile_entry;

        rv = soc_profile_mem_add(unit, &oc->ing_oam_dglp_profile,
                             (void *)entries, 1, &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM DGLP profile table is full - %s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        }
    }
    

    /* Egress OAM dglp profile table initialisation */
    soc_profile_mem_t_init(&oc->egr_oam_dglp_profile);

    entry_words = sizeof(egr_oam_dglp_profile_entry_t) / sizeof(uint32);

    mem = EGR_OAM_DGLP_PROFILEm;
    rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                &oc->egr_oam_dglp_profile);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: EGR OAM DGLP profile create - Failed.\n")));
        return (rv);
    }
    if (!SOC_WARM_BOOT(unit)) {
        profile_index = 0;
        soc_mem_field32_set(unit, EGR_OAM_DGLP_PROFILEm, 
                        &egr_dglp_profile_entry, DGLPf, dglp);
        entries[0] = &egr_dglp_profile_entry;

        rv = soc_profile_mem_add(unit, &oc->egr_oam_dglp_profile,
                             (void *)entries, 1, &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM DGLP profile table is full - %s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        }
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_control_free
 * Purpose:
 *     Free OAM control structure resources allocated by this unit.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_control_free(int unit, _bcm_oam_control_t *oc)
{
    int status = 0;
    _kt2_oam_control[unit] = NULL;

    if (NULL == oc) {
        /* Module already un-initialized. */
        return (BCM_E_NONE);
    }

    /* Free protection mutex. */
    if (NULL != oc->oc_lock) {
        sal_mutex_destroy(oc->oc_lock);
    }

    /* Free hash data storage memory. */
    if (NULL != oc->oam_hash_data) {
        sal_free(oc->oam_hash_data);
    }

    /* Destory endpoint hash table. */
    if (NULL != oc->ma_mep_htbl) {
        status = shr_htb_destroy(&oc->ma_mep_htbl, NULL);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Freeing ma_mep_htbl failed\n")));
        }
    }

    /* Destory group indices list. */
    if (NULL != oc->group_pool) {
        shr_idxres_list_destroy(oc->group_pool);
        oc->group_pool = NULL;
    }

    /* Destroy endpoint indices list. */
    if (NULL != oc->mep_pool) {
        shr_idxres_list_destroy(oc->mep_pool);
        oc->mep_pool = NULL;
    }

    /* Destroy local endpoint indices list. */
    if (NULL != oc->lmep_pool) {
        shr_idxres_list_destroy(oc->lmep_pool);
        oc->lmep_pool = NULL;
    }

    /* Destroy remote endpoint indices list. */
    if (NULL != oc->rmep_pool) {
        shr_idxres_list_destroy(oc->rmep_pool);
        oc->rmep_pool = NULL;
    }

    /* Destroy ingress and egress group indices list. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_pool_destroy(oc));

    if (NULL != oc->egr_ma_idx_pool) {
        shr_idxres_list_destroy(oc->egr_ma_idx_pool);
        oc->egr_ma_idx_pool = NULL;
    }

    /* Destroy LM counter indices list. */
    if (NULL != oc->ing_lm_ctr_pool[0]) {
        shr_aidxres_list_destroy(oc->ing_lm_ctr_pool[0]);
        oc->ing_lm_ctr_pool[0] = NULL;
    }
    if (NULL != oc->ing_lm_ctr_pool[1]) {
        shr_aidxres_list_destroy(oc->ing_lm_ctr_pool[1]);
        oc->ing_lm_ctr_pool[1] = NULL;
    }
    if (soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        /* Destroy bhh indices list. */
        if (NULL != oc->bhh_pool) {
            shr_idxres_list_destroy(oc->bhh_pool);
            oc->bhh_pool = NULL;
        }
        if (NULL != oc->dma_buffer){
            soc_cm_sfree(oc->unit, oc->dma_buffer);
            oc->dma_buffer = NULL;
        }
        if (NULL != oc->dmabuf_reply){
            soc_cm_sfree(oc->unit, oc->dmabuf_reply);
            oc->dmabuf_reply = NULL;
        }

        if (NULL != oc->ing_lm_sec_mep_ctr_pool) {
            shr_aidxres_list_destroy(oc->ing_lm_sec_mep_ctr_pool);
            oc->ing_lm_sec_mep_ctr_pool = NULL;
        }
#endif
    }
    if (soc_feature(unit, soc_feature_oam_pm)) {
#if defined (INCLUDE_BHH)
        if (_BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc)) {
            soc_cm_sfree(oc->unit, oc->pm_bhh_dma_buffer);
        }
#endif
    }
    /* Free group memory. */
    if (NULL != oc->group_info) {
        sal_free(oc->group_info);
    }

    /* Free RMEP H/w to logical index mapping memory. */
    if (NULL != oc->remote_endpoints) {
        sal_free(oc->remote_endpoints);
    }

    /* Destroy ingress serivce priority mapping profile. */
    if (NULL != oc->ing_service_pri_map.tables) {
        soc_profile_mem_destroy(unit, &oc->ing_service_pri_map);
    }
    /* Destroy Egress serivce priority mapping profile. */
    if (NULL != oc->egr_service_pri_map.tables) {
        soc_profile_mem_destroy(unit, &oc->egr_service_pri_map);
    }
    /* Destroy Ingress OAM opcode control profile. */
    if (NULL != oc->oam_opcode_control_profile.tables) {
        soc_profile_mem_destroy(unit, &oc->oam_opcode_control_profile);
    }
    /* Destroy Egress OAM opcode control profile. */
    if (NULL != oc->egr_oam_opcode_control_profile.tables) {
        soc_profile_mem_destroy(unit, &oc->egr_oam_opcode_control_profile);
    }
    /* Destroy Ingress DGLP profile */
    if (NULL != oc->ing_oam_dglp_profile.tables) {
        soc_profile_mem_destroy(unit, &oc->ing_oam_dglp_profile);
    }
    /* Destroy Egress DGLP profile */
    if (NULL != oc->egr_oam_dglp_profile.tables) {
        soc_profile_mem_destroy(unit, &oc->egr_oam_dglp_profile);
    }

#if defined(INCLUDE_BHH) 
    if (soc_feature(unit, soc_feature_oam_pm)) {
        if (kt2_pm_profile_control[unit]) {
            sal_free(kt2_pm_profile_control[unit]);
            kt2_pm_profile_control[unit] = NULL;
        }
    }
#endif /* INCLUDE_BHH */


    /* Free OAM control structure memory. */
    sal_free(oc);
    oc = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_handle_interrupt
 * Purpose:
 *     Process OAM interrupts generated by endpoints.
 * Parameters:
 *     unit  -  (IN) BCM unit number.
 *     field -  (IN) fault field.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_handle_interrupt(int unit, soc_field_t field)
{
    _bcm_oam_interrupt_t *intr;             /* OAM interrupt.                */
    _bcm_oam_control_t   *oc;               /* OAM control structure.        */
    uint32               intr_rval;         /* Interrupt register value.     */
    uint32               intr_cur_status;   /* Interrupt status.             */
    uint32               flags;             /* Interrupt flags.              */
    bcm_oam_group_t      grp_index;         /* MA group index.               */
    bcm_oam_endpoint_t   remote_ep_index;   /* Remote Endpoint index.        */
    int                  intr_multi;        /* Event occured multiple times. */
    int                  intr_count;        /* No. of times event detected.  */
    int                  rv;                /* Operation return status       */
    _bcm_oam_event_handler_t *e_handler;    /* Pointer to Event handler.     */
    rmep_entry_t       rmep_entry;          /* Remote endpoint table entry.  */
    uint32 rmep_last_interface_status = 0;  /* Last Rx Interface TLV from RMEP.  */

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    /* Loop through the supported interrupts for this device. */
    for (intr = _kt2_oam_interrupts; intr->status_register != INVALIDr;
         intr++) {

        rv = soc_reg32_get(unit, intr->status_register,
                           REG_PORT_ANY, 0, &intr_rval);
        if (BCM_FAILURE(rv)) {
            
            continue;
        }

        rv = soc_reg32_set(unit, intr->status_register,
                           REG_PORT_ANY, 0, 0);
        if (BCM_FAILURE(rv)) {
            continue;
        }

        /* Get status of interrupt from hardware. */
        intr_cur_status = soc_reg_field_get(unit, intr->status_register,
                                            intr_rval, VALIDf);
        if (0 == intr_cur_status) {
            /* This interrupt event is not valid, so continue. */
            continue;
        }

        /* Get this interrupt event counter value. */
        intr_count = oc->event_handler_cnt[intr->event_type];

        /* Check if the interrupt is set. */
        if ((1 == intr_cur_status) && (intr_count > 0)) {
            flags = 0;

            /* Get MA group index for this interrupt. */
            if (INVALIDf != intr->group_index_field ) {
                grp_index = soc_reg_field_get(unit, intr->status_register,
                                              intr_rval,
                                              intr->group_index_field);
            } else {
                /* Group not valid for this interrupt. */
                grp_index = BCM_OAM_GROUP_INVALID;
            }

            /* Get H/w index of RMEP for this interrupt. */
            if (INVALIDf != intr->endpoint_index_field ) {
                remote_ep_index = soc_reg_field_get(unit, intr->status_register,
                                              intr_rval,
                                              intr->endpoint_index_field);

                sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));
                rv = READ_RMEPm(unit, MEM_BLOCK_ANY, remote_ep_index, &rmep_entry);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                              "OAM Error: RMEP table read failed "
                              "ep_index: %d ep_id: %d %s.\n"), remote_ep_index,
                              oc->remote_endpoints[remote_ep_index],
                              bcm_errmsg(rv)));
                    continue;
                }
                rmep_last_interface_status = soc_RMEPm_field32_get(unit, &rmep_entry,
                                                                   RMEP_LAST_INTERFACE_STATUSf);

                /* Get Logical index from H/w index for this RMEP. */
                remote_ep_index = oc->remote_endpoints[remote_ep_index];
            } else {
                /* Endpoint not valid for this interrupt. */
                remote_ep_index = BCM_OAM_ENDPOINT_INVALID;
            }

            /* Get interrupt MULTIf status. */
            intr_multi = soc_reg_field_get(unit, intr->status_register,
                                           intr_rval, MULTIf);
            if (1 == intr_multi) {
                /*
                 * Interrupt event asserted more than once.
                 * Set flags status bit to indicate event multiple occurance.
                 */
                flags |= BCM_OAM_EVENT_FLAGS_MULTIPLE;
            }

            /* Check and call all the handlers registerd for this event. */
            for (e_handler = oc->event_handler_list_p; e_handler != NULL;
                 e_handler = e_handler->next_p) {

                /* Check if an event handler is register for this event type. */
                if (SHR_BITGET(e_handler->event_types.w, intr->event_type)) {

                    /* Call the event handler with the call back parameters. */
                    if ((intr->event_type == bcmOAMEventEndpointInterfaceDown) ||
                        (intr->event_type == bcmOAMEventEndpointInterfaceTesting) ||
                        (intr->event_type == bcmOAMEventEndpointInterfaceUnkonwn) ||
                        (intr->event_type == bcmOAMEventEndpointInterfaceDormant) ||
                        (intr->event_type == bcmOAMEventEndpointInterfaceNotPresent) ||
                        (intr->event_type == bcmOAMEventEndpointInterfaceLLDown)) {
                        switch (rmep_last_interface_status) {
                            case _BCM_OAM_TLV_INTERFACE_DOWN:
                                intr->event_type = bcmOAMEventEndpointInterfaceDown;
                                break;

                            case _BCM_OAM_TLV_INTERFACE_TESTING:
                                intr->event_type = bcmOAMEventEndpointInterfaceTesting;
                                break;

                            case _BCM_OAM_TLV_INTERFACE_UNKNOWN:
                                intr->event_type = bcmOAMEventEndpointInterfaceUnkonwn;
                                break;

                            case _BCM_OAM_TLV_INTERFACE_DORMANT:
                                intr->event_type = bcmOAMEventEndpointInterfaceDormant;
                                break;

                            case _BCM_OAM_TLV_INTERFACE_NOTPRESENT:
                                intr->event_type = bcmOAMEventEndpointInterfaceNotPresent;
                                break;

                            case _BCM_OAM_TLV_INTERFACE_LLDOWN:
                                intr->event_type = bcmOAMEventEndpointInterfaceLLDown;
                                break;

                            default:
                                break;
                        }
                    }
                    e_handler->cb(unit, flags, intr->event_type, grp_index,
                                      remote_ep_index, e_handler->user_data);
                }
            }
        }
    }

    _BCM_OAM_UNLOCK(oc);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_events_unregister
 * Purpose:
 *     Unregister all OAM events for this unit.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to OAM control structure.
 * Retruns:
 *     BCM_E_NONE
 */
STATIC int
_bcm_kt2_oam_events_unregister(int unit, _bcm_oam_control_t *oc)
{
    _bcm_oam_event_handler_t *e_handler; /* Pointer to event handler list. */
    _bcm_oam_event_handler_t *e_delete;  /* Event handler to be freed.     */

    /* Control lock taken by the calling routine. */

    e_handler = oc->event_handler_list_p;

    while (e_handler != NULL) {
        e_delete = e_handler;
        e_handler = e_handler->next_p;
        sal_free(e_delete);
    }

    oc->event_handler_list_p = NULL;

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_kt2_oam_clear_rmep
 * Purpose:
 *     Delete RMEP entry or Install fresh RMEP entry in the Hardware, clearing 
 *     the faults.
 *      
 * Parameters:
 *     unit       - (IN) BCM device number
 *     h_data_p   - (IN) Pointer to Endpoint info associated with RMEP entry 
 *                       being cleared. 
 *     valid      - (IN) 0 => Delete the H/w entry
 *                       1 => Install fresh entry with faults cleared.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_clear_rmep (int unit, _bcm_oam_hash_data_t *h_data_p, int valid)
{

    rmep_entry_t       rmep_entry;          /* RMEP table entry.              */
    int                rv = BCM_E_INTERNAL; /* Operation return status entry  */
    uint32             oam_cur_time;    /* Current time in H/w state machine  */

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM: EP id %d, valid %d\n"), 
                 h_data_p->remote_index, valid));
    
    if (h_data_p == NULL) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM ERR: Arg h_data_p NULL check failed\n")));
        return BCM_E_INTERNAL;
    }

    /* RMEP table programming. */
    sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));

    /* if valid==0 delete RMEP entry, else create a clean RMEP entry */
    if (!(valid)) {
        rv = WRITE_RMEPm(unit, MEM_BLOCK_ALL, h_data_p->remote_index,
                         &rmep_entry);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM ERR: Deleting RMEP entry failied\n")));
        }
        return (rv);
    }

    /* Set the MA group index. */
    soc_RMEPm_field32_set(unit, &rmep_entry, MAID_INDEXf, 
                          h_data_p->group_index);

    /*
     * The following steps are necessary to enable CCM timeout events
     * without having received any CCMs.
     */
    soc_RMEPm_field32_set(unit, &rmep_entry, RMEP_TIMESTAMP_VALIDf, 1);

    BCM_IF_ERROR_RETURN(READ_OAM_CURRENT_TIMEr(unit, &oam_cur_time));

    soc_RMEPm_field32_set(unit, &rmep_entry, RMEP_TIMESTAMPf, oam_cur_time);

    soc_RMEPm_field32_set(unit, &rmep_entry, RMEP_RECEIVED_CCMf,
                         _bcm_kt2_oam_ccm_msecs_to_hw_encode(h_data_p->period));
    /* End of timeout setup */


    soc_RMEPm_field32_set(unit, &rmep_entry, VALIDf, 1);

    rv = WRITE_RMEPm(unit, MEM_BLOCK_ALL, h_data_p->remote_index,
                     &rmep_entry);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM ERR: Clearing RMEP entry failied\n")));
        return (rv);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_kt2_oam_clear_ma_state
 * Purpose:
 *     Delete MA_STATE entry or Install fresh MA_STATE entry in the Hardware, 
 *     Clearing the faults.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN) Group info associated with MA_STATE entry being 
 *                       cleared.
 *     index      - (IN) H/w index of MA_STATE entry to be modified. 
 *     valid      - (IN) 0 => Delete the H/w entry
 *                       1 => Install fresh entry with faults cleared.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_clear_ma_state(int unit, _bcm_oam_group_data_t *group_info, 
                            int index, int valid)
{

    ma_state_entry_t       ma_state_entry;       /* MA State table entry.     */
    
    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM *group_info %p, index %d, valid %d\n"), 
                 group_info, index, valid));
               
    if (group_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM ERR: Arg group_info NULL check failed\n")));
        return BCM_E_INTERNAL;
    }

    sal_memset(&ma_state_entry, 0, sizeof(ma_state_entry_t));
    
    /* Mark the entry as valid. */
    soc_MA_STATEm_field32_set(unit, &ma_state_entry, VALIDf, valid);

    /* Set the lowest alarm priority info. */
    if (valid) {
        soc_MA_STATEm_field32_set(unit, &ma_state_entry, LOWESTALARMPRIf,
                                  group_info->lowest_alarm_priority);
    }

    /* Write group information to hardware table. */
    SOC_IF_ERROR_RETURN(WRITE_MA_STATEm(unit, MEM_BLOCK_ALL, index,
                                        &ma_state_entry));
    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_kt2_oam_group_recreate
 * Purpose:
 *     Recreate MA_STATE and RMEP entries, due to current H/w limitation
 *     Currently there is a race condition between H/w and Software
 *     When RMEP is deleted and if it has any faults then, 
 *     MA_STATE remote fault counters need to be decremented by software.
 *     However it may so happen that after reading RMEP faults and just
 *     before RMEP deletion, the fault gets cleared and H/w already decremented
 *     the MA_STATE table, now when software decrements the MA_STATE it would
 *     get MA_STATE table into an inconsistent state.
 *     As a solution we always recreate the entire group on a RMEP deletion.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     group_id - (IN) Group Index, 
 *                as per current implementation this is same as MA_STATE index.
 * Returns:
 *     BCM_E_XXX
 * Expects:
 *      OAM LOCK
 */
STATIC int
_bcm_kt2_oam_group_recreate(int unit, int index)
{
    _bcm_oam_control_t      *oc;                /* OAM control structure.     */
    _bcm_oam_group_data_t   *group_info;        /* OAM group info             */
    _bcm_oam_ep_list_t      *cur_ep_ptr = NULL; /* Pointer to group's EP list */
                                                /* entry.                     */
    int                     rv = BCM_E_NONE;    /* Operation return status.   */

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get the handle to group info */
    group_info = &(oc->group_info[index]);
    
    /*if unused Group, just clear the MA_STATE index, including the valid bit*/
    if (!(group_info->in_use)) {
        LOG_WARN(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "OAM, WARN: Recieved group recreate request for " 
                              "unused Group Id %d\n"), index));
    
        rv = _bcm_kt2_oam_clear_ma_state(unit, group_info, index, 0);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: MA_STATE clear failed group id %d - "
                                   "%s.\n"), index, bcm_errmsg(rv)));
            return rv;
        }

    }
    
    /* 1. Traverse and delete the RMEPs in the group */
    if (group_info->ep_list != NULL) {
        cur_ep_ptr = *(group_info->ep_list);
    }
    while (cur_ep_ptr) {
        if (cur_ep_ptr->ep_data_p->is_remote && cur_ep_ptr->ep_data_p->in_use) {
            rv = _bcm_kt2_oam_clear_rmep(unit, cur_ep_ptr->ep_data_p, 0);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: RMEP delete failed Ep id %d - "
                                       "%s.\n"), cur_ep_ptr->ep_data_p->ep_id, bcm_errmsg(rv)));
                return rv;
            }
        }
        cur_ep_ptr = cur_ep_ptr->next;
    }

    /* 2. Clear the MA_STATE table faults */
    rv = _bcm_kt2_oam_clear_ma_state(unit, group_info, index, 1);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MA_STATE clear failed group id %d - "
                               "%s.\n"), index, bcm_errmsg(rv)));
        return rv;
    }

    /* 3. Recreate the RMEP table entries. Clearing the faults. */
    if (group_info->ep_list != NULL) {
        cur_ep_ptr = *(group_info->ep_list);
    }
    while (cur_ep_ptr) {
        if (cur_ep_ptr->ep_data_p->is_remote && cur_ep_ptr->ep_data_p->in_use) {
            rv = _bcm_kt2_oam_clear_rmep(unit, cur_ep_ptr->ep_data_p, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: RMEP clear failed EP id %d - "
                                       "%s.\n"), cur_ep_ptr->ep_data_p->ep_id, bcm_errmsg(rv)));
                return rv;
            }
        }
        cur_ep_ptr = cur_ep_ptr->next;
    }
    
    return rv;
}


/*
 * Function:
 *     _bcm_kt2_oam_ser_handler
 * Purpose:
 *     Soft Error Correction routine for MA_STATE and RMEP tables.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     mem        - (IN) Memory MA_STATE or RMEP.
 *     index      - (IN) Memory Index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_ser_handler(int unit, soc_mem_t mem, int index)
{
    int                     rv = BCM_E_NONE;    /* Operation return status.   */
    _bcm_oam_control_t      *oc = NULL;         /* OAM control structure.     */
    bcm_oam_endpoint_t      remote_ep_index;    /* Remote Endpoint index.     */
    _bcm_oam_hash_data_t    *ep_data = NULL;    /* Remote Endpoint hash data  */

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM SER on mem %s, index %d\n"), 
                 SOC_MEM_NAME(unit, mem), index));
              
    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    switch(mem){
        case MA_STATEm:
            _BCM_OAM_GROUP_INDEX_VALIDATE(index);
            _BCM_OAM_LOCK(oc);
            rv = _bcm_kt2_oam_group_recreate(unit, index);
            _BCM_OAM_UNLOCK(oc);
            break;
            
        case RMEPm:
            _BCM_OAM_RMEP_INDEX_VALIDATE(index);
            _BCM_OAM_LOCK(oc);
            /* Get the logical index from H/w RMEP index */
            remote_ep_index = oc->remote_endpoints[index];
            if(remote_ep_index == BCM_OAM_ENDPOINT_INVALID) {
                /* If endpoint not in use, just clear the RMEP entry in memory */
                rmep_entry_t rmep_entry;
                sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));

                LOG_WARN(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d), WARN: Recieved Parity Error on"
                                    "index %d & invalid Remote Id\n"), unit, index));
                /* Just clear this entry including valid bit */
                rv = WRITE_RMEPm(unit, MEM_BLOCK_ALL, index,
                                                     &rmep_entry);
            } else {

                /* Get handle to Hash data */
                ep_data = &(oc->oam_hash_data[remote_ep_index]);

                if (!(ep_data->in_use)) {
                    /* If endpoint not in use, just clear the RMEP entry in memory */
                    LOG_WARN(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM, WARN: Recieved Parity Error on"
                                        "unused Remote Id %d\n"), remote_ep_index));
                    /* Just clear this entry including valid bit */
                    rv = _bcm_kt2_oam_clear_rmep(unit, ep_data, 0);
                } else {
                    /* Get the group Id from EP data, call group recreate routine */
                    rv = _bcm_kt2_oam_group_recreate(unit, ep_data->group_index);
                }
            }
            _BCM_OAM_UNLOCK(oc);
            break;

        default:
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM, ERR: Invalid mem in OAM SER correction "
                                   "routine %s\n"), SOC_MEM_NAME(unit, mem)));
            return BCM_E_INTERNAL;
    }

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM SER completed on mem %s, index %d, rv %d\n"), 
                 SOC_MEM_NAME(unit, mem), index, rv));
    
    return rv;
    
}

/*
 * Function:
 *     _bcm_kt2_oam_group_name_mangle
 * Purpose:
 *     Build tbe group name for hardware table write.
 * Parameters:
 *     name_p         -  (IN) OAM group name.
 *     mangled_name_p -  (IN/OUT) Buffer to write the group name in hardware
 *                       format.
 * Retruns:
 *     None
 */
STATIC void
_bcm_kt2_oam_group_name_mangle(uint8 *name_p,
                               uint8 *mangled_name_p)
{
    uint8  *byte_p;    /* Pointer to group name buffer.       */
    int    bytes_left; /* Number of bytes left in group name. */

    bytes_left = BCM_OAM_GROUP_NAME_LENGTH;
    byte_p = (name_p + BCM_OAM_GROUP_NAME_LENGTH - 1);

    while (bytes_left > 0) {
        *mangled_name_p = *byte_p;
        ++mangled_name_p;
        --byte_p;
        --bytes_left;
    }
}

/*
 * Function:
 *      _bcm_kt2_oam_read_clear_faults
 * Purpose:
 *     Clear OAM group and endpoint faults on hardware table read operation.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     index    - (IN) Group/Endpoint hardware table index
 *     mem      - (IN) Memory/table value
 *     entry    - (IN) Pointer to group/endpoint entry
 *     ma_mep   - (IN) Pointer to group/endpoint info structure
 * Returns:
 *     BCM_E_NONE      - No errors.
 *     BCM_E_XXX       - Otherwise.
 */
STATIC int
_bcm_kt2_oam_read_clear_faults(int unit, int index, soc_mem_t mem,
                               uint32 *entry, void *ma_rmep)
{
    bcm_oam_group_info_t    *group_info_p;  /* Pointer to group info         */
                                            /* structure.                    */
    bcm_oam_endpoint_info_t *ep_info_p;     /* Pointer to endpoint info      */
                                            /* structure.                    */
    _bcm_oam_fault_t        *faults_list;   /* Pointer to faults list.       */
    uint32                  *faults;        /* Faults flag bits.             */
    uint32                  *p_faults;      /* Persistent faults bits.       */
    uint32                  clear_p_faults; /* Clear persistent faults bits. */
    uint32                  rval = 0;       /* Hardware register value.      */
    uint32                  clear_mask = 0; /* Mask to clear persistent faults*/

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_read_clear_faults index=%d "
                           "Table=%d.\n"), index, mem));

    /* Switch on memory name. */
    switch (mem) {
        /* OAM group state table. */
        case MA_STATEm:
            /* Set the pointer to the start of the group faults array. */
            faults_list = _kt2_oam_group_faults;

            /* Typecast to group information structure pointer. */
            group_info_p = (bcm_oam_group_info_t *) ma_rmep;

            /* Get group faults information. */
            faults = &group_info_p->faults;
            p_faults = &group_info_p->persistent_faults;
            clear_p_faults = group_info_p->clear_persistent_faults;
            break;

        /* OAM remote endpoint table. */
        case RMEPm:
            faults_list = _kt2_oam_endpoint_faults;
            ep_info_p = (bcm_oam_endpoint_info_t *) ma_rmep;
            faults = &ep_info_p->faults;
            p_faults = &ep_info_p->persistent_faults;
            clear_p_faults = ep_info_p->clear_persistent_faults;
            break;

        default:
            return (BCM_E_NONE);
    }

    /* Loop on list of valid faults. */
    for (;faults_list->mask != 0; ++faults_list) {

        /* Get  faults status. */
        if (0 != soc_mem_field32_get(unit, mem, entry,
                                     faults_list->current_field)) {
            *faults |= faults_list->mask;
        }

        /* Get sticky faults status. */
        if (0 != soc_mem_field32_get(unit, mem, entry,
                                     faults_list->sticky_field)) {

            *p_faults |= faults_list->mask;

            /*
             * If user has request to clear persistent faults,
             * then set BITS_TO_CLEARf field in the register buffer.
             */
            if (clear_p_faults) {
                clear_mask |= faults_list->clear_sticky_mask;
            }
        }
    }

    /* Check if faults need to be cleared. */
    if (clear_mask && clear_p_faults) {

        LOG_VERBOSE(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM: clear_mask %d.\n"),
                     clear_mask));

        soc_reg_field_set(unit, CCM_READ_CONTROLr, &rval, BITS_TO_CLEARf,
                          clear_mask);

        /* Enable clearing of faults. */
        soc_reg_field_set(unit, CCM_READ_CONTROLr, &rval, ENABLE_CLEARf, 1);

        
        if (MA_STATEm == mem) {
            soc_reg_field_set(unit, CCM_READ_CONTROLr, &rval, MEMORYf, 0);
        } else {
            soc_reg_field_set(unit, CCM_READ_CONTROLr, &rval, MEMORYf, 1);
        }

        soc_reg_field_set(unit, CCM_READ_CONTROLr, &rval, INDEXf, index);

        /* Update read control register. */
        BCM_IF_ERROR_RETURN(WRITE_CCM_READ_CONTROLr(unit, rval));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_kt2_oam_get_group
 * Purpose:
 *     Get OAM group information.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     group_index  - (IN) Group hardware table index
 *     group        - (IN) Pointer to group array list
 *     group_info   - (IN/OUT) Pointer to group info structure
 * Returns:
 *     BCM_E_NONE      - No errors.
 *     BCM_E_XXX       - Otherwise.
 */
STATIC int
_bcm_kt2_oam_get_group(int unit, bcm_oam_group_t group_index,
                       _bcm_oam_group_data_t *group_p,
                       bcm_oam_group_info_t *group_info)
{
    maid_reduction_entry_t  maid_reduction_entry; /* MAID reduction entry.    */
    ma_state_entry_t        ma_state_entry;       /* MA_STATE table entry.    */
    int                     rv;                   /* Operation return status. */

    group_info->id = group_index;

    BCM_IF_ERROR_RETURN(READ_MA_STATEm(unit, MEM_BLOCK_ANY,
                                       group_index,
                                       &ma_state_entry));

    group_info->lowest_alarm_priority = soc_MA_STATEm_field32_get
                                            (unit, &ma_state_entry,
                                             LOWESTALARMPRIf);

    rv = _bcm_kt2_oam_read_clear_faults(unit, group_index,
                                        MA_STATEm,
                                        (uint32 *) &ma_state_entry,
                                        group_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Clean Faults Group ID=%d- Failed.\n"),
                   group_index));
        return (rv);
    }
    if (group_info->flags & BCM_OAM_GROUP_GET_FAULTS_ONLY) {
        group_info->flags &= ~BCM_OAM_GROUP_GET_FAULTS_ONLY;
        return BCM_E_NONE;
    }
    sal_memcpy(group_info->name, group_p[group_index].name,
               BCM_OAM_GROUP_NAME_LENGTH);

    BCM_IF_ERROR_RETURN(READ_MAID_REDUCTIONm(unit, MEM_BLOCK_ANY,
                                             group_index,
                                             &maid_reduction_entry));

    if (1 == soc_MAID_REDUCTIONm_field32_get(unit, &maid_reduction_entry,
                                             SW_RDIf)) {
        group_info->flags |= BCM_OAM_GROUP_REMOTE_DEFECT_TX;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_group_ep_list_add
 * Purpose:
 *     Add an endpoint to a group endpoint linked list.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     group_id - (IN) OAM group ID.
 *     ep_id    - (IN) OAM endpoint ID.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_group_ep_list_add(int unit,
                           bcm_oam_group_t group_id,
                           bcm_oam_endpoint_t ep_id)
{
    _bcm_oam_control_t    *oc;       /* Pointer to OAM control structure. */
    _bcm_oam_group_data_t *group_p;  /* Pointer to group data.            */
    _bcm_oam_hash_data_t  *h_data_p; /* Pointer to endpoint hash data.    */
    _bcm_oam_ep_list_t    *ep_list_p = NULL; /* Pointer to endpoint list. */

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    group_p = &oc->group_info[group_id];
    /* coverity[dead_error_condition] */
    if (NULL == group_p) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Group data access for GID=%d failed"
        /* coverity[dead_error_begin] */
                               " %s.\n"), group_id, bcm_errmsg(BCM_E_INTERNAL)));
        return (BCM_E_INTERNAL);
    }

    h_data_p = &oc->oam_hash_data[ep_id];
    /* coverity[dead_error_condition] */
    if (NULL == h_data_p) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint data access for EP=%d failed"
        /* coverity[dead_error_begin] */
                               " %s.\n"), ep_id, bcm_errmsg(BCM_E_INTERNAL)));
        return (BCM_E_INTERNAL);
    }

    _BCM_OAM_ALLOC(ep_list_p, _bcm_oam_ep_list_t, sizeof(_bcm_oam_ep_list_t),
                "EP list");
    if (NULL == ep_list_p) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint list alloc for EP=%d failed"
                               " %s.\n"), ep_id, bcm_errmsg(BCM_E_MEMORY)));
        return (BCM_E_MEMORY);
    }

    ep_list_p->prev = NULL;
    ep_list_p->ep_data_p = h_data_p;
    if (NULL == (*group_p->ep_list)) {
        /* Add endpoint as head node. */
        ep_list_p->next = NULL;
        *group_p->ep_list = ep_list_p;
    } else {
        /* Add the endpoint to the linked list. */
        ep_list_p->next = *group_p->ep_list;
        (*group_p->ep_list)->prev = ep_list_p;
        *group_p->ep_list = ep_list_p;
    }
    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_group_ep_list_add" 
                           " (GID=%d) (EP=%d).\n"), group_id, ep_id));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_group_ep_list_remove
 * Purpose:
 *     Remove an endpoint to a group endpoint linked list.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     group_id - (IN) OAM group ID.
 *     ep_id    - (IN) OAM endpoint ID.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_group_ep_list_remove(int unit,
                              bcm_oam_group_t group_id,
                              bcm_oam_endpoint_t ep_id)
{
    _bcm_oam_control_t    *oc;       /* Pointer to OAM control structure. */
    _bcm_oam_group_data_t *group_p;  /* Pointer to group data.            */
    _bcm_oam_hash_data_t  *h_data_p; /* Pointer to endpoint hash data.    */
    _bcm_oam_ep_list_t    *cur;      /* Current endpoint node pointer.    */
    _bcm_oam_ep_list_t    *del_node; /* Pointer to a node to be deleted.  */

    /* Control lock already taken by calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    group_p = &oc->group_info[group_id];
    cur =  *group_p->ep_list;
    if (NULL == cur) {
        /* No endpoints to remove from this group. */
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: No endpoints to delete in group"
                               " GID:%d.\n"), group_id));
        return (BCM_E_NONE);
    }

    /* Check if head node needs to be deleated. */
    if (ep_id == cur->ep_data_p->ep_id) {
        /* Delete head node. */
        del_node = *group_p->ep_list;
        if ((*group_p->ep_list)->next) {
            *group_p->ep_list = (*group_p->ep_list)->next;
            (*group_p->ep_list)->prev = NULL;
        } else {
            *group_p->ep_list = NULL;
        }
        sal_free(del_node);
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: Head node delete GID=%d - Success\n"),
                   group_id));
        return (BCM_E_NONE);
    }

    /* Traverse the list and delete the matching node. */
    while (NULL != cur->next->next) {
        h_data_p = cur->next->ep_data_p;
        if (NULL == h_data_p) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Group=%d endpoints access failed -"
                                   " %s.\n"), group_id, bcm_errmsg(BCM_E_INTERNAL)));
            return (BCM_E_INTERNAL);
        }

        if (ep_id == h_data_p->ep_id) {
            del_node = cur->next;
            cur->next = del_node->next;
            del_node->next->prev = cur;
            sal_free(del_node);
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Info: Node delete GID=%d - Success\n"),
                       group_id));
            return (BCM_E_NONE);
        }
        cur = cur->next;
    }

    /* Check if tail node needs to be deleted. */
    h_data_p = cur->next->ep_data_p;
    if (ep_id == h_data_p->ep_id) {
        /* Delete tail node. */
        del_node = cur->next;
        cur->next = NULL;
        sal_free(del_node);
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: Tail node delete GID=%d - Success\n"),
                   group_id));
        return (BCM_E_NONE);
    }

    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _bcm_oam_kt2_remote_mep_hw_set
 * Purpose:
 *     Configure hardware tables for a remote endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_oam_kt2_remote_mep_hw_set(int unit,
                           const bcm_oam_endpoint_info_t *ep_info_p)
{
    l3_entry_1_entry_t   l3_entry;          /* Remote view entry.            */
    rmep_entry_t         rmep_entry;        /* Remote MEP entry.             */
    uint32               oam_cur_time;      /* Current time.                 */
    _bcm_oam_hash_data_t *h_data_p = NULL;  /* Hash data pointer.            */
    int                  rv;                /* Operation return status.      */
    _bcm_oam_control_t   *oc;               /* Pointer to control structure. */
    const bcm_oam_endpoint_info_t *ep_p;    /* Pointer to endpoint info.     */

    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Initialize local endpoint info pointer. */
    ep_p = ep_info_p;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get endpoint hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_p->id];
    if (0 == h_data_p->in_use) {
        return (BCM_E_INTERNAL);
    }

    /* RMEP table programming. */
    sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));

    /* Set the MA group index. */
    soc_RMEPm_field32_set(unit, &rmep_entry, MAID_INDEXf,
                          ep_p->group);

    /*
     * The following steps are necessary to enable CCM timeout events
     * without having received any CCMs.
     */
    soc_RMEPm_field32_set(unit, &rmep_entry,
                          RMEP_TIMESTAMP_VALIDf, 1);

    BCM_IF_ERROR_RETURN
        (READ_OAM_CURRENT_TIMEr(unit, &oam_cur_time));

    soc_RMEPm_field32_set(unit, &rmep_entry, RMEP_TIMESTAMPf,
                          oam_cur_time);

    soc_RMEPm_field32_set(unit, &rmep_entry,
                          RMEP_RECEIVED_CCMf,
                         _bcm_kt2_oam_ccm_msecs_to_hw_encode(h_data_p->period));
    /* End of timeout setup */

    soc_RMEPm_field32_set(unit, &rmep_entry, VALIDf, 1);

    rv = WRITE_RMEPm(unit, MEM_BLOCK_ALL, h_data_p->remote_index,
                     &rmep_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: RMEP table write failed EP=%d %s.\n"),
                   ep_info_p->id, bcm_errmsg(rv)));
        return (rv);
    }

    /* L3 unicast table programming. */
    sal_memset(&l3_entry, 0, sizeof(l3_entry_1_entry_t));

    /* Set the CCM interval. */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set
        (unit, &l3_entry, RMEP__CCMf,
        _bcm_kt2_oam_ccm_msecs_to_hw_encode(h_data_p->period));

    /* Set the entry hardware index. */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, RMEP__RMEP_PTRf,
                                h_data_p->remote_index);

    /*
     * Construct endpoint RMEP view key for L3 Table entry
     * insert operation.
     */
    _bcm_kt2_oam_rmep_key_construct(unit, h_data_p, &l3_entry);

    /* Mark the entry as valid. */
    soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, VALIDf, 1);

    /* Install entry in hardware. */
    rv = soc_mem_insert(unit, L3_ENTRY_IPV4_UNICASTm,
                        MEM_BLOCK_ALL, &l3_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: L3 table insert failed EP=%d %s.\n"),
                   ep_p->id, bcm_errmsg(rv)));
        return (rv);
    }
    /* Add the H/w index to logical index mapping for RMEP */
    oc->remote_endpoints[h_data_p->remote_index] = ep_p->id; 

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_kt2_oam_find_egr_lmep
 * Purpose:
 *     Search EGR_MP_GROUP table and return the match entry hardware index and
 *     match entry data.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     h_data_p   - (IN) Pointer to endpoint hash data.
 *     entry_idx  - (OUT) Pointer to match entry hardware index.
 *     mp_group_entry_p - (OUT) Pointer to match entry data.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_find_egr_lmep(int unit, const _bcm_oam_hash_data_t *h_data_p,
                       int *entry_idx,
                       egr_mp_group_entry_t *mp_group_entry_p)
{
    egr_mp_group_entry_t mp_group_entry; /* MP group table
                                           entry to search.      */
    int                rv;       /* Operation return status. */

    if (NULL == h_data_p || NULL == entry_idx || NULL == mp_group_entry_p) {
        return (BCM_E_INTERNAL);
    }

    sal_memset(&mp_group_entry, 0, sizeof(egr_mp_group_entry_t));

    /* Construct endpoint egress MP group  Table entry search operation. */
    _bcm_oam_egr_lmep_key_construct(unit, h_data_p, &mp_group_entry);

    soc_mem_lock(unit, EGR_MP_GROUPm);
    /* Perform the search in EGR_MP_GROUP table. */
    rv = soc_mem_search(unit, EGR_MP_GROUPm, MEM_BLOCK_ANY, entry_idx,
                                  &mp_group_entry, mp_group_entry_p, 0);
    soc_mem_unlock(unit, EGR_MP_GROUPm);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error:  Egr MP group entry lookup "
                              "failed vlan=%d port =%x %s.\n"),
                   h_data_p->vlan, h_data_p->dglp, bcm_errmsg(rv)));
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_find_lmep
 * Purpose:
 *     Search LMEP view table and return the match entry hardware index and
 *     match entry data.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     h_data_p   - (IN) Pointer to endpoint hash data.
 *     entry_idx  - (OUT) Pointer to match entry hardware index.
 *     l3_entry_p - (OUT) Pointer to match entry data.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_find_lmep(int unit,  const _bcm_oam_hash_data_t *h_data_p,
                       int *entry_idx,
                       l3_entry_1_entry_t *l3_entry_p)
{
    l3_entry_1_entry_t l3_entry; /* L3 entry to search.      */
    int                rv;       /* Operation return status. */

    if (NULL == h_data_p || NULL == entry_idx || NULL == l3_entry_p) {
        return (BCM_E_INTERNAL);
    }

    sal_memset(&l3_entry, 0, sizeof(l3_entry_1_entry_t));

    /* Construct endpoint LMEP view key for L3 Table entry search operation. */
    _bcm_kt2_oam_lmep_key_construct(unit, h_data_p, &l3_entry);

    /* Take L3 module protection mutex to block any updates. */
    L3_LOCK(unit);

    /* Perform the search in L3_ENTRY table. */
    rv = soc_mem_search(unit, L3_ENTRY_IPV4_UNICASTm, MEM_BLOCK_ANY, entry_idx,
                        &l3_entry, l3_entry_p, 0);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: L3 entry lookup vlan=%d port=%x %s.\n"),
                   h_data_p->vlan, h_data_p->sglp, bcm_errmsg(rv)));
    }

    /* Release L3 module protection mutex. */
    L3_UNLOCK(unit);

    return (rv);
}

STATIC int
_bcm_kt2_oam_find_port_lmep(int unit, const _bcm_oam_hash_data_t *h_data_p,
                            int *stm_index, source_trunk_map_table_entry_t *stm_entry,
                            uint8 *mdl)
{
    port_tab_entry_t port_entry;
    int rv = BCM_E_NONE;
    int port_id, mod_id;

    rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
            h_data_p->src_pp_port, &port_entry);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    *mdl = soc_PORT_TABm_field32_get(unit, &port_entry, MDL_BITMAPf);

    if (*mdl == 0) {
        /* No port mep enabled already on this port */
        return BCM_E_NOT_FOUND;
    }

    port_id = _BCM_OAM_GLP_PORT_GET(h_data_p->sglp);
    mod_id  = _BCM_OAM_GLP_MODULE_ID_GET(h_data_p->sglp);

    /* Derive index to SOURCE_TRUNK_MAP tbl based on module ID and port*/
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, mod_id,
                                               port_id, stm_index));
    BCM_IF_ERROR_RETURN(READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                      *stm_index, stm_entry));
    return BCM_E_NONE;
}

STATIC
int _bcm_kt2_oam_lm_counters_mems_from_pool_get(int pool_id, soc_mem_t *mems)
{
    switch(pool_id) {
        case 0:
            mems[0] = OAM_LM_COUNTERS_0m;
            mems[1] = EGR_OAM_LM_COUNTERS_0m;
            break;
        case 1:
            mems[0] = OAM_LM_COUNTERS_1m;
            mems[1] = EGR_OAM_LM_COUNTERS_1m;
            break;
        default:
            /* should not happen */
            return BCM_E_INTERNAL;
            break;
    }
    return BCM_E_NONE;
}


STATIC int
_bcm_kt2_oam_lm_counters_hw_values_init(int unit, int pool_id, int ctr_index)
{
    oam_lm_counters_0_entry_t entry;
    /* Ingress and egress. 2 memories */
    soc_mem_t mems[2] = {INVALIDm, INVALIDm};
    int index, entry_offset;

    BCM_IF_ERROR_RETURN(
        _bcm_kt2_oam_lm_counters_mems_from_pool_get(pool_id, mems));

    for (index = 0; index < 2; index++) { 
        sal_memset(&entry, 0, sizeof(entry));

        /* 8 entries allocated per endpoint */
        for(entry_offset = 0; entry_offset < 8; entry_offset++) {
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mems[index], MEM_BLOCK_ALL,
                        ctr_index + entry_offset, &entry));
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_oam_lm_counters_hw_values_reset(int unit, int pool_id, int index)
{

    return _bcm_kt2_oam_lm_counters_hw_values_init(unit, pool_id, index);
}

/*
 * Function:
 *     _bcm_kt2_oam_free_counter
 * Purpose:
 *     Free counter from counter pool.
 * Parameters:
 *     unit      - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_free_counter(int unit, 
                          _bcm_oam_hash_data_t *hash_data) 
{
    int                  rv = BCM_E_NONE;/* Operation return status.         */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    int                  pool_id = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if (hash_data->rx_ctr != _BCM_OAM_INVALID_INDEX) {
        pool_id = (hash_data->rx_ctr >> 24);
        rv =  shr_aidxres_list_free(oc->ing_lm_ctr_pool[pool_id], 
                                       (hash_data->rx_ctr & 0xffffff));
        
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_lm_counters_hw_values_reset(unit, pool_id,
                                             hash_data->rx_ctr & 0xffffff));
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LM counter block "
                                  "free failed (EP=%d) - %s.\n"),
                       hash_data->ep_id, bcm_errmsg(rv)));
            return (rv);
        }
        hash_data->rx_ctr = _BCM_OAM_INVALID_INDEX; 
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_alloc_counter
 * Purpose:
 *     Allocate counter from counter pool.
 * Parameters:
 *     unit      - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_alloc_counter(int unit, int pool_id, 
                           shr_idxres_element_t *ctr_index)
{
    int                  rv;             /* Operation return status.         */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Allocate 8 consecutive couters from the pool */
    rv =  shr_aidxres_list_alloc_block(oc->ing_lm_ctr_pool[pool_id],
                                 8, ctr_index);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: LM counter block alloic failed "
                              "- %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }
    return (BCM_E_NONE);
}

#if defined(INCLUDE_BHH)
/*
 * Function:
 *     _bcm_kt2_oam_bhh_sec_mep_alloc_counter
 * Purpose:
 *     Allocate counter from counter pool.
 * Parameters:
 *     unit      - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_bhh_sec_mep_alloc_counter(int unit, shr_idxres_element_t *ctr_index)
{
    int                  rv;             /* Operation return status.         */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Allocate 8 consecutive couters from the pool */
    rv =  shr_aidxres_list_alloc_block(oc->ing_lm_sec_mep_ctr_pool,
                                 8, ctr_index);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: LM counter block alloc failed "
                              "- %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    return (BCM_E_NONE);
}
#endif
/*
 * Function:
 *     _bcm_kt2_oam_counter_set
 * Purpose:
 *     Set Rx counter for MEP 
 * Parameters:
 *     unit      - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_counter_set(int unit, soc_mem_t  mem, 
                         uint32 *entry,
                         shr_idxres_element_t *ctr_index,
                         int *pool_id) 
{

    _bcm_oam_control_t   *oc;        /* Pointer to control structure.  */
    soc_field_t   ctr1_field;
    soc_field_t   ctr2_field;
    int           ctr1_valid = 0;
    int           ctr2_valid = 0;
    int           rv = BCM_E_NONE;/* Operation return status.         */

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if (mem == L3_ENTRY_IPV4_UNICASTm) { 
        ctr1_field = LMEP__CTR1_VALIDf;
        ctr2_field = LMEP__CTR2_VALIDf;
    } else {
        ctr1_field = CTR1_VALIDf;
        ctr2_field = CTR2_VALIDf;
    }

    ctr1_valid = soc_mem_field32_get(unit, mem, 
                                          (uint32 *)entry, ctr1_field);
    ctr2_valid = soc_mem_field32_get(unit, mem, 
                                          (uint32 *)entry, ctr2_field);

    /* if both counters are already used, we can't enable LM on this endpoint */
    if ((1 == ctr1_valid) && (1 == ctr2_valid)) {
        rv =  BCM_E_RESOURCE;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: 2 counters are already "
                               "allocated for EP on this service interface"
                              "%s.\n"), bcm_errmsg(rv)));
        return (rv);
    } else if ((0 == ctr1_valid) && (1 == ctr2_valid)) {
        *pool_id = 0;
        rv = _bcm_kt2_oam_alloc_counter(unit, *pool_id, ctr_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LM counter block alloc - %s.\n"),
                       bcm_errmsg(rv)));
            return (rv);
        }
        ctr1_valid = 1;
    } else if ((0 == ctr2_valid) && (1 == ctr1_valid)) {
        *pool_id = 1;
        rv = _bcm_kt2_oam_alloc_counter(unit, *pool_id, ctr_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LM counter block alloc - %s.\n"),
                       bcm_errmsg(rv)));
            return (rv);
        }
        ctr2_valid = 1;
    } else {
        *pool_id = 0;
        rv = _bcm_kt2_oam_alloc_counter(unit, *pool_id, ctr_index);
        if (BCM_FAILURE(rv)) {
            *pool_id = 1;
            rv = _bcm_kt2_oam_alloc_counter(unit, *pool_id,  ctr_index);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter block alloc - %s.\n"),
                           bcm_errmsg(rv)));
                return (rv);
            }
        }
        ctr1_valid = 1;
    }
    return (rv);
}
/*
 * Function:
 *     _bcm_kt2_oam_service_pri_profile_add
 * Purpose:
 *     Create ING_SERVICE_PRI_MAP or EGR_SERVICE_PRI_MAP profile entry  
 * Parameters:
 *     unit      - (IN) BCM device number
 *     egr       - (IN) Ingress/egess profile
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 *     profile_index - (IN) Index of the profile entry created
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_oam_service_pri_profile_add(int unit, int egr, 
                                     bcm_oam_endpoint_info_t *endpoint_info,
                                     uint32 *profile_index)
{
    int rv = BCM_E_NONE;            
    int i = 0;
    void *entries[1];
    _bcm_oam_control_t   *oc;        /* Pointer to control structure.  */
    soc_mem_t  mem = ING_SERVICE_PRI_MAPm;
    uint32 mem_entries[BCM_OAM_INTPRI_MAX];
    soc_profile_mem_t    *profile; /* profile to be used ingress or egress    */
   
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    if (egr) {
        mem = EGR_SERVICE_PRI_MAPm;
        profile = &oc->egr_service_pri_map;
    } else { 
        profile = &oc->ing_service_pri_map;
    }
    for (i = 0; i < BCM_OAM_INTPRI_MAX; i++) {
        if (endpoint_info->pri_map[i] > _BCM_OAM_SERVICE_PRI_MAX_OFFSET) {
            return BCM_E_PARAM;
        }
        mem_entries[i] = endpoint_info->pri_map[i];
        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSET_VALIDf)) {
            soc_mem_field32_set(unit, mem, &mem_entries[i], OFFSET_VALIDf, 1);
        }
    }
    soc_mem_lock(unit, mem);
    entries[0] = &mem_entries;
    rv = soc_profile_mem_add(unit, profile, (void *) &entries, 
                                          BCM_OAM_INTPRI_MAX, profile_index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    *profile_index = ((*profile_index)/BCM_OAM_INTPRI_MAX);
    soc_mem_unlock(unit, mem);
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_lmep_counters_set
 * Purpose:
 *     Get Tx counters for LMEP 
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_lmep_counters_set(int unit,
                                  bcm_oam_endpoint_info_t *ep_info_p)
{
    _bcm_oam_hash_data_t *hash_data; /* Pointer to endpoint hash data. */
    _bcm_oam_control_t   *oc;        /* Pointer to control structure.  */
    l3_entry_1_entry_t   l3_entry;       /* L3 entry buffer.                 */
    int                  l3_index = -1;  /* L3 entry hardware index.         */
    shr_idxres_element_t ctr_index = 0;
    int                  pool_id = 0;
    egr_mp_group_entry_t egr_mp_group;   /* Egress MP group tbl entry buffer */
    int                  mp_grp_index = 0;
    int                  rv = BCM_E_NONE;/* Operation return status.         */
    uint32               profile_index = 0;
    uint32               direction = 0; /* Up MEP or Down MEP */

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get the stored endpoint information from hash table. */
    hash_data = &oc->oam_hash_data[ep_info_p->id];
    if (0 == hash_data->in_use) {
        return (BCM_E_INTERNAL);
    }

    if (ep_info_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        direction = 1;
    }

    /*For downMEP Rx counters are in Ing Mp group table and for UpMEP
      it is in egr mp group table */
    if (BCM_SUCCESS
        (_bcm_kt2_oam_find_lmep(unit, hash_data, &l3_index, &l3_entry))) {
        if (hash_data->tx_ctr == _BCM_OAM_INVALID_INDEX) {
            /* Allocate counters and configure the same in L3 entry */
            rv = _bcm_kt2_oam_counter_set(unit,
                                          L3_ENTRY_IPV4_UNICASTm,
                                          (uint32 *)&l3_entry,
                                          &ctr_index, &pool_id);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: L3_ENTRY table update failed "
                                      "for EP=%d due to counter allocation failure "
                                      "%s.\n"), ep_info_p->id, bcm_errmsg(rv)));
                return (rv);
            }
            /* MS 8 bits of counter Id is pool id. LS 24 bits are index */
            hash_data->tx_ctr = pool_id << 24 | (ctr_index);
            hash_data->rx_ctr = hash_data->tx_ctr;
        } else {
            pool_id = hash_data->tx_ctr >> 24;
        }
        ep_info_p->lm_counter_base_id = hash_data->tx_ctr;
        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                                  mep_ctr_info[pool_id].ctr_valid, 1);
        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                     mep_ctr_info[pool_id].ctr_base_ptr,
                     (hash_data->tx_ctr & 0xFFFFFF));
        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                              mep_ctr_info[pool_id].ctr_mep_type, direction);
        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                       mep_ctr_info[pool_id].ctr_mep_mdl, ep_info_p->level);
        /* Create SERVICE_PRI_MAP profile */
        rv = _bcm_kt2_oam_service_pri_profile_add(unit, 0, ep_info_p,
                                                  &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, \
                      (BSL_META_U(unit, \
                                  "OAM Error: L3_ENTRY table update failed "
                                  "for EP=%d due to service pri map profile allocation "
                                  "failure %s.\n"),
                       ep_info_p->id, bcm_errmsg(rv)));
            return (rv);
        }
        hash_data->pri_map_index = profile_index;

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry,
                     mep_ctr_info[pool_id].ctr_profile, profile_index);

        rv = soc_mem_write(unit, L3_ENTRY_IPV4_UNICASTm, MEM_BLOCK_ALL,
                                                   l3_index, &l3_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3_ENTRY table write failed "
                                  "for EP=%d %s.\n"),
                       ep_info_p->id, bcm_errmsg(rv)));
            return (rv);
        }

    } else  {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MP group tbl entry not found-%s.\n"),
                   bcm_errmsg(rv)));
        return rv;
    }

    /*For downMEP Tx counters are in Egr Mp group table and for UpMEP it
      is in ING MP group table */
    if (BCM_SUCCESS(_bcm_kt2_oam_find_egr_lmep(unit, hash_data,
                                    &mp_grp_index, &egr_mp_group))) {
        if (hash_data->tx_ctr == _BCM_OAM_INVALID_INDEX) {
            rv = _bcm_kt2_oam_counter_set(unit, EGR_MP_GROUPm,
                           (uint32 *)&egr_mp_group, &ctr_index, &pool_id);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: EGR_MP_GROUP table update "
                                      "failed for EP=%d due to counter allocation "
                                      "failure  %s.\n"),
                           ep_info_p->id, bcm_errmsg(rv)));
                return (rv);
            }
            /* MS 8 bits of counter Id is pool id. LS 24 bits are index */
            hash_data->tx_ctr = pool_id << 24 | (ctr_index);
            hash_data->rx_ctr = hash_data->tx_ctr;
        } else {
            pool_id = hash_data->tx_ctr >> 24;
        }
        ep_info_p->lm_counter_base_id = hash_data->tx_ctr;
        if (SAL_BOOT_BCMSIM) {
            /* Added as WAR for Cmodel issue */
            /* Construct endpoint egress MP group Table entry key part */
            _bcm_oam_egr_lmep_key_construct(unit, hash_data, &egr_mp_group);
            soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group, VALIDf, 1);
        }
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group,
                     egr_mep_ctr_info[pool_id].ctr_valid, 1);
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group,
                 egr_mep_ctr_info[pool_id].ctr_base_ptr,
                 (hash_data->tx_ctr & 0xFFFFFF));
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group,
                    egr_mep_ctr_info[pool_id].ctr_mep_type, direction);
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group,
                 egr_mep_ctr_info[pool_id].ctr_mep_mdl, ep_info_p->level);

        /* Create SERVICE_PRI_MAP profile */
        rv = _bcm_kt2_oam_service_pri_profile_add(unit, 1, ep_info_p,
                                                  &profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EGR_MP_GROUP table update failed "
                                  "for EP=%d due to service pri map profile allocation "
                                  "failure %s.\n"),
                       ep_info_p->id, bcm_errmsg(rv)));
            return (rv);
        }
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group,
                 egr_mep_ctr_info[pool_id].ctr_profile, profile_index);
        hash_data->egr_pri_map_index = profile_index;

        rv = soc_mem_write(unit, EGR_MP_GROUPm, MEM_BLOCK_ALL,
                                               mp_grp_index, &egr_mp_group);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EGR_MP_GROUP table update failed "
                                  "for EP=%d %s.\n"),
                       ep_info_p->id, bcm_errmsg(rv)));
            return (rv);
        }
    } else  {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: EGR MP group tbl entry not found-%s.\n"),
                   bcm_errmsg(rv)));
        return rv;
    }

    return (rv);
}

/*
 * Function:
 *      bcm_kt2_oam_hw_ccm_tx_ctr_update
 * Purpose:
 *      Update CCM CTR params
 * Parameters:
 *      unit - (IN) Unit number.
 *      ep_info - (IN) Pointer to endpoint parameters
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcm_kt2_oam_hw_ccm_tx_ctr_update(int unit,
        bcm_oam_endpoint_info_t *ep_info) 
{

    _bcm_oam_hash_data_t        *h_data_p;
    _bcm_oam_control_t          *oc;
    int                         rv=BCM_E_NONE;
    lmep_1_entry_t              lmep_1_entry;    /* LMEP_1 table entry.*/
    uint8                       num_counter;
    uint32                      counter_id;
    uint32                      pool;
    lmep_entry_t                lmep_entry;    /* LMEP_1 table entry.*/

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    h_data_p =  &oc->oam_hash_data[ep_info->id];

    /* Get the LMEP_1 table entry. for the endpoint*/
    rv = READ_LMEP_1m(unit, MEM_BLOCK_ANY, h_data_p->local_tx_index, &lmep_1_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: LMEP_1 table read (index=%d) failed "
                            "- %s.\n"), unit, h_data_p->local_tx_index, bcm_errmsg(rv)));
        return rv;
    }
    
    /* Get the LMEP_1 table entry. for the endpoint*/
    rv = READ_LMEPm(unit, MEM_BLOCK_ANY, h_data_p->local_tx_index, &lmep_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: LMEP table read (index=%d) failed "
                            "- %s.\n"), unit, h_data_p->local_tx_index, bcm_errmsg(rv)));
        return rv;
    }
   
    if (_KT2_OAM_COUNTER_SIZE < ep_info->ccm_tx_update_lm_counter_size) {
        return BCM_E_PARAM;
    }

    for(num_counter = 0; num_counter < ep_info->ccm_tx_update_lm_counter_size;
            num_counter++) {

        /* Get the counter pool id */
        counter_id = ep_info->ccm_tx_update_lm_counter_base_id[num_counter] + 
            ep_info->ccm_tx_update_lm_counter_offset[num_counter];
        pool = counter_id >> 24;

        switch(pool){
            case 0:
                /* Set Counter 1 ID and action */
                soc_LMEP_1m_field32_set(unit, &lmep_1_entry, COUNTER_1_IDf, 
                        (counter_id & 0xFFFF));
                soc_LMEP_1m_field32_set(unit, &lmep_1_entry, COUNTER_1_ACTIONf, 1);
                break;
            case 1:
                /* Set Counter 2 ID and action */
                soc_LMEP_1m_field32_set(unit, &lmep_1_entry, COUNTER_2_IDf, 
                        (counter_id & 0xFFFF));
                soc_LMEP_1m_field32_set(unit, &lmep_1_entry, COUNTER_2_ACTIONf, 1);
                break;
            default:
                return BCM_E_PARAM;
        }
    }

    if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        /* TX counter is located in IP (0) */
        soc_LMEPm_field32_set(unit, &lmep_entry, COUNTER_1_LOCATIONf, 0); 
        soc_LMEPm_field32_set(unit, &lmep_entry, COUNTER_2_LOCATIONf, 0); 
    } else {
        /* TX counter is located in EP (1) */
        soc_LMEPm_field32_set(unit, &lmep_entry, COUNTER_1_LOCATIONf, 1); 
        soc_LMEPm_field32_set(unit, &lmep_entry, COUNTER_2_LOCATIONf, 1); 
    } 
    
    rv = WRITE_LMEP_1m(unit, MEM_BLOCK_ALL, h_data_p->local_tx_index,
            &lmep_1_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: LMEP_1 table write (EP=%d)"
                            " failed - %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
        return rv;
    }

    rv = WRITE_LMEPm(unit, MEM_BLOCK_ALL, h_data_p->local_tx_index,
            &lmep_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: LMEP table write (EP=%d)"
                            " failed - %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
        return rv;
    }
    return rv;
}

STATIC int
_bcm_kt2_oam_lmep_down_modify_new_dest_port(int unit, _bcm_oam_hash_data_t *h_data_p)
{

    lmep_1_entry_t              lmep_1_entry;    /* LMEP_1 table entry.*/

    /* Get the LMEP_1 table entry. */
    BCM_IF_ERROR_RETURN(READ_LMEP_1m(unit, MEM_BLOCK_ANY, h_data_p->local_tx_index, &lmep_1_entry));

   
    /* Set the queue number */ 
    soc_LMEP_1m_field32_set(unit, &lmep_1_entry, QUEUE_NUMf,
            SOC_INFO(unit).port_uc_cosq_base[h_data_p->dst_pp_port] +
            h_data_p->int_pri);

    /* Set the PP port */
    soc_LMEP_1m_field32_set(unit, &lmep_1_entry, PP_PORTf, h_data_p->dst_pp_port); 

    SOC_IF_ERROR_RETURN
        (WRITE_LMEP_1m(unit, MEM_BLOCK_ALL, h_data_p->local_tx_index,
                     &lmep_1_entry));


    return (BCM_E_NONE);
}

STATIC int
_bcm_kt2_oam_lmep_up_modify_new_src_port(int unit, _bcm_oam_hash_data_t *h_data_p)
{
  lmep_1_entry_t              lmep_1_entry;    /* LMEP_1 table entry.*/

  /* Get the LMEP_1 table entry. */
  BCM_IF_ERROR_RETURN(READ_LMEP_1m(unit, MEM_BLOCK_ANY, h_data_p->local_tx_index, &lmep_1_entry));

  /* Set the PP port */
  soc_LMEP_1m_field32_set(unit, &lmep_1_entry, PP_PORTf, h_data_p->src_pp_port);

  SOC_IF_ERROR_RETURN
    (WRITE_LMEP_1m(unit, MEM_BLOCK_ALL, h_data_p->local_tx_index,
                   &lmep_1_entry));

  return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_oam_kt2_local_tx_mep_hw_set
 * Purpose:
 *     Configure hardware tables for a local CCM Tx enabled endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_oam_kt2_local_tx_mep_hw_set(int unit,
                                bcm_oam_endpoint_info_t *ep_info_p)
{
    int                  rv = BCM_E_NONE;/* Operation return status.   */
    _bcm_oam_hash_data_t *hash_data; /* Pointer to endpoint hash data. */
    lmep_entry_t         entry;      /* LMEP table entry.              */
    lmep_1_entry_t       entry_1;    /* LMEP_1 table entry.            */
    int                  word;       /* Word index.                    */
    uint32               reversed_maid[BCM_OAM_GROUP_NAME_LENGTH / 4];
                                     /* Group name in Hw format.       */
    _bcm_oam_control_t   *oc;        /* Pointer to control structure.  */
    const bcm_oam_endpoint_info_t *ep_p;/* Pointer to endpoint info.   */
    int                  subport = 0;
    int                  oam_replacement_offset = 0;
    int                  tpid_index = 0;
    _bcm_kt2_subport_info_t subport_info;
    int                  up_mep = 0;
    int                  pp_port = 0;
    int                  vlan_tag_control = 0;
    uint32               tag_type = 0;
    bcm_port_t           up_mep_tx_port = 0;
    bcm_gport_t          gport = 0;

    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Initialize local endpoint info pointer. */
    ep_p = ep_info_p;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get the stored endpoint information from hash table. */
    hash_data = &oc->oam_hash_data[ep_p->id];
    if (0 == hash_data->in_use) {
        return(BCM_E_INTERNAL);
    }

    sal_memset(&entry, 0, sizeof(lmep_entry_t));
    sal_memset(&entry_1, 0, sizeof(lmep_1_entry_t));

    /* Set the Group base index. */
    soc_LMEPm_field32_set(unit, &entry, MAID_INDEXf, ep_p->group);

    /* Set source MAC address to be used on transmitted packets. */
    soc_LMEPm_mac_addr_set(unit, &entry, SAf, ep_p->src_mac_address);

    /* Set the Maintenance Domain Level. */
    soc_LMEPm_field32_set(unit, &entry, MDLf, ep_p->level);

    /* Set endpoint name. */
    soc_LMEPm_field32_set(unit, &entry, MEPIDf, ep_p->name);

    /*
     * Set VLAN ID to be used in the transmitted packet.
     * For link level MEPs, this VLAN_ID == 0.
     */
    soc_LMEPm_field32_set(unit, &entry, SVLAN_TAGf, ep_p->vlan);

    /* Set packet priority value to be used in transmitted packet. */
    soc_LMEPm_field32_set(unit, &entry, PRIORITYf, ep_p->pkt_pri);

    /* Set CVLAN Id */
    soc_LMEPm_field32_set(unit, &entry, CVLAN_TAGf, ((ep_p->inner_pkt_pri << 13) | ep_p->inner_vlan));

    /* Set interval between CCM packet transmissions. */
    soc_LMEPm_field32_set
        (unit, &entry, CCM_INTERVALf,
        _bcm_kt2_oam_ccm_msecs_to_hw_encode(ep_p->ccm_period));

    /* Set Port status TLV in CCM Tx packets. */
    if (ep_p->flags & BCM_OAM_ENDPOINT_PORT_STATE_UPDATE) {
        if (ep_p->port_state > BCM_OAM_PORT_TLV_UP) {
            return BCM_E_PARAM;
        }
        soc_LMEPm_field32_set(unit, &entry, PORT_TLVf,
                              (ep_p->port_state == BCM_OAM_PORT_TLV_UP)
                              ? 1 : 0);
    }


    /* Set the MEP type - up/down */
    soc_LMEPm_field32_set(unit, &entry, MEP_TYPEf, 
                         ((ep_p->flags & BCM_OAM_ENDPOINT_UP_FACING) ? 1 : 0));
    /*
     * Construct group name for hardware.
     * i.e Word-reverse the MAID bytes for hardware.
     */
    for (word = 0; word < (BCM_OAM_GROUP_NAME_LENGTH / 4); ++word) {
        reversed_maid[word]
            = bcm_htonl(((uint32 *) oc->group_info[ep_p->group].name)
                [((BCM_OAM_GROUP_NAME_LENGTH / 4) - 1) - word]);
    }

    /* Set the group name. */
    soc_LMEPm_field_set(unit, &entry, MAIDf, reversed_maid);

    /* Set OAM replacement offset & default TPID */
    if (_BCM_OAM_EP_IS_VP_TYPE(hash_data)) {
        oam_replacement_offset = 1;
        if(hash_data->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
            oam_replacement_offset = 5;
            vlan_tag_control = 3;
            tag_type = _BCM_OAM_DOMAIN_S_PLUS_CVLAN;
        }else if (hash_data->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
            oam_replacement_offset = 3;
            vlan_tag_control = 1;
            tag_type =_BCM_OAM_DOMAIN_CVLAN;
        } else if (hash_data->vlan > 0) {
            oam_replacement_offset = 3;
            vlan_tag_control = 2;
            tag_type =_BCM_OAM_DOMAIN_SVLAN;
        }
    }

    switch (hash_data->oam_domain) {
        case _BCM_OAM_DOMAIN_PORT:
            oam_replacement_offset = 1;
            vlan_tag_control = 0;
            break;

        case _BCM_OAM_DOMAIN_CVLAN:
            vlan_tag_control = 1;
            oam_replacement_offset = 3;
            tag_type =_BCM_OAM_DOMAIN_CVLAN;
            break;

        case _BCM_OAM_DOMAIN_SVLAN: 
            oam_replacement_offset = 3;
            vlan_tag_control = 2;
            tag_type =_BCM_OAM_DOMAIN_SVLAN;
            break;

        case _BCM_OAM_DOMAIN_S_PLUS_CVLAN:
            oam_replacement_offset = 5;
            vlan_tag_control = 3;
            tag_type = _BCM_OAM_DOMAIN_S_PLUS_CVLAN;
            break;

        default:
            break;
    }


    /* if S or S+C vlan - set outer tpid index */
    if ((tag_type == _BCM_OAM_DOMAIN_SVLAN) || 
        (tag_type == _BCM_OAM_DOMAIN_S_PLUS_CVLAN)) { 
        rv = _bcm_kt2_tpid_entry_add(unit, 
                                 (ep_p->outer_tpid ? ep_p->outer_tpid :
                                                 BCM_OAM_DEFAULT_TPID), 
                                 BCM_OAM_TPID_TYPE_OUTER, &tpid_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3 entry config failed in TPID set "
                                  "for EP=%d %s.\n"),
                       ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
        hash_data->outer_tpid_profile_index = tpid_index;
        soc_LMEPm_field32_set(unit, &entry, SVLAN_TPID_INDEXf, tpid_index); 
    }
    if ((tag_type == _BCM_OAM_DOMAIN_CVLAN) || 
        /* C or S+C */
        (tag_type == _BCM_OAM_DOMAIN_S_PLUS_CVLAN)) { 
        rv = _bcm_kt2_tpid_entry_add(unit, 
                                 (ep_p->inner_tpid ? ep_p->inner_tpid :
                                                 BCM_OAM_DEFAULT_TPID), 
                                 BCM_OAM_TPID_TYPE_INNER, &tpid_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3 entry config failed in TPID set "
                                  "for EP=%d""  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
        hash_data->inner_tpid_profile_index = tpid_index;
    }

    gport = hash_data->gport;
    if (BCM_GPORT_IS_TRUNK(ep_p->gport)) {
        gport =  hash_data->resolved_trunk_gport;
    }

    if ((BCM_GPORT_IS_SUBPORT_PORT(gport)) &&
              (_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport))) {
        /* CoE port */
        rv = _bcm_kt2_tpid_entry_add(unit, 
                                 (ep_p->subport_tpid ? ep_p->subport_tpid :
                                                 BCM_OAM_DEFAULT_TPID), 
                                 BCM_OAM_TPID_TYPE_SUBPORT, &tpid_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3 entry config failed in TPID set "
                                  "for EP=%d  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
        soc_LMEPm_field32_set(unit, &entry, 
                              SUBPORT_TAG_TPID_INDEXf, tpid_index); 
        hash_data->subport_tpid_profile_index = tpid_index;
    }

    /* Set LMEP_1 table entry */

    /* If COE or LINKPHY port replacement offset should be increased 
       by 2 to accomodate - stream_id/subport tag */ 
    if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        if ((_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) ||
            (_BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, gport))) {
            oam_replacement_offset += 2; 
            subport = 1 << 2;
        }
    }

    soc_LMEP_1m_field32_set(unit, &entry_1, OAM_REPLACEMENT_OFFSETf, 
                                                oam_replacement_offset);
  

    /* Set Tag status of the generated CCM packet 
       we can set it based on incoming packet tag info 
       if SUBPORT - vlan tag control = 1 << 2 | hash_data->oam_domain & 0x3 */
    if (subport) {
        soc_LMEP_1m_field32_set(unit, &entry_1, VLAN_TAG_CONTROLf, 
                                   (subport  | (vlan_tag_control & 0x3)));
    } else {
        soc_LMEP_1m_field32_set(unit, &entry_1, VLAN_TAG_CONTROLf, 
                                   (vlan_tag_control & 0x3));

    }
    /* allocate TX counters */
        /* allocate TX counters */
    if(ep_info_p->type == bcmOAMEndpointTypeEthernet) {
        if((ep_info_p->ccm_tx_update_lm_counter_size) && 
                (_BCM_OAM_INVALID_INDEX != hash_data->local_tx_index)) {
            BCM_IF_ERROR_RETURN(bcm_kt2_oam_hw_ccm_tx_ctr_update(unit, ep_info_p)); 
        }
    }
    
    if (ep_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        up_mep = 1;
    } 
    /* For DownMEP, set Queue number to which this packet must be enqueued */ 
    if (!up_mep) {
        soc_LMEP_1m_field32_set(unit, &entry_1, QUEUE_NUMf, 
                     SOC_INFO(unit).port_uc_cosq_base[hash_data->dst_pp_port] + 
                     ep_p->int_pri);
     /* Set the destination portt on which packet needs to Tx. */
        pp_port = hash_data->dst_pp_port;
    } else {
        soc_LMEP_1m_field32_set(unit, &entry_1, INT_PRIf,
                ep_p->int_pri);
     /* Set the source on which packet is received */
        pp_port = hash_data->src_pp_port;
    } 

    if (ep_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_VLAN_VP_UP_MEP_IN_HW) {

        up_mep_tx_port = _BCM_KT2_SUBPORT_PORT_ID_GET(ep_p->tx_gport);
        hash_data->src_pp_port = up_mep_tx_port;
        pp_port = hash_data->src_pp_port;
    }

     /* Set the PP port */
    soc_LMEP_1m_field32_set(unit, &entry_1, PP_PORTf, pp_port); 

    /* Set Outgoing Subport tag to be inserted into CCM packet */
    if ((BCM_GPORT_IS_SUBPORT_PORT(gport)) &&
        (_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport))) {
        /* Get the subport tag */
        rv = bcm_kt2_subport_pp_port_subport_info_get(unit, pp_port, 
                                                         &subport_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: failed to get subport tag EP=%d"
                                   "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
        if (subport_info.port_type == _BCM_KT2_SUBPORT_TYPE_SUBTAG) {
                soc_LMEP_1m_field32_set(unit, &entry_1, SUBPORT_TAGf, 
                                             subport_info.subtag);
        }
    }

    /* Set Interface status TLV in CCM Tx packets - 3-bits wide. */
    if (ep_p->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE) {
        if ((ep_p->interface_state < BCM_OAM_INTERFACE_TLV_UP)
            || (ep_p->interface_state > BCM_OAM_INTERFACE_TLV_LLDOWN)) {
            return BCM_E_PARAM;
        }
        soc_LMEP_1m_field32_set(unit, &entry_1, INTERFACE_TLVf,
                              ep_p->interface_state);
    }
    /*
     * When this bit is '1', both port and interface TLV values are set in
     *  CCM Tx packets - set in LMEP table.
     */
    if ((ep_p->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX)
        || (ep_p->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX)) {
        soc_LMEPm_field32_set(unit, &entry, INSERT_TLVf, 1);
    }

    /* Set CCM packet Destination MAC address value. */
    soc_mem_mac_addr_set(unit, LMEP_1m, &entry_1, DAf,
                         ep_p->dst_mac_address);


    /* Write entry to hardware LMEP table. */
    SOC_IF_ERROR_RETURN
        (WRITE_LMEPm(unit, MEM_BLOCK_ALL, hash_data->local_tx_index,
                     &entry));

    /* Write entry to hardware LMEP_1 table. */
    SOC_IF_ERROR_RETURN
        (WRITE_LMEP_1m(unit, MEM_BLOCK_ALL, hash_data->local_tx_index,
                     &entry_1));


    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_stm_table_update
 * Purpose:
 *     Update MA_BASE_PTR in source trunk MAP table 
 * Parameters:
 *     unit      - (IN) BCM device number
 *     mod_id    - (IN) module id
 *     port_id   - (IN) port id
 *     h_data_p  - (IN) endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_stm_table_update(int unit, bcm_module_t mod_id,
                              bcm_port_t port_id, 
                              _bcm_oam_hash_data_t *h_data_p)
{
    int    stm_index = 0;
    int    rv;
    source_trunk_map_table_entry_t stm_entry;

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    /* Derive index to SOURCE_TRUNK_MAP tbl based on module ID and port*/
    rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_id,
                                               port_id, &stm_index);

    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                      stm_index, &stm_entry);

    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_entry, MA_BASE_PTRf,
                                            h_data_p->ma_base_index);
    rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL, stm_index,
                                                                &stm_entry);
    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    return rv;
}


/*
 * Function:
 *     _bcm_kt2_oam_port_mdl_update
 * Purpose:
 *     Update MDL bitmap in port table
 * Parameters:
 *     unit      - (IN) BCM device number
 *     pp_port   - (IN) pp port id
 *     reset     - (IN) Reset MDL bitmap or not
 *     h_data_p  - (IN) endpoint hash data
 *     mdl       - (OUT) Maintenance domain level
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_port_mdl_update(int unit, uint32  pp_port, int reset, 
                             _bcm_oam_hash_data_t *h_data_p, uint8 *mdl) 
{
    port_tab_entry_t     port_entry;

    if (NULL == h_data_p) {
        return (BCM_E_INTERNAL);
    }
    BCM_IF_ERROR_RETURN
          (soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, pp_port, &port_entry));

    /* read and update MDL bitmap */
    *mdl = soc_PORT_TABm_field32_get(unit, &port_entry, MDL_BITMAPf);
    if (reset) {
        /* Clear the MDL bit for this endpoint. */
        *mdl &= ~(1 << h_data_p->level);

    } else {
        *mdl |= (1 << h_data_p->level);
    }
    soc_PORT_TABm_field32_set(unit, &port_entry, MDL_BITMAPf, *mdl);
    BCM_IF_ERROR_RETURN
         (soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, pp_port, &port_entry));

    return BCM_E_NONE;
}            
   
/*
 * Function:
 *     _bcm_kt2_oam_port_mdl_passive_update
 * Purpose:
 *     Update passive MDL bitmap in egress port table
 * Parameters:
 *     unit      - (IN) BCM device number
 *     reset     - (IN) Reset MDL bitmap
 *     h_data_p  - (IN) endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_port_mdl_passive_update(int unit, int reset,
                                     _bcm_oam_hash_data_t *h_data_p, uint8 mdl) 
{
    egr_port_entry_t     egr_port_entry;
    uint8                passive_mdl = 0; /* Maintenance domain level-passive */
    
    if (NULL == h_data_p) {
        return (BCM_E_INTERNAL);
    }

    /* Passive demux should not be done for MIP */
    if (_BCM_OAM_EP_IS_MIP(h_data_p)) {
        return BCM_E_NONE;
    }

    /* Set MDL passive bitmap in EGR_PORT table */
    BCM_IF_ERROR_RETURN
         (soc_mem_read(unit, EGR_PORTm, MEM_BLOCK_ANY, h_data_p->dst_pp_port,
                          &egr_port_entry));

    if (reset) {
        if (mdl > 0) {
            /* Passive MDL bitmap */
            _bcm_kt2_oam_get_passive_mdl_from_active_mdl(unit, mdl,
                                    h_data_p->ma_base_index, &passive_mdl, 0);
        } else {
            passive_mdl = 0;
        }
        soc_EGR_PORTm_field32_set(unit, &egr_port_entry, 
                                            MDL_BITMAP_PASSIVEf, passive_mdl);
    } else {
        /* read and update MDL bitmap */
        mdl = soc_EGR_PORTm_field32_get(unit, &egr_port_entry, 
                                                       MDL_BITMAP_PASSIVEf);
        mdl |= (1 << h_data_p->level);

        /* Set all the bits till the highest configured MDL, so that there are 
           no holes in the bitmap */
        mdl |= ((1 << h_data_p->level) - 1);

        soc_EGR_PORTm_field32_set(unit, &egr_port_entry, 
                                                  MDL_BITMAP_PASSIVEf, mdl);
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_PORTm, MEM_BLOCK_ALL, 
                           h_data_p->dst_pp_port, &egr_port_entry));
    return BCM_E_NONE;
}            

/*
 * Function:
 *     _bcm_kt2_oam_trunk_port_mdl_config
 * Purpose:
 *     Update port mdl and stm table for OAM on trunk case     
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p - (IN) Pointer to hash data 
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_trunk_port_mdl_config(int unit,
                          _bcm_oam_hash_data_t *h_data_p)
{
    int                  rv = BCM_E_NONE;/* Operation return status.         */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    int                  member_count = 0;
    bcm_port_t           *member_array = NULL; /* Trunk member port array.   */
    int                  local_member_count = 0;
    int                  i = 0;
    bcm_port_t           port = 0;
    bcm_module_t         module_id;            /* Module ID                  */
    uint8                mdl = 0;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if(h_data_p->trunk_id == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    /* Get all the local member ports belonging to this trunk */
    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, h_data_p->trunk_id, 
                               NULL, 0, NULL, &member_count));
    if (0 == member_count) {
        /* No members have been added to the trunk group yet */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: No local members have been added to "
                              "the trunk group yet - %s.\n"), bcm_errmsg(rv)));
        return BCM_E_PARAM;
    }

    _BCM_OAM_ALLOC(member_array, bcm_port_t,
                       sizeof(bcm_port_t) * member_count, "Trunk info");
    if (NULL == member_array) {
        return (BCM_E_MEMORY);
    }

    /* Get members of the trunk belonging to this module */
    if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, 
                                                     h_data_p->trunk_id, 
                                                     member_count, member_array,
                                                     &local_member_count))) {
        if (local_member_count > 0) {
            for(i = 0; i < local_member_count; i++) {
                rv = _bcm_kt2_pp_port_to_modport_get(unit, member_array[i],
                                                  &module_id, &port);
                if (BCM_FAILURE(rv)) {
                    sal_free(member_array);
                    return (rv);
                }
                rv = _bcm_kt2_oam_stm_table_update(unit, module_id,
                                               member_array[i], h_data_p);
                if (BCM_FAILURE(rv)) {
                    sal_free(member_array);
                    return (rv);
                }
                rv =_bcm_kt2_oam_port_mdl_update(unit, member_array[i], 0, 
                                                        h_data_p, &mdl); 
                if (BCM_FAILURE(rv)) {
                    sal_free(member_array);
                    return (rv);
                }
            } 
        }
        h_data_p->active_mdl_bitmap = mdl;
    }
    sal_free(member_array);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_port_domain_oam_hw_set
 * Purpose:
 *    Configure port table and stm table entries for port based MEP. 
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_port_domain_oam_hw_set(int unit,
                          const bcm_oam_endpoint_info_t *ep_info_p)
{
    _bcm_oam_hash_data_t *h_data_p;      /* Endpoint hash data pointer.      */
    int                  rv = BCM_E_NONE;/* Operation return status.         */
    uint8                mdl = 0;        /* Maintenance domain level.        */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    const bcm_oam_endpoint_info_t *ep_p; /* Pointer to endpoint info.        */
    bcm_module_t         mod_id = 0;
    bcm_port_t           port_id = 0;

    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Initialize local endpoint info pointer. */
    ep_p = ep_info_p;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get endpoint hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_p->id];

    /*  Set port table and source trunk map table */
    if (0 == ep_info_p->vlan) {
        if(h_data_p->trunk_id != BCM_TRUNK_INVALID) {
            rv = _bcm_kt2_oam_trunk_port_mdl_config(unit, h_data_p);
        } else {
            port_id = _BCM_OAM_GLP_PORT_GET(h_data_p->sglp);
            mod_id  = _BCM_OAM_GLP_MODULE_ID_GET(h_data_p->sglp);
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_oam_stm_table_update(unit, mod_id,
                                               port_id, h_data_p));
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_oam_port_mdl_update(unit, h_data_p->src_pp_port, 0, 
                                             h_data_p,  &mdl)); 
            h_data_p->active_mdl_bitmap = mdl;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oam_port_mdl_passive_update(unit, 0, h_data_p, mdl)); 
    }
    return (rv);
}


/*
 * Function:
 *     _bcm_kt2_oam_l3_entry_set
 * Purpose:
 *     Configure l3 entry tables entry for a local CCM Rx enabled endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_l3_entry_set(int unit,
                          bcm_oam_endpoint_info_t *ep_info_p)
{
    _bcm_oam_hash_data_t *h_data_p;      /* Endpoint hash data pointer.      */
    l3_entry_1_entry_t   l3_entry;       /* L3 entry buffer.                 */
    int                  l3_index = -1;  /* L3 entry hardware index.         */
    int                  rv;             /* Operation return status.         */
    uint8                mdl = 0;        /* Maintenance domain level.        */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    bcm_oam_endpoint_info_t *ep_p;       /* Pointer to endpoint info.        */
    soc_field_t          mdl_field = 0;             
    int                  up_mep = 0;     /* Endpoint is an upMep             */

    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Initialize local endpoint info pointer. */
    ep_p = ep_info_p;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get endpoint hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_p->id];

    if (ep_info_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        up_mep = 1 ;
        mdl_field = LMEP__MDL_BITMAP_PASSIVEf;
    } else {
        mdl_field = LMEP__MDL_BITMAP_ACTIVEf;
    }
    /* L3 entry */
    L3_LOCK(unit);
    if (BCM_SUCCESS
            (_bcm_kt2_oam_find_lmep(unit, h_data_p, &l3_index, &l3_entry))) {
        if ((mdl_field == LMEP__MDL_BITMAP_PASSIVEf) &&
                (_BCM_OAM_EP_IS_MIP(h_data_p))) {
            /* Passive demultiplexing should not be done on a MIP */
            L3_UNLOCK(unit);
            return BCM_E_NONE;
        }

        /* There's already an entry for this */
        mdl = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry, mdl_field);

        /* If active bitmap is 0, so for first active endppoint in the mp_group 
         * need to programme ma_base_ptr field  during down_mep creation */
        if((mdl == 0) && (up_mep == 0)) {
            soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, LMEP__MA_BASE_PTRf,
                    h_data_p->ma_base_index);
        } 

        mdl |= (1 << ep_p->level);

        /* Set all the bits till the highest configured MDL, so that there are 
           no holes in the passive bitmap */
        if (up_mep) {
            mdl |= ((1 << ep_p->level) - 1);
        }
        /* set MDL bitmap passive or active depending on up or down mep */
        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, mdl_field, mdl);

        rv = soc_mem_write(unit, L3_ENTRY_IPV4_UNICASTm, MEM_BLOCK_ALL, 
                                                       l3_index, &l3_entry);
        if (BCM_FAILURE(rv)) {
            L3_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3_ENTRY table update failed for "
                                  "EP=%d" "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
    } else {

        /* This is the first entry at this */
        sal_memset(&l3_entry, 0, sizeof(l3_entry_1_entry_t));

        /* Passive demultiplexing should not be done on a MIP */
        if (!((mdl_field == LMEP__MDL_BITMAP_PASSIVEf)
            && (_BCM_OAM_EP_IS_MIP(h_data_p)))) {
            mdl |= (1 << ep_p->level);

            /* Set all the bits till the highest configured MDL, so that there are 
               no holes in the passive bitmap */
            if (up_mep) {
                mdl |= ((1 << ep_p->level) - 1);
            }
        }
        /* set MDL bitmap passive or active depending on up or down mep */
        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, mdl_field, mdl);

        if (mdl == 0) {
            /* MDL = 0 for case of MIP. Dont make the entry valid by
               programming other fields.  */
            return BCM_E_NONE;
        }

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, 
                                               LMEP__MA_BASE_PTRf,
                                               h_data_p->ma_base_index);

        /* Configure time stamp type 1588(PTP)/NTP */
        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, 
                                       LMEP__TIMESTAMP_TYPEf,
             ((ep_p->timestamp_format == bcmOAMTimestampFormatNTP) ? 1 : 0));

        h_data_p->ts_format = ep_p->timestamp_format;

        /* Construct LMEP view key for L3 Table insert operation. */
        _bcm_kt2_oam_lmep_key_construct(unit, h_data_p, &l3_entry);

        soc_L3_ENTRY_IPV4_UNICASTm_field32_set(unit, &l3_entry, VALIDf, 1);

        rv = soc_mem_insert(unit, L3_ENTRY_IPV4_UNICASTm, 
                                                 MEM_BLOCK_ALL, &l3_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3_ENTRY table insert failed for "
                                  "EP=%d" "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            L3_UNLOCK(unit);
            return (rv);
        }
    }
    if (up_mep == 0) {
        h_data_p->active_mdl_bitmap = mdl;
    }
    L3_UNLOCK(unit);
    return (BCM_E_NONE);
}
        
/*
 * Function:
 *     _bcm_kt2_oam_egr_mp_group_entry_set
 * Purpose:
 *     Configure Egress MP group tables entry for local CCM Rx enabled endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 *     ctr_index - (OUT) LM counter index
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_egr_mp_group_entry_set(int unit,
                          bcm_oam_endpoint_info_t *ep_info_p,
                          shr_idxres_element_t *ctr_index)
{
    _bcm_oam_hash_data_t *h_data_p;      /* Endpoint hash data pointer.      */
    int                  rv;             /* Operation return status.         */
    uint8                mdl = 0;        /* Maintenance domain level.        */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    bcm_oam_endpoint_info_t *ep_p;       /* Pointer to endpoint info.        */
    soc_field_t          mdl_field = 0;             
    egr_mp_group_entry_t egr_mp_group;   /* Egress MP group tbl entry buffer */
    int                  up_mep = 0;     /* Endpoint is an upMep             */
    int                  mp_grp_index = 0;

    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Initialize local endpoint info pointer. */
    ep_p = ep_info_p;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get endpoint hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_p->id];

    if (ep_info_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        up_mep = 1;
        mdl_field = MDL_BITMAP_ACTIVEf;
    } else {
        mdl_field = MDL_BITMAP_PASSIVEf;
    }

    /* form the key to search EGR_MP_GRUP table */
    if (BCM_SUCCESS
        (_bcm_kt2_oam_find_egr_lmep(unit, h_data_p,
                                        &mp_grp_index, &egr_mp_group))) {

        /* There's already an entry for this */
        if (SAL_BOOT_BCMSIM) {
        /* Added as WAR for Cmodel issue */
        /* Construct endpoint egress MP group  Table entry search operation. */
            _bcm_oam_egr_lmep_key_construct(unit, h_data_p, &egr_mp_group);
            soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group, VALIDf, 1);
        }
        mdl = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_group, mdl_field);
        /* If active bitmap is 0, so for first active endppoint in the mp_group 
         * need to programme ma_base_ptr field  during up_mep creation */
        if((mdl == 0) && up_mep) {
            soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group, MA_BASE_PTRf,
                    h_data_p->ma_base_index);
        } 

        /* Passive demultiplexing should not be done on a MIP */
        if (!((mdl_field == MDL_BITMAP_PASSIVEf) 
            && (_BCM_OAM_EP_IS_MIP(h_data_p)))) {
            mdl |= (1 << ep_p->level);

            /* Set all the bits till the highest configured MDL, so that there are 
               no holes in the passive bitmap */
            if (mdl_field == MDL_BITMAP_PASSIVEf) {
                mdl |= ((1 << ep_p->level) - 1);
            }
        }
        /* set MDL bitmap passive or active depending on up or down mep */
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group, mdl_field, mdl);

        soc_mem_lock(unit, EGR_MP_GROUPm);

        rv = soc_mem_write(unit, EGR_MP_GROUPm, MEM_BLOCK_ALL, 
                                             mp_grp_index, &egr_mp_group);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_MP_GROUPm);
            return (rv);
        }
        soc_mem_unlock(unit, EGR_MP_GROUPm);
    } else {

        /* This is the first entry at this */
        sal_memset(&egr_mp_group, 0, sizeof(egr_mp_group_entry_t));

        /* Passive demultiplexing should not be done on a MIP */
        if (!((mdl_field == MDL_BITMAP_PASSIVEf) 
            && (_BCM_OAM_EP_IS_MIP(h_data_p)))) {
            mdl = (1 << ep_p->level);
            /* Set all the bits till the highest configured MDL, so that there are 
               no holes in the passive bitmap */
            if ( mdl_field == MDL_BITMAP_PASSIVEf && !(BHH_EP_TYPE(ep_info_p)) ) {
                mdl |= ((1 << ep_p->level) - 1);
            }
        }

        if (mdl == 0) {
            /* MDL = 0 for case of MIP. Dont make the entry valid by
               programming other fields.  */
            return BCM_E_NONE;
        }

        /* set MDL bitmap passive or active depending on up or down mep */
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group, mdl_field, mdl);

        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group, MA_BASE_PTRf,
                                      h_data_p->ma_base_index);

        /* Configure time stamp type 1588(PTP)/NTP */
        soc_EGR_MP_GROUPm_field32_set(unit, &egr_mp_group, 
                                       TIMESTAMP_TYPEf,
             ((ep_p->timestamp_format == bcmOAMTimestampFormatNTP) ? 1 : 0));

        h_data_p->ts_format = ep_p->timestamp_format;

        /* Construct LMEP view key for L3 Table insert operation. */
        _bcm_oam_egr_lmep_key_construct(unit, h_data_p, &egr_mp_group);

        soc_mem_lock(unit, EGR_MP_GROUPm);

        rv = soc_mem_insert(unit, EGR_MP_GROUPm, MEM_BLOCK_ALL, &egr_mp_group);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_MP_GROUPm);
            return (rv);
        }
        soc_mem_unlock(unit, EGR_MP_GROUPm);
    }
    if (up_mep == 1) {
        h_data_p->active_mdl_bitmap = mdl;
    }
    return (BCM_E_NONE);
}

 STATIC int
_bcm_kt2_oam_ma_idx_to_ep_id_mapping_op (int unit,
                             _bcm_oam_hash_data_t *h_data_p,
                             soc_mem_t mem,
                             int delete)
{
    uint32             mdl_value[1] = { 0 };
    int                bit_pos = 0;
    int                current_offset = 0;
    int                new_offset = 0;
    int                entry_count = 0;
    shr_idxres_list_handle_t pool;
    bcm_oam_endpoint_t *temp_array = NULL;
    _bcm_oam_control_t   *oc = NULL;
    int ma_base_index = _BCM_OAM_INVALID_INDEX;
    int is_upmep = 0;
    l3_entry_1_entry_t l3_entry;       /* LMEP view table entry.            */
    int                l3_index = -1;  /* L3 table hardware index.          */
    egr_mp_group_entry_t egr_mp_group;  /* Egress MP group tbl entry buffer */
    int                mp_grp_index = -1;
    int                     stm_index = 0;
    source_trunk_map_table_entry_t stm_entry;
    uint8 port_mdl = 0;
    int rv = BCM_E_NONE;

    if (NULL == h_data_p) {
        return (BCM_E_INTERNAL);
    }

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if (mem == MA_INDEXm) { 
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit,
                    h_data_p, &entry_count, &pool));
    } else {
        entry_count = 8;
    }

    if (entry_count <= 0) {
        return BCM_E_INTERNAL;
    }


    ma_base_index = h_data_p->ma_base_index;
    is_upmep = (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING)?1:0;

    if (h_data_p->oam_domain == _BCM_OAM_DOMAIN_PORT) {
        rv = _bcm_kt2_oam_find_port_lmep(unit, h_data_p, &stm_index, &stm_entry, &port_mdl);
        if (BCM_SUCCESS(rv)) {
            mdl_value[0] = port_mdl; 
        } else {
            /* If app is not initialized (say rc), then during detach (delete)
             * we will not find the entry. simply return without error */
            if ((delete) && (rv == BCM_E_NOT_FOUND) && !(oc->init)) {
                return BCM_E_NONE;
            } else {
                return BCM_E_INTERNAL;
            }
        }
    } else if (is_upmep) {
        rv = _bcm_kt2_oam_find_egr_lmep(unit, h_data_p, 
                &mp_grp_index, &egr_mp_group);
        if (BCM_SUCCESS(rv)) {
            mdl_value[0] = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_group,
                    MDL_BITMAP_ACTIVEf);
        } else {
            /* If app is not initialized (say rc), then during detach (delete)
             * we will not find the entry. simply return without error */
            if ((delete) && (rv == BCM_E_NOT_FOUND) && !(oc->init)) {
                return BCM_E_NONE;
            } else {
                return BCM_E_INTERNAL;
            }
        }
    } else {
        rv = _bcm_kt2_oam_find_lmep(unit, h_data_p, &l3_index, &l3_entry);
        if (BCM_SUCCESS(rv)) {
            mdl_value[0] = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                                                      LMEP__MDL_BITMAP_ACTIVEf);
        } else {
            /* If app is not initialized (say rc), then during detach (delete)
             * we will not find the entry. simply return without error */
            if ((delete) && (rv == BCM_E_NOT_FOUND) && !(oc->init)) {
                return BCM_E_NONE;
            } else {
                return BCM_E_INTERNAL;
            }
        }
    }

    _BCM_OAM_ALLOC(temp_array, bcm_oam_endpoint_t,
                       sizeof(bcm_oam_endpoint_t) * entry_count, "endpoint id array");
    if (NULL == temp_array) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(temp_array, _BCM_OAM_INVALID_INDEX, sizeof(bcm_oam_endpoint_t) * entry_count);


    if (delete) {
        for (bit_pos = 0; bit_pos <= 7; bit_pos++) {
            if (SHR_BITGET(mdl_value, bit_pos)) {
               if (bit_pos == h_data_p->level) {
                   current_offset++;
               } else {
                   temp_array[new_offset] = 
                    oc->_bcm_oam_ma_idx_to_ep_id_map
                    [_BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_IDX
                        (ma_base_index, current_offset, is_upmep)];
                   new_offset++;
                   current_offset++;
               }    
            }
        }
    } else {
        for (bit_pos = 0; bit_pos <= 7; bit_pos++) {
            if (SHR_BITGET(mdl_value, bit_pos)) {
               if (bit_pos == h_data_p->level) {
                   temp_array[new_offset] = h_data_p->ep_id; 
                   new_offset++;
               } else {
                   temp_array[new_offset] = 
                    oc->_bcm_oam_ma_idx_to_ep_id_map[
                    _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_IDX(ma_base_index,
                            current_offset, is_upmep)];
                   new_offset++;
                   current_offset++;
               }    
            }
        }
    }

    sal_memcpy(&(oc->_bcm_oam_ma_idx_to_ep_id_map[
                _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_IDX
                (ma_base_index, 0, is_upmep)]), 
                temp_array, sizeof(bcm_oam_endpoint_t) * entry_count);

    sal_free(temp_array);
    return (BCM_E_NONE);
}
 
STATIC int
_bcm_kt2_oam_ma_idx_to_ep_id_mapping_add (int unit,
                             _bcm_oam_hash_data_t *h_data_p)
{
    int is_upmep = 0;
    int is_remote = 0;
    int is_rx_enabled = 0;
    soc_mem_t mem = MA_INDEXm;
    int rv = BCM_E_NONE;

    /* h_data_p being NULL could already be some other error
     * scenario. Lets not return error for such a scenario.
     */
    if (h_data_p == NULL) {
        return BCM_E_NONE;
    }

    is_upmep = (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING)?1:0;
    if (is_upmep) {
        mem = EGR_MA_INDEXm;
    }
    is_remote = (h_data_p->flags & BCM_OAM_ENDPOINT_REMOTE)?1:0;
    is_rx_enabled =  (h_data_p->local_rx_enabled == 1) && (h_data_p->ma_base_index != _BCM_OAM_INVALID_INDEX);
    if (is_remote || !is_rx_enabled) {
        /* Dont touch anything for remote endpoints or only tx enabled */
        return BCM_E_NONE;
    }
    rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_op(unit, h_data_p, mem, 0); 

    return rv;
}

STATIC int
_bcm_kt2_oam_ma_idx_to_ep_id_mapping_delete (int unit,
                             _bcm_oam_hash_data_t *h_data_p)
{
    int is_upmep = 0;
    int is_remote = 0;
    int is_rx_enabled = 0;
    soc_mem_t mem = MA_INDEXm;
    int rv = BCM_E_NONE;

    /* h_data_p being NULL could already be some other error
     * scenario. Lets not return error for such a scenario.
     */
    if (h_data_p == NULL) {
        return BCM_E_NONE;
    }

    is_upmep = (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING)?1:0;
    if (is_upmep) {
        mem = EGR_MA_INDEXm;
    }
    is_remote = (h_data_p->flags & BCM_OAM_ENDPOINT_REMOTE)?1:0;
    is_rx_enabled =  (h_data_p->local_rx_enabled == 1) && (h_data_p->ma_base_index != _BCM_OAM_INVALID_INDEX);
    if (is_remote || !is_rx_enabled) {
        /* Dont touch anything for remote endpoints or only tx enabled */
        return BCM_E_NONE;
    }
    rv =  _bcm_kt2_oam_ma_idx_to_ep_id_mapping_op(unit, h_data_p, mem, 1); 

    return rv;
}

STATIC INLINE bcm_oam_endpoint_t
_bcm_kt2_oam_ma_idx_to_ep_id_mapping_get (_bcm_oam_control_t   *oc, int ma_idx)
{
    return oc->_bcm_oam_ma_idx_to_ep_id_map[ma_idx];
}

STATIC int _bcm_kt2_oam_ma_idx_to_ep_id_mapping_init (int unit)
{
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_ALLOC(oc->_bcm_oam_ma_idx_to_ep_id_map, bcm_oam_endpoint_t,
                       _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_TBL_SIZE , "MA_IDX_TO_EP_ID map");

    if (oc->_bcm_oam_ma_idx_to_ep_id_map == NULL) {
        return BCM_E_MEMORY;
    }

    /* Init it to all invalid */
    sal_memset(oc->_bcm_oam_ma_idx_to_ep_id_map, _BCM_OAM_INVALID_INDEX,
                _BCM_KT2_OAM_MA_IDX_TO_EP_ID_MAPPING_TBL_SIZE);
    return BCM_E_NONE;
}

STATIC int _bcm_kt2_oam_ma_idx_to_ep_id_mapping_destroy (int unit)
{
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if (oc->_bcm_oam_ma_idx_to_ep_id_map != NULL) {
        sal_free(oc->_bcm_oam_ma_idx_to_ep_id_map);
        oc->_bcm_oam_ma_idx_to_ep_id_map = NULL;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_kt2_oam_ma_index_entry_set(int unit,
                             _bcm_oam_hash_data_t *h_data_p,
                             soc_mem_t mem,
                             void *ma_idx_ptr, int delete)
{
    int                rv = BCM_E_NONE;
    uint32             mdl_value[1] = { 0 };
    int                bit_pos = 0;
    int                current_offset = 0;
    int                new_offset = 0;
    int                entry_count = 0;
    int                entry_mem_size = 0; 
    ma_index_entry_t *entry_buf;
    ma_index_entry_t *entry;
    ma_index_entry_t *entry_current;
    shr_idxres_list_handle_t pool = NULL;

    if (NULL == h_data_p) {
        return (BCM_E_INTERNAL);
    }

    if (mem == MA_INDEXm) { 
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit,
                    h_data_p, &entry_count, &pool));
        new_offset = entry_count;
        entry_mem_size = sizeof(ma_index_entry_t);
    } else {
        entry_mem_size = sizeof(egr_ma_index_entry_t);
        new_offset = entry_count = 8;
    }
    if(entry_count <= 0) {
        return BCM_E_INTERNAL;
    }
    /* Allocate buffer to store the DMAed table entries. */
    entry_buf = soc_cm_salloc(unit, entry_mem_size * entry_count * 2,
                              "MA index table entry buffer");
    if (NULL == entry_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(entry_buf, 0, entry_mem_size * entry_count * 2);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL, h_data_p->ma_base_index, 
                            (h_data_p->ma_base_index + entry_count - 1), 
                            entry_buf);
    if (BCM_FAILURE(rv)) {
        if (entry_buf) {
            soc_cm_sfree(unit, entry_buf);
        }
        return rv;
    }

    mdl_value[0] = h_data_p->active_mdl_bitmap;

    if (delete) {
        for (bit_pos = 0; bit_pos <= 7; bit_pos++) {
            if (SHR_BITGET(mdl_value, bit_pos)) {
               if (bit_pos == h_data_p->level) {
                   current_offset++;
               } else {
                   entry = soc_mem_table_idx_to_pointer(unit, mem, 
                                                    ma_index_entry_t *,
                                                    entry_buf, new_offset);
                   entry_current = soc_mem_table_idx_to_pointer(unit, mem, 
                                                    ma_index_entry_t *,
                                                    entry_buf, current_offset);
                   sal_memcpy(entry, entry_current, entry_mem_size);
                   new_offset++;
                   current_offset++;
               }    
            }
        }
    } else {
        for (bit_pos = 0; bit_pos <= 7; bit_pos++) {
            if (SHR_BITGET(mdl_value, bit_pos)) {
               if (bit_pos == h_data_p->level) {
                   entry = soc_mem_table_idx_to_pointer(unit, mem, 
                                                    ma_index_entry_t *,
                                                    entry_buf, new_offset);
                   sal_memcpy(entry, ma_idx_ptr, entry_mem_size);
                   /* Doing Mod entry_count since new_offset starts
                      from entry_count */
                   h_data_p->local_rx_index = h_data_p->ma_base_index +
                                     (new_offset % entry_count);
                   new_offset++;
               } else {
                   entry = soc_mem_table_idx_to_pointer(unit, mem, 
                                                    ma_index_entry_t *,
                                                    entry_buf, new_offset);
                   entry_current = soc_mem_table_idx_to_pointer(unit, mem, 
                                                    ma_index_entry_t *,
                                                    entry_buf, current_offset);
                   sal_memcpy(entry, entry_current, entry_mem_size);
                   new_offset++;
                   current_offset++;
               }    
            }
        }
    }
    /* Move to the entry_count index since from there the newly programmed
     * ma_index table memory is present */
    entry = soc_mem_table_idx_to_pointer(unit, mem, ma_index_entry_t *,
                                                    entry_buf, entry_count);
    rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL, h_data_p->ma_base_index,
                             (h_data_p->ma_base_index + entry_count - 1), 
                             entry);
    if (BCM_FAILURE(rv)) {
        if (entry_buf) {
            soc_cm_sfree(unit, entry_buf);
        }
        return rv;
    }
    if (entry_buf) {
        soc_cm_sfree(unit, entry_buf);
    }
    return (BCM_E_NONE);
} 

STATIC int
_bcm_kt2_oam_port_table_ma_index_offset_get(int unit, 
                                            _bcm_oam_hash_data_t *h_data_p, 
                                            int *ma_offset) 
{
    int                rv = BCM_E_NONE;
    uint32             mdl_value[1] = { 0 };
    int                bit_pos = 0;
    bcm_port_t         port_id;              /* Port ID.            */
    int                local_member_count = 0;
    port_tab_entry_t   port_entry;
    int                offset_found = 0;

    if (NULL == h_data_p) {
        return (BCM_E_INTERNAL);
    }
    if(h_data_p->trunk_id != BCM_TRUNK_INVALID) {
        rv = _bcm_kt2_oam_trunk_port_mdl_config(unit, h_data_p);

        /* Get a member of the trunk belonging to this module */
        if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, 
                                           h_data_p->trunk_id, 1,
                                           &port_id,
                                           &local_member_count))) {
        } else {
            return (BCM_E_INTERNAL);
        }
        BCM_IF_ERROR_RETURN
              (soc_mem_read(unit, PORT_TABm, 
                            MEM_BLOCK_ANY, port_id, &port_entry));
    } else {
         BCM_IF_ERROR_RETURN
              (soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, 
                            h_data_p->src_pp_port, &port_entry));
    }
    mdl_value[0] = soc_PORT_TABm_field32_get(unit, &port_entry, MDL_BITMAPf);
    for (bit_pos = 0; bit_pos <= 7; bit_pos++) {
        if (SHR_BITGET(mdl_value, bit_pos)) {
            if (bit_pos == h_data_p->level) {
                offset_found = 1; 
                break;   
            } else {
                *ma_offset += 1;
            }
        }
    }
    h_data_p->active_mdl_bitmap = mdl_value[0];
    /* Added the line below to avoid compiler warning - unused variable */  
    mdl_value[0] = *ma_offset;
    if (offset_found == 0) {
        return (BCM_E_INTERNAL);
    }
    return rv;
}


STATIC int
_bcm_kt2_oam_mp_grp_table_ma_index_offset_get(int unit, 
                                            _bcm_oam_hash_data_t *h_data_p, 
                                            int *ma_offset) 
{
    int                rv = BCM_E_NONE;
    uint32             mdl_value[1] = { 0 };
    int                bit_pos = 0;
    soc_mem_t          mem = 0;
    l3_entry_1_entry_t l3_entry;      /* L3 entry buffer.                 */
    int                entry_index = 0;
    egr_mp_group_entry_t egr_mp_group;  /* Egress MP group tbl entry buffer */
    void               *entry;
    soc_field_t        mdl_field = 0;
    int                offset_found = 0;

    if (NULL == h_data_p) {
        return (BCM_E_INTERNAL);
    }
    if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        mem = EGR_MP_GROUPm;
        mdl_field = MDL_BITMAP_ACTIVEf;
        entry = &egr_mp_group;
        if (BCM_FAILURE(_bcm_kt2_oam_find_egr_lmep(unit, h_data_p, 
                                                   &entry_index, 
                                                   &egr_mp_group))) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EGR MP group tbl entry not found-%s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        }
    } else {
        mem = L3_ENTRY_IPV4_UNICASTm;
        mdl_field = LMEP__MDL_BITMAP_ACTIVEf;
        entry = &l3_entry;
        if (BCM_FAILURE
            (_bcm_kt2_oam_find_lmep(unit, h_data_p, &entry_index, &l3_entry))) {

            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: MP group tbl entry not found - %s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        }
    }
    mdl_value[0] = soc_mem_field32_get(unit, mem, entry, mdl_field);
    for (bit_pos = 0; bit_pos <= 7; bit_pos++) {
        if (SHR_BITGET(mdl_value, bit_pos)) {
            if (bit_pos == h_data_p->level) {
                offset_found = 1; 
                break;   
            } else {
                *ma_offset += 1;
            }
        }
    }
    h_data_p->active_mdl_bitmap = mdl_value[0];
    if (offset_found == 0) {
        return (BCM_E_INTERNAL);
    }
    return rv;
}


STATIC int
_bcm_kt2_oam_ma_index_offset_get(int unit, _bcm_oam_hash_data_t *h_data_p, 
                                 int *ma_offset) 
{
    int                rv = BCM_E_NONE;
    if (NULL == h_data_p) {
        return (BCM_E_INTERNAL);
    }

    /* setting this to 0 is necessary as in the below functions this variable
     * passed as a pointer is just incremented based upon the level.
     * setting it to zero avoids any junk value that might have come in 
     * by the caller function. */
    *ma_offset = 0;

    if (h_data_p->oam_domain == _BCM_OAM_DOMAIN_PORT) {
        rv = _bcm_kt2_oam_port_table_ma_index_offset_get(unit, h_data_p, 
                                                         ma_offset);
        /* BCM_E_NOT_FOUND is not an error when switch is re-initializing
           as entry would have been already deleted by port module init */
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Port table read failed - %s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        }
    } else {
        rv = _bcm_kt2_oam_mp_grp_table_ma_index_offset_get(unit, h_data_p, 
                                                         ma_offset);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: MP group table read failed - %s.\n"),
                       bcm_errmsg(rv)));
            return rv;
        } 
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_kt2_oam_local_rx_mep_hw_set
 * Purpose:
 *     Configure hardware tables for a local CCM Rx enabled endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_local_rx_mep_hw_set(int unit,
                             bcm_oam_endpoint_info_t *ep_info_p)
{
    _bcm_oam_hash_data_t *h_data_p;      /* Endpoint hash data pointer.      */
    ma_index_entry_t     ma_idx_entry;   /* MA_INDEX table entry buffer.     */
    egr_ma_index_entry_t egr_ma_idx_entry; /* EGR_MA_INDEX table entry buffer*/
    oam_opcode_control_profile_entry_t opcode_entry; /* Opcode control 
                                                             profile entry.  */
    egr_oam_opcode_control_profile_entry_t egr_opcode_entry; /* Egress  
                                                 Opcode control profile entry*/
    void                 *entries[1];    /* Pointer to opcode control entry. */
    uint32               profile_index;  /* opcode control profile index.    */
    int                  rv;             /* Operation return status.         */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    bcm_oam_endpoint_info_t *ep_p;       /* Pointer to endpoint info.        */
    soc_mem_t            opcode_profile_mem = 0;
    soc_mem_t            ma_index_mem = 0;
    shr_idxres_element_t egr_ctr_index = 0;
    soc_profile_mem_t    *opcode_control_profile; /* profile to be used 
                                                        ingress or egress    */
    void                 *ma_idx_ptr;
    void                 *opcode_profile_entry;

    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Initialize local endpoint info pointer. */
    ep_p = ep_info_p;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get endpoint hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_p->id];
    if ( 0 == h_data_p->in_use) {
        return (BCM_E_INTERNAL);
    }

    if (ep_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        opcode_profile_mem = EGR_OAM_OPCODE_CONTROL_PROFILEm;
        opcode_control_profile = &oc->egr_oam_opcode_control_profile;
        ma_index_mem   = EGR_MA_INDEXm;
        ma_idx_ptr = &egr_ma_idx_entry; 
        opcode_profile_entry = &egr_opcode_entry;
        /* Clear the entry data. */
        sal_memset(&egr_opcode_entry, 0, 
                   sizeof(egr_oam_opcode_control_profile_entry_t));
        sal_memset(&egr_ma_idx_entry, 0, sizeof(egr_ma_index_entry_t));
    } else {
        opcode_profile_mem = OAM_OPCODE_CONTROL_PROFILEm;
        opcode_control_profile = &oc->oam_opcode_control_profile;
        ma_index_mem   = MA_INDEXm;
        ma_idx_ptr = &ma_idx_entry;
        opcode_profile_entry = &opcode_entry;
        /* Clear the entry data. */
        sal_memset(&opcode_entry, 0, 
                   sizeof(oam_opcode_control_profile_entry_t));
        sal_memset(&ma_idx_entry, 0, sizeof(ma_index_entry_t));
    }

    /* Construct the opcode control profile table entry. */
    if (ep_p->opcode_flags & _BCM_KT2_OAM_OPCODE_MASK) {
        /* Use application specified opcode control settings. */
        rv = _bcm_kt2_oam_opcode_profile_entry_set(unit, h_data_p, 
                                                   opcode_profile_mem,
                                                   ep_p->opcode_flags,
                                                   opcode_profile_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Opcode profile set failed for EP=%d"
                                   "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
    } else {
        /* Use default opcode control profile settings. */
        if (_BCM_OAM_EP_IS_MIP(h_data_p)) {
            rv = _bcm_kt2_oam_mip_opcode_profile_entry_init(unit, opcode_profile_mem,
                    opcode_profile_entry);
        } else {
            rv = _bcm_kt2_oam_opcode_profile_entry_init(unit, opcode_profile_mem, 
                    opcode_profile_entry);
        }
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Opcode profile init failed for EP=%d"
                                   "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    soc_mem_lock(unit, opcode_profile_mem);
    /* Add entry to profile table.  */
    entries[0] = opcode_profile_entry;

    rv = soc_profile_mem_add(unit, opcode_control_profile,
                             (void *)entries, 1, &profile_index);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Opcode profile add failed for EP=%d"
                               "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
        /* Release opcode control profile table lock. */
        soc_mem_unlock(unit, opcode_profile_mem);
        return (rv);
    }

    /* Store endpoint OAM opcode profile table index value. */
    h_data_p->profile_index = profile_index;

    /* Release opcode control profile table lock. */
    soc_mem_unlock(unit, opcode_profile_mem);

    /*
     * MA_INDEX table programming.
     */


    /* Set group index value. */
    /* In case of BHH this needs to be the session Index */
#if defined(INCLUDE_BHH)
    if ( BHH_EP_TYPE(ep_p) ) {
        soc_mem_field32_set(unit, ma_index_mem, ma_idx_ptr, MA_PTRf,
                            BCM_OAM_BHH_GET_UKERNEL_EP(ep_p->id));
    } else 
#endif
    {
        soc_mem_field32_set(unit, ma_index_mem, ma_idx_ptr, MA_PTRf,
                            ep_p->group);
    }
    


    /* Set OAM opcode control profile table index. */
    soc_mem_field32_set(unit, ma_index_mem, ma_idx_ptr,
                              OAM_OPCODE_CONTROL_PROFILE_PTRf,
                              h_data_p->profile_index);

    soc_mem_field32_set(unit, ma_index_mem, ma_idx_ptr,
                                  INT_PRIf,
                                  ep_p->int_pri);


    /* If the OAM domain is port, set source trunk map table */
    if (h_data_p->oam_domain == _BCM_OAM_DOMAIN_PORT) {
        rv = _bcm_port_domain_oam_hw_set(unit, ep_info_p);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: port OAM config failed for EP=%d"
                                   "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
    } else {
        rv = _bcm_kt2_oam_l3_entry_set(unit, ep_info_p);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3 entry config failed for EP=%d"
                                   "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }
        rv = _bcm_kt2_oam_egr_mp_group_entry_set(unit, ep_info_p, 
                                                       &egr_ctr_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Egress MP group entry config "
                                  "failed for EP=%d %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        }

        if ((ep_p->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT)) {
            rv = _bcm_kt2_oam_lmep_counters_set(unit, ep_info_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Counter config failed for EP=%d"
                                       "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
                if (h_data_p->rx_ctr != _BCM_OAM_INVALID_INDEX) {
                    rv =  shr_aidxres_list_free(oc->ing_lm_ctr_pool[
                                                  (h_data_p->rx_ctr >> 24)],
                                                  (h_data_p->rx_ctr & 0xffffff));
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "OAM Error: Ing LM counter block "
                                              "free failed (EP=%d) - %s.\n"),
                                   ep_info_p->id, bcm_errmsg(rv)));
                        return (rv);
                    }
                }
                return (rv);
            }
        }
    }
    rv = _bcm_kt2_oam_ma_index_entry_set(unit, h_data_p, 
                                         ma_index_mem, ma_idx_ptr, 0);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MA_INDEX table write failed for EP=%d"
                               "  %s.\n"), ep_p->id, bcm_errmsg(rv)));
        return (rv);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_upmep_rx_endpoint_reserve
 * Purpose:
 *     Reserve an hardware index in the group state table to maintain the state
 *     for a CCM Rx enabled endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_upmep_rx_endpoint_reserve(int unit,  
                             const bcm_oam_endpoint_info_t *ep_info_p)
{
    _bcm_oam_hash_data_t   *h_data_p;     /* Endpoint hash data pointer.      */
    int                    rv;            /* Operation return status.         */
    egr_mp_group_entry_t   egr_mp_grp_entry; /* egress mp group entry buffer. */
    int                    mp_group_index = -1;/*MP group entry hardware index*/
    int                    count = 0;     /* Successful Hw indices allocated. */
    uint8                  mdl = 0;       /* Maintenance domain level.        */
    int                    rx_index[1 << _BCM_OAM_EP_LEVEL_BIT_COUNT] = {0};
                                          /* Endpoint Rx hardware index.      */
    uint16                 ma_base_idx;   /* Base pointer to endpoint state   */
                                          /* table [MA_INDEX = (BASE + MDL)]. */
    _bcm_oam_control_t     *oc;           /* Pointer to control structure.    */
    const bcm_oam_endpoint_info_t *ep_p;  /* Pointer to endpoint information. */

    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Initialize local endpoint information pointer. */
    ep_p = ep_info_p;

    /* Get endpoint hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_p->id];
    if (0 == h_data_p->in_use) {
        return(BCM_E_INTERNAL);
    }

    /* Initialize egress MP group table entry buffer. */
    sal_memset(&egr_mp_grp_entry, 0, sizeof(egr_mp_group_entry_t));

    /* Find out if a matching entry already installed in hardware table. */
    rv = _bcm_kt2_oam_find_egr_lmep(unit, h_data_p, &mp_group_index, 
                                                        &egr_mp_grp_entry);
    if (BCM_FAILURE(rv)) {
        /*
         * If NO match found, allocate a new hardware index from EGR_MA_INDEX
         * pool.
         */

        /*
         * Endpoint MDL values can be (0-7) i.e 8 MDLs are supported per-MA
         * group endpoints. While allocating the base index, next 7 hardware
         * indices are also reserved.
         */
        rv = shr_idxres_list_alloc_set(oc->egr_ma_idx_pool, 8,
                                       (shr_idxres_element_t *) rx_index,
                                       (shr_idxres_element_t *) &count);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EGR MA_INDEX index alloc failed EP:%d"
                                   " %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        } else {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Info: Egr MA_INDEX alloc for EP:%d success"
                                   " rx_idx_base:%d alloc-count:%d.\n"), ep_p->id,
                       rx_index[0], count));
            h_data_p->ma_base_index = rx_index[0];
        }

    } else if (BCM_SUCCESS(rv)) {

        /* Matching entry found, get installed entry MDL value. */
        mdl = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_grp_entry, 
                                                       MDL_BITMAP_ACTIVEf);

        if (mdl == 0) {
            /* This is the first one in the active bitmap.
               Allocate the hw indexes. */
            /*
             * Endpoint MDL values can be (0-7) i.e 8 MDLs are supported per-MA
             * group endpoints. While allocating the base index, next 7 hardware
             * indices are also reserved.
             */
            rv = shr_idxres_list_alloc_set(oc->egr_ma_idx_pool, 8,
                    (shr_idxres_element_t *) rx_index,
                    (shr_idxres_element_t *) &count);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: EGR MA_INDEX index alloc failed EP:%d"
                                    " %s.\n"), unit, ep_p->id, bcm_errmsg(rv)));
                return (rv);
            } else {
                LOG_DEBUG(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Info: Egr MA_INDEX alloc for EP:%d success"
                                    " rx_idx_base:%d alloc-count:%d.\n"), unit, ep_p->id,
                         rx_index[0], count));
                h_data_p->ma_base_index = rx_index[0];
            }
        } else if (0 == (mdl & (1 << ep_p->level))) { /* Findout if MDLs are same. */
            ma_base_idx = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_grp_entry,
                                                      MA_BASE_PTRf);
            h_data_p->ma_base_index = ma_base_idx;
        } else {
            /* Rx index already taken return error. */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: No free Rx index found for EP:%d"
                                   " %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (BCM_E_RESOURCE);
        }
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_downmep_rx_endpoint_reserve
 * Purpose:
 *     Reserve an hardware index in the group state table to maintain the state
 *     for a CCM Rx enabled endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to remote endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_downmep_rx_endpoint_reserve(int unit, 
                                const bcm_oam_endpoint_info_t *ep_info_p)
{
    _bcm_oam_hash_data_t   *h_data_p;     /* Endpoint hash data pointer.      */
    int                    rv;            /* Operation return status.         */
    l3_entry_1_entry_t     l3_entry;      /* L3 entry buffer.                 */
    int                    l3_index = -1; /* L3 entry hardware index.         */
    int                    count = 0;     /* Successful Hw indices allocated. */
    uint8                  mdl = 0;       /* Maintenance domain level.        */
    int                    rx_index[1 << _BCM_OAM_EP_LEVEL_BIT_COUNT] = {0};
                                          /* Endpoint Rx hardware index.      */
    uint16                 ma_base_idx;   /* Base pointer to endpoint state   */
                                          /* table [MA_INDEX = (BASE + MDL)]. */
    _bcm_oam_control_t     *oc;           /* Pointer to control structure.    */
    const bcm_oam_endpoint_info_t *ep_p;  /* Pointer to endpoint information. */
    int                     stm_index = 0;
    source_trunk_map_table_entry_t stm_entry;
    int                    ma_idx_entry_count = 0;
    shr_idxres_list_handle_t pool = NULL;


    if (NULL == ep_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Initialize local endpoint information pointer. */
    ep_p = ep_info_p;

    /* Get endpoint hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_p->id];
    if ( 0 == h_data_p->in_use) {
        return (BCM_E_INTERNAL);
    }
    
    /* Initialize L3 entry buffer. */
    sal_memset(&l3_entry, 0, sizeof(l3_entry_1_entry_t));
    sal_memset(&stm_entry, 0, sizeof(source_trunk_map_table_entry_t));

    if (h_data_p->oam_domain == _BCM_OAM_DOMAIN_PORT) {
        rv = _bcm_kt2_oam_find_port_lmep(unit, h_data_p, &stm_index, &stm_entry, &mdl);
    } else {
        /* Find out if a matching entry already installed in hardware table. */
        rv = _bcm_kt2_oam_find_lmep(unit, h_data_p, &l3_index, &l3_entry);
    }
    if (BCM_FAILURE(rv)) {
        /*
         * If NO match found, allocate a new hardware index from MA_INDEX
         * pool.
         */

        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit, h_data_p,
                    &ma_idx_entry_count, &pool));
        /*
         * Endpoint MDL values can be (0-7) i.e 8 MDLs are supported per-MA
         * group endpoints. While allocating the base index, next 7 hardware
         * indices are also reserved.
         */
        rv = shr_idxres_list_alloc_set(pool, ma_idx_entry_count,
                           (shr_idxres_element_t *) rx_index,
                           (shr_idxres_element_t *) &count);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: MA_INDEX index alloc failed EP:%d"
                                   " %s.\n"), ep_p->id, bcm_errmsg(rv)));
            return (rv);
        } else {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Info: MA_INDEX alloc for EP:%d success."
                                   " rx_idx_base:%d alloc-count:%d.\n"), ep_p->id,
                       rx_index[0], count));
        }

        h_data_p->ma_base_index = rx_index[0];
    } else if (BCM_SUCCESS(rv)) {
        if (h_data_p->oam_domain == _BCM_OAM_DOMAIN_PORT) {
            /* MDL already found.*/
            /* Findout if MDLs are same. */
            if (0 == (mdl & (1 << ep_p->level))) {
                ma_base_idx =
                    soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm_entry,
                            MA_BASE_PTRf);
                h_data_p->ma_base_index = ma_base_idx;
            } else {
                /* Rx index already taken return error. */
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM Error: No free Rx index found for EP:%d"
                                    " %s.\n"), ep_p->id, bcm_errmsg(rv)));
                return (BCM_E_RESOURCE);
            }
        } else {
            /* Matching entry found, get installed entry MDL value. */
            mdl = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                    LMEP__MDL_BITMAP_ACTIVEf);

            if (mdl == 0) {
                /* This is the first one in the active bitmap.
                   Allocate the hw indexes. */
                BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit, h_data_p,
                            &ma_idx_entry_count, &pool));
                /*
                 * Endpoint MDL values can be (0-7) i.e 8 MDLs are supported per-MA
                 * group endpoints. While allocating the base index, next 7 hardware
                 * indices are also reserved.
                 */
                rv = shr_idxres_list_alloc_set(pool, ma_idx_entry_count,
                        (shr_idxres_element_t *) rx_index,
                        (shr_idxres_element_t *) &count);

                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: MA_INDEX index alloc failed EP:%d"
                                        " %s.\n"), unit, ep_p->id, bcm_errmsg(rv)));
                    return (rv);
                } else {
                    LOG_DEBUG(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Info: MA_INDEX alloc for EP:%d success."
                                        " rx_idx_base:%d alloc-count:%d.\n"), unit, ep_p->id,
                             rx_index[0], count));
                }

                h_data_p->ma_base_index = rx_index[0];
            } else if (0 == (mdl & (1 << ep_p->level))) { /* Findout if MDLs are same. */
                ma_base_idx = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                        LMEP__MA_BASE_PTRf);
                h_data_p->ma_base_index = ma_base_idx;
            } else {
                /* Rx index already taken return error. */
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM Error: No free Rx index found for EP:%d"
                                    " %s.\n"), ep_p->id, bcm_errmsg(rv)));
                return (BCM_E_RESOURCE);
            }
        }
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_remote_endpoint_delete
 * Purpose:
 *     Delete a remote endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p  - (IN) Pointer to endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_remote_endpoint_delete(int unit,
                                    _bcm_oam_hash_data_t *h_data_p)
{
    _bcm_oam_control_t *oc;            /* Pointer to OAM control structure.  */
    l3_entry_1_entry_t l3_entry;       /* RMEP view table entry.             */
    ma_state_entry_t   ma_state_entry; /* Group state machine table entry.   */
    rmep_entry_t       rmep_entry;     /* Remote endpoint table entry.       */
    int                rv;             /* Operation return status.           */
    uint32 some_rmep_ccm_defect_counter = 0; /* No. of RMEPs in MA with CCM  */
                                         /* defects.                         */
    uint32 some_rdi_defect_counter = 0;  /* No. of RMEPs in MA with RDI      */
                                         /* defects.                         */
    uint32 cur_some_rdi_defect = 0;      /* Any RMEP in MA with RDI defect.  */
    uint32 cur_some_rmep_ccm_defect = 0; /* Any RMEP in MA with CCM defect.  */
    uint32 cur_rmep_ccm_defect = 0;      /* RMEP lookup failed or CCM        */
                                         /* interval mismatch.               */
    uint32 cur_rmep_last_rdi = 0;        /* Last CCM RDI Rx from this RMEP.  */
    uint32 first, last, validLow, validHigh, freeCount, allocCount;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));

    /* Get remote endpoint entry value from hardware. */
    rv = READ_RMEPm(unit, MEM_BLOCK_ANY, h_data_p->remote_index, &rmep_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: RMEP table read failed for EP=%d"
                               "%s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
        return (rv);
    }

    cur_rmep_ccm_defect
        = soc_RMEPm_field32_get(unit, &rmep_entry,
                                CURRENT_RMEP_CCM_DEFECTf);

    cur_rmep_last_rdi
        = soc_RMEPm_field32_get(unit, &rmep_entry,
                                CURRENT_RMEP_LAST_RDIf);

    sal_memset(&ma_state_entry, 0, sizeof(ma_state_entry));
    rv = READ_MA_STATEm(unit, MEM_BLOCK_ANY, h_data_p->group_index,
                        &ma_state_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Group state (GID=%d) table read"
                               " failed - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
        return (rv);
    }

    if ((0 != cur_rmep_ccm_defect) || (0 != cur_rmep_last_rdi)) {
        some_rmep_ccm_defect_counter
            = soc_MA_STATEm_field32_get(unit, &ma_state_entry,
                                        SOME_RMEP_CCM_DEFECT_COUNTERf);
        cur_some_rmep_ccm_defect
            = soc_MA_STATEm_field32_get(unit, &ma_state_entry,
                                        CURRENT_SOME_RMEP_CCM_DEFECTf);

        if ((0 != cur_rmep_ccm_defect)
            && (some_rmep_ccm_defect_counter > 0)) {
            --some_rmep_ccm_defect_counter;
            soc_MA_STATEm_field32_set
                (unit, &ma_state_entry, SOME_RMEP_CCM_DEFECT_COUNTERf,
                 some_rmep_ccm_defect_counter);

            if (0 == some_rmep_ccm_defect_counter) {
                cur_some_rmep_ccm_defect = 0;
                soc_MA_STATEm_field32_set
                    (unit, &ma_state_entry, CURRENT_SOME_RMEP_CCM_DEFECTf,
                     cur_some_rmep_ccm_defect);
            }
        }

        some_rdi_defect_counter
            = soc_MA_STATEm_field32_get(unit, &ma_state_entry,
                                        SOME_RDI_DEFECT_COUNTERf);
        cur_some_rdi_defect
            = soc_MA_STATEm_field32_get(unit, &ma_state_entry,
                                        CURRENT_SOME_RDI_DEFECTf);

        if ((0 != cur_rmep_last_rdi)
            && (some_rdi_defect_counter > 0)) {
            --some_rdi_defect_counter;
            soc_MA_STATEm_field32_set
                (unit, &ma_state_entry, SOME_RDI_DEFECT_COUNTERf,
                 some_rdi_defect_counter);

            if (0 == some_rdi_defect_counter) {
                cur_some_rdi_defect = 0;
                soc_MA_STATEm_field32_set
                    (unit, &ma_state_entry, CURRENT_SOME_RDI_DEFECTf,
                     cur_some_rdi_defect);
            }
        }

        rv = WRITE_MA_STATEm(unit, MEM_BLOCK_ALL, h_data_p->group_index,
                             &ma_state_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Group state (GID=%d) table write"
                                   " failed - %s.\n"), h_data_p->group_index,
                       bcm_errmsg(rv)));
            return (rv);
        }
    }

    /* Clear RMEP table entry for this endpoint index. */
    sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));
    rv = WRITE_RMEPm(unit, MEM_BLOCK_ALL, h_data_p->remote_index,
                     &rmep_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: RMEP table write index=%x (EP=%d)"
                               " - %s.\n"), h_data_p->remote_index,
                   h_data_p->ep_id, bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(&l3_entry, 0, sizeof(l3_entry_1_entry_t));

    /* Construct endpoint RMEP view key for L3 Table entry delete operation. */
    _bcm_kt2_oam_rmep_key_construct(unit, h_data_p, &l3_entry);

    rv = soc_mem_delete(unit, L3_ENTRY_IPV4_UNICASTm, MEM_BLOCK_ALL, &l3_entry);
    if (BCM_FAILURE(rv) && (oc->init)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: RMEP view update (EP=%d)"
                               " - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
        return (rv);
    }

    /* Return ID back to free RMEP ID pool. */
    BCM_IF_ERROR_RETURN
        (shr_idxres_list_free(oc->rmep_pool, h_data_p->remote_index));

    rv = shr_idxres_list_state(oc->rmep_pool,
                                   &first, &last,
                                   &validLow, &validHigh,
                                   &freeCount, &allocCount);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: RMEP pool state get failed "
                               " - %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    if(allocCount == 0)
    {
        soc_MA_STATEm_field32_set
            (unit, &ma_state_entry, CURRENT_XCON_CCM_DEFECTf,
             0);
        soc_MA_STATEm_field32_set
            (unit, &ma_state_entry, CURRENT_ERROR_CCM_DEFECTf,
             0);
        soc_MA_STATEm_field32_set
            (unit, &ma_state_entry, STICKY_ERROR_CCM_DEFECTf,
             0);
        soc_MA_STATEm_field32_set
            (unit, &ma_state_entry, ERROR_CCM_DEFECT_TIMESTAMPf,
             0);
        soc_MA_STATEm_field32_set
            (unit, &ma_state_entry, ERROR_CCM_DEFECT_RECEIVE_CCMf,
             0);
        rv = WRITE_MA_STATEm(unit, MEM_BLOCK_ALL, h_data_p->group_index,
                &ma_state_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Error: Group state (GID=%d) table write"
                                " failed - %s.\n"), h_data_p->group_index,
                     bcm_errmsg(rv)));
            return (rv);
        }
    }

    /* Clear the H/w index to logical index Mapping for RMEP */
    oc->remote_endpoints[h_data_p->remote_index] = BCM_OAM_ENDPOINT_INVALID; 


    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_clear_counter
 * Purpose:
 *    Remove the counter associated with the endpoint being deleted 
 * Parameters:
 *     unit      - (IN) BCM device number
 *     mem       - (IN) L3_ENTRY_IPV4_UNICAST/EGR_MP_GROUP
 *     index     - (IN) Index of the table entry to be modified
 *     h_data_p  - (IN) Pointer to endpoint hash data
 *     entry     - (IN) pointer to table entry
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_clear_counter(int unit, soc_mem_t mem,
                           int index,
                           _bcm_oam_hash_data_t *h_data_p, 
                           void *entry)
{
    int  rv = BCM_E_NONE;
    mep_ctr_info_t  *ctr_info;
    int           ctr_type = 0;
    int           ctr1_valid = 0;
    int           ctr2_valid = 0;
    int           ctr_mdl = 0;
    int           up_mep = 0;

    if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        up_mep = 1;
    }
    if (mem == L3_ENTRY_IPV4_UNICASTm) {
        ctr_info = mep_ctr_info;
    } else {
        ctr_info = egr_mep_ctr_info;
    }
    ctr1_valid = soc_mem_field32_get(unit, mem, 
                                    (uint32 *)entry, ctr_info[0].ctr_valid);
    if (ctr1_valid) {
        ctr_type = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[0].ctr_mep_type); 
        ctr_mdl = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[0].ctr_mep_mdl); 
        if ((ctr_type == up_mep) && (ctr_mdl == h_data_p->level)) {
            ctr1_valid = 0;
            rv = soc_mem_field32_modify(unit, mem, index,
                                                     ctr_info[0].ctr_valid, 0);
        }
    }   
    ctr2_valid = soc_mem_field32_get(unit, mem, 
                                    (uint32 *)entry, ctr_info[1].ctr_valid);
    if (ctr2_valid) {
        ctr_type = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[1].ctr_mep_type); 
        ctr_mdl = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[1].ctr_mep_mdl); 
        if ((ctr_type == up_mep) && (ctr_mdl == h_data_p->level)) {
            rv = soc_mem_field32_modify(unit, mem, index,
                                                     ctr_info[1].ctr_valid, 0);
            ctr2_valid = 0;
        }
    }

    /* return rx & tx counters allocated if any */
    rv =_bcm_kt2_oam_free_counter(unit, h_data_p);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM Error: LM counter block "
                            "free failed (EP=%d) - %s.\n"),
                 h_data_p->ep_id, bcm_errmsg(rv)));
        return (rv);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_stm_table_clear
 * Purpose:
 *     Clear MA base ptr and return MA index to the pool
 * Parameters:
 *     unit      - (IN) BCM device number
 *     mod_id    - (IN) Module Id
 *     index     - (IN) stm member index
 *     port_id   - (IN) port number
 *     h_data_p - (IN) Pointer to hash data 
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_stm_table_clear(int unit, bcm_module_t mod_id, int index,
                              bcm_port_t port_id, 
                              _bcm_oam_hash_data_t *h_data_p)
{
    int                  rv = BCM_E_NONE;/* Operation return status.        */
    int                  stm_index = 0;
    source_trunk_map_table_entry_t stm_entry;
    uint16               ma_base_idx;   /* Base pointer to endpoint state   */
                                        /* table [MA_INDEX = (BASE + MDL)]. */
    int                  idx = 0;    
    _bcm_oam_control_t   *oc;           /* Pointer to control structure.    */
    int                  count = 0;
    int                  ma_idx_entry_count = 0;
    shr_idxres_list_handle_t pool = NULL;

    uint32 rx_index[1 << _BCM_OAM_EP_LEVEL_BIT_COUNT] = {0};

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);

    rv = _bcm_esw_src_mod_port_table_index_get(unit,
                                        mod_id, port_id, &stm_index);
    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                                   stm_index, &stm_entry);

    if(BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    ma_base_idx = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, 
                                                 &stm_entry, MA_BASE_PTRf);
    if (index == 0) {
        /* Return Rx indices to free pool. */
        for (idx = 0; idx < (1 << _BCM_OAM_EP_LEVEL_BIT_COUNT); idx++) {
            rx_index[idx] = ma_base_idx + idx;
        }

        rv = _bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit, h_data_p,
                    &ma_idx_entry_count, &pool);
        if(BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
            return rv;
        }

        rv = shr_idxres_list_free_set(pool, ma_idx_entry_count,
                                     (shr_idxres_element_t *) rx_index,
                                     (shr_idxres_element_t *) &count);
        if (BCM_FAILURE(rv) || (8 != count)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Rx index list free (EP=%d)"
                                   " (count=%d).\n"), h_data_p->ep_id, count));
            soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
            return (rv);
        }
    }

    /* Set MA_BASE_PTR to 0 */
    soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_entry, MA_BASE_PTRf, 0);

    rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL, stm_index,
                                                                &stm_entry);
    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_trunk_port_mdl_update
 * Purpose:
 *     Modify port, egr port and stm table entry
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p - (IN) Pointer to hash data 
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_trunk_port_mdl_update(int unit,
                          _bcm_oam_hash_data_t *h_data_p)
{
    int                  rv = BCM_E_NONE;      /* Operation return status.    */
    _bcm_oam_control_t   *oc;                  /* Pointer to control structure*/
    int                  member_count = 0;
    bcm_port_t           *member_array = NULL; /* Trunk member port array.    */
    int                  local_member_count = 0;
    int                  i = 0;
    bcm_port_t           pp_port = 0;
    bcm_module_t         module_id;           /* Module ID                   */
    uint8                mdl = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if(h_data_p->trunk_id == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    /* Get all the local member ports belonging to this trunk */
    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, h_data_p->trunk_id, 
                               NULL, 0, NULL, &member_count));
    if (0 == member_count) {
        /* No members have been added to the trunk group yet */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: No local members have been added to "
                              "the trunk group yet - %s.\n"), bcm_errmsg(rv)));
        return BCM_E_PARAM;
    }

    _BCM_OAM_ALLOC(member_array, bcm_port_t,
                       sizeof(bcm_port_t) * member_count, "Trunk info");
    if (NULL == member_array) {
        return (BCM_E_MEMORY);
    }

    /* Get members of the trunk belonging to this module */
    if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, 
                                                     h_data_p->trunk_id, 
                                                     member_count, member_array,
                                                     &local_member_count))) {
        if (local_member_count > 0) {
            for(i = 0; i < local_member_count; i++) {
                rv = _bcm_kt2_pp_port_to_modport_get(unit, member_array[i],
                                                  &module_id, &pp_port);
                if (BCM_FAILURE(rv)) {
                    sal_free(member_array);
                    return (rv);
                }
                rv =_bcm_kt2_oam_port_mdl_update(unit, pp_port, 1, 
                                                 h_data_p, &mdl); 
                if (BCM_FAILURE(rv)) {
                    sal_free(member_array);
                    return (rv);
                }
            } 
            if (mdl == 0) { 
                for(i = 0; i < local_member_count; i++) {
                    rv = _bcm_kt2_oam_stm_table_clear(unit, module_id, i,
                                               member_array[i], h_data_p);
                    if (BCM_FAILURE(rv)) {
                        sal_free(member_array);
                        return (rv);
                    }
                }
            }
            rv = _bcm_kt2_oam_port_mdl_passive_update(unit, 1, h_data_p, mdl); 
            if (BCM_FAILURE(rv)) {
                sal_free(member_array);
                return (rv);
            }

        }
    }
    sal_free(member_array);
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_port_domain_mdl_bitmap_update
 * Purpose:
 *     Update Port table MDL bitmap and STM table MA_BASE_PTR. 
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p  - (IN) Pointer to endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_port_domain_mdl_bitmap_update(int unit, _bcm_oam_hash_data_t *h_data_p)
{
    int                  rv = BCM_E_NONE;/* Operation return status.         */
    uint8                mdl;            /* Maintenance domain level.        */
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    bcm_module_t         mod_id = 0;
    bcm_port_t           port_id = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

   /* If the OAM domain is port */
    if (_BCM_OAM_DOMAIN_PORT != h_data_p->oam_domain) {
        return (BCM_E_INTERNAL);
    }

    if(h_data_p->trunk_id != BCM_TRUNK_INVALID) {
        rv = _bcm_kt2_oam_trunk_port_mdl_update(unit, h_data_p);
    } else {
        port_id = _BCM_OAM_GLP_PORT_GET(h_data_p->sglp);
        mod_id  = _BCM_OAM_GLP_MODULE_ID_GET(h_data_p->sglp);
        BCM_IF_ERROR_RETURN
                (_bcm_kt2_oam_port_mdl_update(unit, h_data_p->src_pp_port, 
                                             1, h_data_p, &mdl)); 
        BCM_IF_ERROR_RETURN
                (_bcm_kt2_oam_port_mdl_passive_update(unit, 1, h_data_p, mdl)); 
        if (mdl == 0) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_oam_stm_table_clear(unit, mod_id, 0,
                                               port_id, h_data_p));
        }
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_l3_entry_destroy
 * Purpose:
 *     Update Ingress MP group table(L3 Entry- LMEP view)entry.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p  - (IN) Pointer to endpoint hash data
 *     active_mdl -(IN) Active MDL bitmap value
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_l3_entry_destroy(int unit, _bcm_oam_hash_data_t *h_data_p, 
                              uint8 *active_mdl)
{
    _bcm_oam_control_t *oc;            /* Pointer to OAM control structure. */
    l3_entry_1_entry_t l3_entry;       /* LMEP view table entry.            */
    int                l3_index = -1;  /* L3 table hardware index.          */
    int                rv;             /* Operation return status.          */
    uint8              mdl;            /* Maintenance domain level.         */
    uint8              other_mdl = 0;  /* Maintenance domain level.         */
    uint32             ma_base_index;  /* Endpoint tbl base index. */
    uint32 rx_index[1 << _BCM_OAM_EP_LEVEL_BIT_COUNT] = {0};
                                       /* Endpoint Rx hardware index.       */
    uint32 count;                      /* No. of Rx indices freed           */
                                       /* successfully.                     */
    int                idx;            /* Iterator variable.                */
    soc_field_t        mdl_field = 0;
    soc_field_t        other_mdl_field = 0;
    int                upmep = 0;
    int                ma_idx_entry_count = 0;
    shr_idxres_list_handle_t pool = NULL;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    /* Check whether up/down MEP */
    if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        mdl_field = LMEP__MDL_BITMAP_PASSIVEf;
        other_mdl_field = LMEP__MDL_BITMAP_ACTIVEf;
        upmep = 1;
    } else {
        mdl_field = LMEP__MDL_BITMAP_ACTIVEf;
        other_mdl_field = LMEP__MDL_BITMAP_PASSIVEf;
    }
    rv = _bcm_kt2_oam_find_lmep(unit, h_data_p, &l3_index, &l3_entry);
    if (BCM_SUCCESS(rv)) {
        /* Endpoint found, get MDL bitmap value. */
        mdl = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                                                     mdl_field);

        /* Passive demultiplexing should not be done on a MIP.
         * Hence do not touch PASSIVE bitmap when destroying
         * also. 
         */
        if (!((mdl_field == LMEP__MDL_BITMAP_PASSIVEf) &&
             (_BCM_OAM_EP_IS_MIP(h_data_p)))) {
            /* Clear the MDL bit for this endpoint. */
            mdl &= ~(1 << h_data_p->level);

            if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
                /* Passive MDL bitmap */
                _bcm_kt2_oam_get_passive_mdl_from_active_mdl(unit, *active_mdl, 
                                                h_data_p->ma_base_index, &mdl, 1);
            } else {
                *active_mdl = mdl;
            }
        }
        /* Take L3 module protection mutex to block any updates. */
        L3_LOCK(unit);
        if (0 != mdl) {
            /* Valid endpoints exist for other MDLs at this index. */
            rv = soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm, l3_index,
                                                     mdl_field, mdl);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error:L3 entry LMEP view update(EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                L3_UNLOCK(unit);
                return (rv);
            }
            /* Clear the counter, if any */
            rv = _bcm_kt2_oam_clear_counter(unit, L3_ENTRY_IPV4_UNICASTm, 
                                            l3_index, h_data_p, 
                                            (void *)&l3_entry); 
            if (BCM_FAILURE(rv)) {
                /* coverity[copy_paste_error] */
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Clear counter failed (EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                L3_UNLOCK(unit);
                return (rv);
            }

        } else {
            /* check if other MDL bitmap is also zero (passive bitmap incase 
               of downmep and active bitmap in case of upmep), if so delete the
               entry completely */
            other_mdl = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                                                     other_mdl_field);
            if (0 == other_mdl) {
                rv = soc_mem_delete_index(unit, L3_ENTRY_IPV4_UNICASTm, 
                                          MEM_BLOCK_ALL, l3_index);
                rv =_bcm_kt2_oam_free_counter(unit, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LM counter block "
                                          "free failed (EP=%d) - %s.\n"),
                               h_data_p->ep_id, bcm_errmsg(rv)));
                    L3_UNLOCK(unit);
                    return (rv);
                }
            } else {
                /* Valid endpoints exist for other MDLs at this index. */
                rv = soc_mem_field32_modify(unit, L3_ENTRY_IPV4_UNICASTm, 
                                            l3_index, mdl_field, mdl);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LMEP view update (EP=%d) -"
                                           " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                    L3_UNLOCK(unit);
                    return (rv);
                }
                /* Clear the counter, if any */
                rv = _bcm_kt2_oam_clear_counter(unit, L3_ENTRY_IPV4_UNICASTm, 
                                                l3_index, h_data_p, 
                                                (void *)&l3_entry); 
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Clear counter failed (EP=%d) -"
                                           " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                    L3_UNLOCK(unit);
                    return (rv);
                }
                   
            }
        }
        L3_UNLOCK(unit);

        /* This is the last Rx endpoint in this OAM group. */
        if ((0 == mdl) && (upmep == 0)) {
            ma_base_index = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, 
                                                            &l3_entry,
                                                            LMEP__MA_BASE_PTRf);
            /* Return Rx indices to free pool. */
            for (idx = 0; idx < (1 << _BCM_OAM_EP_LEVEL_BIT_COUNT); idx++) {
                rx_index[idx] = ma_base_index + idx;
            }
            BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit, h_data_p,
                        &ma_idx_entry_count, &pool));
            rv = shr_idxres_list_free_set(pool, ma_idx_entry_count,
                                          (shr_idxres_element_t *) rx_index,
                                          (shr_idxres_element_t *) &count);
            if (BCM_FAILURE(rv) || (8 != count)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Rx index list free (EP=%d)"
                                       " (count=%d).\n"), h_data_p->ep_id, count));
                return (rv);
            }
        }
        /* Delete ING_SERVICE pri map profile entry for this endpoint. */
        if (h_data_p->pri_map_index != _BCM_OAM_INVALID_INDEX) { 
            rv = soc_profile_mem_delete(unit, &oc->ing_service_pri_map,
                                (h_data_p->pri_map_index * BCM_OAM_INTPRI_MAX));
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Profile table update error (idx=%d)"
                                       "- %s.\n"), h_data_p->pri_map_index, bcm_errmsg(rv)));
                return (rv);
            }
            h_data_p->pri_map_index = _BCM_OAM_INVALID_INDEX;
        }
    } else if (BCM_FAILURE(rv) && (oc->init)) {
        /* We do not program passive bitmap for MIP. So dont throw error
         * if you don't find an entry, since we might not have created
         * such an entry at all. */
        if (!((mdl_field == LMEP__MDL_BITMAP_PASSIVEf) &&
                    (_BCM_OAM_EP_IS_MIP(h_data_p)))) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error:  L3 entry table lookup (EP=%d) -"
                                " %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_egr_mp_group_entry_destroy
 * Purpose:
 *     Update Egress MP group table entry.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p  - (IN) Pointer to endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_egr_mp_group_entry_destroy(int unit, 
                                    _bcm_oam_hash_data_t *h_data_p,
                                    uint8 *active_mdl)
{
    _bcm_oam_control_t *oc;            /* Pointer to OAM control structure. */
    uint8              mdl;            /* Maintenance domain level.         */
    uint8              other_mdl = 0;  /* Maintenance domain level.         */
    int                rv;             /* Operation return status.          */
    uint32             ma_base_index;  /* Endpoint tbl base index. */
    uint32 rx_index[1 << _BCM_OAM_EP_LEVEL_BIT_COUNT] = {0};
                                       /* Endpoint Rx hardware index.       */
    uint32 count;                      /* No. of Rx indices freed           */
                                       /* successfully.                     */
    int                idx;            /* Iterator variable.                */
    soc_field_t        mdl_field = 0;
    soc_field_t        other_mdl_field = 0;
    egr_mp_group_entry_t egr_mp_group;  /* Egress MP group tbl entry buffer */
    int                mp_grp_index = 0;
    int                upmep = 0;
#if defined(INCLUDE_BHH)
    _bcm_oam_hash_data_t *hash_data;
    bcm_oam_endpoint_t    ep_id;
    int                   key1 = 0, key2 = 0;
#endif /* INCLUDE_BHH */

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Check whether up/down MEP */
    if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        mdl_field = MDL_BITMAP_ACTIVEf;
        other_mdl_field = MDL_BITMAP_PASSIVEf;
        upmep = 1;
    } else {
        mdl_field = MDL_BITMAP_PASSIVEf;
        other_mdl_field = MDL_BITMAP_ACTIVEf;
    }
    rv = _bcm_kt2_oam_find_egr_lmep(unit, h_data_p, 
                                          &mp_grp_index, &egr_mp_group);
    if (BCM_SUCCESS(rv)) {

#if defined(INCLUDE_BHH)
        if (BHH_EP_TYPE(h_data_p)) {

            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, h_data_p->egress_if)) {
                key1 = h_data_p->egress_if - BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, h_data_p->egress_if)) {
                    key1 = h_data_p->egress_if - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                }
            }

            for (ep_id = _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET;
                 ep_id < (_BCM_OAM_BHH_KT2_ENDPOINT_OFFSET + oc->bhh_endpoint_count);
                 ep_id++) {

                hash_data = &(oc->oam_hash_data[ep_id]);
                if ((!hash_data->in_use) || (hash_data->ep_id == h_data_p->ep_id)) {
                    continue;
                }

                if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, hash_data->egress_if)) {
                    key2 = hash_data->egress_if - BCM_XGS3_EGRESS_IDX_MIN(unit);
                } else {
                    if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, hash_data->egress_if)) {
                        key2 = h_data_p->egress_if -
                                   BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    }
                }

                if (key1 == key2) {
                    /* There are other EPs using the same NHI */
                    return BCM_E_NONE;
                }
            }
        }

#endif /* INCLUDE_BHH */

        /* Endpoint found, get MDL bitmap value. */
        mdl = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_group,
                                                     mdl_field);

        /* Passive demultiplexing should not be done on a MIP.
         * Hence do not touch PASSIVE bitmap when destroying
         * also. 
         */
        if (!((mdl_field == MDL_BITMAP_PASSIVEf) &&
             (_BCM_OAM_EP_IS_MIP(h_data_p)))) {
            /* Clear the MDL bit for this endpoint. */
            mdl &= ~(1 << h_data_p->level);

            if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
                *active_mdl = mdl;
            } else{
                /* Passive MDL bitmap */
                _bcm_kt2_oam_get_passive_mdl_from_active_mdl(unit, *active_mdl,
                                                 h_data_p->ma_base_index, &mdl, 0);
            }
        }

        /* Take L3 module protection mutex to block any updates. */
        soc_mem_lock(unit, EGR_MP_GROUPm);
        if (0 != mdl) {
            /* Valid endpoints exist for other MDLs at this index. */
            rv = soc_mem_field32_modify(unit, EGR_MP_GROUPm, mp_grp_index,
                                                     mdl_field, mdl);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: EGR MP group table update (EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                soc_mem_unlock(unit, EGR_MP_GROUPm);
                return (rv);
            }
            /* Clear the counter, if any */
            rv = _bcm_kt2_oam_clear_counter(unit, EGR_MP_GROUPm, mp_grp_index,
                                            h_data_p, (void *)&egr_mp_group); 
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error:EGR MP group table update(EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                soc_mem_unlock(unit, EGR_MP_GROUPm);
                return (rv);
            }
        } else {
            /* check if other MDL bitmap is also zero (passive bitmap in case 
               of downmep and active bitmap in case of upmep), if so delete the
               entry completely */
            other_mdl = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_group,
                                                     other_mdl_field);
            if (0 == other_mdl) {
                rv = soc_mem_delete_index(unit, EGR_MP_GROUPm, MEM_BLOCK_ALL,
                                          mp_grp_index);
                rv =_bcm_kt2_oam_free_counter(unit, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LM counter block "
                                          "free failed (EP=%d) - %s.\n"),
                               h_data_p->ep_id, bcm_errmsg(rv)));
                    soc_mem_unlock(unit, EGR_MP_GROUPm);
                    return (rv);
                }
            } else {
                rv = soc_mem_field32_modify(unit, EGR_MP_GROUPm, mp_grp_index,
                                                     mdl_field, mdl);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: EGR MP group table update "
                                          "(EP=%d) -" " %s.\n"), 
                               h_data_p->ep_id, bcm_errmsg(rv)));
                    soc_mem_unlock(unit, EGR_MP_GROUPm);
                    return (rv);
                }
                /* Clear the counter, if any */
                rv = _bcm_kt2_oam_clear_counter(unit, EGR_MP_GROUPm, mp_grp_index,
                                               h_data_p, (void *)&egr_mp_group); 
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error:EGR MP group tbl update(EP=%d)-"
                                           " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                    soc_mem_unlock(unit, EGR_MP_GROUPm);
                    return (rv);
                }
            }
        }
        soc_mem_unlock(unit, EGR_MP_GROUPm);

        /* This is the last Rx endpoint in this OAM group. */
        if ((0 == mdl) && (upmep)) {
            ma_base_index = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_group,
                                                        MA_BASE_PTRf);
            /* Return Rx indices to free pool. */
            for (idx = 0; idx < (1 << _BCM_OAM_EP_LEVEL_BIT_COUNT); idx++) {
                rx_index[idx] = ma_base_index + idx;
            }

            rv = shr_idxres_list_free_set(oc->egr_ma_idx_pool, 8,
                                          (shr_idxres_element_t *) rx_index,
                                          (shr_idxres_element_t *) &count);
            if (BCM_FAILURE(rv) || (8 != count)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Egr Rx index list free (EP=%d)"
                                       " (count=%d).\n"), h_data_p->ep_id, count));
                return (rv);
            }
        }
        /* Delete EGR_SERVICE pri map profile entry for this endpoint. */
        if (h_data_p->egr_pri_map_index != _BCM_OAM_INVALID_INDEX) {
            rv = soc_profile_mem_delete(unit, &oc->egr_service_pri_map,
                          (h_data_p->egr_pri_map_index * BCM_OAM_INTPRI_MAX));
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAMError:Profile table update error (idx=%d)"
                                       "- %s.\n"), h_data_p->egr_pri_map_index, bcm_errmsg(rv)));
                return (rv);
            }
            h_data_p->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
        }
    } else if (BCM_FAILURE(rv) && (oc->init)) {
        /* We do not program passive bitmap for MIP. So dont throw error
         * if you don't find an entry, since we might not have created
         * such an entry at all. */
        if (!((mdl_field == MDL_BITMAP_PASSIVEf) &&
                    (_BCM_OAM_EP_IS_MIP(h_data_p)))) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: LMEP table write (EP=%d) -"
                                " %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_local_endpoint_delete
 * Purpose:
 *     Delete a local endpoint.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p  - (IN) Pointer to endpoint hash data
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_local_endpoint_delete(int unit, _bcm_oam_hash_data_t *h_data_p)
{
    _bcm_oam_control_t *oc;            /* Pointer to OAM control structure. */
    lmep_entry_t       lmep_entry;     /* Local endpoint table entry.       */
    int                rv;             /* Operation return status.          */
    lmep_1_entry_t     lmep_1_entry;   /* LMEP_1 table entry.               */
    soc_profile_mem_t  *opcode_control_profile; /* profile used -
                                                        ingress or egress   */
    soc_profile_mem_t  *dglp_profile;  /* dglp profile used -
                                                        ingress or egress   */
    ma_index_entry_t   ma_idx_entry;   /* MA_INDEX table entry buffer.      */
    ma_index_entry_t   egr_ma_idx_entry;/* MA_INDEX table entry buffer.     */
    void               *ma_idx_ptr;
    soc_mem_t          ma_index_mem = 0;
    uint8              active_mdl = 0;
    int                ma_offset = 0;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if (1 == h_data_p->local_tx_enabled) {

        /* Clear endpoint Tx config in hardware. */
        sal_memset(&lmep_entry, 0, sizeof(lmep_entry_t));
        sal_memset(&lmep_1_entry, 0, sizeof(lmep_1_entry_t));
        rv = WRITE_LMEPm(unit, MEM_BLOCK_ALL, h_data_p->local_tx_index,
                         &lmep_entry);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LMEP table write (EP=%d)"
                                   " failed - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }
        rv = WRITE_LMEP_1m(unit, MEM_BLOCK_ALL, h_data_p->local_tx_index,
                         &lmep_1_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LMEP_1 table write (EP=%d)"
                                   " failed - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }

        /* Return ID back to free LMEP ID pool. */
        BCM_IF_ERROR_RETURN
            (shr_idxres_list_free(oc->lmep_pool, h_data_p->local_tx_index));
    }

    if (1 == h_data_p->local_rx_enabled) {
        /* Check whether up/down MEP */
        if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            opcode_control_profile = &oc->egr_oam_opcode_control_profile;
            dglp_profile = &oc->egr_oam_dglp_profile;
            ma_index_mem = EGR_MA_INDEXm;
            sal_memset(&egr_ma_idx_entry, 0, sizeof(egr_ma_index_entry_t));
            ma_idx_ptr = &egr_ma_idx_entry;
        } else {
            opcode_control_profile = &oc->oam_opcode_control_profile;
            dglp_profile = &oc->ing_oam_dglp_profile;
            ma_index_mem = MA_INDEXm;
            sal_memset(&ma_idx_entry, 0, sizeof(ma_index_entry_t));
            ma_idx_ptr = &ma_idx_entry;
        }

        /* Delete OAM opcode profile entry for this endpoint. */
        rv = soc_profile_mem_delete(unit, opcode_control_profile,
                                    h_data_p->profile_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Profile table update error (idx=%d)"
                                   "- %s.\n"), h_data_p->profile_index, bcm_errmsg(rv)));
            return (rv);
        }
        /* Delete DGLP  profile entry for this endpoint. */
        if (h_data_p->dglp1_profile_index != _BCM_OAM_INVALID_INDEX) { 
            rv = soc_profile_mem_delete(unit, dglp_profile,
                                        h_data_p->dglp1_profile_index);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Profile table update "
                                      "error (idx=%d)" "- %s.\n"), 
                           h_data_p->dglp1_profile_index, bcm_errmsg(rv)));
                return (rv);
            }
        }
        if (h_data_p->dglp2_profile_index != _BCM_OAM_INVALID_INDEX) { 
            rv = soc_profile_mem_delete(unit, dglp_profile,
                                        h_data_p->dglp2_profile_index);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Profile table update "
                                      "error (idx=%d)" "- %s.\n"), 
                           h_data_p->dglp2_profile_index, bcm_errmsg(rv)));
                return (rv);
            }
        }
        /* Get MA_INDEX offset  */
        rv = _bcm_kt2_oam_ma_index_offset_get(unit, h_data_p, &ma_offset);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: MA INDEX table offset get failed EP=%d"
                                   "  %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }

        h_data_p->local_rx_index = h_data_p->ma_base_index + ma_offset;

        /* Delete MA INDEX entry */
        rv = _bcm_kt2_oam_ma_index_entry_set(unit, h_data_p, 
                                         ma_index_mem, ma_idx_ptr, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: MA INDEX table delete failed EP=%d"
                                   "  %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }
        if (h_data_p->oam_domain == _BCM_OAM_DOMAIN_PORT) {
            rv = _bcm_port_domain_mdl_bitmap_update(unit, h_data_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: port OAM MDL update failed EP=%d"
                                       "  %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
        } else if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            rv = _bcm_kt2_oam_egr_mp_group_entry_destroy(unit, h_data_p, 
                                                         &active_mdl);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Egr MP Group  update (EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }


            /* Update Ingress MP group table(L3_ENTRY) */
            rv = _bcm_kt2_oam_l3_entry_destroy(unit, h_data_p, &active_mdl);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LMEP view update (EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
        } else {
            /* Update Ingress MP group table(L3_ENTRY) */
            rv = _bcm_kt2_oam_l3_entry_destroy(unit, h_data_p, &active_mdl);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LMEP view update (EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);

            }
            rv = _bcm_kt2_oam_egr_mp_group_entry_destroy(unit, h_data_p,
                                                         &active_mdl);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Egr MP Group  update (EP=%d) -"
                                       " %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
        } 
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_kt2_oam_port_table_key_update1
 * Purpose:
 *     Update OAM_KEY1 and OAM_KEY2 in port table 
 * Parameters:
 *     unit          - (IN) BCM device number
 *     mem           - (IN) PORT_TAB/EGR_PORT
 *     pp_port       - (IN) PP port
 *     h_data_p      - (IN/OUT) Pointer to endpoint hash data 
 * Returns:
 *      BCM_E_XXX
 */
int 
 _bcm_kt2_oam_port_table_key_update1(int unit, soc_mem_t mem, int pp_port,
                                    _bcm_oam_hash_data_t *h_data_p)
{
    _bcm_oam_control_t   *oc;            /* Pointer to OAM control structure. */
    int                  rv = BCM_E_NONE;  /* Operation return status.        */
    port_tab_entry_t     port_entry;
    egr_port_entry_t egr_port_entry;
    uint32               key_type = 0;
    uint32               oam_key1 = 0;
    uint32               oam_key2 = 0;
    void                 *port_entry_ptr;
    uint16               *key1_ref_count = NULL;
    uint16               *key2_ref_count = NULL;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Set the search key type. */
    if (_BCM_OAM_EP_IS_VP_TYPE(h_data_p)) {
        key_type = _BCM_OAM_DOMAIN_VP;
    } else if(h_data_p->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
        key_type = _BCM_OAM_DOMAIN_CVLAN;
    } else if(h_data_p->flags & 
                     BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
        key_type = _BCM_OAM_DOMAIN_S_PLUS_CVLAN;
    } else if((h_data_p->vlan == 0) && (h_data_p->inner_vlan == 0)) {
        key_type = _BCM_OAM_DOMAIN_PORT;
    } else {
        key_type = _BCM_OAM_DOMAIN_SVLAN;
    }

    if (mem == PORT_TABm) {
        port_entry_ptr = &port_entry;
        key1_ref_count = &oc->oam_key1_ref_count[0];
        key2_ref_count = &oc->oam_key2_ref_count[0];
    } else {
        port_entry_ptr = &egr_port_entry;
        key1_ref_count = &oc->egr_oam_key1_ref_count[0];
        key2_ref_count = &oc->egr_oam_key2_ref_count[0];
    }    

    /* 
    We need to set the OAM_KEY1 and OAM_KEY2 fields of the PORT_TABLE and 
    EGR_PORT_TABLE based on the key_type of OAM */

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, pp_port, port_entry_ptr);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Port table read - %s.\n"),
                   bcm_errmsg(rv)));
        return (rv);
    }
    oam_key1 = soc_mem_field32_get(unit, mem, port_entry_ptr, OAM_KEY1f);
    oam_key2 = soc_mem_field32_get(unit, mem, port_entry_ptr, OAM_KEY2f);

    if (key_type != _BCM_OAM_DOMAIN_PORT) {
         if ((oam_key1 > 0) && (oam_key2 > 0)) {
             if ((oam_key1 == key_type) || (oam_key2 == key_type)) {
             } else {
                 LOG_ERROR(BSL_LS_BCM_OAM,
                           (BSL_META_U(unit,
                                       "OAM Error: Invalid OAM domain to resolve "
                                       "(EP=%d) - %s.\n"),
                            h_data_p->ep_id, bcm_errmsg(rv)));
                 return (BCM_E_PARAM);
             }
         }
     }
 
    if (key_type != _BCM_OAM_DOMAIN_PORT) {
        if ((oam_key1 == 0) && (oam_key2 == 0)) {
            soc_mem_field32_set(unit, mem, port_entry_ptr, OAM_KEY1f, key_type);
            key1_ref_count[pp_port]++;  
        } else if ((oam_key1 != key_type) && (oam_key2 == 0)) {
            if (key_type > oam_key1) {
                soc_mem_field32_set(unit, mem, port_entry_ptr, OAM_KEY2f, 
                                                                 key_type);
                key2_ref_count[pp_port]++;  
            } else {
                key2_ref_count[pp_port] = key1_ref_count[pp_port];  
                soc_mem_field32_set(unit, mem, port_entry_ptr, 
                                                      OAM_KEY1f, key_type);
                key1_ref_count[pp_port] = 1;
                soc_mem_field32_set(unit, mem, port_entry_ptr,
                                                      OAM_KEY2f, oam_key1);
            }
        } else if ((oam_key2 != key_type) && (oam_key1 == 0)) {
            if (key_type < oam_key2) {
                soc_mem_field32_set(unit, mem, port_entry_ptr, OAM_KEY1f, 
                                                                 key_type);
                key1_ref_count[pp_port]++;
            } else {
                key1_ref_count[pp_port] = key2_ref_count[pp_port];  
                soc_mem_field32_set(unit, mem, port_entry_ptr, 
                                                      OAM_KEY1f, oam_key2);
                soc_mem_field32_set(unit, mem, port_entry_ptr,
                                                      OAM_KEY2f, key_type);
                key2_ref_count[pp_port] = 1;
            }
        } else if (oam_key1 == key_type) {
            key1_ref_count[pp_port]++;
        } else if (oam_key2 == key_type) {
            key2_ref_count[pp_port]++ ;
        }
        if (SOC_WARM_BOOT(unit)) {
            return rv;
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, pp_port, port_entry_ptr);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Port table update failed  "
                                  "%s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_port_table_key_update
 * Purpose:
 *     Update port table OAM key fields. 
 * Parameters:
 *     unit          - (IN) BCM device number
 *     mem           - (IN) PORT_TAB/EGR_PORT
 *     h_data_p      - (IN/OUT) Pointer to endpoint hash data
 * Returns:
 *      BCM_E_XXX
 */
int 
 _bcm_kt2_oam_port_table_key_update(int unit, soc_mem_t mem, 
                                    _bcm_oam_hash_data_t *h_data_p)
{
    int                  rv = BCM_E_NONE;      /* Operation return status. */
    bcm_port_t           *member_array = NULL; /* Trunk member port array. */
    bcm_module_t         module_id;            /* Module ID                */
    bcm_port_t           pp_port = 0;
    int                  member_count = 0;
    int                  local_member_count = 0;
    int                  i = 0; 

    if ((h_data_p->trunk_id != BCM_TRUNK_INVALID)) {
        /* Get count of ports in this trunk. */
        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, h_data_p->trunk_id, 
                               NULL, 0, NULL, &member_count));
        if (0 == member_count) {
            /* No members have been added to the trunk group yet */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: No local members have been added to "
                                  "the trunk group yet - %s.\n"), bcm_errmsg(rv)));
            return BCM_E_PARAM;
        }

        _BCM_OAM_ALLOC(member_array, bcm_port_t,
                       sizeof(bcm_port_t) * member_count, "Trunk info");
        if (NULL == member_array) {
            return (BCM_E_MEMORY);
        
        }

        /* Get a member of the trunk belonging to this module */
        if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, 
                                        h_data_p->trunk_id, 
                                        member_count, member_array,
                                        &local_member_count))) {
            if (local_member_count > 0) {
                for(i = 0; i < local_member_count; i++) {
                    rv = _bcm_kt2_pp_port_to_modport_get(unit, member_array[i],
                                                         &module_id, &pp_port);
                    if (BCM_FAILURE(rv)) {
                        sal_free(member_array);
                        return (rv);
                    }
                    rv = _bcm_kt2_oam_port_table_key_update1(unit, mem, 
                                                     pp_port, h_data_p);
                    if (BCM_FAILURE(rv)) {
                        sal_free(member_array);
                        return (rv);
                    }
                }

            }
        }
        sal_free(member_array);
    } else {
        if (mem == PORT_TABm) {
            pp_port = h_data_p->src_pp_port;
        } else {
            pp_port = h_data_p->dst_pp_port;
        }
        rv = _bcm_kt2_oam_port_table_key_update1(unit, mem, pp_port, h_data_p);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_decrement_key_ref_count1
 * Purpose:
 *     Decrement PORT tab key1/key2 usage ref count.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     src_pp_port - (IN) Source PP port
 *     dst_pp_port - (IN) destination  PP port
 *     h_data_p    - (IN) Endpoint hash data.
 * Retruns:
 *     BCM_E_XXX
 */
 int 
_bcm_kt2_oam_decrement_key_ref_count1(int unit, uint32 src_pp_port, 
                                      uint32 dst_pp_port,
                                      _bcm_oam_hash_data_t *h_data_p)
{
    _bcm_oam_control_t   *oc;        /* Pointer to OAM control structure. */
    int                  rv = BCM_E_NONE;  /* Operation return status.    */
    port_tab_entry_t     port_entry;
    egr_port_entry_t egr_port_entry;
    uint32               key_type = 0;
    uint32               oam_key1 = 0;
    uint32               oam_key2 = 0;
    int                  update_entry = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Set the search key type. */
    if (_BCM_OAM_EP_IS_VP_TYPE(h_data_p)) {
        key_type = _BCM_OAM_DOMAIN_VP;
    } else if(h_data_p->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
        key_type = _BCM_OAM_DOMAIN_CVLAN;
    } else if(h_data_p->flags & 
                     BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
        key_type = _BCM_OAM_DOMAIN_S_PLUS_CVLAN;
    } else if((h_data_p->vlan == 0) && (h_data_p->inner_vlan == 0)) {
        key_type = _BCM_OAM_DOMAIN_PORT;
    } else {
        key_type = _BCM_OAM_DOMAIN_SVLAN;
    }

    rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, src_pp_port, &port_entry);
    if (BCM_FAILURE(rv)) {
         LOG_ERROR(BSL_LS_BCM_OAM,
                   (BSL_META_U(unit,
                               "OAM Error: Port table entry read failed  - %s.\n"),
                    bcm_errmsg(rv)));
        return (rv);
    }
    rv = soc_mem_read(unit, EGR_PORTm, MEM_BLOCK_ANY, 
                      dst_pp_port, &egr_port_entry);
    if (BCM_FAILURE(rv)) {
         LOG_ERROR(BSL_LS_BCM_OAM,
                   (BSL_META_U(unit,
                               "OAM Error: Port table entry read failed  - %s.\n"),
                    bcm_errmsg(rv)));
        return (rv);
    }

    oam_key1 = soc_PORT_TABm_field32_get(unit, &port_entry, OAM_KEY1f); 
    oam_key2 = soc_PORT_TABm_field32_get(unit, &port_entry, OAM_KEY2f); 
   
    if (key_type != _BCM_OAM_DOMAIN_PORT) {
        if (key_type == oam_key1) {
            oc->oam_key1_ref_count[src_pp_port]--;
            if (oc->oam_key1_ref_count[src_pp_port] == 0) {
                soc_PORT_TABm_field32_set(unit, &port_entry, OAM_KEY1f, 0);
                update_entry = 1;
            }
        } else if (key_type == oam_key2) {
            oc->oam_key2_ref_count[src_pp_port]--;
            if (oc->oam_key2_ref_count[src_pp_port] == 0) {
                soc_PORT_TABm_field32_set(unit, &port_entry, OAM_KEY2f, 0);
                update_entry = 1;
            }
        }
        if (update_entry) {
            rv = soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, 
                               src_pp_port, &port_entry);
            update_entry = 0;
        }

        oam_key1 = soc_EGR_PORTm_field32_get(unit, &egr_port_entry, OAM_KEY1f); 
        oam_key2 = soc_EGR_PORTm_field32_get(unit, &egr_port_entry, OAM_KEY2f); 

        if (key_type == oam_key1) {
            oc->egr_oam_key1_ref_count[dst_pp_port]--;
            if (oc->egr_oam_key1_ref_count[dst_pp_port] == 0) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry, OAM_KEY1f, 0);
                update_entry = 1;
            }
        } else if (key_type == oam_key2) {
            oc->egr_oam_key2_ref_count[dst_pp_port]--;
            if (oc->egr_oam_key2_ref_count[dst_pp_port] == 0) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry, OAM_KEY2f, 0);
                update_entry = 1;
            }
        }
        if (update_entry) {
            rv = soc_mem_write(unit, EGR_PORTm, MEM_BLOCK_ALL, 
                               dst_pp_port, &egr_port_entry);
            update_entry = 0;
        }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_decrement_key_ref_count
 * Purpose:
 *     Decrement PORT tab key1/key2 usage ref count.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     h_data_p - (IN) Endpoint hash data.
 * Retruns:
 *     BCM_E_XXX
 */
 int 
_bcm_kt2_oam_decrement_key_ref_count(int unit, 
                                     _bcm_oam_hash_data_t *h_data_p)
{
    int                  rv = BCM_E_NONE;      /* Operation return status.    */
    bcm_port_t           *member_array = NULL; /* Trunk member port array.    */
    bcm_module_t         module_id;            /* Module ID                   */
    bcm_port_t           src_pp_port = 0;
    int                  member_count = 0;
    int                  local_member_count = 0;
    int                  i = 0; 

    if (h_data_p->trunk_id != BCM_TRUNK_INVALID) {
        /* Get count of ports in this trunk. */
        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, h_data_p->trunk_id, 
                               NULL, 0, NULL, &member_count));
        if (0 == member_count) {
            /* No members have been added to the trunk group yet */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: No local members have been added to "
                                  "the trunk group yet - %s.\n"), bcm_errmsg(rv)));
            return BCM_E_PARAM;
        }

        _BCM_OAM_ALLOC(member_array, bcm_port_t,
                       sizeof(bcm_port_t) * member_count, "Trunk info");
        if (NULL == member_array) {
            return (BCM_E_MEMORY);
        
        }

        /* Get a member of the trunk belonging to this module */
        if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, 
                                        h_data_p->trunk_id, 
                                        member_count, member_array,
                                        &local_member_count))) {
            if (local_member_count > 0) {
                for(i = 0; i < local_member_count; i++) {
                    rv = _bcm_kt2_pp_port_to_modport_get(unit, member_array[i],
                                                     &module_id, &src_pp_port);
                    if (BCM_FAILURE(rv)) {
                        sal_free(member_array);
                        return (rv);
                    }
                    rv = _bcm_kt2_oam_decrement_key_ref_count1(unit, src_pp_port,
                                               h_data_p->dst_pp_port, h_data_p);
                    if (BCM_FAILURE(rv)) {
                        sal_free(member_array);
                        return (rv);
                    }
                }

            }
        }
        sal_free(member_array);
    } else {
        rv = _bcm_kt2_oam_decrement_key_ref_count1(unit, h_data_p->src_pp_port, 
                                              h_data_p->dst_pp_port, h_data_p);
    }
    return rv;
}


     
/*
 * Function: _bcm_kt2_oam_tpid_decrement_ref_count
 *     
 * Purpose:
 *     Decrement TPID reference count.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     h_data_p  - (IN) Endpoint hash data.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int 
_bcm_kt2_oam_tpid_decrement_ref_count(int unit, _bcm_oam_hash_data_t *h_data_p)
{
    int    rv = BCM_E_NONE;
    uint32 tag_type = 0;

    /* If local CCM tx generation was not enabled, then the tpid entries
     * would not have been set. So do not try to delete them even though the
     * h_data_p->outer/inner/subport_tpid has a non zero value */
    if (!h_data_p->local_tx_enabled) {
        return BCM_E_NONE;
    }

    if (_BCM_OAM_EP_IS_VP_TYPE(h_data_p)) {
        if(h_data_p->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
            tag_type = _BCM_OAM_DOMAIN_S_PLUS_CVLAN;
        } else if (h_data_p->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
            tag_type =_BCM_OAM_DOMAIN_CVLAN;
        } else if (h_data_p->vlan > 0) {
            tag_type =_BCM_OAM_DOMAIN_SVLAN;
        }
    } else {
        tag_type =h_data_p->oam_domain;
    }

    /* if S or S+C vlan - set outer tpid index */
    if (((tag_type == _BCM_OAM_DOMAIN_SVLAN) || 
        (tag_type == _BCM_OAM_DOMAIN_S_PLUS_CVLAN)) &&
        (h_data_p->outer_tpid > 0)) { 
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_kt2_tpid_entry_delete(unit,
                                           h_data_p->outer_tpid_profile_index,
                                           BCM_OAM_TPID_TYPE_OUTER);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: L3 entry config failed in Outer TPID "
                                      "decrement ref count""  %s.\n"), bcm_errmsg(rv)));
                return (rv);
            }
        }
    }
    if (((tag_type == _BCM_OAM_DOMAIN_CVLAN) || 
        (tag_type == _BCM_OAM_DOMAIN_S_PLUS_CVLAN)) &&
        (h_data_p->inner_tpid > 0)) { 
        /* C or S+C */
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_kt2_tpid_entry_delete(unit,
                                           h_data_p->inner_tpid_profile_index,
                                           BCM_OAM_TPID_TYPE_INNER);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: L3 entry config failed in Inner TPID "
                                      "decrement ref count""  %s.\n"), bcm_errmsg(rv)));
                return (rv);
            }
        }
    }
    if ((BCM_GPORT_IS_SUBPORT_PORT(h_data_p->gport)) &&
              (_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, h_data_p->gport)) && 
              (h_data_p->subport_tpid > 0)) {
        /* CoE port */
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_kt2_tpid_entry_delete(unit,
                                            h_data_p->subport_tpid_profile_index,
                                            BCM_OAM_TPID_TYPE_SUBPORT);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: L3 entry config failed in Subport TPID  "
                                      "decrement ref count""  %s.\n"), bcm_errmsg(rv)));
                return (rv);
            }
        }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_endpoint_destroy
 * Purpose:
 *     Delete an endpoint and free all its allocated resources.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     ep_id  - (IN) Endpoint ID value.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_endpoint_destroy(int unit,
                              bcm_oam_endpoint_t ep_id)
{
    _bcm_oam_control_t      *oc;        /* Pointer to OAM control structure. */
    _bcm_oam_hash_data_t    *h_data_p;  /* Pointer to endpoint data.         */
    _bcm_oam_hash_key_t     hash_key;   /* Hash key buffer for lookup.       */
    bcm_oam_endpoint_info_t ep_info;    /* Endpoint information.             */
    _bcm_oam_hash_data_t    h_data;     /* Pointer to endpoint data.         */
    int                     rv;         /* Operation return status.          */
#if defined(INCLUDE_BHH)
    uint16 reply_len;
    bcm_oam_endpoint_t bhh_pool_ep_id;
#endif

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get the hash data pointer. */
    h_data_p = &oc->oam_hash_data[ep_id];
    if(0 == h_data_p->in_use){
        return BCM_E_NOT_FOUND; 
    } 

    rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_delete(unit, h_data_p);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Remove from ma_idx to ep id map (EP=%d) -"
                            " %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
        return (rv);
    }

    if(bcmOAMEndpointTypeEthernet == h_data_p->type)
    {

    if (h_data_p->flags & BCM_OAM_ENDPOINT_REMOTE) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oam_remote_endpoint_delete(unit, h_data_p));

    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oam_local_endpoint_delete(unit, h_data_p));
        /* decrement TPID ref count */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oam_tpid_decrement_ref_count(unit, h_data_p));
    }

    /* Remove endpoint for group's endpoint list. */
    rv = _bcm_kt2_oam_group_ep_list_remove(unit, h_data_p->group_index,
                                       h_data_p->ep_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Remove from group list (EP=%d) -"
                               " %s.\n"), ep_id, bcm_errmsg(rv)));
        return (rv);
    }


    /* Return ID back to free MEP ID pool.*/
    BCM_IF_ERROR_RETURN(shr_idxres_list_free(oc->mep_pool, ep_id));

    /* Decrement PORT tab key1/key2 usage ref count */
    if (!(h_data_p->flags & BCM_OAM_ENDPOINT_REMOTE)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oam_decrement_key_ref_count(unit, h_data_p));
    }
    /* Initialize endpoint info structure. */
    bcm_oam_endpoint_info_t_init(&ep_info);

    /* Set up endpoint information for key construction. */
    ep_info.group = h_data_p->group_index;
    ep_info.name = h_data_p->name;
    ep_info.gport = h_data_p->gport;
    ep_info.level = h_data_p->level;
    ep_info.vlan = h_data_p->vlan;
    ep_info.inner_vlan = h_data_p->inner_vlan;
    ep_info.flags = h_data_p->flags;

    /* Construct hash key for lookup + delete operation. */
    _bcm_kt2_oam_ep_hash_key_construct(unit, oc, &ep_info, &hash_key);

    /* Remove entry from hash table. */
    BCM_IF_ERROR_RETURN(shr_htb_find(oc->ma_mep_htbl, hash_key,
                                     (shr_htb_data_t *)&h_data,
                                     1));

    /* Clear the hash data memory previously occupied by this endpoint. */
    _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
    }
    /*
     * BHH specific
     */
    else if (soc_feature(unit, soc_feature_bhh)
            && BHH_EP_TYPE(h_data_p)) {
#if defined(INCLUDE_BHH)

        if (h_data_p->is_remote) {
            /*
            * BHH uses same index for local and remote.  So, delete always goes through
            * local endpoint destory
            */
            return (BCM_E_NONE);
        } else {

            bhh_pool_ep_id = BCM_OAM_BHH_GET_UKERNEL_EP(ep_id);

            /* Delete BHH Session in HW. BCM_E_NOT_FOUND is not an error when
               switch is re-initializing as entry would have been already 
               deleted by respective module init. */ 
            rv =  _bcm_kt2_oam_bhh_session_hw_delete(unit, h_data_p);
            if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: bhh_session_hw_delete failled " 
                                       "(EP=%d) - %s.\n"), ep_id, bcm_errmsg(rv)));
                return (rv);
            }

            if (!(h_data_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU)) {
                /* Send BHH Session Delete message to uC. Error response is not an  
                   during switch re-init as uKernel is reloaded and its state is 
                   cleared before oam init. As such endpoint wont exist during oam init*/

                _bcm_kt2_oam_bhh_msg_send_receive(unit,
                        MOS_MSG_SUBCLASS_BHH_SESS_DELETE,
                        (int)bhh_pool_ep_id, 0,
                        MOS_MSG_SUBCLASS_BHH_SESS_DELETE_REPLY,
                        &reply_len);
            }
            if (!BHH_EP_MPLS_SECTION_TYPE(h_data_p)) {
                rv = _bcm_kt2_oam_local_endpoint_delete(unit, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error:BHH local_endpoint_delete failled "
                                           "(EP=%d) - %s.\n"), ep_id, bcm_errmsg(rv)));
                    return (rv);
                }
            }
            h_data_p->in_use = 0;
        

             /* Remove endpoint for group's endpoint list. */
            rv = _bcm_kt2_oam_group_ep_list_remove(unit, h_data_p->group_index,
                                       h_data_p->ep_id);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Remove from group list (EP=%d) -"
                                       " %s.\n"), ep_id, bcm_errmsg(rv)));
                return (rv);
            }


            /* Return ID back to free MEP ID pool.*/
            BCM_IF_ERROR_RETURN(shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_id));

            /* Set up endpoint information for key construction. */
            ep_info.group = h_data_p->group_index;
            ep_info.name = h_data_p->name;
            ep_info.gport = h_data_p->gport;
            ep_info.level = h_data_p->level;
            ep_info.vlan = h_data_p->vlan;
            ep_info.inner_vlan = h_data_p->inner_vlan;
            ep_info.flags = h_data_p->flags;

            /* Construct hash key for lookup + delete operation. */
            _bcm_kt2_oam_ep_hash_key_construct(unit, oc, &ep_info, &hash_key);

            /* Remove entry from hash table. */
            BCM_IF_ERROR_RETURN(shr_htb_find(oc->ma_mep_htbl, hash_key,
                                             (shr_htb_data_t *)&h_data,
                                             1));

            /* Clear the hash data memory previously occupied by this endpoint. */
            _BCM_OAM_HASH_DATA_CLEAR(h_data_p);

        }
#else
        return (BCM_E_UNAVAIL);
#endif /* INCLUDE_BHH */
    }
    else {
        return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_group_endpoints_destroy
 * Purpose:
 *     Delete all endpoints associated with a group and free all
 *     resources allocated by these endpoints. 
 * Parameters:
 *     unit      - (IN) BCM device number
 *     g_info_p  - (IN) Pointer to group information
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_group_endpoints_destroy(int unit,
                                     _bcm_oam_group_data_t *g_info_p)
{
    bcm_oam_endpoint_t    ep_id; /* Endpoint ID.                   */
    _bcm_oam_ep_list_t    *cur;  /* Pointer to endpoint list node. */
    int                   rv;    /* Operation return status.       */

    if (NULL == g_info_p) {
        return (BCM_E_INTERNAL);
    }

    /* Get the endpoint list head pointer. */
    cur = *(g_info_p->ep_list);
    if (NULL == cur) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: No endpoints in group.\n")));
        return (BCM_E_NONE);
    }

    while (NULL != cur) {
        ep_id = cur->ep_data_p->ep_id;

        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: GID=%d EP:%d.\n"),
                   cur->ep_data_p->group_index, ep_id));

        cur = cur->next;

        rv = _bcm_kt2_oam_endpoint_destroy(unit, ep_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint destroy (EP=%d) - "
                                   "%s.\n"), ep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_trunk_resolve
 * Purpose:
 *     Resolve a trunk value to SGLP, DGLP value and Tx gport.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     trunk_id    - (IN) Pointer to trunk id
 *     trunk_index - (IN) Index in the trunk array to Tx pkts
 *     src_glp     - (IN/OUT) Pointer to source generic logical port value.
 *     dst_glp     - (IN/OUT) Pointer to destination generic logical port value.
 *     src_pp_port - (IN/OUT) Pointer to source pp port value.
 *     dst_pp_port - (IN/OUT) Pointer to destination pp port value.
 *     tx_gport    - (OUT) Gport on which PDUs need to be sent
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_trunk_resolve(int unit, bcm_trunk_t trunk_id, int trunk_index,
                           bcm_port_t *src_pp_port, bcm_port_t *dst_pp_port,
                           uint32 *src_glp, uint32 *dst_glp, bcm_gport_t *tx_gport)
{
    bcm_port_t          port_id = 0;
    bcm_port_t          port = 0;
    bcm_module_t        module_id = 0;
    int                 local_id;
    bcm_trunk_member_t *member_array = NULL;
    int                 member_count = 0;
    int                 local_member_count = 0;
    bcm_trunk_info_t    trunk_info;
    bcm_trunk_t         tid = BCM_TRUNK_INVALID;
    int                 rv = BCM_E_NONE;


    if (BCM_TRUNK_INVALID == trunk_id)  {
        /* Has to be a valid Trunk. */
        return (BCM_E_PARAM);
    }

    /* Construct Hw SGLP value. */
    _BCM_KT2_OAM_MOD_PORT_TO_GLP(unit, module_id, port_id, 1,
                                 trunk_id, *src_glp);

    /* Get a member of the trunk belonging to this module */
    if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, trunk_id, 1,
                                                     &port_id,
                                                     &local_member_count))) {

        BCM_IF_ERROR_RETURN(_bcm_kt2_pp_port_to_modport_get(unit, port_id,
                                                            &module_id, &port));
        *src_pp_port = port_id;
    }

    /* Get count of ports in this trunk. */
    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, trunk_id, NULL, 0,
                                          NULL, &member_count));
    if (0 == member_count) {
        /* No members have been added to the trunk group yet */
        return BCM_E_PARAM;
    }

    _BCM_OAM_ALLOC(member_array, bcm_trunk_member_t,
                   sizeof(bcm_trunk_member_t) * member_count, "Trunk info");
    if (NULL == member_array) {
        return (BCM_E_MEMORY);
    }

    /* Get Trunk Info for the Trunk ID. */
    rv = bcm_esw_trunk_get(unit, trunk_id, &trunk_info,
                           member_count, member_array, &member_count);
    if (BCM_FAILURE(rv)) {
        sal_free(member_array);
        return (rv);
    }

    /* Check if the input trunk_index is valid. */
    if (trunk_index >= member_count) {
        sal_free(member_array);
        return BCM_E_PARAM;
    }


    /* Get the Modid and Port value using Trunk Index value. */
    rv = _bcm_esw_gport_resolve(unit, member_array[trunk_index].gport,
                                &module_id, &port_id, &tid, &local_id);
    if (BCM_FAILURE(rv)) {
        sal_free(member_array);
        return (rv);
    }
    sal_free(member_array);

    /* Construct Hw DGLP value. */
    _BCM_KT2_OAM_MOD_PORT_TO_GLP(unit, module_id, port_id, 0, -1, *dst_glp);

    /* Get HW PP port */
    BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(unit, module_id,
                                                        port_id, dst_pp_port));
    *dst_glp = *src_glp;
    *tx_gport = member_array[trunk_index].gport;

    return BCM_E_NONE;

}
/*
 * Function:
 *     _bcm_kt2_oam_endpoint_gport_resolve
 * Purpose:
 *     Resolve an endpoint GPORT value to SGLP and DGLP value.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     ep_info_p  - (IN/OUT) Pointer to endpoint information.
 *     src_glp    - (IN/OUT) Pointer to source generic logical port value.
 *     dst_glp    - (IN/OUT) Pointer to destination generic logical port value.
 *     src_pp_port- (IN/OUT) Pointer to source pp port value.
 *     dst_pp_port- (IN/OUT) Pointer to destination pp port value.
 *     svp        - (IN/OUT) Pointer to VP value
 *     trunk_id   - (IN/OUT) Pointer to trunk id
 *     tx_gport   - (OUT) Gport on which PDUs need to be sent
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_endpoint_gport_resolve(int unit, bcm_oam_endpoint_info_t *ep_info_p,
                                    uint32 *src_glp, uint32 *dst_glp,
                                    bcm_port_t *src_pp_port,
                                    bcm_port_t *dst_pp_port, uint32 *svp,
                                    bcm_trunk_t *trunk_id, int *is_vp_valid,
                                    bcm_gport_t *tx_gport)
{
    bcm_module_t       module_id;            /* Module ID           */
    bcm_port_t         port_id;              /* Port ID.            */
    bcm_port_t         port;                 /* Port ID.            */
    int                local_id;             /* Hardware ID.        */
    int                tx_enabled = 0;       /* CCM Tx enabled.     */
    int                local_member_count = 0;
    int                rv = BCM_E_NONE;                   /* Return status.      */
    uint8              glp_valid = 0;        /* Logical port valid. */
    int                is_local = 0;
    uint8              is_port_mep = 0;

    is_port_mep = ((ep_info_p->vlan == 0) && (ep_info_p->inner_vlan == 0));
    /* Get Trunk ID or (Modid + Port) value from Gport */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, ep_info_p->gport, &module_id,
                                &port_id, trunk_id, &local_id));
 
    /* Set CCM endpoint Tx status only for local endpoints. */
    if (!(ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE)) {
        tx_enabled
            = (ep_info_p->ccm_period
                != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) ? 1 : 0;
    }

    /*
     * If Gport is Trunk type, _bcm_esw_gport_resolve()
     * sets trunk_id. Using Trunk ID, get Dst Modid and Port value.
     */
    if (BCM_GPORT_IS_TRUNK(ep_info_p->gport)) {

        /*
         * CCM Tx is enabled on a trunk member port.
         * trunk_index value is required to derive the Modid and Port info.
         */
        if (1 == tx_enabled && _BCM_OAM_INVALID_INDEX == ep_info_p->trunk_index) {
            /* Invalid Trunk member index passed. */
            return (BCM_E_PORT);
        }

        BCM_IF_ERROR_RETURN(
              _bcm_kt2_oam_trunk_resolve(unit, *trunk_id, ep_info_p->trunk_index,
                                         src_pp_port, dst_pp_port,
                                         src_glp, dst_glp, tx_gport));
        glp_valid = 1;
    }

    /*
     * Application can resolve the trunk and pass the desginated
     * port as Gport value. Check if the Gport belongs to a trunk.
     */
    if ((BCM_TRUNK_INVALID == (*trunk_id))
        && (BCM_GPORT_IS_MODPORT(ep_info_p->gport)
        || BCM_GPORT_IS_LOCAL(ep_info_p->gport))) {

        /* When Gport is ModPort or Port type, _bcm_esw_gport_resolve()
         * returns Modid and Port value. Use these values to make the DGLP
         * value.
         */
        _BCM_KT2_OAM_MOD_PORT_TO_GLP(unit, module_id, port_id, 0, -1,
            *dst_glp);
        /* get destination PP port */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_modport_to_pp_port_get(unit, module_id, port_id,
                                             dst_pp_port));

        /* Use the Modid, Port value and determine if the port
         * belongs to a Trunk.
         */
        if (!is_port_mep) {
            /* Dont find all other trunk members if it is a port mep */
            rv = bcm_esw_trunk_find(unit, module_id, port_id, trunk_id);
        }
        if (BCM_SUCCESS(rv) && !is_port_mep) {
            /*
             * Port is member of a valid trunk.
             * Now create the SGLP value from Trunk ID.
             */
            /* Get a member of the trunk belonging to this module */
            if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit, *trunk_id, 1,
                                        &port_id,
                                        &local_member_count))) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_pp_port_to_modport_get(unit, port_id,
                                                      &module_id, &port));
                /* Get HW PP port */
                *src_pp_port = port_id;

                _BCM_KT2_OAM_MOD_PORT_TO_GLP(unit, module_id, port, 1, 
                                             *trunk_id, *src_glp);
                *dst_glp = *src_glp;
            } else {
                *src_pp_port = *dst_pp_port; 
            }
        } else {
            /* Port not a member of trunk. DGLP and SGLP are the same. */
            *src_glp = *dst_glp;
            *src_pp_port = *dst_pp_port; 
        }
        glp_valid = 1;
        *tx_gport = ep_info_p->gport;
    }
    if ((SOC_GPORT_IS_MIM_PORT(ep_info_p->gport)) ||
        (SOC_GPORT_IS_MPLS_PORT(ep_info_p->gport)) ||
        (SOC_GPORT_IS_VLAN_PORT(ep_info_p->gport))) {
        *svp = local_id;

        if(BCM_TRUNK_INVALID == (*trunk_id)){
            rv = _bcm_esw_modid_is_local(unit, module_id, &is_local);
            if(BCM_SUCCESS(rv) && (is_local)) {
                _BCM_KT2_OAM_MOD_PORT_TO_GLP(unit, module_id, port_id, 0,
                                             -1, *dst_glp);
                /* get destination PP port */
                BCM_IF_ERROR_RETURN(
                              _bcm_kt2_modport_to_pp_port_get(unit,
                                                              module_id, port_id,
                                                              dst_pp_port));
                *src_glp     = *dst_glp;
                *src_pp_port = *dst_pp_port;
            }
            *tx_gport = ep_info_p->gport;
        } else {
            /*
             * CCM Tx is enabled on a trunk member port.
             * trunk_index value is required to derive the Modid and Port info.
             */
            if ((1 == tx_enabled) &&
                            (_BCM_OAM_INVALID_INDEX == ep_info_p->trunk_index)) {
                /* Invalid Trunk member index passed. */
                return (BCM_E_PORT);
            }
            BCM_IF_ERROR_RETURN(
               _bcm_kt2_oam_trunk_resolve(unit, *trunk_id, ep_info_p->trunk_index,
                                          src_pp_port, dst_pp_port,
                                          src_glp, dst_glp, tx_gport));
        }
        glp_valid    = 1;
        *is_vp_valid = 1;
    }

    /* LinkPhy/CoE case */
    if ((BCM_GPORT_IS_SUBPORT_PORT(ep_info_p->gport))) {
        if ((_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, ep_info_p->gport)) ||
            (_BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(unit, ep_info_p->gport))) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_modport_to_pp_port_get(unit, module_id, port_id,
                                                dst_pp_port));
            *src_pp_port = *dst_pp_port; 
            _BCM_KT2_OAM_MOD_PORT_TO_GLP(unit, module_id, port_id, 0, -1,
                                             *dst_glp);
                /* Use the Modid, Port value and determine if the port
                 * belongs to a Trunk.
                 */
            if (!is_port_mep) {
                rv = bcm_esw_trunk_find(unit, module_id, port_id, trunk_id);
            }
            if (BCM_SUCCESS(rv) && !is_port_mep) {
                /*
                 * Port is member of a valid trunk.
                 * Now create the SGLP value from Trunk ID.
                 */
                /* Get a member of the trunk belonging to this module */
                if (BCM_SUCCESS(_bcm_esw_trunk_local_members_get(unit,  
                                        *trunk_id, 1,
                                        &port_id,
                                        &local_member_count))) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt2_pp_port_to_modport_get(unit, port_id,
                                                      &module_id, &port));
                   /* Get HW PP port */
                    *src_pp_port = port_id;

                   _BCM_KT2_OAM_MOD_PORT_TO_GLP(unit, module_id, port, 1, 
                                                *trunk_id, *src_glp);
                   *dst_glp = *src_glp;
                }
            } else {
                *src_glp = *dst_glp;
            }
            glp_valid = 1;
            BCM_GPORT_MODPORT_SET(*tx_gport, module_id, port_id);
        } 
    }

    /*
     * At this point, both src_glp and dst_glp should be valid.
     * Gport types other than TRUNK, MODPORT or LOCAL are not valid.
     */
    if (0 == glp_valid) {
        return (BCM_E_PORT);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_mepid_validate
 * Purpose:
 *     Validate an endpoint MEP id.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     ep_info_p - (IN) Pointer to endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_mepid_validate(int unit,  
                            bcm_oam_endpoint_info_t *ep_info_p)
{
    _bcm_oam_control_t *oc;
    _bcm_oam_group_data_t *group_p;   /* Pointer to group list.         */
    _bcm_oam_ep_list_t    *cur;     /* Current head node pointer.       */
    _bcm_oam_hash_data_t  *h_data_p = NULL; /* Endpoint hash data pointer. */

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get the group memory pointer. */
    group_p = &oc->group_info[ep_info_p->group];

    if (1 != group_p->in_use) {
        return BCM_E_NONE;
    }    
    cur =  *group_p->ep_list;

    /* Traverse the list and delete the matching node. */
    while (NULL != cur) {
        h_data_p = cur->ep_data_p;
        if (NULL == h_data_p) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "OAM Error: Hash data is empty\n")));
            return (BCM_E_INTERNAL);
        }
        if (ep_info_p->name  == h_data_p->name) { 
            if (((ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE) && 
                        (cur->ep_data_p->ep_id == ep_info_p->id)) || 
                    ((h_data_p->flags & BCM_OAM_ENDPOINT_REMOTE) != 
                     (ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE))) {
            } else {
                return BCM_E_PARAM;
            }    
        }
        cur = cur->next;
    }    
    return (BCM_E_NONE);
}        

/*
 * Function:
 *     _bcm_kt2_oam_endpoint_params_validate
 * Purpose:
 *     Validate an endpoint parameters.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     oc        - (IN) Pointer to OAM control structure.
 *     hash_key  - (IN) Pointer to endpoint hash key value.
 *     ep_info_p - (IN) Pointer to endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_endpoint_params_validate(int unit,
                                  _bcm_oam_control_t *oc,
                                  _bcm_oam_hash_key_t *hash_key,
                                  bcm_oam_endpoint_info_t *ep_info_p)
{
    int                 rv;  /* Operation return status. */
    _bcm_oam_hash_data_t h_stored_data;
    int remote  = 0;


    /* Get MEP remote endpoint status. */
    remote = (ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE) ? 1 : 0;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: "
                          "_bcm_kt2_oam_endpoint_params_validate.\n")));

    /* Endpoint must be 802.1ag/Ethernet OAM type. */
    if ((bcmOAMEndpointTypeEthernet != ep_info_p->type) 
#if defined(INCLUDE_BHH)
        &&  /* BHH/Y.1731 OAM type */
        (!BHH_EP_TYPE(ep_info_p))
#endif /* INCLUDE_BHH */
        )
    {
        /* Other OAM types are not supported, return error. */
        return BCM_E_UNAVAIL;
    }

    /* Supported MDL level is 0 - 7. */
    if ((!remote) &&
        ((ep_info_p->level < 0) || (ep_info_p->level > _BCM_OAM_EP_LEVEL_MAX))) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: EP Level should be in the range(0-%d).\n"),
                   _BCM_OAM_EP_LEVEL_MAX));
        return (BCM_E_PARAM);
     }

     /* Supported MEPID EP Name range is 1 - 8191, Skipping range check for MIP */
     if (((ep_info_p->name < _BCM_OAM_EP_NAME_MIN)
         || (ep_info_p->name > _BCM_OAM_EP_NAME_MAX)) &&
         !(ep_info_p->flags & BCM_OAM_ENDPOINT_INTERMEDIATE)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MEP Name should be in the range(%d-%d).\n"),
                   _BCM_OAM_EP_NAME_MIN, _BCM_OAM_EP_NAME_MAX));
        return (BCM_E_PARAM);
     }

    /*
     * Check and return error if invalid flag bits are set for remote
     * endpoint.
     */
    if ((ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE)
        && (ep_info_p->flags & _BCM_OAM_REMOTE_EP_INVALID_FLAGS_MASK)) {

        return (BCM_E_PARAM);

    }

    /*
     * Check and return error if MEPID is not unique within the group
     */
    if(_bcm_kt2_oam_mepid_validate(unit, ep_info_p)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MEPID:%x passed is not unique in group %x\n"),
                   ep_info_p->name, ep_info_p->group));

        return (BCM_E_EXISTS);
    }

    /* For replace operation, endpoint ID is required. */
    if ((ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)
        && !(ep_info_p->flags & BCM_OAM_ENDPOINT_WITH_ID)) {

        return (BCM_E_PARAM);

    }
    /* Port based MEP is supported only for down MEP */
    if ((0 == ep_info_p->vlan) && (0 == ep_info_p->inner_vlan) &&
        (ep_info_p->flags & BCM_OAM_ENDPOINT_UP_FACING) && (!remote)) {
        return (BCM_E_PARAM);
    }

    /* If it is a Port + CVlan based MEP and the
     * CVlan (inner_vlan) is 0, return E_PARAM
     */
    if ((0 == ep_info_p->inner_vlan) &&
        (ep_info_p->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN)) {
        return (BCM_E_PARAM);
    }

    /* If it is a Port + S + CVlan based MEP and one of SVlan(vlan)
     * or CVlan (inner_vlan) is 0, return E_PARAM
     */
    if (((0 == ep_info_p->inner_vlan) || (0 == ep_info_p->vlan)) &&
        (ep_info_p->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN)) {
        return (BCM_E_PARAM);
    }

    /* Validate endpoint index value. */
    if (ep_info_p->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        _BCM_OAM_EP_INDEX_VALIDATE(ep_info_p->id);
    }

    /* MIP's are not created with non-zero ccm period */
    if ((ep_info_p->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) &&
        (ep_info_p->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)) {
         return (BCM_E_PARAM);
    }

    /* Validate endpoint group id. */
    _BCM_OAM_GROUP_INDEX_VALIDATE(ep_info_p->group);

    rv = shr_idxres_list_elem_state(oc->group_pool, ep_info_p->group);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Group (GID:%d) does not exist.\n"),
                   ep_info_p->group));
        return (BCM_E_PARAM);
    }
  
    if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        /*
         * BHH can have multiple LSPs/endpoint on same port/MDL
         * BHH does not have h/w support.  So, return here
         */
        if ((bcmOAMEndpointTypeBHHMPLS == ep_info_p->type) ||
            (bcmOAMEndpointTypeBHHMPLSVccv == ep_info_p->type)) {
            return (BCM_E_NONE);
        }
#endif
    }

#if defined(KEY_PRINT)
    _bcm_oam_hash_key_print(hash_key);
#endif

    /*
     * Lookup using hash key value.
     * Last param value '0' specifies keep the match entry.
     * Value '1' would mean remove the entry from the table.
     * Matched Params:
     *      Group Name + Group ID + Endpoint Name + VLAN + MDL + Gport.
     */
    rv = shr_htb_find(oc->ma_mep_htbl, *hash_key,
                      (shr_htb_data_t *)&h_stored_data, 0);
    if (BCM_SUCCESS(rv)
        && !(ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)) {

        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint ID=%d %s.\n"),
                   ep_info_p->id, bcm_errmsg(BCM_E_EXISTS)));

        /* Endpoint must not be in use expect for Replace operation. */
        return (BCM_E_EXISTS);

    } else {

        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: Endpoint ID=%d Available. %s.\n"),
                   ep_info_p->id, bcm_errmsg(rv)));

    }

    

    return (BCM_E_NONE);
}

typedef struct _bcm_oam_flexible_oam_domain_vlan_ctrl_type1_s {
    uint32            start_index;
    uint32            end_index;
    soc_field_t       tag_source;
    soc_field_t       data_processing;
    soc_field_t       oam_processing;
} _bcm_oam_flexible_oam_domain_c_vlan_ctrl_type1_t;

typedef struct _bcm_oam_flexible_oam_domain_vlan_ctrl_type2_s {
    uint32            start_index;
    uint32            no_entries;
    uint32            increment;
    soc_field_t       tag_source_1;
    soc_field_t       tag_source_2;
    soc_field_t       data_processing;
    soc_field_t       oam_processing;
} _bcm_oam_flexible_oam_domain_vlan_ctrl_type2_t;

/*
 * Function:
 *     _bcm_kt2_oam_ing_flexible_oam_domain_ctrl_set
 * Purpose:
 *    Set the default values for ingress flexible oam domain control 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 * Index to the ing flexible domain control table is
 *        index[5] = pars_inner_pri_tagged;
 *        index[4] = pars_inner_tagged;
 *        index[3] = pars_outer_tagged;
 *        index[2] = vxlt_inner_tagged;
 *        index[1] = vxlt_outer_tagged;
 *        index[0] = svp_valid;
 */
STATIC int
_bcm_kt2_oam_ing_flexible_oam_domain_ctrl_set(int unit)
{
    int      rv = BCM_E_NONE;
    ing_oam_flexible_domain_control_entry_t *ent_buf;
    ing_oam_flexible_domain_control_entry_t *ent;
    int      max_index = 0; 
    int      index = 0; 
    int      end_index = 0; 
    int      loop_index = 0; 
    int      entry_mem_size = 0;
    int      entry_index = 0;
    uint32   action_set = 1;


    _bcm_oam_flexible_oam_domain_c_vlan_ctrl_type1_t c_vlan_ctrl_info[] = {
      /* Set values for C VLAN MEP pre-VXLT actions */
           /* Action for incoming untagged packets  */
           { 4,   7, CVLAN_DOMAIN_PRE_VXLT_CVLAN_TAG_SOURCEf, 
                     CVLAN_DOMAIN_PRE_VXLT_DATA_PROCESSING_ENABLEf, 
                     CVLAN_DOMAIN_PRE_VXLT_OAM_PROCESSING_ENABLEf },
           /* Action for incoming single inner tagged but not 
             priority tagged packets */
           { 16, 23, CVLAN_DOMAIN_PRE_VXLT_CVLAN_TAG_SOURCEf, 
                     CVLAN_DOMAIN_PRE_VXLT_DATA_PROCESSING_ENABLEf, 
                     CVLAN_DOMAIN_PRE_VXLT_OAM_PROCESSING_ENABLEf },
           /* Action for incoming single inner priority tagged packets */
           { 52, 55, CVLAN_DOMAIN_PRE_VXLT_CVLAN_TAG_SOURCEf, 
                     CVLAN_DOMAIN_PRE_VXLT_DATA_PROCESSING_ENABLEf, 
                     CVLAN_DOMAIN_PRE_VXLT_OAM_PROCESSING_ENABLEf },
       /* Set values for C VLAN MEP post-VXLT actions */
           /* Action for incoming untagged packets */
           { 4,   7, CVLAN_DOMAIN_POST_VXLT_CVLAN_TAG_SOURCEf, 
                     CVLAN_DOMAIN_POST_VXLT_DATA_PROCESSING_ENABLEf, 
                     CVLAN_DOMAIN_POST_VXLT_OAM_PROCESSING_ENABLEf },
           /* Action for incoming single inner tagged but not 
             priority tagged packets */
           { 20, 23, CVLAN_DOMAIN_POST_VXLT_CVLAN_TAG_SOURCEf, 
                     CVLAN_DOMAIN_POST_VXLT_DATA_PROCESSING_ENABLEf, 
                     CVLAN_DOMAIN_POST_VXLT_OAM_PROCESSING_ENABLEf },
           /* Action for incoming single inner priority tagged packets */
           { 52, 55, CVLAN_DOMAIN_POST_VXLT_CVLAN_TAG_SOURCEf, 
                     CVLAN_DOMAIN_POST_VXLT_DATA_PROCESSING_ENABLEf, 
                     CVLAN_DOMAIN_POST_VXLT_OAM_PROCESSING_ENABLEf },
           { 0 } /* table terminator */ 
    };

    _bcm_oam_flexible_oam_domain_vlan_ctrl_type2_t s_vlan_ctrl_info[] = {

        /* Set values for S VLAN MEP actions */
           /* Action for incoming packet with single outer tagged after 
              VXLT  */
           { 2, 2, 8,  SVLAN_DOMAIN_SVLAN_TAG_SOURCEf, 0,
                       SVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                       SVLAN_DOMAIN_OAM_PROCESSING_ENABLEf},
           /* Action for incoming packet with double tag after VXLT  */
           { 6, 2, 8,  SVLAN_DOMAIN_SVLAN_TAG_SOURCEf, 0,
                       SVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                       SVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },

       /* Set values for S+C VLAN MEP actions */
           /* Action for incoming packet with double tag after VXLT  */
           { 6, 2, 8,  SVLAN_CVLAN_DOMAIN_SVLAN_TAG_SOURCEf, 
                       SVLAN_CVLAN_DOMAIN_CVLAN_TAG_SOURCEf, 
                       SVLAN_CVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                       SVLAN_CVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },
           { 6, 2, 8,  0, 0, 
                       SVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                       SVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },
           { 6, 2, 8,  0, 0, 
                       CVLAN_DOMAIN_POST_VXLT_OAM_PROCESSING_ENABLEf, 
                       CVLAN_DOMAIN_POST_VXLT_DATA_PROCESSING_ENABLEf },
           { 6, 2, 8,  0, 
                       0, 
                       CVLAN_DOMAIN_PRE_VXLT_OAM_PROCESSING_ENABLEf, 
                       CVLAN_DOMAIN_PRE_VXLT_DATA_PROCESSING_ENABLEf },
           { 0, 0 } /* table terminator */ 
    };

    _bcm_oam_flexible_oam_domain_c_vlan_ctrl_type1_t *ctrl_info;
    _bcm_oam_flexible_oam_domain_vlan_ctrl_type2_t *s_ctrl_info;


    /* Read the table entries into the buffer. */
    max_index = soc_mem_index_max(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm);

    entry_mem_size = sizeof(ing_oam_flexible_domain_control_entry_t);
    /* Allocate buffer to store the DMAed table entries. */
    ent_buf = soc_cm_salloc(unit, entry_mem_size * (max_index + 1),
                            "OAM flexible domain control table entry buffer");
    if (NULL == ent_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(ent_buf, 0, sizeof(entry_mem_size) * (max_index + 1));

    /* Set action for C vlan */ 
    for (index = 0; ;index++) { 
        ctrl_info = &c_vlan_ctrl_info[index];
        if(ctrl_info->start_index == 0) {
            /* End of table */
            break;
        }
        for (entry_index = ctrl_info->start_index; 
             entry_index <= ctrl_info->end_index; entry_index++) {
            ent = soc_mem_table_idx_to_pointer
                    (unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                     ing_oam_flexible_domain_control_entry_t *,
                     ent_buf, entry_index);

            soc_mem_field_set(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                         (uint32 *)ent, ctrl_info->tag_source, &action_set);
            soc_mem_field_set(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                        (uint32 *)ent, ctrl_info->data_processing, &action_set);
            soc_mem_field_set(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                         (uint32 *)ent, ctrl_info->oam_processing, &action_set);
        }
    } 
    /* Set action for S and S+C vlan */ 
    for (index = 0; ;index++) { 
        s_ctrl_info = &s_vlan_ctrl_info[index];
        if(s_ctrl_info->start_index == 0) {
            /* End of table */
            break;
        }
        entry_index = s_ctrl_info->start_index;
        loop_index = s_ctrl_info->start_index;
        end_index = entry_index + s_ctrl_info->no_entries;
        while (entry_index <= max_index) {  
            for (; loop_index < end_index; loop_index++) {
                ent = soc_mem_table_idx_to_pointer(unit, 
                                      ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                                      ing_oam_flexible_domain_control_entry_t *,
                                      ent_buf, loop_index);
                if (s_ctrl_info->tag_source_1 != 0) {
                    soc_mem_field_set(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                         (uint32 *)ent, s_ctrl_info->tag_source_1, &action_set);
                }    
                if (s_ctrl_info->tag_source_2 != 0) {
                    soc_mem_field_set(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                         (uint32 *)ent, s_ctrl_info->tag_source_2, &action_set);
                }
                soc_mem_field_set(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                      (uint32 *)ent, s_ctrl_info->data_processing, &action_set);
                soc_mem_field_set(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                       (uint32 *)ent, s_ctrl_info->oam_processing, &action_set);
            }
            entry_index += s_ctrl_info->increment;
            loop_index = entry_index;
            end_index = (entry_index + s_ctrl_info->no_entries);
        } 
    }    

    rv = soc_mem_write_range(unit, ING_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                             MEM_BLOCK_ALL, 0, max_index, ent_buf); 
    if (BCM_FAILURE(rv)) {
        if (ent_buf) {
            soc_cm_sfree(unit, ent_buf);
        }
        return rv;
    }
    if (ent_buf) {
        soc_cm_sfree(unit, ent_buf);
    }

    return rv;
}
/*
 * Function:
 *     _bcm_kt2_oam_egr_flexible_oam_domain_ctrl_set
 * Purpose:
 *    Set the default values for egress flexible oam domain control 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_kt2_oam_egr_flexible_oam_domain_ctrl_set(int unit)
{
    int      rv = BCM_E_NONE;
    egr_oam_flexible_domain_control_entry_t *egr_ent_buf;
    egr_oam_flexible_domain_control_entry_t *ent;
    int      max_index = 0; 
    int      index = 0; 
    int      end_index = 0; 
    int      loop_index = 0; 
    int      entry_mem_size = 0;
    int      entry_index = 0;
    uint32   action_set = 1;

    _bcm_oam_flexible_oam_domain_vlan_ctrl_type2_t vlan_ctrl_info[] = {

          /*  CVLAN MEP before VXLT */
           { 16, 2, 4, 0, 0, 
                       CVLAN_DOMAIN_PRE_VXLT_DATA_PROCESSING_ENABLEf, 
                       CVLAN_DOMAIN_PRE_VXLT_OAM_PROCESSING_ENABLEf },
          /* CVLAN MEP after VXLT */
           { 4,  2, 8, CVLAN_DOMAIN_POST_VXLT_CVLAN_TAG_SOURCEf, 0, 
                       CVLAN_DOMAIN_POST_VXLT_DATA_PROCESSING_ENABLEf, 
                       CVLAN_DOMAIN_POST_VXLT_OAM_PROCESSING_ENABLEf },
          /* SVLAN MEP */
           { 8,  8, 16, 0, 0, 
                        SVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                        SVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },
          /* S+C VLAN MEP */
           { 14, 2, 18, SVLAN_CVLAN_DOMAIN_CVLAN_TAG_SOURCEf, 0, 
                        SVLAN_CVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                        SVLAN_CVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },

           { 24, 8, 8, 0, 0, 
                       SVLAN_CVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                       SVLAN_CVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },
           { 24, 8, 8, 0, 0, 
                       CVLAN_DOMAIN_PRE_VXLT_DATA_PROCESSING_ENABLEf, 
                       CVLAN_DOMAIN_PRE_VXLT_OAM_PROCESSING_ENABLEf },
           { 26, 2, 2, 0, 0, 
                       SVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                       SVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },
           { 30, 2, 2, 0, 0, 
                       SVLAN_DOMAIN_DATA_PROCESSING_ENABLEf, 
                       SVLAN_DOMAIN_OAM_PROCESSING_ENABLEf },
           { 30, 2, 2, CVLAN_DOMAIN_POST_VXLT_CVLAN_TAG_SOURCEf, 0, 
                       CVLAN_DOMAIN_POST_VXLT_DATA_PROCESSING_ENABLEf, 
                       CVLAN_DOMAIN_POST_VXLT_OAM_PROCESSING_ENABLEf },
           { 0, 0 } /* table terminator */ 
    };

    _bcm_oam_flexible_oam_domain_vlan_ctrl_type2_t *ctrl_info;


    /* Read the table entries into the buffer. */
    max_index = soc_mem_index_max(unit, EGR_OAM_FLEXIBLE_DOMAIN_CONTROLm);

    entry_mem_size = sizeof(egr_oam_flexible_domain_control_entry_t);
    /* Allocate buffer to store the DMAed table entries. */
    egr_ent_buf = soc_cm_salloc(unit, entry_mem_size * (max_index + 1),
                            "OAM flexible domain control table entry buffer");
    if (NULL == egr_ent_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(egr_ent_buf, 0, sizeof(entry_mem_size) * (max_index + 1));

    /* Set action for C, S and S+C vlan */ 
    for (index = 0; ;index++) { 
        ctrl_info = &vlan_ctrl_info[index];
        if(ctrl_info->start_index == 0) {
            /* End of table */
            break;
        }
        entry_index = ctrl_info->start_index;
        loop_index = ctrl_info->start_index;
        end_index = entry_index + ctrl_info->no_entries;
        while (entry_index <= max_index) {  
            for (; loop_index < end_index; loop_index++) {
                ent = soc_mem_table_idx_to_pointer
                    (unit, EGR_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                     egr_oam_flexible_domain_control_entry_t *,
                     egr_ent_buf, loop_index);

                if (ctrl_info->tag_source_1 != 0) {
                    soc_mem_field_set(unit, EGR_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                           (uint32 *)ent, ctrl_info->tag_source_1, &action_set);
                }
                if (ctrl_info->tag_source_2 != 0) {
                    soc_mem_field_set(unit, EGR_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                           (uint32 *)ent, ctrl_info->tag_source_2, &action_set);
                }
                soc_mem_field_set(unit, EGR_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                        (uint32 *)ent, ctrl_info->data_processing, &action_set);
                soc_mem_field_set(unit, EGR_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                         (uint32 *)ent, ctrl_info->oam_processing, &action_set);
            }
            entry_index += ctrl_info->increment;
            loop_index = entry_index;
            end_index = (entry_index + ctrl_info->no_entries);
        } 
    }    

    rv = soc_mem_write_range(unit, EGR_OAM_FLEXIBLE_DOMAIN_CONTROLm, 
                             MEM_BLOCK_ALL, 0, max_index, egr_ent_buf); 
    if (BCM_FAILURE(rv)) {
        if (egr_ent_buf) {
            soc_cm_sfree(unit, egr_ent_buf);
        }
        return rv;
    }
    if (egr_ent_buf) {
        soc_cm_sfree(unit, egr_ent_buf);
    }
    return rv;
}


/*
 * Function:
 *     _bcm_kt2_oam_flexible_oam_domain_ctrl_set
 * Purpose:
 *    Set the default values for flexible oam domain control 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_flexible_oam_domain_ctrl_set(int unit)
{
    int      rv = BCM_E_NONE;
    /* Set ingress flexible domain control */
    rv = _bcm_kt2_oam_ing_flexible_oam_domain_ctrl_set(unit); 
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Set egress flexible domain control */
    rv = _bcm_kt2_oam_egr_flexible_oam_domain_ctrl_set(unit); 
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    return rv;
}


typedef struct _bcm_oam_ing_flexible_drop_ctrl_s {
    soc_reg_t         drop_ctrl_reg;
    uint32            vfp_drop_ctrl;
    uint32            olp_drop_ctrl;
    uint32            spst_drop_ctrl;
    uint32            tag_drop_ctrl;
    uint32            cvlan_bfr_vxlt_drop_ctrl;
    uint32            cvlan_after_vxlt_drop_ctrl;
    uint32            e2e_drop_ctrl;
    uint32            vlan_drop_ctrl;
    uint32            disc_drop_ctrl;
} _bcm_oam_ing_flexible_drop_ctrl_t;

typedef struct _bcm_oam_egr_flexible_drop_ctrl_s {
    soc_reg_t         drop_ctrl_reg;
    uint32            egr_drop_ctrl;
    uint32            evxlt_payload_drop_ctrl;
    uint32            cvlan_after_vxlt_drop_ctrl;
    uint32            cvlan_before_vxlt_drop_ctrl;
} _bcm_oam_egr_flexible_drop_ctrl_t;

/*
 * Function:
 *     _bcm_kt2_oam_ing_flexible_drop_ctrl_set
 * Purpose:
 *    Set the default values for flexible ingress drop control 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_ing_flexible_drop_ctrl_set(int unit)
{
    uint32 en_rval = 0;
    int index = 0;
    _bcm_oam_ing_flexible_drop_ctrl_t *flex_drop_info;
    _bcm_oam_ing_flexible_drop_ctrl_t ing_flex_drop[] = {
        { OAM_PORT_INTERFACE_DROP_CONTROLr, 0, 1, 0, 0, 0, 0, 1, 0, 0xe01c1 },
        { OAM_C_INTERFACE_DROP_CONTROLr,    1, 1, 0, 0, 0, 1, 1, 0, 0xe01fd },
        { OAM_S_INTERFACE_DROP_CONTROLr,    1, 1, 0, 1, 1, 1, 1, 5, 0xe01fd },
        { OAM_S_C_INTERFACE_DROP_CONTROLr,  1, 1, 1, 1, 1, 1, 1, 7, 0xe01fd },
        { OAM_SVP_INTERFACE_DROP_CONTROLr,  1, 1, 1, 1, 1, 1, 1, 7, 0xe01fd },
        { 0 }, /* End of table */
    };

    for (index = 0; ;index++) { 
        flex_drop_info = &ing_flex_drop[index];
        if(flex_drop_info->drop_ctrl_reg == 0) {
            /* End of table */
            break;
        }

	SOC_IF_ERROR_RETURN(soc_reg32_get(unit, flex_drop_info->drop_ctrl_reg, 
                      REG_PORT_ANY, 0, &en_rval));
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, VFP_DROP_CTRLf,
                            flex_drop_info->vfp_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, OLP_ERROR_DROP_CTRLf,
                            flex_drop_info->olp_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, SPST_NOT_IN_FORWARDING_STATE_DROP_CTRLf,
                            flex_drop_info->spst_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, DISCARD_TAG_UNTAG_CTRLf,
                            flex_drop_info->tag_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, 
                            CVLAN_INTF_BEFORE_VXLT_VXLT_MISS_DROP_CTRLf,
                            flex_drop_info->cvlan_bfr_vxlt_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, 
                            CVLAN_INTF_AFTER_VXLT_VXLT_MISS_DROP_CTRLf,
                            flex_drop_info->cvlan_after_vxlt_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, ERRORED_E2E_PKT_DROP_CTRLf,
                            flex_drop_info->e2e_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, VLAN_DROP_VECTOR_CTRLf,
                            flex_drop_info->vlan_drop_ctrl);
        soc_reg_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, DISC_DROP_VECTOR_CTRLf,
                            flex_drop_info->disc_drop_ctrl);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, flex_drop_info->drop_ctrl_reg,
                            REG_PORT_ANY, 0, en_rval));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_egr_flexible_drop_ctrl_set
 * Purpose:
 *    Set the default values for flexible egress drop control 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_egr_flexible_drop_ctrl_set(int unit)
{
    uint64 en_rval, set_val;
    int index = 0;

    _bcm_oam_egr_flexible_drop_ctrl_t *flex_drop_info;
    _bcm_oam_egr_flexible_drop_ctrl_t egr_flex_drop[] = {
        { EGR_OAM_PORT_INTERFACE_DROP_CONTROL_64r, 0x1e0787d, 1, 1, 1 },
        { EGR_OAM_C_INTERFACE_DROP_CONTROL_64r,    0x1e0787c, 1, 1, 0 },
        { EGR_OAM_S_INTERFACE_DROP_CONTROL_64r,    0x1e06848, 1, 0, 0 },
        { EGR_OAM_S_C_INTERFACE_DROP_CONTROL_64r,  0x1e06048, 1, 0, 0 },
        { EGR_OAM_DVP_INTERFACE_DROP_CONTROL_64r,  0x1e06048, 1, 0, 0 },
        { 0 }, /* End of table */
    };

    COMPILER_64_ZERO(en_rval); 

    for (index = 0; ;index++) { 
        flex_drop_info = &egr_flex_drop[index];
        if(flex_drop_info->drop_ctrl_reg == 0) {
            /* End of table */
            break;
        }

	SOC_IF_ERROR_RETURN(soc_reg64_get(unit, flex_drop_info->drop_ctrl_reg, 
                                          REG_PORT_ANY, 0, &en_rval));
        
        COMPILER_64_SET(set_val, 0, flex_drop_info->egr_drop_ctrl);

        soc_reg64_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, EGR_DROP_VECTOR_CTRLf,
                            set_val);

        COMPILER_64_SET(set_val, 0, flex_drop_info->evxlt_payload_drop_ctrl);
        soc_reg64_field_set(unit, flex_drop_info->drop_ctrl_reg, 
                            &en_rval, 
                            EVXLT_APPLIED_TO_PAYLOAD_VXLT_MISS_DROP_CTRLf,
                            set_val);

        COMPILER_64_SET(set_val, 0, flex_drop_info->cvlan_after_vxlt_drop_ctrl);
        soc_reg64_field_set(unit, flex_drop_info->drop_ctrl_reg, 
           &en_rval, 
           EVXLT_APPLIED_TO_OUTER_L2_CVLAN_INTF_AFTER_VXLT_VXLT_MISS_DROP_CTRLf,
           set_val);

        COMPILER_64_SET(set_val, 0, flex_drop_info->cvlan_before_vxlt_drop_ctrl);
        soc_reg64_field_set(unit, flex_drop_info->drop_ctrl_reg, 
          &en_rval,
          EVXLT_APPLIED_TO_OUTER_L2_CVLAN_INTF_BEFORE_VXLT_VXLT_MISS_DROP_CTRLf,
          set_val);

        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, flex_drop_info->drop_ctrl_reg,
                            REG_PORT_ANY, 0, en_rval));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_flexible_drop_ctrl_set
 * Purpose:
 *    Set the default values for flexible ingress and egress drop control 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_flexible_drop_ctrl_set(int unit)
{
    int rv = BCM_E_NONE;
    /* Set ingress flexible drop control */
    rv = _bcm_kt2_oam_ing_flexible_drop_ctrl_set(unit); 
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Set egress flexible drop control */
    rv = _bcm_kt2_oam_egr_flexible_drop_ctrl_set(unit); 
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_s_intf_passive_proc_ctrl_set
 * Purpose:
 *    Set the default values for s-interface passive processing control 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_s_intf_passive_proc_ctrl_set(int unit)
{
    uint32 en_rval = 0;
    /* Set ingress s_intf passive processing control */
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr, 
                      REG_PORT_ANY, 0, &en_rval));

    soc_reg_field_set(unit, OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr, 
                        &en_rval, SPST_NOT_IN_FORWARDING_STATE_DROP_CTRLf,
                        0x1);
    /* Set ENIFILTER_DROP, INVALID_TPID_DROP and INVALID_VLAN_DROP  */
    soc_reg_field_set(unit, OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr, 
                        &en_rval, VLAN_DROP_VECTOR_CTRLf,
                        0x7);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr,
                        REG_PORT_ANY, 0, en_rval));

    en_rval = 0;
    /* Set egress s_intf passive processing control */
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, EGR_OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr, 
                      REG_PORT_ANY, 0, &en_rval));


    /* Set NOT_VLAN_MEMBER_DROP, STG_BLOCK_DROP, STG_DISABLE_DROP 
       For details - refer to EGR_DROP_VECTOR */ 
    soc_reg_field_set(unit, EGR_OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr, 
                        &en_rval, EGR_DROP_VECTOR_CTRLf,
                        0x1030);

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit,
                        EGR_OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr,
                        REG_PORT_ANY, 0, en_rval));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_hg_olp_enable
 * Purpose:
 *    Enable OLP handling on HG ports 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_hg_olp_enable(int unit)
{
    bcm_pbmp_t ports;
    bcm_port_t port;
    iarb_ing_physical_port_entry_t entry;

    BCM_PBMP_ASSIGN(ports, PBMP_PORT_ALL(unit));
    PBMP_ITER(ports, port) {
        if (IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PHYSICAL_PORTm, 
                                             MEM_BLOCK_ANY, port, &entry));
            soc_IARB_ING_PHYSICAL_PORTm_field32_set(unit, &entry, 
                                                    OLP_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, IARB_ING_PHYSICAL_PORTm, 
                                              MEM_BLOCK_ALL, port, &entry));
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
 *     _bcm_kt2_oam_olp_header_type_mapping_set
 * Purpose:
 *    Set default olp header type mapping
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 * MEP_TYPE      COMPRESSED_HDR_TYPE       HDR_TYPE          HDR_SUBTYPE
 *
 * ==========================================================================
 *  Downmep/IFP      3'b000                8'd1(L2_HDR)      8'd0(NULL)
 *  Downmep/IFP      3'b001                8'd0(OAM_HDR)     8'd2(CCM/BHH-CCM)
 *  Downmep/IFP      3'b010                8'd0(OAM_HDR)     8'd3(BFD)
 *  Downmep/IFP      3'b011                8'd0(OAM_HDR)     8'd4(LM/DM)
 *  Upmep            3'b000                N/A               N/A
 *  Upmep            3'b001                8'd0(OAM_HDR)     8'd5(CCM)
 *  Upmep            3'b010                N/A               N/A
 *  Upmep            3'b011                8'd0(OAM_HDR)     8'd6(LM/DM)
 */
STATIC int
_bcm_kt2_oam_olp_header_type_mapping_set(int unit)
{
    int                                 index = 0;
    egr_olp_header_type_mapping_entry_t entry;

    for (index = 0; index < kt2_olp_hdr_type_count; index++) {

        if (kt2_olp_hdr_type_mapping[index].reuse) {
            continue;
        }

        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(unit,
                                                     &entry,
                                                     HDR_TYPEf,
                                                     _BCM_KT2_OLP_OAM_HDR);

        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(
                                            unit,
                                            &entry,
                                            HDR_SUBTYPEf,
                                            kt2_olp_hdr_type_mapping[index].subtype);

        SOC_IF_ERROR_RETURN(
              WRITE_EGR_OLP_HEADER_TYPE_MAPPINGm(
                                       unit,
                                       MEM_BLOCK_ALL,
                                       kt2_olp_hdr_type_mapping[index].mem_index,
                                       &entry));
    }

    /* Program L2 HDR at the first entry */
    memset(&entry, 0, sizeof(entry));
    soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(unit,
                                                 &entry,
                                                 HDR_TYPEf,
                                                 _BCM_KT2_OLP_L2_HDR);
    SOC_IF_ERROR_RETURN(
             WRITE_EGR_OLP_HEADER_TYPE_MAPPINGm(unit,
                                                MEM_BLOCK_ALL,
                                                0,
                                                &entry));
    return BCM_E_NONE;
}

/*
 * Function: _bcm_kt2_oam_olp_fp_hw_index_get
 *
 * Purpose:
 *     Get OLP_HDR_TYPE_COMPRESSED corresponding to subtype
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_oam_olp_fp_hw_index_get(int unit,
                                  bcm_field_olp_header_type_t olp_hdr_type,
                                  int *hwindex)
{
    int index;

    switch (olp_hdr_type) {
        /* KT2 does not allow IFP to program UpMEP header type for
         * ADD_OLP_HEADER action, as the OLP_HDR_TYPE_COMPRESSED in
         * FP_POLICY_TABLE is not wide enough to index Up MEP subtypes
         * in EGR_OLP_HEADER_TYPE_MAPPING
         */

        case bcmFieldOlpHeaderTypeEthOamUpMepCcm:
        case bcmFieldOlpHeaderTypeEthOamUpMepLm:
        case bcmFieldOlpHeaderTypeEthOamUpMepDm:
            return BCM_E_PARAM;

        default:
            break;
    }

    for (index = 0; index < kt2_olp_hdr_type_count; index++) {
        if (kt2_olp_hdr_type_mapping[index].field_olp_hdr_type == olp_hdr_type) {
            *hwindex = kt2_olp_hdr_type_mapping[index].mem_index;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function: _bcm_kt2_oam_olp_hw_index_olp_type_get
 *
 * Purpose:
 *     Get subtype corresponding to OLP_HDR_TYPE_COMPRESSED
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_oam_olp_hw_index_olp_type_get(int unit,
                                        int hwindex,
                                        bcm_field_olp_header_type_t *olp_hdr_type)
{
    int index;

    for (index = 0; index < kt2_olp_hdr_type_count; index++) {
        if (kt2_olp_hdr_type_mapping[index].mem_index == hwindex) {

            *olp_hdr_type =
                      kt2_olp_hdr_type_mapping[hwindex].field_olp_hdr_type;

            return BCM_E_NONE;
        }
    }
    return BCM_E_PARAM;
}

/*
 * Function:
 *     _bcm_kt2_oam_drop_ctrl_set
 * Purpose:
 *    Set OAM drop control to not to drop wrong version OAM packets 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_drop_ctrl_set(int unit) 
{
    uint32   drop_ctrl = 0;
    soc_reg_field_set(unit, OAM_DROP_CONTROLr, &drop_ctrl,
                      IFP_OAM_UNKNOWN_OPCODE_VERSION_DROPf, 0);
    
    SOC_IF_ERROR_RETURN(WRITE_OAM_DROP_CONTROLr(unit, drop_ctrl));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_lm_cng_cpu_ctrl_set_default
 * Purpose:
 *     Set OAM LM cng and cpu data control
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_lm_cng_cpu_ctrl_set_default(int unit) 
{
    uint32   rval = 0;
    /* Set OAM LM CNG control such that
       1. All colored packets are counted, 
       2. Use LM_COLOR_MODE bit to qualify pre FP CNG for UPMEP TX 
          counters in IP
       3. Use LM_COLOR_MODE bit to qualify post FP CNG for DOWNMEP RX 
          counters in IP     
    */
    soc_reg_field_set(unit, OAM_LM_CNG_CONTROLr, &rval,
                      LM_COLOR_MODEf, 1);
    soc_reg_field_set(unit, OAM_LM_CNG_CONTROLr, &rval,
                      UPMEP_TX_CNG_SOURCEf, 1);
    soc_reg_field_set(unit, OAM_LM_CNG_CONTROLr, &rval,
                      DOWNMEP_RX_CNG_SOURCEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OAM_LM_CNG_CONTROLr(unit, rval));

    rval = 0;
    /* include all colored packets for LM accounting on egress side */
    soc_reg_field_set(unit, EGR_OAM_LM_CNG_CONTROLr, &rval,
                                            LM_COLOR_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_OAM_LM_CNG_CONTROLr(unit, rval));

    rval = 0;

    /* Set OAM_LM_CPU_DATA_CONTROL to not to count
       CPU generated data packets for loss measurement */
    soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                                 OAM_LCPU_TX_CNT_DISABLEf, 1);
    soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                                 OAM_LCPU_RX_CNT_DISABLEf, 1);
    soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                                 OAM_SRCPORT0_TX_CNT_DISABLEf, 1);
    soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                                 OAM_SRCPORT0_RX_CNT_DISABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_OAM_LM_CPU_DATA_CONTROLr(unit, rval));

    return (BCM_E_NONE);
} 
/*
 * Function:
 *     _bcm_kt2_oam_lm_cng_cpu_ctrl_set
 * Purpose:
 *     Set OAM LM cng and cpu data control
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_lm_cng_cpu_ctrl_set (int unit, bcm_oam_endpoint_info_t *ep) 
{
    uint32   rval = 0;
    uint32   egr_rval = 0;
    int      up_mep = 0;
    int      color_mode = 0;
    int      cng_source = 0;
    int      count_pkt = 1;
    if (ep->lm_flags) {
        /* read LM CNG register */
        SOC_IF_ERROR_RETURN(READ_OAM_LM_CNG_CONTROLr(unit, &rval));
        SOC_IF_ERROR_RETURN(READ_EGR_OAM_LM_CNG_CONTROLr(unit, &egr_rval));

        if (ep->lm_flags & BCM_OAM_LOSS_COUNT_GREEN_AND_YELLOW) {
            color_mode = 1;
        }  
        soc_reg_field_set(unit, OAM_LM_CNG_CONTROLr, &rval,
                          LM_COLOR_MODEf, color_mode);
        soc_reg_field_set(unit, EGR_OAM_LM_CNG_CONTROLr, &egr_rval,
                          LM_COLOR_MODEf, color_mode);

        if (ep->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            up_mep = 1;
        }
        if (ep->lm_flags & BCM_OAM_LOSS_COUNT_POST_TRAFFIC_CONDITIONING) {
            cng_source = 1;
        }
        if (up_mep) {
            soc_reg_field_set(unit, OAM_LM_CNG_CONTROLr, &rval,
                              UPMEP_TX_CNG_SOURCEf, cng_source);
        } else {
            soc_reg_field_set(unit, OAM_LM_CNG_CONTROLr, &rval,
                                  DOWNMEP_RX_CNG_SOURCEf, cng_source);
        }
         /* write back the registrs */  
        SOC_IF_ERROR_RETURN(WRITE_OAM_LM_CNG_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_EGR_OAM_LM_CNG_CONTROLr(unit, egr_rval));
         /* Read LM CPU data control */
        SOC_IF_ERROR_RETURN(READ_OAM_LM_CPU_DATA_CONTROLr(unit, &rval));
        if (ep->lm_flags & BCM_OAM_LOSS_COUNT_CPU_RX_PKT) {
            count_pkt = 0; 
        }
        soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                          OAM_LCPU_RX_CNT_DISABLEf, count_pkt);
        soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                          OAM_SRCPORT0_RX_CNT_DISABLEf, count_pkt);

        if (ep->lm_flags & BCM_OAM_LOSS_COUNT_CPU_TX_PKT) {
            count_pkt = 0; 
        } else {
            count_pkt = 1; 
        }
        soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                          OAM_LCPU_TX_CNT_DISABLEf, count_pkt);
        soc_reg_field_set(unit, OAM_LM_CPU_DATA_CONTROLr, &rval,
                          OAM_SRCPORT0_TX_CNT_DISABLEf, count_pkt);
        SOC_IF_ERROR_RETURN(WRITE_OAM_LM_CPU_DATA_CONTROLr(unit, rval));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_olp_magic_port_set
 * Purpose:
 *     Set Magic port used in OLP-XGS communication 
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_olp_magic_port_set(int unit)
{
    uint64 rval, set_val;
    int    modid;
    COMPILER_64_ZERO(rval); 
    /* configure modid and the magic port */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
    SOC_IF_ERROR_RETURN(READ_IARB_OLP_CONFIG_1r(unit, &rval));

    COMPILER_64_SET(set_val, 0, modid);
    soc_reg64_field_set(unit, IARB_OLP_CONFIG_1r, &rval, MY_MODIDf, set_val);
    COMPILER_64_SET(set_val, 0, _BCM_OAM_OLP_COMMUNICATION_PORT);
    soc_reg64_field_set(unit, IARB_OLP_CONFIG_1r, &rval, MY_PORT_NUMf, set_val);

    SOC_IF_ERROR_RETURN(WRITE_IARB_OLP_CONFIG_1r(unit, rval));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_macsa_zero_check_disable
 * Purpose:
 *     Disable MACSA Zero check  
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_macsa_zero_check_disable(int unit)
{
    uint32   rval = 0;
    soc_reg_field_set(unit, OAM_CONTROLr, &rval,
                      MACSA_ZERO_CHECK_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OAM_CONTROLr(unit, rval));
    soc_reg_field_set(unit, EGR_OAM_CONTROLr, &rval,
                      MACSA_ZERO_CHECK_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_OAM_CONTROLr(unit, rval));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_endpoint_cleanup
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
_bcm_kt2_oam_endpoint_cleanup(int unit, int upmep, 
                              _bcm_oam_hash_key_t  hash_key,
                              _bcm_oam_hash_data_t *hash_data)
{
    _bcm_oam_control_t   *oc;            /* Pointer to control structure.    */
    int                  rv = BCM_E_NONE;
    _bcm_oam_hash_data_t h_data_stored;     /* Stored hash data.            */
    uint8              active_mdl = 0;

    rv = _bcm_kt2_oam_control_get(unit, &oc);
    if (BCM_FAILURE(rv)) {
        return;
    }
    if (upmep) {
        /* Delete l3_entry, if already added */
        rv = _bcm_kt2_oam_egr_mp_group_entry_destroy(unit, hash_data, 
                                                         &active_mdl);
        /* Update Ingress MP group table(L3_ENTRY) */
        rv = _bcm_kt2_oam_l3_entry_destroy(unit, hash_data, &active_mdl);
    } else {
        /* Update Ingress MP group table(L3_ENTRY) */
        rv = _bcm_kt2_oam_l3_entry_destroy(unit, hash_data, &active_mdl);
        /* Delete l3_entry, if already added */
        rv = _bcm_kt2_oam_egr_mp_group_entry_destroy(unit, hash_data, 
                                                         &active_mdl);
    }
    /* return rx & tx counters allocated if any */
    rv =_bcm_kt2_oam_free_counter(unit, hash_data);

    /* Clear opcode profile entry, if any */
    if (hash_data->profile_index != _BCM_OAM_INVALID_INDEX) { 
        if (upmep) {
            rv = soc_profile_mem_delete(unit, 
                                        &oc->egr_oam_opcode_control_profile, 
                                        hash_data->profile_index);
        } else {
            rv = soc_profile_mem_delete(unit, &oc->oam_opcode_control_profile, 
                                    hash_data->profile_index);
        }
    }
    /* Clear dglp profile, if any */
    if (hash_data->dglp1_profile_index != _BCM_OAM_INVALID_INDEX) {
        if (upmep) {
            rv = soc_profile_mem_delete(unit, &oc->egr_oam_dglp_profile, 
                                    hash_data->dglp1_profile_index);
        } else {
            rv = soc_profile_mem_delete(unit, &oc->ing_oam_dglp_profile, 
                                    hash_data->dglp1_profile_index);
        }
    }
 
    if (hash_data->dglp2_profile_index != _BCM_OAM_INVALID_INDEX) {
        if (upmep) {
            rv = soc_profile_mem_delete(unit, &oc->egr_oam_dglp_profile, 
                                    hash_data->dglp2_profile_index);
        } else {
            rv = soc_profile_mem_delete(unit, &oc->ing_oam_dglp_profile, 
                                    hash_data->dglp2_profile_index);
        }
    }
    /* Clear Service pri map profile, if configured */
    if (hash_data->pri_map_index != _BCM_OAM_INVALID_INDEX) {
        rv = soc_profile_mem_delete(unit, &oc->ing_service_pri_map, 
                                    hash_data->pri_map_index);

    }
    if (hash_data->egr_pri_map_index != _BCM_OAM_INVALID_INDEX) {
        rv = soc_profile_mem_delete(unit, &oc->egr_service_pri_map, 
                                    hash_data->egr_pri_map_index);
    }
    /* Decrement TPID ref count, if already incremented */
    if (hash_data->outer_tpid_profile_index != _BCM_OAM_INVALID_INDEX) {
        rv = _bcm_kt2_tpid_entry_delete(unit, 
                                        hash_data->outer_tpid_profile_index,
                                        BCM_OAM_TPID_TYPE_OUTER);
    }
    if (hash_data->subport_tpid_profile_index != _BCM_OAM_INVALID_INDEX) {
        rv = _bcm_kt2_tpid_entry_delete(unit, 
                                        hash_data->subport_tpid_profile_index,
                                        BCM_OAM_TPID_TYPE_SUBPORT);
    }
    if (hash_data->inner_tpid_profile_index != _BCM_OAM_INVALID_INDEX) {
        rv = _bcm_kt2_tpid_entry_delete(unit, 
                                        hash_data->inner_tpid_profile_index,
                                        BCM_OAM_TPID_TYPE_INNER);
    }
    
    /* It might already have been deleted as well. 
     * So just log it as debug, in case it fails */
    rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_delete(unit, hash_data);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Remove from ma_idx to ep id map (EP=%d) -"
                               " %s.\n"), unit, hash_data->ep_id, bcm_errmsg(rv)));
    }
#if defined(INCLUDE_BHH)
    if((hash_data->type == bcmOAMEndpointTypeBHHMPLS) ||
       (hash_data->type == bcmOAMEndpointTypeBHHMPLSVccv)) {
        shr_idxres_list_free(oc->bhh_pool,
                             BCM_OAM_BHH_GET_UKERNEL_EP(hash_data->ep_id));
    } else
#endif
    {
        if (1 == hash_data->is_remote) {
            /* If remote endpoint, return index to remp pool */
            shr_idxres_list_free(oc->rmep_pool, hash_data->remote_index);
        } else {
            /* If local endpoint, return index to lmep pool */
            shr_idxres_list_free(oc->lmep_pool, hash_data->local_tx_index);
        }

        /* return index to mep pool */
        shr_idxres_list_free(oc->mep_pool, hash_data->ep_id);
    }
    /* Return entry to hash data entry to free pool. */
    shr_htb_find(oc->ma_mep_htbl, hash_key, 
                                 (shr_htb_data_t *)&h_data_stored, 1);

    /* Clear contents of hash data element. */
    _BCM_OAM_HASH_DATA_CLEAR(hash_data);
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void
_bcm_kt2_oam_group_info_dump(int unit, bcm_oam_group_t group_index,
                             _bcm_oam_group_data_t *group_p)
{
    int                     rv;
    bcm_oam_group_info_t    group_info;

    /* Initialize the group information structure. */
    bcm_oam_group_info_t_init(&group_info);

    /* Retrieve group information and set in group_info structure. */
    rv = _bcm_kt2_oam_get_group(unit, group_index, group_p, &group_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: _bcm_kt2_oam_group_info_dump"
                              " (GID=%d) - %s.\n"),
                              unit, group_index, bcm_errmsg(rv)));
        return;
    }
    LOG_CLI((BSL_META_U(unit,"------------------------------------------\r\n")));
    LOG_CLI((BSL_META_U(unit,"Group ID = %d details\r\n"), group_index));
    LOG_CLI((BSL_META_U(unit,"Group Name =  %s\r\n"), group_info.name));
    LOG_CLI((BSL_META_U(unit,"Flags =  %d\r\n"), group_info.flags));
    LOG_CLI((BSL_META_U(unit,"Faults =  %d\r\n"), group_info.faults));
    LOG_CLI((BSL_META_U(unit,"Persistent_faults =  %d\r\n"),
                              group_info.persistent_faults));
    LOG_CLI((BSL_META_U(unit,"clear_persistent_faults =  %d\r\n"),
                              group_info.clear_persistent_faults));
    LOG_CLI((BSL_META_U(unit,"lowest_alarm_priority =  %d\r\n"),
                              group_info.lowest_alarm_priority));
    LOG_CLI((BSL_META_U(unit,"------------------------------------------\r\n")));
}

void
_bcm_kt2_oam_hash_data_dump(int unit, _bcm_oam_hash_data_t *h_data_p)
{
    LOG_CLI((BSL_META_U(unit,"------------------------------------------\r\n")));
    LOG_CLI((BSL_META_U(unit,"EP ID = %d\r\n"),h_data_p->ep_id));
    LOG_CLI((BSL_META_U(unit,"Type = %d\r\n"),h_data_p->type));
    LOG_CLI((BSL_META_U(unit,"In use = %d\r\n"),h_data_p->in_use));
    LOG_CLI((BSL_META_U(unit,"Remote = %d\r\n"),h_data_p->is_remote));
    LOG_CLI((BSL_META_U(unit,"Name = %d\r\n"),h_data_p->name));
    LOG_CLI((BSL_META_U(unit,"Level = %d\r\n"),h_data_p->level));
    LOG_CLI((BSL_META_U(unit,"Outer Vlan = %d\r\n"),h_data_p->vlan));
    LOG_CLI((BSL_META_U(unit,"Inner Vlan = %d\r\n"),h_data_p->inner_vlan));
    LOG_CLI((BSL_META_U(unit,"Gport = %d\r\n"),h_data_p->gport));
    LOG_CLI((BSL_META_U(unit,"SGLP = %d\r\n"),h_data_p->sglp));
    LOG_CLI((BSL_META_U(unit,"DGLP = %d\r\n"),h_data_p->dglp));
    LOG_CLI((BSL_META_U(unit,"Tx ena = %d\r\n"),h_data_p->local_tx_enabled));
    LOG_CLI((BSL_META_U(unit,"Rx ena = %d\r\n"),h_data_p->local_rx_enabled));
    LOG_CLI((BSL_META_U(unit,"Grp idx = %d\r\n"),h_data_p->group_index));
    LOG_CLI((BSL_META_U(unit,"Remote idx = %d\r\n"),h_data_p->remote_index));
    LOG_CLI((BSL_META_U(unit,"LM ctr idx = %d\r\n"),h_data_p->lm_counter_index));
    LOG_CLI((BSL_META_U(unit,"Pri map idx = %d\r\n"),h_data_p->pri_map_index));
    LOG_CLI((BSL_META_U(unit,"Profile Idx = %d\r\n"),h_data_p->profile_index));
    LOG_CLI((BSL_META_U(unit,"Local Tx index = %d\r\n"),h_data_p->local_tx_index));
    LOG_CLI((BSL_META_U(unit,"Local Rx index = %d\r\n"),h_data_p->local_rx_index));
    LOG_CLI((BSL_META_U(unit,"VP = %d\r\n"),h_data_p->vp));
    LOG_CLI((BSL_META_U(unit,"Flags = 0x%x\r\n"),h_data_p->flags));
    LOG_CLI((BSL_META_U(unit,"Flags2 = 0x%x\r\n"),h_data_p->flags2));
    LOG_CLI((BSL_META_U(unit,"Vfp_entry = 0x%x\r\n"),h_data_p->vfp_entry));
    LOG_CLI((BSL_META_U(unit,"Fp_entry_tx = 0x%x\r\n"),h_data_p->fp_entry_tx));
    LOG_CLI((BSL_META_U(unit,"Fp_entry_rx = 0x%x\r\n"),h_data_p->fp_entry_rx));
    LOG_CLI((BSL_META_U(unit,"Period = %d\r\n"),h_data_p->period));
    LOG_CLI((BSL_META_U(unit,"Opcode flags = 0x%x\r\n"),h_data_p->opcode_flags));
    LOG_CLI((BSL_META_U(unit,"TS format = %d\r\n"),h_data_p->ts_format));
    LOG_CLI((BSL_META_U(unit,"Label = 0x%x\r\n"),h_data_p->label));
    LOG_CLI((BSL_META_U(unit,"Vlan Pri = %d\r\n"),h_data_p->vlan_pri));
    LOG_CLI((BSL_META_U(unit,"Egress if = 0x%x\r\n"),h_data_p->egress_if));
    LOG_CLI((BSL_META_U(unit,"Int pri = %d\r\n"),h_data_p->int_pri));
    LOG_CLI((BSL_META_U(unit,"Trunk index = %d\r\n"),h_data_p->trunk_index));
    LOG_CLI((BSL_META_U(unit,"Domain = %d\r\n"),h_data_p->oam_domain));
    LOG_CLI((BSL_META_U(unit,"Egr pri map idx = %d\r\n"),h_data_p->egr_pri_map_index));
    LOG_CLI((BSL_META_U(unit,"Rx ctr = %d\r\n"),h_data_p->rx_ctr));
    LOG_CLI((BSL_META_U(unit,"Tx ctr = %d\r\n"),h_data_p->tx_ctr));
    LOG_CLI((BSL_META_U(unit,"src pp port = %d\r\n"),h_data_p->src_pp_port));
    LOG_CLI((BSL_META_U(unit,"dst pp port = %d\r\n"),h_data_p->dst_pp_port));
    LOG_CLI((BSL_META_U(unit,"DGLP1 = %d\r\n"),h_data_p->dglp1));
    LOG_CLI((BSL_META_U(unit,"DGLP2 = %d\r\n"),h_data_p->dglp2));
    LOG_CLI((BSL_META_U(unit,"DGLP1 Profile idx = %d\r\n"),h_data_p->dglp1_profile_index));
    LOG_CLI((BSL_META_U(unit,"DGLP2 Profile idx = %d\r\n"),h_data_p->dglp2_profile_index));
    LOG_CLI((BSL_META_U(unit,"Outer TPID = 0x%04X\r\n"),h_data_p->outer_tpid));
    LOG_CLI((BSL_META_U(unit,"Inner TPID = 0x%04X\r\n"),h_data_p->inner_tpid));
    LOG_CLI((BSL_META_U(unit,"SubPort TPID = %d\r\n"),h_data_p->subport_tpid));
    LOG_CLI((BSL_META_U(unit,"Outer TPID profile idx = %d\r\n"),h_data_p->outer_tpid_profile_index));
    LOG_CLI((BSL_META_U(unit,"Inner TPID profile idx = %d\r\n"),h_data_p->inner_tpid_profile_index));
    LOG_CLI((BSL_META_U(unit,"SubPort TPID profile idx = %d\r\n"),h_data_p->subport_tpid_profile_index));
    LOG_CLI((BSL_META_U(unit,"MA Base idx = %d\r\n"),h_data_p->ma_base_index));
    LOG_CLI((BSL_META_U(unit,"Trunk ID = %d\r\n"),h_data_p->trunk_id));
    LOG_CLI((BSL_META_U(unit,"Active MDL BITMAP = 0x%x\r\n"),h_data_p->active_mdl_bitmap));
    LOG_CLI((BSL_META_U(unit,"Internal flags = 0x%x\r\n"),h_data_p->int_flags));
#if defined(INCLUDE_BHH)
    LOG_CLI((BSL_META_U(unit,"Cpu qid = %d\r\n"),h_data_p->cpu_qid));
    LOG_CLI((BSL_META_U(unit,"BHH Endpoint Index = %d\r\n"),h_data_p->bhh_endpoint_index));
    LOG_CLI((BSL_META_U(unit,"BHH DM ENTRY= %d\r\n"),h_data_p->bhh_dm_entry_rx));
    LOG_CLI((BSL_META_U(unit,"BHH LM ENTRY= %d\r\n"),h_data_p->bhh_lm_entry_rx));
    LOG_CLI((BSL_META_U(unit,"BHH ENTRY PKT RX= %d\r\n"),h_data_p->bhh_entry_pkt_rx));
    LOG_CLI((BSL_META_U(unit,"BHH ENTRY PKT TX= %d\r\n"),h_data_p->bhh_entry_pkt_tx));
    LOG_CLI((BSL_META_U(unit,"Src Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             h_data_p->src_mac_address[0], h_data_p->src_mac_address[1],
             h_data_p->src_mac_address[2], h_data_p->src_mac_address[3],
             h_data_p->src_mac_address[4], h_data_p->src_mac_address[5]));
    LOG_CLI((BSL_META_U(unit,"Dst Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             h_data_p->dst_mac_address[0], h_data_p->dst_mac_address[1],
             h_data_p->dst_mac_address[2], h_data_p->dst_mac_address[3],
             h_data_p->dst_mac_address[4], h_data_p->dst_mac_address[5]));
    LOG_CLI((BSL_META_U(unit,"Inner vlan pri = %d\r\n"),h_data_p->inner_vlan_pri));
    LOG_CLI((BSL_META_U(unit,"egr label= %d\r\n"),h_data_p->egr_label));
    LOG_CLI((BSL_META_U(unit,"egr label exp= %d\r\n"),h_data_p->egr_label_exp));
    LOG_CLI((BSL_META_U(unit,"egr label ttl= %d\r\n"),h_data_p->egr_label_ttl));
#endif
    LOG_CLI((BSL_META_U(unit,"Vccv Type = %d\r\n"),h_data_p->vccv_type));
    LOG_CLI((BSL_META_U(unit,"VPN = %d\r\n"),h_data_p->vpn));
    LOG_CLI((BSL_META_U(unit,"PM profile attached = %d\r\n"),h_data_p->pm_profile_attached));
    LOG_CLI((BSL_META_U(unit,"CTR Field id = %d\r\n"),h_data_p->ctr_field_id));
    LOG_CLI((BSL_META_U(unit,"EGR CTR Field id = %d\r\n"),h_data_p->egr_ctr_field_id));
    LOG_CLI((BSL_META_U(unit,"Resolved trunk gport based on trunk "
                        "index 0x%x\r\n"),
                        h_data_p->resolved_trunk_gport));
    LOG_CLI((BSL_META_U(unit,"------------------------------------------\r\n")));
}

/*
 * Function:
 *     _bcm_oam_sw_dump
 * Purpose:
 *     Displays oam information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_kt2_oam_sw_dump(int unit)
{
    int                    rv;
    bcm_oam_group_t        grp_idx;
    _bcm_oam_control_t     *oc = NULL;
    _bcm_oam_group_data_t  *group_p = NULL;
    _bcm_oam_ep_list_t     *ep_list_p = NULL;
    _bcm_oam_hash_data_t   *ep_data_p = NULL;

    LOG_CLI((BSL_META_U(unit,"OAM\n")));
    /* Get OAM control structure. */
    rv = _bcm_kt2_oam_control_get(unit, &oc);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                "OAM(unit %d) Error: Get oam control variable\n"),
                 unit));

        return;
    }

    _BCM_OAM_LOCK(oc);

    group_p = oc->group_info;
    for (grp_idx = 0; grp_idx < oc->group_count; ++grp_idx) {
        if (group_p[grp_idx].in_use == 1) {
            ep_list_p = *(group_p[grp_idx].ep_list);
            _bcm_kt2_oam_group_info_dump(unit, grp_idx, group_p);
            while (ep_list_p != NULL) {
                ep_data_p = ep_list_p->ep_data_p;
                _bcm_kt2_oam_hash_data_dump(unit, ep_data_p);
                ep_list_p = ep_list_p->next;
            }
        }
    }

    _BCM_OAM_UNLOCK(oc);
}

void _bcm_kt2_oam_ma_idx_to_ep_id_mapping_print(int unit)
{
    _bcm_oam_control_t   *oc = NULL;            /* Pointer to control structure.    */
    int i = 0;
    int rv = BCM_E_NONE;

    rv = _bcm_kt2_oam_control_get(unit, &oc);
    if ((BCM_FAILURE(rv)) || (oc == NULL)) {
        return;
    }
    _BCM_OAM_LOCK(oc);

    if (oc->_bcm_oam_ma_idx_to_ep_id_map != NULL) {
        for (i = 0; i < (oc->ma_idx_count + oc->egr_ma_idx_count);i++) {
            if (oc->_bcm_oam_ma_idx_to_ep_id_map[i] != _BCM_OAM_INVALID_INDEX) {
                LOG_CLI((BSL_META_U(unit,"MA_IDX = %d EP_ID = %d\r\n"),
                        i, oc->_bcm_oam_ma_idx_to_ep_id_map[i]));
            } 
        }
    }
    _BCM_OAM_UNLOCK(oc);
} 
#endif

#if defined(BCM_WARM_BOOT_SUPPORT)
#ifdef _KATANA2_DEBUG
/*
 * Function:
 *     _bcm_kt2_oam_tx_counter_recover
 * Purpose:
 *     Recover Rx counter for MEP 
 * Parameters:
 *     unit      - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_tx_counter_recover(int unit, soc_mem_t  mem, 
                                uint32 *entry,
                                _bcm_oam_hash_data_t   *h_data_p)
{
    _bcm_oam_control_t   *oc;        /* Pointer to control structure.  */
    int           rv = BCM_E_NONE;/* Operation return status.         */
    int           up_mep = 0;
    mep_ctr_info_t  *ctr_info;
    shr_aidxres_list_handle_t *ctr_pool;
    int           *map;
    int           ctr_type = 0;
    int           ctr_valid = 0;
    int           ctr_mdl = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    if (mem == L3_ENTRY_IPV4_UNICASTm) {
        up_mep = 1;
        ctr_info = mep_ctr_info;
        map = &h_data_p->pri_map_index;
    } else if (mem == EGR_MP_GROUPm) {
        ctr_info = egr_mep_ctr_info;
        map = &h_data_p->egr_pri_map_index;
    } else {
        return BCM_E_INTERNAL; 
    }
    ctr_pool = oc->ing_lm_ctr_pool;
    ctr_valid = soc_mem_field32_get(unit, mem, 
                                    (uint32 *)entry, ctr_info[0].ctr_valid);

    if (ctr_valid) {
        ctr_type = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[0].ctr_mep_type); 
        ctr_mdl = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[0].ctr_mep_mdl); 
        if ((ctr_type == up_mep) && (ctr_mdl == h_data_p->level)) {
            h_data_p->tx_ctr = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                 ctr_info[0].ctr_base_ptr);

            h_data_p->flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
            *map = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                  ctr_info[0].ctr_profile);
            /* Allocate 8 consecutive couters from the pool */
            rv =  shr_aidxres_list_reserve_block(ctr_pool[0],
                                                       h_data_p->tx_ctr, 8);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter block alloc failed "
                                      "(EP=%d) - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
        }
    }
    ctr_valid = soc_mem_field32_get(unit, mem, 
                                    (uint32 *)entry, ctr_info[1].ctr_valid);
    if (ctr_valid) {
        ctr_type = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[1].ctr_mep_type); 
        ctr_mdl = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[1].ctr_mep_mdl); 
        if ((ctr_type == up_mep) && (ctr_mdl == h_data_p->level)) {
            h_data_p->tx_ctr = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                 ctr_info[1].ctr_base_ptr);

            h_data_p->flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
            *map = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                  ctr_info[1].ctr_profile);
            /* Allocate 8 consecutive couters from the pool */
            rv =  shr_aidxres_list_reserve_block(ctr_pool[1],
                                                        h_data_p->tx_ctr, 8);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter block alloc failed "
                                      "(EP=%d) - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
        }
    }
    return (rv);
}

#endif /* _KATANA2_DEBUG */

/*
 * Function:
 *     _bcm_kt2_oam_rx_counter_recover
 * Purpose:
 *     Recover Rx counter for MEP 
 * Parameters:
 *     unit      - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_rx_counter_recover(int unit, soc_mem_t  mem, 
                                uint32 *entry,
                                _bcm_oam_hash_data_t   *h_data_p)
{
    _bcm_oam_control_t   *oc;        /* Pointer to control structure.  */
    int           rv = BCM_E_NONE;/* Operation return status.         */
    int           up_mep = 0;
    mep_ctr_info_t  *ctr_info;
    shr_aidxres_list_handle_t *ctr_pool;
    int           *map;
    int           ctr_valid = 0;
    int           ctr_mdl = 0;
    int           ctr_type = 0;
    soc_profile_mem_t    *pri_map_profile;  
                                                        

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    if (mem == EGR_MP_GROUPm) {
        up_mep = 1;
        ctr_info = egr_mep_ctr_info;
        map = &h_data_p->egr_pri_map_index;
        pri_map_profile =  &oc->egr_service_pri_map;
    } else {
        ctr_info = mep_ctr_info;
        map = &h_data_p->pri_map_index;
        pri_map_profile =  &oc->ing_service_pri_map;
    } 
    ctr_pool = oc->ing_lm_ctr_pool;
    ctr_valid = soc_mem_field32_get(unit, mem, 
                                    (uint32 *)entry, ctr_info[0].ctr_valid);

    if (ctr_valid) {
        ctr_type = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[0].ctr_mep_type); 
        ctr_mdl = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[0].ctr_mep_mdl); 
        if ((ctr_type == up_mep) && (ctr_mdl == h_data_p->level)) {
            h_data_p->rx_ctr = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                 ctr_info[0].ctr_base_ptr);

            h_data_p->flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
            *map = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                  ctr_info[0].ctr_profile);
            rv = soc_profile_mem_reference(unit, pri_map_profile,
                               ((*map) * BCM_OAM_INTPRI_MAX), BCM_OAM_INTPRI_MAX);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter profile ref count "
                                      "increment failed (EP=%d) - %s.\n"),
                           h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
            /* Allocate 8 consecutive couters from the pool */
            rv =  shr_aidxres_list_reserve_block(ctr_pool[0],
                                                       h_data_p->rx_ctr, 8);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter block alloc failed "
                                      "(EP=%d) - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
        }
    }
    ctr_valid = soc_mem_field32_get(unit, mem, 
                                    (uint32 *)entry, ctr_info[1].ctr_valid);
    if (ctr_valid) {
        ctr_type = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[1].ctr_mep_type); 
        ctr_mdl = soc_mem_field32_get(unit, mem, (uint32 *)entry,
                                                 ctr_info[1].ctr_mep_mdl); 
        if ((ctr_type == up_mep) && (ctr_mdl == h_data_p->level)) {
            h_data_p->rx_ctr = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                 ctr_info[1].ctr_base_ptr);

            h_data_p->flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
            *map = soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                                  ctr_info[1].ctr_profile);
            rv = soc_profile_mem_reference(unit, pri_map_profile,
                           ((*map) * BCM_OAM_INTPRI_MAX), BCM_OAM_INTPRI_MAX);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter profile ref count  "
                                      "increment failed (EP=%d) - %s.\n"),
                           h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
            /* Allocate 8 consecutive couters from the pool */
            rv =  shr_aidxres_list_reserve_block(ctr_pool[1],
                                                      h_data_p->rx_ctr, 8);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter block alloc failed "
                                      "(EP=%d) - %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
            }
        }
    }
    return (rv);
}
/*
 * Function:
 *     _bcm_kt2_oam_endpoint_alloc
 * Purpose:
 *     Allocate an endpoint memory element.
 * Parameters:
 *     ep_pp - (IN/OUT) Pointer to endpoint address pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_endpoint_alloc(bcm_oam_endpoint_info_t **ep_pp)
{
    bcm_oam_endpoint_info_t *ep_p = NULL;

    _BCM_OAM_ALLOC(ep_p, bcm_oam_endpoint_info_t,
                   sizeof(bcm_oam_endpoint_info_t), "Endpoint info");
    if (NULL == ep_p) {
        return (BCM_E_MEMORY);
    }
    
    *ep_pp = ep_p;

    return (BCM_E_NONE);

}

/*
 * Function:
 *     _bcm_kt2_oam_sync
 * Purpose:
 *     Store OAM configuration to level two storage cache memory.
 * Parameters:
 *     unit - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_oam_sync(int unit)
{
    int                 rv;  /* Operation return status. */
    _bcm_oam_control_t  *oc;
    int                 alloc_size = 0;
    int                 stable_size;
    soc_scache_handle_t scache_handle;
    uint8               *oam_scache;
    uint8               *rmep_scache = NULL;
    uint8               mdl = 0;
    int                 grp_idx;
    _bcm_oam_group_data_t *group_p;   /* Pointer to group list.         */
    int                 group_count = 0;
    _bcm_oam_ep_list_t  *ep_list_p = NULL;
    _bcm_oam_hash_data_t ep_data;
    int                 ma_offset;
    int                 idx = 0;
    _bcm_oam_hash_data_t    *h_data_p = NULL;       /* Endpoint hash data pointer.   */
    

    /* Get OAM module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;                                                      
    }

    /* Get handle to control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    /* Initialize to group array pointer. */
    group_p = oc->group_info;

    for (grp_idx = 0; grp_idx < oc->group_count; grp_idx++) {

        /* Check if the group is in use. */
        if (BCM_E_EXISTS
            == shr_idxres_list_elem_state(oc->group_pool, grp_idx)) {
            group_count++;
        }
    }
    alloc_size += BCM_OAM_GROUP_NAME_LENGTH * (oc->group_count);
    /* To store OAM group count. */
    alloc_size += sizeof(int);

    /* To store FP GID. */
    alloc_size += 3 * sizeof(bcm_field_group_t);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM, 0);

    /* To store EP ids */
    alloc_size += (_BCM_KT2_OAM_ENDPOINT_ID_SCACHE_SIZE);

    /* Allocate memory to store oc->eth_oam_mp_group_vlan_key */
    alloc_size += sizeof(uint8);

    /* Allocate memory to store all RMEP entries info */
    alloc_size += (_BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_SIZE);

    /* Check if memory has already been allocated */
    rv = _bcm_esw_scache_ptr_get(unit,
                                 scache_handle,
                                 0,
                                 alloc_size,
                                 &oam_scache,
                                 BCM_WB_DEFAULT_VERSION,
                                 NULL
                                 );
    if (!SOC_WARM_BOOT(unit) && (BCM_E_NOT_FOUND == rv)) {
        rv = _bcm_esw_scache_ptr_get(unit,
                                     scache_handle,
                                     1,
                                     alloc_size,
                                     &oam_scache,
                                     BCM_WB_DEFAULT_VERSION,
                                     NULL
                                     );
        if (BCM_FAILURE(rv)
            || (NULL == oam_scache)) {
            goto cleanup;
        }
    } else if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Store the FP groups */
    sal_memcpy(oam_scache, &oc->vfp_group, sizeof(bcm_field_group_t));
    oam_scache += sizeof(bcm_field_group_t);

    sal_memcpy(oam_scache, &oc->fp_vp_group, sizeof(bcm_field_group_t));
    oam_scache += sizeof(bcm_field_group_t);

    sal_memcpy(oam_scache, &oc->fp_glp_group, sizeof(bcm_field_group_t));
    oam_scache += sizeof(bcm_field_group_t);

    sal_memcpy(oam_scache, &group_count, sizeof(int));
    oam_scache += sizeof(int);

    for (grp_idx = 0; grp_idx < oc->group_count; ++grp_idx)
    {
        /* Check if the group is in use. */
        if (BCM_E_EXISTS
                == shr_idxres_list_elem_state(oc->group_pool, grp_idx)) {

            sal_memcpy(oam_scache, group_p[grp_idx].name,
                       BCM_OAM_GROUP_NAME_LENGTH);

            oam_scache += BCM_OAM_GROUP_NAME_LENGTH;
        }
    }

    /* Store logical endpoint indexes
       Following is scache allocation for different types of EPs
       -----------------------------------------------------------------------
      | Rx DownMEP indexes | Rx UpMEP indexes | RMEP indexes | Tx MEP indexes |
       -----------------------------------------------------------------------
     */
    sal_memset(oam_scache, 0, _BCM_KT2_OAM_ENDPOINT_ID_SCACHE_SIZE);

    for (grp_idx = 0; grp_idx < oc->group_count; ++grp_idx)
    {
        if (group_p[grp_idx].in_use == 1) {
            ep_list_p = *(group_p[grp_idx].ep_list);
            while (ep_list_p != NULL) {
                sal_memcpy(&ep_data, ep_list_p->ep_data_p, sizeof(_bcm_oam_hash_data_t)); 
                if (ep_data.type == bcmOAMEndpointTypeEthernet) {

                    if (ep_data.is_remote) {
                        sal_memcpy(oam_scache + 
                                (_BCM_KT2_OAM_RMEP_SCACHE_LOCATION(ep_data.remote_index)), 
                                &(ep_data.ep_id), sizeof (bcm_oam_endpoint_t)); 
                    } else {
                        if (ep_data.local_tx_enabled) {
                            sal_memcpy(oam_scache + 
                                (_BCM_KT2_OAM_TX_MEP_SCACHE_LOCATION(ep_data.local_tx_index)),
                                    &(ep_data.ep_id), sizeof (bcm_oam_endpoint_t));
                        }
                        if (ep_data.local_rx_enabled) {
                            if (ep_data.flags & BCM_OAM_ENDPOINT_UP_FACING) {
                                sal_memcpy(oam_scache + 
                                        (_BCM_KT2_OAM_RX_UPMEP_SCACHE_LOCATION(ep_data.local_rx_index)),
                                        &(ep_data.ep_id), sizeof (bcm_oam_endpoint_t));
                            } else {
                            _bcm_kt2_oam_ma_index_offset_get(unit, &ep_data, &ma_offset);
                            ep_data.local_rx_index = ep_data.ma_base_index + ma_offset;
                            sal_memcpy(oam_scache + 
                                    (_BCM_KT2_OAM_RX_DOWNMEP_SCACHE_LOCATION(ep_data.local_rx_index)),
                                        &(ep_data.ep_id), sizeof (bcm_oam_endpoint_t));
                            }

                        }
                    }
                }
                ep_list_p = ep_list_p->next;
            }
        }
    }
    oam_scache +=  _BCM_KT2_OAM_ENDPOINT_ID_SCACHE_SIZE;

    sal_memcpy(oam_scache, &(oc->eth_oam_mp_group_vlan_key), sizeof(uint8));
    oam_scache += sizeof(uint8);

    for (idx = 0; idx < oc->ep_count; idx++) {
        h_data_p = &oc->oam_hash_data[idx];
        if (ETH_TYPE(h_data_p->type) && (h_data_p->in_use)) {
            sal_memcpy(oam_scache, &(h_data_p->flags), sizeof(uint32));
        }
        oam_scache += sizeof(uint32);
    }

    /*   Following is scache allocation for RMEP EPs info */
    sal_memset(oam_scache, 0, _BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_SIZE);

    for (grp_idx = 0; grp_idx < oc->group_count; ++grp_idx)
    {
        if (group_p[grp_idx].in_use == 1) {
            ep_list_p = *(group_p[grp_idx].ep_list);
            while (ep_list_p != NULL) {
                sal_memcpy(&ep_data, ep_list_p->ep_data_p, sizeof(_bcm_oam_hash_data_t));
                if ((ep_data.type == bcmOAMEndpointTypeEthernet) && (ep_data.is_remote)) {
                    rmep_scache = oam_scache + (_BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_LOCATION(ep_data.remote_index));
                    sal_memcpy(rmep_scache, &(ep_data.ep_id), sizeof (bcm_oam_endpoint_t));
                    rmep_scache += sizeof(bcm_oam_endpoint_t);
                    mdl = ep_data.level;
                    sal_memcpy(rmep_scache, &(mdl), sizeof(uint8));
                    rmep_scache += sizeof(uint8);
                    sal_memcpy(rmep_scache, &(ep_data.vlan), sizeof(bcm_vlan_t));
                    rmep_scache += sizeof(bcm_vlan_t);
                    sal_memcpy(rmep_scache, &(ep_data.gport), sizeof(bcm_gport_t));
                    rmep_scache += sizeof(bcm_gport_t);
                }
                ep_list_p = ep_list_p->next;
            }
        }
    }
    oam_scache +=  _BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_SIZE;

cleanup:
    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_wb_group_recover
 * Purpose:
 *     Recover OAM group configuration.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     stable_size - (IN) OAM module Level2 storage size.
 *     oam_scache  - (IN) Pointer to scache address pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_wb_group_recover(int unit, int stable_size, uint8 **oam_scache)
{
    int                    index;                    /* Hw table index.     */
    _bcm_oam_group_data_t  *group_p;                 /* Group info pointer. */
    maid_reduction_entry_t maid_entry;               /* Group entry info.   */
    ma_state_entry_t       ma_state_ent;             /* Group state info.   */
    int                    maid_reduction_valid = 0; /* Group valid.        */
    int                    ma_state_valid = 0;       /* Group state valid.  */
    _bcm_oam_control_t     *oc;                      /* Pointer to Control  */
                                                     /* structure.          */
    int                     rv;                      /* Operation return    */
                                                     /* status.             */

    /* Control lock taken by calling routine. */
    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    for (index = 0; index < oc->group_count; index++) {

        rv = READ_MAID_REDUCTIONm(unit, MEM_BLOCK_ANY, index,
                                  &maid_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: (GID=%d) MAID_REDUCTION table read"
                                   " failed  - %s.\n"), index, bcm_errmsg(rv)));
            goto cleanup;
        }

        rv = READ_MA_STATEm(unit, MEM_BLOCK_ANY, index, &ma_state_ent);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: (GID=%d) MA_STATE table read"
                                   " failed  - %s.\n"), index, bcm_errmsg(rv)));
            goto cleanup;
        }

        maid_reduction_valid
            = soc_MAID_REDUCTIONm_field32_get(unit, &maid_entry, VALIDf);

        ma_state_valid
            = soc_MA_STATEm_field32_get(unit, &ma_state_ent, VALIDf);

        if (maid_reduction_valid || ma_state_valid) {

            /* Entry must be valid in both the tables, else return error. */
            if (!maid_reduction_valid || !ma_state_valid) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            /* Get the group memory pointer. */
            group_p = &oc->group_info[index];
            if (1 == group_p->in_use) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {

                /* Set group name as zeros. */
                sal_memset(group_p->name, 0, BCM_OAM_GROUP_NAME_LENGTH);

            } else {
                    /* Get the group name from stored info. */
                    sal_memcpy(group_p->name, *oam_scache, 
                            BCM_OAM_GROUP_NAME_LENGTH);
                    *oam_scache = (*oam_scache + BCM_OAM_GROUP_NAME_LENGTH);
            }

            /* Reserve the group index. */
            rv = shr_idxres_list_reserve(oc->group_pool, index, index);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: (GID=%d) Index reserve "
                                       " failed  - %s.\n"), index, bcm_errmsg(rv)));
                rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                goto cleanup;
            }

            /* Create the linked list to maintain group endpoint information. */
            _BCM_OAM_ALLOC((group_p->ep_list), _bcm_oam_ep_list_t *,
                           sizeof(_bcm_oam_ep_list_t *), "EP list head");
            if (NULL == group_p->ep_list) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            /* Initialize head node.*/
            *group_p->ep_list =  NULL;
        }
    }

    return (BCM_E_NONE);

cleanup:

    if (BCM_E_EXISTS
        == shr_idxres_list_elem_state(oc->group_pool, index)) {
        shr_idxres_list_free(oc->group_pool, index);
    }

    return (rv);

}

/*
 * Function:
 *     _bcm_kt2_oam_rmep_recover
 * Purpose:
 *     Recover OAM remote endpoint configuration.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     index       - (IN) Remote MEP hardware index.
 *     l3_entry    - (IN) RMEP view table entry pointer.
 *     scache      - (IN) Pointer to secondary storage buffer pointer.
 *     recovered_ver - (IN) warmboot recovery version.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_rmep_recover(int unit,
                          int index,
                          l3_entry_1_entry_t *l3_entry,
                          uint8 **scache,
                          uint16 recovered_ver)
{
    rmep_entry_t            rmep_entry;      /* Remote table entry.           */
    _bcm_oam_hash_data_t    *h_data_p;       /* Endpoint hash data pointer.   */
    _bcm_oam_control_t      *oc;             /* Pointer to control structure. */
    int                     rv;              /* Operation return status.      */
    bcm_oam_endpoint_info_t *ep_info = NULL; /* Endpoint information.         */
    _bcm_oam_hash_key_t     hash_key;        /* Hash key buffer for lookup.   */
    int                     ep_id;           /* Endpoint ID.                  */
    int                     rmep_index;      /* Index of RMEP table           */ 

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_rmep_recover.\n")));
    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    rmep_index = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, l3_entry, RMEP__RMEP_PTRf);

    if (recovered_ver >= BCM_WB_VERSION_1_3) {
        /* Recover EP index from scache */ 
        sal_memcpy(&ep_id, *scache + (_BCM_KT2_OAM_RMEP_SCACHE_LOCATION(rmep_index)),
                sizeof(bcm_oam_endpoint_t));
        rv = shr_idxres_list_reserve(oc->mep_pool, ep_id, ep_id);
        if (BCM_FAILURE(rv)) {
            rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Error: (EP_ID=%d) Index reserve "
                                " failed  - %s.\n"), ep_id, bcm_errmsg(rv)));
            return (rv);
        }

    } else {

        /* Allocate the next available endpoint index. */
        rv = shr_idxres_list_alloc(oc->mep_pool,
                (shr_idxres_element_t *)&ep_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Error: Endpoint alloc (EP=%d) - %s.\n"),
                     ep_id, bcm_errmsg(rv)));
            return (rv);
        }
    } 
    
    h_data_p = &oc->oam_hash_data[ep_id];
    if (1 == h_data_p->in_use) {
        return(BCM_E_INTERNAL);
    }

    /* 
     * Clear the hash data element contents before
     * storing values.
     */
    _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
    h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;
    h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
    h_data_p->pri_map_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->profile_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->dglp1_profile_index = _BCM_OAM_INVALID_INDEX; 
    h_data_p->dglp2_profile_index = _BCM_OAM_INVALID_INDEX; 
    h_data_p->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->outer_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->subport_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->inner_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->trunk_id = BCM_TRUNK_INVALID;
    h_data_p->gport = 0;
    /* Get RMEP table index from LMEP view entry. */
    h_data_p->remote_index
        = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, l3_entry, RMEP__RMEP_PTRf);

    /* Get RMEP table entry contents. */
    rv = READ_RMEPm(unit, MEM_BLOCK_ANY, h_data_p->remote_index, &rmep_entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: RMEP (index=%d) read failed  - %s.\n"),
                   h_data_p->remote_index, bcm_errmsg(rv)));
        goto cleanup;
    }

   /* Has to be a valid RMEP, return error otherwise. */
    if (!soc_RMEPm_field32_get(unit, &rmep_entry, VALIDf)) {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }
    
    h_data_p->ep_id = ep_id;

    h_data_p->is_remote = 1;

    h_data_p->flags |= BCM_OAM_ENDPOINT_REMOTE;

    h_data_p->group_index
        = soc_RMEPm_field32_get(unit, &rmep_entry, MAID_INDEXf);

    h_data_p->period
        = _bcm_kt2_oam_ccm_hw_encode_to_msecs
            ((int) soc_RMEPm_field32_get(unit, &rmep_entry,
                                         RMEP_RECEIVED_CCMf));

    h_data_p->name
        = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, l3_entry, RMEP__MEPIDf);

    h_data_p->local_tx_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->local_rx_index = _BCM_OAM_INVALID_INDEX;
    h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
    h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;

    rv = shr_idxres_list_reserve(oc->rmep_pool,
                                 h_data_p->remote_index,
                                 h_data_p->remote_index);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: (RMEP=%d) Index reserve failed  - %s.\n"),
                   h_data_p->remote_index, bcm_errmsg(rv)));
        rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
        goto cleanup;
    }

    h_data_p->in_use = 1;

    rv = _bcm_kt2_oam_group_ep_list_add(unit, h_data_p->group_index, ep_id);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    rv = _bcm_kt2_oam_endpoint_alloc(&ep_info);
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_group_ep_list_remove(unit, h_data_p->group_index, ep_id);
        goto cleanup;
    }

    bcm_oam_endpoint_info_t_init(ep_info);

    /* Set up endpoint information for key construction. */
    ep_info->group = h_data_p->group_index;
    ep_info->name = h_data_p->name;
    ep_info->gport = h_data_p->gport;
    ep_info->id= h_data_p->ep_id;
    /* Calculate hash key for hash table insert operation. */
    _bcm_kt2_oam_ep_hash_key_construct(unit, oc, ep_info, &hash_key);

    if ((recovered_ver < BCM_WB_VERSION_1_5) 
            || (!ETH_TYPE(h_data_p->type))) {
        rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, h_data_p);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Error: Hash table insert failed "
                                "EP=%d %s.\n"), h_data_p->ep_id, bcm_errmsg(rv)));
            _bcm_kt2_oam_group_ep_list_remove(unit, h_data_p->group_index, ep_id);
            goto cleanup;
        } else {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Info: Hash Tbl (EP=%d) inserted"
                                " - %s.\n"), ep_id, bcm_errmsg(rv)));
        }
    }

    /* Add the H/w index to logical index mapping for RMEP */
    oc->remote_endpoints[h_data_p->remote_index] = ep_info->id;

    sal_free(ep_info);

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_rmep_recover - done.\n")));

    return (rv);

cleanup:

    if (NULL != ep_info) {
        sal_free(ep_info);
    }

    /* Release the endpoint ID if in use. */
    if (BCM_E_EXISTS
        == shr_idxres_list_elem_state(oc->mep_pool, ep_id)) {
        shr_idxres_list_free(oc->mep_pool, ep_id);
    }

    /* Release the remote index if in use. */
    if ((1 == h_data_p->in_use)
        && (BCM_E_EXISTS
            == shr_idxres_list_elem_state(oc->rmep_pool,
                                          h_data_p->remote_index))) {
        shr_idxres_list_free(oc->rmep_pool, h_data_p->remote_index);

    }
    _BCM_OAM_HASH_DATA_CLEAR(h_data_p);

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_rmep_recover - error_done.\n")));
    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_port_lmep_rx_config_recover
 * Purpose:
 *     Recover OAM local endpoint Rx configuration for port based DownMEPs.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     index       - (IN) Port entry hardware index.
 *     port_entry  - (IN) port table entry pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_port_lmep_rx_config_recover(int unit,
                         int index,
                         port_tab_entry_t *port_entry,
                         uint8 **scache,
                         uint16 recovered_ver)
{

    _bcm_oam_hash_data_t    *h_data_p = NULL; /* Endpoint hash data pointer. */
    ma_index_entry_t        ma_idx_entry; /* MA_INDEX table entry.           */
    _bcm_oam_control_t      *oc;          /* Pointer to control structure.   */
    uint8                   mdl_bitmap;   /* Endpoint domain level bitmap.   */
    uint8                   mdl;          /* Maintenance domain level.       */
    _bcm_oam_hash_key_t     hash_key;     /* Hash key buffer for lookup.     */
    int                     rv, tmp_rv;   /* Operation return status.        */
    _bcm_gport_dest_t       gport_dest;   /* Gport specification.            */
    bcm_gport_t             gport;        /* Gport value.                    */
    int                     ep_id;        /* Endpoint ID.                    */
    bcm_module_t            my_modid;
    int                     stm_index = 0;
    source_trunk_map_table_entry_t stm_entry;
    bcm_oam_endpoint_info_t *ep_info = NULL; /* Endpoint information.         */
    int                     ma_offset = 0;
    int                     ma_idx_entry_count = 0;
    shr_idxres_list_handle_t pool = NULL;
    int                     ma_idx;       /* MA_TABLE index                  */
    int                     ma_base_idx = 0;
    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_port_lmep_rx_config_recover .\n")));
    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* recover port based DownMEPs */
    mdl_bitmap = soc_PORT_TABm_field32_get(unit, port_entry, MDL_BITMAPf); 
    for (mdl = 0; mdl < _BCM_OAM_EP_LEVEL_COUNT; mdl++) {

        if (mdl_bitmap & (1 << mdl)) {
            if (recovered_ver >= BCM_WB_VERSION_1_3) {

                BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
                BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit,
                            my_modid, index, &stm_index));
                SOC_IF_ERROR_RETURN(READ_SOURCE_TRUNK_MAP_TABLEm(unit, 
                            MEM_BLOCK_ANY, stm_index, &stm_entry));
                ma_base_idx = 
                    soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm_entry, 
                            MA_BASE_PTRf);

                ma_idx = ma_base_idx + ma_offset;
                sal_memcpy(&ep_id, *(scache) + (_BCM_KT2_OAM_RX_DOWNMEP_SCACHE_LOCATION(ma_idx)),
                        sizeof(bcm_oam_endpoint_t));
                rv = shr_idxres_list_reserve(oc->mep_pool, ep_id, ep_id);
                if (BCM_FAILURE(rv)) {
                    rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                    LOG_ERROR(BSL_LS_BCM_OAM, 
                            (BSL_META_U(unit,
                                        "OAM Error: (EP_ID=%d) Index reserve "
                                        " failed  - %s.\n"), ep_id, bcm_errmsg(rv)));
                    return (rv);
                }
            } else {
                /* Allocate the next available endpoint index. */
                rv = shr_idxres_list_alloc(oc->mep_pool,
                        (shr_idxres_element_t *)&ep_id);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Endpoint alloc (EP=%d) - %s.\n"),
                             ep_id, bcm_errmsg(rv)));
                    goto cleanup;
                }
            }
            h_data_p = &oc->oam_hash_data[ep_id];
            if (1 == h_data_p->in_use) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            /* 
             * Clear the hash data element contents before
             * storing values.
             */
            _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
            _BCM_OAM_HASH_DATA_HW_IDX_INIT(h_data_p);
            h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->pri_map_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->dglp1_profile_index = _BCM_OAM_INVALID_INDEX; 
            h_data_p->dglp2_profile_index = _BCM_OAM_INVALID_INDEX; 
            h_data_p->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->outer_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->subport_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->inner_tpid_profile_index = _BCM_OAM_INVALID_INDEX;


            h_data_p->ep_id = ep_id;
            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
            BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit,
                                              my_modid, index, &stm_index));
            SOC_IF_ERROR_RETURN(READ_SOURCE_TRUNK_MAP_TABLEm(unit, 
                                    MEM_BLOCK_ANY, stm_index, &stm_entry));
            h_data_p->ma_base_index = 
                    soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm_entry, 
                         MA_BASE_PTRf);
            h_data_p->local_rx_index = h_data_p->ma_base_index + ma_offset;
            ma_offset++;

            rv = READ_MA_INDEXm(unit, MEM_BLOCK_ANY, 
                                 h_data_p->local_rx_index, &ma_idx_entry);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: MA_INDEX(index:%d) "
                                      "read failed -" " %s.\n"),
                           h_data_p->local_rx_index, bcm_errmsg(rv)));
                goto cleanup;
            }

            h_data_p->in_use = 1;
            h_data_p->is_remote = 0;
            h_data_p->local_rx_enabled = 1;
            h_data_p->flags |= BCM_OAM_ENDPOINT_CCM_RX;
            h_data_p->group_index
                = soc_MA_INDEXm_field32_get(unit, &ma_idx_entry, MA_PTRf);

            h_data_p->profile_index
                = soc_MA_INDEXm_field32_get
                    (unit, &ma_idx_entry, OAM_OPCODE_CONTROL_PROFILE_PTRf);

            rv = soc_profile_mem_reference(unit, &oc->oam_opcode_control_profile,
                                       h_data_p->profile_index, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Profile entry recover failed "
                                      "(EP=%d) - %s.\n"), ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }
            h_data_p->name = 0xffff;
            h_data_p->level = mdl;
            h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->oam_domain = 0;

            /* Generic logical port type, construct gport from GLP.  */
            _bcm_gport_dest_t_init(&gport_dest);

            gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            gport_dest.modid = my_modid;
            gport_dest.port = index;
            h_data_p->sglp = index;

            rv = _bcm_esw_gport_construct(unit, &gport_dest, &gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Gport construct failed -"
                                       " %s.\n"), bcm_errmsg(rv)));
                 goto cleanup;
            }
            h_data_p->gport = gport;

            rv = _bcm_kt2_oam_group_ep_list_add(unit, h_data_p->group_index, ep_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_add(unit, h_data_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Remove mapping from ma_idx_to_ep_id list (EP=%d) -"
                                    " %s.\n"), unit, ep_id, bcm_errmsg(rv)));
                _bcm_kt2_oam_group_ep_list_remove(unit,
                                              h_data_p->group_index,
                                              ep_id);
                goto cleanup;
            }

            rv = _bcm_kt2_oam_endpoint_alloc(&ep_info);
            if (BCM_FAILURE(rv)) {
                _bcm_kt2_oam_group_ep_list_remove(unit,
                                              h_data_p->group_index,
                                              ep_id);
                goto cleanup;
            }

            bcm_oam_endpoint_info_t_init(ep_info);

            /* Set up endpoint information for key construction. */
            ep_info->group = h_data_p->group_index;
            ep_info->name = h_data_p->name;
            ep_info->gport = h_data_p->gport;
            ep_info->level = h_data_p->level;
            ep_info->vlan = 0;
            ep_info->inner_vlan = 0;

            if ((recovered_ver < BCM_WB_VERSION_1_5) 
                    || (!ETH_TYPE(h_data_p->type))) {
                /*
                 * Calculate hash key for hash table insert
                 * operation.
                 */
                _bcm_kt2_oam_ep_hash_key_construct(unit, oc, ep_info, &hash_key);


                rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Hash table insert"
                                        " (EP=%d) failed - %s.\n"),
                             h_data_p->ep_id, bcm_errmsg(rv)));

                    _bcm_kt2_oam_group_ep_list_remove(unit,
                            h_data_p->group_index,
                            ep_id);
                    goto cleanup;
                } else {
                    LOG_DEBUG(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Info: Hash Tbl (EP=%d)"
                                        " inserted  - %s.\n"), ep_id,
                             bcm_errmsg(rv)));
                }
            }
            sal_free(ep_info);
            ep_info = NULL;
        }
    } /* end of for loop */
    if (mdl_bitmap) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit, h_data_p,
                    &ma_idx_entry_count,&pool));
        rv = shr_idxres_list_reserve(pool,
                                     h_data_p->ma_base_index,
                                     h_data_p->ma_base_index + ma_idx_entry_count -1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: (LMEP=%d) MA Index reserve "
                                  "failed" "  - %s.\n"), h_data_p->ma_base_index,
                       bcm_errmsg(rv)));
            rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
            goto cleanup;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_rx_config_recover"
                           " - done.\n")));
    return (BCM_E_NONE);

cleanup:
    /* Lets not check return value of delete during cleanup */
    tmp_rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_delete(unit, h_data_p);
    if (BCM_FAILURE(tmp_rv)) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Remove from ma_idx to ep id map (EP=%d) "
                              "during cleanup - %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
    }

    if (NULL != ep_info) {
        sal_free(ep_info);
    }

    if (BCM_E_EXISTS
        == shr_idxres_list_elem_state(oc->mep_pool, ep_id)) {
        shr_idxres_list_free(oc->mep_pool, ep_id);
    }
    if (NULL != h_data_p
        && (BCM_E_EXISTS
            == shr_idxres_list_elem_state(pool,
                                          h_data_p->ma_base_index))) {
        shr_idxres_list_free(pool, h_data_p->ma_base_index);
    }
    if (NULL != h_data_p) {
        _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_rx_config_recover"
                           " - error_done.\n")));
    return (rv);

}

/*
 * Function:
 *     _bcm_kt2_oam_lmep_upmep_rx_config_recover
 * Purpose:
 *     Recover OAM local endpoint Rx configuration for UpMEPS.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     index       - (IN) Remote MEP hardware index.
 *     egr_mp_grp_entry - (IN) Egress MP group table entry pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_lmep_upmep_rx_config_recover(int unit,
                         int index,
                         egr_mp_group_entry_t *egr_mp_grp_entry,
                         uint8 **scache,
                         uint16 recovered_ver)
{
    _bcm_oam_hash_data_t    *h_data_p = NULL; /* Endpoint hash data pointer. */
    ma_index_entry_t        ma_idx_entry; /* MA_INDEX table entry.           */
    _bcm_oam_control_t      *oc;          /* Pointer to control structure.   */
    uint8                   mdl_bitmap;   /* Endpoint domain level bitmap.   */
    uint8                   mdl;          /* Maintenance domain level.       */
    int                     rv, tmp_rv;   /* Operation return status.        */
    _bcm_gport_dest_t       gport_dest;   /* Gport specification.            */
    bcm_gport_t             gport;        /* Gport value.                    */
    uint16                  glp;          /* Generic logical port value.     */
    int                     ep_id;        /* Endpoint ID.                    */
    _bcm_oam_hash_key_t     hash_key;     /* Hash key buffer for lookup.     */
    bcm_oam_endpoint_info_t *ep_info = NULL; /* Endpoint information.        */
    int                     lmep_type = 0;
    int                     timestamp_type = 0;
    int                     ma_offset = 0;
    int                     ma_base_idx = 0;
    int                     ma_idx      = 0;
    l3_entry_1_entry_t      l3_entry;
    int                     l3_index = -1;
    uint8                   mdl_mip = 0;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_upmep_rx_config_recover .\n")));
    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* recover UpMEPs */
    mdl_bitmap
        = soc_EGR_MP_GROUPm_field32_get(unit, egr_mp_grp_entry, MDL_BITMAP_ACTIVEf);
    
    for (mdl = 0; mdl < _BCM_OAM_EP_LEVEL_COUNT; mdl++) {
            
        if (mdl_bitmap & (1 << mdl)) {
            if (recovered_ver >= BCM_WB_VERSION_1_3) {

                ma_base_idx = soc_EGR_MP_GROUPm_field32_get(unit, egr_mp_grp_entry,
                        MA_BASE_PTRf);

                ma_idx = ma_base_idx + ma_offset;

                sal_memcpy(&ep_id, *(scache) + (_BCM_KT2_OAM_RX_UPMEP_SCACHE_LOCATION(ma_idx)), 
                        sizeof(bcm_oam_endpoint_t));
                rv = shr_idxres_list_reserve(oc->mep_pool, ep_id, ep_id);
                if (BCM_FAILURE(rv)) {
                    rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                    LOG_ERROR(BSL_LS_BCM_OAM, 
                            (BSL_META_U(unit,
                                        "OAM Error: (EP_ID=%d) Index reserve "
                                        " failed  - %s.\n"), ep_id, bcm_errmsg(rv)));
                    return (rv);
                }
            } else {
                /* Allocate the next available endpoint index. */
                rv = shr_idxres_list_alloc(oc->mep_pool,
                        (shr_idxres_element_t *)&ep_id);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Endpoint alloc (EP=%d) - %s.\n"),
                             ep_id, bcm_errmsg(rv)));
                    goto cleanup;
                }
            }
            h_data_p = &oc->oam_hash_data[ep_id];
            if (1 == h_data_p->in_use) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }

            /* 
             * Clear the hash data element contents before
             * storing values.
             */
            _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
            _BCM_OAM_HASH_DATA_HW_IDX_INIT(h_data_p);
            h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->pri_map_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->dglp1_profile_index = _BCM_OAM_INVALID_INDEX; 
            h_data_p->dglp2_profile_index = _BCM_OAM_INVALID_INDEX; 
            h_data_p->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->outer_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->subport_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->inner_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->trunk_id = BCM_TRUNK_INVALID;

            h_data_p->ep_id = ep_id;
            h_data_p->ma_base_index = 
                (soc_EGR_MP_GROUPm_field32_get(unit, egr_mp_grp_entry, 
                             MA_BASE_PTRf));
            h_data_p->local_rx_index = h_data_p->ma_base_index + ma_offset;
            ma_offset++;

            rv = READ_EGR_MA_INDEXm(unit, MEM_BLOCK_ANY, 
                                 h_data_p->local_rx_index, &ma_idx_entry);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: EGR_MA_INDEX(index:%d) "
                                      "read failed -" " %s.\n"),
                           h_data_p->local_rx_index, bcm_errmsg(rv)));
                goto cleanup;
            }

            h_data_p->flags |= BCM_OAM_ENDPOINT_UP_FACING;
            h_data_p->in_use = 1;
            h_data_p->is_remote = 0;
            h_data_p->local_rx_enabled = 1;
            h_data_p->flags |= BCM_OAM_ENDPOINT_CCM_RX;
            h_data_p->group_index
                = soc_EGR_MA_INDEXm_field32_get(unit, &ma_idx_entry, MA_PTRf);

            h_data_p->profile_index
                = soc_EGR_MA_INDEXm_field32_get
                    (unit, &ma_idx_entry, OAM_OPCODE_CONTROL_PROFILE_PTRf);

            rv = soc_profile_mem_reference(unit, &oc->egr_oam_opcode_control_profile,
                                       h_data_p->profile_index, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Profile entry recover failed "
                                      "(EP=%d) - %s.\n"), ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }

            h_data_p->dglp1_profile_index =
                     soc_MA_INDEXm_field32_get
                                (unit, &ma_idx_entry, DGLP1_PROFILE_PTRf);
            rv = soc_profile_mem_reference(unit, &oc->egr_oam_dglp_profile,
                                         h_data_p->dglp1_profile_index, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: DGLP profile ref count  "
                                      "increment failed (EP=%d) - %s.\n"),
                           h_data_p->ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }
            h_data_p->dglp2_profile_index =
                soc_MA_INDEXm_field32_get
                    (unit, &ma_idx_entry, DGLP1_PROFILE_PTRf);
            rv = soc_profile_mem_reference(unit, &oc->egr_oam_dglp_profile,
                                         h_data_p->dglp2_profile_index, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: DGLP profile ref count  "
                                      "increment failed (EP=%d) - %s.\n"),
                           h_data_p->ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }
#if defined(INCLUDE_BHH)
            h_data_p->cpu_qid
                = soc_EGR_MA_INDEXm_field32_get(unit, &ma_idx_entry, INT_PRIf);
#endif
            h_data_p->name = 0xffff;
            h_data_p->level = mdl;
            h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;
            /* recover counters */
            rv = _bcm_kt2_oam_rx_counter_recover(unit, EGR_MP_GROUPm, 
                                         (uint32 *)egr_mp_grp_entry, h_data_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM RX counter recover failed "
                                      "(EP=%d) - %s.\n"), ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }
            /* rx and tx counters are located at same index, but on diff mem */
            if (h_data_p->rx_ctr != _BCM_OAM_INVALID_INDEX) {
                h_data_p->tx_ctr = h_data_p->rx_ctr;
            }

            /* time stamp */
            timestamp_type = soc_EGR_MP_GROUPm_field32_get(unit,
                                            egr_mp_grp_entry, TIMESTAMP_TYPEf);  
            if (timestamp_type) {
                h_data_p->ts_format = timestamp_type;
            } 
            h_data_p->oam_domain = soc_EGR_MP_GROUPm_field32_get(unit,
                                                 egr_mp_grp_entry, KEY_TYPEf);

            switch (h_data_p->oam_domain) {
                case _BCM_OAM_DOMAIN_CVLAN:
                    h_data_p->flags |= BCM_OAM_ENDPOINT_MATCH_INNER_VLAN;
                    h_data_p->inner_vlan = soc_EGR_MP_GROUPm_field32_get(unit, 
                                                     egr_mp_grp_entry, CVIDf);
                    break;

                case _BCM_OAM_DOMAIN_SVLAN:
                    h_data_p->vlan = soc_EGR_MP_GROUPm_field32_get(unit, 
                                                     egr_mp_grp_entry, SVIDf);
                    break;

                case _BCM_OAM_DOMAIN_S_PLUS_CVLAN:
                    h_data_p->vlan = soc_EGR_MP_GROUPm_field32_get(unit, 
                                                     egr_mp_grp_entry, SVIDf);
                    h_data_p->inner_vlan = soc_EGR_MP_GROUPm_field32_get(unit, 
                                                       egr_mp_grp_entry, CVIDf);
                    h_data_p->flags |= BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN;
                    break;
                
                case _BCM_OAM_DOMAIN_VP:
                    h_data_p->vp = soc_EGR_MP_GROUPm_field32_get(unit, 
                                                     egr_mp_grp_entry, DVPf);
                    h_data_p->int_flags |= _BCM_OAM_ENDPOINT_IS_VP_BASED;
                    break;
                default :
                    break;
  
            }

            glp = soc_EGR_MP_GROUPm_field32_get(unit, egr_mp_grp_entry, DGLPf);

            lmep_type = soc_EGR_MP_GROUPm_field32_get(unit, egr_mp_grp_entry, 
                                                                 KEY_TYPEf);

            if (lmep_type == _BCM_OAM_DOMAIN_VP) { 
#if defined(INCLUDE_L3)
                if (_bcm_vp_used_get(unit, glp, _bcmVpTypeMim)) {

                    BCM_GPORT_MIM_PORT_ID_SET(h_data_p->gport, h_data_p->vp);

                } else if (_bcm_vp_used_get(unit, glp, _bcmVpTypeMpls)) {

                    BCM_GPORT_MPLS_PORT_ID_SET(h_data_p->gport, h_data_p->vp);

                } else {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Invalid Virtual Port (SVP=%d)"
                                           " - %s.\n"), h_data_p->vp,
                               bcm_errmsg(BCM_E_INTERNAL)));
                    return (BCM_E_INTERNAL);
                }
#endif
            } else {
                /*
                 * Generic logical port type, construct gport from GLP.
                 */
                h_data_p->dglp = glp;

                _bcm_gport_dest_t_init(&gport_dest);

                if (_BCM_OAM_GLP_TRUNK_BIT_GET(glp)) {
                    gport_dest.tgid = _BCM_OAM_GLP_TRUNK_ID_GET(glp);
                    gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    h_data_p->trunk_id =  _BCM_OAM_GLP_TRUNK_ID_GET(glp);
                } else {
                    gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    gport_dest.modid = _BCM_OAM_GLP_MODULE_ID_GET(glp);
                    gport_dest.port = _BCM_OAM_GLP_PORT_GET(glp);
                }

                rv = _bcm_esw_gport_construct(unit, &gport_dest, &gport);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Gport construct failed -"
                                           " %s.\n"), bcm_errmsg(rv)));
                    goto cleanup;
                }

                h_data_p->gport = gport;
            }
            h_data_p->dglp = glp;
            h_data_p->sglp = glp;

            sal_memset(&l3_entry, 0, sizeof(l3_entry_1_entry_t));
            L3_LOCK(unit);
            if (BCM_SUCCESS
                    (_bcm_kt2_oam_find_lmep(unit, h_data_p,
                                            &l3_index, &l3_entry))) {
                mdl_mip = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry,
                        LMEP__MDL_BITMAP_PASSIVEf);
            }
            L3_UNLOCK(unit);

            if (!(mdl_mip & (1 << h_data_p->level))) {
                /* Set the MIP flag */
                h_data_p->flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;
            }

            rv = _bcm_kt2_oam_group_ep_list_add(unit, h_data_p->group_index, ep_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_add(unit, h_data_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Add mapping from ma_idx_to_ep_id list (EP=%d) -"
                                    " %s.\n"), unit, ep_id, bcm_errmsg(rv)));
                _bcm_kt2_oam_group_ep_list_remove(unit,
                                              h_data_p->group_index,
                                              ep_id);
                goto cleanup;
            }


            rv = _bcm_kt2_oam_endpoint_alloc(&ep_info);
            if (BCM_FAILURE(rv)) {
                _bcm_kt2_oam_group_ep_list_remove(unit,
                                              h_data_p->group_index,
                                              ep_id);
                goto cleanup;
            }

            bcm_oam_endpoint_info_t_init(ep_info);

            /* Set up endpoint information for key construction. */
            ep_info->group = h_data_p->group_index;
            ep_info->name = h_data_p->name;
            ep_info->gport = h_data_p->gport;
            ep_info->level = h_data_p->level;
            ep_info->vlan = h_data_p->vlan;
            ep_info->inner_vlan = h_data_p->inner_vlan;
            ep_info->flags = h_data_p->flags;
            if ((recovered_ver < BCM_WB_VERSION_1_5) 
                    || (!ETH_TYPE(h_data_p->type))) {
                /*
                 * Calculate hash key for hash table insert
                 * operation.
                 */
                _bcm_kt2_oam_ep_hash_key_construct(unit, oc, ep_info, &hash_key);


                rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Hash table insert"
                                        " (EP=%d) failed - %s.\n"),
                             h_data_p->ep_id, bcm_errmsg(rv)));

                    _bcm_kt2_oam_group_ep_list_remove(unit,
                            h_data_p->group_index,
                            ep_id);
                    goto cleanup;
                } else {
                    LOG_DEBUG(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Info: Hash Tbl (EP=%d)"
                                        " inserted  - %s.\n"), ep_id,
                             bcm_errmsg(rv)));
                }
            }
            sal_free(ep_info);
            ep_info = NULL;

        }
    } /* end of for loop */
    if (mdl_bitmap) {
        rv = shr_idxres_list_reserve(oc->egr_ma_idx_pool,
                                         h_data_p->ma_base_index,
                                         h_data_p->ma_base_index + 7);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: (LMEP=%d) EGR MA Index reserve "
                                  "failed" "  - %s.\n"), h_data_p->remote_index,
                       bcm_errmsg(rv)));
            rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
            goto cleanup;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_rx_config_recover"
                           " - done.\n")));
    return (BCM_E_NONE);

cleanup:
    /* Lets not check return value of delete during cleanup */
    tmp_rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_delete(unit, h_data_p);
    if (BCM_FAILURE(tmp_rv)) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Remove from ma_idx to ep id map (EP=%d) "
                              "during cleanup - %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
    }

    if (NULL != ep_info) {
        sal_free(ep_info);
    }

    if (BCM_E_EXISTS
        == shr_idxres_list_elem_state(oc->mep_pool, ep_id)) {
        shr_idxres_list_free(oc->mep_pool, ep_id);
    }
    /* return rx & tx counters allocated if any */
    if (NULL != h_data_p) {
        rv =_bcm_kt2_oam_free_counter(unit, h_data_p);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LM counter block "
                                  "free failed (EP=%d) - %s.\n"),
                       ep_id, bcm_errmsg(rv)));
        }
    }
    if (NULL != h_data_p
        && (BCM_E_EXISTS
            == shr_idxres_list_elem_state(oc->egr_ma_idx_pool,
                                          h_data_p->ma_base_index))) {

        shr_idxres_list_free(oc->egr_ma_idx_pool, h_data_p->ma_base_index);

    }
    if (NULL != h_data_p) {
        _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
    }
    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_rx_config_recover"
                           " - error_done.\n")));
    return (rv);
}
/*
 * Function:
 *     _bcm_kt2_oam_lmep_downmep_rx_config_recover
 * Purpose:
 *     Recover OAM local endpoint Rx configuration for DownMEPS.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     index       - (IN) Remote MEP hardware index.
 *     l3_entry    - (IN) LMEP view table entry pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_lmep_downmep_rx_config_recover(int unit,
                                    int index,
                                    l3_entry_1_entry_t *l3_entry,
                                    uint8 **scache,
                                    uint16 recovered_ver)
{
    _bcm_oam_hash_data_t    *h_data_p = NULL; /* Endpoint hash data pointer. */
    ma_index_entry_t        ma_idx_entry; /* IA_INDEX table entry.           */
    _bcm_oam_control_t      *oc;          /* Pointer to control structure.   */
    uint8                   mdl_bitmap;   /* Endpoint domain level bitmap.   */
    uint8                   mdl;          /* Maintenance domain level.       */
    int                     rv, tmp_rv;   /* Operation return status.        */
    _bcm_gport_dest_t       gport_dest;   /* Gport specification.            */
    bcm_gport_t             gport;        /* Gport value.                    */
    uint16                  glp;          /* Generic logical port value.     */
    int                     ep_id;        /* Endpoint ID.                    */
    _bcm_oam_hash_key_t     hash_key;     /* Hash key buffer for lookup.     */
    bcm_oam_endpoint_info_t *ep_info = NULL;  /* Endpoint information.        */
    int                     lmep_type = 0;
    int                     timestamp_type = 0;
    int                     ma_offset = 0;
    int                     ma_idx_entry_count = 0;
    shr_idxres_list_handle_t pool = NULL;
    int                     ma_base_idx = 0;
    int                     ma_idx      = 0;
    egr_mp_group_entry_t    egr_mp_group;
    int                     mp_grp_index = 0;
    uint8                   mdl_mip = 0;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: "
                          "_bcm_kt2_oam_lmep_downmep_rx_config_recover .\n")));
    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* recover DownMEPs */
    mdl_bitmap = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, 
                                              l3_entry, LMEP__MDL_BITMAP_ACTIVEf);
    
    ma_base_idx = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, l3_entry, LMEP__MA_BASE_PTRf);
    
    for (mdl = 0; mdl < _BCM_OAM_EP_LEVEL_COUNT; mdl++) {

        if (mdl_bitmap & (1 << mdl)) {

            if (recovered_ver >= BCM_WB_VERSION_1_3) {
                ma_idx = ma_base_idx + ma_offset;
                sal_memcpy(&ep_id, *(scache) + (_BCM_KT2_OAM_RX_DOWNMEP_SCACHE_LOCATION(ma_idx)),
                        sizeof(bcm_oam_endpoint_t));
                rv = shr_idxres_list_reserve(oc->mep_pool, ep_id, ep_id);
                if (BCM_FAILURE(rv)) {
                    rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                    LOG_ERROR(BSL_LS_BCM_OAM, 
                            (BSL_META_U(unit,
                                        "OAM Error: (EP_ID=%d) Index reserve "
                                        " failed  - %s.\n"), ep_id, bcm_errmsg(rv)));
                    return (rv);
                }
            } else {
                /* Allocate the next available endpoint index. */
                rv = shr_idxres_list_alloc(oc->mep_pool,
                        (shr_idxres_element_t *)&ep_id);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Endpoint alloc (EP=%d) - %s.\n"),
                             ep_id, bcm_errmsg(rv)));
                    goto cleanup;
                }
            }
            h_data_p = &oc->oam_hash_data[ep_id];
            if (1 == h_data_p->in_use) {
                return(BCM_E_INTERNAL);
            }

            /* 
             * Clear the hash data element contents before
             * storing values.
             */
            _BCM_OAM_HASH_DATA_CLEAR(h_data_p);

            _BCM_OAM_HASH_DATA_HW_IDX_INIT(h_data_p);
            h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
            h_data_p->pri_map_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->outer_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->subport_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->inner_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
            h_data_p->trunk_id =  BCM_TRUNK_INVALID;



            h_data_p->ep_id = ep_id;
            h_data_p->ma_base_index = 
                soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, l3_entry,
                        LMEP__MA_BASE_PTRf);
            h_data_p->local_rx_index = h_data_p->ma_base_index + ma_offset;
            ma_offset++;

            rv = READ_MA_INDEXm
                    (unit, MEM_BLOCK_ANY, h_data_p->local_rx_index,
                     &ma_idx_entry);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: MA_INDEX (index:%d) read failed -"
                                       " %s.\n"), h_data_p->local_rx_index,
                           bcm_errmsg(rv)));
                goto cleanup;
            }

            h_data_p->in_use = 1;

            h_data_p->is_remote = 0;

            h_data_p->local_rx_enabled = 1;

            h_data_p->flags |= BCM_OAM_ENDPOINT_CCM_RX;

            h_data_p->group_index
                = soc_MA_INDEXm_field32_get(unit, &ma_idx_entry, MA_PTRf);
            h_data_p->int_pri = soc_MA_INDEXm_field32_get(unit, &ma_idx_entry, INT_PRIf);

            h_data_p->profile_index
                = soc_MA_INDEXm_field32_get
                    (unit, &ma_idx_entry, OAM_OPCODE_CONTROL_PROFILE_PTRf);
            rv = soc_profile_mem_reference(unit, &oc->oam_opcode_control_profile,
                                       h_data_p->profile_index, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Profile entry recover failed "
                                      "(EP=%d) - %s.\n"), ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }

            h_data_p->dglp1_profile_index =
                     soc_MA_INDEXm_field32_get
                                (unit, &ma_idx_entry, DGLP1_PROFILE_PTRf);
            rv = soc_profile_mem_reference(unit, &oc->ing_oam_dglp_profile,
                                         h_data_p->dglp1_profile_index, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: DGLP profile ref count  "
                                      "increment failed (EP=%d) - %s.\n"),
                           h_data_p->ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }
            h_data_p->dglp2_profile_index =
                soc_MA_INDEXm_field32_get
                    (unit, &ma_idx_entry, DGLP1_PROFILE_PTRf);
            rv = soc_profile_mem_reference(unit, &oc->ing_oam_dglp_profile,
                                         h_data_p->dglp2_profile_index, 1);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: DGLP profile ref count  "
                                      "increment failed (EP=%d) - %s.\n"),
                           h_data_p->ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }

            h_data_p->name = 0xffff;
            
            h_data_p->level = mdl;

            h_data_p->oam_domain = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                                 l3_entry,
                                                 LMEP__OAM_LMEP_KEY_SUBTYPEf);

            switch (h_data_p->oam_domain) {
                case _BCM_OAM_DOMAIN_CVLAN:
                    h_data_p->flags |= BCM_OAM_ENDPOINT_MATCH_INNER_VLAN;
                    h_data_p->inner_vlan = 
                          soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, 
                                                         l3_entry, LMEP__CVIDf);
                    break;

                case _BCM_OAM_DOMAIN_SVLAN:
                    h_data_p->vlan = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                                     l3_entry, LMEP__SVIDf);
                    break;

                case _BCM_OAM_DOMAIN_S_PLUS_CVLAN:
                    h_data_p->vlan = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                                     l3_entry, LMEP__SVIDf);
                    h_data_p->inner_vlan = 
                             soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, 
                                                         l3_entry, LMEP__CVIDf);
                    h_data_p->flags |= 
                                   BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN;
                    break;
                
                case _BCM_OAM_DOMAIN_VP:
                    h_data_p->vp = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, 
                                                     l3_entry, LMEP__SVPf);
                    h_data_p->int_flags |= _BCM_OAM_ENDPOINT_IS_VP_BASED;
                    break;
                default :
                    break;
  
            }

            rv = _bcm_kt2_oam_rx_counter_recover(unit, 
                       L3_ENTRY_IPV4_UNICASTm, (uint32 *)l3_entry, h_data_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LM counter recover failed "
                                      "(EP=%d) - %s.\n"), ep_id, bcm_errmsg(rv)));
                goto cleanup;
            }
            /* rx and tx counters are located at same index, but on diff mem */
            if (h_data_p->rx_ctr != _BCM_OAM_INVALID_INDEX) {
                h_data_p->tx_ctr = h_data_p->rx_ctr;
            }

            /* time stamp */
            timestamp_type = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit,
                                               l3_entry, LMEP__TIMESTAMP_TYPEf);
            if (timestamp_type) {
                h_data_p->ts_format = timestamp_type;
            } 
            glp = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, l3_entry, 
                                                         LMEP__SGLPf);

            lmep_type = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, l3_entry, 
                                                   LMEP__OAM_LMEP_KEY_SUBTYPEf);

            if (lmep_type == _BCM_OAM_DOMAIN_VP) { 
#if defined(INCLUDE_L3)
                if (_bcm_vp_used_get(unit, glp, _bcmVpTypeMim)) {

                    BCM_GPORT_MIM_PORT_ID_SET(h_data_p->gport, h_data_p->vp);

                } else if (_bcm_vp_used_get(unit, glp, _bcmVpTypeMpls)) {

                    BCM_GPORT_MPLS_PORT_ID_SET(h_data_p->gport, h_data_p->vp);

                } else {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Invalid Virtual Port (SVP=%d)"
                                           " - %s.\n"), h_data_p->vp,
                               bcm_errmsg(BCM_E_INTERNAL)));
                    return (BCM_E_INTERNAL);
                }
#endif
            } else {
                /*
                 * Generic logical port type, construct gport from GLP.
                 */
                _bcm_gport_dest_t_init(&gport_dest);

                if (_BCM_OAM_GLP_TRUNK_BIT_GET(glp)) {
                    gport_dest.tgid = _BCM_OAM_GLP_TRUNK_ID_GET(glp);
                    gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    h_data_p->trunk_id =  _BCM_OAM_GLP_TRUNK_ID_GET(glp);
                } else {
                    gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    gport_dest.modid = _BCM_OAM_GLP_MODULE_ID_GET(glp);
                    gport_dest.port = _BCM_OAM_GLP_PORT_GET(glp);
                }

                rv = _bcm_esw_gport_construct(unit, &gport_dest, &gport);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Gport construct failed -"
                                           " %s.\n"), bcm_errmsg(rv)));
                    goto cleanup;
                }

                h_data_p->gport = gport;
            }
            h_data_p->dglp = glp;
            h_data_p->sglp = glp;

            sal_memset(&egr_mp_group, 0, sizeof(egr_mp_group_entry_t));
            if (BCM_SUCCESS
                    (_bcm_kt2_oam_find_egr_lmep(unit, h_data_p,
                                                &mp_grp_index,
                                                &egr_mp_group))) {
                mdl_mip = soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_group,
                        MDL_BITMAP_PASSIVEf);
            }

            if (!(mdl_mip & (1 << h_data_p->level))) {
                /* Set the MIP flag */
                h_data_p->flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;
            }

            rv = _bcm_kt2_oam_group_ep_list_add(unit, h_data_p->group_index, ep_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_add(unit, h_data_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Add mapping from ma_idx_to_ep_id list (EP=%d) -"
                                    " %s.\n"), unit, ep_id, bcm_errmsg(rv)));
                _bcm_kt2_oam_group_ep_list_remove(unit,
                                              h_data_p->group_index,
                                              ep_id);
                goto cleanup;
            } 


            rv = _bcm_kt2_oam_endpoint_alloc(&ep_info);
            if (BCM_FAILURE(rv)) {
                _bcm_kt2_oam_group_ep_list_remove(unit,
                                              h_data_p->group_index,
                                              ep_id);
                goto cleanup;
            }

            bcm_oam_endpoint_info_t_init(ep_info);

            /* Set up endpoint information for key construction. */
            ep_info->group = h_data_p->group_index;
            ep_info->name = h_data_p->name;
            ep_info->gport = h_data_p->gport;
            ep_info->level = h_data_p->level;
            ep_info->vlan = h_data_p->vlan;
            ep_info->inner_vlan = h_data_p->inner_vlan;
            if ((recovered_ver < BCM_WB_VERSION_1_5) 
                    || (!ETH_TYPE(h_data_p->type))) {
                /*
                 * Calculate hash key for hash table insert
                 * operation.
                 */
                _bcm_kt2_oam_ep_hash_key_construct(unit, oc, ep_info, &hash_key);


                rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Hash table insert"
                                        " (EP=%d) failed - %s.\n"),
                             h_data_p->ep_id, bcm_errmsg(rv)));

                    _bcm_kt2_oam_group_ep_list_remove(unit,
                            h_data_p->group_index,
                            ep_id);
                    goto cleanup;
                } else {
                    LOG_DEBUG(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Info: Hash Tbl (EP=%d)"
                                        " inserted  - %s.\n"), ep_id,
                             bcm_errmsg(rv)));
                }
            }
            sal_free(ep_info);
            ep_info = NULL;
        }
    } /* end of for loop */
    
    if (mdl_bitmap) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(unit,
                    h_data_p, &ma_idx_entry_count, &pool));
        rv = shr_idxres_list_reserve(pool,
                                         h_data_p->ma_base_index,
                                         h_data_p->ma_base_index + ma_idx_entry_count -1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: (RMEP=%d) Index reserve failed"
                                   "  - %s.\n"), h_data_p->remote_index,
                       bcm_errmsg(rv)));
            rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
            goto cleanup;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_rx_config_recover"
                           " - done.\n")));
    return (BCM_E_NONE);

cleanup:
    tmp_rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_delete(unit, h_data_p);
    if (BCM_FAILURE(tmp_rv)) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Remove from ma_idx to ep id map (EP=%d) "
                              "during cleanup - %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
    }


    if (NULL != ep_info) {
        sal_free(ep_info);
    }

    if (BCM_E_EXISTS
        == shr_idxres_list_elem_state(oc->mep_pool, ep_id)) {
        shr_idxres_list_free(oc->mep_pool, ep_id);
    }
    /* return rx & tx counters allocated if any */
    if (NULL != h_data_p) {
        rv =_bcm_kt2_oam_free_counter(unit, h_data_p);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LM counter block "
                                  "free failed (EP=%d) - %s.\n"),
                       ep_id, bcm_errmsg(rv)));
        }
    }
    if (NULL != h_data_p && (BCM_E_EXISTS
            == shr_idxres_list_elem_state(pool,
                                          h_data_p->ma_base_index))) {

        shr_idxres_list_free(pool, h_data_p->ma_base_index);

    }
    if (NULL != h_data_p) {
        _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
    }
    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_rx_config_recover"
                           " - error_done.\n")));
    return (rv);
}

/*
 * Function:
 *      _bcm_kt2_oam_lmep_tx_config_recover
 * Purpose:
 *     Recover OAM local endpoint Tx configuration.
 * Parameters:
 *     unit        - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_lmep_tx_config_recover(int unit,
                                  uint8 **scache,
                                  uint16 recovered_ver)
{
    _bcm_gport_dest_t       gport_dest;   /* Gport specification.             */
    bcm_gport_t             gport;        /* Gport value.                     */
    int                     index;        /* Hardware index.                  */
    lmep_entry_t            lmep_entry;   /* LMEP table entry buffer.         */
    lmep_1_entry_t          lmep_1_entry; /* LMEP table entry buffer.         */
    maid_reduction_entry_t  maid_red_ent; /* MAID_REDUCTION table entry buf.  */
    _bcm_oam_hash_key_t     hash_key;     /* Hash key buffer for lookup.      */
    _bcm_oam_group_data_t   *g_info_p;    /* Group information pointer.       */
    _bcm_oam_control_t      *oc;          /* Pointer to control structure.    */
    bcm_module_t            modid;        /* Module ID.                       */
    bcm_port_t              port_id;      /* Port ID.                         */
    bcm_trunk_t             trunk_id;     /* Trunk ID.                        */
    uint32                  grp_idx;      /* Group index.                     */
    uint16                  glp;          /* Generic logical port.            */
    uint16                  vlan;         /* VLAN ID.                         */
    uint16                  inner_vlan;   /* Inner VLAN ID.                   */
    uint8                   level;        /* Maintenance domain level.        */
    _bcm_oam_ep_list_t      *cur;         /* Current head node pointer.       */
    int                     ep_id = -1;   /* Endpoint ID.                     */
    uint8                   match_found = 0; /* Matching endpoint found.      */
    int                     rv;           /* Operation return status.         */
    bcm_oam_endpoint_info_t *ep_info = NULL; /* Endpoint information.         */
    _bcm_oam_hash_data_t    *h_data_p = NULL; /* Endpoint hash data pointer.  */
    _bcm_oam_hash_data_t    sh_data; /* Endpoint hash data pointer. */
    uint32                  ccm_period = 0;
    egr_port_entry_t        egr_port_entry;
    uint8                   mdl_mip = 0;
    int                     trunk_index = -1;
    int                     tpid_index = 0;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_tx_config_recover.\n")));

    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    rv = _bcm_kt2_oam_endpoint_alloc(&ep_info);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /*
     * At this point, Remote MEP and Local MEP Rx config has been
     *  recovered. Now, recover the Tx config for Local MEPs.
     */
    for (index = 0; index < oc->lmep_count; index++) {

        /* Get the LMEP table entry. */
        rv = READ_LMEPm(unit, MEM_BLOCK_ANY, index, &lmep_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LMEP table read (index=%d) failed "
                                   "- %s.\n"), index, bcm_errmsg(rv)));
            goto cleanup;
        }

        /* Get the LMEP_1 table entry. */
        rv = READ_LMEP_1m(unit, MEM_BLOCK_ANY, index, &lmep_1_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: LMEP_1 table read (index=%d) failed "
                                   "- %s.\n"), index, bcm_errmsg(rv)));
            goto cleanup;
        }
        ccm_period = _bcm_kt2_oam_ccm_hw_encode_to_msecs
                      ((int) soc_LMEPm_field32_get(unit, &lmep_entry,
                                                   CCM_INTERVALf));
        if (ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
            continue;
        } 
        grp_idx = soc_LMEPm_field32_get(unit, &lmep_entry, MAID_INDEXf);

        rv = READ_MAID_REDUCTIONm(unit, MEM_BLOCK_ANY, grp_idx, &maid_red_ent);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: MAID_REDU read (GID=%d) failed "
                                   "- %s.\n"), grp_idx, bcm_errmsg(rv)));
            goto cleanup;
        }

        if (soc_MAID_REDUCTIONm_field32_get(unit, &maid_red_ent, VALIDf)) {

            /* Get pointer to group memory. */
            g_info_p = &oc->group_info[grp_idx];

            vlan = soc_LMEPm_field32_get(unit, &lmep_entry, VLAN_IDf);
            inner_vlan = soc_LMEPm_field32_get(unit, &lmep_entry, CVLAN_TAGf);
            /* Only extract the VLAN ID portion of the tag. */
            inner_vlan = inner_vlan & 0xfff;
            rv = READ_LMEP_1m(unit, MEM_BLOCK_ANY, index, &lmep_1_entry);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LMEP_1 table read (index=%d) "
                                      "failed " "- %s.\n"), index, bcm_errmsg(rv)));
                goto cleanup;
            }

            glp = soc_LMEP_1m_field32_get(unit, &lmep_1_entry, PP_PORTf);
            rv = _bcm_kt2_pp_port_to_modport_get(unit, glp,
                                                  &modid, &port_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            level = soc_LMEPm_field32_get(unit, &lmep_entry, MDLf);

            trunk_id = BCM_TRUNK_INVALID;
            if (vlan != 0 || inner_vlan != 0) {
                rv = bcm_esw_trunk_find(unit, modid, port_id, &trunk_id);
                if (BCM_FAILURE(rv)
                        && (BCM_E_NOT_FOUND != rv)) {
                    goto cleanup;
                }
            }

            _bcm_gport_dest_t_init(&gport_dest);

            if (BCM_TRUNK_INVALID != trunk_id) {
                gport_dest.tgid = trunk_id;
                gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
 
                rv = _bcm_esw_oam_lmep_tx_trunk_config_recover(unit, 
                        trunk_id, 
                        port_id,
                        &trunk_index);

                if(BCM_FAILURE(rv)) {
                    goto cleanup;
                }

            } else {
                gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                gport_dest.modid = _BCM_OAM_GLP_MODULE_ID_GET(glp);
                gport_dest.port = _BCM_OAM_GLP_PORT_GET(glp);
            }

            rv = _bcm_esw_gport_construct(unit, &gport_dest, &gport);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Gport construct failed"
                                       " - %s.\n"), bcm_errmsg(rv)));
                goto cleanup;
            }
            /* Get the endpoint list head pointer. */
            cur = *(g_info_p->ep_list);
            if (NULL != cur) {
                while (NULL != cur) {
                    h_data_p = cur->ep_data_p;
                    if (NULL == h_data_p) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "OAM Error: Group (GID=%d) NULL"
                                               " endpoint list.\n"), grp_idx));
                        rv = BCM_E_INTERNAL;
                        goto cleanup;
                    }

                    if (vlan == h_data_p->vlan &&
                        inner_vlan == h_data_p->inner_vlan 
                        && gport == h_data_p->gport
                        && level == h_data_p->level
                        && 1 == h_data_p->local_rx_enabled) {
                        match_found = 1;
                        break;
                    }
                    cur = cur->next;
                }
            }

            if (1 == match_found) {

                bcm_oam_endpoint_info_t_init(ep_info);

                /* Set up endpoint information for key construction. */
                ep_info->group = h_data_p->group_index;
                ep_info->name = h_data_p->name;
                ep_info->gport = h_data_p->gport;
                ep_info->level = h_data_p->level;
                ep_info->vlan = h_data_p->vlan;
                ep_info->inner_vlan = h_data_p->inner_vlan;
                ep_info->flags = h_data_p->flags;

                if ((recovered_ver < BCM_WB_VERSION_1_5) 
                        || (!ETH_TYPE(h_data_p->type))) {
                    /*
                     * Calculate hash key for hash table insert
                     * operation.
                     */
                    _bcm_kt2_oam_ep_hash_key_construct(unit, oc, ep_info, 
                            &hash_key);

                    /*
                     * Delete insert done by Local Rx endpoint recovery code.
                     * Endpoint name has been recovered and will result
                     * in a different hash index.
                     */
                    rv = shr_htb_find(oc->ma_mep_htbl, hash_key,
                            (shr_htb_data_t *)&sh_data, 1);
                    if (BCM_E_NOT_FOUND == rv) {
                        goto cleanup;
                    }
                }

            } else {
                
                if (recovered_ver >= BCM_WB_VERSION_1_3) {

                    sal_memcpy(&ep_id, 
                            *(scache) + _BCM_KT2_OAM_TX_MEP_SCACHE_LOCATION(index), 
                            sizeof(bcm_oam_endpoint_t));                

                    rv = shr_idxres_list_reserve(oc->mep_pool, ep_id, ep_id);
                    if (BCM_FAILURE(rv)) {
                        rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "OAM Error: (EP_ID=%d) Index reserve "
                                            " failed  - %s.\n"), ep_id, bcm_errmsg(rv)));
                        goto cleanup;
                    }
                } else {

                    /* Allocate the next available endpoint index. */
                    rv = shr_idxres_list_alloc(oc->mep_pool,
                            (shr_idxres_element_t *)&ep_id);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                (BSL_META_U(unit,
                                            "OAM Error: Endpoint alloc (EP=%d)"
                                            " - %s.\n"), ep_id, bcm_errmsg(rv)));
                        goto cleanup;
                    }
                }
                h_data_p = &oc->oam_hash_data[ep_id];
                if (1 == h_data_p->in_use) {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }

                _BCM_OAM_HASH_DATA_CLEAR(h_data_p);

                _BCM_OAM_HASH_DATA_HW_IDX_INIT(h_data_p);

                rv = _bcm_kt2_oam_group_ep_list_add(unit, grp_idx, ep_id);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Adding (EP=%d)"
                                           " to (GID=%d) failed - %s.\n"),
                               ep_id, grp_idx, bcm_errmsg(rv)));
                    goto cleanup;
                }

                h_data_p->ep_id = ep_id;
                h_data_p->group_index = grp_idx;
                h_data_p->local_rx_enabled = 0;
                h_data_p->vlan = vlan;
                h_data_p->inner_vlan = inner_vlan;
                h_data_p->gport = gport;
                h_data_p->level = level;
                if ((h_data_p->vlan == 0) && (h_data_p->inner_vlan == 0)) {
                    h_data_p->oam_domain = _BCM_OAM_DOMAIN_PORT; 
                } 
                h_data_p->tx_ctr = _BCM_OAM_INVALID_INDEX;
                h_data_p->rx_ctr = _BCM_OAM_INVALID_INDEX;
                h_data_p->pri_map_index = _BCM_OAM_INVALID_INDEX;
                h_data_p->profile_index = _BCM_OAM_INVALID_INDEX;
                h_data_p->dglp1_profile_index = _BCM_OAM_INVALID_INDEX; 
                h_data_p->dglp2_profile_index = _BCM_OAM_INVALID_INDEX; 
                h_data_p->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
                h_data_p->outer_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
                h_data_p->subport_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
                h_data_p->inner_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
                h_data_p->trunk_id =  BCM_TRUNK_INVALID;
            }

            h_data_p->src_pp_port = glp;
            h_data_p->dst_pp_port = glp;
            h_data_p->is_remote = 0;
            h_data_p->in_use = 1;
            h_data_p->local_tx_enabled = 1;
            h_data_p->local_tx_index = index;
            
            if((-1) != trunk_index) {
                h_data_p->trunk_index = trunk_index;
            }

            h_data_p->name
                = soc_LMEPm_field32_get(unit, &lmep_entry, MEPIDf);
            h_data_p->period
                = _bcm_kt2_oam_ccm_hw_encode_to_msecs
                      ((int) soc_LMEPm_field32_get(unit, &lmep_entry,
                                                   CCM_INTERVALf));

            if (BCM_PBMP_MEMBER(SOC_INFO(unit).general_pp_port_pbm, glp)) {
                h_data_p->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_VLAN_VP_UP_MEP_IN_HW;
            }
            if (h_data_p->oam_domain != _BCM_OAM_DOMAIN_PORT) {
                rv = _bcm_kt2_oam_port_table_key_update(unit, PORT_TABm, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Unable to increment key ref count "
                                           " - %s.\n"), bcm_errmsg(rv)));
                    goto cleanup;
                } 
                rv = _bcm_kt2_oam_port_table_key_update(unit, EGR_PORTm, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Unable to increment key ref count "
                                           " - %s.\n"), bcm_errmsg(rv)));
                    goto cleanup;
                }
            } else {
                rv = soc_mem_read(unit, EGR_PORTm, MEM_BLOCK_ANY,
                                  h_data_p->dst_pp_port, &egr_port_entry);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                             "OAM Error: EGR_PORT Table read failed"
                             " - %s.\n"), bcm_errmsg(rv)));
                    goto cleanup;
                }
                mdl_mip = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                        MDL_BITMAP_PASSIVEf);
                if (!(mdl_mip & (1 << h_data_p->level))) {
                    /* Set the MIP flag */
                    h_data_p->flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;
                }
            }

            /* For ethernet CCM TX endpoints, fetch TPIDs from HW.
             */
            if (h_data_p->vlan != 0) {
                tpid_index = soc_LMEPm_field32_get(unit, &lmep_entry, 
                        SVLAN_TPID_INDEXf);
                rv = _bcm_kt2_oam_tpid_get(unit, BCM_OAM_TPID_TYPE_OUTER,
                        tpid_index, &(h_data_p->outer_tpid));
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: Outer TPID fetch failed"
                                        " EP=%d %s.\n"), unit, ep_id, 
                             bcm_errmsg(rv)));
                    goto cleanup;
                }
                h_data_p->outer_tpid_profile_index = tpid_index;
                KT2_OAM_OUTER_TPID_REF_COUNT(unit, tpid_index)++;
            }

            if (h_data_p->inner_vlan != 0) {
                rv = _bcm_kt2_oam_tpid_get(unit, BCM_OAM_TPID_TYPE_INNER,
                        0, &(h_data_p->inner_tpid));
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: Inner TPID fetch failed"
                                        " EP=%d %s.\n"), unit, ep_id, 
                             bcm_errmsg(rv)));
                    goto cleanup;
                }
                h_data_p->inner_tpid_profile_index = 0;
                KT2_OAM_INNER_TPID_REF_COUNT(unit, 0)++;
            }

            if ((BCM_GPORT_IS_SUBPORT_PORT(h_data_p->gport)) &&
                    (_BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, h_data_p->gport))) {
                tpid_index = soc_LMEPm_field32_get(unit, &lmep_entry, 
                        SUBPORT_TAG_TPID_INDEXf);
                rv = _bcm_kt2_oam_tpid_get(unit, BCM_OAM_TPID_TYPE_SUBPORT,
                        tpid_index, &(h_data_p->subport_tpid));
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: Subport TPID fetch failed"
                                        " EP=%d %s.\n"), unit, ep_id, 
                             bcm_errmsg(rv)));
                    goto cleanup;
                }
                h_data_p->subport_tpid_profile_index = tpid_index;
                KT2_OAM_SUBPORT_TPID_REF_COUNT(unit, tpid_index)++;
            }

            rv = shr_idxres_list_reserve(oc->lmep_pool,
                                         h_data_p->local_tx_index,
                                         h_data_p->local_tx_index);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Tx index=%d reserve failed"
                                       " - %s.\n"), index, bcm_errmsg(rv)));

                _bcm_kt2_oam_group_ep_list_remove(unit,
                                              h_data_p->group_index,
                                              h_data_p->ep_id);

                rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                goto cleanup;
            }

            bcm_oam_endpoint_info_t_init(ep_info);

            /* Set up endpoint information for key construction. */
            ep_info->group = h_data_p->group_index;
            ep_info->name = h_data_p->name;
            ep_info->gport = h_data_p->gport;
            ep_info->level = h_data_p->level;
            ep_info->vlan = h_data_p->vlan;
            ep_info->inner_vlan = h_data_p->inner_vlan;
            ep_info->flags = h_data_p->flags;

            if ((recovered_ver < BCM_WB_VERSION_1_5) 
                    || (!ETH_TYPE(h_data_p->type))) {
                /*
                 * Calculate hash key for hash table insert
                 * operation.
                 */
                _bcm_kt2_oam_ep_hash_key_construct(unit, oc, ep_info, &hash_key);

                rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Hash table insert"
                                        " (EP=%d) failed - %s.\n"),
                             h_data_p->ep_id, bcm_errmsg(rv)));

                    _bcm_kt2_oam_group_ep_list_remove(unit,
                            h_data_p->group_index,
                            h_data_p->ep_id);

                    goto cleanup;
                } else {
                    LOG_DEBUG(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Info: Hash Tbl (EP=%d)"
                                        " inserted  - %s.\n"), h_data_p->ep_id,
                             bcm_errmsg(rv)));
                }
            }

        }

        match_found = 0;
        h_data_p = NULL;
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_tx_config_recover"
                           " - done.\n")));
    sal_free(ep_info);
    return (rv);

cleanup:

    if (NULL != ep_info) {
        sal_free(ep_info);
    }

    if (0 == match_found && NULL != h_data_p) {
        /* Return endpoint index to MEP pool. */
        if (BCM_E_EXISTS
            == shr_idxres_list_elem_state(oc->mep_pool, ep_id)) {
            shr_idxres_list_free(oc->mep_pool, ep_id);
        }
        _BCM_OAM_HASH_DATA_CLEAR(h_data_p);
    }

    if (BCM_E_EXISTS
        == shr_idxres_list_elem_state(oc->lmep_pool, index)) {
        shr_idxres_list_free(oc->lmep_pool, index);
    }

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: _bcm_kt2_oam_lmep_tx_config_recover"
                           " - error_done.\n")));
    return (rv);
}

/*
 * Function:
 *     _bcm_kt2_oam_wb_endpoints_recover
 * Purpose:
 *     Recover OAM local endpoint Rx configuration.
 * Parameters:
 *     unit        - Device unit number.
 *     stable_size - OAM module Level2 memory size.
 *     oam_scache  - Pointer to secondary storage buffer pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_wb_endpoints_recover(int unit,
                                  int stable_size,
                                  uint8 **oam_scache,
                                  uint16 recovered_ver)
{
    int                     index;        /* Hardware index.                  */
    uint32                  entry_count;  /* Max entries in L3_ENTRY_1/
                                                          EGR_MP_GROUP table. */
    l3_entry_1_entry_t      l3_entry;     /* L3 table entry.                  */
    _bcm_oam_control_t      *oc;          /* Pointer to control structure.    */
    int                     rv;           /* Operation return status.         */
    egr_mp_group_entry_t    egr_mp_grp_entry;/* Egr MP group tbl entry buffer */
    port_tab_entry_t        port_entry;   /* Port table entry buffer          */

    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get number of L3 table entries. */
    entry_count = soc_mem_index_count(unit, L3_ENTRY_IPV4_UNICASTm);

    /* Now get valid OAM endpoint entries. */
    for (index = 0; index < entry_count; index++) {

        rv = READ_L3_ENTRY_IPV4_UNICASTm(unit, MEM_BLOCK_ANY, index, &l3_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: L3_ENTRY (index=%d) read"
                                   " failed  - %s.\n"), index, bcm_errmsg(rv)));
                return (rv);
        }

        if (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry, VALIDf)) {

            switch (soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, &l3_entry, 
                                                          KEY_TYPEf)) {

                case SOC_MEM_KEY_L3_ENTRY_RMEP:
                    rv = _bcm_kt2_oam_rmep_recover(unit, index,
                                                   &l3_entry, oam_scache, recovered_ver);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "OAM Error: Remote endpoint"
                                               " (index=%d) reconstruct failed  - %s.\n"),
                                   index, bcm_errmsg(rv)));
                        return (rv);
                    }
                    break;

                /* Recover all downMEP's */
                case SOC_MEM_KEY_L3_ENTRY_LMEP:
                    rv = _bcm_kt2_oam_lmep_downmep_rx_config_recover(unit,
                                                             index,
                                                             &l3_entry,
                                                             oam_scache,
                                                             recovered_ver);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "OAM Error: Local endpoint DownMEP"
                                               " (index=%d) reconstruct failed  - %s.\n"),
                                   index, bcm_errmsg(rv)));
                        return (rv);
                    }
                    break;

                default:
                    /* Not an OAM entry. */
                    continue;
            }
        }
    }
    /* Recover all UpMEPs */
    /* Get number of EGR_MP_GROUP table entries. */
    entry_count = soc_mem_index_count(unit, EGR_MP_GROUPm);

    /* Now get valid OAM endpoint entries. */
    for (index = 0; index < entry_count; index++) {
        rv = READ_EGR_MP_GROUPm(unit, MEM_BLOCK_ANY, index, &egr_mp_grp_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EGR_MP_GROUP ENTRY (index=%d) read"
                                   " failed  - %s.\n"), index, bcm_errmsg(rv)));
                return (rv);
        }
        if (soc_EGR_MP_GROUPm_field32_get(unit, &egr_mp_grp_entry, VALIDf)) {
            rv = _bcm_kt2_oam_lmep_upmep_rx_config_recover(unit, index,
                                                             &egr_mp_grp_entry, oam_scache, recovered_ver);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Local endpoint UpMEP"
                                       " (index=%d) reconstruct failed  - %s.\n"),
                           index, bcm_errmsg(rv)));
                return (rv);
            }
        }
    } 

    /* recover endpoints from Port table */
    /* Get number of PORT_TABLE table entries. */
    entry_count = soc_mem_index_count(unit, PORT_TABm);
    /* Now get valid OAM endpoint entries. */
    for (index = 0; index < entry_count; index++) {
        rv = READ_PORT_TABm(unit, MEM_BLOCK_ANY, index, &port_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Port table ENTRY (index=%d) read"
                                   " failed  - %s.\n"), index, bcm_errmsg(rv)));
                return (rv);
        }
        if (soc_PORT_TABm_field32_get(unit, &port_entry, OAM_ENABLEf)) {
            rv = _bcm_kt2_oam_port_lmep_rx_config_recover(unit, index,
                                                             &port_entry,
                                                             oam_scache, recovered_ver);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Port based local endpoint "
                                       " (index=%d) reconstruct failed  - %s.\n"),
                           index, bcm_errmsg(rv)));
                return (rv);
            }
        }
    } 
    rv = _bcm_kt2_oam_lmep_tx_config_recover(unit, oam_scache, recovered_ver);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint Tx config recovery"
                               " failed  - %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *       bcm_kt2_oam_scache_alloc
 * Purpose:
 *     Allocate scache memory for OAM module 
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - OAM not support on this device.
 *     BCM_E_MEMORY  - Allocation failure
 *     CM_E_XXX     - Error code from bcm_XX_oam_init()
 *     BCM_E_NONE    - Success
 */
STATIC int
bcm_kt2_oam_scache_alloc(int unit) {
    _bcm_oam_control_t *oc;
    soc_scache_handle_t scache_handle;
    uint8 *oam_scache;
    int alloc_sz = 0;


    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    alloc_sz = BCM_OAM_GROUP_NAME_LENGTH * (oc->group_count);

    /* Number of oam groups */
    alloc_sz += sizeof(int);

    /* VFP group, IFP VP group, IFP GLP group */
    alloc_sz += 3 * sizeof(bcm_field_group_t);
    
    /* Allocate memory to store logical endpoint Ids */
    alloc_sz += (_BCM_KT2_OAM_ENDPOINT_ID_SCACHE_SIZE);

    /* Allocate memory to store oc->eth_oam_mp_group_vlan_key */
    alloc_sz += sizeof(uint8);

    /* FLAGS are stored for all ethernet OAM endpoints */
    alloc_sz += (sizeof(uint32) * oc->ep_count);

    /* Allocate memory to store all RMEP entries info */
    alloc_sz += (_BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_SIZE);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 1,
                        alloc_sz, &oam_scache, BCM_WB_DEFAULT_VERSION, NULL));
    return BCM_E_NONE;
}

/* Given h_data_p, this function fills the endpoint_info required for
 * hash_key generation and inserts it at appropriate location.
 *
 * oc->lock is already assumed to be taken by caller */
int _bcm_kt2_oam_hash_tbl_insert_hash_data_entry(int unit, _bcm_oam_hash_data_t *h_data_p)
{
    bcm_oam_endpoint_info_t ep_info;
    _bcm_oam_control_t  *oc = NULL;                  /* Pointer to OAM control structure. */
    _bcm_oam_hash_key_t     hash_key;                /* Hash key buffer for lookup.      */
    int rv = BCM_E_NONE;


    if (h_data_p == NULL) {
        return BCM_E_INTERNAL;
    }

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Initialize endpoint info structure. */
    bcm_oam_endpoint_info_t_init(&ep_info);

    /* Set up endpoint information for key construction. */
    ep_info.group = h_data_p->group_index;
    ep_info.name = h_data_p->name;
    ep_info.gport = h_data_p->gport;
    ep_info.level = h_data_p->level;
    ep_info.vlan = h_data_p->vlan;
    ep_info.inner_vlan = h_data_p->inner_vlan;
    ep_info.flags = h_data_p->flags;

    /* Construct hash key for lookup */
    _bcm_kt2_oam_ep_hash_key_construct(unit, oc, &ep_info, &hash_key);

    /* Insert entry from hash table. */
    rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, h_data_p);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Hash table insert failed "
                               "EP=%d %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
    } else {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Info: Hash Tbl (EP=%d) inserted"
                               " - %s.\n"), unit, h_data_p->ep_id, bcm_errmsg(rv)));
    }

    return rv;
}

/*
 * Function:
 *    _bcm_kt2_oam_wb_rmep_endpoints_info_recover
 * Purpose:
 *     Recover OAM remote endpoint entry configuration.
 * Parameters:
 *     unit           - Device unit number.
 *     rmep_scache    - Pointer to rmep entry scache.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_wb_rmep_endpoints_info_recover(int unit,
                                            uint8 **rmep_scache)
{
    _bcm_oam_control_t      *oc = NULL;   /* Pointer to control structure.    */
    _bcm_oam_hash_data_t    *h_data_p = NULL;
    uint8                   *rmep_entry_info; /* Pointer to RMEP Enry Info    */
    uint8                   mdl = 0;
    int                     ep_id;            /* Endpoint ID.                  */
    int                     idx = 0;
    int                     rv;           /* Operation return status.         */

    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    for (idx = 0; idx < oc->ep_count; idx++) {
        h_data_p = &oc->oam_hash_data[idx];
        if (ETH_TYPE(h_data_p->type)
            && (h_data_p->in_use)
            && (h_data_p->is_remote)) {

            /* Recover rmep entry sync info from rmep_scache */
            rmep_entry_info = *rmep_scache + (_BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_LOCATION(h_data_p->remote_index));
            /* Recover EP ID from rmep_scache */
            sal_memcpy(&ep_id, rmep_entry_info, sizeof(bcm_oam_endpoint_t));

            if (ep_id != h_data_p->ep_id) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Error: (RMEP EP =%d) Entry Info recovery failed.\n"),
                      h_data_p->ep_id));
                return (BCM_E_FAIL);
            }

            rmep_entry_info += sizeof(bcm_oam_endpoint_t);
            /* Recover MDL from rmep_scache */
            sal_memcpy(&mdl, rmep_entry_info, sizeof(uint8));
            h_data_p->level = mdl;
            rmep_entry_info += sizeof(uint8);
            /* Recover VLAN from rmep_scache */
            sal_memcpy(&h_data_p->vlan, rmep_entry_info, sizeof(bcm_vlan_t));
            rmep_entry_info += sizeof(bcm_vlan_t);
            /* Recover GPORT from rmep_scache */
            sal_memcpy(&h_data_p->gport, rmep_entry_info, sizeof(bcm_gport_t));
            rmep_entry_info += sizeof(bcm_gport_t);

            rv = _bcm_kt2_oam_hash_tbl_insert_hash_data_entry(unit, h_data_p);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: Endpoint %d hash tbl insert failed  - %s.\n"),
                    unit, h_data_p->ep_id, bcm_errmsg(rv)));
                return (rv);
           }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_reinit
 * Purpose:
 *     Reconstruct OAM module software state.
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_reinit(int unit)
{
    uint32              group_count = 0; /* Stored OAM group count.           */
    int                 stable_size = 0; /* Secondary storage size.           */
    uint8               *oam_scache;     /* Pointer to scache memory.         */
    soc_scache_handle_t scache_handle;   /* Scache memory handler.            */
    _bcm_oam_control_t  *oc;             /* Pointer to OAM control structure. */
    int                 rv;              /* Operation return status.          */
    uint16              recovered_ver = 0;
    int                 realloc_size = 0;
    int                 idx = 0;
    _bcm_oam_hash_data_t    *h_data_p = NULL;       /* Endpoint hash data pointer.   */

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: OAM warm boot recovery.....\n")));

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM, 0);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &oam_scache, BCM_WB_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            bcm_kt2_oam_scache_alloc(unit);
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NONE;
        } else if (BCM_FAILURE(rv)) {
            goto cleanup;
        } else {
            if (recovered_ver < BCM_WB_VERSION_1_2 && MA_INDEX_ALLOC_SCHEME_SEPARATE_POOL) {
                LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Warn:"
                                " Warm upgrade from a older version which does not support "
                                "flexible endpoint hw index allocation. Reverting to older "
                                "scheme.\n")));
                oc->ma_idx_alloc_scheme = _BCM_OAM_MA_IDX_ALLOC_COMBINED_POOL;
                /* Destroy the previous allocation and re-allocate based on this scheme */
                rv = _bcm_kt2_oam_ma_idx_pool_destroy(oc);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
                rv = _bcm_kt2_oam_ma_idx_pool_create(oc);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }
            }

            /* Recover the FP groups */
            sal_memcpy(&oc->vfp_group, oam_scache, sizeof(bcm_field_group_t));
            oam_scache += sizeof(bcm_field_group_t);

            sal_memcpy(&oc->fp_vp_group, oam_scache, sizeof(bcm_field_group_t));
            oam_scache += sizeof(bcm_field_group_t);

            sal_memcpy(&oc->fp_glp_group, oam_scache, sizeof(bcm_field_group_t));
            oam_scache += sizeof(bcm_field_group_t);

            /* Recover the OAM groups */
            sal_memcpy(&group_count, oam_scache, sizeof(int));
            oam_scache += sizeof(int);
            
        }
    } else {
        rv = BCM_E_NONE;
        goto cleanup;
    }

    rv = _bcm_kt2_oam_wb_group_recover(unit, stable_size, &oam_scache);
    
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Group recovery failed  - %s.\n"),
                   bcm_errmsg(rv)));
        goto cleanup;
    }

    rv = _bcm_kt2_oam_wb_endpoints_recover(unit, stable_size, &oam_scache, recovered_ver);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint recovery failed  - %s.\n"),
                   bcm_errmsg(rv)));
        goto cleanup;
    }
    
    if(recovered_ver >= BCM_WB_VERSION_1_3) {
        oam_scache += (_BCM_KT2_OAM_ENDPOINT_ID_SCACHE_SIZE);
    }

    if (recovered_ver >= BCM_WB_VERSION_1_4) {
        sal_memcpy(&oc->eth_oam_mp_group_vlan_key, oam_scache, sizeof(uint8));
        oam_scache += sizeof(uint8);
    } else {
        oc->eth_oam_mp_group_vlan_key =
                               _BCM_KT2_ETH_OAM_MP_GROUP_KEY_DOMAIN_INDEPENDANT;
    }
    if (recovered_ver >= BCM_WB_VERSION_1_5) {
        for (idx = 0; idx < oc->ep_count; idx++) {
            h_data_p = &oc->oam_hash_data[idx];
            if (ETH_TYPE(h_data_p->type) && (h_data_p->in_use)) {
                sal_memcpy(&(h_data_p->flags), oam_scache, sizeof(uint32));
                if ((recovered_ver >= BCM_WB_VERSION_1_6)
                    && (h_data_p->is_remote)) {
                        oam_scache += sizeof(uint32);
                        continue;
                }
                rv = _bcm_kt2_oam_hash_tbl_insert_hash_data_entry(unit, h_data_p);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: Endpoint %d hash tbl insert failed  - %s.\n"),
                             unit, h_data_p->ep_id, bcm_errmsg(rv)));
                    goto cleanup;
                }
            }
            oam_scache += sizeof(uint32);
        }
    }

    if (recovered_ver >= BCM_WB_VERSION_1_6) {
        rv = _bcm_kt2_oam_wb_rmep_endpoints_info_recover(unit, &oam_scache);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: RMEP Endpoint info recovery failed  - %s.\n"),
                   bcm_errmsg(rv)));
            goto cleanup;
        }
        oam_scache += (_BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_SIZE);
    }

    /* In BCM_WB_VERSION_1_1 onwards we allocate memory for max OAM groups and
       not just the groups created, as such while upgrading from 
       BCM_WB_VERSION_1_0 to BCM_WB_VERSION_1_1 or greater we must add the 
       differential.
    */
    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        if (recovered_ver < BCM_WB_VERSION_1_1) {
            realloc_size += (BCM_OAM_GROUP_NAME_LENGTH * (oc->group_count - group_count));
        }

        /* BCM_WB_VERSION_1_3 onwards addition memory is used to store
         * software endpoint ids */
        if(recovered_ver < BCM_WB_VERSION_1_3) {
            realloc_size += (_BCM_KT2_OAM_ENDPOINT_ID_SCACHE_SIZE);

        }

        if (recovered_ver < BCM_WB_VERSION_1_4) {
            realloc_size += sizeof(uint8);
        }

        if (recovered_ver < BCM_WB_VERSION_1_5) {
            realloc_size += (sizeof(uint32) * oc->ep_count);
        }

        if (recovered_ver < BCM_WB_VERSION_1_6) {
            realloc_size += (_BCM_KT2_OAM_RMEP_ENTRY_INFO_SCACHE_SIZE);
        }

        /* Added the check to avoid any like-to-like recovery where realloc is not required */
        if (realloc_size) {
            SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM, 0);
            rv = soc_scache_realloc(unit, scache_handle, realloc_size);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: scache alloc failed"
                                    "  - %s.\n"),
                         unit, bcm_errmsg(rv)));
                goto cleanup;
            }
        }
    }

 
cleanup:
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

#if defined(INCLUDE_BHH)
/*
 * Function:
 *     _bcm_kt2_oam_bhh_cos_map_recover
 * Purpose:
 *     Reconstruct CoS Map config done for BHH
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_oam_bhh_cos_map_recover(int unit)
{
    int                    rv = BCM_E_NONE;
    _bcm_oam_control_t     *oc;
    int                    index;
    int                    cosq_map_size;
    bcm_rx_reasons_t       reasons, reasons_mask;
    bcm_rx_reasons_t       ach_error_reasons, invalid_error_reasons;
    bcm_rx_reasons_t       bhh_lb_reasons;
    uint8                  int_prio, int_prio_mask;
    uint32                 packet_type, packet_type_mask;
    bcm_cos_queue_t        cosq;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NONE;
    }

    rv = bcm_esw_rx_cosq_mapping_size_get(unit, &cosq_map_size);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "BHH Error:hw init. cosq maps size %s.\n"),
                   bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    BCM_RX_REASON_CLEAR_ALL(ach_error_reasons);
    BCM_RX_REASON_SET(ach_error_reasons, bcmRxReasonBHHOAM);
    BCM_RX_REASON_SET(ach_error_reasons, bcmRxReasonOAMCCMSlowpath);

    BCM_RX_REASON_CLEAR_ALL(invalid_error_reasons);
    BCM_RX_REASON_SET(invalid_error_reasons, bcmRxReasonBHHOAM);
    BCM_RX_REASON_SET(invalid_error_reasons, bcmRxReasonOAMLMDM);

    BCM_RX_REASON_CLEAR_ALL(bhh_lb_reasons);
    BCM_RX_REASON_SET(bhh_lb_reasons, bcmRxReasonBHHOAM);
    BCM_RX_REASON_SET(bhh_lb_reasons, bcmRxReasonOAMSlowpath);

    oc->cpu_cosq_ach_error_index       = -1;
    oc->cpu_cosq_invalid_error_index   = -1;
    oc->bhh_lb_index                   = -1;
    for (index = 0; index < cosq_map_size; index++) {
        rv = bcm_esw_rx_cosq_mapping_get(unit, index,
                                         &reasons, &reasons_mask,
                                         &int_prio, &int_prio_mask,
                                         &packet_type, &packet_type_mask,
                                         &cosq);
        if (rv == BCM_E_NONE) {

            if (BCM_RX_REASON_EQ(reasons, ach_error_reasons) &&
                BCM_RX_REASON_EQ(reasons_mask, ach_error_reasons)) {
                oc->cpu_cosq_ach_error_index = index;
                oc->cpu_cosq = cosq;
            } else if (BCM_RX_REASON_EQ(reasons, invalid_error_reasons) &&
                       BCM_RX_REASON_EQ(reasons_mask, invalid_error_reasons)) {
                oc->cpu_cosq_invalid_error_index = index;
            } else if (BCM_RX_REASON_EQ(reasons, bhh_lb_reasons) &&
                       BCM_RX_REASON_EQ(reasons_mask, bhh_lb_reasons)) {
                oc->bhh_lb_index = index;
            }
        }
        if( -1 != oc->cpu_cosq_ach_error_index         &&
            -1 != oc->cpu_cosq_invalid_error_index     &&
            -1 != oc->bhh_lb_index) {
            break;
        }
    }

    if (index >= cosq_map_size) {
        LOG_VERBOSE(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                            "BHH Error: Could not recover cos map entry \n")));
    }

    _BCM_OAM_UNLOCK(oc);
    return rv;
}
#endif /* INCLUDE_BHH */
#endif /* BCM_WARM_BOOT_SUPPORT */

STATIC int
_bcm_kt2_oam_ma_idx_num_entries_and_pool_get(int unit,
                       _bcm_oam_hash_data_t *h_data_p,
                              int *num_ma_idx_entries,
                       shr_idxres_list_handle_t *pool)
{
    _bcm_oam_control_t *oc = NULL; 

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if ((h_data_p == NULL) || (h_data_p->in_use == 0)) {
        return BCM_E_PARAM;
    }

    switch (oc->ma_idx_alloc_scheme) {
        case _BCM_OAM_MA_IDX_ALLOC_COMBINED_POOL:
            *num_ma_idx_entries = 8;
            *pool = oc->ma_idx_pool;
            return BCM_E_NONE;
            break;
        case _BCM_OAM_MA_IDX_ALLOC_SEPARATE_POOL:
            if (h_data_p->type == bcmOAMEndpointTypeEthernet) {
                *num_ma_idx_entries = 8;
                *pool = oc->ma_idx_pool;
            } else if (BHH_EP_TYPE(h_data_p)) {
                *num_ma_idx_entries = 1;
                *pool = oc->mpls_oam_ma_idx_pool;
            }
            return BCM_E_NONE;
            break;
        default:
            /* Should not hit */
            break;
    }
    return BCM_E_INTERNAL; 
}

STATIC int
_bcm_kt2_oam_ma_idx_pool_create(_bcm_oam_control_t *oc)
{
    int rv = BCM_E_NONE;

    if (MA_INDEX_ALLOC_SCHEME_COMBINED_POOL) {
        rv = shr_idxres_list_create(&oc->ma_idx_pool, 0, 
                oc->ma_idx_count - 1,
                0, oc->ma_idx_count -1, "ma_idx pool");
        if (BCM_FAILURE(rv)) {
            return (rv);

        }
    } else if (MA_INDEX_ALLOC_SCHEME_SEPARATE_POOL) {
        if (_BCM_ETH_OAM_NUM_SESSIONS != 0) {
            rv = shr_idxres_list_create(&oc->ma_idx_pool, 0, _BCM_ETH_OAM_NUM_SESSIONS-1,
                    0, _BCM_ETH_OAM_NUM_SESSIONS-1, "ma_idx pool");
            if (BCM_FAILURE(rv)) {
                return (rv);

            }
        }

        if (_BCM_MPLS_OAM_NUM_SESSIONS != 0) {
            rv = shr_idxres_list_create(&oc->mpls_oam_ma_idx_pool, 
                    _BCM_ETH_OAM_NUM_SESSIONS, 
                    _BCM_ETH_OAM_NUM_SESSIONS + _BCM_MPLS_OAM_NUM_SESSIONS-1,
                    _BCM_ETH_OAM_NUM_SESSIONS, 
                    _BCM_ETH_OAM_NUM_SESSIONS + _BCM_MPLS_OAM_NUM_SESSIONS-1,
                    "mpls oam ma_idx pool");
            if (BCM_FAILURE(rv)) {
                return (rv);

            }
        }
    } else {
        return (BCM_E_INTERNAL);

    }
    return rv;
}

STATIC int
_bcm_kt2_oam_ma_idx_pool_destroy(_bcm_oam_control_t *oc)
{
    if (NULL != oc->ma_idx_pool) {
        shr_idxres_list_destroy(oc->ma_idx_pool);
        oc->ma_idx_pool = NULL;
    }
    if (NULL != oc->mpls_oam_ma_idx_pool) {
        shr_idxres_list_destroy(oc->mpls_oam_ma_idx_pool);
        oc->mpls_oam_ma_idx_pool = NULL;
    }
    return BCM_E_NONE;
}


/* * * * * * * * * * * * * * * * * * * *
 *            OAM BCM APIs             *
 */
/*
 * Function: bcm_kt2_oam_init
 *
 * Purpose:
 *     Initialize OAM module.
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - OAM not support on this device.
 *     BCM_E_MEMORY  - Allocation failure
 *     CM_E_XXX     - Error code from bcm_XX_oam_init()
 *     BCM_E_NONE    - Success
 */
int
bcm_kt2_oam_init(int unit)
{
    _bcm_oam_control_t *oc = NULL;  /* OAM control structure.     */
    int             rv;             /* Operation return value.    */
    uint32          size;           /* Size of memory allocation. */
    bcm_port_t      port;           /* Port number.               */
    int             index = 0;
    bcm_oam_endpoint_t   ep_index;  /* Endpoint index.               */
    bcm_pbmp_t      all_pbmp;
#if defined(INCLUDE_BHH)
    int uc;
    uint8 carrier_code[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    char *carrier_code_str;
    uint32 node_id = 0;
    bhh_sdk_msg_ctrl_init_t msg_init;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int priority;
    int ukernel_not_ready = 0;
    uint8 bhh_consolidate_final_event = 0;
#endif
    /* Ensure that the unit has OAM support. */
    if (!soc_feature(unit, soc_feature_oam)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: OAM not supported \n")));
        return (BCM_E_UNAVAIL);
    }

    /* Detach first if the module has been previously initialized. */
    if (NULL != _kt2_oam_control[unit]) {
        _kt2_oam_control[unit]->init = FALSE;
        rv = bcm_kt2_oam_detach(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Module deinit - %s.\n"),
                       bcm_errmsg(rv)));
            return (rv);
        }
    }

    /* Allocate OAM control memeory for this unit. */
    _BCM_OAM_ALLOC(oc, _bcm_oam_control_t, sizeof (_bcm_oam_control_t),
               "OAM control");
    if (NULL == oc) {
        return (BCM_E_MEMORY);
    }

    if (soc_feature(unit, soc_feature_bhh)) {
#ifdef INCLUDE_BHH
        /*
         * Initialize uController side
         */

        /*
         * Start BHH application in BTE (Broadcom Task Engine) uController,
         * if not already running (warm boot).
         * Determine which uController is running BHH  by choosing the first
         * uC that returns successfully.
         */
        for (uc = 0; uc < SOC_INFO(unit).num_ucs; uc++) {
            if (!soc_uc_in_reset(unit, uc)) {
                rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_BHH,
                                           _BHH_UC_MSG_TIMEOUT_USECS,
                                           BHH_SDK_VERSION,
                                           BHH_UC_MIN_VERSION,
                                           _bcm_oam_bhh_appl_callback, NULL);
                if (SOC_E_NONE == rv) {
                    /* BHH started successfully */
                    oc->uc_num = uc;
                    break;
                }
            }
        }

        if (uc >= SOC_INFO(unit).num_ucs) {  /* Could not find or start BHH appl */
            ukernel_not_ready = 1;
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "uKernel Not Ready, bhh not started\n")));
        }
#endif
    }


    /* Get number of endpoints and groups supported by this unit. */
    rv = _bcm_kt2_oam_group_endpoint_count_init(unit, oc);
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    if (soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
    oc->unit = unit;

    oc->ukernel_not_ready = ukernel_not_ready;
    /* Get SOC properties for BHH */
    oc->bhh_endpoint_count = soc_property_get(unit, spn_BHH_NUM_SESSIONS, 128);
    oc->bhh_max_encap_length =
                            soc_property_get(unit,
                                             spn_BHH_ENCAP_MAX_LENGTH,
                                             _BCM_OAM_BHH_DEFAULT_ENCAP_LENGTH);


    if(oc->bhh_max_encap_length > _BCM_OAM_BHH_MAX_ENCAP_LENGTH) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Invalid max encap_length=%u\n"),
                              oc->bhh_max_encap_length));
        return BCM_E_CONFIG;
    }

    carrier_code_str = soc_property_get_str(unit, spn_BHH_CARRIER_CODE);
    if (carrier_code_str != NULL) {
        /*
         * Note that the carrier code is specified in colon separated
         * MAC address format.
         */
        if (_shr_parse_macaddr(carrier_code_str, carrier_code) < 0) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (BCM_E_INTERNAL);
        }
    }

    node_id = soc_property_get(unit, spn_BHH_NODE_ID, 0);
    bhh_consolidate_final_event = soc_property_get(unit, 
                                           spn_BHH_CONSOLIDATED_FINAL_EVENT, 0);
    oc->ep_count += oc->bhh_endpoint_count;

    if(oc->ep_count >= _BCM_OAM_KATANA2_ENDPOINT_MAX) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: OAM EP count %d not supported \n"),
                   oc->ep_count));
        _bcm_kt2_oam_control_free(unit, oc);
        return (BCM_E_PARAM);
    }
#endif /* INCLUDE_BHH */
    }

    _BCM_MPLS_OAM_NUM_SESSIONS = soc_property_get(unit, spn_MPLS_OAM_NUM_SESSIONS,
            _BCM_MPLS_OAM_NUM_SESSIONS_INVALID);

    if(_BCM_MPLS_OAM_NUM_SESSIONS ==
            _BCM_MPLS_OAM_NUM_SESSIONS_INVALID) {
        /* Entry does not exist. Revert to old scheme */
        oc->ma_idx_alloc_scheme = _BCM_OAM_MA_IDX_ALLOC_COMBINED_POOL;
    } else if (_BCM_MPLS_OAM_NUM_SESSIONS > soc_mem_index_max(unit, MA_INDEXm)){
        /* If it is greater than max index of MA_INDEX table,
         * set it to max_index value and set it to newer index alloc scheme.
         */
        _BCM_MPLS_OAM_NUM_SESSIONS = soc_mem_index_max(unit, MA_INDEXm);
        _BCM_ETH_OAM_NUM_SESSIONS  = 0;
        oc->ma_idx_alloc_scheme = _BCM_OAM_MA_IDX_ALLOC_SEPARATE_POOL;
    } else {
        _BCM_ETH_OAM_NUM_SESSIONS =
            soc_mem_index_count(unit, MA_INDEXm) - _BCM_MPLS_OAM_NUM_SESSIONS;
        oc->ma_idx_alloc_scheme = _BCM_OAM_MA_IDX_ALLOC_SEPARATE_POOL;
    }

    /* Mem_1: Allocate hash data memory */
    /* size = sizeof(_bcm_oam_hash_data_t) * oc->ep_count; */
    size = sizeof(_bcm_oam_hash_data_t) * _BCM_OAM_KATANA2_ENDPOINT_MAX;

    _BCM_OAM_ALLOC(oc->oam_hash_data, _bcm_oam_hash_data_t, size, "Hash data");
    if (NULL == oc->oam_hash_data) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (BCM_E_MEMORY);
    }

    /* Mem_2: Allocate group memory */
    size = sizeof(_bcm_oam_group_data_t) * oc->group_count;

    _BCM_OAM_ALLOC(oc->group_info, _bcm_oam_group_data_t, size, "Group Info");

    if (NULL == oc->group_info) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (BCM_E_MEMORY);
    }

    /* Allocate RMEP H/w to logical index mapping memory */
    size = sizeof(bcm_oam_endpoint_t) * oc->rmep_count;

    _BCM_OAM_ALLOC(oc->remote_endpoints, bcm_oam_endpoint_t, size, "RMEP Mapping");
    if (NULL == oc->remote_endpoints) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (BCM_E_MEMORY);
    }
    /* Initialize the mapping to BCM_OAM_ENDPOINT_INVALID */ 
    for (ep_index = 0; ep_index < oc->rmep_count; ++ep_index) {
        oc->remote_endpoints[ep_index] = BCM_OAM_ENDPOINT_INVALID;
    }

    /* Mem_3: Create application endpoint list. */
    rv = shr_idxres_list_create(&oc->mep_pool, 0, oc->ep_count - 1,
                                0, oc->ep_count -1, "endpoint pool");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    /* Mem_4: Create local MEP endpoint list. */
    rv = shr_idxres_list_create(&oc->lmep_pool, 0, oc->lmep_count - 1,
                                0, oc->lmep_count -1, "lmep pool");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    /* Mem_5: Create Remote MEP endpoint list. */
    rv = shr_idxres_list_create(&oc->rmep_pool, 0, oc->rmep_count - 1,
                                0, oc->rmep_count -1, "rmep pool");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    /* Mem_6: Create MEP Rx state tracker table index endpoint list - DnMEP */
    rv =_bcm_kt2_oam_ma_idx_pool_create(oc);
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);

    }

    /* Mem_7: Create MEP Rx state tracker table index endpoint list -UpMEP */
    rv = shr_idxres_list_create(&oc->egr_ma_idx_pool, 0, 
                                oc->egr_ma_idx_count - 1,
                                0, oc->egr_ma_idx_count -1, "egr_ma_idx pool");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);

    }

    /* Initialize to domain specific key by default, WB recovery might
     * overwrite it, depending on older SDK.
     */
    oc->eth_oam_mp_group_vlan_key = _BCM_KT2_ETH_OAM_MP_GROUP_KEY_DOMAIN_SPECIFIC;

    if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
    /* Create BHH endpoint list. */
    rv = shr_idxres_list_create(&oc->bhh_pool, 0, oc->bhh_endpoint_count - 1,
                                0, oc->bhh_endpoint_count - 1, "bhh pool");
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Creating BHH pool failed \n")));
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    /* Reserve ep pool from mep_pool for BHH, manange BHH mep using bhh_pool */
    rv = shr_idxres_list_reserve(oc->mep_pool, _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET,
                _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET + oc->bhh_endpoint_count - 1);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Reserving BHH endpoints from mep_pool "
                              "failed \n")));
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

#endif /* INCLUDE_BHH */
    }

    /* Mem_4: Create group list. */
    rv = shr_idxres_list_create(&oc->group_pool, 0, oc->group_count - 1,
                                0, oc->group_count - 1, "group pool");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }
    if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        if(ukernel_not_ready == 0){
            int bhh_num_lm_sec_meps = soc_property_get(unit, spn_BHH_NUM_LM_ENABLED_SECTION_MEPS, 0);
            int bhh_num_lm_sec_counters = bhh_num_lm_sec_meps * 8;

            if( bhh_num_lm_sec_counters > oc->lm_counter_cnt) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                    "OAM Error: Number of LM enabled section meps cannot be more than "
                                    "available lm counter count \n")));
                _bcm_kt2_oam_control_free(unit, oc);
                return (BCM_E_PARAM);
            }
            if (bhh_num_lm_sec_counters) {
                oc->lm_counter_cnt = oc->lm_counter_cnt - bhh_num_lm_sec_counters;

                rv = shr_aidxres_list_create(&oc->ing_lm_sec_mep_ctr_pool, 0,
                                              bhh_num_lm_sec_counters - 1, 0,
                                              bhh_num_lm_sec_counters - 1, 8, "ing_lm_sec_mep_ctr_pool");
                if (BCM_FAILURE(rv)) {
                    _bcm_kt2_oam_control_free(unit, oc);
                    return (rv);
                }
            }
        }
#endif
    }

    /* Index management for Ingress LM counter pool 1 */
    rv = shr_aidxres_list_create(&oc->ing_lm_ctr_pool[0], 0, 
                         oc->lm_counter_cnt - 1, 0, 
                         oc->lm_counter_cnt - 1, 8,  "ing_lm_idx pool1");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);

    }
    /* Index management for Ingress LM counter pool 2 */
    rv = shr_aidxres_list_create(&oc->ing_lm_ctr_pool[1], 0, 
                         oc->lm_counter_cnt - 1, 0,
                         oc->lm_counter_cnt - 1, 8, "ing_lm_idx pool2");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    
    /* Mem_5: Create MA group and MEP hash table. */
    rv = shr_htb_create(&oc->ma_mep_htbl, _BCM_OAM_KATANA2_ENDPOINT_MAX,
                        sizeof(_bcm_oam_hash_key_t), "MA/MEP Hash");
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    /* Create protection mutex. */
    oc->oc_lock = sal_mutex_create("oam_control.lock");
    if (NULL == oc->oc_lock) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (BCM_E_MEMORY);
    }

    /* Register device OAM interrupt handler & SER handler call back routine.*/
    soc_kt2_oam_handler_register(unit, _bcm_kt2_oam_handle_interrupt);
    soc_kt2_oam_ser_handler_register(unit, _bcm_kt2_oam_ser_handler);

    /* Set up the unit OAM control structure. */
    _kt2_oam_control[unit] = oc;
#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_oam_pm)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_pm_init(unit));
    }
#endif
    /* Set up OAM module related profile tables. */
    rv = _bcm_kt2_oam_profile_tables_init(unit, oc);
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }

    /* Initialize all the TPID tables */
    rv =  _bcm_kt2_outer_tpid_init(unit); 
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }
    rv =  _bcm_kt2_inner_tpid_init(unit); 
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }
    rv =  _bcm_kt2_subport_tpid_init(unit); 
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return (rv);
    }
    rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_kt2_oam_control_free(unit, oc);
        return rv;
    }


#if defined(BCM_WARM_BOOT_SUPPORT)
    if(!SOC_WARM_BOOT(unit)) {
        bcm_kt2_oam_scache_alloc(unit);
    }
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_kt2_oam_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
    } else 
#endif
    {
        BCM_PBMP_CLEAR(all_pbmp);
        BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

        if (soc_feature(unit, soc_feature_flex_port)) {
            BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &all_pbmp));
        }
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
        }

        /* Enable OAM processing on all ports of this unit. */
        PBMP_ITER(all_pbmp, port) {
            rv = bcm_esw_port_control_set(unit, port, bcmPortControlOAMEnable,
                                          TRUE);
            if (BCM_FAILURE(rv)) {
                _bcm_kt2_oam_control_free(unit, oc);
                return (rv);
            }
        }

        /* Enable CCM Rx timeouts. */
        rv = _bcm_kt2_oam_ccm_rx_timeout_set(unit, 1);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }

        /* Enable CCM Tx control. */
        rv = _bcm_kt2_oam_ccm_tx_config_set(unit, 1);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }


        /* 
         * Misc config: Enable IFP lookup on the CPU port. 
         */
        rv = _bcm_kt2_oam_misc_config(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set all the non CFM opcodes in opcode group -1 */
        oc->opcode_grp_bmp[0] = ~(_BCM_OAM_OPCODE_TYPE_CFM_MASK);
        oc->opcode_grp_1_bmp[0] = 0; 
        oc->opcode_grp_2_bmp[0] = 0; 
        for (index = 1; index < 8; index ++) {
            oc->opcode_grp_bmp[index] = _BCM_OAM_OPCODE_TYPE_NON_CFM_MASK; 
            oc->opcode_grp_1_bmp[index] = 0; 
            oc->opcode_grp_2_bmp[index] = 0; 
        }
        /* Init OAM opcode group */ 
        rv =_bcm_kt2_oam_opcode_group_init(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set default values for flexible oam domain control  */
        rv = _bcm_kt2_oam_flexible_oam_domain_ctrl_set(unit); 
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set default values for flexible ingress and egress drop control */
        rv = _bcm_kt2_oam_flexible_drop_ctrl_set(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set default values for s-interface passive processing control */
        rv = _bcm_kt2_oam_s_intf_passive_proc_ctrl_set(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        } 
        /* Enable OLP handling on HG ports */
        rv = _bcm_kt2_oam_hg_olp_enable(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set default olp header type mapping */
        rv = _bcm_kt2_oam_olp_header_type_mapping_set(unit); 
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set OAM drop control to not to drop wrong version OAM packets */
        rv = _bcm_kt2_oam_drop_ctrl_set(unit); 
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set OAM LM cng and cpu data control*/
        rv = _bcm_kt2_oam_lm_cng_cpu_ctrl_set_default(unit); 
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Set Magic port used in OLP-XGS communication */
        rv = _bcm_kt2_oam_olp_magic_port_set(unit); 
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
        /* Disable MACSA Zero check  */
        rv = _bcm_kt2_oam_macsa_zero_check_disable(unit); 
        if (BCM_FAILURE(rv)) {
            _bcm_kt2_oam_control_free(unit, oc);
            return (rv);
        }
    
    }

      /*
     * BHH init
     */
    if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        if(ukernel_not_ready == 0){
            /*
             * Initialize HOST side
             */
            oc->cpu_cosq = soc_property_get(unit, spn_BHH_COSQ, BHH_COSQ_INVALID);

            /*
             * Allocate DMA buffers
             *
             * DMA buffer will be used to send and receive 'long' messages
             * between SDK Host and uController (BTE).
             */
            oc->dma_buffer_len = sizeof(shr_bhh_msg_ctrl_t);

            /* Adding  feature specific for dma_buf_len as currently
             * required buffer length for faults_get can be configurable
             * by user during init. So, taking the max value of either of
             * them for the purpose.Putting it under fature_check for
             * giving flexibility to user.
             */
#ifdef INCLUDE_OAM_FAULTS_MULTI_GET
            if(soc_feature(unit, soc_feature_faults_multi_ep_get)) {
                oc->dma_buffer_len = MAX((oc->bhh_endpoint_count * sizeof(uint32)), oc->dma_buffer_len);
            }
#endif
            oc->dma_buffer = soc_cm_salloc(unit, oc->dma_buffer_len,
                    "BHH DMA buffer");
            if (!oc->dma_buffer) {
                _bcm_kt2_oam_control_free(unit, oc);
                return (BCM_E_MEMORY);
            }
            sal_memset(oc->dma_buffer, 0, oc->dma_buffer_len);

            oc->dmabuf_reply = soc_cm_salloc(unit, oc->dma_buffer_len,
                    "BHH uC reply");
            if (!oc->dmabuf_reply) {
                _bcm_kt2_oam_control_free(unit, oc);
                return (BCM_E_MEMORY);
            }
            sal_memset(oc->dmabuf_reply, 0, oc->dma_buffer_len);

            /* RX DMA channel (0..3) local to the uC */
            oc->rx_channel = BCM_KT_BHH_RX_CHANNEL;

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit)) {
#endif
            /* Set control message data */
            sal_memset(&msg_init, 0, sizeof(msg_init));
            msg_init.num_sessions       = oc->bhh_endpoint_count;
            msg_init.rx_channel         = oc->rx_channel;
            msg_init.node_id            = node_id;
            sal_memcpy(msg_init.carrier_code, carrier_code, SHR_BHH_CARRIER_CODE_LEN);
            msg_init.max_encap_length = oc->bhh_max_encap_length;
            if (bhh_consolidate_final_event) {
                msg_init.flags |= BHH_SDK_MSG_CTRL_INIT_ONLY_FINAL_EVENT;
            }
            
            msg_init.data_collection_mode = oc->pm_bhh_lmdm_data_collection_mode;

            /* Pack control message data into DMA buffer */
            buffer     = oc->dma_buffer;
            buffer_ptr = bhh_sdk_msg_ctrl_init_pack(unit, buffer, &msg_init);
            buffer_len = buffer_ptr - buffer;

            /* Send BHH Init message to uC */
            rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                    MOS_MSG_SUBCLASS_BHH_INIT,
                    buffer_len, 0,
                    MOS_MSG_SUBCLASS_BHH_INIT_REPLY,
                    &reply_len);

            if (BCM_FAILURE(rv) || (reply_len != 0)) {
                _bcm_kt2_oam_control_free(unit, oc);
                return (BCM_E_INTERNAL);
            }
#if defined(BCM_WARM_BOOT_SUPPORT)
    }
#endif
            /*
             * Start event message callback thread
             */
            priority = soc_property_get(unit, spn_BHH_THREAD_PRI, BHH_THREAD_PRI_DFLT);

            if (oc->event_thread_id == NULL) {
                if ((oc->event_thread_id =
                     sal_thread_create("bcmBHH", SAL_THREAD_STKSZ,
                                       priority,
                                       _bcm_kt2_oam_bhh_callback_thread,
                                       (void*)oc)) == SAL_THREAD_ERROR) {
                    oc->event_thread_id = NULL;
                    BCM_IF_ERROR_RETURN(bcm_kt2_oam_detach(unit));
                    return (BCM_E_MEMORY);
                }
            }
            /*
             * End BHH init
             */
        }
        /*
         * Initialize HW
         */
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            rv = _bcm_kt2_oam_bhh_cos_map_recover(unit);
        } else
#endif
        {
            rv = _bcm_kt2_oam_bhh_hw_init(unit);
        }

        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(bcm_kt2_oam_detach(unit));
            return rv;
        }

#endif /* INCLUDE_BHH */
    }

    /* OAM initialization complete. */
    _kt2_oam_control[unit]->init = TRUE;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_kt2_oam_detach
 * Purpose:
 *     Shut down OAM subsystem
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_detach(int unit)
{
    _bcm_oam_control_t *oc;  /* Pointer to OAM control structure. */
    bcm_port_t         port; /* Port number.                      */
    int                rv;   /* Operation return status.          */
    bcm_pbmp_t         all_pbmp;
#if defined(INCLUDE_BHH)
    uint16 reply_len;
    sal_usecs_t timeout = 0;
#endif

    /* Get the device OAM module handle. */
    oc = _kt2_oam_control[unit];
    if (NULL == oc) {
        /* Module already uninitialized. */
        return (BCM_E_NONE);
    }

    /* Unregister all OAM interrupt event handlers and SER handler. */
    soc_kt2_oam_handler_register(unit, NULL);
    soc_kt2_oam_ser_handler_register(unit, NULL);

    if (NULL != oc->oc_lock) {
        _BCM_OAM_LOCK(oc);
    }

    rv = _bcm_kt2_oam_events_unregister(unit, oc);
    if (BCM_FAILURE(rv)) {
        if (NULL != oc->oc_lock) {
            _BCM_OAM_UNLOCK(oc);
        }
        return (rv);
    }

    /* Disable CCM Rx Timeouts. */
    rv = _bcm_kt2_oam_ccm_rx_timeout_set(unit, 0);
    if (BCM_FAILURE(rv)) {
        if (NULL != oc->oc_lock) {
            _BCM_OAM_UNLOCK(oc);
        }
        return (rv);
    }

    /* Disable CCM Tx control for the device. */
    rv = _bcm_kt2_oam_ccm_tx_config_set(unit, 0);
    if (BCM_FAILURE(rv)) {
        if (NULL != oc->oc_lock) {
            _BCM_OAM_UNLOCK(oc);
        }
        return (rv);
    }

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
    
    if (soc_feature(unit, soc_feature_flex_port)) {
        rv = _bcm_kt2_flexio_pbmp_update(unit, &all_pbmp);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            return rv;
        }
    }
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
    }

    /* Disable OAM PDU Rx processing on all ports. */
    PBMP_ITER(all_pbmp, port) {
        rv = bcm_esw_port_control_set(unit, port,
                                      bcmPortControlOAMEnable, 0);
        if (BCM_FAILURE(rv)) {
            if (NULL != oc->oc_lock) {
                _BCM_OAM_UNLOCK(oc);
            }
            return (rv);
        }
    }
    

     /*
     * BHH specific
     */
    if (soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        if (oc->ukernel_not_ready == 0) {
            /*
             * Event Handler thread exit signal
             */
            timeout = sal_time_usecs() + 5000000;
            while (NULL !=  oc->event_thread_id) {
                soc_cmic_uc_msg_receive_cancel(unit, oc->uc_num,
                                               MOS_MSG_CLASS_BHH_EVENT);

                if (sal_time_usecs() < timeout) {
                    /*give some time to already running bhh callback thread
                     * to schedule and exit */
                    sal_usleep(10000);
                } else {
                    /*timeout*/
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "BHH event thread did not exit.\n")));
                    _BCM_OAM_UNLOCK(oc);
                    return BCM_E_INTERNAL;
                }
            }

            /*
             * Send BHH Uninit message to uC
             * Ignore error since that may indicate uKernel was reloaded.
             */
#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit)) {
#endif
            rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                                              MOS_MSG_SUBCLASS_BHH_UNINIT,
                                              0, 0,
                                              MOS_MSG_SUBCLASS_BHH_UNINIT_REPLY,
                                              &reply_len);
            if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                if (NULL != oc->oc_lock) {
                    _BCM_OAM_UNLOCK(oc);
                }
                return BCM_E_INTERNAL;
            }
#if defined(BCM_WARM_BOOT_SUPPORT)
    }
#endif

            /*
             * Delete CPU COS queue mapping entries for BHH packets
             */
            if (!SOC_WARM_BOOT(unit)) {
                if (oc->cpu_cosq_ach_error_index >= 0) {
                    rv = bcm_esw_rx_cosq_mapping_delete(unit,
                                                  oc->cpu_cosq_ach_error_index);
                    if (BCM_FAILURE(rv)) {
                        if (NULL != oc->oc_lock) {
                            _BCM_OAM_UNLOCK(oc);
                        }
                        return (rv);
                    }
                    oc->cpu_cosq_ach_error_index = -1;
                }
                if (oc->cpu_cosq_invalid_error_index >= 0) {
                    rv = bcm_esw_rx_cosq_mapping_delete(unit,
                                              oc->cpu_cosq_invalid_error_index);
                    if (BCM_FAILURE(rv)) {
                        if (NULL != oc->oc_lock) {
                            _BCM_OAM_UNLOCK(oc);
                        }
                        return (rv);
                    }
                    oc->cpu_cosq_invalid_error_index = -1;
                }
                if (oc->bhh_lb_index >= 0) {
                    rv = bcm_esw_rx_cosq_mapping_delete(unit,
                                                        oc->bhh_lb_index);
                    if (BCM_FAILURE(rv)) {
                        if (NULL != oc->oc_lock) {
                            _BCM_OAM_UNLOCK(oc);
                        }
                        return (rv);
                    }
                    oc->bhh_lb_index = -1;
                }
            }
        }
#endif /* INCLUDE_BHH */
    }

    /* Destroy all groups and assoicated endpoints and free the resources. */
    rv = bcm_kt2_oam_group_destroy_all(unit);
    if (BCM_FAILURE(rv)) {
        if (NULL != oc->oc_lock) {
            _BCM_OAM_UNLOCK(oc);
        }
        return (rv);
    }
    rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_destroy(unit);
    if (BCM_FAILURE(rv)) {
        if (NULL != oc->oc_lock) {
            _BCM_OAM_UNLOCK(oc);
        }
        return (rv);
    }

    /* Release the protection mutex. */
    _BCM_OAM_UNLOCK(oc);

    /* Free OAM module allocated resources. */
    _bcm_kt2_oam_control_free(unit, oc);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_kt2_oam_group_create
 * Purpose:
 *     Create or replace an OAM group object
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group information.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_group_create(int unit, bcm_oam_group_info_t *group_info)
{
    ma_state_entry_t       ma_state_entry;       /* MA State table entry.     */
    uint8                  grp_name_hw_buf[BCM_OAM_GROUP_NAME_LENGTH]; /* Grp */
                                                 /* name.                     */
    maid_reduction_entry_t maid_reduction_entry; /* MA ID reduction table     */
    _bcm_oam_group_data_t  *ma_group;            /* Pointer to group info.    */
    _bcm_oam_control_t     *oc;                  /* OAM control structure.    */
    int                    rv;                   /* Operation return status.  */
                                                 /* entry.                    */
    uint8                  sw_rdi;               /* Remote defect indicator.  */

#if defined(BCM_KATANA2_SUPPORT)  && defined(INCLUDE_BHH)
    bhh_sdk_msg_ctrl_sess_set_t msg_sess;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    uint32 session_flags = 0;
    _bcm_oam_hash_data_t *h_data_p;
    _bcm_oam_ep_list_t      *cur;
#endif

    /* Validate input parameter. */
    if (NULL == group_info) {
        return (BCM_E_PARAM);
    }

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Validate group id. */
    if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        _BCM_OAM_GROUP_INDEX_VALIDATE(group_info->id);
    }

    _BCM_OAM_LOCK(oc);

    /*
     * If MA group create is called with replace flag bit set.
     *  - Check and return error if a group does not exist with the ID.
     */
    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {

            /* Search the list with the MA Group ID value. */
            rv = shr_idxres_list_elem_state(oc->group_pool, group_info->id);
            if (BCM_E_EXISTS != rv) {
                _BCM_OAM_UNLOCK(oc);
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Group does not exist.\n")));
                return (BCM_E_PARAM);
            }
        } else {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Replace command needs a "
                                  "valid Group ID.\n")));
            return (BCM_E_PARAM);
        }
    } else if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        /*
         * If MA group create is called with ID flag bit set.
         *  - Check and return error if the ID is already in use.
         */
        rv = shr_idxres_list_reserve(oc->group_pool, group_info->id,
                                     group_info->id);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
        }
    } else {
        /* Reserve the next available group index. */
         rv = shr_idxres_list_alloc(oc->group_pool,
                                    (shr_idxres_element_t *) &group_info->id);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Group allocation (GID=%d)"
                                   " %s\n"), group_info->id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    /* Get this group memory to store group information. */
    ma_group = &oc->group_info[group_info->id];

    /* Store the group name. */
    sal_memcpy(&ma_group->name, &group_info->name, BCM_OAM_GROUP_NAME_LENGTH);

    /* Store Lowest Alarm Priority */
    ma_group->lowest_alarm_priority = group_info->lowest_alarm_priority;

    if(!(group_info->flags & BCM_OAM_GROUP_REPLACE)) {
        _BCM_OAM_ALLOC((ma_group->ep_list),_bcm_oam_ep_list_t *,
               sizeof(_bcm_oam_ep_list_t *), "EP list head");

        /* Initialize head to NULL.*/
        *ma_group->ep_list =  NULL;
    }

    /*
     * Maintenance Association ID - Reduction table update.
     */

    /* Prepare group name for hardware write. */
    _bcm_kt2_oam_group_name_mangle(ma_group->name, grp_name_hw_buf);

    sal_memset(&maid_reduction_entry, 0, sizeof(maid_reduction_entry_t));

    /* Calculate CRC32 value for the group name string and set in entry. */
    soc_MAID_REDUCTIONm_field32_set
        (unit, &maid_reduction_entry, REDUCED_MAIDf,
         soc_draco_crc32(grp_name_hw_buf, BCM_OAM_GROUP_NAME_LENGTH));

    /* Check if software RDI flag bit needs to be set in hardware. */
    sw_rdi = ((group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) ? 1 : 0);

    /* Set RDI status for out going CCM PDUs. */
    soc_MAID_REDUCTIONm_field32_set(unit, &maid_reduction_entry, SW_RDIf,
                                    sw_rdi);

    if (soc_feature(unit, soc_feature_bhh)) {
#if defined(BCM_KATANA2_SUPPORT)  && defined(INCLUDE_BHH)
        /* Send message to uC to set the Soft RDI   */
        if (group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) { 

            /* Get the endpoint list head pointer. */
            cur = *(ma_group->ep_list);

            while (NULL != cur) {
                h_data_p = cur->ep_data_p;
                if (NULL == h_data_p) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                        "OAM Error: Group=%d endpoints access failed -"
                         " %s.\n"), group_info->id, 
                         bcm_errmsg(BCM_E_INTERNAL)));
                    _BCM_OAM_UNLOCK(oc);
                    return (BCM_E_INTERNAL);
                }
                if (h_data_p->oam_domain == _BCM_OAM_DOMAIN_BHH) {
                    /* Set the RDI flag in session bits */
                    session_flags |= SHR_BHH_SESS_SET_F_RDI;

                    /* Get the session is from endpoint */
                    msg_sess.sess_id =
                          BCM_OAM_BHH_GET_UKERNEL_EP(h_data_p->ep_id);
                
                    /* Pack control message data into DMA buffer */
                    msg_sess.flags = session_flags;

                    buffer     = oc->dma_buffer;
                    buffer_ptr = 
                        bhh_sdk_msg_ctrl_sess_set_pack(buffer, &msg_sess);
                    buffer_len = buffer_ptr - buffer;

                    /* Send BHH Session Update message to uC */
                    rv =  _bcm_kt2_oam_bhh_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_BHH_SESS_SET,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_BHH_SESS_SET_REPLY,
                                         &reply_len);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                                   "OAM Error: ukernel msg failed for"
                                   "%s.\n"), bcm_errmsg(rv)));
                        _BCM_OAM_UNLOCK(oc);
                        return (BCM_E_INTERNAL);
                    }
                }
                cur = cur->next;
            }    
        }
#endif
    }

    /* Enable hardware lookup for this entry. */
    soc_MAID_REDUCTIONm_field32_set(unit, &maid_reduction_entry, VALIDf, 1);


    /* Write entry to hardware. */
    rv = WRITE_MAID_REDUCTIONm(unit, MEM_BLOCK_ALL, group_info->id,
                               &maid_reduction_entry);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    /*
     * Maintenance Association State table update.
     */
    sal_memset(&ma_state_entry, 0, sizeof(ma_state_entry_t));

    /*
     * If it is a group info replace operation, retain previous group
     * defect status.
     */
    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        rv = READ_MA_STATEm(unit, MEM_BLOCK_ALL, group_info->id,
                            &ma_state_entry);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            return rv;
        }
    }

    /* Set the lowest alarm priority info. */
    soc_MA_STATEm_field32_set(unit, &ma_state_entry, LOWESTALARMPRIf,
                              group_info->lowest_alarm_priority);

    /* Mark the entry as valid. */
    soc_MA_STATEm_field32_set(unit, &ma_state_entry, VALIDf, 1);

    /* Write group information to hardware table. */
    rv = WRITE_MA_STATEm(unit, MEM_BLOCK_ALL, group_info->id,
                                        &ma_state_entry);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    /* Make the group as in used status. */
    ma_group->in_use = 1;

    _BCM_OAM_UNLOCK(oc);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

#if defined(INCLUDE_BHH)
/*
 * Function:
 *      _bcm_kt2_oam_get_group_sw_rdi
 * Purpose:
 *     Get OAM group SW RDI information.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     group_index  - (IN) Group hardware table index
 *     group_sw_rdi - (OUT) Pointer to group SW RDI
 * Returns:
 *     BCM_E_NONE      - No errors.
 *     BCM_E_XXX       - Otherwise.
 */
STATIC int
_bcm_kt2_oam_get_group_sw_rdi(int unit, bcm_oam_group_t group_index,
                       uint8 *group_sw_rdi)
{
    maid_reduction_entry_t  maid_reduction_entry; /* MAID reduction entry.    */

    BCM_IF_ERROR_RETURN(READ_MAID_REDUCTIONm(unit, MEM_BLOCK_ANY,
                                             group_index,
                                             &maid_reduction_entry));

    if (1 == soc_MAID_REDUCTIONm_field32_get(unit, &maid_reduction_entry,
                                             SW_RDIf)) {
        *group_sw_rdi = 1;
    }
    return (BCM_E_NONE);
}
#endif

/*
 * Function:
 *     bcm_kt2_oam_group_get
 * Purpose:
 *     Get an OAM group object
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group      - (IN) OAM Group ID.
 *     group_info - (OUT) Pointer to group information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oam_group_get(int unit, bcm_oam_group_t group,
                      bcm_oam_group_info_t *group_info)
{
    _bcm_oam_control_t    *oc;      /* Pointer to OAM control structure. */
    _bcm_oam_group_data_t *group_p; /* Pointer to group list.            */
    int                   rv;       /* Operation return status.          */

#if defined(INCLUDE_BHH)
    bhh_sdk_msg_ctrl_sess_get_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    _bcm_oam_hash_data_t *h_data_p;
    _bcm_oam_ep_list_t      *cur = NULL;
    int sess_id = 0;
#endif

    /* Validate input parameter. */
    if (NULL == group_info) {
        return (BCM_E_PARAM);
    }

    /* Get OAM device control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Validate group index. */
    _BCM_OAM_GROUP_INDEX_VALIDATE(group);

    _BCM_OAM_LOCK(oc);

    /* Check if the group is in use. */
    rv = shr_idxres_list_elem_state(oc->group_pool, group);
    if (BCM_E_EXISTS != rv) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: GID=%d - %s.\n"),
                   group, bcm_errmsg(rv)));
        return (rv);
    }

    /* Get pointer to this group in the group list. */
    group_p = oc->group_info;

    /* Get the group information. */
    rv = _bcm_kt2_oam_get_group(unit, group, group_p, group_info);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: bcm_kt2_oam_group_get Group ID=%d "
                               "- Failed.\n"), group));
        return (rv);
    }

    if (soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)

        if (!oc->ukernel_not_ready) {
            group_p = &oc->group_info[group_info->id];
            /* Get the endpoint list head pointer. */
            if (group_p->ep_list != NULL) {
                cur = *(group_p->ep_list);
            }

            while (NULL != cur) {
                h_data_p = cur->ep_data_p;
                if (NULL == h_data_p) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                                                          "OAM(unit %d) Error: Group=%d endpoints access failed -"
                                                          " %s.\n"), unit, group_info->id,
                                               bcm_errmsg(BCM_E_INTERNAL)));
                    _BCM_OAM_UNLOCK(oc);
                    return (BCM_E_INTERNAL);
                }
                if ((h_data_p->oam_domain == _BCM_OAM_DOMAIN_BHH) &&
                    !(h_data_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU)) {
                    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(h_data_p->ep_id);
                    rv = _bcm_kt2_oam_bhh_msg_send_receive(
                                             unit,
                                             MOS_MSG_SUBCLASS_BHH_SESS_GET,
                                             sess_id, 0,
                                             MOS_MSG_SUBCLASS_BHH_SESS_GET_REPLY,
                                             &reply_len);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                    "OAM(unit %d) Error: ukernel msg failed for"
                                    "EP=%d %s.\n"), unit, h_data_p->ep_id,
                                   bcm_errmsg(rv)));
                        _BCM_OAM_UNLOCK(oc);
                        return (rv);
                    }

                    /* Pack control message data into DMA buffer */
                    buffer = oc->dma_buffer;
                    buffer_ptr = bhh_sdk_msg_ctrl_sess_get_unpack(buffer, &msg);
                    buffer_len = buffer_ptr - buffer;

                    if (reply_len != buffer_len) {
                        rv = BCM_E_INTERNAL;
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                     "OAM(unit %d) Error: ukernel msg failed for"
                                     " EP=%d %s.\n"), unit, h_data_p->ep_id,
                                   bcm_errmsg(rv)));
                        _BCM_OAM_UNLOCK(oc);
                        return (rv);
                    } else {
                        if(msg.fault_flags & BHH_BTE_EVENT_CCM_TIMEOUT){
                            group_info->faults |= BCM_OAM_BHH_FAULT_CCM_TIMEOUT;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CCM_RDI){
                            group_info->faults |= BCM_OAM_BHH_FAULT_CCM_RDI;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL){
                            group_info->faults |=
                                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID){
                            group_info->faults |=
                                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID){
                            group_info->faults |=
                                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD){
                            group_info->faults |=
                                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY){
                            group_info->faults |=
                                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CSF_LOS) {
                            group_info->faults |=
                                BCM_OAM_ENDPOINT_BHH_FAULT_CSF_LOS;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CSF_FDI) {
                            group_info->faults |=
                                BCM_OAM_ENDPOINT_BHH_FAULT_CSF_FDI;
                        }
                        if(msg.fault_flags & BHH_BTE_EVENT_CSF_RDI) {
                            group_info->faults |=
                                BCM_OAM_ENDPOINT_BHH_FAULT_CSF_RDI;
                        }
                    }
                }
                cur = cur->next;
            }
        }
#endif
    }

    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_kt2_oam_group_destroy
 * Purpose:
 *     Destroy an OAM group object and its associated endpoints.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     group - (IN) The ID of the OAM group object to destroy
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_group_destroy(int unit,
                          bcm_oam_group_t group)
{
    _bcm_oam_control_t     *oc;       /* Pointer to OAM control structure. */
    _bcm_oam_group_data_t  *g_info_p; /* Pointer to group list.            */
    int                    rv;        /* Operation return status.          */
    maid_reduction_entry_t maid_reduction_entry; /* MAID_REDUCTION entry.  */
    ma_state_entry_t       ma_state_entry; /* MA_STATE table entry.        */

    /* Get OAM device control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Validate group index. */
    _BCM_OAM_GROUP_INDEX_VALIDATE(group);

    _BCM_OAM_LOCK(oc);

    /* Check if the group is in use. */
    rv = shr_idxres_list_elem_state(oc->group_pool, group);
    if (BCM_E_EXISTS != rv) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: GID=%d - %s.\n"),
                   group, bcm_errmsg(rv)));
        return (rv);
    }

    /* Get pointer to this group in the group list. */
    g_info_p = &oc->group_info[group];

    rv = _bcm_kt2_oam_group_endpoints_destroy(unit, g_info_p);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: bcm_kt2_oam_endpoint_destroy_all"
                               " (GID=%d) - %s.\n"), group, bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(&maid_reduction_entry, 0, sizeof(maid_reduction_entry_t));
    rv = WRITE_MAID_REDUCTIONm(unit, MEM_BLOCK_ALL, group,
                          &maid_reduction_entry);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MAID REDUCTION write "
                               "(GID=%d) - %s.\n"), group, bcm_errmsg(rv)));
        return (rv);
    }

    /*
     * Maintenance Association State table update.
     */
    sal_memset(&ma_state_entry, 0, sizeof(ma_state_entry_t));

    rv = WRITE_MA_STATEm(unit, MEM_BLOCK_ALL, group, &ma_state_entry);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MA STATE write "
                               "(GID=%d) - %s.\n"), group, bcm_errmsg(rv)));
        return (rv);
    }

    /* Return Group ID back to free group. */
    rv = shr_idxres_list_free(oc->group_pool, group);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif
    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_kt2_oam_group_destroy_all
 * Purpose:
 *     Destroy all OAM group objects and their associated endpoints.
 * Parameters:
 *     unit  - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_group_destroy_all(int unit)
{
    _bcm_oam_control_t *oc;   /* Pointer to OAM control structure.    */
    int                group; /* Maintenance Association group index. */
    int                rv;    /* Operation return status .            */

    /* Get device OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);
    for (group = 0; group < oc->group_count; group++) {

        /* Check if the group is in use. */
        rv = shr_idxres_list_elem_state(oc->group_pool, group);
        if (BCM_E_EXISTS != rv) {
            continue;
        }

        rv = bcm_kt2_oam_group_destroy(unit, group);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Group destroy failed "
                                   "(GID=%d) - %s.\n"), group, bcm_errmsg(rv)));
            return (rv);
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);                                      
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_kt2_oam_group_traverse
 * Purpose:
 *     Traverse the entire set of OAM groups, calling a specified
 *     callback for each one
 * Parameters:
 *     unit  - (IN) BCM device number
 *     cb        - (IN) Pointer to call back function.
 *     user_data - (IN) Pointer to user supplied data.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                           void *user_data)
{
    _bcm_oam_control_t    *oc;        /* OAM control structure pointer. */
    bcm_oam_group_info_t  group_info; /* Group information to be set.   */
    bcm_oam_group_t       grp_idx;    /* MA Group index.                */
    _bcm_oam_group_data_t *group_p;   /* Pointer to group list.         */
    int                   rv;         /* Operation return status.       */

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Get device OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    /* Initialize to group array pointer. */
    group_p = oc->group_info;

    for (grp_idx = 0; grp_idx < oc->group_count; grp_idx++) {

        /* Check if the group is in use. */
        if (BCM_E_EXISTS
            == shr_idxres_list_elem_state(oc->group_pool, grp_idx)) {

            /* Initialize the group information structure. */
            bcm_oam_group_info_t_init(&group_info);

            /* Retrieve group information and set in group_info structure. */
            rv = _bcm_kt2_oam_get_group(unit, grp_idx, group_p, &group_info);
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: _bcm_kt2_oam_get_group "
                                       "(GID=%d) - %s.\n"), grp_idx, bcm_errmsg(rv)));
                return (rv);
            }

            /* Call the user call back routine with group information. */
            rv = cb(unit, &group_info, user_data);
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: User call back routine "
                                       "(GID=%d) - %s.\n"), grp_idx, bcm_errmsg(rv)));
                return (rv);
            }
        }
    }

    _BCM_OAM_UNLOCK(oc);

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_kt2_oam_endpoint_lm_update
 * Purpose:
 *     Update LM related params of an endpoint
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_kt2_oam_endpoint_lm_update(int unit, bcm_oam_endpoint_info_t *ep_info)
{
    _bcm_oam_hash_data_t         *h_data_p;
    _bcm_oam_control_t           *oc;
    int                           loss_add, loss_del;
    int                           loss_configured;
    soc_profile_mem_t             pri_map_prof_mem;
    uint32                        pri_ent[BCM_OAM_INTPRI_MAX]; /* ing profile */
    /* ing profile copy from HW*/
    uint32                        pri_ent_copy[BCM_OAM_INTPRI_MAX];
    int                           i;
    void                          *entries[1];
    int                           lm_ctr_pool_id = -1;
    int                           mem_index;
    egr_mp_group_entry_t          egr_mp_group;
    l3_entry_1_entry_t            l3_entry;       /* LMEP view table entry.*/
    int                           l3_index = -1;  /* L3 table hardware index.*/
    uint32                        profile_index = -1;
    int                           rv = BCM_E_NONE;
    soc_mem_t                     mem = ING_SERVICE_PRI_MAPm;;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if (ep_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
        /* Only LMEPs can update Loss parameters */
        return BCM_E_PARAM;
    }

    if (ep_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) {
        /* Only MEPs can update LM */
        return BCM_E_PARAM;
    }

    /* Validate endpoint index value. */
    _BCM_OAM_EP_INDEX_VALIDATE(ep_info->id);

    h_data_p =  &oc->oam_hash_data[ep_info->id];
    if (0 == h_data_p->in_use) {
        return BCM_E_NOT_FOUND;
    }

    /* Figure out if loss is being added or deleted */
    loss_add = loss_del = 0;
    loss_configured = 1; /* Indicates if BCM_OAM_ENDPOINT_LOSS_MEASUREMENT flag is
                          * present in new & old config
                          */
    if (h_data_p->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
        if (!(ep_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT)) {
            loss_del = 1;
        }
    } else {
        if (ep_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
            loss_add = 1;
        } else {
            /* BCM_OAM_ENDPOINT_LOSS_MEASUREMENT flag is not present in old/new
             * config.
             */
            loss_configured = 0;
        }
    }

    /* If loss configuration is present in either old & new configuration, then
     * find out if something is being modified in loss, updates will result in
     * delete and add.
     */
    if (1 == loss_configured && 0 == loss_add && 0 == loss_del) {

        lm_ctr_pool_id = (h_data_p->rx_ctr >> 24) & 0xFF;

        if (lm_ctr_pool_id != ep_info->lm_ctr_pool_id) {
            loss_del = loss_add = 1;
        }
        
        if(h_data_p->pri_map_index != _BCM_OAM_INVALID_INDEX) {

            pri_map_prof_mem = oc->ing_service_pri_map;

            for (i = 0; i < BCM_OAM_INTPRI_MAX; i++) {
                if (ep_info->pri_map[i] > _BCM_OAM_SERVICE_PRI_MAX_OFFSET) {
                    return BCM_E_PARAM;
                }
                pri_ent[i] = ep_info->pri_map[i];
                if (SOC_MEM_FIELD_VALID(unit, mem, OFFSET_VALIDf)) {
                    soc_mem_field32_set(unit, mem, &pri_ent[i], OFFSET_VALIDf, 1);
                }
            }

            soc_mem_lock(unit, mem);
          
            entries[0] = &pri_ent_copy;

            rv = soc_profile_mem_get(unit, &pri_map_prof_mem,
                    (h_data_p->pri_map_index * BCM_OAM_INTPRI_MAX),
                    BCM_OAM_INTPRI_MAX, (void *) entries);
            
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
            
            if (sal_memcmp(pri_ent, pri_ent_copy, 
                        sizeof(uint32)*BCM_OAM_INTPRI_MAX)){
                rv = soc_profile_mem_delete(unit, &pri_map_prof_mem,
                        (h_data_p->pri_map_index * BCM_OAM_INTPRI_MAX));
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM Error: Profile table update error (idx=%d)"
                                        "- %s.\n"), h_data_p->pri_map_index, bcm_errmsg(rv)));
                    soc_mem_unlock(unit, mem);
                    return (rv);
                }

                h_data_p->pri_map_index = profile_index;

                loss_del = loss_add = 1;
            }
            soc_mem_unlock(unit, mem);
        } else {
            /* If for some reason i.e. warmboot,index became invalid 
             * though loss is configured */
            return BCM_E_INTERNAL;
        }

    }

    if (1 == loss_del) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_find_lmep(unit, h_data_p, &l3_index,
                                                   &l3_entry));
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_clear_counter(unit, L3_ENTRY_IPV4_UNICASTm,
                                                       l3_index, h_data_p,
                                                       (void *)&l3_entry));

        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_find_egr_lmep(unit, h_data_p,
                                                       &mem_index,
                                                       &egr_mp_group));
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_clear_counter(unit, EGR_MP_GROUPm,
                                                       mem_index, h_data_p,
                                                       (void *)&egr_mp_group));
        h_data_p->flags &= ~BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
    }

    if (1 == loss_add) {
        h_data_p->flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_lmep_counters_set(unit, ep_info));
    }

    if(ep_info->type == bcmOAMEndpointTypeEthernet) {
        if((ep_info->ccm_tx_update_lm_counter_size) && 
        (_BCM_OAM_INVALID_INDEX != h_data_p->local_tx_index)) {
            BCM_IF_ERROR_RETURN(bcm_kt2_oam_hw_ccm_tx_ctr_update(unit, ep_info)); 
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_kt2_oam_endpoint_create
 * Purpose:
 *     Create or replace an OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oam_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_oam_hash_data_t *hash_data = NULL; /* Endpoint hash data pointer.  */
    _bcm_oam_hash_key_t  hash_key;          /* Hash Key buffer.             */
    int                  ep_req_index;      /* Requested endpoint index.    */
    int                  rv;                /* Operation return status.     */
    _bcm_oam_control_t   *oc;               /* Pointer to OAM control       */
                                            /* structure.                   */
    uint32               sglp = 0;          /* Source global logical port.  */
    uint32               dglp = 0;          /* Dest global logical port.    */
    uint32               svp = 0;           /* Source virtual port          */
    bcm_port_t           src_pp_port = 0;   /* Source pp port.              */
    bcm_port_t           dst_pp_port = 0;   /* Dest pp port.                */
    int                  mep_ccm_tx = 0;    /* Endpoint CCM Tx status.      */
    int                  mep_ccm_rx = 0;    /* Endpoint CCM Rx status.      */
    int                  remote = 0;        /* Remote endpoint status.      */
    int                  up_mep = 0;        /* Endpoint is an upMep         */
    int                  key_type = 0;
    port_tab_entry_t     port_entry;
    int                  oam_key1 = 0;
    int                  oam_key2 = 0;
    bcm_trunk_t          trunk_id = BCM_TRUNK_INVALID; 
    int                  is_vp_valid = 0;
    bcm_gport_t          tx_gport = BCM_GPORT_INVALID;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: bcm_kt2_oam_endpoint_create "
                           "Endpoint ID=%d.\n"), endpoint_info->id));

    /* Validate input parameter. */
    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

#if defined(KEY_PRINT)
    _bcm_oam_hash_key_print(&hash_key);
#endif

    /* Calculate the hash key for given enpoint input parameters. */
    _bcm_kt2_oam_ep_hash_key_construct(unit, oc, endpoint_info, &hash_key);

    /* Validate endpoint input parameters. */
    rv = _bcm_kt2_oam_endpoint_params_validate(unit, oc, &hash_key,
                                               endpoint_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: (EP=%d) - %s.\n"),
                   endpoint_info->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) &&
            (endpoint_info->flags2 & BCM_OAM_ENDPOINT2_UPDATE_COUNTER_ACTION)) {
        /* Update LM parameters */
        rv =  bcm_kt2_oam_endpoint_lm_update(unit, endpoint_info);
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        if(BHH_EP_TYPE(endpoint_info))
        {
            if ((oc->ukernel_not_ready == 1) && 
                    (!(endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU))){
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: BTE(ukernel) "
                                    "not ready.\n"), unit));
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_INIT);
            }
            rv = bcm_kt2_oam_bhh_endpoint_create(unit, endpoint_info, &hash_key);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: BHH Endpoint create (EP=%d) - %s.\n"),
                           endpoint_info->id, bcm_errmsg(rv)));
            }
            _BCM_OAM_UNLOCK(oc);

            return (rv);
        }
#endif /* INCLUDE_BHH */
    }

    /* Get MEP remote endpoint status. */
    remote = (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) ? 1 : 0;

    /* For remote endpoints gport is not required.
       Only group and MEP ID info is required. */
    if (!remote) {
        /* Resolve given endpoint gport value to Source GLP and Dest GLP values. */
        rv = _bcm_kt2_oam_endpoint_gport_resolve(unit, endpoint_info, &sglp, &dglp,
                                                 &src_pp_port, &dst_pp_port,
                                                 &svp, &trunk_id, &is_vp_valid,
                                                 &tx_gport);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Error: Gport resolve (EP=%d) - %s.\n"),
                     endpoint_info->id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
    }

    /* Get MEP CCM Tx status. */
    mep_ccm_tx
        = ((endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)
            ? 1 : 0);

    /* Get MEP CCM Rx status. */
    mep_ccm_rx
        = ((endpoint_info->flags & _BCM_OAM_EP_RX_ENABLE) ? 1 : 0);

  
    /* Check whether up/down MEP */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        up_mep = 1;
    }
    /* Set the search key type. */
    if (is_vp_valid) {
        key_type = _BCM_OAM_DOMAIN_VP;
    } else if(endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
        key_type = _BCM_OAM_DOMAIN_CVLAN;
    } else if(endpoint_info->flags & 
                           BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
        key_type = _BCM_OAM_DOMAIN_S_PLUS_CVLAN;
    } else if((endpoint_info->vlan == 0) && (endpoint_info->inner_vlan == 0)) {
        key_type = _BCM_OAM_DOMAIN_PORT;
    } else {
        key_type = _BCM_OAM_DOMAIN_SVLAN;
    }

    /* We need to set the OAM_KEY1 and OAM_KEY2 fields of the PORT_TABLE and 
       EGR_PORT_TABLE based on the key_type of OAM */
    if (trunk_id != BCM_TRUNK_INVALID) {
        rv = soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, 
                                                 src_pp_port, &port_entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint create (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }

        oam_key1 = soc_PORT_TABm_field32_get(unit, &port_entry, OAM_KEY1f); 
        oam_key2 = soc_PORT_TABm_field32_get(unit, &port_entry, OAM_KEY2f); 

        if (key_type != _BCM_OAM_DOMAIN_PORT) {
             if ((oam_key1 > 0) && (oam_key2 > 0)) {
                if ((oam_key1 == key_type) || (oam_key2 == key_type)) {
                } else {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: Invalid OAM domain to "
                                          "resolve (EP=%d) - %s.\n"),
                               endpoint_info->id, bcm_errmsg(rv)));
                    _BCM_OAM_UNLOCK(oc);
                    return (BCM_E_PARAM);
                }
            }
        }
    }

    /* For Downmep CCM tx purposes, int_pri variable is used like a cosq.
     * Hence verify if it is in the valid range, Else return error.
     * For UPMEP CCM tx purposes, int_pri is used as internal priority only.
     * Hence verify if it is in valid range of internal priority
     */
    if (mep_ccm_tx &&  !up_mep && 
        (!BCM_COSQ_QUEUE_VALID(unit, endpoint_info->int_pri))) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_PARAM);
    } else if (mep_ccm_tx &&  up_mep && 
      (!(endpoint_info->int_pri >= 0 && endpoint_info->int_pri < BCM_OAM_INTPRI_MAX))) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_PARAM);
    }

    /* Replace an existing endpoint. */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) {
        
        rv = _bcm_kt2_oam_endpoint_destroy(unit, endpoint_info->id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint destroy (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
    }

    /* Create a new endpoint with the requested ID. */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        ep_req_index = endpoint_info->id;
        rv = shr_idxres_list_reserve(oc->mep_pool, ep_req_index,
                                     ep_req_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint reserve (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return ((rv == BCM_E_RESOURCE) ? BCM_E_EXISTS : rv);
        }
    } else {
        /* Allocate the next available endpoint index. */
        rv = shr_idxres_list_alloc(oc->mep_pool,
                                   (shr_idxres_element_t *)&ep_req_index);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint alloc failed - %s.\n"),
                       bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        /* Set the allocated endpoint id value. */
        endpoint_info->id =  ep_req_index;
    }

    /* Get the hash data pointer where the data is to be stored. */
    hash_data = &oc->oam_hash_data[ep_req_index];

    /* Clear the hash data element contents before storing the values. */
    _BCM_OAM_HASH_DATA_CLEAR(hash_data);
    hash_data->type = endpoint_info->type;
    hash_data->ep_id = endpoint_info->id;
    hash_data->is_remote = remote;
    hash_data->local_tx_enabled = mep_ccm_tx;
    hash_data->local_rx_enabled = mep_ccm_rx;
    hash_data->group_index = endpoint_info->group;
    hash_data->name  = endpoint_info->name;
    hash_data->level = endpoint_info->level;
    hash_data->vlan = endpoint_info->vlan;
    hash_data->inner_vlan = endpoint_info->inner_vlan;
    hash_data->gport = endpoint_info->gport;
    hash_data->sglp = sglp;
    hash_data->dglp = dglp;
    hash_data->src_pp_port = src_pp_port;
    hash_data->dst_pp_port = dst_pp_port;
    hash_data->flags = endpoint_info->flags;
    hash_data->opcode_flags = ( endpoint_info->opcode_flags &
            _BCM_KT2_OAM_OPCODE_MASK );
    hash_data->period = endpoint_info->ccm_period;
    hash_data->in_use = 1;
    hash_data->oam_domain = key_type;
    hash_data->outer_tpid = endpoint_info->outer_tpid;
    hash_data->inner_tpid = endpoint_info->inner_tpid;
    hash_data->subport_tpid = endpoint_info->subport_tpid;
    hash_data->int_pri = endpoint_info->int_pri;
    hash_data->trunk_index = endpoint_info->trunk_index;
    if (is_vp_valid) {
        hash_data->vp = svp;
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            /* For implementation of VP based lookup on egress side (UP MEP)
             * We need to set EGR_DVP_ATTRIBUTE OAM_KEY3 = 1.
             */
            rv = bcm_esw_port_control_set(unit, endpoint_info->gport,
                    bcmPortControlOamLookupWithDvp,
                    1);
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }
        hash_data->int_flags |= _BCM_OAM_ENDPOINT_IS_VP_BASED;
    }
    if(endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
        hash_data->inner_vlan = endpoint_info->inner_vlan;
    }
    hash_data->trunk_id = trunk_id;
    if (BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
        hash_data->resolved_trunk_gport = tx_gport;
    }
    /* Initialize hardware index as invalid indices. */
    hash_data->local_tx_index = _BCM_OAM_INVALID_INDEX;
    hash_data->local_rx_index = _BCM_OAM_INVALID_INDEX;
    hash_data->remote_index = _BCM_OAM_INVALID_INDEX;
    hash_data->rx_ctr = _BCM_OAM_INVALID_INDEX;
    hash_data->tx_ctr = _BCM_OAM_INVALID_INDEX;
    hash_data->profile_index = _BCM_OAM_INVALID_INDEX;
    hash_data->dglp1_profile_index = _BCM_OAM_INVALID_INDEX;
    hash_data->dglp2_profile_index = _BCM_OAM_INVALID_INDEX;
    hash_data->pri_map_index = _BCM_OAM_INVALID_INDEX;
    hash_data->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
    hash_data->outer_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
    hash_data->subport_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
    hash_data->inner_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
    hash_data->ma_base_index = _BCM_OAM_INVALID_INDEX;

    if (1 == remote) {
        /* Allocate the next available index for RMEP table. */
        rv = shr_idxres_list_alloc
                (oc->rmep_pool,
                 (shr_idxres_element_t *)&hash_data->remote_index);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: RMEP index alloc failed EP:%d %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            return (rv);
        }
    } else {

        /* Allocate the next available index for LMEP table. */
        if (1 == mep_ccm_tx) {
            rv = shr_idxres_list_alloc
                    (oc->lmep_pool,
                     (shr_idxres_element_t *)&hash_data->local_tx_index);
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: LMEP Tx index alloc failed EP:%d "
                                       "%s.\n"), endpoint_info->id, bcm_errmsg(rv)));
                return (rv);
            }
        }

        /* Allocate the next available index for MA_INDEX table. */
        if (1 == mep_ccm_rx) {
           /* check up or down MEP */
            if (1 == up_mep) {
                rv =  _bcm_kt2_oam_upmep_rx_endpoint_reserve(unit, 
                                                          endpoint_info);
                if (BCM_FAILURE(rv)) {
                    if (1 == mep_ccm_tx) {
                    /* Return Tx index to the LMEP pool. */
                        shr_idxres_list_free(oc->lmep_pool,
                                         hash_data->local_tx_index);
                    }

                    /* Return endpoint index to MEP pool. */
                    shr_idxres_list_free(oc->mep_pool, endpoint_info->id);

                    _BCM_OAM_UNLOCK(oc);
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LMEP Rx index alloc failed "
                                          "EP:%d " "%s.\n"),
                               endpoint_info->id, bcm_errmsg(rv)));
                    return (rv);
                }
            } else { /* down Mep */
                rv =  _bcm_kt2_oam_downmep_rx_endpoint_reserve(unit, 
                                                               endpoint_info);
                if (BCM_FAILURE(rv)) {
                    if (1 == mep_ccm_tx) {
                    /* Return Tx index to the LMEP pool. */
                        shr_idxres_list_free(oc->lmep_pool,
                                         hash_data->local_tx_index);
                    }

                    /* Return endpoint index to MEP pool. */
                    shr_idxres_list_free(oc->mep_pool, endpoint_info->id);

                    _BCM_OAM_UNLOCK(oc);
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LMEP Rx index alloc "
                                          "failed EP:%d " "%s.\n"), endpoint_info->id, 
                               bcm_errmsg(rv)));
                    return (rv);
                }
            }
        }
    }
    rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, hash_data);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM Error: Hash table insert failed EP=%d %s.\n"),
                 endpoint_info->id, bcm_errmsg(rv)));
        _bcm_kt2_oam_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    if (1 == remote) {
        rv = _bcm_oam_kt2_remote_mep_hw_set(unit, endpoint_info);
        if (BCM_FAILURE(rv)) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Remote MEP set failed EP=%d %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
    } else {
        if (mep_ccm_rx) {
            rv = _bcm_kt2_oam_local_rx_mep_hw_set(unit, endpoint_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Rx config failed for EP=%d %s.\n"),
                           endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, up_mep,
                                              hash_key, hash_data);
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }
        if (mep_ccm_tx) {
            rv = _bcm_oam_kt2_local_tx_mep_hw_set(unit, endpoint_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Tx config failed for EP=%d %s.\n"),
                           endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, up_mep,
                                              hash_key, hash_data);
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }

        rv = _bcm_kt2_oam_port_table_key_update(unit, PORT_TABm, hash_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint create (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        rv = _bcm_kt2_oam_port_table_key_update(unit, EGR_PORTm, hash_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint create (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        /* Set OAM LM cng and cpu data control from LM flags*/
        if(hash_data->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
            _bcm_kt2_oam_lm_cng_cpu_ctrl_set(unit, endpoint_info); 
        }
    }

    rv = _bcm_kt2_oam_group_ep_list_add(unit, endpoint_info->group,
            endpoint_info->id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: failed to add entry to ep list "
                              "for EP=%d %s.\n"), endpoint_info->id, bcm_errmsg(rv)));
        _bcm_kt2_oam_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_add(unit, hash_data);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: failed to add mapping to ma_idx_to_ep_id list "
                            "for EP=%d %s.\n"), unit, endpoint_info->id, bcm_errmsg(rv)));
        _bcm_kt2_oam_endpoint_cleanup(unit, up_mep, hash_key, hash_data);
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
    _BCM_OAM_UNLOCK(oc);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_kt2_oam_hw_ccm_tx_ctr_get
 * Purpose:
 *      Update CCM CTR params
 * Parameters:
 *      unit - (IN) Unit number.
 *      lmep_1_entry - (IN) lemp_1 entry read from HW
 *      ep_info - (OUT) Pointer to get endpoint parameters
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
void
bcm_kt2_oam_hw_ccm_tx_ctr_get(int                       unit, 
                                lmep_1_entry_t          *lmep_1_entry,
                                bcm_oam_endpoint_info_t *endpoint_info)
{
    int num_counter = 0;
    uint32 counter_base_id = 0;

    if(soc_LMEP_1m_field32_get(unit, lmep_1_entry, COUNTER_1_ACTIONf)) {

        counter_base_id = soc_LMEP_1m_field32_get(unit, lmep_1_entry, COUNTER_1_IDf);
        endpoint_info->ccm_tx_update_lm_counter_base_id[0] = (counter_base_id & 0xFFF8);
        endpoint_info->ccm_tx_update_lm_counter_offset[0] = counter_base_id % 8;
        num_counter++;
    }

    if(soc_LMEP_1m_field32_get(unit, lmep_1_entry, COUNTER_2_ACTIONf)) {

        counter_base_id = soc_LMEP_1m_field32_get(unit, lmep_1_entry, COUNTER_2_IDf);
        endpoint_info->ccm_tx_update_lm_counter_base_id[1] = (counter_base_id & 0xFFF8) + (1 << 24);
        endpoint_info->ccm_tx_update_lm_counter_offset[1] = counter_base_id % 8;
        num_counter++;
    }

    endpoint_info->ccm_tx_update_lm_counter_size = num_counter;

    return;
}


 /*
 * Function:
 *      bcm_kt2_oam_endpoint_get
 * Purpose:
 *      Get an OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint      - (IN) Endpoint ID
 *     endpoint_info - (OUT) Pointer to OAM endpoint information buffer.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint,
                         bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_oam_hash_data_t *h_data_p;      /* Pointer to endpoint hash data.    */
    int                  rv = BCM_E_NONE;/* Operation return status.          */
    _bcm_oam_control_t   *oc;            /* Pointer to OAM control structure. */
    rmep_entry_t         rmep_entry;     /* Remote MEP entry buffer.          */
    lmep_entry_t         lmep_entry;     /* Local MEP entry buffer.           */
    lmep_1_entry_t       lmep_1_entry;   /* Local MEP entry buffer.           */
    void *entries[1];
    uint32 mem_entries[BCM_OAM_INTPRI_MAX];
    int  i = 0;
    uint16 inner_vlan_pri = 0;
#if defined(INCLUDE_BHH)
    bhh_sdk_msg_ctrl_sess_get_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;
    bcm_l3_egress_t l3_egress;
    bcm_l3_intf_t l3_intf;
#endif
    uint16 glp = 0;
    bcm_gport_t          tx_gport = 0;       /* Gport value.                     */

    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint);
    _BCM_OAM_LOCK(oc);

    h_data_p =  &oc->oam_hash_data[endpoint];
    if (NULL == h_data_p) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_INTERNAL);
    }

    if (0 == h_data_p->in_use) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_NOT_FOUND);
    }
    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM Info: Endpoint (EP=%d) remote=%d local_tx=%d"
                           "local_tx_idx=%d local_rx_en=%d local_rx_idx=%d oam_domain=%d\n"),
               endpoint, h_data_p->is_remote, h_data_p->local_tx_enabled,
               h_data_p->local_tx_index, h_data_p->local_rx_enabled,
               h_data_p->local_rx_index, h_data_p->oam_domain));
    if (bcmOAMEndpointTypeEthernet == h_data_p->type) {
        if (1 == h_data_p->is_remote) {

            sal_memset(&rmep_entry, 0, sizeof(rmep_entry_t));

            /* Get hardware table entry information.  */
            rv = READ_RMEPm(unit, MEM_BLOCK_ANY, h_data_p->remote_index,
                            &rmep_entry);
            if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: RMEP table read failed for"
                                           " EP=%d %s.\n"), endpoint, bcm_errmsg(rv)));
                    _BCM_OAM_UNLOCK(oc);
                    return (rv);
            }

            rv = _bcm_kt2_oam_read_clear_faults(unit, h_data_p->remote_index,
                                            RMEPm, (uint32 *) &rmep_entry,
                                            (void *) endpoint_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: RMEP table read failed for"
                                       " EP=%d %s.\n"), endpoint, bcm_errmsg(rv)));
                    _BCM_OAM_UNLOCK(oc);
                    return (rv);
            }
            if (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_GET_FAULTS_ONLY) {
                endpoint_info->flags2 &= ~BCM_OAM_ENDPOINT_FLAGS2_GET_FAULTS_ONLY;
                _BCM_OAM_UNLOCK(oc);
                return BCM_E_NONE;
            }
        } else {
            if (1 == h_data_p->local_tx_enabled) {

                sal_memset(&lmep_entry, 0, sizeof(lmep_entry_t));

                /* Get hardware table entry information.  */
                rv = READ_LMEPm(unit, MEM_BLOCK_ANY, h_data_p->local_tx_index,
                                &lmep_entry);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LMEP table read failed for EP=%d"
                                           " %s.\n"), endpoint, bcm_errmsg(rv)));
                    _BCM_OAM_UNLOCK(oc);
                    return (rv);
                }

                soc_LMEPm_mac_addr_get(unit, &lmep_entry, SAf,
                                   endpoint_info->src_mac_address);

                endpoint_info->pkt_pri
                    = soc_LMEPm_field32_get(unit, &lmep_entry, PRIORITYf);

                endpoint_info->port_state
                    = (soc_LMEPm_field32_get
                        (unit, &lmep_entry, PORT_TLVf)
                        ? BCM_OAM_PORT_TLV_UP : BCM_OAM_PORT_TLV_BLOCKED);
                inner_vlan_pri = soc_LMEPm_field32_get(unit, &lmep_entry, CVLAN_TAGf);
                endpoint_info->inner_vlan = 0xFFF & inner_vlan_pri;
                endpoint_info->inner_pkt_pri = inner_vlan_pri >> 13;
                sal_memset(&lmep_1_entry, 0, sizeof(lmep_1_entry_t));

                /* Get hardware table entry information.  */
                rv = READ_LMEP_1m(unit, MEM_BLOCK_ANY, h_data_p->local_tx_index,
                                &lmep_1_entry);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LMEP_1 table read failed "
                                          "for EP=%d"" %s.\n"), endpoint, bcm_errmsg(rv)));
                    _BCM_OAM_UNLOCK(oc);
                    return (rv);
                }
                soc_LMEP_1m_mac_addr_get(unit, &lmep_1_entry, DAf,
                                     endpoint_info->dst_mac_address);

                if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
                    endpoint_info->int_pri = soc_LMEP_1m_field32_get(unit,
                            &lmep_1_entry, INT_PRIf);
                } else {
                    endpoint_info->int_pri = h_data_p->int_pri;
                }

                endpoint_info->interface_state
                   = soc_LMEP_1m_field32_get(unit, &lmep_1_entry, INTERFACE_TLVf);

                bcm_kt2_oam_hw_ccm_tx_ctr_get(unit, &lmep_1_entry, endpoint_info);
                glp = soc_LMEP_1m_field32_get(unit, &lmep_1_entry, PP_PORTf);
                _BCM_KT2_SUBPORT_PORT_ID_SET(tx_gport, glp);
                if (BCM_PBMP_MEMBER(SOC_INFO(unit).general_pp_port_pbm, glp)) {
                    _BCM_KT2_SUBPORT_PORT_TYPE_SET(tx_gport, _BCM_KT2_SUBPORT_TYPE_GENERAL);
                    endpoint_info->tx_gport = tx_gport;
                    endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_VLAN_VP_UP_MEP_IN_HW;
                }
            }
        }
    }
    else if (soc_feature(unit, soc_feature_bhh) &&
             BHH_EP_TYPE(h_data_p)) {
#if defined(INCLUDE_BHH)
        if (!(h_data_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU)) {
            sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint);
            rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                    MOS_MSG_SUBCLASS_BHH_SESS_GET,
                    sess_id, 0,
                    MOS_MSG_SUBCLASS_BHH_SESS_GET_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: ukernel msg failed for"
                                    " EP=%d %s.\n"), unit, endpoint, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }

            /* Pack control message data into DMA buffer */
            buffer = oc->dma_buffer;
            buffer_ptr = bhh_sdk_msg_ctrl_sess_get_unpack(buffer, &msg);
            buffer_len = buffer_ptr - buffer;

            if (reply_len != buffer_len) {
                rv = BCM_E_INTERNAL;
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: ukernel msg failed for"
                                    " EP=%d %s.\n"), unit, endpoint, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            } else {
                endpoint_info->int_pri = msg.tx_cos;
                endpoint_info->pkt_pri = msg.tx_pri;
                endpoint_info->mpls_exp = msg.priority;

                if(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE){
                    endpoint_info->name       = msg.remote_mep_id;
                    endpoint_info->ccm_period = msg.remote_period;
                } else {
                    endpoint_info->name       = msg.mep_id;
                    endpoint_info->ccm_period = msg.local_period;
                }

                if(msg.fault_flags & BHH_BTE_EVENT_CCM_TIMEOUT){
                    endpoint_info->faults |= BCM_OAM_BHH_FAULT_CCM_TIMEOUT;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CCM_RDI){
                    endpoint_info->faults |= BCM_OAM_BHH_FAULT_CCM_RDI;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL){
                    endpoint_info->faults |=
                                 BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID){
                    endpoint_info->faults |=
                                 BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID){
                    endpoint_info->faults |=
                                 BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD){
                    endpoint_info->faults |=
                                 BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY){
                    endpoint_info->faults |=
                                 BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CSF_LOS) {
                    endpoint_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_LOS;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CSF_FDI) {
                    endpoint_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_FDI;
                }
                if(msg.fault_flags & BHH_BTE_EVENT_CSF_RDI) {
                    endpoint_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_RDI;
                }
            }
        }else {
            endpoint_info->ccm_period = h_data_p->period;
            endpoint_info->int_pri = h_data_p->int_pri;
            endpoint_info->pkt_pri = h_data_p->vlan_pri;
        }

        endpoint_info->intf_id    = h_data_p->egress_if;
        endpoint_info->cpu_qid    =  h_data_p->cpu_qid;
        endpoint_info->mpls_label = h_data_p->label;
        endpoint_info->gport      = h_data_p->gport;
        endpoint_info->vpn        = h_data_p->vpn;
        endpoint_info->vccv_type  = h_data_p->vccv_type;
        endpoint_info->egress_label.label = h_data_p->egr_label;
        endpoint_info->egress_label.exp = h_data_p->egr_label_exp;
        endpoint_info->egress_label.ttl = h_data_p->egr_label_ttl;
        endpoint_info->inner_pkt_pri = h_data_p->inner_vlan_pri;
        if (!BHH_EP_MPLS_SECTION_TYPE(h_data_p)) {
            /*
            * Get MAC address
            */
            bcm_l3_egress_t_init(&l3_egress);
            bcm_l3_intf_t_init(&l3_intf);

            if (BCM_FAILURE
                (bcm_esw_l3_egress_get(unit, h_data_p->egress_if, &l3_egress))) {
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_INTERNAL);
            }

            l3_intf.l3a_intf_id = l3_egress.intf;
            if (BCM_FAILURE(bcm_esw_l3_intf_get(unit, &l3_intf))) {
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_INTERNAL);
            }

            sal_memcpy(endpoint_info->src_mac_address, l3_intf.l3a_mac_addr,
                   _BHH_MAC_ADDR_LENGTH);
        } else {
            sal_memcpy(endpoint_info->src_mac_address, h_data_p->src_mac_address,
                _BHH_MAC_ADDR_LENGTH);
            sal_memcpy(endpoint_info->dst_mac_address, h_data_p->dst_mac_address,
                _BHH_MAC_ADDR_LENGTH);
        }
#else
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_UNAVAIL);
#endif /* INCLUDE_BHH */
    }
    else {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_PARAM);
    }

    /* BHH MEP Id & period already filled from uC msg */
    if (!BHH_EP_TYPE(h_data_p)) {
        endpoint_info->name = h_data_p->name;
        endpoint_info->ccm_period = h_data_p->period;
    }

    endpoint_info->id = endpoint;
    endpoint_info->group = h_data_p->group_index;
    endpoint_info->vlan = h_data_p->vlan;
    endpoint_info->inner_vlan = h_data_p->inner_vlan;
    endpoint_info->level = h_data_p->level;
    endpoint_info->gport = h_data_p->gport;
    endpoint_info->trunk_index = h_data_p->trunk_index;
    endpoint_info->flags |= h_data_p->flags;
    endpoint_info->flags &= ~(BCM_OAM_ENDPOINT_WITH_ID);
    endpoint_info->opcode_flags = h_data_p->opcode_flags;
    endpoint_info->type = h_data_p->type;
    endpoint_info->lm_counter_base_id = h_data_p->rx_ctr;
    endpoint_info->outer_tpid = h_data_p->outer_tpid;
    endpoint_info->inner_tpid = h_data_p->inner_tpid;
    endpoint_info->subport_tpid = h_data_p->subport_tpid;
    endpoint_info->timestamp_format = h_data_p->ts_format;
    entries[0] = &mem_entries;
    if (h_data_p->pri_map_index != _BCM_OAM_INVALID_INDEX) {
        rv = soc_profile_mem_get(unit, &oc->ing_service_pri_map,
                           (h_data_p->pri_map_index * BCM_OAM_INTPRI_MAX),
                           BCM_OAM_INTPRI_MAX, (void *)entries);
        for (i = 0; i < BCM_OAM_INTPRI_MAX ; i++) {
            if (SOC_MEM_FIELD_VALID(unit, ING_SERVICE_PRI_MAPm, OFFSET_VALIDf)) {
                soc_mem_field32_set(unit, ING_SERVICE_PRI_MAPm, &mem_entries[i],
                                    OFFSET_VALIDf, 0);
                endpoint_info->pri_map[i] = mem_entries[i];
            }
        }
        endpoint_info->pri_map_id = h_data_p->pri_map_index;
    }
    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_kt2_oam_endpoint_destroy
 * Purpose:
 *     Destroy an OAM endpoint object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     endpoint - (IN) Endpoint ID to destroy.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    _bcm_oam_control_t *oc; /* Pointer to OAM control structure. */
    int rv;                 /* Operation return status.          */

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    /* Validate endpoint index value. */
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint);

    _BCM_OAM_LOCK(oc);

    rv = _bcm_kt2_oam_endpoint_destroy(unit, endpoint);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint destroy EP=%d failed - "
                               "%s.\n"), endpoint, bcm_errmsg(rv)));
    }

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_endpoint_destroy_all
 * Purpose:
 *     Destroy all OAM endpoint objects associated with a group.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     group - (IN) The OAM group whose endpoints should be destroyed
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group)
{
    _bcm_oam_control_t    *oc; /* Pointer to OAM control structure. */
    int                   rv;  /* Operation return status.          */
    _bcm_oam_group_data_t *g_info_p;

    /* Get OAM device control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Validate group index. */
    _BCM_OAM_GROUP_INDEX_VALIDATE(group);

    _BCM_OAM_LOCK(oc);

    /* Check if the group is in use. */
    rv = shr_idxres_list_elem_state(oc->group_pool, group);
    if (BCM_E_EXISTS != rv) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Group ID=%d does not exist.\n"),
                   group));
        return (rv);
    }

    /* Get the group data pointer. */
    g_info_p = &oc->group_info[group];
    rv = _bcm_kt2_oam_group_endpoints_destroy(unit, g_info_p);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Group (GID=%d) endpoints destroy"
                               " failed - %s.\n"), group, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_rmep_endpoint_traverse
 * Purpose:
 *     Traverse the set of OAM rmep endpoints associated with the
 *     specified level in group, calling a specified callback for each one
 * Parameters:
 *     unit      - (IN) BCM device number
 *     level     - (IN) The OAM group's MDL whose endpoints should be traversed
 *     tail      - (IN) A pointer to tail node of endpoint list.
 *     cb        - (IN) A pointer to the callback function to call for each OAM
 *                      endpoint in the specified group
 *     user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oam_rmep_endpoint_traverse(int unit, int level,
                                   _bcm_oam_ep_list_t *tail,
                                   bcm_oam_endpoint_traverse_cb cb,
                                   void *user_data)
{
    bcm_oam_endpoint_info_t ep_info;
    _bcm_oam_hash_data_t    *h_data_p;
    int                     rv = BCM_E_NONE;  /* Operation return status. */

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    while (NULL != tail) {
        h_data_p = tail->ep_data_p;

        bcm_oam_endpoint_info_t_init(&ep_info);

        rv = bcm_kt2_oam_endpoint_get(unit, h_data_p->ep_id, &ep_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EP=%d info get failed %s.\n"),
                       h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }

        if (!(ep_info.flags & BCM_OAM_ENDPOINT_REMOTE)) {
            tail = tail->prev;
            continue;
        }

        if (ep_info.level != level) {
            tail = tail->prev;
            continue;
        }

        rv = cb(unit, &ep_info, user_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EP=%d callback failed - %s.\n"),
                       h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }
        tail = tail->prev;
    }

    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_endpoint_traverse
 * Purpose:
 *     Traverse the set of OAM endpoints associated with the
 *     specified group, calling a specified callback for each one
 * Parameters:
 *     unit      - (IN) BCM device number
 *     group     - (IN) The OAM group whose endpoints should be traversed
 *     cb        - (IN) A pointer to the callback function to call for each OAM
 *                      endpoint in the specified group
 *     user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oam_endpoint_traverse(int unit, bcm_oam_group_t group,
                              bcm_oam_endpoint_traverse_cb cb,
                              void *user_data)
{
    _bcm_oam_control_t      *oc; /* Pointer to OAM control structure. */
    int                     rv;  /* Operation return status.          */
    bcm_oam_endpoint_info_t ep_info;
    _bcm_oam_hash_data_t    *h_data_p;
    _bcm_oam_ep_list_t      *cur, *tail;
    _bcm_oam_group_data_t   *g_info_p;
    int                     rmep_orgniz_skip = 0;
                                           /* To retain original behavior     */
                                           /* during wb upgrade from recovery */
                                           /* version 1.5 to 1.6              */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Get OAM device control structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Validate group index. */
    _BCM_OAM_GROUP_INDEX_VALIDATE(group);

    _BCM_OAM_LOCK(oc);

    /* Check if the group is in use. */
    rv = shr_idxres_list_elem_state(oc->group_pool, group);
    if (BCM_E_EXISTS != rv) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Group ID=%d does not exist.\n"),
                   group));
        return (rv);
    }

    /* Get the group data pointer. */
    g_info_p = &oc->group_info[group];

    /* Get the endpoint list head pointer. */
    cur = *(g_info_p->ep_list);
    if (NULL == cur) {
        _BCM_OAM_UNLOCK(oc);
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: No endpoints in group GID=%d.\n"),
                   group));
        return (BCM_E_NONE);
    }

    /* Traverse to the tail of the list. */
    while (NULL != cur->next) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Info: GID=%d EP:%d.\n"),
                   cur->ep_data_p->group_index, cur->ep_data_p->ep_id));
        cur = cur->next;
    }
    tail = cur;

    while (NULL != cur) {
        h_data_p = cur->ep_data_p;
        if (NULL == h_data_p) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Group=%d endpoints access failed -"
                                   " %s.\n"), group, bcm_errmsg(BCM_E_INTERNAL)));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INTERNAL);
        }
        bcm_oam_endpoint_info_t_init(&ep_info);

        rv = bcm_kt2_oam_endpoint_get(unit, h_data_p->ep_id, &ep_info);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EP=%d info get failed %s.\n"),
                       h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }

        /* Skip RMEP Entries */
        if (ep_info.flags & BCM_OAM_ENDPOINT_REMOTE) {
            if (ep_info.vlan || ep_info.gport) {
                cur = cur->prev;
                continue;
            } else {
                rmep_orgniz_skip = 1;
            }
        }

        rv = cb(unit, &ep_info, user_data);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: EP=%d callback failed - %s.\n"),
                       h_data_p->ep_id, bcm_errmsg(rv)));
            return (rv);
        }

        /* Traverse RMEP Entries */
        if (!rmep_orgniz_skip) {
            rv = bcm_kt2_oam_rmep_endpoint_traverse(unit, ep_info.level, tail, cb, user_data);
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }
        cur = cur->prev;
    }
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_event_register
 * Purpose:
 *     Register a callback for handling OAM events
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM events for which the specified
 *                        callback should be called.
 *     cb          - (IN) A pointer to the callback function to call for
 *                        the specified OAM events
 *     user_data   - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oam_event_register(int unit, bcm_oam_event_types_t event_types,
                           bcm_oam_event_cb cb, void *user_data)
{
    _bcm_oam_control_t       *oc;
    _bcm_oam_event_handler_t *event_h_p;
    _bcm_oam_event_handler_t *prev_p = NULL;
    bcm_oam_event_type_t     e_type;
    uint32                   rval;
    int                      hw_update = 0;
    uint32                   event_bmp;
    int                      rv;             /* Operation return status. */

    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Check if an event is set for register in the events bitmap. */
    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, event_bmp);
    if (0 == event_bmp) {
        /* No events specified. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: No events specified for register.\n")));
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }

    if (NULL == event_h_p) {

        _BCM_OAM_ALLOC(event_h_p, _bcm_oam_event_handler_t,
             sizeof(_bcm_oam_event_handler_t), "OAM event handler");

        if (NULL == event_h_p) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Event handler alloc failed -"
                                   " %s.\n"), bcm_errmsg(BCM_E_MEMORY)));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_MEMORY);
        }

        event_h_p->next_p = NULL;
        event_h_p->cb = cb;

        SHR_BITCLR_RANGE(event_h_p->event_types.w, 0, bcmOAMEventCount);
        if (prev_p != NULL) {
            prev_p->next_p = event_h_p;
        } else {
            oc->event_handler_list_p = event_h_p;
        }
    }

    rv = READ_CCM_INTERRUPT_CONTROLr(unit, &rval);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: CCM interrupt control read failed -"
                               " %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    for (e_type = 0; e_type < bcmOAMEventCount; ++e_type) {
        if (SHR_BITGET(event_types.w, e_type)) {
            if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
                /*
                * BHH events are generated by the uKernel
                */
                if ((e_type == bcmOAMEventBHHLBTimeout) ||
                    (e_type == bcmOAMEventBHHLBDiscoveryUpdate) ||
                    (e_type == bcmOAMEventBHHCCMTimeout) ||
                    (e_type == bcmOAMEventBHHCCMTimeoutClear) ||
                    (e_type == bcmOAMEventBHHCCMState) ||
                    (e_type == bcmOAMEventBHHCCMRdi) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevel) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriod) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriority) ||
                    (e_type == bcmOAMEventBHHCCMRdiClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevelClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriodClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriorityClear) ||
                    (e_type == bcmOAMEventCsfLos) ||
                    (e_type == bcmOAMEventCsfFdi) ||
                    (e_type == bcmOAMEventCsfRdi) ||
                    (e_type == bcmOAMEventBhhPmCounterRollover) ||
                    (e_type == bcmOAMEventCsfDci)) {
                    SHR_BITSET(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] += 1;
                    continue;
                }
#endif
            }
            if (!soc_reg_field_valid
                    (unit, CCM_INTERRUPT_CONTROLr,
                     _kt2_oam_intr_en_fields[e_type].field)) {
                continue;
            }

            if (!SHR_BITGET(event_h_p->event_types.w, e_type)) {
                /* Add this event to the registered events list. */
                SHR_BITSET(event_h_p->event_types.w, e_type);
                oc->event_handler_cnt[e_type] += 1;
                if (1 == oc->event_handler_cnt[e_type]) {
                    hw_update = 1;
                     soc_reg_field_set
                        (unit, CCM_INTERRUPT_CONTROLr, &rval,
                         _kt2_oam_intr_en_fields[e_type].field, 1);
                }
            }
        }
    }

    event_h_p->user_data = user_data;

    if (1 == hw_update) {
        rv = WRITE_CCM_INTERRUPT_CONTROLr(unit, rval);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: CCM interrupt control write failed -"
                                   " %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
    }
    if (soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        /*
         * Update BHH Events mask
         */
        rv = _bcm_kt2_oam_bhh_event_mask_set(unit);
#endif /* INCLUDE_BHH */
    }

    _BCM_OAM_UNLOCK(oc);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_kt2_oam_event_unregister
 * Purpose:
 *     Remove a registered event from the event handler list.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM events for which the specified
 *                        callback should not be called
 *     cb          - (IN) A pointer to the callback function to unregister
 *                        from the specified OAM events
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_oam_event_unregister(int unit, bcm_oam_event_types_t event_types,
                             bcm_oam_event_cb cb)
{
    _bcm_oam_control_t       *oc;
    _bcm_oam_event_handler_t *event_h_p;
    _bcm_oam_event_handler_t *prev_p = NULL;
    bcm_oam_event_type_t     e_type;
    uint32                   rval;
    int                      hw_update = 0;
    uint32                   event_bmp;
    int                      rv;             /* Operation return status. */
  
    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Check if an event is set for unregister in the events bitmap. */
    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, event_bmp);
    if (0 == event_bmp) {
        /* No events specified. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: No events specified for register.\n")));
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }

    if (NULL == event_h_p) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_NOT_FOUND);
    }
 
    rv = READ_CCM_INTERRUPT_CONTROLr(unit, &rval);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: CCM interrupt control read failed -"
                               " %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    for (e_type = 0; e_type < bcmOAMEventCount; ++e_type) {

        if (SHR_BITGET(event_types.w, e_type)) {
            if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
                /*
                * BHH events are generated by the uKernel
                */
                if ((e_type == bcmOAMEventBHHLBTimeout) ||
                    (e_type == bcmOAMEventBHHLBDiscoveryUpdate) ||
                    (e_type == bcmOAMEventBHHCCMTimeout) ||
                    (e_type == bcmOAMEventBHHCCMTimeoutClear) ||
                    (e_type == bcmOAMEventBHHCCMState) ||
                    (e_type == bcmOAMEventBHHCCMRdi) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevel) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriod) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriority) ||
                    (e_type == bcmOAMEventBHHCCMRdiClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevelClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriodClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriorityClear) ||
                    (e_type == bcmOAMEventCsfLos) ||
                    (e_type == bcmOAMEventCsfFdi) ||
                    (e_type == bcmOAMEventCsfRdi) ||
                    (e_type == bcmOAMEventCsfDci) ||
                    (e_type == bcmOAMEventBhhPmCounterRollover)) {
                    SHR_BITCLR(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] -= 1;
                    continue;
                }
#endif
            }

            if (!soc_reg_field_valid
                    (unit, CCM_INTERRUPT_CONTROLr,
                     _kt2_oam_intr_en_fields[e_type].field)) {
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_UNAVAIL);
            }

            if ((oc->event_handler_cnt[e_type] > 0)
                && SHR_BITGET(event_h_p->event_types.w, e_type)) {

                /* Remove this event from the registered events list. */
                SHR_BITCLR(event_h_p->event_types.w, e_type);

                oc->event_handler_cnt[e_type] -= 1;

                if (0 == oc->event_handler_cnt[e_type]) {
                    hw_update = 1;
                     soc_reg_field_set
                        (unit, CCM_INTERRUPT_CONTROLr, &rval,
                         _kt2_oam_intr_en_fields[e_type].field, 0);
                }
            }
        }
    }

    if (1 == hw_update) {
        rv = WRITE_CCM_INTERRUPT_CONTROLr(unit, rval);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: CCM interrupt control write failed -"
                                   " %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
    }

    SHR_BITTEST_RANGE(event_h_p->event_types.w, 0, bcmOAMEventCount, event_bmp);

    if (0 == event_bmp) {

        if (NULL != prev_p) {

            prev_p->next_p = event_h_p->next_p;
            
        } else {

            oc->event_handler_list_p = event_h_p->next_p;

        }
        sal_free(event_h_p);
    }
    if(soc_feature(unit, soc_feature_bhh)) {
#if defined(INCLUDE_BHH)
        /*
         * Update BHH Events mask
         */
        rv = _bcm_kt2_oam_bhh_event_mask_set(unit);
#endif /* INCLUDE_BHH */
    }
    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_kt2_oam_endpoint_action_set
 * Purpose:
 *     Remove a registered event from the event handler list.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     endpoint    - (IN) Endpoint id
 *     action      - (IN) Set of OAM endpoint actions, for a set of
 *                        opcodes specified.
 * Returns:
 *      BCM_E_XXX
 *  In order to put the opcodes into opcode group1 and group2, first call the
 *  action set API with opcode action and set of opcodes that need to use 
 *  this action. This will put all these opcodes under group1. To put
 *  the opcode to group2, call the set API with an action(with all the opcodes 
 *  that need this action) which is differing compared to group1. This will 
 *  classify the opcode to group2 
 */
int bcm_kt2_oam_endpoint_action_set(int unit, bcm_oam_endpoint_t endpoint, 
                                    bcm_oam_endpoint_action_t *action) 
{
    int            rv = BCM_E_NONE;  /* Operation return status.          */
    _bcm_oam_hash_data_t *h_data_p;  /* Pointer to endpoint hash data.    */
    _bcm_oam_control_t   *oc;        /* Pointer to OAM control structure. */
    void                 *entries[1]; 
    oam_opcode_control_profile_entry_t opcode_profile;
    egr_oam_opcode_control_profile_entry_t egr_opcode_profile;
    soc_profile_mem_t    *profile_mem_ptr;
    soc_mem_t     ma_index_mem = MA_INDEXm;
    ma_index_entry_t     ma_idx_entry;   /* MA_INDEX table entry buffer.     */
    egr_ma_index_entry_t egr_ma_idx_entry; /* EGR_MA_INDEX table entry buffer*/
    void                 *ma_idx_ptr;
    int                  ma_offset = 0;
    uint32               old_profile_index = 0;
    char                 free_old_profile = 0;
    /*setting default value to 0 whcih will be converted to 1 
     * if opcode control profile set is touched */
    uint8                opcode_profile_changed = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint);

    sal_memset(&opcode_profile, 0, sizeof(oam_opcode_control_profile_entry_t));
    sal_memset(&egr_opcode_profile, 0, 
                             sizeof(egr_oam_opcode_control_profile_entry_t));

    _BCM_OAM_LOCK(oc);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->mep_pool, endpoint);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   endpoint, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p =  &oc->oam_hash_data[endpoint];
    if (NULL == h_data_p) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_INTERNAL);
    }

    if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        entries[0] = &egr_opcode_profile;
        profile_mem_ptr =  &oc->egr_oam_opcode_control_profile;
        ma_index_mem = EGR_MA_INDEXm;
        ma_idx_ptr = &egr_ma_idx_entry; 
    } else {
        entries[0] = &opcode_profile;
        profile_mem_ptr =  &oc->oam_opcode_control_profile;
        ma_idx_ptr = &ma_idx_entry; 
    }

    /* Get MA_INDEX offset  */
    rv = _bcm_kt2_oam_ma_index_offset_get(unit, h_data_p, &ma_offset);

    h_data_p->local_rx_index = h_data_p->ma_base_index + ma_offset;
    
    /* Using the profile index, get the profile */
    if ((h_data_p->profile_index == _BCM_OAM_INVALID_INDEX) ||
        (h_data_p->profile_index == 0)) {
      /* No profile exists or default profile is used, create one */
      /* convert action into opcode profile entry */
        rv = _bcm_kt2_oam_convert_action_to_opcode_entry(unit, action, 
                                                    endpoint, 
                                                    entries[0],
                                                    &opcode_profile_changed);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Failed to convert action to "
                                  "profile -" " %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
        if(opcode_profile_changed) {
            rv = soc_profile_mem_add(unit, profile_mem_ptr,
                    (void *)entries, 1, 
                    (uint32 *) &h_data_p->profile_index);
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM Error: Soc profile mem add failed -"
                                    " %s.\n"), bcm_errmsg(rv)));
                return (rv);
            }
        }
    } else {
        free_old_profile = 1;
        old_profile_index = h_data_p->profile_index;

        rv = soc_profile_mem_get(unit, profile_mem_ptr,
                                 old_profile_index, 1, entries);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Soc profile mem get failed -"
                                   " %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
        /* convert action into opcode profile entry */
        _bcm_kt2_oam_convert_action_to_opcode_entry(unit, action, 
                                                    endpoint, 
                                                    entries[0],
                                                    &opcode_profile_changed);

        /* Add new profile */
        rv = soc_profile_mem_add(unit, profile_mem_ptr,
                                 (void *)entries, 1, 
                                 (uint32 *) &h_data_p->profile_index);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Soc profile mem add failed -"
                                   " %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }
    } 
    /* Set OAM opcode control profile table index. */
    rv = soc_mem_read(unit, ma_index_mem, MEM_BLOCK_ANY, 
                      h_data_p->local_rx_index, ma_idx_ptr);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MA index table entry  read failed  "
                              "%s.\n"), bcm_errmsg(rv)));
        return (rv);
    }
    soc_mem_field32_set(unit, ma_index_mem, ma_idx_ptr,
                              OAM_OPCODE_CONTROL_PROFILE_PTRf,
                              h_data_p->profile_index);
    rv = soc_mem_write(unit, ma_index_mem, MEM_BLOCK_ALL,
                  h_data_p->local_rx_index, ma_idx_ptr);

    /* Delete old profile once ma_index is pointing to new one*/
    if(free_old_profile) {
        rv = soc_profile_mem_delete(unit, profile_mem_ptr,
                                    old_profile_index);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Soc profile mem delete failed -"
                                   " %s.\n"), bcm_errmsg(rv)));
            return (rv);
        }

    }

    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *     _kt2_oam_opcode_control_field_set
 * Purpose:
 *   Set the fields of opcode profile entry 
 * Parameters:
 *     unit        - (IN) BCM device number
 *     field_index - (IN) Opcode profile action to be set
 *     mem         - (IN) Opcode profile memory - ing/egr
 *     oam_opcode  - (IN) Oam opcode for which action needs to be set
 *     value       - (IN) value of the opcode action
 *     profile     - (OUT) Pointer to the opcode profile table index.
 * Returns:
 *      BCM_E_XXX
 */
int
_kt2_oam_opcode_control_field_set(int unit, bcm_oam_action_type_t field_index, 
                                  soc_mem_t mem, int oam_opcode, int value, 
                                  void *profile) 
{
    _bcm_oam_control_t   *oc;        /* Pointer to OAM control structure. */
    ingress_oam_opcode_group_entry_t entry;
    int grp_1_modified = 0;
    int grp_2_modified = 0;
    uint32 opcode_bmp_1 = 0;
    uint32 opcode_bmp_2 = 0;
    soc_field_t cfm_field[bcmOAMActionCount+1][_BCM_OAM_OPCODE_TYPE_NON_CFM_FIRST] =  { 
                   /* bcmOAMActionCountEnable */ 
                 { 0, 0, 0, 0, 0, 0}, 
                   /* bcmOAMActionMeterEnable */ 
                 { 0, 0, 0, 0, 0, 0}, 
                 /* bcmOAMActionDrop */
                 { 0, CCM_DROPf, LBR_DROPf, LBM_UC_DROPf, 
                      LTR_DROPf, LTM_DROPf },    
                  /* bcmOAMActionCopyToCpu */
                 { 0, CCM_COPYTO_CPUf, LBR_COPYTO_CPUf, LBM_UC_COPYTO_CPUf, 
                      LTR_COPYTO_CPUf, LTM_COPYTO_CPUf},
                  /* bcmOAMActionFwdAsData */
                 { 0, 0, 0, 0, 0, 0 },
                   /* bcmOAMActionFwd */
                 { 0, FWD_LMEP_PKTf, LBR_FWDf, LBM_ACTIONf, LTR_FWDf, LTM_FWDf},
                 /* bcmOAMActionUcDrop */
                 { 0, 0, 0, LBM_UC_DROPf, 0, 0}, 
                  /* bcmOAMActionUcCopyToCpu */
                 { 0, 0, 0, LBM_UC_COPYTO_CPUf, 0, 0},
                  /* bcmOAMActionUcFwdAsData */
                 { 0, 0, 0, 0, 0, 0 },
                  /* bcmOAMActionUcFwd */
                 { 0, 0, 0, LBM_ACTIONf, 0, 0},
                 /* bcmOAMActionMcDrop */
                 { 0, 0, 0, LBM_MC_DROPf, 0, 0},
                    /* bcmOAMActionMcCopyToCpu */
                 { 0, 0, 0, LBM_MC_COPYTO_CPUf, 0, 0},
                    /* bcmOAMActionMcFwdAsData */
                 { 0, 0, 0, 0, 0, 0},
                   /* bcmOAMActionMcFwd*/
                 { 0, 0, 0, LBM_MC_FWDf, 0, 0},
                   /* bcmOAMActionLowMdlDrop */
                 { 0, LOW_MDL_CCM_FWD_ACTIONf, LOW_MDL_LB_LT_DROPf, 
                      LOW_MDL_LB_LT_DROPf, LOW_MDL_LB_LT_DROPf, 
                      LOW_MDL_LB_LT_DROPf },
                  /*  bcmOAMActionLowMdlCopyToCpu */
                 { 0, LOW_MDL_CCM_COPYTO_CPUf, LOW_MDL_LB_LT_COPYTO_CPUf, 
                      LOW_MDL_LB_LT_COPYTO_CPUf, 
                      LOW_MDL_LB_LT_COPYTO_CPUf, LOW_MDL_LB_LT_COPYTO_CPUf}, 
                  /* bcmOAMActionLowMdlFwdAsData */
                 { 0, LOW_MDL_CCM_FWD_ACTIONf, LOW_MDL_LB_LT_DROPf, 
                      LOW_MDL_LB_LT_DROPf, 
                      LOW_MDL_LB_LT_DROPf, LOW_MDL_LB_LT_DROPf}, 
                   /* bcmOAMActionLowMdlFwd*/
                 { 0, 0, LOW_MDL_LB_LT_FWDf, LOW_MDL_LB_LT_FWDf, 0, 0},
                    /* bcmOAMActionMyStationMissCopyToCpu */
                 { 0, 0, LB_LT_UC_MY_STATION_MISS_COPYTO_CPUf, 
                   LB_LT_UC_MY_STATION_MISS_COPYTO_CPUf,
                   LB_LT_UC_MY_STATION_MISS_COPYTO_CPUf,
                   LB_LT_UC_MY_STATION_MISS_COPYTO_CPUf},
                    /* bcmOAMActionMyStationMissDrop */
                 { 0, 0, LB_LT_UC_MY_STATION_MISS_DROPf, 
                   LB_LT_UC_MY_STATION_MISS_DROPf,
                   LB_LT_UC_MY_STATION_MISS_DROPf, 
                   LB_LT_UC_MY_STATION_MISS_DROPf},
                    /* bcmOAMActionMyStationMissFwdAsData */
                 { 0, 0, 0, 0, 0, 0},
                    /* bcmOAMActionMyStationMissFwd */
                 { 0, 0, LB_LT_UC_MY_STATION_MISS_FWDf, 
                   LB_LT_UC_MY_STATION_MISS_FWDf,
                   LB_LT_UC_MY_STATION_MISS_FWDf,
                   LB_LT_UC_MY_STATION_MISS_FWDf},
                   /* bcmOAMActionProcessInHw */
                 { 0, CCM_PROCESS_IN_HWf, 0, 0, 0, 0}, 
                   /* bcmOAMActionLowMdlCcmFwdAsRegularCcm */ 
                 { 0, LOW_MDL_CCM_FWD_ACTIONf, 0, 0, 0, 0}, 
                 { 0, 0, 0, 0, 0, 0} 
    };

    soc_field_t non_cfm_field[bcmOAMActionCount+1][2]= {
                   /* bcmOAMActionCountEnable */ 
                 { 0, 0 },
                   /* bcmOAMActionMeterEnable */ 
                 { 0, 0 },
                   /* bcmOAMActionDrop */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_HIT_ACTIONf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_HIT_ACTIONf },
                   /* bcmOAMActionCopyToCpu */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_HIT_COPYTO_CPUf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_HIT_COPYTO_CPUf },
                   /* bcmOAMActionFwdAsData */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_HIT_ACTIONf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_HIT_ACTIONf },
                   /* bcmOAMActionFwd */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_HIT_ACTIONf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_HIT_ACTIONf },
                   /* bcmOAMActionUcDrop */
                 { 0, 0 },
                   /* bcmOAMActionUcCopyToCpu */
                 { 0, 0 },
                   /* bcmOAMActionUcFwdAsData */
                 { 0, 0 },
                   /* bcmOAMActionUcFwd */
                 { 0, 0 },
                   /* bcmOAMActionMcDrop */
                 { OTHER_OPCODE_GROUP_1_MC_ACTIONf,
                   OTHER_OPCODE_GROUP_2_MC_ACTIONf },
                   /* bcmOAMActionMcCopyToCpu */
                 { OTHER_OPCODE_GROUP_1_MC_COPYTO_CPUf,
                   OTHER_OPCODE_GROUP_2_MC_COPYTO_CPUf },
                   /* bcmOAMActionMcFwdAsData */
                 { OTHER_OPCODE_GROUP_1_MC_ACTIONf,
                   OTHER_OPCODE_GROUP_2_MC_ACTIONf },
                   /* bcmOAMActionMcFwd*/
                 { OTHER_OPCODE_GROUP_1_MC_ACTIONf,
                   OTHER_OPCODE_GROUP_2_MC_ACTIONf },
                   /* bcmOAMActionLowMdlDrop */
                 { OTHER_OPCODE_GROUP_1_LOW_MDL_ACTIONf,
                   OTHER_OPCODE_GROUP_2_LOW_MDL_ACTIONf},
                   /* bcmOAMActionLowMdlCopyToCpu */
                 { OTHER_OPCODE_GROUP_1_LOW_MDL_COPYTO_CPUf,
                   OTHER_OPCODE_GROUP_2_LOW_MDL_COPYTO_CPUf },
                   /* bcmOAMActionLowMdlFwdAsData */
                 { OTHER_OPCODE_GROUP_1_LOW_MDL_ACTIONf,
                   OTHER_OPCODE_GROUP_2_LOW_MDL_ACTIONf },
                   /* bcmOAMActionLowMdlFwd*/
                 { OTHER_OPCODE_GROUP_1_LOW_MDL_ACTIONf,
                   OTHER_OPCODE_GROUP_2_LOW_MDL_ACTIONf},
                   /* bcmOAMActionMyStationMissCopyToCpu */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_MISS_COPYTO_CPUf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_MISS_COPYTO_CPUf },
                   /* bcmOAMActionMyStationMissDrop */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_MISS_ACTIONf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_MISS_ACTIONf },
                   /* bcmOAMActionMyStationMissFwdAsData */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_MISS_ACTIONf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_MISS_ACTIONf },
                   /* bcmOAMActionMyStationMissFwd */
                 { OTHER_OPCODE_GROUP_1_UC_MY_STATION_MISS_ACTIONf,
                   OTHER_OPCODE_GROUP_2_UC_MY_STATION_MISS_ACTIONf},
                   /* bcmOAMActionProcessInHw */
                 { 0, 0 },
                   /* bcmOAMActionLowMdlCcmFwdAsRegularCcm */ 
                 { 0, 0 },
                 { 0, 0 }
    };

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    SHR_BITTEST_RANGE(oc->opcode_grp_1_bmp, 0, 
                          _BCM_OAM_OPCODE_TYPE_NON_CFM_LAST, opcode_bmp_1);
    SHR_BITTEST_RANGE(oc->opcode_grp_2_bmp, 0,
                          _BCM_OAM_OPCODE_TYPE_NON_CFM_LAST, opcode_bmp_2);
    if (oam_opcode < _BCM_OAM_OPCODE_TYPE_NON_CFM_FIRST) {
        if (cfm_field[field_index][oam_opcode] != 0) {
            soc_mem_field32_set(unit, mem, profile, 
                   cfm_field[field_index][oam_opcode], value);
        }
    } else {
        if ((opcode_bmp_1 == 0) && (opcode_bmp_2 == 0)) {
           if (SHR_BITGET(oc->opcode_grp_bmp, oam_opcode)) {
               SHR_BITSET(oc->opcode_grp_1_bmp, oam_opcode);
               SHR_BITCLR(oc->opcode_grp_bmp, oam_opcode);
               opcode_bmp_1 = 1;
               grp_1_modified = 1;
           }
        }
        /* Changing the action for same opcode */
        if (SHR_BITGET(oc->opcode_grp_1_bmp, oam_opcode)) {
           if (non_cfm_field[field_index][0] != 0) {
               soc_mem_field32_set(unit, mem, profile, 
                       non_cfm_field[field_index][0], value);
           } 
        } else if (SHR_BITGET(oc->opcode_grp_2_bmp, oam_opcode)) {
           if (non_cfm_field[field_index][1] != 0) {
               soc_mem_field32_set(unit, mem, profile, 
                                non_cfm_field[field_index][1], value);
           }
        } else if (SHR_BITGET(oc->opcode_grp_bmp, oam_opcode)) {
            if (value == soc_mem_field32_get(unit, mem, profile, 
                       non_cfm_field[field_index][0])) {
               /* Add this opcode to group 1 */
                grp_1_modified = 1;
                SHR_BITSET(oc->opcode_grp_1_bmp, oam_opcode);
                SHR_BITCLR(oc->opcode_grp_bmp, oam_opcode);
                if (non_cfm_field[field_index][0] != 0) {
                    soc_mem_field32_set(unit, mem, profile, 
                                non_cfm_field[field_index][0], value);
                }
            } else if (value == soc_mem_field32_get(unit, mem, profile, 
                       non_cfm_field[field_index][1])) {
               /* Add this opcode to group 2 */
                grp_2_modified = 1;
                SHR_BITSET(oc->opcode_grp_2_bmp, oam_opcode);
                SHR_BITCLR(oc->opcode_grp_bmp, oam_opcode);
                if (non_cfm_field[field_index][1] != 0) {
                    soc_mem_field32_set(unit, mem, profile, 
                                non_cfm_field[field_index][1], value);
                }
            } else if (opcode_bmp_1 == 0) { 
                grp_1_modified = 1;
                SHR_BITSET(oc->opcode_grp_1_bmp, oam_opcode);
                SHR_BITCLR(oc->opcode_grp_bmp, oam_opcode);
                if (non_cfm_field[field_index][0] != 0) {
                    soc_mem_field32_set(unit, mem, profile, 
                                non_cfm_field[field_index][0], value);
                }
            } else if (opcode_bmp_2 == 0) { 
                grp_2_modified = 1;
                SHR_BITSET(oc->opcode_grp_2_bmp, oam_opcode);
                SHR_BITCLR(oc->opcode_grp_bmp, oam_opcode);
                if (non_cfm_field[field_index][1] != 0) {
                    soc_mem_field32_set(unit, mem, profile, 
                                non_cfm_field[field_index][1], value);
                }
            } else {
                /* Add the opcode to group 1 by default */ 
                grp_1_modified = 1;
                SHR_BITSET(oc->opcode_grp_1_bmp, oam_opcode);
                SHR_BITCLR(oc->opcode_grp_bmp, oam_opcode);
                if (non_cfm_field[field_index][0] != 0) {
                    soc_mem_field32_set(unit, mem, profile, 
                                non_cfm_field[field_index][0], value);
                }
            }
        } else {
            return (BCM_E_PARAM);
        }
    }
    if (mem == OAM_OPCODE_CONTROL_PROFILEm) {
        mem = INGRESS_OAM_OPCODE_GROUPm;
    } else {
        mem = EGR_OAM_OPCODE_GROUPm;
    }
    if (grp_1_modified) {
        soc_mem_field32_set(unit, mem, &entry, OPCODE_GROUP_VALIDf, 1);
        soc_mem_field32_set(unit, mem, &entry, OPCODE_GROUPf, 0);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, 
                                          MEM_BLOCK_ANY, oam_opcode, &entry));

    } else if(grp_2_modified) {
        soc_mem_field32_set(unit, mem, &entry, OPCODE_GROUP_VALIDf, 1);
        soc_mem_field32_set(unit, mem, &entry, OPCODE_GROUPf, 1);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, 
                                          MEM_BLOCK_ANY, oam_opcode, &entry));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_kt2_oam_resolve_dglp
 * Purpose:
 *   Add an dglp profile entry 
 * Parameters:
 *     unit        - (IN) BCM device number
 *     mem         - (IN) dglp profile memory - ing/egr
 *     dglp        - (IN) DGLP value
 *     profile     - (OUT) Pointer to the dglp profile table index.
 *     olp_enable  - (OUT) Pointer to field indicating whether olp is enabled on
 *                         the port. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_oam_resolve_dglp(int unit, soc_mem_t mem, uint32 dglp, 
                          uint32 *profile_index, int *olp_enable) 
{ 
    int rv = BCM_E_NONE;
    egr_olp_dgpp_config_entry_t *buf;
    egr_olp_dgpp_config_entry_t *entry;
    egr_oam_dglp_profile_entry_t egr_dglp_profile_entry;
    ing_oam_dglp_profile_entry_t ing_dglp_profile_entry;
    _bcm_oam_control_t   *oc;        /* Pointer to OAM control structure. */
    int         index_max = 0;
    int         index = 0;
    uint32      configured_dglp = 0;
    void        *entries[1];
    void        *profile_entry;
    soc_mem_t   profile_mem;
    soc_profile_mem_t profile_ptr;
    int         entry_mem_size = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    if (mem == EGR_OAM_OPCODE_CONTROL_PROFILEm) {
        profile_entry = (void *)&egr_dglp_profile_entry;
        profile_mem = EGR_OAM_DGLP_PROFILEm;
        profile_ptr = oc->egr_oam_dglp_profile;
    } else {
        profile_entry = (void *)&ing_dglp_profile_entry;
        profile_mem = ING_OAM_DGLP_PROFILEm;
        profile_ptr = oc->ing_oam_dglp_profile;
    }
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
    /* Add entry to dglp profile table.  */
    soc_mem_field32_set(unit, profile_mem, profile_entry, DGLPf, dglp);
    entries[0] = profile_entry;

    rv = soc_profile_mem_add(unit, &profile_ptr,
                             (void *)entries, 1, profile_index);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM DGLP profile table is full - %s.\n"),
                   bcm_errmsg(rv)));
        return rv;
    }
    return (BCM_E_NONE);
} 

/*
 * Function:
 *     _bcm_kt2_ma_index_entry_modify
 * Purpose:
 *    Set dglp profile pointer index in the MA index table entry 
 * Parameters:
 *     unit        - (IN) BCM device number
 *     index       - (IN) MA index table index
 *     mem         - (IN) MA index memory - ing/egr
 *     profile_index- (IN)DGLP profile index
 *     olp_enable  - (IN) indicates whether olp is enabled for this entry
 *     dglp_prt    - (OUT) Pointer to dglp1/2 profile.  
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_kt2_ma_index_entry_modify(int unit, int index, soc_mem_t mem, 
                               int profile_index, int olp_enable, int *dglp_ptr)
{
    int  rv = BCM_E_NONE;
    ma_index_entry_t     ma_idx_entry;     /* MA_INDEX table entry buffer.    */
    egr_ma_index_entry_t egr_ma_idx_entry; /* EGR_MA_INDEX table entry buffer.*/
    void *entry;

    if (mem == EGR_MA_INDEXm) {
        entry = (void *)&egr_ma_idx_entry;
    } else {
        entry = (void *)&ma_idx_entry;
    }
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: MA index table entry  read failed  "
                              "%s.\n"), bcm_errmsg(rv)));
        return (rv);
    }
    if (0 == soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                 DGLP1_PROFILE_PTRf)){
        
         soc_mem_field32_set(unit, mem, entry, 
                             DGLP1_PROFILE_PTRf, profile_index);
         if (olp_enable) {
             soc_mem_field32_set(unit, mem, entry, DGLP1_OLP_HDR_ADDf, 1);
         }
         *dglp_ptr = _BCM_OAM_DGLP1_PROFILE_PTR;
    } else if (0 == soc_mem_field32_get(unit, mem, (uint32 *)entry, 
                                 DGLP2_PROFILE_PTRf)){
        
         soc_mem_field32_set(unit, mem, entry, 
                             DGLP2_PROFILE_PTRf, profile_index);
         if (olp_enable) {
             soc_mem_field32_set(unit, mem, entry, DGLP2_OLP_HDR_ADDf, 1);
         }
         *dglp_ptr = _BCM_OAM_DGLP2_PROFILE_PTR;
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, entry));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _kt2_oam_counter_control_set
 * Purpose:
 *    Enable/disable LM counter for a particular opcode 
 * Parameters:
 *     unit        - (IN) BCM device number
 *     opcode_i    - (IN) OAM opcode for which counter needs 
 *                        to be enabled/disabled 
 *     value       - (IN) Counter enable/disable
 * Returns:
 *      BCM_E_XXX
 */
int
_kt2_oam_counter_control_set(int unit, int opcode_i, int value)
{
    int rv = BCM_E_NONE;
    egr_lm_counter_control_entry_t  egr_ctr;
    lm_counter_control_entry_t      ing_ctr;

    rv = soc_mem_read(unit, LM_COUNTER_CONTROLm, MEM_BLOCK_ANY,
                      opcode_i, &ing_ctr);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Counter control table entry  read "
                              "failed  %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }
    soc_LM_COUNTER_CONTROLm_field32_set(unit, &ing_ctr, COUNT_ENABLEf, value);
    rv = soc_mem_write(unit, LM_COUNTER_CONTROLm, MEM_BLOCK_ALL, 
                       opcode_i, &ing_ctr);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Counter control table entry  wtite "
                              "failed  %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }
    rv = soc_mem_read(unit, EGR_LM_COUNTER_CONTROLm, MEM_BLOCK_ANY,
                      opcode_i, &egr_ctr);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Counter control table entry  read "
                              "failed  %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }
    soc_EGR_LM_COUNTER_CONTROLm_field32_set(unit, &egr_ctr, 
                                            COUNT_ENABLEf, value);
    rv = soc_mem_write(unit, EGR_LM_COUNTER_CONTROLm, MEM_BLOCK_ALL, 
                       opcode_i, &ing_ctr);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Counter control table entry  wtite "
                              "failed  %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_convert_action_to_opcode_entry
 * Purpose:
 *    Convert endpoint action to opcode profile entry action 
 * Parameters:
 *     unit        - (IN) BCM device number
 *     action      - (IN) Pointer to endpoint action
 *     ep_id       - (IN) Endpoint id
 *     profile     - (IN) Opcode profile table entry
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_kt2_oam_convert_action_to_opcode_entry(int unit,
                                            bcm_oam_endpoint_action_t *action, 
                                            bcm_oam_endpoint_t ep_id,
                                            void *profile,
                                            uint8  *opcode_profile_changed) 
{
    int rv = BCM_E_NONE;
    int action_i = 0;
    int opcode_i = 0;
    int break_the_loop = 0;
    int profile_index = _BCM_OAM_INVALID_INDEX;
    int olp_enable = 0;
    _bcm_oam_control_t   *oc;        /* Pointer to OAM control structure. */
    _bcm_oam_hash_data_t *h_data_p;  /* Pointer to endpoint hash data.    */
    soc_mem_t     mem = OAM_OPCODE_CONTROL_PROFILEm;
    soc_mem_t     ma_index_mem = MA_INDEXm;
    int           dglp_ptr = 0;
    int           skip_ma_index_modify = 0;
    int           value = 0;        
    bcm_module_t  module_id;
    bcm_port_t    port_id;
    bcm_trunk_t   trunk_id = BCM_TRUNK_INVALID;
    int           local_id;
    uint32        dglp = 0;
    int           reset = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    h_data_p = &oc->oam_hash_data[ep_id];
    if (h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING) {
        ma_index_mem = EGR_MA_INDEXm;
        mem = EGR_OAM_OPCODE_CONTROL_PROFILEm;
    }

    reset = (action->flags & BCM_OAM_ENDPOINT_ACTION_DISABLE) ? 1 : 0;

    for (action_i = 0; action_i < bcmOAMActionCount; action_i++) {
        if (BCM_OAM_ACTION_GET(*action, action_i)) {
            for (opcode_i = 0; opcode_i <= _BCM_OAM_OPCODE_TYPE_NON_CFM_LAST; 
                                                                opcode_i++) {
                if (BCM_OAM_OPCODE_GET(*action, opcode_i)) {
                    switch (action_i) {
                        case bcmOAMActionCountEnable:  
                            rv = _kt2_oam_counter_control_set(unit, opcode_i,
                                                              !reset);
                            if (BCM_FAILURE(rv)) {
                               LOG_ERROR(BSL_LS_BCM_OAM,
                                         (BSL_META_U(unit,
                                                     "OAM Error:Counter control "
                                                     "set - %s.\n"), bcm_errmsg(rv)));
                                return (rv);
                            }
                            break;

                        case bcmOAMActionDrop:
                        case bcmOAMActionUcDrop:             
                        case bcmOAMActionMcDrop:                
                        case bcmOAMActionLowMdlDrop:        
                        case bcmOAMActionMyStationMissDrop:
                            if ((action_i == bcmOAMActionLowMdlDrop) &&
                                (opcode_i == _BCM_OAM_OPCODE_TYPE_CCM)) {
                                value = reset ? 0 : _BCM_OAM_LOW_MDL_DROP_PACKET;
                            } else {
                                value = !reset;
                            }
                            rv = _kt2_oam_opcode_control_field_set(unit, 
                                             action_i, mem, opcode_i, 
                                             value, profile);
                            *opcode_profile_changed = 1;
                            break;

                        case bcmOAMActionCopyToCpu:             
                        case bcmOAMActionUcCopyToCpu:       
                        case bcmOAMActionMcCopyToCpu:          
                        case bcmOAMActionLowMdlCopyToCpu:  
                        case bcmOAMActionMyStationMissCopyToCpu:
                            rv = _kt2_oam_opcode_control_field_set(unit, 
                                           action_i, mem, opcode_i,
                                            !reset, profile);
                            *opcode_profile_changed = 1;
                            break;

                        case bcmOAMActionFwdAsData:             
                            if (opcode_i == _BCM_OAM_OPCODE_TYPE_CCM) { 
                                /* Need not check reset in this case
                                 * since this is default case
                                 */
                                soc_mem_field32_set(unit, mem, profile, 
                                                    LOW_MDL_CCM_FWD_ACTIONf, 0);
                               break_the_loop = 1;
                            }
                            *opcode_profile_changed = 1;
                            break;

                        case bcmOAMActionFwd:      
                        case bcmOAMActionUcFwd:       
                        case bcmOAMActionMcFwd:   
                        case bcmOAMActionLowMdlFwd:
                        case bcmOAMActionMyStationMissFwd:
                            /* forward packets to DGLP1 or DGLP2 */
                            if (!skip_ma_index_modify) {
                                SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                                             action->destination, &module_id, 
                                             &port_id, &trunk_id, &local_id));
                                if (BCM_GPORT_IS_TRUNK(action->destination) && 
                                             (trunk_id != BCM_TRUNK_INVALID)) {
                                    /* Set LAG ID indicator bit */
                                    dglp |= (1 << DGLP_LAG_ID_INDICATOR_SHIFT_BITS); 
                                }
                                dglp |= ((module_id << DGLP_MODULE_ID_SHIFT_BITS) + port_id); 

                                if (dglp == h_data_p->dglp1) {
                                    skip_ma_index_modify = 1;
                                    dglp_ptr = _BCM_OAM_DGLP1_PROFILE_PTR;
                                } else if (dglp == h_data_p->dglp2) {
                                    skip_ma_index_modify = 1;
                                    dglp_ptr = _BCM_OAM_DGLP2_PROFILE_PTR;
                                /* if both dglp1 and dglp2 are already set, 
                                   return error */
                                } else if ((h_data_p->dglp1 > 0) && 
                                           (h_data_p->dglp2 > 0)) {
                                    LOG_ERROR(BSL_LS_BCM_OAM,
                                              (BSL_META_U(unit,
                                                          "OAM Error: Invalid action"
                                                          "destination - %s.\n"), bcm_errmsg(rv)));
                                    return BCM_E_PARAM;
                                }
                                if (!skip_ma_index_modify) {
                                    rv = _bcm_kt2_oam_resolve_dglp(unit, mem, 
                                                 dglp, (uint32 *)&profile_index,
                                                 &olp_enable); 
                                    if (BCM_FAILURE(rv)) {
                                       LOG_ERROR(BSL_LS_BCM_OAM,
                                                 (BSL_META_U(unit,
                                                             "OAM Error: Gport  "
                                                             "to dglp-%s\n"), bcm_errmsg(rv)));
                                       return (rv);
                                    }
                                    if (profile_index != _BCM_OAM_INVALID_INDEX) {
                                        /* modify MA_INDEX table entry */       
                                        rv = _bcm_kt2_ma_index_entry_modify(unit,
                                                   h_data_p->local_rx_index, 
                                                   ma_index_mem, profile_index, 
                                                   olp_enable, &dglp_ptr);
                                        if (BCM_FAILURE(rv)) {
                                            return rv;
                                        }
                                        if (dglp_ptr == 
                                                   _BCM_OAM_DGLP1_PROFILE_PTR) {
                                            h_data_p->dglp1 = dglp;
                                            h_data_p->dglp1_profile_index = 
                                                                  profile_index;
                                        } else {
                                            h_data_p->dglp2 = dglp;
                                            h_data_p->dglp2_profile_index = 
                                                                  profile_index;
                                        }
                                        skip_ma_index_modify = 1; 
                                    }
                                }
                            }
                            if (dglp_ptr == 0) {
                                break;
                            }
                            /* Opcode control value is to bet set to 2 for 
                               DGLP1 and 3 for DGLP2 */
                            if (((opcode_i == _BCM_OAM_OPCODE_TYPE_LBM) && 
                                 (action_i != bcmOAMActionMcFwd))  || 
                               (opcode_i >= _BCM_OAM_OPCODE_TYPE_NON_CFM_FIRST)) {
                                value = dglp_ptr + 1;
                            } else {
                                value = dglp_ptr;
                            }
                            _kt2_oam_opcode_control_field_set(unit, action_i, 
                                           mem, opcode_i, value, profile);
                            *opcode_profile_changed = 1;
                            break;

                        case bcmOAMActionUcFwdAsData:      
                        case bcmOAMActionMcFwdAsData:         
                        case bcmOAMActionLowMdlFwdAsData:  
                        case bcmOAMActionMyStationMissFwdAsData:
                            /* Need not check reset in this case
                             * since this is default case
                             */
                            rv = _kt2_oam_opcode_control_field_set(unit, 
                                      action_i, mem, opcode_i, 
                                      _BCM_OAM_FWD_AS_DATA, profile);
                            *opcode_profile_changed = 1;
                            break;

                        case bcmOAMActionProcessInHw:           
                            if (opcode_i == _BCM_OAM_OPCODE_TYPE_CCM) { 
                                soc_mem_field32_set(unit, mem, profile, 
                                                    CCM_PROCESS_IN_HWf, !reset);
                            } else if (opcode_i == _BCM_OAM_OPCODE_TYPE_LBM) {
                                soc_mem_field32_set(unit, mem, profile, 
                                       LBM_ACTIONf, !reset);
                            }
                            break_the_loop = 1;
                            /* TBD - When CCM processing is done in HW, MA_PTR
                               value in MA_INDEX table should be less than 2k. 
                               If not, we should return error */
                            *opcode_profile_changed = 1;
                            break;

                        case bcmOAMActionLowMdlCcmFwdAsRegularCcm:
                            if (opcode_i == _BCM_OAM_OPCODE_TYPE_CCM) { 
                                soc_mem_field32_set(unit, mem, profile, 
                                                   LOW_MDL_CCM_FWD_ACTIONf, !reset);
                               break_the_loop = 1;
                            }
                            *opcode_profile_changed = 1;
                            break;

                        default:
                            break;
                    }
                }
                if (break_the_loop) { 
                    dglp_ptr = 0;
                    break_the_loop = 0;
                    olp_enable = 0;
                    profile_index = _BCM_OAM_INVALID_INDEX;
                    break;   
                }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_flex_drop_get
 * Purpose:
 *     Query the value of the device-wide OAM control.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     type        - (IN) OAM control type
 *     arg         - (OUT) A pointer to value of the control 
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_kt2_oam_flex_drop_get(int unit, bcm_oam_control_type_t type,
                                     uint64  *arg) 
{
   uint32 rval_32;
   soc_reg_t  reg = 0;
   uint32     is_64bit_reg = 0; 
   int        rv = BCM_E_NONE;

   switch(type) {
       case bcmOamControlFlexibleDropPort:
           reg = OAM_PORT_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropCVlan:
           reg = OAM_C_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropSVlan:
           reg = OAM_S_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropCPlusSVlan:
           reg = OAM_S_C_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropVP:
           reg = OAM_SVP_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropEgressPort:
           reg = EGR_OAM_PORT_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressCVlan:
           reg = EGR_OAM_C_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressSVlan:
           reg = EGR_OAM_S_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressCPlusSVlan:
           reg = EGR_OAM_S_C_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressVP:
           reg = EGR_OAM_DVP_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropPasssiveSap:
           reg = OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropEgressPasssiveSap:
           reg = EGR_OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr; 
           break;

       default:
           return (BCM_E_UNAVAIL); 
           break;
    }
    if (is_64bit_reg == 0) {
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval_32);
        if(BCM_SUCCESS(rv)) {
           COMPILER_64_SET(*arg, 0, rval_32);
        }
    } else {
        rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, arg);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_kt2_oam_flex_drop_get
 * Purpose:
 *     Query the value of the device-wide OAM control.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     type        - (IN) OAM control type
 *     arg         - (IN) value to be set for the control 
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_kt2_oam_flex_drop_set(int unit, bcm_oam_control_type_t type,
                               uint64  arg) 
{
   uint32 rval_32;
   soc_reg_t  reg = 0;
   uint32     is_64bit_reg = 0; 
   int        rv = BCM_E_NONE;

   switch(type) {
       case bcmOamControlFlexibleDropPort:
           reg = OAM_PORT_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropCVlan:
           reg = OAM_C_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropSVlan:
           reg = OAM_S_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropCPlusSVlan:
           reg = OAM_S_C_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropVP:
           reg = OAM_SVP_INTERFACE_DROP_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropEgressPort:
           reg = EGR_OAM_PORT_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressCVlan:
           reg = EGR_OAM_C_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressSVlan:
           reg = EGR_OAM_S_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressCPlusSVlan:
           reg = EGR_OAM_S_C_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropEgressVP:
           reg = EGR_OAM_DVP_INTERFACE_DROP_CONTROL_64r; 
           is_64bit_reg = 1;
           break;

       case bcmOamControlFlexibleDropPasssiveSap:
           reg = OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr; 
           break;

       case bcmOamControlFlexibleDropEgressPasssiveSap:
           reg = EGR_OAM_S_INTERFACE_PASSIVE_PROCESSING_CONTROLr; 
           break;

       default:
           return (BCM_E_UNAVAIL); 
           break;
    }
    if (is_64bit_reg == 0) {
        COMPILER_64_TO_32_LO(rval_32, arg); 
        rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval_32);
    } else {
        rv = soc_reg64_set(unit, reg, REG_PORT_ANY, 0, arg);
    }
    return rv;
}
/*
 * Function:
 *     bcm_kt2_oam_control_get
 * Purpose:
 *     Query the value of the device-wide OAM control.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     type        - (IN) OAM control type
 *     arg         - (OUT) A pointer to value of the control 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_kt2_oam_control_get(int unit, bcm_oam_control_type_t type,
                                     uint64  *arg) 
{
    int        rv = BCM_E_NONE;

    switch(type) {
        case bcmOamControlFlexibleDropPort:
        case bcmOamControlFlexibleDropCVlan:
        case bcmOamControlFlexibleDropSVlan:
        case bcmOamControlFlexibleDropCPlusSVlan:
        case bcmOamControlFlexibleDropVP:
        case bcmOamControlFlexibleDropEgressPort:
        case bcmOamControlFlexibleDropEgressCVlan:
        case bcmOamControlFlexibleDropEgressSVlan:
        case bcmOamControlFlexibleDropEgressCPlusSVlan:
        case bcmOamControlFlexibleDropEgressVP:
        case bcmOamControlFlexibleDropPasssiveSap:
        case bcmOamControlFlexibleDropEgressPasssiveSap:
            rv = _bcm_kt2_oam_flex_drop_get(unit, type, arg);
            break; 
        default:
            rv = BCM_E_UNAVAIL; 
            break;
    }
    return rv;
}

/*
 * Function:
 *     bcm_kt2_oam_control_set
 * Purpose:
 *     Set the value of the device-wide OAM control.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     type        - (IN) OAM control type
 *     arg         - (IN) value to be set for control 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_kt2_oam_control_set(int unit, bcm_oam_control_type_t type,
                                     uint64  arg) 
{
    int        rv = BCM_E_NONE;
    switch(type) {
        case bcmOamControlFlexibleDropPort:
        case bcmOamControlFlexibleDropCVlan:
        case bcmOamControlFlexibleDropSVlan:
        case bcmOamControlFlexibleDropCPlusSVlan:
        case bcmOamControlFlexibleDropVP:
        case bcmOamControlFlexibleDropEgressPort:
        case bcmOamControlFlexibleDropEgressCVlan:
        case bcmOamControlFlexibleDropEgressSVlan:
        case bcmOamControlFlexibleDropEgressCPlusSVlan:
        case bcmOamControlFlexibleDropEgressVP:
        case bcmOamControlFlexibleDropPasssiveSap:
        case bcmOamControlFlexibleDropEgressPasssiveSap:
            rv = _bcm_kt2_oam_flex_drop_set(unit, type, arg);
            break; 
        default:
            rv = BCM_E_UNAVAIL; 
            break;
    }
    return rv;
}

/*
 * Function:
 *     bcm_kt2_oam_upmep_cosq_set
 * Purpose:
 *     Set the value of the device-wide UP MEP PDU type CPU queue.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     upmep_pdu_type        - (IN) UP MEP PDU type
 *     cosq        - (IN) value to be set for CPU queue
 * Returns:
 *      BCM_E_XXX
 */
int bcm_kt2_oam_upmep_cosq_set(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                               bcm_cos_queue_t cosq)
{
    int        rv = BCM_E_NONE;
    uint64 cpu_control_2_64;
    uint64 write_val;

    SOC_IF_ERROR_RETURN(
            READ_EGR_CPU_CONTROL_2_64r(unit, &cpu_control_2_64));
    switch(upmep_pdu_type) {
        case bcmOamUpmepPduTypeCcm:
            COMPILER_64_SET(write_val, 0, cosq);
            soc_reg64_field_set(unit, EGR_CPU_CONTROL_2_64r, &cpu_control_2_64,
                    OAM_CCM_SLOWPATH_CPU_COSf, write_val);
            break;
        case bcmOamUpmepPduTypeLmDm:
            COMPILER_64_SET(write_val, 0, cosq);
            soc_reg64_field_set(unit, EGR_CPU_CONTROL_2_64r, &cpu_control_2_64,
                    OAM_LMDM_CPU_COSf, write_val);
            break;
        case bcmOamUpmepPduTypeSlowpath:
            COMPILER_64_SET(write_val, 0, cosq);
            soc_reg64_field_set(unit, EGR_CPU_CONTROL_2_64r, &cpu_control_2_64,
                    OAM_SLOWPATH_CPU_COSf, write_val);
            break;
        case bcmOamUpmepPduTypeError:
            COMPILER_64_SET(write_val, 0, cosq);
            soc_reg64_field_set(unit, EGR_CPU_CONTROL_2_64r, &cpu_control_2_64,
                    OAM_ERROR_CPU_COSf, write_val);
            break;
        case bcmOamUpmepPduTypeCount :
            rv = BCM_E_PARAM;
            break;
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }
    if (rv == BCM_E_NONE) {
        SOC_IF_ERROR_RETURN(
                WRITE_EGR_CPU_CONTROL_2_64r(unit, cpu_control_2_64));
    }
    return rv;
}

/*
 * Function:
 *     bcm_kt2_oam_upmep_cosq_get
 * Purpose:
 *     Query the value of the device-wide UP MEP PDU type CPU queue.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     upmep_pdu_type        - (IN) UP MEP PDU type
 *     cosq        - (OUT) pointer to CPU queue to be get
 * Returns:
 *      BCM_E_XXX
 */
int bcm_kt2_oam_upmep_cosq_get(int unit, bcm_oam_upmep_pdu_type_t upmep_pdu_type,
                               bcm_cos_queue_t *cosq)
{
   int        rv = BCM_E_NONE;
    uint64 cpu_control_2_64;

    SOC_IF_ERROR_RETURN(
            READ_EGR_CPU_CONTROL_2_64r(unit, &cpu_control_2_64));
    switch(upmep_pdu_type) {
        case bcmOamUpmepPduTypeCcm:
            *cosq = soc_reg64_field32_get(unit, EGR_CPU_CONTROL_2_64r,
            cpu_control_2_64, OAM_CCM_SLOWPATH_CPU_COSf);
            break;
        case bcmOamUpmepPduTypeLmDm:
            *cosq = soc_reg64_field32_get(unit, EGR_CPU_CONTROL_2_64r,
            cpu_control_2_64, OAM_LMDM_CPU_COSf);
            break;
        case bcmOamUpmepPduTypeSlowpath:
            *cosq = soc_reg64_field32_get(unit, EGR_CPU_CONTROL_2_64r,
            cpu_control_2_64, OAM_SLOWPATH_CPU_COSf);
            break;
        case bcmOamUpmepPduTypeError:
            *cosq = soc_reg64_field32_get(unit, EGR_CPU_CONTROL_2_64r,
            cpu_control_2_64, OAM_ERROR_CPU_COSf);
            break;
        case bcmOamUpmepPduTypeCount :
            rv = BCM_E_PARAM;
            break;
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }
    return rv;
}

#if defined(INCLUDE_BHH)
/*
 * Function:
 *      bcm_kt2_oam_mpls_tp_channel_type_tx_get
 * Purpose:
 *      Get MPLS TP(BHH) ACH Channel type
 * Parameters:
 *      unit                (IN)  BCM device number
 *      channel_type        (IN)  Channel type is ignored in XGS devices
 *      Value               (OUT) User define MPLS TP(BHH) ACH channel type
 *
 * Returns:
 *      BCM_E_NONE          No error
 *      BCM_E_XXXX          Error
 */
int
bcm_kt2_oam_mpls_tp_channel_type_tx_get(int unit,
                                        bcm_oam_mpls_tp_channel_type_t channel_type,
                                        int *value)
{
    *value = kt2_oam_mpls_tp_ach_channel_type;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oam_mpls_tp_channel_type_tx_set
 * Purpose:
 *      Update MPLS TP(BHH) ACH Channel type with user defined value
 * Parameters:
 *      unit                (IN) BCM device number
 *      channel_type        (IN) Channel type is ignored in XGS devices
 *      Value               (IN) User define MPLS TP(BHH) ACH channel type
 *
 * Returns:
 *      BCM_E_NONE          No error
 *      BCM_E_XXXX          Error
 */
int
bcm_kt2_oam_mpls_tp_channel_type_tx_set(int unit,
                                        bcm_oam_mpls_tp_channel_type_t channel_type,
                                        int value)
{
    uint8               *buffer, *buffer_ptr;
    uint16              buffer_len, reply_len;
    int                 rv = BCM_E_NONE;
    uint32              *msg;
    _bcm_oam_control_t   *oc;               /* OAM control structure.        */

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    if ((value != SHR_BHH_ACH_CHANNEL_TYPE) &&
        (value != SHR_BHH_ACH_CHANNEL_TYPE_1)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_CONFIG;
    }

    kt2_oam_mpls_tp_ach_channel_type = value;
    msg = (uint32 *)&kt2_oam_mpls_tp_ach_channel_type;

    /* Set BHH ACH Type in BHH_RX_ACH_TYPE register */
    rv = WRITE_BHH_RX_ACH_TYPEr(unit, kt2_oam_mpls_tp_ach_channel_type);

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_ach_channel_type_msg_pack(buffer, msg);
    buffer_len = buffer_ptr - buffer;

    rv = _bcm_kt2_oam_bhh_msg_send_receive(
                                           unit,
                                           MOS_MSG_SUBCLASS_BHH_ACH_CHANNEL_TYPE,
                                           buffer_len, 0,
                                           MOS_MSG_SUBCLASS_BHH_ACH_CHANNEL_TYPE_REPLY,
                                           &reply_len);

    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_encap_hw_set
 * Purpose:
 *      Sets BHH encapsulation type in HW device.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      module_id       - (IN) Module id.
 *      port_id         - (IN) Port.
 *      is_local        - (IN) Indicates if module id is local.
 *      endpoint_config - (IN) Pointer to BHH endpoint structure.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt2_oam_bhh_encap_hw_set(int unit, _bcm_oam_hash_data_t *h_data_p,
                         bcm_module_t module_id, bcm_port_t port_id,
                         int is_local, bcm_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
#if defined(BCM_KATANA2_SUPPORT) && defined(BCM_MPLS_SUPPORT)
    bcm_l3_egress_t l3_egress;
    int ingress_if;
    int i;
    int num_entries;
    mpls_entry_entry_t mpls_entry;
    mpls_entry_entry_t mpls_key;
    int vccv_type = 0;
    int mpls_index = 0;
#endif /* BCM_KATANA2_SUPPORT &&  BCM_MPLS_SUPPORT */

    switch(h_data_p->type) {
    case bcmOAMEndpointTypeBHHMPLS:
    case bcmOAMEndpointTypeBHHMPLSVccv:

#if defined(BCM_KATANA2_SUPPORT) && defined(BCM_MPLS_SUPPORT)
        SOC_IF_ERROR_RETURN(bcm_tr_mpls_lock (unit));

        sal_memset(&mpls_key, 0, sizeof(mpls_key));
        soc_MPLS_ENTRYm_field32_set(unit, &mpls_key, MPLS__MPLS_LABELf,
                                    h_data_p->label);

        rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &mpls_index,
                            &mpls_key, &mpls_entry, 0);
        if (SOC_FAILURE(rv)) {
            bcm_tr_mpls_unlock (unit);
            return rv;
        }

        if ((soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry,
                                         VALIDf) != 0x1)) {
            bcm_tr_mpls_unlock (unit);
            return (BCM_E_PARAM);
        }

        if (h_data_p->type == bcmOAMEndpointTypeBHHMPLSVccv) {
            /* Get the control channel type */
            vccv_type = soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry,
                                                    MPLS__PW_CC_TYPEf);

            if (vccv_type == 0) { /* No VCCV type configured */
                soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry,
                                            MPLS__PW_CC_TYPEf,
                                            (h_data_p->vccv_type + 1));

            } else if (vccv_type != (h_data_p->vccv_type + 1)) {
                /* Endpoint add configuration conflicts with MPLS port add config */
                bcm_tr_mpls_unlock(unit);
                return BCM_E_PARAM;
            }

        }

        soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry,
                                    MPLS__SESSION_IDENTIFIER_TYPEf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, MPLS__BHH_ENABLEf, 1);
        rv = soc_mem_write(unit, MPLS_ENTRYm,
                           MEM_BLOCK_ANY, mpls_index,
                           &mpls_entry);
        if (rv != SOC_E_NONE) {
            bcm_tr_mpls_unlock(unit);
            return rv;
        }

        /*
         * PW CC-3
         *
         * Set MPLS entry DECAP_USE_TTL=0 for corresponding
         * Tunnel Terminator label.
         */
        if (h_data_p->type == bcmOAMEndpointTypeBHHMPLSVccv) {
            if (h_data_p->vccv_type == bcmOamBhhVccvTtl) {
                rv = bcm_esw_l3_egress_get(unit, h_data_p->egress_if,
                                           &l3_egress);
                if (rv != BCM_E_NONE) {
                    bcm_tr_mpls_unlock(unit);
                    return rv;
                }

                /* Look for Tunnel Terminator label */
                num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
                for (i = 0; i < num_entries; i++) {
                    rv = READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, i, &mpls_entry);
                    if (rv != SOC_E_NONE) {
                        bcm_tr_mpls_unlock(unit);
                        return rv;
                    }

                    if (!soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, VALIDf)) {
                        continue;
                    }
                    if (soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry,
                                              MPLS__MPLS_ACTION_IF_BOSf) == 0x1) {
                        continue;    /* L2_SVP */
                    }

                    ingress_if = soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry,
                                                             MPLS__L3_IIFf);
                    if (ingress_if == l3_egress.intf) {
                        /* Label found */
                        soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry,
                                                    MPLS__DECAP_USE_TTLf, 0);
                        rv = soc_mem_write(unit, MPLS_ENTRYm,
                                           MEM_BLOCK_ALL, i,
                                           &mpls_entry);
                        if (rv != SOC_E_NONE) {
                            bcm_tr_mpls_unlock(unit);
                            return rv;
                        }
                        break;
                    }
                }
            }
        }
        bcm_tr_mpls_unlock (unit);
#else
        rv = BCM_E_UNAVAIL;
#endif /* BCM_KATANA2_SUPPORT &&  BCM_MPLS_SUPPORT */

        break;

    default:
        rv = BCM_E_UNAVAIL;
        break;
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_encap_data_dump
 * Purpose:
 *      Dumps buffer contents.
 * Parameters:
 *      buffer  - (IN) Buffer to dump data.
 *      length  - (IN) Length of buffer.
 * Returns:
 *      None
 */
void
_bcm_kt2_oam_bhh_encap_data_dump(uint8 *buffer, int length)
{
    int i;

    LOG_CLI((BSL_META("\nBHH encapsulation (length=%d):\n"), length));

    for (i = 0; i < length; i++) {
        if ((i % 16) == 0) {
            LOG_CLI((BSL_META("\n")));
        }
        LOG_CLI((BSL_META(" %02x"), buffer[i]));
    }

    LOG_CLI((BSL_META("\n")));
    return;
}

STATIC int
_bcm_kt2_oam_bhh_ach_header_get(uint32 packet_flags, _ach_header_t *ach)
{
    sal_memset(ach, 0, sizeof(*ach));

    ach->f_nibble = SHR_BHH_ACH_FIRST_NIBBLE;
    ach->version  = SHR_BHH_ACH_VERSION;
    ach->reserved = 0;

    ach->channel_type = kt2_oam_mpls_tp_ach_channel_type;

    return (BCM_E_NONE);
}

STATIC int
_bcm_kt2_oam_bhh_mpls_label_get(uint32 label, uint8 exp, uint8 s, uint8 ttl,
                           _mpls_label_t *mpls)
{
    sal_memset(mpls, 0, sizeof(*mpls));

    mpls->label = label;
    mpls->exp   = exp & 0x07;
    mpls->s     = s;
    if(ttl)
        mpls->ttl   = ttl;
    else
        mpls->ttl   = _BHH_MPLS_DFLT_TTL;

    return (BCM_E_NONE);
}

STATIC int
_bcm_kt2_oam_bhh_mpls_gal_label_get(_mpls_label_t *mpls, uint8 oam_exp)
{
    return _bcm_kt2_oam_bhh_mpls_label_get(SHR_BHH_MPLS_GAL_LABEL,
                                      oam_exp, 1, 1, mpls);
}

STATIC int
_bcm_kt2_oam_bhh_mpls_labels_get(int unit, _bcm_oam_hash_data_t *h_data_p,
                                uint32 packet_flags,
                                int max_count, 
                                _mpls_label_t *pw_label,
                                _mpls_label_t *mpls,
                                int *mpls_count, bcm_if_t *l3_intf_id)
{
    int count = 0;
    bcm_l3_egress_t l3_egress;
    bcm_mpls_port_t mpls_port;
    bcm_mpls_egress_label_t label_array[_BHH_MPLS_MAX_LABELS];
    bcm_mpls_egress_label_t tmp_label;
    int label_count;
    int i = 0;

     /* Get L3 objects */
    bcm_l3_egress_t_init(&l3_egress);
    if (BCM_FAILURE(bcm_esw_l3_egress_get(unit, h_data_p->egress_if, &l3_egress))) {
        return (BCM_E_PARAM);
    }

    /* Look for a tunnel associated with this interface */
    if (BCM_SUCCESS
        (bcm_esw_mpls_tunnel_initiator_get(unit, l3_egress.intf,
                                           _BHH_MPLS_MAX_LABELS,
                                           label_array, &label_count))) {
        /* We need to swap the labels configured in tunnel initiator as
         * it returns labels in reverse order than how it should be in pkt.
         */
        for (i = 0; i < (label_count / 2); i++) {
            sal_memcpy(&tmp_label, &label_array[i], sizeof(tmp_label));
            sal_memcpy(&label_array[i], &label_array[label_count - 1 - i],
                       sizeof(tmp_label));
            sal_memcpy(&label_array[label_count - 1 - i], &tmp_label,
                       sizeof(tmp_label));
        }

        /* Add VRL Label */
        if (h_data_p->type == bcmOAMEndpointTypeBHHMPLS) {
            if ((l3_egress.mpls_label != BCM_MPLS_LABEL_INVALID) &&
                (h_data_p->egr_label != BCM_MPLS_LABEL_INVALID)  &&
                (label_count < _BHH_MPLS_MAX_LABELS) ) {

                label_array[label_count].label = l3_egress.mpls_label;
                label_array[label_count].exp = l3_egress.mpls_exp;
                label_array[label_count].ttl = l3_egress.mpls_ttl;
                label_count++;
            }
            /* Traverse through Label stack and match on control label, this
             * label should be the one just above GAL in the label stack, any
             * following labels should not be added.
             */
            for (i = 0;  i < label_count; i++) {
                if (label_array[i].label == h_data_p->egr_label) {
                    label_count = i + 1;
                    label_array[i].exp = h_data_p->egr_label_exp;
                    if (soc_property_get(unit, spn_MPLS_OAM_EGRESS_LABEL_TTL, 0) &&
                        h_data_p->egr_label_ttl) {
                        label_array[i].ttl = h_data_p->egr_label_ttl;
                    }
                    break;
                }
            }
        }
    }

        /* MPLS Router Alert */
    if (packet_flags & _BHH_ENCAP_PKT_MPLS_ROUTER_ALERT) {
        /* Ignore overrun error as RAL is only added for PW MEPs and so overrun
         * condition will not exist.
         */
        /* coverity[overrun-local] */
        label_array[label_count].label = SHR_BHH_MPLS_ROUTER_ALERT_LABEL;
        label_array[label_count].exp = 0;
        label_array[label_count].ttl = 0;
        label_count++;
    }

    /* Use GPORT to resolve interface */
    if (BCM_GPORT_IS_MPLS_PORT(h_data_p->gport)) {
        /* Get mpls port and label info */
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.mpls_port_id = h_data_p->gport;
        if (BCM_FAILURE(bcm_esw_mpls_port_get(unit, h_data_p->vpn,
                                              &mpls_port))) {
            return (BCM_E_PARAM);
        }
        if (h_data_p->type == bcmOAMEndpointTypeBHHMPLSVccv &&
            h_data_p->vccv_type == bcmOamBhhVccvTtl) {
            mpls_port.egress_label.ttl = 0x1;
        }

        label_array[label_count].label = mpls_port.egress_label.label;
        if (h_data_p->egr_label == mpls_port.egress_label.label) {
            label_array[label_count].exp = h_data_p->egr_label_exp;
            if (soc_property_get(unit, spn_MPLS_OAM_EGRESS_LABEL_TTL, 0) &&
                    h_data_p->egr_label_ttl) {
                label_array[label_count].ttl = h_data_p->egr_label_ttl;
            }
        } else {
            label_array[label_count].exp   = mpls_port.egress_label.exp;
            label_array[label_count].ttl   = mpls_port.egress_label.ttl;
        }
        label_count++;
    }


    for (i=0;  i < label_count; i++) {
        _bcm_kt2_oam_bhh_mpls_label_get(label_array[i].label,
                                        label_array[i].exp,
                                        0,
                                        label_array[i].ttl,
                                        &mpls[count++]);
    }

    /* Set Bottom of Stack if there is no GAL label */
    if (!(packet_flags & _BHH_ENCAP_PKT_GAL)) {
        mpls[count-1].s = 1;
    }

    *mpls_count = count;

    return (BCM_E_NONE);

}

STATIC int
_bcm_kt2_oam_bhh_l2_header_get(int unit, _bcm_oam_hash_data_t *h_data_p,
                               bcm_port_t port, uint16 etype,
                               _l2_header_t *l2)
{
    uint16 tpid;
    bcm_l3_egress_t l3_egress;
    bcm_l3_intf_t l3_intf;
    bcm_vlan_control_vlan_t vc;
    int tpid_select;
    bcm_pbmp_t pbmp, ubmp;

    sal_memset(l2, 0, sizeof(*l2));

    if (!BHH_EP_MPLS_SECTION_TYPE(h_data_p)) {
        /* Get L3 interfaces */
        bcm_l3_egress_t_init(&l3_egress);
        bcm_l3_intf_t_init(&l3_intf);

        if (BCM_FAILURE
            (bcm_esw_l3_egress_get(unit, h_data_p->egress_if, &l3_egress))) {
            return (BCM_E_PARAM);
        }

        l3_intf.l3a_intf_id = l3_egress.intf;
        if (BCM_FAILURE(bcm_esw_l3_intf_get(unit, &l3_intf))) {
            return (BCM_E_PARAM);
        }

        /* Get TPID */
        BCM_IF_ERROR_RETURN(bcm_esw_vlan_control_port_get(unit,
                                                          port,
                                                          bcmVlanPortOuterTpidSelect,
                                                          &tpid_select));
        if (tpid_select == BCM_PORT_OUTER_TPID) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_tpid_get(unit, port, &tpid));
        } else {
            BCM_IF_ERROR_RETURN(
                         bcm_esw_vlan_control_vlan_get(unit, l3_intf.l3a_vid, &vc));
            tpid = vc.outer_tpid;
        }

        sal_memcpy(l2->dst_mac, l3_egress.mac_addr, _BHH_MAC_ADDR_LENGTH);
        sal_memcpy(l2->src_mac, l3_intf.l3a_mac_addr, _BHH_MAC_ADDR_LENGTH);
        l2->vlan_tag.tpid     = tpid;
        l2->vlan_tag.tci.prio = h_data_p->vlan_pri;
        l2->vlan_tag.tci.cfi  = 0;
        l2->vlan_tag.tci.vid  = l3_intf.l3a_vid;

        BCM_IF_ERROR_RETURN(bcm_esw_vlan_port_get(unit,
                                                  l2->vlan_tag.tci.vid,
                                                  &pbmp,
                                                  &ubmp));
        if (BCM_PBMP_MEMBER(ubmp, port)) {
            l2->vlan_tag.tpid = 0;  /* Set to 0 to indicate untagged */
        }

        if (l3_intf.l3a_inner_vlan != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_inner_tpid_get(unit, port, &tpid));
            l2->vlan_tag_inner.tpid     = tpid;
            l2->vlan_tag_inner.tci.prio = h_data_p->inner_vlan_pri;
            l2->vlan_tag_inner.tci.cfi  = 0;
            l2->vlan_tag_inner.tci.vid  = l3_intf.l3a_inner_vlan;
        }
        l2->etype             = etype;
    } else {
        sal_memcpy(l2->dst_mac, h_data_p->dst_mac_address, _BHH_MAC_ADDR_LENGTH);
        sal_memcpy(l2->src_mac, h_data_p->src_mac_address, _BHH_MAC_ADDR_LENGTH);
        if((h_data_p->type == bcmOAMEndpointTypeBhhSectionOuterVlan) ||
            (h_data_p->type == bcmOAMEndpointTypeBhhSectionInnervlan) ||
            (h_data_p->type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)){
            if (h_data_p->vlan != 0) {
                l2->vlan_tag.tpid     = h_data_p->outer_tpid;
                l2->vlan_tag.tci.prio = h_data_p->vlan_pri;
                l2->vlan_tag.tci.cfi  = 0;
                l2->vlan_tag.tci.vid  = h_data_p->vlan;
            }
        }
        if((h_data_p->type == bcmOAMEndpointTypeBhhSectionInnervlan) ||
            (h_data_p->type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)){
            l2->vlan_tag_inner.tpid     = h_data_p->inner_tpid;
            l2->vlan_tag_inner.tci.prio = h_data_p->inner_vlan_pri;
            l2->vlan_tag_inner.tci.cfi  = 0;
            l2->vlan_tag_inner.tci.vid  = h_data_p->inner_vlan;
        }
         l2->etype = SHR_BHH_L2_ETYPE_MPLS_UCAST;
    }
    return BCM_E_NONE;
}
STATIC uint8 *
_bcm_kt2_oam_bhh_ach_header_pack(uint8 *buffer, _ach_header_t *ach)
{
    uint32  tmp;

    tmp = ((ach->f_nibble & 0xf) << 28) | ((ach->version & 0xf) << 24) |
        (ach->reserved << 16) | ach->channel_type;

    _BHH_ENCAP_PACK_U32(buffer, tmp);

    return (buffer);
}

STATIC uint8 *
_bcm_kt2_oam_bhh_mpls_label_pack(uint8 *buffer, _mpls_label_t *mpls)
{
    uint32  tmp;

    tmp = ((mpls->label & 0xfffff) << 12) | ((mpls->exp & 0x7) << 9) |
        ((mpls->s & 0x1) << 8) | mpls->ttl;
    _BHH_ENCAP_PACK_U32(buffer, tmp);

    return (buffer);
}

STATIC uint8 *
_bcm_kt2_oam_bhh_l2_header_pack(uint8 *buffer, _l2_header_t *l2)
{
    uint32  tmp;
    int     i;

    for (i = 0; i < _BHH_MAC_ADDR_LENGTH; i++) {
        _BHH_ENCAP_PACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < _BHH_MAC_ADDR_LENGTH; i++) {
        _BHH_ENCAP_PACK_U8(buffer, l2->src_mac[i]);
    }

    /* Vlan Tag tpid = 0 indicates untagged */
    if (0 != l2->vlan_tag.tpid) {
        tmp =  (l2->vlan_tag.tpid            << 16) |
              ((l2->vlan_tag.tci.prio & 0x7) << 13) |
              ((l2->vlan_tag.tci.cfi  & 0x1) << 12) |
               (l2->vlan_tag.tci.vid  & 0xfff);
        _BHH_ENCAP_PACK_U32(buffer, tmp);
    }

    /*Inner Vlan Tag inner_tpid = 0 indicates single tag  */
    if (0 != l2->vlan_tag_inner.tpid) {
        tmp = (l2->vlan_tag_inner.tpid            << 16) |
             ((l2->vlan_tag_inner.tci.prio & 0x7) << 13) |
             ((l2->vlan_tag_inner.tci.cfi  & 0x1) << 12) |
              (l2->vlan_tag_inner.tci.vid  & 0xfff);
        _BHH_ENCAP_PACK_U32(buffer, tmp);
    }
    _BHH_ENCAP_PACK_U16(buffer, l2->etype);

    return (buffer);
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_encap_build_pack
 * Purpose:
 *      Builds and packs the BHH packet encapsulation for a given
 *      BHH tunnel type.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      port            - (IN) Port.
 *      endpoint_config - (IN/OUT) Pointer to BHH endpoint structure.
 *      packet_flags    - (IN) Flags for building packet.
 *      buffer          - (OUT) Buffer returning BHH encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BHH encapsulation includes only all the
 *      encapsulation headers/labels and does not include
 *      the BHH control packet.
 */
STATIC int
_bcm_kt2_oam_bhh_encap_build_pack(int unit, bcm_port_t port,
                             _bcm_oam_hash_data_t *h_data_p,
                             uint32 packet_flags,
                             uint8 *buffer,
                             uint32 *encap_length)
{
    uint8          *cur_ptr = buffer;
    uint16         etype = 0;
    bcm_if_t       l3_intf_id = -1;
    _ach_header_t  ach;
    _mpls_label_t  mpls_gal;
    _mpls_label_t  mpls_labels[_BHH_MPLS_MAX_LABELS];
    _mpls_label_t  pw_label;
    int            mpls_count = 0;
    _l2_header_t   l2;
    int i;


    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if (packet_flags & _BHH_ENCAP_PKT_G_ACH) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oam_bhh_ach_header_get(packet_flags, &ach));
    }

    if (packet_flags & _BHH_ENCAP_PKT_GAL) {
        if (h_data_p->egr_label == SHR_BHH_MPLS_GAL_LABEL) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_oam_bhh_mpls_gal_label_get(&mpls_gal,
                                                     h_data_p->egr_label_exp));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_oam_bhh_mpls_gal_label_get(&mpls_gal, 0));
        }
    }

    if (packet_flags & _BHH_ENCAP_PKT_MPLS) {
        etype = SHR_BHH_L2_ETYPE_MPLS_UCAST;
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_oam_bhh_mpls_labels_get(unit, h_data_p,
                                             packet_flags,    
                                             _BHH_MPLS_MAX_LABELS,
                                             &pw_label,
                                             mpls_labels, 
                                             &mpls_count,
                                             &l3_intf_id));
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oam_bhh_l2_header_get(unit, 
                                       h_data_p,
                                       port, 
                                       etype, 
                                       &l2));

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;

    /* L2 Header is always present */
    cur_ptr = _bcm_kt2_oam_bhh_l2_header_pack(cur_ptr, &l2);

    if (packet_flags & _BHH_ENCAP_PKT_MPLS) {
        for (i = 0;i < mpls_count;i++) {
            cur_ptr = _bcm_kt2_oam_bhh_mpls_label_pack(cur_ptr, &mpls_labels[i]);
        }
    }

    if (packet_flags & _BHH_ENCAP_PKT_GAL) {    
        cur_ptr = _bcm_kt2_oam_bhh_mpls_label_pack(cur_ptr, &mpls_gal);
    }

    if (packet_flags & _BHH_ENCAP_PKT_G_ACH) {
        cur_ptr = _bcm_kt2_oam_bhh_ach_header_pack(cur_ptr, &ach);
    }


    /* Set BHH encapsulation length */
    *encap_length = cur_ptr - buffer;

    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_kt2_oam_bhh_encap_create
 * Purpose:
 *      Creates a BHH packet encapsulation.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      port_id         - (IN) Port.
 *      endpoint_config - (IN/OUT) Pointer to BHH endpoint structure.
 *      encap_data      - (OUT) Buffer returning BHH encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BHH encapsulation buffer includes all the
 *      corresponding headers/labels EXCEPT for the BHH control packet.
 */
STATIC int
_bcm_kt2_oam_bhh_encap_create(int unit, bcm_port_t port_id,
                         _bcm_oam_hash_data_t *h_data_p,
                         uint8 *encap_data,
                         uint8  *encap_type,
                         uint32 *encap_length)
{
    uint32 packet_flags;
    _bcm_oam_control_t *oc;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    packet_flags = 0;

    /*
     * Get BHH encapsulation packet format flags
     *
     * Also, perform the following for each BHH tunnel type:
     * - Check for valid parameter values
     * - Set specific values required by the BHH tunnel definition 
     *   (e.g. such as ttl=1,...)
     */
    switch (h_data_p->type) {
    case bcmOAMEndpointTypeBHHMPLS:
        packet_flags |=
            (_BHH_ENCAP_PKT_MPLS |
             _BHH_ENCAP_PKT_GAL |
             _BHH_ENCAP_PKT_G_ACH);
        break;

    case bcmOAMEndpointTypeBHHMPLSVccv:
        switch(h_data_p->vccv_type) {

        case bcmOamBhhVccvChannelAch:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_G_ACH);
        break;

        case bcmOamBhhVccvRouterAlert:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_MPLS_ROUTER_ALERT |
                 _BHH_ENCAP_PKT_G_ACH); 
            break;

        case bcmOamBhhVccvTtl:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_G_ACH); 
            break;

    case bcmOamBhhVccvGal13:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_GAL  |
                 _BHH_ENCAP_PKT_G_ACH);
            break;

    default:
        return (BCM_E_PARAM);
        break;
    }
        break;
    case bcmOAMEndpointTypeBhhSection:
    case bcmOAMEndpointTypeBhhSectionInnervlan:
    case bcmOAMEndpointTypeBhhSectionOuterVlan:
    case bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan:
            packet_flags |=
                (_BHH_ENCAP_PKT_GAL |
                 _BHH_ENCAP_PKT_G_ACH);
            break;
    default:
        return (BCM_E_PARAM);
    }

    /* Build header/labels and pack in buffer */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_oam_bhh_encap_build_pack(unit, port_id,
                                      h_data_p,
                                      packet_flags,
                                      encap_data,
                                  encap_length));

    /* Set encap type (indicates uC side that checksum is required) */
    *encap_type = SHR_BHH_ENCAP_TYPE_RAW;

#ifdef _BHH_DEBUG_DUMP
    _bcm_kt2_oam_bhh_encap_data_dump(encap_data, *encap_length);
#endif

    if (*encap_length > oc->bhh_max_encap_length) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                "OAM(unit %d) Error: Encap length greater than max,"
                "encap_length=%u max=%u\n"),
                 unit, *encap_length, oc->bhh_max_encap_length));
        return BCM_E_CONFIG;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_appl_callback
 * Purpose:
 *      Update FW BHH appl state
 * Parameters:
 *      unit  - (IN) Unit number.
 *      uC    - (IN) core number.
 *      stage - (IN) core reset stage.
 *      user_data - (IN) data pointer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int _bcm_kt2_oam_bhh_appl_callback(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data) {
    _bcm_oam_control_t *oc;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);
    oc->ukernel_not_ready = 1;

    _BCM_OAM_UNLOCK(oc);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oam_bhh_endpoint_create
 * Purpose:
 *      Initialize the HW for BHH packet processing.
 *      Configure:
 *      - Copy to CPU BHH error packets
 *      - CPU COS Queue for BHH packets
 *      - RX DMA channel
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_kt2_oam_bhh_endpoint_create(int unit, 
                                bcm_oam_endpoint_info_t *endpoint_info,
                                _bcm_oam_hash_key_t  *hash_key)
{
    _bcm_oam_control_t *oc;
    _bcm_oam_hash_data_t *hash_data = NULL; /* Endpoint hash data pointer.  */
    _bcm_oam_group_data_t *group_p;         /* Pointer to group data.       */
    int                  ep_req_index;      /* Requested endpoint index.    */
    int                  rv = BCM_E_NONE;   /* Operation return status.     */
    int                  is_remote = 0;     /* Remote endpoint status.      */
    int                  is_replace;
    int                  is_local = 0;         
    uint32               sglp = 0;          /* Source global logical port.  */
    uint32               dglp = 0;          /* Dest global logical port.    */
    bcm_module_t         module_id;         /* Module ID                    */
    bcm_port_t           port_id;
    bcm_trunk_t          trunk_id;
    int                  local_id;
    uint32               svp = 0;           /* Source virtual port          */
    bcm_port_t           src_pp_port = 0;   /* Source pp port.              */
    bcm_port_t           dst_pp_port = 0;   /* Dest pp port.                */
    bhh_sdk_msg_ctrl_sess_set_t msg_sess;
    bhh_sdk_msg_ctrl_rmep_create_t msg_rmep_create;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int encap = 0;
    uint32 session_flags;
    int bhh_pool_ep_idx = 0;
    egr_l3_next_hop_entry_t egr_nh_entry;
    int egr_nh_index;
    int is_vp_valid = 0;
    uint8 group_sw_rdi = 0;
    bcm_gport_t tx_gport;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "BHH Info: bcm_kt2_oam_bhh_endpoint_create"
                           "Endpoint ID=%d.\n"), endpoint_info->id));

    _BCM_OAM_BHH_IS_VALID(unit);

    /* Validate input parameter. */
    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Perform BHH specific validation checks */
    /* BHH EP can only be at the MAX Level */
    if ( endpoint_info->level != _BCM_OAM_EP_LEVEL_MAX ) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: EP Level should be equal to %d\n"),
                   _BCM_OAM_EP_LEVEL_MAX));
        return (BCM_E_PARAM);
    }

    /* Get MEP remote endpoint status. */
    is_remote = (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) ? 1 : 0;

    is_replace = ((endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) != 0);

    /* Validate EP Id if BCM_OAM_ENDPOINT_WITH_ID flag is set */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        if((endpoint_info->id < _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET) ||
           (endpoint_info->id >= (_BCM_OAM_BHH_KT2_ENDPOINT_OFFSET 
                                  + oc->bhh_endpoint_count))) {
            return (BCM_E_PARAM);
        }
    }

    /* Validate remote EP Id if BCM_OAM_ENDPOINT_REMOTE flag is set */
    if (is_remote) {
        if((endpoint_info->local_id < _BCM_OAM_BHH_KT2_ENDPOINT_OFFSET) ||
           (endpoint_info->local_id >= (_BCM_OAM_BHH_KT2_ENDPOINT_OFFSET
                                        + oc->bhh_endpoint_count))) {
            return (BCM_E_PARAM);
        }
    }

    if (is_replace) {
        hash_data = &oc->oam_hash_data[endpoint_info->id];
        if (!hash_data->in_use) {
            return (BCM_E_NOT_FOUND);
        }
        
        /* Delete original and recreate new EP */
        rv = _bcm_kt2_oam_endpoint_destroy(unit, endpoint_info->id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint destroy (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    /*
       BHH API have been using L3_EGRESS type in 
       bcm_oam_endpoint_info_t.intf field. 
       However for supporting LM/DM on Katana2 this should be DVP_EGRESS type for PW,
       only CCM will work with L3_EGRESS type, retaining the old type for 
       backward compatibility
    */
    if(!BHH_EP_MPLS_SECTION_TYPE(endpoint_info)){
        if ((endpoint_info->type == bcmOAMEndpointTypeBHHMPLSVccv) &&
            (endpoint_info->flags & (BCM_OAM_ENDPOINT_LOSS_MEASUREMENT |
                                     BCM_OAM_ENDPOINT_DELAY_MEASUREMENT))) {
                if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, endpoint_info->intf_id)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: bcm_oam_endpoint_info_t.intf type"
                                           " not valid. It should be DVP_EGRESS type for LM/DM support.\n")));
                    return (BCM_E_PARAM);
                }
        } else {
            if ( !BCM_XGS3_L3_EGRESS_IDX_VALID(unit, endpoint_info->intf_id) &
                !BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, endpoint_info->intf_id) ) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: bcm_oam_endpoint_info_t.intf type"
                                       " not valid. It should be DVP_EGRESS or L3_EGRESS type.\n")));
                return (BCM_E_PARAM);
            }
        }
    }
    /* Now that we passed the validation checks
     * Create a new endpoint with the requested ID. */
    if ( !is_remote && (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) ) {

        hash_data = &oc->oam_hash_data[endpoint_info->id];

        if (!is_replace && hash_data->in_use) {
            return (BCM_E_EXISTS);
        }

        ep_req_index = endpoint_info->id;
        bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(ep_req_index);

        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "BHH Error: Endpoint check (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            return (rv);
        }

        if(!is_replace) {
            rv = shr_idxres_list_reserve(oc->bhh_pool, bhh_pool_ep_idx,
                                     bhh_pool_ep_idx);
            if (BCM_FAILURE(rv)) {
                rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "BHH Error: Endpoint reserve (EP=%d) - %s.\n"),
                           endpoint_info->id, bcm_errmsg(rv)));
                return (rv);
            }
        }
    } else {

        /* BHH uses local and remote same index */
        if(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            ep_req_index = endpoint_info->local_id;
            bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(ep_req_index);
        }
        else {
            /* Allocate the next available endpoint index. */
            rv = shr_idxres_list_alloc(oc->bhh_pool,
                                   (shr_idxres_element_t *)&bhh_pool_ep_idx);
            if (BCM_FAILURE(rv)) {
                rv = (rv == BCM_E_RESOURCE) ? (BCM_E_FULL) : rv;
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "BHH Error: Endpoint alloc failed - %s.\n"),
                           bcm_errmsg(rv)));
                return (rv);
            }
            /* Set the allocated endpoint id value. */
            ep_req_index = BCM_OAM_BHH_GET_SDK_EP(bhh_pool_ep_idx);
        }
        endpoint_info->id =  ep_req_index;
    }

    /* Get the hash data pointer where the data is to be stored. */
    hash_data = &oc->oam_hash_data[ep_req_index];
    group_p = &oc->group_info[endpoint_info->group];

    /*
     * The uKernel is not provisioned until both endpoints (local and remote)
     * are provisioned in the host.
     */
    if (is_remote) {

        if (!hash_data->in_use) {
            return (BCM_E_NOT_FOUND);
        } else if (hash_data->flags & BCM_OAM_ENDPOINT_REMOTE) {
            return (BCM_E_EXISTS);
        }

        if(!(endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU)){
            /*
             * Now that both ends are provisioned the uKernel can be 
             * configured.
             */
            msg_rmep_create.sess_id = bhh_pool_ep_idx;
            msg_rmep_create.flags = 0;
            msg_rmep_create.enable = 1;
            msg_rmep_create.remote_mep_id = endpoint_info->name;

            msg_rmep_create.period = _kt2_ccm_intervals[
                _bcm_kt2_oam_ccm_msecs_to_hw_encode(endpoint_info->ccm_period)];

            /* Pack control message data into DMA buffer */
            buffer     = oc->dma_buffer;
            buffer_ptr = bhh_sdk_msg_ctrl_rmep_create_pack(buffer, &msg_rmep_create);
            buffer_len = buffer_ptr - buffer;

            /* Send BHH Session Update message to uC */
            rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                    MOS_MSG_SUBCLASS_BHH_RMEP_CREATE,
                    buffer_len, 0,
                    MOS_MSG_SUBCLASS_BHH_RMEP_CREATE_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv) || (reply_len != 0)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Endpoint destroy (EP=%d) - %s.\n"),
                         unit, endpoint_info->id, bcm_errmsg(rv)));
                return (BCM_E_INTERNAL);
            }
        }

    } else {
    
        /* Resolve given endpoint gport value to SGLP and DGLP values. */
        rv = _bcm_kt2_oam_endpoint_gport_resolve(unit, endpoint_info, &sglp,
                                                 &dglp, &src_pp_port,
                                                 &dst_pp_port, &svp, &trunk_id,
                                                 &is_vp_valid, &tx_gport);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Gport resolve (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            /* Return endpoint index to MEP pool. */
            shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
            return (rv);
        }
    
        /* Clear the hash data element contents before storing the values. */
        _BCM_OAM_HASH_DATA_CLEAR(hash_data);
        hash_data->oam_domain = _BCM_OAM_DOMAIN_BHH;
        hash_data->type = endpoint_info->type;
        hash_data->ep_id = endpoint_info->id;
        hash_data->group_index = endpoint_info->group;
        hash_data->level = endpoint_info->level;
        hash_data->vlan = endpoint_info->vlan;
        hash_data->inner_vlan = endpoint_info->inner_vlan;
        hash_data->outer_tpid = endpoint_info->outer_tpid;
        hash_data->inner_tpid = endpoint_info->inner_tpid;
        hash_data->vlan_pri = endpoint_info->pkt_pri;
        hash_data->inner_vlan_pri = endpoint_info->inner_pkt_pri;
        hash_data->gport = endpoint_info->gport;
        hash_data->trunk_index = endpoint_info->trunk_index;
        hash_data->sglp = sglp;
        hash_data->dglp = dglp;
        hash_data->flags = endpoint_info->flags;
        hash_data->flags2 = endpoint_info->flags2;
        hash_data->period = endpoint_info->ccm_period;
        hash_data->vccv_type = endpoint_info->vccv_type;
        hash_data->vpn  = endpoint_info->vpn;
        hash_data->name      = endpoint_info->name;
        hash_data->egress_if = endpoint_info->intf_id;
        hash_data->cpu_qid   = endpoint_info->cpu_qid;
        hash_data->int_pri   = endpoint_info->int_pri;
        hash_data->label     = endpoint_info->mpls_label;
        hash_data->local_tx_enabled = 0;
        hash_data->local_rx_enabled = 1;
        hash_data->ts_format = endpoint_info->timestamp_format;
        hash_data->egr_label = endpoint_info->egress_label.label;
        hash_data->egr_label_exp = endpoint_info->egress_label.exp;
        hash_data->egr_label_ttl = endpoint_info->egress_label.ttl;

        /* Initialize hardware index as invalid indices. */
        hash_data->local_tx_index = _BCM_OAM_INVALID_INDEX;
        hash_data->local_rx_index = _BCM_OAM_INVALID_INDEX;
        hash_data->remote_index = _BCM_OAM_INVALID_INDEX;
        hash_data->dglp1_profile_index = _BCM_OAM_INVALID_INDEX;
        hash_data->dglp2_profile_index = _BCM_OAM_INVALID_INDEX;
        hash_data->rx_ctr = _BCM_OAM_INVALID_INDEX;
        hash_data->tx_ctr = _BCM_OAM_INVALID_INDEX;
        hash_data->profile_index = _BCM_OAM_INVALID_INDEX;
        hash_data->pri_map_index = _BCM_OAM_INVALID_INDEX;
        hash_data->egr_pri_map_index = _BCM_OAM_INVALID_INDEX;
        hash_data->outer_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
        hash_data->subport_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
        hash_data->inner_tpid_profile_index = _BCM_OAM_INVALID_INDEX;
        hash_data->ma_base_index = _BCM_OAM_INVALID_INDEX;
        hash_data->pm_profile_attached = _BCM_OAM_INVALID_INDEX;
        sal_memcpy(hash_data->dst_mac_address, endpoint_info->dst_mac_address,
                    _BHH_MAC_ADDR_LENGTH);
        sal_memcpy(hash_data->src_mac_address, endpoint_info->src_mac_address,
                        _BHH_MAC_ADDR_LENGTH);
        hash_data->in_use = 1;

        if(!BHH_EP_MPLS_SECTION_TYPE(endpoint_info)) {

            /* Now that we have reserved the EP index, We can reserve MA INDEX */
            rv =  _bcm_kt2_oam_downmep_rx_endpoint_reserve(unit, endpoint_info);
            if (BCM_FAILURE(rv)) {
                /* Return endpoint index to MEP pool. */
                shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM Error: EP MA INDEX alloc"
                                    "failed EP:%d %s.\n"), endpoint_info->id,
                        bcm_errmsg(rv)));
                return (rv);
            }

            rv = _bcm_kt2_oam_local_rx_mep_hw_set(unit, endpoint_info);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: Rx config failed for EP=%d %s.\n"),
                        endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                return (rv);
            }

            /* Enable BHH in the EGR_L3_NEXT_HOP table */
            if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, endpoint_info->intf_id)) {
                egr_nh_index = endpoint_info->intf_id -
                                   BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            }
            else {
                egr_nh_index = endpoint_info->intf_id -
                                   BCM_XGS3_EGRESS_IDX_MIN(unit);
            }
            rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                    egr_nh_index, &egr_nh_entry);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: reading EGR_L3_NEXT_HOP,"
                                      "for EP=%d %s.\n"), endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                return (rv);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh_entry, MPLS__BHH_ENABLEf, 1); 
            rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL, 
                    egr_nh_index, &egr_nh_entry);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM Error: writing EGR_L3_NEXT_HOP,"
                                      "for EP=%d %s.\n"), endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                return (rv);
            }
        } else {
            if(hash_data->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
                shr_idxres_element_t ctr_index;

                rv = _bcm_kt2_oam_bhh_sec_mep_alloc_counter(unit, &ctr_index);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: LM counter block alloc - %s.\n"),
                                          bcm_errmsg(rv)));
                    _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                    return (rv);
                }
                hash_data->rx_ctr = (0 << 24) | (ctr_index); /* Pool id 0 for ingress counters*/
                hash_data->tx_ctr = (1 << 24) | (ctr_index); /* Pool id 1 for egress counters */
            }
        }
        /* Get the Trunk, Port and Modid info for this Gport */
        rv = _bcm_esw_gport_resolve(unit, tx_gport, &module_id, &port_id,
                                    &trunk_id, &local_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "BHH Error: Gport resolve (EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
            return (rv);
        }

        /* Get local port used for TX BHH packet */
        rv = _bcm_esw_modid_is_local(unit, module_id, &is_local);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "BHH Error: local port for "
                                  "BHH TX failed(EP=%d) - %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
            return (rv);
        }

        if (!is_local) {    /* HG port */
            rv = bcm_esw_stk_modport_get(unit, module_id, &port_id);
            if(BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "BHH Error: HG port get failed "
                                      "(EP=%d) - %s.\n"),
                           endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                return (rv);
            }
        }

        /* Create or Update */
        session_flags = (is_replace) ? 0 : SHR_BHH_SESS_SET_F_CREATE;

        /* Set Endpoint config entry */
        hash_data->bhh_endpoint_index = ep_req_index;

        /* Set Encapsulation in HW */
        if ((!BHH_EP_MPLS_SECTION_TYPE(endpoint_info)) &&
            ((!is_replace) || (endpoint_info->flags & BCM_BHH_ENDPOINT_ENCAP_SET))) {
            rv = _bcm_kt2_oam_bhh_encap_hw_set(unit, hash_data, module_id,
                          port_id, is_local, endpoint_info);
            if(BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "BHH Error: HW encap set failed (EP=%d)"
                                      " - %s.\n"),
                           endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                return (rv);
            }
        }

        encap = 1;

        if(!(endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU)) {
            /* Set control message data */
            sal_memset(&msg_sess, 0, sizeof(msg_sess));

            /*
             * Set the BHH encapsulation data
             *
             * The function _bcm_kt2_oam_bhh_encap_create() is called first
             * since this sets some fields in 'hash_data' which are
             * used in the message.
             */
            if (encap) {
                rv = _bcm_kt2_oam_bhh_encap_create(unit,
                        port_id,
                        hash_data,
                        msg_sess.encap_data,
                        &msg_sess.encap_type,
                        &msg_sess.encap_length);

                if(BCM_FAILURE(rv))
                {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "BHH Error: Endpoint destroy (EP=%d) - %s.\n"),
                             endpoint_info->id, bcm_errmsg(rv)));
                    _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                    return (rv);
                }
            }

            /*
             * Endpoint can be one of: CCM, LB, LM or DM. The default is CCM,
             * all others modes must be specified via a config flag.
 */
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_LOOPBACK)
                session_flags |= SHR_BHH_SESS_SET_F_LB;
            else if (endpoint_info->flags & BCM_OAM_ENDPOINT_DELAY_MEASUREMENT)
                session_flags |= SHR_BHH_SESS_SET_F_DM;
            else if (endpoint_info->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT)
                session_flags |= SHR_BHH_SESS_SET_F_LM;
            else
                session_flags |= SHR_BHH_SESS_SET_F_CCM;

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE)
                session_flags |= SHR_BHH_SESS_SET_F_MIP;

            _bcm_kt2_oam_get_group_sw_rdi (unit, endpoint_info->group,
                    &group_sw_rdi);
            if (group_sw_rdi) {
                session_flags |= SHR_BHH_SESS_SET_F_RDI;
            }
            /*if (!local_tx_enabled)
              session_flags |= SHR_BHH_SESS_SET_F_PASSIVE;*/

            msg_sess.sess_id = bhh_pool_ep_idx;
            msg_sess.flags   = session_flags;

            msg_sess.mel     = hash_data->level;
            msg_sess.mep_id  = hash_data->name;
            sal_memcpy(msg_sess.meg_id, group_p->name, BCM_OAM_GROUP_NAME_LENGTH);

            msg_sess.period = _kt2_ccm_intervals[
                _bcm_kt2_oam_ccm_msecs_to_hw_encode(endpoint_info->ccm_period)];

            msg_sess.if_num  = endpoint_info->intf_id;
            msg_sess.tx_port = port_id;
            msg_sess.tx_cos  = endpoint_info->int_pri;
            msg_sess.tx_pri  = endpoint_info->pkt_pri;
            msg_sess.tx_qnum = SOC_INFO(unit).port_uc_cosq_base[port_id] + endpoint_info->int_pri;
            msg_sess.lm_counter_index
                = hash_data->tx_ctr;

            msg_sess.mpls_label = endpoint_info->mpls_label;

            switch (endpoint_info->type) {
            case bcmOAMEndpointTypeBHHMPLS:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_LSP;
                msg_sess.priority   = endpoint_info->mpls_exp;
                break;

            case bcmOAMEndpointTypeBHHMPLSVccv:
                switch(endpoint_info->vccv_type) {

                case bcmOamBhhVccvChannelAch:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_1;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                case bcmOamBhhVccvRouterAlert:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_2;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                case bcmOamBhhVccvTtl:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_3;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                case bcmOamBhhVccvGal13:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_4;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                default:
                    return BCM_E_PARAM;
                    break;
                }
                break;

            case bcmOAMEndpointTypeBhhSection:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION;
                msg_sess.priority   = 0;
                break;
            case bcmOAMEndpointTypeBhhSectionInnervlan:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION_INNERVLAN;
                msg_sess.priority   = endpoint_info->inner_pkt_pri;
                break;
            case bcmOAMEndpointTypeBhhSectionOuterVlan:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION_OUTERVLAN;
                msg_sess.priority   = endpoint_info->pkt_pri;
                break;
            case bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION_OUTER_PLUS_INNERVLAN;
                msg_sess.priority   = endpoint_info->pkt_pri;
                break;

            default:
                return BCM_E_PARAM;
            }

            /* Pack control message data into DMA buffer */
            buffer     = oc->dma_buffer;
            buffer_ptr = bhh_sdk_msg_ctrl_sess_set_pack(buffer, &msg_sess);
            buffer_len = buffer_ptr - buffer;

            /* Send BHH Session Update message to uC */
            rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                    MOS_MSG_SUBCLASS_BHH_SESS_SET,
                    buffer_len, 0,
                    MOS_MSG_SUBCLASS_BHH_SESS_SET_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv) || (reply_len != 0)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH Error: Endpoint destroy (EP=%d) - %s.\n"),
                         endpoint_info->id, bcm_errmsg(rv)));
                _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
                return (rv);
            }
        }

        if (!is_replace) {
            rv = _bcm_kt2_oam_group_ep_list_add(unit, endpoint_info->group,
                    endpoint_info->id);
        }
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Tx config failed for EP=%d %s.\n"),
                       endpoint_info->id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
            return (rv);
        }
        rv = _bcm_kt2_oam_ma_idx_to_ep_id_mapping_add(unit, hash_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: Add mapping from ma_idx_to_ep_id list (EP=%d) -"
                                " %s.\n"), unit, hash_data->ep_id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
            return (rv);
        }

        /* hash collision */
        rv = shr_htb_insert(oc->ma_mep_htbl, hash_key, hash_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM Error: Hash table insert failed EP=%d %s.\n"),
                     endpoint_info->id, bcm_errmsg(rv)));
            _bcm_kt2_oam_endpoint_cleanup(unit, 0, *hash_key, hash_data);
            return (rv);
        }
    }
    hash_data->in_use = 1;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return (rv);
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_hw_init
 * Purpose:
 *      Initialize the HW for BHH packet processing.
 *      Configure:
 *      - Copy to CPU BHH error packets
 *      - CPU COS Queue for BHH packets
 *      - RX DMA channel
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt2_oam_bhh_hw_init(int unit)
{
    int rv = BCM_E_NONE;
    _bcm_oam_control_t   *oc;               /* OAM control structure.        */
    uint32 val=0;
    int index;
    int ach_error_index;
    int invalid_error_index;
    int bhh_lb_index;
    int cosq_map_size;
    bcm_rx_reasons_t reasons, reasons_mask;
    uint8 int_prio, int_prio_mask;
    uint32 packet_type, packet_type_mask;
    bcm_cos_queue_t cosq;
    bcm_rx_chan_t chan_id;
    int num_cosq = 0;
    int min_cosq, max_cosq;
    int i;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    /*
     * Send BHH lookup failure packet to CPU
     * Configure CPU_CONTROL_M register
     * Field BHH_SESSION_NOT_FOUND_TO_CPU
     */
    rv = READ_CPU_CONTROL_Mr(unit, &val);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "BHH Error:hw init. Read CPU Control Reg %s.\n"),
                   bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Steer lookup failure packet to CPU */
    soc_reg_field_set(unit, CPU_CONTROL_Mr, &val,
                      BHH_SESSION_NOT_FOUND_TO_CPUf, 1);

    rv = WRITE_CPU_CONTROL_Mr(unit, val);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "BHH Error:hw init. Write CPU_CONTROL_M Reg %s.\n"),
                   bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Set BHH ACH Type in BHH_RX_ACH_TYPE register */
    rv = WRITE_BHH_RX_ACH_TYPEr(unit, SHR_BHH_ACH_CHANNEL_TYPE); 

    if(oc->ukernel_not_ready == 0){
        /* 
         * Get COSQ for BHH
         */
        min_cosq = 0;
        for (i = 0; i < SOC_CMCS_NUM(unit); i++) {
            num_cosq = NUM_CPU_ARM_COSQ(unit, i);
            if (i == oc->uc_num + 1) {
                break;
            }
            min_cosq += num_cosq;
        }
        max_cosq = min_cosq + num_cosq - 1;

        if(max_cosq < min_cosq) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH Error: "
                                "No BHH COS Queue available from uC%d - %s\n"),
                     oc->uc_num, bcm_errmsg(BCM_E_CONFIG)));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_CONFIG);
        }

        /* check user configured COSq */
        if (oc->cpu_cosq != BHH_COSQ_INVALID) {
            if ((oc->cpu_cosq < min_cosq) ||
                    (oc->cpu_cosq > max_cosq)) {
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_CONFIG);
            }
            min_cosq = max_cosq = oc->cpu_cosq;
        }


        /*
         * Assign RX DMA channel to CPU COS Queue
         * (This is the RX channel to listen on for BHH packets).
         *
         * DMA channels (12) are assigned 4 per processor:
         * (see /src/bcm/common/rx.c)
         *   channels 0..3  --> PCI host
         *   channels 4..7  --> uController 0
         *   chnanels 8..11 --> uController 1
         *
         * The uControllers designate the 4 local DMA channels as follows:
         *   local channel  0     --> TX
         *   local channel  1..3  --> RX
         *
         * Each uController application needs to use a different
         * RX DMA channel to listen on.
         */
        chan_id = (BCM_RX_CHANNELS * (SOC_ARM_CMC(unit, oc->uc_num))) +
            oc->rx_channel;

        for (i = max_cosq; i >= min_cosq; i--) {
            rv = _bcm_common_rx_queue_channel_set(unit, i, chan_id);
            if(BCM_SUCCESS(rv)) {
                oc->cpu_cosq = i;
                break;
            }
        }

        if (i < min_cosq) {
            rv = BCM_E_RESOURCE;
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH Error: hw init."
                                " queue channel set %s.\n"),
                     bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }

        /*
         * Direct BHH packets to designated CPU COS Queue...or more accurately
         * BFD error packets.
         *
         * Reasons:
         *   - bcmRxReasonBFD:               BFD error
         *   - bcmRxReasonBfdUnknownVersion: BFD Unknown ACH
         * NOTE:
         *     The user input 'cpu_qid' (bcm_BHH_endpoint_t) could be
         *     used to select different CPU COS queue. Currently,
         *     all priorities are mapped into the same CPU COS Queue.
         */

        /* Find available entries in CPU COS queue map table */
        ach_error_index    = -1;   /* COSQ map index for error packets */
        invalid_error_index    = -1;   /* COSQ map index for error packets */
        bhh_lb_index    = -1;   /* COSQ map index for LB packets */
        rv = bcm_esw_rx_cosq_mapping_size_get(unit, &cosq_map_size);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH Error:hw init. cosq maps size %s.\n"),
                     bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }

        for (index = cosq_map_size-1; index >= 0; index--) { 
            rv = bcm_esw_rx_cosq_mapping_get(unit, index,
                    &reasons, &reasons_mask,
                    &int_prio, &int_prio_mask,
                    &packet_type, &packet_type_mask,
                    &cosq);
            if (rv == BCM_E_NOT_FOUND) {
                /* Assign first available index  before default entries*/
                rv = BCM_E_NONE;
                if (ach_error_index == -1) {
                    ach_error_index = index;
                } else if (invalid_error_index == -1) {
                    invalid_error_index = index;
                } else if (bhh_lb_index == -1) {
                    bhh_lb_index = index;
                    break;
                }
            }
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH Error:hw init. cosq maps get %s.\n"),
                         bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }

        if (ach_error_index == -1 || invalid_error_index == -1) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH Error:hw init. ACH error %s.\n"),
                     bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_FULL);
        }

        /* Set CPU COS Queue mapping */
        BCM_RX_REASON_CLEAR_ALL(reasons);
        BCM_RX_REASON_SET(reasons, bcmRxReasonBHHOAM);         /* BHH Packet     */
        BCM_RX_REASON_SET(reasons, bcmRxReasonOAMCCMSlowpath); /*OAM Slowpath CCM*/

        rv = bcm_esw_rx_cosq_mapping_set(unit, ach_error_index,
                reasons, reasons,
                0, 0, /* Any Internal Prio */  
                0, 0, /* Any packet type   */
                oc->cpu_cosq);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH Error:hw init. cosq map set %s.\n"),
                     bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        oc->cpu_cosq_ach_error_index = ach_error_index;


        BCM_RX_REASON_CLEAR_ALL(reasons);
        BCM_RX_REASON_SET(reasons, bcmRxReasonBHHOAM);      /* BHH Packet         */
        BCM_RX_REASON_SET(reasons, bcmRxReasonOAMLMDM); /*OAM Slowpath(LB/LBR)*/

        rv = bcm_esw_rx_cosq_mapping_set(unit, invalid_error_index,
                reasons, reasons,
                0, 0, /* Any Internal Prio */  
                0, 0, /* Any packet type */
                oc->cpu_cosq);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH Error:hw init. cosq map set %s.\n"),
                     bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        oc->cpu_cosq_invalid_error_index = invalid_error_index;
    
        BCM_RX_REASON_CLEAR_ALL(reasons);
        BCM_RX_REASON_SET(reasons, bcmRxReasonBHHOAM);      /* BHH Packet     */
        BCM_RX_REASON_SET(reasons, bcmRxReasonOAMSlowpath); /*OAM Slowpath */

        rv = bcm_esw_rx_cosq_mapping_set(unit, bhh_lb_index,
                                         reasons, reasons,
                                         0, 0, /* Any Internal Prio */  
                                         0, 0, /* Any packet type   */
                                         oc->cpu_cosq);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "BHH(unit %d) Error:hw init. cosq map set for"
                                  " Loopback %s.\n"),
                       unit, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        oc->bhh_lb_index = bhh_lb_index;
    }

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_session_hw_delete
 * Purpose:
 *      Delete BHH Session in HW device.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      h_data_p- (IN) Pointer to BHH endpoint structure.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt2_oam_bhh_session_hw_delete(int unit, _bcm_oam_hash_data_t *h_data_p)
{
    int rv = BCM_E_NONE;
#if defined(BCM_KATANA2_SUPPORT) && defined(BCM_MPLS_SUPPORT)
    mpls_entry_entry_t mpls_entry;
    mpls_entry_entry_t mpls_key;
    int mpls_index = 0;
#endif /* BCM_KATANA2_SUPPORT &&  BCM_MPLS_SUPPORT */


    switch(h_data_p->type) {
    case bcmOAMEndpointTypeBHHMPLS:
    case bcmOAMEndpointTypeBHHMPLSVccv:
#if defined(BCM_KATANA2_SUPPORT) && defined(BCM_MPLS_SUPPORT)
        SOC_IF_ERROR_RETURN(bcm_tr_mpls_lock (unit));

        sal_memset(&mpls_key, 0, sizeof(mpls_key));
        soc_MPLS_ENTRYm_field32_set(unit, &mpls_key,
                                    MPLS__MPLS_LABELf, h_data_p->label);

        rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &mpls_index,
                                           &mpls_key, &mpls_entry, 0);
        /* It can so happen that this function is called when the switch is
           re-initailizing, then MPLS ENTRY will not be found, as MPLS init 
           happens before OAM init and it would have cleared the MPLS Entry, as 
           such its not an error scenario. Hence dont return failure from here.*/

        if (BCM_SUCCESS(rv)) {
            if ((soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry, VALIDf) == 0x1)) 
            {
                soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, 
                                                        MPLS__BFD_ENABLEf, 0);
                rv = soc_mem_write(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, 
                        mpls_index, &mpls_entry);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "OAM Error: MPLS_ENTRY write failed - "
                                           "%s.\n"), bcm_errmsg(rv)));
                    bcm_tr_mpls_unlock(unit);
                    return (rv);
                }
            }
        }
        bcm_tr_mpls_unlock (unit);
#endif /* BCM_KATANA2_SUPPORT &&  BCM_MPLS_SUPPORT */
        break;

    default:
        break;
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_msg_send_receive
 * Purpose:
 *      Sends given BHH control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_subclass  - (IN) BHH message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 *
 *     BHH Long Control message:
 *     - BHH control messages that require send/receive of information
 *       that cannot fit in the uc_msg 'len' and 'data' fields need to
 *       use DMA operations to exchange information (long control message).
 *
 *     - BHH convention for long control messages for
 *        'mos_msg_data_t' fields:
 *          'len'    size of the DMA buffer to send to uController
 *          'data'   physical DMA memory address to send or receive
 *
 *      DMA Operations:
 *      - DMA read/write operation is performed when a long BHH control
 *        message is involved.
 *
 *      - Messages that require DMA operation (long control message)
 *        is indicated by MOS_MSG_DMA_MSG().
 *
 *      - Callers must 'pack' and 'unpack' corresponding information
 *        into/from DMA buffer indicated by BHH_INFO(unit)->dma_buffer.
 */
STATIC int
_bcm_kt2_oam_bhh_msg_send_receive(int unit, uint8 s_subclass,
                                    uint16 s_len, uint32 s_data,
                                    uint8 r_subclass, uint16 *r_len)
{
    int rv;
    _bcm_oam_control_t *oc;
    mos_msg_data_t send, reply;
    uint8 *dma_buffer;
    int dma_buffer_len;
    uint32 uc_rv;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_BHH;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    dma_buffer = oc->dma_buffer;
    dma_buffer_len = oc->dma_buffer_len;
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, oc->uc_num,
                                      &send, &reply,
                                      _BHH_UC_MSG_TIMEOUT_USECS);

    /* Check reply class, subclass */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != MOS_MSG_CLASS_BHH) ||
        (reply.s.subclass != r_subclass)) {
        return (BCM_E_INTERNAL);
    }

    /* Convert BHH uController error code to BCM */
    uc_rv = bcm_ntohl(reply.s.data);
    switch(uc_rv) {
    case SHR_BHH_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_BHH_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_BHH_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_BHH_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_BHH_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_BHH_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_BHH_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_BHH_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }
        
    *r_len = bcm_ntohs(reply.s.len);

    return (rv);
}

/*
 * Function:
 *      _bcm_kt2_oam_bhh_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to BFD info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_kt2_oam_bhh_callback_thread(void *param)
{
    int rv;
    _bcm_oam_control_t *oc = (_bcm_oam_control_t *)param;
    bcm_oam_event_types_t events;
    bcm_oam_event_type_t event_type;
    bhh_msg_event_t event_msg;
    int sess_id;
    uint32 event_mask, flags = 0;
    _bcm_oam_event_handler_t *event_handler_p;
    _bcm_oam_hash_data_t *h_data_p;
    int ep_id = 0;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];

    thread_name[0] = 0;
    sal_thread_name(oc->event_thread_id, thread_name, sizeof (thread_name));

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(oc->unit, oc->uc_num,
                                     MOS_MSG_CLASS_BHH_EVENT, &event_msg,
                                     sal_sem_FOREVER);

        if (BCM_FAILURE(rv)) {
            break;  /*  Thread exit */
        }

        event_handler_p = oc->event_handler_list_p;
        /* Get data from event message */
        sess_id = (int)bcm_ntohs(event_msg.s.len);
        ep_id = BCM_OAM_BHH_GET_SDK_EP(sess_id);

        if (sess_id < 0 ||
            sess_id >= oc->ep_count) {
            LOG_CLI((BSL_META_U(oc->unit,
                                "Invalid sess_id:%d \n"), sess_id));
            continue;
        }
        /* Check endpoint status. */
        rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
        if ((BCM_E_EXISTS != rv)) {
            /* Endpoint not in use. */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "OAM Error: Endpoint EP=%d %s.\n"),
                       ep_id, bcm_errmsg(rv)));
        }
        h_data_p = &oc->oam_hash_data[ep_id];
        event_mask = bcm_ntohl(event_msg.s.data);


        /* Set events */
        sal_memset(&events, 0, sizeof(events));

        if (event_mask & BHH_BTE_EVENT_LB_TIMEOUT) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH LB Timeout ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventBHHLBTimeout] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHLBTimeout);
        }
        if (event_mask & BHH_BTE_EVENT_LB_DISCOVERY_UPDATE) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH LB Discovery Update ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHLBDiscoveryUpdate] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHLBDiscoveryUpdate);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_TIMEOUT) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM Timeout ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMTimeout] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMTimeout);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_TIMEOUT_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM Timeout Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMTimeoutClear] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMTimeoutClear);
        }
        if (event_mask & BHH_BTE_EVENT_STATE) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH State ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMState] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMState);
        }

        if (event_mask & BHH_BTE_EVENT_CCM_RDI) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM RDI ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMRdi] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMRdi);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM  Unknown MEG LEVEL ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegLevel] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegLevel);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM  Unknown MEG ID ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegId] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegId);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM  Unknown MEP ID ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMepId] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMepId);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPeriod] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPeriod);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPriority] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPriority);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_RDI_CLEAR) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMRdiClear] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMRdiClear);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL_CLEAR) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegLevelClear] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegLevelClear);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID_CLEAR) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegIdClear] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegIdClear);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID_CLEAR) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMepIdClear] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMepIdClear);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD_CLEAR) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPeriodClear] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPeriodClear);
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY_CLEAR) {
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPriorityClear] > 0)
            SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPriorityClear);
        }
        if (event_mask & BHH_BTE_EVENT_CSF_LOS) {
            if (oc->event_handler_cnt[bcmOAMEventCsfLos] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfLos);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CSF_FDI) {
            if (oc->event_handler_cnt[bcmOAMEventCsfFdi] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfFdi);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CSF_RDI) {
            if (oc->event_handler_cnt[bcmOAMEventCsfRdi] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfRdi);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CSF_DCI) {
            if (oc->event_handler_cnt[bcmOAMEventCsfDci] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfDci);
            }
        }
        if (event_mask & BHH_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                    (BSL_META_U(oc->unit,
                                "****** OAM PM BHH Counter Rollover ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventBhhPmCounterRollover] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBhhPmCounterRollover);
            }
        }
        /* Loop over registered callbacks,
         * If any match the events field, then invoke
         */
        for (event_handler_p = oc->event_handler_list_p;
             event_handler_p != NULL;
             event_handler_p = event_handler_p->next_p) {
            for (event_type = bcmOAMEventBHHLBTimeout; event_type < bcmOAMEventCount; ++event_type) {
                if (SHR_BITGET(events.w, event_type)) {
                    if (SHR_BITGET(event_handler_p->event_types.w,
                                   event_type)) {
                        event_handler_p->cb(oc->unit,
                                        flags,
                                        event_type,
                                        h_data_p->group_index, /* Group index */
                                        ep_id, /* Endpoint index */
                                        event_handler_p->user_data);
                    }
                }
            }
        }
    }

    oc->event_thread_id = NULL;
    LOG_VERBOSE(BSL_LS_BCM_OAM,
              (BSL_META_U(oc->unit,
                          "Thread Exit:%s\n"), thread_name));
    sal_thread_exit(0);
}

/* BHH Event Handler */
typedef struct _event_handler_s {
    struct _event_handler_s *next;
    bcm_oam_event_types_t event_types;
    bcm_oam_event_cb cb;
    void *user_data;
} _event_handler_t;

/*
 * Function:
 *      _bcm_kt2_oam_bhh_event_mask_set
 * Purpose:
 *      Set the BHH Events mask.
 *      Events are set per BHH module.
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_XXX  Operation failed
 * Notes:
 */
STATIC int
_bcm_kt2_oam_bhh_event_mask_set(int unit)
{
    _bcm_oam_control_t *oc;
    _bcm_oam_event_handler_t *event_handler_p;
    uint32 event_mask = 0;
    uint16 reply_len;
    int rv = BCM_E_NONE;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    /* Get event mask from all callbacks */
    for (event_handler_p = oc->event_handler_list_p;
         event_handler_p != NULL;
         event_handler_p = event_handler_p->next_p) {

        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHLBTimeout)) {
            event_mask |= BHH_BTE_EVENT_LB_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHLBDiscoveryUpdate)) {
            event_mask |= BHH_BTE_EVENT_LB_DISCOVERY_UPDATE;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMTimeout)) {
            event_mask |= BHH_BTE_EVENT_CCM_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMTimeoutClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_TIMEOUT_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMState)) {
            event_mask |= BHH_BTE_EVENT_STATE;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMRdi)) {
            event_mask |= BHH_BTE_EVENT_CCM_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegLevel)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegId)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMepId)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPeriod)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPriority)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMRdiClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_RDI_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegLevelClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegIdClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMepIdClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPeriodClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPriorityClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfLos)) {
            event_mask |= BHH_BTE_EVENT_CSF_LOS;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfFdi)) {
            event_mask |= BHH_BTE_EVENT_CSF_FDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfRdi)) {
            event_mask |= BHH_BTE_EVENT_CSF_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfDci)) {
            event_mask |= BHH_BTE_EVENT_CSF_DCI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                    bcmOAMEventBhhPmCounterRollover)) {
            event_mask |= BHH_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER;
        }
    }

    /* Update BHH event mask in uKernel */
    if (event_mask != oc->event_mask) {
        /* Send BHH Event Mask message to uC */
        rv = _bcm_kt2_oam_bhh_msg_send_receive
                (unit,
                 MOS_MSG_SUBCLASS_BHH_EVENT_MASK_SET,
                 0, event_mask,
                 MOS_MSG_SUBCLASS_BHH_EVENT_MASK_SET_REPLY,
                 &reply_len);

        if(BCM_SUCCESS(rv) && (reply_len != 0)) {
            rv = BCM_E_INTERNAL;
        }
    }

    oc->event_mask = event_mask;

    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_loopback_add
 * Purpose:
 *     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_p)
{
    _bcm_oam_control_t *oc;
    _bcm_oam_hash_data_t *h_data_p;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_loopback_add_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    uint32 flags = 0;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }


    BCM_OAM_BHH_VALIDATE_EP(loopback_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loopback_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   loopback_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[loopback_p->id];
    if (h_data_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }


    /*
     * Only BHH is supported
     */
    if (BHH_EP_TYPE(h_data_p)) {

        /*
         * Convert host space flags to uKernel space flags
         */

        if (loopback_p->flags & BCM_OAM_LOOPBACK_TX_ENABLE) {
            flags |= BCM_BHH_TX_ENABLE;
        }

        if (loopback_p->flags & BCM_OAM_BHH_INC_REQUESTING_MEP_TLV) {
            flags |= BCM_BHH_INC_REQUESTING_MEP_TLV;
        }

        if (loopback_p->flags & BCM_OAM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV) {
            flags |= BCM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV;
        } else if (loopback_p->flags & BCM_OAM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV) {
            flags |= BCM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV;
        } else if (loopback_p->flags & BCM_OAM_BHH_LBM_ICC_MEP_TLV) {
            flags |= BCM_BHH_LBM_ICC_MEP_TLV;
        } else if (loopback_p->flags & BCM_OAM_BHH_LBM_ICC_MIP_TLV) {
            flags |= BCM_BHH_LBM_ICC_MIP_TLV;
        } else {
            flags |= BCM_BHH_LBM_ICC_MEP_TLV; /* Default */
        }

        if (loopback_p->flags & BCM_OAM_BHH_LBR_ICC_MEP_TLV) {
            flags |= BCM_BHH_LBR_ICC_MEP_TLV;
        } else if (loopback_p->flags & BCM_OAM_BHH_LBR_ICC_MIP_TLV) {
            flags |= BCM_BHH_LBR_ICC_MIP_TLV;
        }

        sal_memset(&msg, 0, sizeof(msg));
        msg.flags   = flags;
        msg.sess_id = sess_id;
        msg.int_pri = loopback_p->int_pri;
        msg.pkt_pri = loopback_p->pkt_pri;

        /*
         * Set period
         */
        msg.period  = _kt2_ccm_intervals[
                             _bcm_kt2_oam_ccm_msecs_to_hw_encode(
                                                         loopback_p->period)];

        /*
         * Check TTL
         */
        if (loopback_p->ttl == 0 || loopback_p->ttl > 255) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
        }

        if (BHH_EP_MPLS_SECTION_TYPE(h_data_p) ||
            (h_data_p->type == bcmOAMEndpointTypeBHHMPLSVccv &&
             h_data_p->vccv_type == bcmOamBhhVccvTtl)) {

            msg.ttl = 1; /* Only TTL 1 is valid */
        } else {
            msg.ttl = loopback_p->ttl;
        }

        /* Pack control message data into DMA buffer */
        buffer     = oc->dma_buffer;
        buffer_ptr = bhh_sdk_msg_ctrl_loopback_add_pack(buffer, &msg);
        buffer_len = buffer_ptr - buffer;

        /* Send BHH Session Update message to uC */
        rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                                         MOS_MSG_SUBCLASS_BHH_LOOPBACK_ADD,
                                         buffer_len, 0,
                                         MOS_MSG_SUBCLASS_BHH_LOOPBACK_ADD_REPLY,
                                         &reply_len);

        if(BCM_SUCCESS(rv) && (reply_len != 0)) {
            rv =  BCM_E_INTERNAL;
        }
    }
    else {
        rv =  BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_loopback_get
 * Purpose:
 *     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_p)
{
    _bcm_oam_control_t *oc;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_loopback_get_t msg;
    _bcm_oam_hash_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }


    BCM_OAM_BHH_VALIDATE_EP(loopback_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loopback_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   loopback_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[loopback_p->id];

    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

        sal_memset(&msg, 0, sizeof(msg));
        /* Send BHH Session Update message to uC */
        rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                                        MOS_MSG_SUBCLASS_BHH_LOOPBACK_GET,
                                        sess_id, 0,
                                        MOS_MSG_SUBCLASS_BHH_LOOPBACK_GET_REPLY,
                                        &reply_len);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM Error: Endpoint EP=%d %s.\n"),
                       loopback_p->id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }

        /* Pack control message data into DMA buffer */
        buffer     = oc->dma_buffer;
        buffer_ptr = bhh_sdk_msg_ctrl_loopback_get_unpack(buffer, &msg);
        buffer_len = buffer_ptr - buffer;


        if (reply_len != buffer_len) {
            rv =  BCM_E_INTERNAL;
        } else {
            /*
             * Convert kernel space flags to host space flags
             */
            if (msg.flags & BCM_BHH_INC_REQUESTING_MEP_TLV) {
                loopback_p->flags |= BCM_OAM_BHH_INC_REQUESTING_MEP_TLV;
            }

            if (msg.flags & BCM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV) {
                loopback_p->flags |= BCM_OAM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV;
            } else if (msg.flags & BCM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV) {
                loopback_p->flags |= BCM_OAM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV;
            } else if (msg.flags & BCM_BHH_LBM_ICC_MEP_TLV) {
                loopback_p->flags |= BCM_OAM_BHH_LBM_ICC_MEP_TLV;
            } else if (msg.flags & BCM_BHH_LBM_ICC_MIP_TLV) {
                loopback_p->flags |= BCM_OAM_BHH_LBM_ICC_MIP_TLV;
            }

            if (msg.flags & BCM_BHH_LBR_ICC_MEP_TLV) {
                loopback_p->flags |= BCM_OAM_BHH_LBR_ICC_MEP_TLV;
            } else if (msg.flags & BCM_BHH_LBR_ICC_MIP_TLV) {
                loopback_p->flags |= BCM_OAM_BHH_LBR_ICC_MIP_TLV;
            }

            if (msg.flags & BCM_BHH_TX_ENABLE) {
                loopback_p->flags |= BCM_OAM_LOOPBACK_TX_ENABLE;
            }

            loopback_p->period                  = msg.period;
            loopback_p->ttl                     = msg.ttl;
            loopback_p->discovered_me.flags     = msg.discovery_flags;
            loopback_p->discovered_me.name      = msg.discovery_id;
            loopback_p->discovered_me.ttl       = msg.discovery_ttl;
            loopback_p->rx_count                = msg.rx_count;
            loopback_p->tx_count                = msg.tx_count;
            loopback_p->drop_count              = msg.drop_count;
            loopback_p->unexpected_response     = msg.unexpected_response;
            loopback_p->out_of_sequence         = msg.out_of_sequence;
            loopback_p->local_mipid_missmatch   = msg.local_mipid_missmatch;
            loopback_p->remote_mipid_missmatch  = msg.remote_mipid_missmatch;
            loopback_p->invalid_target_mep_tlv  = msg.invalid_target_mep_tlv;
            loopback_p->invalid_mep_tlv_subtype = msg.invalid_mep_tlv_subtype;
            loopback_p->invalid_tlv_offset      = msg.invalid_tlv_offset;
            loopback_p->int_pri                 = msg.int_pri;
            loopback_p->pkt_pri                 = msg.pkt_pri;

            rv = BCM_E_NONE;
        }
    }
    else {
        rv = BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_loopback_delete
 * Purpose:
 *     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_p)
{
    _bcm_oam_control_t *oc;
    int rv = BCM_E_NONE;
    shr_bhh_msg_ctrl_loopback_delete_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;
    _bcm_oam_hash_data_t *h_data_p;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }


    BCM_OAM_BHH_VALIDATE_EP(loopback_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loopback_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   loopback_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[loopback_p->id];

    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

        msg.sess_id = sess_id;

        /* Pack control message data into DMA buffer */
        buffer     = oc->dma_buffer;
        buffer_ptr = shr_bhh_msg_ctrl_loopback_delete_pack(buffer, &msg);
        buffer_len = buffer_ptr - buffer;

        /* Send BHH Session Update message to uC */
        rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                      MOS_MSG_SUBCLASS_BHH_LOOPBACK_DELETE,
                      buffer_len, 0,
                      MOS_MSG_SUBCLASS_BHH_LOOPBACK_DELETE_REPLY,
                      &reply_len);

        if(BCM_SUCCESS(rv) && (reply_len != 0))
            rv =  BCM_E_INTERNAL;
    }
    else {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_loss_add
 * Purpose:
 *        Loss Measurement add     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_loss_add(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_oam_control_t *oc;
    _bcm_oam_hash_data_t *h_data_p;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_loss_add_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    uint32 flags = 0;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }


    BCM_OAM_BHH_VALIDATE_EP(loss_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loss_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[loss_p->id];
    if (h_data_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

	/*
	 * Convert host space flags to uKernel space flags
	 */
	if (loss_p->flags & BCM_OAM_LOSS_SINGLE_ENDED)
	    flags |= BCM_BHH_LM_SINGLE_ENDED;

	if (loss_p->flags & BCM_OAM_LOSS_TX_ENABLE)
	    flags |= BCM_BHH_LM_TX_ENABLE;

        sal_memset(&msg, 0, sizeof(msg));
	msg.flags   = flags;
	msg.sess_id = sess_id;

	/*
	 * Set period
	 */
	msg.period  = _kt2_ccm_intervals[_bcm_kt2_oam_ccm_msecs_to_hw_encode(loss_p->period)];
    msg.int_pri = loss_p->int_pri;
    msg.pkt_pri = loss_p->pkt_pri;

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_loss_add_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_ADD,
                      buffer_len, 0,
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_ADD_REPLY,
                      &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0)) 
        rv =  BCM_E_INTERNAL;
    }
    else {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

int
bcm_kt2_oam_bhh_convert_ep_to_time_spec(bcm_time_spec_t* bts, int sec, int ns)
{
    int rv = BCM_E_NONE;

    if (bts != NULL) {
        /* if both seconds and nanoseconds are negative or both positive,
         * then the ucode's subtraction is ok.
         * if seconds and nanoseconds have different signs, then "borrow"
         * 1000000000 nanoseconds from seconds.
         */
        if ((sec < 0) && (ns > 0)) {
            ns -= 1000000000;
            sec += 1;
        } else if ((sec > 0) && (ns < 0)) {
            ns += 1000000000;
            sec -= 1;
        }

        if (ns < 0) {
            /* if still negative, then something else is wrong.
             * the nanoseconds field is the difference between two
             * (non-negative) time-stamps.
             */
            rv = BCM_E_INTERNAL;
        }

        /* if seconds is negative then set the bts is-negative flag,
         * and use the absolute value of seconds & nanoseconds.
         */
        bts->isnegative  = (sec < 0 ? 1 : 0);
        bts->seconds     = BCM_OAM_BHH_ABS(sec);
        bts->nanoseconds = BCM_OAM_BHH_ABS(ns);
    } else {
        rv = BCM_E_INTERNAL;
    }

    return rv;
}

#ifdef _KATANA2_DEBUG
/* d (difference) = m (minuend) - s (subtrahend) */
int
bcm_oam_bhh_time_spec_subtract(bcm_time_spec_t* d, bcm_time_spec_t* m, bcm_time_spec_t* s)
{
    int rv = BCM_E_NONE;
    int32 d_ns = 0;
    int32 d_s = 0;

    if ((d != NULL) && (m != NULL) && (s != NULL)) {
        /* subtract the nanoseconds first, then borrow is necessary. */
        d_ns = m->nanoseconds - s->nanoseconds;
        if (d_ns < 0) {
            m->seconds = m->seconds - 1;
            d_ns = 1000000000 + d_ns;
        }
        if (d_ns < 0) {
            /* if still negative, then error */
            rv = BCM_E_INTERNAL;
            d_ns = abs(d_ns);
        }
        d->nanoseconds = d_ns;

        /* subtract the seconds next, check for negative. */
        d_s = m->seconds - s->seconds;
        if (d_s < 0) {
            d->isnegative = TRUE;
            d_s = abs(d_s);
        } else {
            d->isnegative = FALSE;
        }
        d->seconds = d_s;

    } else {
        rv = BCM_E_INTERNAL;
    }

    return rv;
}
#endif

/*
 * Function:
 *     bcm_kt2_oam_loss_get
 * Purpose:
 *     Loss Measurement get
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_loss_get(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_oam_control_t *oc;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_loss_get_t msg;
    _bcm_oam_hash_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(loss_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loss_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[loss_p->id];

    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

    sal_memset(&msg, 0, sizeof(msg));
    /* Send BHH Session Update message to uC */
    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_GET,
                      sess_id, 0,
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_GET_REPLY,
                      &reply_len);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_loss_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    if (reply_len != buffer_len) {
        rv =  BCM_E_INTERNAL;
    } else {
        /*
         * Convert kernel space flags to host space flags
         */
        if (msg.flags & BCM_BHH_LM_SINGLE_ENDED) {
            loss_p->flags |= BCM_OAM_LOSS_SINGLE_ENDED;
        }

        if (msg.flags & BCM_BHH_LM_TX_ENABLE) {
            loss_p->flags |= BCM_OAM_LOSS_TX_ENABLE;
        }

        if (msg.flags & BCM_BHH_LM_SLM) {
            loss_p->flags |= BCM_OAM_LOSS_SLM;
        }

        loss_p->period = msg.period;
        loss_p->loss_threshold = msg.loss_threshold;             
        loss_p->loss_nearend = msg.loss_nearend;               
        loss_p->loss_farend = msg.loss_farend;    
        loss_p->tx_nearend = msg.tx_nearend;            
        loss_p->rx_nearend = msg.rx_nearend;              
        loss_p->tx_farend = msg.tx_farend;               
        loss_p->rx_farend = msg.rx_farend;               
        loss_p->rx_oam_packets = msg.rx_oam_packets;          
        loss_p->tx_oam_packets = msg.tx_oam_packets;          
        loss_p->int_pri = msg.int_pri;
        loss_p->pkt_pri = msg.pkt_pri;         

        rv = BCM_E_NONE;
    }
    }
    else {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_loss_delete
 * Purpose:
 *    Loss Measurement Delete 
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_loss_delete(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_oam_control_t *oc;
    int rv = BCM_E_NONE;
    shr_bhh_msg_ctrl_loss_delete_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;
    _bcm_oam_hash_data_t *h_data_p;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(loss_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loss_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[loss_p->id];

    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

        msg.sess_id = sess_id;

        /* Pack control message data into DMA buffer */
        buffer     = oc->dma_buffer;
        buffer_ptr = shr_bhh_msg_ctrl_loss_delete_pack(buffer, &msg);
        buffer_len = buffer_ptr - buffer;

        /* Send BHH Session Update message to uC */
        rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_DELETE,
                      buffer_len, 0,
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_DELETE_REPLY,
                      &reply_len);

        if(BCM_SUCCESS(rv) && (reply_len != 0))
            rv =  BCM_E_INTERNAL;
    }
    else {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_delay_add
 * Purpose:
 *     Delay Measurement add     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_delay_add(int unit, bcm_oam_delay_t *delay_p)
{
    _bcm_oam_control_t *oc;
    _bcm_oam_hash_data_t *h_data_p;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_delay_add_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    uint32 flags = 0;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }


    BCM_OAM_BHH_VALIDATE_EP(delay_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(delay_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[delay_p->id];
    if (h_data_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

    /*
     * Convert host space flags to uKernel space flags
     */
    if (delay_p->flags & BCM_OAM_DELAY_ONE_WAY)
        flags |= BCM_BHH_DM_ONE_WAY;

    if (delay_p->flags & BCM_OAM_DELAY_TX_ENABLE)
        flags |= BCM_BHH_DM_TX_ENABLE;

    sal_memset(&msg, 0, sizeof(msg));
    msg.flags   = flags;
    msg.sess_id = sess_id;
    msg.int_pri = delay_p->int_pri;
    msg.pkt_pri = delay_p->pkt_pri;

    h_data_p->ts_format = delay_p->timestamp_format;

    if(delay_p->timestamp_format == bcmOAMTimestampFormatIEEE1588v1)
        msg.dm_format = BCM_BHH_DM_TYPE_PTP;
    else
        msg.dm_format = BCM_BHH_DM_TYPE_NTP;

    /*
     * Set period
     */
    msg.period  = _kt2_ccm_intervals[_bcm_kt2_oam_ccm_msecs_to_hw_encode(delay_p->period)];

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_delay_add_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                      MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_ADD,
                      buffer_len, 0,
                      MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_ADD_REPLY,
                      &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0)) 
        rv =  BCM_E_INTERNAL;
    }
   else {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_delay_get
 * Purpose:
 *     Delay Measurements get 
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_delay_get(int unit, bcm_oam_delay_t *delay_p)
{
    _bcm_oam_control_t *oc;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_delay_get_t msg;
    _bcm_oam_hash_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(delay_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(delay_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[delay_p->id];


    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

    sal_memset(&msg, 0, sizeof(msg));
    /* Send BHH Session Update message to uC */
    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                      MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_GET,
                      sess_id, 0,
                      MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_GET_REPLY,
                      &reply_len);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_delay_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;


    if (reply_len != buffer_len) {
        rv =  BCM_E_INTERNAL;
    } else {
        /*
         * Convert kernel space flags to host space flags
         */
        if (msg.flags & BCM_BHH_DM_ONE_WAY)
            delay_p->flags |= BCM_OAM_DELAY_ONE_WAY;

        if (msg.flags & BCM_BHH_DM_TX_ENABLE)
            delay_p->flags |= BCM_OAM_DELAY_TX_ENABLE;

        delay_p->period = msg.period;

        if(msg.dm_format == BCM_BHH_DM_TYPE_PTP)
            delay_p->timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
        else
            delay_p->timestamp_format = bcmOAMTimestampFormatNTP;

        rv = bcm_kt2_oam_bhh_convert_ep_to_time_spec(&(delay_p->delay), 
                                       msg.delay_seconds, msg.delay_nanoseconds);
        if(BCM_SUCCESS(rv)) {
            rv = bcm_kt2_oam_bhh_convert_ep_to_time_spec(&(delay_p->txf), 
                                       msg.txf_seconds, msg.txf_nanoseconds);
        }
        if(BCM_SUCCESS(rv)) {
            rv = bcm_kt2_oam_bhh_convert_ep_to_time_spec(&(delay_p->rxf), 
                                       msg.rxf_seconds, msg.rxf_nanoseconds);
        }
        if(BCM_SUCCESS(rv)) {
            rv = bcm_kt2_oam_bhh_convert_ep_to_time_spec(&(delay_p->txb), 
                                       msg.txb_seconds, msg.txb_nanoseconds);
        }
        if(BCM_SUCCESS(rv)) {
            rv = bcm_kt2_oam_bhh_convert_ep_to_time_spec(&(delay_p->rxb), 
                                       msg.rxb_seconds, msg.rxb_nanoseconds);
        }
        delay_p->rx_oam_packets = msg.rx_oam_packets;
        delay_p->tx_oam_packets = msg.tx_oam_packets;
        delay_p->int_pri = msg.int_pri;
        delay_p->pkt_pri = msg.pkt_pri;

        rv = BCM_E_NONE;
    }
    }
    else {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     bcm_kt2_oam_delay_delete
 * Purpose:
 *     Delay Measurement Delete 
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_kt2_oam_delay_delete(int unit, bcm_oam_delay_t *delay_p)
{
    _bcm_oam_control_t *oc;
    int rv = BCM_E_NONE;
    shr_bhh_msg_ctrl_delay_delete_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;
    _bcm_oam_hash_data_t *h_data_p;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(delay_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(delay_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM Error: Endpoint EP=%d %s.\n"),
                   delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[delay_p->id];

    /*
     * Only BHH is supported
     */
    if(BHH_EP_TYPE(h_data_p)) {

        msg.sess_id = sess_id;

        /* Pack control message data into DMA buffer */
        buffer     = oc->dma_buffer;
        buffer_ptr = shr_bhh_msg_ctrl_delay_delete_pack(buffer, &msg);
        buffer_len = buffer_ptr - buffer;

        /* Send BHH Session Update message to uC */
        rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                      MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_DELETE,
                      buffer_len, 0,
                      MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_DELETE_REPLY,
                      &reply_len);

        if(BCM_SUCCESS(rv) && (reply_len != 0))
            rv =  BCM_E_INTERNAL;
    }
    else {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

int _bcm_kt2_oam_pm_init(int unit)
{
    /* Profile data structure initialization */
    _bcm_oam_pm_profile_control_t *pmc = NULL;
    int rv = BCM_E_NONE;
    _bcm_oam_control_t *oc = NULL;

    rv = _bcm_kt2_oam_control_get(unit, &oc);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Oam control get failed "
                            "- %s.\n"), unit, bcm_errmsg(rv)));
        return BCM_E_INIT;
    }
   
    _BCM_OAM_LOCK(oc);
    
    oc->pm_bhh_lmdm_data_collection_mode = soc_property_get(unit,
            spn_BHH_DATA_COLLECTION_MODE, _BCM_OAM_PM_COLLECTION_MODE_NONE);

    /* Raw mode is not supported in KT2. Internally it will be taken as none. */
    if ( _BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)) {

        oc->pm_bhh_lmdm_data_collection_mode = _BCM_OAM_PM_COLLECTION_MODE_NONE;

        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Raw mode is not supported in KT2."
                            "Selected mode is none \n"), unit));

    }

    /* Allocate profiles only if one of them has processed stats 
     * collection enabled 
     */
    if ((_BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc))) {
        _BCM_OAM_ALLOC(pmc, _bcm_oam_pm_profile_control_t, 
                sizeof (_bcm_oam_pm_profile_control_t),
                "OAM PM profile control");
        if (!pmc) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_MEMORY;
        }

        kt2_pm_profile_control[unit] = pmc;
    }
    /*
     * Allocate DMA buffers and raw data buffers.
     *
     * 1) DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
 */
    
    /* DMA buffer */
    if (_BCM_KT2_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc)) {
        oc->pm_bhh_dma_buffer_len = sizeof(shr_oam_pm_msg_ctrl_t);
        oc->pm_bhh_dma_buffer = soc_cm_salloc(unit, oc->pm_bhh_dma_buffer_len,
                                              "PM DMA buffer");
        if (!oc->pm_bhh_dma_buffer) {
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_MEMORY);
        }
        sal_memset(oc->pm_bhh_dma_buffer, 0, oc->pm_bhh_dma_buffer_len);
    }

    _BCM_OAM_UNLOCK(oc);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_oam_pm_msg_send_receive
 * Purpose:
 *      Sends given PM control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_class     - (IN) PM message class.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 *
 *     PM Long Control message:
 *     - PM control messages that require send/receive of information
 *       that cannot fit in the uc_msg 'len' and 'data' fields need to
 *       use DMA operations to exchange information (long control message).
 *
 *     - PM convention for long control messages for
 *        'mos_msg_data_t' fields:
 *          'len'    size of the DMA buffer to send to uController
 *          'data'   physical DMA memory address to send or receive
 *
 *      DMA Operations:
 *      - DMA read/write operation is performed when a long BHH control
 *        message is involved.
 *
 *      - Messages that require DMA operation (long control message)
 *        is indicated by MOS_MSG_DMA_MSG().
 *
 *      - Callers must 'pack' and 'unpack' corresponding information
 *        into/from DMA buffer indicated by dma_buffer.
 */
STATIC int
_bcm_kt2_oam_pm_msg_send_receive(int unit, uint8 s_class, uint8 s_subclass,
                                    uint16 s_len, uint32 s_data,
                                    uint8 r_subclass, uint16 *r_len)
{
    int rv = BCM_E_NONE;
    _bcm_oam_control_t *oc;
    mos_msg_data_t send, reply;
    uint8 *dma_buffer = NULL;
    int dma_buffer_len = 0;
    uint32 uc_rv;
    int uc_num;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = s_class;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */

    dma_buffer = oc->pm_bhh_dma_buffer;
    dma_buffer_len = oc->pm_bhh_dma_buffer_len;

    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    switch (s_class) {
        case MOS_MSG_CLASS_BHH:
            uc_num = oc->uc_num;
            break;
        
        default:
            return BCM_E_PARAM;
            break;
    }

    /* coverity[unreachable] */
    rv = soc_cmic_uc_msg_send_receive(unit, uc_num,
                                      &send, &reply,
                                      _PM_UC_MSG_TIMEOUT_USECS);

    /* Check reply class, subclass */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != s_class) || /* We can expect the same class that was sent */
        (reply.s.subclass != r_subclass)) {
        return (BCM_E_INTERNAL);
    }

    /* Convert OAM PM uController error code to BCM */
    uc_rv = bcm_ntohl(reply.s.data);
    switch(uc_rv) {
    case SHR_OAM_PM_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_OAM_PM_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_OAM_PM_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_OAM_PM_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_OAM_PM_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_OAM_PM_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_OAM_PM_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_OAM_PM_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_OAM_PM_UC_E_VERSION:
        rv = BCM_E_CONFIG;
        break;
    case SHR_OAM_PM_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }
        
    *r_len = bcm_ntohs(reply.s.len);

    return (rv);
}

int _bcm_kt2_pm_profile_compare(bcm_oam_pm_profile_info_t *profile_info1, 
                                bcm_oam_pm_profile_info_t *profile_info2)
{
    if (!(sal_memcmp(profile_info1->bin_edges, profile_info2->bin_edges,
                     BCM_OAM_MAX_PM_PROFILE_BIN_EDGES * sizeof(uint32)))) {
        /* Match */
        return 1;
    }
    return 0;
}

int _bcm_kt2_pm_profile_exists(int unit, bcm_oam_pm_profile_info_t *profile_info)
{
    int id = 0;
    _bcm_oam_pm_profile_control_t *pmc = NULL; 

    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        return 0; 
    }

    for (id = 0; id < _BCM_OAM_MAX_PM_PROFILES; id++) {
        if (_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, id)) { /* If a profile is in use */
            if (_bcm_kt2_pm_profile_compare(
                                         _BCM_KT2_PM_CTRL_PROFILE_PTR(pmc, id), 
                                            profile_info)) {
                return 1;
            }
        }
    }
    return 0;
}

int 
_bcm_kt2_find_free_pm_profile_id(int unit, int *pm_profile_id)
{
    int id = 0;
    _bcm_oam_pm_profile_control_t *pmc = NULL; 

    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        return BCM_E_INIT; 
    }

    /* return the lowest un used id */
    for (id = 0; id < _BCM_OAM_MAX_PM_PROFILES; id++) {
        if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, id))) { /* If a profile is not in use */
            *pm_profile_id = id; /* Assign it and return */
            return BCM_E_NONE;
        }
    }
    return BCM_E_RESOURCE;
}

int
_bcm_kt2_oam_pm_profile_delete(int unit, bcm_oam_pm_profile_t profile_id, int all) 
{
    _bcm_oam_pm_profile_control_t *pmc; 

    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        return BCM_E_INIT; 
    }

    /* Check it only if it is a delete for a particular profile */
    if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, profile_id)) && !all) {
        /* It is not in use. Return error */
        return BCM_E_NOT_FOUND;
    }
    /* Zero out that profile */
    sal_memset(_BCM_KT2_PM_CTRL_PROFILE_PTR(pmc, profile_id), 0, 
                sizeof(bcm_oam_pm_profile_info_t));
    /* Mark it un used */
    _BCM_KT2_SET_PM_CTRL_PROFILE_NOT_IN_USE(pmc, profile_id);

    return BCM_E_NONE;
}

int
bcm_kt2_oam_pm_profile_delete(int unit, bcm_oam_pm_profile_t profile_id)
{
    _bcm_oam_control_t *oc = NULL;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc); 

    if (!(_BCM_KT2_PM_PROFILE_ID_VALID(profile_id))) {
        
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }
    
    rv = _bcm_kt2_oam_pm_profile_delete(unit, profile_id, 0);
    
    _BCM_OAM_UNLOCK(oc);
    return rv;
}

int
bcm_kt2_oam_pm_profile_delete_all(int unit)
{
    int id;
    _bcm_oam_control_t *oc = NULL;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);    

    for (id = 0; id < _BCM_OAM_MAX_PM_PROFILES; id++) {
        rv = _bcm_kt2_oam_pm_profile_delete(unit, id, 1);

        if(rv != BCM_E_NONE) {
            _BCM_OAM_UNLOCK(oc);
        }
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

int 
bcm_kt2_oam_pm_profile_create(int unit, bcm_oam_pm_profile_info_t *profile_info)
{
    _bcm_oam_pm_profile_control_t *pmc = NULL;
    bcm_oam_pm_profile_info_t     *profile_ptr = NULL;
    int pm_profile_id = 0;
    _bcm_oam_control_t *oc = NULL;
    int rv=BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);
    
    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT; 
    }

    if (_BCM_KT2_PM_PROFILE_WITH_ID_FLAG_SET(profile_info)) {
        if (!(_BCM_KT2_PM_PROFILE_ID_VALID(profile_info->id))) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
        }
    }

    if (_BCM_KT2_PM_PROFILE_REPLACE_FLAG_SET(profile_info)) {
        if (!(_BCM_KT2_PM_PROFILE_WITH_ID_FLAG_SET(profile_info))) {
            /* replace needs the id to be present */
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
        }

        if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, profile_info->id))) {
            /* Trying to replace a non-existent PM profile */
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NOT_FOUND;
        }

        /* Delete the previous profile. */
        rv = _bcm_kt2_oam_pm_profile_delete(unit,
                                             profile_info->id, 0);

        if(rv != BCM_E_NONE) {
            _BCM_OAM_UNLOCK(oc);
            return rv;
        }
    }

    if (_BCM_KT2_PM_PROFILE_WITH_ID_FLAG_SET(profile_info)) {
        if (_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, profile_info->id)) {
            /* That profile id is in use. */
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_EXISTS;
        }
        /* Else use that as the id */
        pm_profile_id = profile_info->id;
    } else { 
        rv = _bcm_kt2_find_free_pm_profile_id(unit, 
                                                    &pm_profile_id);
        if(rv != BCM_E_NONE) {
            _BCM_OAM_UNLOCK(oc);
            return rv;
        }    
    }

    /* Check if such a profile already exists. */
    if (_bcm_kt2_pm_profile_exists(unit, profile_info)) {
        
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_EXISTS;
    }

    /* Copy it to local profile information */ 
    profile_ptr = _BCM_KT2_PM_CTRL_PROFILE_PTR(pmc, pm_profile_id);
    sal_memcpy(profile_ptr, profile_info, 
                sizeof(bcm_oam_pm_profile_info_t));
    
    /* Update the id in the profile */
    profile_ptr->id = profile_info->id =  pm_profile_id;

    /* Current flags have only transient meaning, so clear them */
    profile_ptr->flags = 0;

    /* Mark that profile used */
    _BCM_KT2_SET_PM_CTRL_PROFILE_IN_USE(pmc, pm_profile_id);
    
    _BCM_OAM_UNLOCK(oc);
    
    return BCM_E_NONE;
}

int
bcm_kt2_oam_pm_profile_get(int unit, bcm_oam_pm_profile_info_t *profile_info)
{
    _bcm_oam_pm_profile_control_t *pmc;
    int profile_id = profile_info->id; 

    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        return BCM_E_INIT; 
    }

    if (!(_BCM_KT2_PM_PROFILE_ID_VALID(profile_id))) {
        return BCM_E_PARAM;
    }

    if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
        /* It is not in use. Return error */
        return BCM_E_NOT_FOUND;
    }

    /* Copy the profile to the profile structure passed */
    sal_memcpy(profile_info, _BCM_KT2_PM_CTRL_PROFILE_PTR(pmc, profile_id),
               sizeof(bcm_oam_pm_profile_info_t));

    return BCM_E_NONE;  
}

int bcm_kt2_oam_pm_profile_attach(int unit, bcm_oam_endpoint_t endpoint_id,
                              bcm_oam_pm_profile_t profile_id)
{
    _bcm_oam_pm_profile_control_t *pmc = NULL;
    bcm_oam_pm_profile_info_t *profile_info = NULL;
    _bcm_oam_control_t *oc = NULL;
    _bcm_oam_hash_data_t *h_data_p = NULL;
    int rv = BCM_E_NONE;
    shr_oam_pm_msg_ctrl_profile_attach_t msg_profile_attach;
    uint8                    msg_class = 0;
    uint16                   reply_len = 0;
    uint8                    msg_subclass = 0;
    uint8                    msg_reply_subclass = 0;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint_id);
    _BCM_OAM_LOCK(oc);

    h_data_p =  &oc->oam_hash_data[endpoint_id];

    if (!BHH_EP_TYPE(h_data_p)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: EP=%d is not BHH endpoint\n"),
                 unit, endpoint_id));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_UNAVAIL);        
    }    
    
    if (0 == h_data_p->in_use) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                 unit, endpoint_id));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_NOT_FOUND);
    }
    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT; 
    }

    if (!(_BCM_KT2_PM_PROFILE_ID_VALID(profile_id))) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    /* Check profile id exists or not */
    if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
        /* It is not in use. Return error */
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }
    /* Check if a profile id is already attached to this endpoint */
    if (h_data_p->pm_profile_attached != _BCM_OAM_INVALID_INDEX) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_EXISTS;
    }

    /* Check if delay measurement flag is present for the endpoint */
    if (!(h_data_p->flags & BCM_OAM_ENDPOINT_DELAY_MEASUREMENT)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    profile_info = _BCM_KT2_PM_CTRL_PROFILE_PTR(pmc, profile_id);

    /* Send info to u-Kernel about the profile attached with the endpoint */
    sal_memset(&msg_profile_attach, 0, sizeof(msg_profile_attach));

    msg_class = MOS_MSG_CLASS_BHH;
    msg_subclass = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_ATTACH;
    msg_reply_subclass = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_ATTACH_REPLY;
    msg_profile_attach.sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
   
    msg_profile_attach.profile_id = profile_id;
    msg_profile_attach.profile_flags = profile_info->flags;
    sal_memcpy(&(msg_profile_attach.profile_edges), &(profile_info->bin_edges), 
               (sizeof(uint32) * SHR_OAM_PM_MAX_PM_BIN_EDGES));

    /* Pack control message data into DMA buffer */
    buffer = oc->pm_bhh_dma_buffer;
    buffer_ptr = shr_oam_pm_msg_ctrl_profile_attach_pack(buffer, &msg_profile_attach);
    buffer_len = buffer_ptr - buffer;


    rv = _bcm_kt2_oam_pm_msg_send_receive 
        (unit, msg_class, 
         msg_subclass,
         buffer_len, 0, 
         msg_reply_subclass,
         &reply_len);

    if(BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }

    /* Attach it to the endpoint */
    h_data_p->pm_profile_attached = profile_id;

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

int bcm_kt2_oam_pm_profile_detach(int unit, bcm_oam_endpoint_t endpoint_id, 
                                  bcm_oam_pm_profile_t profile_id)
{
    _bcm_oam_pm_profile_control_t *pmc = NULL;
    _bcm_oam_control_t *oc = NULL;
    _bcm_oam_hash_data_t *h_data_p = NULL;
    int rv = BCM_E_NONE;
    uint8                    msg_class = 0;
    uint16                   reply_len = 0;
    uint8                    msg_subclass = 0;
    uint8                    msg_reply_subclass = 0;
    uint16                   sess_id = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint_id);
    _BCM_OAM_LOCK(oc);

    h_data_p =  &oc->oam_hash_data[endpoint_id];

    if (!BHH_EP_TYPE(h_data_p)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: EP=%d is not BHH endpoint\n"),
                 unit, endpoint_id));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_UNAVAIL);        
    }    

    if (0 == h_data_p->in_use) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                 unit, endpoint_id));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_NOT_FOUND);
    }
    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT; 
    }

    if (!(_BCM_KT2_PM_PROFILE_ID_VALID(profile_id))) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    /* Check profile id exists or not */
    if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
        /* It is not in use. Return error */
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }

    /* Check if the passed profile id is the one that is attached */
    if (profile_id != h_data_p->pm_profile_attached) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    msg_class = MOS_MSG_CLASS_BHH;
    msg_subclass = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_DETACH;
    msg_reply_subclass = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_DETACH_REPLY;
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
    
    rv = _bcm_kt2_oam_pm_msg_send_receive 
        (unit, msg_class, 
         msg_subclass,
         sess_id, profile_id, 
         msg_reply_subclass,
         &reply_len);

    if(BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }
    /* Detach the profile from endpoint */
    h_data_p->pm_profile_attached = _BCM_OAM_INVALID_INDEX;

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}


int bcm_kt2_oam_pm_profile_traverse(int unit, 
                    bcm_oam_pm_profile_traverse_cb cb, void *user_data)
{
    int                     rv = BCM_E_NONE;  /* Operation return status.          */
    bcm_oam_pm_profile_info_t *profile_info = NULL;
    _bcm_oam_pm_profile_control_t *pmc = NULL;
    int profile_id;

    /* Validate input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }
    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        return BCM_E_INIT; 
    }

    for (profile_id = 0; profile_id < _BCM_OAM_MAX_PM_PROFILES; profile_id++) {
        /* Check profile id is in use or not */
        if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
            /* Skip that profile id */
            continue;
        }
        profile_info = _BCM_KT2_PM_CTRL_PROFILE_PTR(pmc, profile_id);
        rv = cb(unit, profile_info, user_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: EP=%d callback failed - %s.\n"),
                       unit, profile_id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    return (rv);
}

void _bcm_kt2_oam_copy_stats_msg_to_stats_ptr(shr_oam_pm_msg_ctrl_pm_stats_get_t *stats_msg,
                                             bcm_oam_pm_stats_t *stats_ptr,
                                             uint8 pm_stat_extra_elem_feature)
{
    stats_ptr->far_loss_min = stats_msg->far_loss_min;
    stats_ptr->far_tx_min = stats_msg->far_tx_min;   
    stats_ptr->far_loss_max = stats_msg->far_loss_max; 
    stats_ptr->far_tx_max = stats_msg->far_tx_max;
    stats_ptr->far_loss = stats_msg->far_loss;
    stats_ptr->near_loss_min = stats_msg->near_loss_min;
    stats_ptr->near_tx_min = stats_msg->near_tx_min;  
    stats_ptr->near_loss_max = stats_msg->near_loss_max;
    stats_ptr->near_tx_max = stats_msg->near_tx_max;  
    stats_ptr->near_loss = stats_msg->near_loss;    
    stats_ptr->lm_tx_count = stats_msg->lm_tx_count;  
    stats_ptr->DM_min = stats_msg->DM_min;       
    stats_ptr->DM_max = stats_msg->DM_max;
    if(stats_msg->dm_rx_count) {
        stats_ptr->DM_avg = stats_msg->DM_avg/stats_msg->dm_rx_count;
    }
    stats_ptr->dm_tx_count = stats_msg->dm_tx_count;  
    stats_ptr->profile_id = stats_msg->profile_id;
    sal_memcpy(&(stats_ptr->bin_counters), &(stats_msg->bin_counters), sizeof(uint32) * (SHR_OAM_PM_MAX_PM_BIN_EDGES + 1));
    if(pm_stat_extra_elem_feature) {
        stats_ptr->lm_rx_count = stats_msg->lm_rx_count;
        stats_ptr->dm_rx_count = stats_msg->dm_rx_count;
        stats_ptr->far_total_tx_pkt_count = stats_msg->far_total_tx_pkt_count;
        stats_ptr->near_total_tx_pkt_count = stats_msg->near_total_tx_pkt_count;
    }

    if (stats_msg->flags & SHR_OAM_PM_STATS_FLAG_COUNTER_ROLLOVER) {
        stats_ptr->flags |= BCM_OAM_PM_STATS_PROCESSED_COUNTER_ROLLOVER;
    }
}

int bcm_kt2_oam_pm_stats_get(int unit,
                            bcm_oam_endpoint_t endpoint_id, bcm_oam_pm_stats_t *stats_ptr)
{
    _bcm_oam_pm_profile_control_t *pmc = NULL;
    _bcm_oam_control_t *oc = NULL;
    _bcm_oam_hash_data_t *h_data_p = NULL;
    shr_oam_pm_msg_ctrl_pm_stats_get_t stats_msg;
    int rv = BCM_E_NONE;
    uint16 sess_id = 0;
    uint8                    msg_class = 0;
    uint16                   reply_len = 0;
    uint8                    msg_subclass = 0;
    uint8                    msg_reply_subclass = 0;
    uint8                   *dma_buffer = NULL;
    uint8                   pm_stat_extra_elem_feature = 0;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint_id);
    _BCM_OAM_LOCK(oc);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->mep_pool, endpoint_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, endpoint_id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p =  &oc->oam_hash_data[endpoint_id];

    if (!BHH_EP_TYPE(h_data_p)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: EP=%d is not BHH endpoint\n"),
                 unit, endpoint_id));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_UNAVAIL);        
    }    

    if (0 == h_data_p->in_use) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                 unit, endpoint_id));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_INTERNAL);
    }

    pmc = kt2_pm_profile_control[unit];

    if (!pmc) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT; 
    }


    msg_class = MOS_MSG_CLASS_BHH;
    msg_subclass = MOS_MSG_SUBCLASS_BHH_PM_STATS_GET;
    msg_reply_subclass = MOS_MSG_SUBCLASS_BHH_PM_STATS_GET_REPLY;
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
    pm_stat_extra_elem_feature = BHH_UC_FEATURE_CHECK(BHH_PM_STAT_EXTRA_ELEM);

    dma_buffer = oc->pm_bhh_dma_buffer;
    rv = _bcm_kt2_oam_pm_msg_send_receive 
        (unit, msg_class, 
         msg_subclass,
         /* Pass the buffer pointer in data and sess id in length fields.*/
         sess_id, soc_cm_l2p(unit, dma_buffer),
         msg_reply_subclass,
         &reply_len);

    if(BCM_FAILURE(rv) || reply_len == 0) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }

    shr_oam_pm_msg_ctrl_pm_stats_get_unpack(dma_buffer, &stats_msg, pm_stat_extra_elem_feature);

    _bcm_kt2_oam_copy_stats_msg_to_stats_ptr(&stats_msg, stats_ptr, pm_stat_extra_elem_feature);

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}


int
bcm_kt2_oam_pm_profile_attach_get(
        int unit,
        bcm_oam_endpoint_t endpoint_id,
        bcm_oam_pm_profile_t *profile_id)
{
    _bcm_oam_pm_profile_control_t *pmc = NULL;
    _bcm_oam_control_t *oc = NULL;
    _bcm_oam_hash_data_t *h_data_p = NULL;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint_id);
    _BCM_OAM_LOCK(oc);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->mep_pool, endpoint_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, endpoint_id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p =  &oc->oam_hash_data[endpoint_id];

    if (0 == h_data_p->in_use) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                 unit, endpoint_id));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_INTERNAL);
    }
    pmc = kt2_pm_profile_control[unit];
    
    if (!pmc) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT; 
    }

    /* Get the attached profile */
    *profile_id = h_data_p->pm_profile_attached;

    if (*profile_id == _BCM_OAM_INVALID_INDEX) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    } 
    /* Check profile id is in use or not */
    if (!(_BCM_KT2_PM_CTRL_PROFILE_IN_USE(pmc, *profile_id))) {
        /* It is not in use. Return error */
        /* Ideally this should not happen */
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_oam_csf_add
 * Purpose:
 *      Start CSF PDU transmission
 * Parameters:
 *      unit    - (IN)    Unit number.
 *      csf_ptr - (INOUT) CSF object
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_kt2_oam_csf_add(int unit, bcm_oam_csf_t *csf_p)
{
    _bcm_oam_control_t         *oc;
    _bcm_oam_hash_data_t       *h_data_p;
    bhh_sdk_msg_ctrl_csf_add_t  msg;
    uint8                      *buffer, *buffer_ptr;
    uint16                      buffer_len, reply_len;
    int                         sess_id = 0;
    int                         rv = BCM_E_NONE;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(csf_p->id);
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(csf_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    if ((csf_p->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1S) &&
        (csf_p->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1M)) {
        /* Only 1S and 1M are supported */
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    h_data_p = &oc->oam_hash_data[csf_p->id];
    /*
     * Only BHH is supported
     */
    if (!BHH_EP_TYPE(h_data_p)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.sess_id = sess_id;
    msg.int_pri = csf_p->int_pri;
    msg.pkt_pri = csf_p->pkt_pri;

    switch(csf_p->type) {
        case BCM_OAM_CSF_LOS:
            msg.type = SHR_CSF_TYPE_LOS;
            break;

        case BCM_OAM_CSF_FDI:
            msg.type = SHR_CSF_TYPE_FDI;
            break;

        case BCM_OAM_CSF_RDI:
            msg.type = SHR_CSF_TYPE_RDI;
            break;

        case BCM_OAM_CSF_DCI:
            msg.type = SHR_CSF_TYPE_DCI;
            break;

        default:
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
    }

    /*
     * Set period
     */
    msg.period  = csf_p->period;

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_csf_add_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                                           MOS_MSG_SUBCLASS_BHH_CSF_ADD,
                                           buffer_len, 0,
                                           MOS_MSG_SUBCLASS_BHH_CSF_ADD_REPLY,
                                           &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }

    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *      bcm_kt2_oam_csf_get
 * Purpose:
 *      Get CSF info
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      csf_ptr - (OUT) CSF object
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_kt2_oam_csf_get(int unit, bcm_oam_csf_t *csf_p)
{
    _bcm_oam_control_t          *oc;
    bhh_sdk_msg_ctrl_csf_get_t   msg;
    _bcm_oam_hash_data_t        *h_data_p;
    uint8                       *buffer, *buffer_ptr;
    uint16                       buffer_len, reply_len;
    int                          sess_id = 0;
    int                          rv = BCM_E_NONE;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(csf_p->id);
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(csf_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[csf_p->id];

    if (!BHH_EP_TYPE(h_data_p)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }


    sal_memset(&msg, 0, sizeof(msg));

    /* Send BHH Session Update message to uC */
    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                                           MOS_MSG_SUBCLASS_BHH_CSF_GET,
                                           sess_id, 0,
                                           MOS_MSG_SUBCLASS_BHH_CSF_GET_REPLY,
                                           &reply_len);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_csf_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;


    if (reply_len != buffer_len) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }

    switch(msg.type) {
        case SHR_CSF_TYPE_LOS:
            csf_p->type = BCM_OAM_CSF_LOS;
            break;

        case SHR_CSF_TYPE_FDI:
            csf_p->type = BCM_OAM_CSF_FDI;
            break;

        case SHR_CSF_TYPE_RDI:
            csf_p->type = BCM_OAM_CSF_RDI;
            break;

        case SHR_CSF_TYPE_DCI:
            csf_p->type = BCM_OAM_CSF_DCI;
            break;

        default:
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_INTERNAL;
    }

    csf_p->period  = msg.period;
    csf_p->flags   = msg.flags;
    csf_p->pkt_pri = msg.pkt_pri;
    csf_p->int_pri = msg.int_pri;


    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_kt2_oam_csf_delete
 * Purpose:
 *      Stop CSF PDU transmission
 * Parameters:
 *      unit    - (IN  Unit number.
 *      csf_ptr - (IN) CSF object
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_kt2_oam_csf_delete(int unit, bcm_oam_csf_t *csf_p)
{
    _bcm_oam_control_t   *oc;
    _bcm_oam_hash_data_t *h_data_p;
    uint16                reply_len;
    int                   sess_id;
    int                   rv = BCM_E_NONE;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(csf_p->id);
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(csf_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->oam_hash_data[csf_p->id];

    if (!BHH_EP_TYPE(h_data_p)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    /* Send BHH Session Update message to uC */
    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
                                           MOS_MSG_SUBCLASS_BHH_CSF_DELETE,
                                           sess_id, 0,
                                           MOS_MSG_SUBCLASS_BHH_CSF_DELETE_REPLY,
                                           &reply_len);
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}
#endif /* defined(INCLUDE_BHH) */

/*
 * Function:
 *      _bcm_kt2_bhh_endpoint_faults_multi_get
 * Purpose:
 *      Function to get faults for multiple BHH endpoints
 *      in a single call.
 * Parameters:
 *      unit                (IN) BCM device number
 *      flags               (IN) Flags is kept unused as of now.
 *                                This is for any future enhancement
 *      max_endpoints       (IN) Number of max endpoint for the protocol
 *      faults              (OUT) Pointer to faults for all endpoints
 *      endpoint_count      (OUT) Pointer to Number of valid endpoints with faults,
 *                                filled by get API.
 *
 * Returns:
 *      BCM_E_NONE          No error
 *      BCM_E_XXXX          Error
 */

#if defined (INCLUDE_BHH)
int _bcm_kt2_bhh_endpoint_faults_multi_get(
        int unit,
        uint32 flags,
        uint32 max_endpoints,
        bcm_oam_endpoint_fault_t *faults,
        uint32 *endpoint_count)
{
    int rv = BCM_E_NONE;
    _bcm_oam_control_t *oc = NULL;
    bcm_oam_endpoint_fault_t *faults_temp = faults;
    uint8 *buffer;
    uint16 reply_len;
    uint32 sess_count;
    uint32 ep_id;
    uint32 ep_count = 0;
    uint32 faults_bitmap;
    _bcm_oam_hash_data_t *h_data_p;      /* Pointer to endpoint hash data.    */

    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    if(max_endpoints != oc->bhh_endpoint_count) {
        rv = BCM_E_PARAM;
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    sal_memset(faults, 0, ((sizeof(bcm_oam_endpoint_fault_t))*max_endpoints));

    rv = _bcm_kt2_oam_bhh_msg_send_receive(unit,
            MOS_MSG_SUBCLASS_BHH_FAULTS_MULTI_GET,
            0, 0,
            MOS_MSG_SUBCLASS_BHH_FAULTS_MULTI_GET_REPLY,
            &reply_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: BHH uKernel msg failed for"
                            "faults multi get %s.\n"), unit,
                 bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    if(reply_len != (oc->bhh_endpoint_count * sizeof(uint32))) {
        rv = BCM_E_INTERNAL;
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
    /* Unpack control message data into DMA buffer */
    buffer = oc->dma_buffer;
    for(sess_count = 0;sess_count < oc->bhh_endpoint_count;sess_count++)
    {
        ep_id = BCM_OAM_BHH_GET_SDK_EP(sess_count);

        /* Validate endpoint index value. */
        BCM_OAM_BHH_VALIDATE_EP(ep_id);

        h_data_p =  &oc->oam_hash_data[ep_id];

        /* If endpoint not in use, ignore and continue for next session_id).
         * If session is running in hostCPU, skip and Go to ep_id.*/
        if(!(h_data_p->in_use) || (h_data_p->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REDIRECT_TO_CPU)) {
           continue;
        }

         _SHR_UNPACK_U32(buffer, faults_bitmap);

        faults_temp->endpoint_id = BCM_OAM_BHH_GET_SDK_EP(sess_count);

        if(faults_bitmap & BHH_BTE_EVENT_CCM_TIMEOUT){
            faults_temp->faults |= BCM_OAM_BHH_FAULT_CCM_TIMEOUT;
        }
        if(faults_bitmap & BHH_BTE_EVENT_CCM_RDI){
            faults_temp->faults |= BCM_OAM_BHH_FAULT_CCM_RDI;
        }
        if(faults_bitmap & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL){
            faults_temp->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL;
        }
        if(faults_bitmap & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID){
            faults_temp->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID;
        }
         if(faults_bitmap & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID){
            faults_temp->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID;
        }
        if(faults_bitmap & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD){
            faults_temp->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD;
        }
        if(faults_bitmap & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY){
            faults_temp->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY;
        }

        ep_count++;
        faults_temp++;

    }

    *endpoint_count = ep_count;
    _BCM_OAM_UNLOCK(oc);
    return (rv);

}
#endif

/* Function:
 *      bcm_kt2_oam_endpoint_faults_multi_get
 * Purpose:
 *      Function to get faults for multiple endpoints per protocl
 *      in a single call.
 * Parameters:
 *      unit                (IN) BCM device number
 *      flags               (IN) Flags is kept unused as of now.
 *                               This is for any future enhancement
 *      endpoint_protocol   (IN) Protocol type of the endpoints to retrieve faults
 *      max_endpoints       (IN) Number of max endpoint for the protocol
 *      faults              (OUT) Pointer to faults for all endpoints
 *      endpoint_count      (OUT) Pointer to Number of valid endpoints with faults,
 *                                by get API.
 *
 * Returns:
 *      BCM_E_NONE          No error
 *      BCM_E_XXXX          Error
 */
int bcm_kt2_oam_endpoint_faults_multi_get(
        int unit,
        uint32 flags,
        bcm_oam_protocol_type_t endpoint_protocol,
        uint32 max_endpoints,
        bcm_oam_endpoint_fault_t *faults,
        uint32 *endpoint_count)

{
    int rv = BCM_E_NONE;

    if(!faults) {
        rv = BCM_E_PARAM;
        return (rv);
    }

    switch(endpoint_protocol) {

#if defined(INCLUDE_BHH)
        case bcmOamProtocolBhh:
            *endpoint_count = 0;
            rv = _bcm_kt2_bhh_endpoint_faults_multi_get(
                    unit,
                    flags,
                    max_endpoints,
                    faults,
                    endpoint_count);
        break;
#endif
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }

    return rv;

}

/* This function assumes passed endpoint is a PORT MEP configured over trunk. */
int _bcm_kt2_oam_ep_trunk_ports_add_del_internal(int unit, _bcm_oam_hash_data_t *h_data_p, 
                                    int max_ports, bcm_gport_t *port_arr, uint8 add)
{
    _bcm_oam_hash_data_t old_hash_data;
    bcm_oam_endpoint_info_t ep_info;
    uint32 sglp = 0, dglp = 0;
    bcm_port_t src_pp_port = 0, dst_pp_port = 0, pp_port = 0;
    uint32 svp = 0;
    int vp_valid = 0;
    bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
    bcm_gport_t tx_gport;
    int i, module_id = 0;
    uint8 active_mdl = 0;
    

    bcm_oam_endpoint_info_t_init(&ep_info);
    /* **********************************************
     *               SW configurations
 */
    /* Keep a copy of old hash data for comparisons.
     */
    sal_memcpy(&old_hash_data, h_data_p, sizeof(_bcm_oam_hash_data_t));


    /* Call endpoint get to get the endpoint info */
    BCM_IF_ERROR_RETURN(bcm_kt2_oam_endpoint_get(unit, h_data_p->ep_id, &ep_info));
    /* First call endpoint gport resolve to correctly update new
     * sglp, dglp, src_pp_port, dst_pp_port variables.
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_endpoint_gport_resolve(unit, &ep_info, &sglp, &dglp,
                                                            &src_pp_port, &dst_pp_port, &svp,
                                                            &trunk_id, &vp_valid, &tx_gport));
                                                            
    /* Update the h_data_p. */
    h_data_p->sglp                 = sglp;
    h_data_p->dglp                 = dglp;
    h_data_p->src_pp_port          = src_pp_port;
    h_data_p->dst_pp_port          = dst_pp_port;
    h_data_p->resolved_trunk_gport = tx_gport;

    /* ************************************************
     *               HW configurations
 */

    for (i = 0; i < max_ports; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_pp_port_to_modport_get(unit, port_arr[i],
                                             &module_id, &pp_port));
        if (add) {
            /* _bcm_kt2_oam_trunk_port_mdl_config needs to be called so that
             * new trunk members are programmed with MDL_BITMAP correctly.
             */
            BCM_IF_ERROR_RETURN(_bcm_kt2_oam_stm_table_update(unit, module_id, pp_port, h_data_p));
            BCM_IF_ERROR_RETURN(_bcm_kt2_oam_port_mdl_update(unit, pp_port, 0, h_data_p, &active_mdl));
        } else {
            /* Delete the old ports' MDL BITMAP using _bcm_kt2_oam_port_mdl_update. And
             * clear STM table if this is the last endpoint on this port.
             */
            BCM_IF_ERROR_RETURN(_bcm_kt2_oam_port_mdl_update(unit, pp_port, 1, h_data_p, &active_mdl));
            if (active_mdl == 0) {
                /* Setting index = -1 so that rx indexes are not freed up, since there are
                 * other ports on the trunk on which this endpoint is placed.
                 */
                BCM_IF_ERROR_RETURN(_bcm_kt2_oam_stm_table_clear(unit, module_id, -1,
                            pp_port, 
                            h_data_p));
            }
        }
    }

    if ((h_data_p->dst_pp_port != old_hash_data.dst_pp_port)
        || (h_data_p->src_pp_port != old_hash_data.src_pp_port)) {
        /* If src_pp_port or dst_pp_port has changed. delete the old EGR_PORT
         * MDL_PASSIVE BITMAP and program the new port's EGR_PORT MDL_PASSIVE BITMAP.
         */
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_port_mdl_passive_update(unit,1, &old_hash_data, 0));
        BCM_IF_ERROR_RETURN(_bcm_kt2_oam_port_mdl_passive_update(unit, 0 /* not reset */,
                                                                 h_data_p, active_mdl));
    }

    if (!(h_data_p->flags & BCM_OAM_ENDPOINT_UP_FACING)) {
        if ((h_data_p->dst_pp_port != old_hash_data.dst_pp_port)
            && (ep_info.ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)) {
            /* If dst_pp_port has changed, modify the LMEP table to reflect new
             * dst pp port and corresponding queue number.
             */
            BCM_IF_ERROR_RETURN(_bcm_kt2_oam_lmep_down_modify_new_dest_port(unit, h_data_p));
        }
    } else {
        if ((h_data_p->src_pp_port != old_hash_data.src_pp_port)
            && (ep_info.ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)) {
            /* If src_pp_port has changed, modify the LMEP table to reflect new
             * src pp port.
             */
            BCM_IF_ERROR_RETURN(_bcm_kt2_oam_lmep_up_modify_new_src_port(unit, h_data_p));
        }
    }

    return BCM_E_NONE;
}


int _bcm_kt2_oam_trunk_ports_add_del_internal(int unit, bcm_gport_t trunk_gport, 
                                int max_ports, bcm_gport_t *port_arr, uint8 add)
{
    int rv = BCM_E_NONE;
    bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
    _bcm_oam_control_t  *oc = NULL;
    _bcm_oam_hash_data_t *h_data_p = NULL;
    int i;


    /* Validate port array and max ports inputs */
    if (port_arr == NULL || max_ports == 0) {
        return BCM_E_PARAM;
    }

    /* Check the gport is TRUNK */ 
    if (!(BCM_GPORT_IS_TRUNK(trunk_gport))) {
        return BCM_E_PARAM;
    }

    /* Get Trunk ID value from Gport */
    trunk_id = BCM_GPORT_TRUNK_GET(trunk_gport);

    if (trunk_id == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    /* Get OAM control structure */
    BCM_IF_ERROR_RETURN(_bcm_kt2_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    for (i = 0; i < oc->ep_count; i++) {
        h_data_p = &oc->oam_hash_data[i];
        /* This API is supported only for PORT MEPs on Trunks */
        if ((ETH_TYPE(h_data_p->type)) && 
            (h_data_p->in_use) && 
            (h_data_p->oam_domain == _BCM_OAM_DOMAIN_PORT) &&
            (h_data_p->gport      == trunk_gport) &&
            (h_data_p->trunk_id == trunk_id)) {
            rv = _bcm_kt2_oam_ep_trunk_ports_add_del_internal(unit, h_data_p, max_ports, port_arr, add); 
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                return rv;
            }
        }
    }
    _BCM_OAM_UNLOCK(oc);
    return rv;
}

int bcm_kt2_oam_trunk_ports_add(int unit, bcm_gport_t trunk_gport, 
                                int max_ports, bcm_gport_t *port_arr)
{
    return _bcm_kt2_oam_trunk_ports_add_del_internal(unit, trunk_gport, max_ports, port_arr, 1);
}
int bcm_kt2_oam_trunk_ports_delete(int unit, bcm_gport_t trunk_gport, 
                                int max_ports, bcm_gport_t *port_arr)
{
    return _bcm_kt2_oam_trunk_ports_add_del_internal(unit, trunk_gport, max_ports, port_arr, 0);
}
#endif /* BCM_KATANA2_SUPPORT */
