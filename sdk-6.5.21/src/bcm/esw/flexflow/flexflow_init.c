/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/core/libc.h>
#if defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/flow.h>
#include <bcm_int/esw/switch.h>
#include <soc/esw/flow_db_core.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/trident3.h>

_bcm_flow_bookkeeping_t   *_bcm_flow_bk_info[BCM_MAX_NUM_UNITS] = { 0 };

#define VIRTUAL_INFO(_unit_)    (&_bcm_virtual_bk_info[_unit_])
#define L3_INFO(_unit_)         (&_bcm_l3_bk_info[_unit_])

/*
 * Function:
 *      _bcm_td3_flow_free_resource
 * Purpose:
 *      Free all allocated software resources 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */

STATIC void
_bcm_td3_flow_free_resource(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info = FLOW_INFO(unit);

    /* If software tables were not allocated we are done. */ 
    if (NULL == FLOW_INFO(unit)) {
        return;
    }

    /* Destroy EGR_DVP_ATTRIBUTE usage bitmap  */
    if (flow_info->dvp_attr_bitmap) {
        sal_free(flow_info->dvp_attr_bitmap);
        flow_info->dvp_attr_bitmap = NULL;
    }

    /* Destroy soft init tunnel table   */
    if (flow_info->init_tunnel) {
        sal_free(flow_info->init_tunnel);
        flow_info->init_tunnel = NULL;
    }

    /* Destroy iif_ref_cnt table   */
    if (flow_info->iif_ref_cnt) {
        sal_free(flow_info->iif_ref_cnt);
        flow_info->iif_ref_cnt = NULL;
    }

    /* Destroy vp_ref_cnt table   */
    if (flow_info->vp_ref_cnt) {
        sal_free(flow_info->vp_ref_cnt);
        flow_info->vp_ref_cnt = NULL;
    }

/*
    if (flow_info->flow_tunnel_term) {
        sal_free(flow_info->flow_tunnel_term);
        flow_info->flow_tunnel_term = NULL;
    }
*/

    if (flow_info->flow_vpn_vlan) {
        sal_free(flow_info->flow_vpn_vlan);
        flow_info->flow_vpn_vlan = NULL;
    }

    if (flow_info->match_key) {
        sal_free(flow_info->match_key);
        flow_info->match_key = NULL;
    }

    /* Free module data. */
    sal_free(FLOW_INFO(unit));
    FLOW_INFO(unit) = NULL;
}
/*
 * Function:
 *      bcm_td3_flow_allocate_bk
 * Purpose:
 *      Initialize FLOW software book-kepping
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcm_td3_flow_allocate_bk(int unit)
{
    /* Allocate/Init unit software tables. */
    if (NULL == FLOW_INFO(unit)) {
        BCM_TD3_FLOW_ALLOC(FLOW_INFO(unit), sizeof(_bcm_flow_bookkeeping_t),
                          "flow_bk_module_data");
        if (NULL == FLOW_INFO(unit)) {
            return (BCM_E_MEMORY);
        } else {
            FLOW_INFO(unit)->initialized = FALSE;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_flow_hw_clear
 * Purpose:
 *      Free all allocated hardware resources
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td3_flow_hw_clear(int unit)
{
    int rv = BCM_E_NONE, rv1 = BCM_E_NONE;

    rv = bcmi_esw_flow_tunnel_terminator_destroy_all(unit);
    if (BCM_FAILURE(rv)) {
        rv1 = rv;
    }

    rv = bcmi_esw_flow_tunnel_initiator_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv1)) {
        rv1 = rv;
    }

    rv = bcmi_esw_flow_vpn_destroy_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv1)) {
        rv1 = rv;
    }


    return rv1;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_2_0                SOC_SCACHE_VERSION(2, 0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_2_0

STATIC int _bcm_flow_wb_alloc(int unit);

/*
 * Function:
 *      _bcm_flow_wb_recover
 *
 * Purpose:
 *      Recover VXLAN module info for Level 2 Warm Boot from persisitent memory
 *
 * Warm Boot Version Map:
 *      see _bcm_esw_flow_sync definition
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_wb_recover(int unit)
{
    int sz = 0, rv = BCM_E_NONE;
    int num_dvp = 0;
    int num_iif;
    int num_vp;
    int stable_size;
    uint16 recovered_ver = 0;
    uint8 *flow_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_flow_bookkeeping_t *flow_info;
    int num_soft_tnl = 0;
    int i;
    int additional_scache_size = 0;

    flow_info = FLOW_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOW, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &flow_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return _bcm_flow_wb_alloc(unit);
    }

    if (flow_scache_ptr != NULL) {
        num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
        num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);
        sz = sizeof(uint16);  /* idx of _bcm_flow_init_tunnel_entry_t */

        /* recover init_tunnel state */
        for (i = 0; i < num_soft_tnl; i++) {
            sal_memcpy(&flow_info->init_tunnel[i].idx, flow_scache_ptr, sz);
            flow_scache_ptr += sz;
        }

        /* recover the dvp_attr_bitmap */
        sz = SHR_BITALLOCSIZE(num_dvp);
        sal_memcpy(flow_info->dvp_attr_bitmap,flow_scache_ptr,sz);
        flow_scache_ptr += sz;

        num_iif = soc_mem_index_count(unit, L3_IIFm);
        sz =  sizeof(uint16);  /* iif_ref_cnt */
        for (i = 0; i < num_iif; i++) {
            sal_memcpy(&flow_info->iif_ref_cnt[i], flow_scache_ptr, sz);
            flow_scache_ptr += sz;
        }

        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        sz =  sizeof(uint16);  /* vp_ref_cnt */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&flow_info->vp_ref_cnt[i], flow_scache_ptr, sz);
            flow_scache_ptr += sz;
        }

        /* recover the memory usage reference count table */
        sz = sizeof(uint32);                  
        for (i = 0; i < _BM_FLOW_MEM_USE_MAX; i++) {
            sal_memcpy(&flow_info->mem_use_cnt[i], flow_scache_ptr, sz);
            flow_scache_ptr += sz;
        }

        sz = sizeof(bcm_flow_match_vp_t);
        if (recovered_ver >= BCM_WB_VERSION_2_0) {
             for (i = 0; i < num_vp; i++) {
                sal_memcpy(&flow_info->match_key[i], flow_scache_ptr, sz);
                flow_scache_ptr += sz;
            }
        } else {
            additional_scache_size += num_vp * sz;
        }

        /* Reallocate additional scache size if current scache requirement
         * is more than the one recovered.
         */ 
        if (additional_scache_size > 0) {
            rv = soc_scache_realloc(unit,scache_handle,additional_scache_size);
            if(BCM_FAILURE(rv)) {
               return rv;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_wb_alloc
 *
 * Purpose:
 *      Alloc persisitent memory for Level 2 Warm Boot scache.
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_wb_alloc(int unit)
{
    int alloc_sz = 0, rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    int num_soft_tnl = 0, num_dvp = 0;
    uint8 *flow_scache_ptr = NULL;
    int stable_size;
    int num_iif;
    int num_vp;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* init tunnel entry table size */
    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
    num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);
    /* num_soft_tnl * idx of _bcm_flow_init_tunnel_entry_t */
    alloc_sz = num_soft_tnl * sizeof(uint16); 

    /* dvp_attr_bitmap size */
    alloc_sz += SHR_BITALLOCSIZE(num_dvp);

    /* iif_ref_cnt table size */
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    /* num_iif * iif_ref_cnt */
    alloc_sz += num_iif * sizeof(uint16);

    /* vp_ref_cnt table size */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    /* num_vp * vp_ref_cnt */
    alloc_sz += num_vp * sizeof(uint16);

    /* mem_use_cnt table size */
    alloc_sz += _BM_FLOW_MEM_USE_MAX * sizeof(uint32);

    /*match_key array size from BCM_WB_VERSION_2_0*/
     alloc_sz += num_vp * sizeof(bcm_flow_match_vp_t);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOW, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, (uint8**)&flow_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_next_hop_refcnt_restore
 * Purpose:
 *      Recover the next hop egress object reference count increased by 
 * bcm_flow_port_encap_set when attaching the object to the flow port.
 * This routine is only used in warmboot.
 * 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_next_hop_refcnt_restore(int unit)
{
    int num_vp;
    ing_dvp_table_entry_t dvp;
    int vp;
    int nh_index;
    int ecmp;
    int ecmp_index;
    int width;
    egr_l3_next_hop_entry_t egr_nh_entry;
    int entry_type = 0;

    /* restore nextHop reference count set by flow_port_encap_set */
    num_vp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
    for (vp = 1; vp < num_vp; vp++) {
        if (_BCM_FLOW_EGR_DVP_USED_GET(unit,vp)) {
            SOC_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY,
                                vp, &dvp));
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  NEXT_HOP_INDEXf);
            ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  ECMPf);
            ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
            if (ecmp) {
                width = BCM_XGS3_L3_MAX_ECMP_MODE(unit) ?
                      _BCM_SINGLE_WIDE : _BCM_DOUBLE_WIDE;

                /* increment ecmp group reference count. */
                BCM_XGS3_L3_ENT_REF_CNT_INC
                    (BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp), ecmp_index, width);

            } else if (nh_index) {
                BCM_XGS3_L3_ENT_REF_CNT_INC(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                      nh_index, _BCM_SINGLE_WIDE);

                /* restore tpid reference count set by flow_port_encap_set */
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                    MEM_BLOCK_ANY, nh_index, &egr_nh_entry));

                entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                            &egr_nh_entry, DATA_TYPEf);
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
                   (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW)) {
                    int rv;
                    /* new VxLAN decoupled mode - recover TPID ref count */
                    rv = _bcm_td3_flex_egrnh_tpid_recover(unit, &egr_nh_entry);
                    BCM_IF_ERROR_RETURN(rv);
                }
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_encap_traverse_cb_tpid_refcnt(int unit,
        bcm_flow_encap_config_t *info, uint32 num_of_fields,
        bcm_flow_logical_field_t *field,
        void *user_data)
{
    int rv;
    int tpid_inx;

    if (info->flags & (BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE |
                       BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE |
                       BCM_FLOW_ENCAP_FLAG_TPID_REPLACE |
                       BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) ) {
        /* tpid reference count */
        if (info->flags & _BCM_FLOW_ENCAP_FLAG_TPID_INDEX) {
            tpid_inx = info->tpid;
            (void)_bcm_fb2_outer_tpid_tab_ref_count_add(unit,tpid_inx,1);
        } else if (info->tpid) {
            rv = _bcm_fb2_outer_tpid_lkup(unit,info->tpid, &tpid_inx);
            if (BCM_SUCCESS(rv)) {
               (void)_bcm_fb2_outer_tpid_tab_ref_count_add(unit,tpid_inx,1);
            }
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *    _bcm_flow_egr_mac_da_restore
 * Purpose:
 *    WB recovery for egr mac da profile ref count
 * Parameters:
 *    unit           - (IN) Device Number
 *    ecmp_nh_index - (IN) ecmp group number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_flow_egr_mac_da_restore(int unit)
{
    int min, max, i;
    soc_mem_t mem;
    uint32 view_id;
    uint32 data_type = 0;
    soc_flow_db_ctrl_field_t field_list;
    uint32 l3_if_entry[SOC_MAX_MEM_WORDS];
    int num_fields = 0;
    int rv = BCM_E_NONE;

    sal_memset(&l3_if_entry, 0, sizeof(l3_if_entry));

    mem = BCM_XGS3_L3_MEM(unit, intf);
    min = soc_mem_index_min(unit, mem);
    max = soc_mem_index_max(unit, mem);

    for (i = min; i < max; i++) {
         BCM_IF_ERROR_RETURN
             (BCM_XGS3_MEM_READ(unit, mem, i, &l3_if_entry));

         if (SOC_MEM_FIELD_VALID(unit, mem, DATA_TYPEf)) {
             data_type = soc_mem_field32_get(unit, mem, &l3_if_entry, DATA_TYPEf);
         }

         if (data_type == 0) {
            continue;
         }

         if(soc_mem_field_valid(unit, mem, FLOW_SELECT_CTRL_IDf)) {
             field_list.field_id = FLOW_SELECT_CTRL_IDf;
             field_list.value = soc_mem_field32_get(unit, mem,
                 &l3_if_entry, FLOW_SELECT_CTRL_IDf);
             num_fields = 1;
         }
         /* coverity[callee_ptr_arith : FALSE] */
         rv = soc_flow_db_mem_to_view_id_get(unit,
                 mem,
                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                 data_type,
                 num_fields,
                 &field_list,
                 &view_id);

        /* Recover MAC_DA profile count */
        if ((rv == BCM_E_NONE) && soc_mem_field_valid(unit, view_id,
             PROFILED_ASSIGNMENT_SRC_MAC_ADDRESS_ACTION_SETf)) {

            uint32 macda_idx;

            macda_idx = soc_mem_field32_get(unit, view_id, &l3_if_entry,
                PROFILED_ASSIGNMENT_SRC_MAC_ADDRESS_ACTION_SETf);

            _bcm_common_profile_mem_ref_cnt_update(
                    unit, EGR_MAC_DA_PROFILEm, macda_idx, 1);
        }
    }

    return rv;
}
/*
 * Function:
 *      _bcm_flow_reinit
 * Purpose:
 *      Warm boot recovery for the VXLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_reinit(int unit)
{
    int rv = BCM_E_NONE;

    /* Tunnel initiator hash and ref-count recovery */
    rv = _bcm_flow_tunnel_initiator_reinit(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Recover L2 scache */
    rv = _bcm_flow_wb_recover(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* increase the used next hop object reference count */
    rv = _bcm_flow_next_hop_refcnt_restore(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* increase the used egr mac da reference count */
    rv = _bcm_flow_egr_mac_da_restore(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* traverse egress adaptation tables to recover the tpid reference count */
    rv = bcmi_esw_flow_encap_traverse(0,
                    _bcm_flow_encap_traverse_cb_tpid_refcnt,NULL);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flow_sync
 *
 * Purpose:
 *      Record flow module persistent info for Level 2 Warm Boot
 *
 * Warm Boot Version Map:
 *  WB_VERSION_1_0
* Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_flow_sync(int unit)
{
    int sz = 0, rv = BCM_E_NONE;
    int num_soft_tnl = 0, num_dvp = 0;
    int stable_size;
    uint8 *flow_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_flow_bookkeeping_t *flow_info;
    int num_iif;
    int num_vp;
    int i;

    if (!soc_feature(unit, soc_feature_flex_flow)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_flow_check_init(unit));

    flow_info = FLOW_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOW, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &flow_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
        return rv;
    }

    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
    num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);

    /* save init_tunnel entry table */
    sz = sizeof(uint16); /* idx of _bcm_flow_init_tunnel_entry_t */
    for (i = 0; i < num_soft_tnl; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->init_tunnel[i].idx, sz);
        flow_scache_ptr += sz;
    }

    /* save the dvp_attr_bitmap */
    sz = SHR_BITALLOCSIZE(num_dvp);
    sal_memcpy(flow_scache_ptr, flow_info->dvp_attr_bitmap, sz);
    flow_scache_ptr += sz;

    /* save iif_ref_cnt table */
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    sz = sizeof(uint16); /* iif_ref_cnt */ 
    for (i = 0; i < num_iif; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->iif_ref_cnt[i], sz);
        flow_scache_ptr += sz;
    }

    /* save vp_ref_cnt table */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    sz = sizeof(uint16); /* vp_ref_cnt */ 
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->vp_ref_cnt[i], sz);
        flow_scache_ptr += sz;
    }

    /* save the memory usage reference count table */
    sz = sizeof(uint32);  
    for (i = 0; i < _BM_FLOW_MEM_USE_MAX; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->mem_use_cnt[i], sz);
        flow_scache_ptr += sz;
    }

    /* save match_key array */
    sz = sizeof(bcm_flow_match_vp_t);
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->match_key[i], sz);
        flow_scache_ptr += sz;
    }
 
    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcmi_esw_flow_init
 * Purpose:
 *      Initialize the FLOW software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_esw_flow_init(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info;
    int num_dvp; 
    int rv = BCM_E_NONE;
    int num_vfi = 0;
    int num_tnl;
    int num_iif;
    int num_vp;
    uint64 rval;

    if (!L3_INFO(unit)->l3_initialized) {
        LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit,
                              "L3 module must be initialized prior to FLOW Init\n")));
        return BCM_E_CONFIG;
    }

    if (BCM_FAILURE(soc_flow_db_status_get(unit))) {
        return BCM_E_INIT;
    }
    /* Allocate BK Info */
    BCM_IF_ERROR_RETURN(bcm_td3_flow_allocate_bk(unit));
    flow_info = FLOW_INFO(unit);

    /*
     * allocate resources
     */
    if (flow_info->initialized) {
         BCM_IF_ERROR_RETURN(bcmi_esw_flow_cleanup(unit));
         BCM_IF_ERROR_RETURN(bcm_td3_flow_allocate_bk(unit));
         flow_info = FLOW_INFO(unit);
    }

    /* Create EGR_DVP_ATTRIBUTEm usage bitmap */
    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);

    flow_info->dvp_attr_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_dvp), "dvp_attr_bitmap");
    if (flow_info->dvp_attr_bitmap == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->dvp_attr_bitmap, 0, SHR_BITALLOCSIZE(num_dvp));

    /* Create soft tunnel index table  */
    /* L2 tunnel associated with dvp, dvp represents the soft tunnnel index */
    num_tnl = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);

    /* L3 tunnel associated with egr_intf, soft tunnel index: 
     * max l2 tunnel + EGR_L3_INTFm index. 
     */
    num_tnl += soc_mem_index_count(unit, EGR_L3_INTFm);
    flow_info->init_tunnel =
        sal_alloc(sizeof(_bcm_flow_init_tunnel_entry_t) * num_tnl, 
                  "flow_init_tunnel");
    if (flow_info->init_tunnel == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->init_tunnel, 0, 
               sizeof(_bcm_flow_init_tunnel_entry_t) * num_tnl);

    /* Create iif_ref_cnt table */
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    flow_info->iif_ref_cnt =
        sal_alloc(sizeof(uint16) * num_iif,
                  "flow_match iif_ref_cnt");
    if (flow_info->iif_ref_cnt == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->iif_ref_cnt, 0,
               sizeof(uint16) * num_iif);

    /* Create vp_ref_cnt table */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    flow_info->vp_ref_cnt =
        sal_alloc(sizeof(uint16) * num_vp,
                  "flow_match vp_ref_cnt");
    if (flow_info->vp_ref_cnt == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->vp_ref_cnt, 0,
               sizeof(uint16) * num_vp);

    flow_info->match_key =
        sal_alloc(sizeof(bcm_flow_match_vp_t) * num_vp,
                  "flow_match match_key");
    if (flow_info->match_key == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->match_key, 0,
               sizeof(bcm_flow_match_vp_t) * num_vp);

    rv = READ_EGR_SEQUENCE_NUMBER_CTRLr(unit, &rval);
    if (SOC_FAILURE(rv)) {
         _bcm_td3_flow_free_resource(unit);
         return rv;
    }
    flow_info->frag_base_inx = soc_reg64_field32_get(unit, 
             EGR_SEQUENCE_NUMBER_CTRLr, rval, EGR_IP_TUNNEL_OFFSET_BASEf);

    /* Create FLOW protection mutex. */
    flow_info->flow_mutex = sal_mutex_create("flow_mutex");
    if (!flow_info->flow_mutex) {
         _bcm_td3_flow_free_resource(unit);
         return BCM_E_MEMORY;
    }

#if 0
    if (NULL == flow_info->flow_tunnel_term) {
        flow_info->flow_tunnel_term =
            sal_alloc(sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp, "flow tunnel term store");
        if (flow_info->flow_tunnel_term == NULL) {
            _bcm_td3_flow_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(flow_info->flow_tunnel_term, 0, 
                sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp);
    }

#endif

    num_vfi = soc_mem_index_count(unit, VFIm);
    if (NULL == flow_info->flow_vpn_vlan) {
        flow_info->flow_vpn_vlan =
            sal_alloc(sizeof(bcm_vlan_t) * num_vfi, "flow vpn vlan store");
        if (flow_info->flow_vpn_vlan == NULL) {
            _bcm_td3_flow_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(flow_info->flow_vpn_vlan, 0, sizeof(bcm_vlan_t) * num_vfi);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_flow_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_td3_flow_free_resource(unit);
        }
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_flow_wb_alloc(unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

        if (BCM_SUCCESS(rv) &&
            soc_property_get(unit, spn_IP6_VXLAN_MSHG_ENABLE, 0)) {
            bcmi_esw_flow_mshg_prune_egress_init(unit);
        }
    }

    /* Mark the state as initialized */
    flow_info->initialized = TRUE;

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_check_init
 * Purpose:
 *      Check if FLOW is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_esw_flow_check_init(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    flow_info = FLOW_INFO(unit);

    if ((flow_info == NULL) || (flow_info->initialized == FALSE)) {
         return BCM_E_INIT;
    } else {
         return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcmi_esw_flow_cleanup
 * Purpose:
 *      DeInit  FLOW software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_esw_flow_cleanup(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info;
    int rv = BCM_E_UNAVAIL;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    flow_info = FLOW_INFO(unit);

    if (FALSE == flow_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcmi_esw_flow_lock (unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) { 
        rv = _bcm_td3_flow_hw_clear(unit);
    }

    /* Mark the state as uninitialized */
    flow_info->initialized = FALSE;

    sal_mutex_give(flow_info->flow_mutex);

    /* Destroy protection mutex. */
    sal_mutex_destroy(flow_info->flow_mutex );

    /* Free software resources */
    (void) _bcm_td3_flow_free_resource(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_lock
 * Purpose:
 *      Take FLOW Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

 int 
 bcmi_esw_flow_lock(int unit)
{
   int rv=BCM_E_NONE;

   rv = bcmi_esw_flow_check_init(unit);
   
   if ( rv == BCM_E_NONE ) {
           sal_mutex_take(FLOW_INFO((unit))->flow_mutex, sal_mutex_FOREVER);
   }
   return rv; 
}

/*
 * Function:
 *      bcmi_esw_flow_unlock
 * Purpose:
 *      Release  FLOW Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


void
bcmi_esw_flow_unlock(int unit)
{
   int rv=BCM_E_NONE;

   rv = bcmi_esw_flow_check_init(unit);
    if ( rv == BCM_E_NONE ) {
         sal_mutex_give(FLOW_INFO((unit))->flow_mutex);
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
STATIC
void _bcmi_esw_flow_sw_dump(int unit)
{
    int num_dvp = 0;
    int num_iif = 0;
    int num_vp  = 0;
    int num_vfi = 0;
    int num_soft_tnl = 0;
    _bcm_flow_bookkeeping_t *flow_info = NULL;
    int i = 0;
    uint16 flag = FALSE;

    flow_info = FLOW_INFO(unit);

    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
    num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    num_vfi = soc_mem_index_count(unit, VFIm);

    for (i = 0, flag = FALSE; i < num_vfi; i++) {
        if (VIRTUAL_INFO(unit)->flow_vfi_bitmap) {
            if (SHR_BITGET(VIRTUAL_INFO(unit)->flow_vfi_bitmap, i)) {
                if (flag == TRUE) {
                    LOG_CLI((BSL_META_U(unit," , %d"), i));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                      "%-32s : %d \n"),"Flow VFI", i));
                }
                flag = TRUE;
            }
        }
    }

    for (i = 0, flag = FALSE; i < num_vp; i++) {
        if (VIRTUAL_INFO(unit)->flow_vp_bitmap) {
            if (SHR_BITGET(VIRTUAL_INFO(unit)->flow_vp_bitmap, i)) {
                if (flag == TRUE) {
                    LOG_CLI((BSL_META_U(unit," , %d"), i));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                         "\n%-32s : %d"), "Flow VP", i));
                }
                flag = TRUE;
            }
        }
    }

    for (i = 0, flag = FALSE; i < num_soft_tnl; i++) {
        if(flow_info->init_tunnel[i].idx) {
            if (flag == TRUE) {
                LOG_CLI((BSL_META_U(unit," , %d"), i));
            } else {
                LOG_CLI((BSL_META_U(unit,
                    "\n%-32s : %d"),
                    "init_tunnel soft index",
                    num_soft_tnl));
            }
            flag = TRUE;
        }
    }

    LOG_CLI((BSL_META_U(unit,
             "\n%-32s : 0x%x"),
             "DVP Attribute bitmap",*flow_info->dvp_attr_bitmap));

    for (i = 0; i < num_iif;  i++) {
        if (flow_info->iif_ref_cnt[i]) {
            LOG_CLI((BSL_META_U(unit,
                 "\n %-10s : %d, %-14s : %d"),
                 "IIF index", i, "IIF use count", flow_info->iif_ref_cnt[i]));
        }
    }

    for (i = 0; i < num_vp; i++) {
        if (flow_info->vp_ref_cnt[i]) {
            LOG_CLI((BSL_META_U(unit,
                 "\n%-10s : %d,  %-14s : %d"),
                 "VP index", i, "VP use count", flow_info->vp_ref_cnt[i]));
        }
    }

    LOG_CLI((BSL_META_U(unit,"\n")));
    return;
}
/*
 * Function:
 *     bcmi_esw_flow_sw_dump
 * Purpose:
 *     Displays flow information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcmi_esw_flow_sw_dump(int unit)
{
    if (soc_feature(unit, soc_feature_flex_flow)) {
        _bcmi_esw_flow_sw_dump(unit);
    }
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif
