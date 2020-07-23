/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flexflow_encap.c
 * Purpose:     Flex flow Encap APIs.
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef INCLUDE_L3

#include <assert.h>
#include <bcm/error.h>
#include <soc/error.h>
#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
#include <bcm_int/esw/subport.h>
#endif

/* encap criteria specifies the key field composition excluding key type. A
 * same criteria can associate with both fixed key type and flex key type.
 * encap criteria along with flow_handle and flow_option 
 * determines the key type. In a get/delete operation, user is required to 
 * specify all these parameters.  
 *
 * for network-facing side:
 *   L3_INTF  -- no fixed key type
 *   VFI      -- L2GRE_VFI, VXLAN_VFI and flex key types
 *   VFI_DVP  -- L2GRE_VFI_DVP,VXLAN_VFI_DVP and flex key types
 *   VFI_DVP_GROUP -- VFI_DVP_GROUP and flex key types
 *   VRF_MAPPING   -- VRF_MAPPING and flex key types
 *
 * for access-facing side:
 *   L3_INTF  -- no fixed key type
 *   VFI_DVP  -- VLAN_XLATE_VFI and flex key types
 *   VFI_DVP_GROUP -- VFI_DVP_GROUP and flex key types
 */

#define _BCM_FLOW_DATA_TYPE_COMMON      0
#define _BCM_FLOW_PDD_FIELD_MAX         50
#define _BCM_FLOW_MODID_SHIFTER        8
#define _BCM_FLOW_PORT_MASK            8
#define _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX  25

STATIC int _bcm_flow_nexthop_entry_set(int unit, int nh_index, int drop,
              bcm_flow_port_encap_t *pe_info, int is_network_port);
STATIC int _bcm_flow_sd_tag_set(int unit, bcm_flow_encap_config_t *info,
                         uint32 *sd_tag, int *tpid_index);
STATIC int _bcm_flow_sd_tag_get(int unit, bcm_flow_encap_config_t *info,
                          uint32 sd_tag, int  *tpid_idx);
STATIC int _bcm_flow_port_encap_info_convert(bcm_flow_port_encap_t *pe_info,
        bcm_flow_encap_config_t *info, int port_encap2config);
STATIC void _bcm_flow_egr_nh_sd_tag_get(int unit, soc_mem_t hw_mem, 
                  uint32 *entry, uint32 *sd_tag);
STATIC int _bcm_flow_linked_tbl_sd_tag_get(int unit, soc_mem_t view_id, 
              uint32 *egr_nh, uint32 *sd_tag);
STATIC int _bcm_flow_sd_tag_set_to_entry(int unit,
            bcm_flow_encap_config_t *info, soc_mem_t mem, uint32 *entry,
            int *tpid_index);
STATIC int _bcm_flow_sd_tag_get_from_entry(int unit,
            bcm_flow_encap_config_t *info, soc_mem_t mem, uint32 *entry,
            int  *tpid_idx);
STATIC int _bcm_flow_sd_tag_set_to_format(int unit,
            bcm_flow_encap_config_t *info, soc_mem_t mem, uint32 *entry,
            int *tpid_index);
STATIC int _bcm_flow_sd_tag_get_from_format(int unit,
            bcm_flow_encap_config_t *info, soc_mem_t mem, uint32 *entry,
            int  *tpid_idx);


/*
 * Function:
 *      bcmi_esw_flow_dvp_vlan_xlate_key_set
 * Description:
 *   Set egress vlan translation key per flow gport. call from  
 *   bcm_vlan_control_port_set with control type bcmVlanPortTranslateEgressKey
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_id         - (IN) vxlan gport
 *      key_type      - (IN) Hardware egress vlan translation key type.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int
bcmi_esw_flow_dvp_vlan_xlate_key_set(int unit, bcm_gport_t port_id, 
                                     int key_type)
{
    int vp, egr_key_type_value = 0, rv = BCM_E_UNAVAIL;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    int network_port = 0;
    egr_dvp_attribute_entry_t egr_dvp_attribute;
    _bcm_vp_info_t vp_info;
    soc_field_t evxlt_field_id = 0;
    int key_type_value;
    uint32 data_type;

    vp = BCM_GPORT_FLOW_PORT_ID_GET(port_id);
    if (vp < 0) {
        return BCM_E_NOT_FOUND;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    /* make sure the dvp is initialized */
    if (!_BCM_FLOW_EGR_DVP_USED_GET(unit,vp)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
          "DVP %d not initialized, call bcm_flow_port_encap_set first\n"), vp));
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_dvp_vtkey_type_value_get(unit, key_type,
                                                           &key_type_value));
 
    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port = TRUE;
    } else {
        network_port = FALSE;
    }

    /* bcmVlanTranslateEgressKeyVpn only for network port*/
    if ((!network_port) && (key_type == bcmVlanTranslateEgressKeyVpn)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&egr_dvp_attribute, 0,
               sizeof(egr_dvp_attribute_entry_t));

    soc_mem_lock(unit,mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit,mem);
        return rv;
    }

    data_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        DATA_TYPEf);
    if (data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) {  
        evxlt_field_id = VXLAN__EVXLT_KEY_SELf;
    } else if (data_type == _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) { 
        evxlt_field_id = L2GRE__EVXLT_KEY_SELf;
    } else if (data_type > _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) { 
       /* flex type */
        /* use vxlt_control_id */
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
          "DVP %d is for flex flow, TBD\n"), vp));
    } else if (data_type == _BCM_FLOW_DATA_TYPE_COMMON) { 
        evxlt_field_id = COMMON__EVXLT_KEY_SELf;
    }
    if (evxlt_field_id != 0) {
        egr_key_type_value = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        evxlt_field_id);
       
        if (key_type_value != egr_key_type_value) {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute,
                            evxlt_field_id, key_type);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                               vp, &egr_dvp_attribute);
        } else {
            /* already exist, same key */
            rv = BCM_E_NONE;
        }
    }
    soc_mem_unlock(unit,mem);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_dvp_vlan_xlate_key_get
 * Description:
 *   Get egress vlan translation key per flow gport. call from  
 *   bcm_vlan_control_port_get with control type bcmVlanPortTranslateEgressKey
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_id         - (IN) vxlan gport
 *      key_type      - (OUT) logical egress vlan translation key type.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int
bcmi_esw_flow_dvp_vlan_xlate_key_get(int unit, bcm_gport_t port_id, 
                                     int *key_type)
{
    int vp, egr_key_type_value = 0, rv = BCM_E_UNAVAIL;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    int network_port = 0;
    egr_dvp_attribute_entry_t egr_dvp_attribute;
    _bcm_vp_info_t vp_info;
    soc_field_t evxlt_field_id = 0;
    uint32 data_type;

    vp = BCM_GPORT_FLOW_PORT_ID_GET(port_id);
    if (vp < 0) {
        return BCM_E_NOT_FOUND;
    }
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        return BCM_E_NOT_FOUND;
    }

    /* make sure the dvp is initialized */
    if (!_BCM_FLOW_EGR_DVP_USED_GET(unit,vp)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
          "DVP %d not initialized, call bcm_flow_port_encap_set first\n"), vp));
        return BCM_E_NOT_FOUND;
    }
 
    /* Check for Network-Port */
    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port = TRUE;
    } else {
        network_port = FALSE;
    }

    sal_memset(&egr_dvp_attribute, 0,
               sizeof(egr_dvp_attribute_entry_t));

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    data_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        DATA_TYPEf);
    if (data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) {  
        evxlt_field_id = VXLAN__EVXLT_KEY_SELf;
    } else if (data_type == _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) { 
        evxlt_field_id = L2GRE__EVXLT_KEY_SELf;
    } else if (data_type > _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) { 
       /* flex type */
        /* use vxlt_control_id */
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
          "DVP %d is for flex flow, TBD\n"), vp));
    } else if (data_type == _BCM_FLOW_DATA_TYPE_COMMON) { 
        evxlt_field_id = COMMON__EVXLT_KEY_SELf;
    }
    if (evxlt_field_id != 0) {
        egr_key_type_value = soc_mem_field32_get(unit, mem, &egr_dvp_attribute,
                                        evxlt_field_id);
        rv = _bcm_esw_dvp_vtkey_type_get(unit, 
                                 egr_key_type_value, key_type);

        /* bcmVlanTranslateEgressKeyVpn only for network port*/
        if ((!network_port) && (*key_type == bcmVlanTranslateEgressKeyVpn)) {
            return BCM_E_UNAVAIL;
        }
    }
    return rv;
}

STATIC int
_bcm_flow_egress_nh_vntag_check (int unit,
                       int nh_inx,
                       int *vntag_used)
{
    egr_l3_next_hop_entry_t egr_nh_entry;
    int vntag_action;
    int vntag_dst;

    *vntag_used = FALSE;
    
    BCM_IF_ERROR_RETURN(READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
        nh_inx, &egr_nh_entry));

    vntag_action = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                 L3__VNTAG_ACTIONSf);

    vntag_dst = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh_entry,
                 L3__VNTAG_DST_VIFf);

    if (vntag_dst && vntag_action) {
        *vntag_used = TRUE;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_egress_network_dvp_set(int unit,
                       bcm_flow_port_encap_t *info,
                       int nh_idx,
                       bcm_flow_logical_field_t *field,
                       int num_of_fields)
{

    int rv=BCM_E_UNAVAIL;
    int local_drop = 0;
    int network_group=0;
    soc_mem_t mem_view_id;
    soc_mem_t mem_id = 0;
    int dvp;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 dvp_ctrl;
    soc_field_t dvp_grp_f;
    soc_field_t network_grp_f;
    soc_field_t dvp_port_f;
    soc_field_t dis_vp_p_f;
    soc_field_t del_vntag_f;
    soc_field_t class_id_f;
    soc_field_t mtu_val_f;
    soc_field_t mtu_en_f;
    soc_field_t uuc_drop_f;
    soc_field_t umc_drop_f;
    soc_field_t bc_drop_f;
    uint32 data_ids[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
    uint32 cnt;
    int i;
    int j;
    int vntag_used;

    dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    mem_id = EGR_DVP_ATTRIBUTEm;

    /* get the physical memory view */
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,info->flow_handle,
                   info->flow_option, SOC_FLOW_DB_FUNC_ENCAP_SET_ID,
                   (uint32 *)&mem_view_id);
        BCM_IF_ERROR_RETURN(rv);
        LOG_VERBOSE(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
             "dvp: %d mem_view_id: 0x%x\n"), dvp, mem_view_id));

        mem_id = mem_view_id;
    }

    if (info->options & BCM_FLOW_PORT_ENCAP_OPTION_REPLACE) {
        if (!_BCM_FLOW_EGR_DVP_USED_GET(unit,dvp)) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit,mem_id, MEM_BLOCK_ANY, dvp, 
                                         entry));
    } else {
        sal_memset(entry, 0, sizeof(entry));
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            /* flex view, initialize all control fields such as data_type */
            soc_flow_db_mem_view_entry_init(unit,mem_view_id,entry);
        }
    }

    local_drop = (info->flags & BCM_FLOW_PORT_ENCAP_FLAG_DROP) ? 1 : 0;

    if (info->valid_elements & BCM_FLOW_PORT_ENCAP_NETWORK_GROUP_VALID) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)){
            network_group = info->network_group; 
            rv = _bcm_validate_splithorizon_network_group(unit,
                   TRUE, &network_group);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
        soc_mem_field32_set(unit, mem_id, entry, DATA_TYPEf, 
                           info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN?
                                   _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE:
                                   _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE);

        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            dvp_grp_f    = VXLAN__VT_DVP_GROUP_IDf;
            network_grp_f = VXLAN__DVP_NETWORK_GROUPf;
            dvp_port_f   = VXLAN__DVP_IS_NETWORK_PORTf;
            dis_vp_p_f   = VXLAN__DISABLE_VP_PRUNINGf;
            del_vntag_f  = VXLAN__DELETE_VNTAGf;
            class_id_f   = VXLAN__CLASS_IDf;
            mtu_val_f    = VXLAN__MTU_VALUEf;
            mtu_en_f     = VXLAN__MTU_ENABLEf;
            uuc_drop_f   = VXLAN__UUC_DROPf;
            umc_drop_f   = VXLAN__UMC_DROPf;
            bc_drop_f    = VXLAN__BC_DROPf;
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            dvp_grp_f    = L2GRE__VT_DVP_GROUP_IDf;
            network_grp_f = L2GRE__DVP_NETWORK_GROUPf;
            dvp_port_f   = L2GRE__DVP_IS_NETWORK_PORTf;
            dis_vp_p_f   = L2GRE__DISABLE_VP_PRUNINGf;
            del_vntag_f  = L2GRE__DELETE_VNTAGf;
            class_id_f   = L2GRE__CLASS_IDf;
            mtu_val_f    = L2GRE__MTU_VALUEf;
            mtu_en_f     = L2GRE__MTU_ENABLEf;
            uuc_drop_f   = L2GRE__UUC_DROPf;
            umc_drop_f   = L2GRE__UMC_DROPf;
            bc_drop_f    = L2GRE__BC_DROPf;
        } else {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                 "flow handle: %d unsupported\n"), info->flow_handle));
            return BCM_E_PARAM;
        }

        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_NETWORK_GROUP_VALID) {
            if (soc_feature(unit, soc_feature_multiple_split_horizon_group)){
                soc_mem_field32_set(unit, mem_id, entry, network_grp_f, 
                                    network_group);
            } else {
                soc_mem_field32_set(unit, mem_id, entry, dvp_port_f, 1);
            }
        }
        soc_mem_field32_set(unit, mem_id, entry, dis_vp_p_f, 0);

        /* Application INT IFA uses VNTAG field in egr_nh to carry DGPP
         * Don't set this field it is set in egr_nh. 
         */
        BCM_IF_ERROR_RETURN(_bcm_flow_egress_nh_vntag_check (unit, nh_idx,
                 &vntag_used));
        if (vntag_used == FALSE) {
            soc_mem_field32_set(unit, mem_id, entry, del_vntag_f, 1);
        }
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_CLASS_ID_VALID) {
            soc_mem_field32_set(unit, mem_id, entry, class_id_f,info->class_id);
        }
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_MTU_VALID) {
            soc_mem_field32_set(unit, mem_id, entry, mtu_val_f, info->mtu);
            soc_mem_field32_set(unit, mem_id, entry, mtu_en_f, info->mtu? 1: 0);
        }
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_DVP_GROUP_VALID) {
            soc_mem_field32_set(unit, mem_id, entry, dvp_grp_f,info->dvp_group);
        }
        soc_mem_field32_set(unit, mem_id, entry, uuc_drop_f, local_drop);
        soc_mem_field32_set(unit, mem_id, entry, umc_drop_f, local_drop);
        soc_mem_field32_set(unit, mem_id, entry, bc_drop_f,  local_drop);

    } else {

        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_NETWORK_GROUP_VALID) {
            if (SOC_MEM_FIELD_VALID(unit,mem_id,SWITCHING_CTRLS_ACTION_SETf)) {
                dvp_ctrl = 0;
                if (soc_feature(unit, soc_feature_multiple_split_horizon_group)){
                    soc_format_field32_set(unit, DVP_SWITCHING_CTRLS_ACTION_SETfmt,
                                       &dvp_ctrl, DVP_NETWORK_GROUPf,network_group);
                } 
                soc_format_field32_set(unit, DVP_SWITCHING_CTRLS_ACTION_SETfmt,
                                 &dvp_ctrl, DISABLE_VP_PRUNINGf,0);

                soc_format_field32_set(unit, DVP_SWITCHING_CTRLS_ACTION_SETfmt,
                                 &dvp_ctrl, UUC_DROPf,local_drop);
                soc_format_field32_set(unit, DVP_SWITCHING_CTRLS_ACTION_SETfmt,
                                 &dvp_ctrl, UMC_DROPf,local_drop);
                soc_format_field32_set(unit, DVP_SWITCHING_CTRLS_ACTION_SETfmt,
                                 &dvp_ctrl, BC_DROPf,local_drop);
                soc_mem_field32_set(unit, mem_id, entry,
                                     SWITCHING_CTRLS_ACTION_SETf, dvp_ctrl);
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                     "SWITCHING_CTRLS_ACTION_SETf not defined in memory view\n")));
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_CLASS_ID_VALID) {
            if (SOC_MEM_FIELD_VALID(unit,mem_id,CLASS_ID_ACTION_SETf)) {
                soc_mem_field32_set(unit, mem_id, entry, CLASS_ID_ACTION_SETf,
                                    info->class_id);
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                     "CLASS_ID_ACTION_SETf not defined in memory view\n")));
                return BCM_E_INTERNAL;
            }
        }
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_MTU_VALID) {
            if (SOC_MEM_FIELD_VALID(unit,mem_id,MTU_CONTROLS_ACTION_SETf)) {
                uint32 mtu = 0;

                soc_format_field32_set(unit, MTU_CONTROLS_ACTION_SETfmt,
                                       &mtu, MTU_VALUEf,info->mtu);
                soc_format_field32_set(unit, MTU_CONTROLS_ACTION_SETfmt,
                                       &mtu, MTU_ENABLEf,info->mtu? 1 : 0);
                soc_mem_field32_set(unit, mem_id, entry, 
                                    MTU_CONTROLS_ACTION_SETf, mtu);
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                     "MTU_CONTROLS_ACTION_SETf not defined in memory view\n")));
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_DVP_GROUP_VALID) {
            if (SOC_MEM_FIELD_VALID(unit,mem_id,VXLT_CLASS_IDf)) {
                soc_mem_field32_set(unit, mem_id, entry, VXLT_CLASS_IDf,
                                    info->dvp_group);
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                     "VXLT_CLASS_IDf not defined in memory view\n")));
                return BCM_E_PARAM;
            }
        }

        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_FLEX_DATA_VALID) {
            rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem_id,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, data_ids, &cnt);
            BCM_IF_ERROR_RETURN(rv);
            for (i = 0; i < num_of_fields; i++) {
                for (j = 0; j < cnt; j++) {
                    if (field[i].id == data_ids[j]) {
                        soc_mem_field32_set(unit,mem_id, entry,
                              field[i].id, field[i].value);
                        break;
                    }
                }
            }
        }
    }
    rv = soc_mem_write(unit, mem_id, MEM_BLOCK_ALL, dvp, entry);
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

STATIC int
_bcm_flow_egress_dvp_get(int unit,
                       bcm_flow_port_encap_t *info,
                       bcm_flow_logical_field_t *field,
                       int num_of_fields)
{

    int rv;
    int local_drop = 0;
    soc_mem_t mem_id = 0;
    int dvp;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 dvp_ctrl;
    soc_field_t dvp_grp_f;
    soc_field_t netowk_grp_f;
    soc_field_t class_id_f;
    soc_field_t mtu_val_f;
    soc_field_t uuc_drop_f;
    uint32 data_type;
    uint32 data_ids[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
    uint32 cnt;
    int i;
    int j;
    soc_mem_t mem_view_id;

    dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    mem_id = EGR_DVP_ATTRIBUTEm;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,mem_id, MEM_BLOCK_ANY, dvp, 
                                         entry));
    data_type = soc_mem_field32_get(unit, mem_id, entry, DATA_TYPEf);
    
    if (data_type == _BCM_FLOW_DATA_TYPE_COMMON) {
        info->flow_handle = 0;
        info->mtu = soc_mem_field32_get(unit, mem_id, entry,
                                COMMON__MTU_VALUEf);
        info->network_group = soc_mem_field32_get(unit, mem_id, entry,
                                COMMON__DVP_NETWORK_GROUPf);
        return BCM_E_NONE;
    } else if (data_type == _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE) {
        info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
    } else if (data_type == _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {
        info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
    } else { /* flex */
        /* find the memory view based on the memory and key type */
        rv = soc_flow_db_mem_to_view_id_get (unit, mem_id, 
                    SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                    0, NULL, (uint32 *)&mem_view_id);
        SOC_IF_ERROR_RETURN(rv);
        mem_id = mem_view_id;
    }

    if (data_type <= _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE) {  /* fixed view */
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            netowk_grp_f = VXLAN__DVP_NETWORK_GROUPf;
            dvp_grp_f    = VXLAN__VT_DVP_GROUP_IDf;
            class_id_f   = VXLAN__CLASS_IDf;
            mtu_val_f    = VXLAN__MTU_VALUEf;
            uuc_drop_f   = VXLAN__UUC_DROPf;
        } else {
            netowk_grp_f = L2GRE__DVP_NETWORK_GROUPf;
            dvp_grp_f    = L2GRE__VT_DVP_GROUP_IDf;
            class_id_f   = L2GRE__CLASS_IDf;
            mtu_val_f    = L2GRE__MTU_VALUEf;
            uuc_drop_f   = L2GRE__UUC_DROPf;
        }
        info->network_group = soc_mem_field32_get(unit, mem_id, entry, 
                                    netowk_grp_f);
        info->dvp_group = soc_mem_field32_get(unit, mem_id, entry, 
                                    dvp_grp_f);
        
        info->class_id = soc_mem_field32_get(unit, mem_id, entry, class_id_f);
        info->mtu = soc_mem_field32_get(unit, mem_id, entry, mtu_val_f);
        local_drop = soc_mem_field32_get(unit, mem_id, entry, uuc_drop_f);
        if (local_drop) {
            info->flags |= BCM_FLOW_PORT_ENCAP_FLAG_DROP;
        }

    } else {  /* flex view */
        if (SOC_MEM_FIELD_VALID(unit,mem_id,SWITCHING_CTRLS_ACTION_SETf)) {
            dvp_ctrl = soc_mem_field32_get(unit, mem_id, entry,
                                     SWITCHING_CTRLS_ACTION_SETf);
            info->network_group = soc_format_field32_get(unit, 
                                   DVP_SWITCHING_CTRLS_ACTION_SETfmt,
                                   &dvp_ctrl, DVP_NETWORK_GROUPf);
            local_drop = soc_format_field32_get(unit, 
                               DVP_SWITCHING_CTRLS_ACTION_SETfmt,
                               &dvp_ctrl, UUC_DROPf);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_id,CLASS_ID_ACTION_SETf)) {
            info->class_id = soc_mem_field32_get(unit, mem_id, entry, 
                                CLASS_ID_ACTION_SETf);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_id,MTU_CONTROLS_ACTION_SETf)) {
            uint32 mtu = 0;

            mtu = soc_mem_field32_get(unit, mem_id, entry,
                                    MTU_CONTROLS_ACTION_SETf);
            info->mtu = soc_format_field32_get(unit, MTU_CONTROLS_ACTION_SETfmt,
                                   &mtu, MTU_VALUEf);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_id,VXLT_CLASS_IDf)) {
            info->dvp_group  = soc_mem_field32_get(unit, mem_id, entry, 
                                        VXLT_CLASS_IDf);
        }

        if (field != NULL && num_of_fields) {
            rv = soc_flow_db_mem_view_field_list_get(unit,
                          mem_id,
                          SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                          _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, data_ids, &cnt);
            BCM_IF_ERROR_RETURN(rv);
            for (i = 0; i < num_of_fields; i++) {
                for (j = 0; j < cnt; j++) {
                    if (field[i].id == data_ids[j]) {
                        field[i].value = soc_mem_field32_get(unit,mem_id, entry,
                              field[i].id);
                        break;
                    }
                }
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_egress_access_dvp_set(int unit,
                       bcm_flow_port_encap_t *info,
                       bcm_flow_logical_field_t *field,
                       int num_of_fields)
{
    int rv = BCM_E_UNAVAIL;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int dvp;
    soc_mem_t mem_view_id;
    uint32 data_ids[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
    uint32 cnt;
    int i;
    int j;
    int network_group = 0;

    dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);

    /* get the physical memory view */
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,info->flow_handle,
                   info->flow_option, SOC_FLOW_DB_FUNC_ENCAP_SET_ID,
                   (uint32 *)&mem_view_id);
        BCM_IF_ERROR_RETURN(rv);
        mem = mem_view_id;
    }

    if (info->options & BCM_FLOW_PORT_ENCAP_OPTION_REPLACE) {
        if (!_BCM_FLOW_EGR_DVP_USED_GET(unit,dvp)) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit,mem, MEM_BLOCK_ANY, dvp,
                                         entry));
    } else {
        sal_memset(entry, 0, sizeof(entry));
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            /* flex view, initialize all control fields such as data_type */
            soc_flow_db_mem_view_entry_init(unit,mem_view_id,entry);
        }
    }

    if (info->valid_elements & BCM_FLOW_PORT_ENCAP_NETWORK_GROUP_VALID) {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)){
            network_group = info->network_group;
            rv = _bcm_validate_splithorizon_network_group(unit,
                   TRUE, &network_group);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_MTU_VALID) {
            /* L2 MTU - This is different from L3 MTU */
            if (info->mtu > 0) {
                soc_mem_field32_set(unit, mem, entry, COMMON__MTU_VALUEf,
                                    info->mtu);
                soc_mem_field32_set(unit, mem, entry, COMMON__MTU_ENABLEf, 0x1);
            } else {
                /* Disable mtu checking if mtu = 0 */
                soc_mem_field32_set(unit, mem, entry, COMMON__MTU_ENABLEf, 0x0);
            }
        }

        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_NETWORK_GROUP_VALID) {
            if (soc_feature(unit, soc_feature_multiple_split_horizon_group)){
                soc_mem_field32_set(unit, mem, entry, COMMON__DVP_NETWORK_GROUPf,
                                    network_group);
            }
        }
    } else {
        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_MTU_VALID) {
            if (SOC_MEM_FIELD_VALID(unit,mem,MTU_CONTROLS_ACTION_SETf)) {
                uint32 mtu = 0;

                soc_format_field32_set(unit, MTU_CONTROLS_ACTION_SETfmt,
                                       &mtu, MTU_VALUEf,info->mtu);
                soc_format_field32_set(unit, MTU_CONTROLS_ACTION_SETfmt,
                                       &mtu, MTU_ENABLEf,info->mtu? 1 : 0);
                soc_mem_field32_set(unit, mem, entry,
                                    MTU_CONTROLS_ACTION_SETf, mtu);
            }
        }

        if (info->valid_elements & BCM_FLOW_PORT_ENCAP_FLEX_DATA_VALID) {
            rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, data_ids, &cnt);
            BCM_IF_ERROR_RETURN(rv);
            for (i = 0; i < num_of_fields; i++) {
                for (j = 0; j < cnt; j++) {
                    if (field[i].id == data_ids[j]) {
                        soc_mem_field32_set(unit,mem, entry,
                              field[i].id, field[i].value);
                        break;
                    }
                }
            }
        }
    }
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, dvp, entry);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/*
 * Function:
 *      _bcm_flow_nh_cnt_dec
 * Purpose:
 *      Decrease vxlan port's nexthop count.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      vp    - (IN) Virtual port number.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_nh_cnt_dec(int unit, int vp)
{
    int nh_ecmp_index = -1;
    ing_dvp_table_entry_t dvp;
    uint32  flags = 0;
    int  ref_count = 0;
    int ecmp = 0;

    BCM_IF_ERROR_RETURN(
        READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (ecmp) {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
        flags = BCM_L3_MULTIPATH;
        /* Decrement reference count */
        BCM_IF_ERROR_RETURN(
             bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
    } else {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
        if(nh_ecmp_index != 0) {
             /* Decrement reference count */
             BCM_IF_ERROR_RETURN(
                  bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_ecmp_member_egress_get_first
 * Purpose:
 *      Modify flow ecmp member object's Egress next hop entry
 * Parameters:
 *      unit - Device Number
 *      ecmp_index - ECMP index
 *      index - poiter to first next hop index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_ecmp_member_egress_get_first(int unit, int ecmp_index, int* index)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size;
    bcm_if_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }
    alloc_size = sizeof(bcm_if_t)*BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_xgs3_l3_egress_multipath_get(unit, ecmp_index, BCM_XGS3_L3_ECMP_MAX(unit),
                                  ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                ecmp_member_array[i])) {
        *index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));
    } else {
        rv = BCM_E_PARAM;
        goto clean_up;
    }
clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_flow_port_egr_nh_get
 * Purpose:
 *      Get FLOW Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_port_egr_nh_get(int unit, bcm_flow_port_encap_t *info)
{
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    bcm_flow_encap_config_t einfo;
    int nh_ecmp_index=0;
    uint32 ecmp=0;
    int vp_type;
    int rv = BCM_E_NONE;
    uint32 action_set = 0;
    uint8 process_sd_tag = 0;
    uint8 process_higig_controls = 0;
    uint8 process_switching_controls = 0;
    int i;
    int vp;
    int data_type;
    uint32 view_id = 0;
    uint32 flex_data_array[_BCM_FLOW_PDD_FIELD_MAX];
    uint32 flex_data_count;
    int old_tpid_idx;
    soc_mem_t mem;
    int num_vp;
    uint32 sd_tag;
 
    vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if ((vp <= 0) || (vp >= num_vp)) {
        return BCM_E_PARAM;
    } 
    mem = EGR_L3_NEXT_HOPm;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    vp_type = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf); 
    if (vp_type == _BCM_FLOW_VXLAN_INGRESS_DEST_VP_TYPE ||
        vp_type == _BCM_FLOW_L2GRE_INGRESS_DEST_VP_TYPE) {
        ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
        if (ecmp) {
            nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                      ECMP_PTRf);
            info->egress_if = nh_ecmp_index + 
                                        BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
            /*get ecmp's first member nexthop index*/
            BCM_IF_ERROR_RETURN(_bcm_flow_ecmp_member_egress_get_first(unit,
                      info->egress_if, &nh_ecmp_index));
        } else {
            nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                       NEXT_HOP_INDEXf);
            /* Extract next hop index from unipath egress object. */
            info->egress_if  =  nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
        }

    } else if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                          VP_TYPEf) == _BCM_FLOW_DEST_VP_TYPE_ACCESS) {
        /* Egress into Access-side, find the tunnel_if */
        BCM_IF_ERROR_RETURN(
            READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, 
                                   NEXT_HOP_INDEXf);
        /* Extract next hop index from unipath egress object. */
        info->egress_if  =  nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                      nh_ecmp_index, &egr_nh));

    data_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);

    if (BCMI_L3_NH_FLEX_VIEW(data_type)) {
        /* Get view id corresponding to the data type. */
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &view_id);
        BCM_IF_ERROR_RETURN(rv);

        /* Get PDD field list corresponding to the view id. */
        rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            _BCM_FLOW_PDD_FIELD_MAX,
                            flex_data_array,
                            &flex_data_count);

        BCM_IF_ERROR_RETURN(rv);

        for (i=0; i<flex_data_count; i++) {
            switch (flex_data_array[i]) {
                case SD_TAG_ACTION_SETf:
                    process_sd_tag = 1;
                    break;
                case HIGIG_CONTROLS_ACTION_SETf:
                    process_higig_controls = 1;
                    break;
                case SWITCHING_CTRLS_ACTION_SETf:
                    process_switching_controls = 1;
                    break;
                default:
                    break;
            }
        }
    } 

    if (BCMI_L3_NH_FLEX_VIEW(data_type) || 
        (data_type == _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW) ||
        (data_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW)) {

        bcm_flow_encap_config_t_init(&einfo);
        /* just to give a flex handle to indicate the flex view */
        if (BCMI_L3_NH_FLEX_VIEW(data_type)) {
            einfo.flow_handle = SOC_FLOW_DB_FLOW_ID_START + 1;
            if (process_sd_tag) {
                sd_tag = soc_mem_field32_get(unit, view_id, &egr_nh,
                              SD_TAG_ACTION_SETf);
            } else {
                /* check other linked tables */
                (void)_bcm_flow_linked_tbl_sd_tag_get(unit, view_id, 
                         (uint32 *)&egr_nh, &sd_tag);
            }
        } else {
            if (data_type != _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
                _bcm_flow_egr_nh_sd_tag_get(unit, mem, (uint32 *)&egr_nh, &sd_tag);
            }
        }

        if (data_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
            rv = _bcm_flow_sd_tag_get_from_entry(unit, &einfo, mem,
                    (uint32 *)&egr_nh, &old_tpid_idx);
        } else {
            rv = _bcm_flow_sd_tag_get(unit, &einfo, sd_tag, &old_tpid_idx);
        }
        BCM_IF_ERROR_RETURN(rv);
        rv = _bcm_flow_port_encap_info_convert(info, &einfo, FALSE);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (BCMI_L3_NH_FLEX_VIEW(data_type)) {
        if (process_switching_controls) {
            int drop;
            action_set = 0;

            action_set = soc_mem_field32_get(unit, view_id, &egr_nh,
                              SWITCHING_CTRLS_ACTION_SETf);
            drop = soc_format_field32_get(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                               &action_set, L2_DROPf);
            if (drop) {
                info->flags |= BCM_FLOW_PORT_ENCAP_FLAG_DROP;
            }
            if (process_higig_controls) {
                action_set = soc_mem_field32_get(unit, view_id, &egr_nh,
                                  HIGIG_CONTROLS_ACTION_SETf);
                if (soc_format_field32_get(unit, HIGIG_CONTROLS_ACTION_SETfmt,
                                       &action_set, HG_HDR_SELf)) {
                    info->flags |= BCM_FLOW_PORT_ENCAP_FLAG_LOCAL;
                }
            }
        }
    } else {
       if (data_type == _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW) {
           if (soc_mem_field32_get(unit, mem, &egr_nh, SD_TAG__BC_DROPf)
              || soc_mem_field32_get(unit, mem, &egr_nh, SD_TAG__UUC_DROPf)
              || soc_mem_field32_get(unit, mem, &egr_nh, SD_TAG__UMC_DROPf)) {
            info->flags |= BCM_FLOW_PORT_ENCAP_FLAG_DROP;
           }
       } else if (data_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
           if (!soc_mem_field32_get(unit, mem, &egr_nh, L3__HG_HDR_SELf)) {
               info->flags |= BCM_FLOW_PORT_ENCAP_FLAG_LOCAL;
           }
       } else if (data_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
           if (soc_mem_field32_get(unit, mem, &egr_nh, L2_OTAG__BC_DROPf)
              || soc_mem_field32_get(unit, mem, &egr_nh, L2_OTAG__UUC_DROPf)
              || soc_mem_field32_get(unit, mem, &egr_nh, L2_OTAG__UMC_DROPf)) {
            info->flags |= BCM_FLOW_PORT_ENCAP_FLAG_DROP;
           }
       }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_flow_ingress_dvp_2_set
 * Purpose:
 *     Set Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  flow_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_flow_ingress_dvp_2_set(int unit, int vp, uint32 mpath_flag,
                   int vp_nh_ecmp_index, bcm_flow_port_encap_t *info,
                   int is_network_port)
{
    ing_dvp_2_table_entry_t dvp;
    int rv = BCM_E_NONE;
    int network_group=0;

    if (info->options & BCM_FLOW_PORT_ENCAP_OPTION_REPLACE) {
        BCM_IF_ERROR_RETURN(
            READ_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    } else {
        sal_memset(&dvp, 0, sizeof(ing_dvp_2_table_entry_t));
    }

    if (info->valid_elements & BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID) {
        if (mpath_flag) {
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                  NEXT_HOP_INDEXf, 0x0);
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                 ECMPf, 0x1);
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                 ECMP_PTRf, vp_nh_ecmp_index);
        } else {
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                 ECMPf, 0x0);
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                 ECMP_PTRf, 0);
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                                 NEXT_HOP_INDEXf, vp_nh_ecmp_index);
        }
   } 

   network_group = info->network_group;
   rv = _bcm_validate_splithorizon_network_group(unit, is_network_port, 
                      &network_group);
   BCM_IF_ERROR_RETURN(rv);

   /* Enable DVP as Network_port */
   if (is_network_port) {
       /* Not setting dvp as network port for RIOT as all
          routed packet gets dropped if overlay SVP and DVP
          both are network ports.
          SVP and DVP as network ports is a valid used case
          but reg file says that this field
          is set when DVP is a TRILL port */
#ifdef BCM_RIOT_SUPPORT
       if ((BCMI_RIOT_IS_ENABLED(unit))) {
           if (soc_feature(unit,
               soc_feature_multiple_split_horizon_group)) {
               soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                       NETWORK_GROUPf, 0x0);
           } else {
               soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                       NETWORK_PORTf, 0x0);
           }
       } else
#endif
       {
           if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
               soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,
                                 network_group);
           } else {
               soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                             NETWORK_PORTf, 0x1);
           }
       }
       if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
           soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                      VP_TYPEf, _BCM_FLOW_L2GRE_INGRESS_DEST_VP_TYPE);
       } else {
           soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                      VP_TYPEf, _BCM_FLOW_VXLAN_INGRESS_DEST_VP_TYPE);
       }
    } else {
        if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, NETWORK_GROUPf,
                            network_group);
        } else {
            soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp, NETWORK_PORTf, 0x0);
        }
        soc_ING_DVP_2_TABLEm_field32_set(unit, &dvp,
                           VP_TYPEf, _BCM_FLOW_DEST_VP_TYPE_ACCESS);
    }

    rv =  WRITE_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
    return rv;
}

/*
 * Function:
 *     _bcm_flow_ingress_dvp_set
 * Purpose:
 *     Set Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  flow_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_flow_ingress_dvp_set(int unit, int vp, uint32 mpath_flag,
                int vp_nh_ecmp_index, bcm_flow_port_encap_t *info, 
                int is_network_port)
{
    ing_dvp_table_entry_t dvp;
    int rv = BCM_E_NONE;

    if (info->options & BCM_FLOW_PORT_ENCAP_OPTION_REPLACE) {
        BCM_IF_ERROR_RETURN(
            READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    } else {
        sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    }

    if (info->valid_elements & BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID) {
        if (mpath_flag) {
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                      NEXT_HOP_INDEXf, 0x0);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                      ECMPf, 0x1);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                      ECMP_PTRf, vp_nh_ecmp_index);
        } else {
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                      ECMPf, 0x0);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                      ECMP_PTRf, 0);
            soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                                 NEXT_HOP_INDEXf, vp_nh_ecmp_index);
        }
    }

   if (is_network_port) {
       if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
           soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                      VP_TYPEf, _BCM_FLOW_VXLAN_INGRESS_DEST_VP_TYPE);
       } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
           soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                      VP_TYPEf, _BCM_FLOW_L2GRE_INGRESS_DEST_VP_TYPE);
       } else {
           soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                      VP_TYPEf, _BCM_FLOW_VXLAN_INGRESS_DEST_VP_TYPE);
      }
   } else {
       soc_ING_DVP_TABLEm_field32_set(unit, &dvp,
                      VP_TYPEf, _BCM_FLOW_DEST_VP_TYPE_ACCESS);
   }

   rv =  WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
   return rv;
}

STATIC int
_bcm_flow_port_encap_info_convert( 
        bcm_flow_port_encap_t *pe_info,
        bcm_flow_encap_config_t *info,
        int port_encap2config)
{
    uint32 pe_vl[] = {BCM_FLOW_PORT_ENCAP_VLAN_VALID,
                      BCM_FLOW_PORT_ENCAP_PKT_PRI_VALID,
                      BCM_FLOW_PORT_ENCAP_PKT_CFI_VALID,
                      0};
    uint32 pe_flg[] = {BCM_FLOW_PORT_ENCAP_FLAG_SERVICE_VLAN_ADD,
                     BCM_FLOW_PORT_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE,
                     BCM_FLOW_PORT_ENCAP_FLAG_VLAN_REPLACE,
                     BCM_FLOW_PORT_ENCAP_FLAG_VLAN_DELETE,
                     BCM_FLOW_PORT_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE,
                     BCM_FLOW_PORT_ENCAP_FLAG_VLAN_PRI_REPLACE, 
                     BCM_FLOW_PORT_ENCAP_FLAG_PRI_REPLACE,
                     BCM_FLOW_PORT_ENCAP_FLAG_TPID_REPLACE,
                     0};
    uint32 e_vl[] = {BCM_FLOW_ENCAP_VLAN_VALID,
                      BCM_FLOW_ENCAP_PKT_PRI_VALID,
                      BCM_FLOW_ENCAP_PKT_CFI_VALID,
                      0};
    uint32 e_flg[] = {BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD,
                    BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE,
                    BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE,
                    BCM_FLOW_ENCAP_FLAG_VLAN_DELETE,
                    BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE,
                    BCM_FLOW_ENCAP_FLAG_VLAN_PRI_REPLACE,
                    BCM_FLOW_ENCAP_FLAG_PRI_REPLACE,
                    BCM_FLOW_ENCAP_FLAG_TPID_REPLACE,
                    0};
    int i;

    if (port_encap2config) {
        i = 0;
        while (pe_vl[i]) {
            if (pe_info->valid_elements & pe_vl[i]) {
                info->valid_elements |= e_vl[i];
            }
            i++;
        }
        i = 0;
        while (pe_flg[i]) {
            if (pe_info->flags & pe_flg[i]) {
                info->flags |= e_flg[i];
            }
            i++;
        }
        info->vlan = pe_info->vlan;
        info->pri = pe_info->pri;
        info->cfi = pe_info->cfi;
        info->tpid = pe_info->tpid;
    } else {   /* reverse */
        i = 0;
        while (e_vl[i]) {
            if (info->valid_elements & e_vl[i]) {
                pe_info->valid_elements |= pe_vl[i];
            }
            i++;
        }
        i = 0;
        while (e_flg[i]) {
            if (info->flags & e_flg[i]) {
                pe_info->flags |= pe_flg[i];
            }
            i++;
        }
        pe_info->vlan = info->vlan;
        pe_info->pri = info->pri;
        pe_info->cfi = info->cfi;
        pe_info->tpid = info->tpid;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_vxlan_ecmp_higig_ppd_select
 * Purpose:
 *      Modify vxlan ecmp member object's higig ppd select
 * Parameters:
 *      unit - Device Number
 *      ecmp_index - ECMP index
 *      flags - vxlan vp flags
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_ecmp_nexthop_entry_set(int unit, int ecmp_index, 
              bcm_flow_port_encap_t *pe_info, int is_network_port)
{
    int i = 0, ecmp_member_count = 0;
    int alloc_size, index;
    bcm_if_t *ecmp_member_array = NULL;
    int rv = BCM_E_NONE;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_index)) {
        return BCM_E_PARAM;
    }

    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    ecmp_member_array = sal_alloc(alloc_size, "ecmp member array");
    if (NULL == ecmp_member_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(ecmp_member_array, 0, alloc_size);

    rv = bcm_xgs3_l3_egress_multipath_get(unit, ecmp_index, 
                               BCM_XGS3_L3_ECMP_MAX(unit),
                              ecmp_member_array, &ecmp_member_count);
    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }

    while(i < ecmp_member_count) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                    ecmp_member_array[i])) {
            index = (ecmp_member_array[i] - BCM_XGS3_EGRESS_IDX_MIN(unit));

        } else {
            rv = BCM_E_PARAM;
            goto clean_up;
        }
        rv = _bcm_flow_nexthop_entry_set(unit, index, 0,
              pe_info, is_network_port);

        if (BCM_FAILURE(rv)) {
            goto clean_up;
        }

        i++;
    }

clean_up:
    if (ecmp_member_array) {
        sal_free(ecmp_member_array);
        ecmp_member_array = NULL;
    }

    return rv;
}

STATIC void
_bcm_flow_egr_nh_sd_tag_set(int unit, soc_mem_t hw_mem, uint32 *entry,
                            uint32 sd_tag)
{
    int tag_format;
    uint32 value;

    tag_format = SD_TAG_DATA_FORMAT_DOUBLEfmt;

    value = soc_format_field32_get(unit,tag_format,&sd_tag, SD_TAG_VIDf);
    soc_mem_field32_set(unit, hw_mem, entry, SD_TAG__SD_TAG_VIDf, value);

    value = soc_format_field32_get(unit,tag_format,&sd_tag, SD_TAG_NEW_PRIf);
    soc_mem_field32_set(unit, hw_mem, entry, SD_TAG__NEW_PRIf, value);
    
    value = soc_format_field32_get(unit,tag_format,&sd_tag, SD_TAG_NEW_CFIf);
    soc_mem_field32_set(unit, hw_mem, entry, SD_TAG__NEW_CFIf, value);
    
    value = soc_format_field32_get(unit,tag_format,&sd_tag, SD_TAG_REMARK_CFIf);
    soc_mem_field32_set(unit, hw_mem, entry, SD_TAG__SD_TAG_REMARK_CFIf, value);
    
    value = soc_format_field32_get(unit,tag_format,&sd_tag, 
                                        SD_TAG_ACTION_IF_NOT_PRESENTf);
    soc_mem_field32_set(unit, hw_mem, entry, 
                             SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf, value);
    
    value = soc_format_field32_get(unit,tag_format,&sd_tag, 
                                        SD_TAG_ACTION_IF_PRESENTf);
    soc_mem_field32_set(unit, hw_mem, entry, 
                             SD_TAG__SD_TAG_ACTION_IF_PRESENTf, value);

    value = soc_format_field32_get(unit,tag_format,&sd_tag, SD_TAG_TPID_INDEXf);
    soc_mem_field32_set(unit, hw_mem, entry, SD_TAG__SD_TAG_TPID_INDEXf, value);
}

STATIC void
_bcm_flow_egr_nh_sd_tag_get(int unit, soc_mem_t hw_mem, uint32 *entry,
                            uint32 *sd_tag)
{
    int tag_format;
    uint32 value;

    tag_format = SD_TAG_DATA_FORMAT_DOUBLEfmt;

    *sd_tag = 0;
    value = soc_mem_field32_get(unit, hw_mem, entry, SD_TAG__SD_TAG_VIDf);
    soc_format_field32_set(unit,tag_format,sd_tag, SD_TAG_VIDf, value);

    value = soc_mem_field32_get(unit, hw_mem, entry, SD_TAG__NEW_PRIf);
    soc_format_field32_set(unit,tag_format,sd_tag, SD_TAG_NEW_PRIf, value);

    value = soc_mem_field32_get(unit, hw_mem, entry, SD_TAG__NEW_CFIf);
    soc_format_field32_set(unit,tag_format,sd_tag, SD_TAG_NEW_CFIf, value);

    value = soc_mem_field32_get(unit, hw_mem, entry,SD_TAG__SD_TAG_REMARK_CFIf);
    soc_format_field32_set(unit,tag_format,sd_tag, SD_TAG_REMARK_CFIf, value);

    value = soc_mem_field32_get(unit, hw_mem, entry,
                                SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
    soc_format_field32_set(unit,tag_format,sd_tag, 
                                SD_TAG_ACTION_IF_NOT_PRESENTf, value);

    value = soc_mem_field32_get(unit, hw_mem, entry,
                                SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
    soc_format_field32_set(unit,tag_format,sd_tag, 
                                SD_TAG_ACTION_IF_PRESENTf, value);

    value = soc_mem_field32_get(unit, hw_mem, entry,SD_TAG__SD_TAG_TPID_INDEXf);
    soc_format_field32_set(unit,tag_format,sd_tag, SD_TAG_TPID_INDEXf, value);
}

STATIC int
_bcm_flow_linked_tbl_sd_tag_set(int unit, soc_mem_t view_id, 
              uint32 *egr_nh, uint32 sd_tag)
{
    uint32 nptr_entry[SOC_MAX_MEM_WORDS];
    uint32 action_set;
    int next_ptr_type;
    int next_ptr;
    int data_type;
    uint32 nptr_view_id;
    soc_mem_t mem;
    int rv = SOC_E_NOT_FOUND;

    if (!SOC_MEM_FIELD_VALID(unit,view_id,NEXT_PTR_ACTION_SETf)) {
        return SOC_E_NOT_FOUND; 
    }
    action_set = soc_mem_field32_get(unit, view_id, egr_nh, 
                                           NEXT_PTR_ACTION_SETf);
    next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                      &action_set, NEXT_PTR_TYPEf);
    next_ptr = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                      &action_set, NEXT_PTRf);

    /* for now only handle VC_AND_SWAP table */
    if (next_ptr_type == 3) { /* VC_AND_SWAP */
        mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;
    } else {
        mem = 0;
    }
   
    if (mem) {
        soc_mem_lock(unit,mem);
        rv =  soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                      next_ptr, nptr_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit,mem);
            return rv;
        }
        data_type = soc_mem_field32_get(unit, mem, nptr_entry, DATA_TYPEf);

        rv = soc_flow_db_mem_to_view_id_get(unit,
                         mem,
                         SOC_FLOW_DB_KEY_TYPE_INVALID,
                         data_type,
                         0,
                         NULL,
                         &nptr_view_id);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit,mem);
            return rv;
        }
        if (SOC_MEM_FIELD_VALID(unit,nptr_view_id, SD_TAG_ACTION_SETf)) {
            soc_mem_field32_set(unit, nptr_view_id, nptr_entry, 
                                SD_TAG_ACTION_SETf, sd_tag);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, next_ptr, nptr_entry);

        }
        soc_mem_unlock(unit,mem);
    }
    return rv; 
}
STATIC int
_bcm_flow_linked_tbl_sd_tag_get(int unit, soc_mem_t view_id, 
              uint32 *egr_nh, uint32 *sd_tag)
{
    uint32 nptr_entry[SOC_MAX_MEM_WORDS];
    uint32 action_set;
    int next_ptr_type;
    int next_ptr;
    int data_type;
    uint32 nptr_view_id;
    soc_mem_t mem;

    *sd_tag = 0;
    if (!SOC_MEM_FIELD_VALID(unit,view_id,NEXT_PTR_ACTION_SETf)) {
        return SOC_E_NOT_FOUND; 
    }
    action_set = soc_mem_field32_get(unit, view_id, egr_nh, 
                                           NEXT_PTR_ACTION_SETf);
    next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                      &action_set, NEXT_PTR_TYPEf);
    next_ptr = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                      &action_set, NEXT_PTRf);

    /* for now only handle VC_AND_SWAP table */
    if (next_ptr_type == 3) { /* VC_AND_SWAP */
        mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;
    } else {
        mem = 0;
    }
   
    if (mem) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                      next_ptr, nptr_entry));
        data_type = soc_mem_field32_get(unit, mem, nptr_entry, DATA_TYPEf);

        SOC_IF_ERROR_RETURN(soc_flow_db_mem_to_view_id_get(unit,
                         mem,
                         SOC_FLOW_DB_KEY_TYPE_INVALID,
                         data_type,
                         0,
                         NULL,
                         &nptr_view_id));
        if (SOC_MEM_FIELD_VALID(unit,nptr_view_id, SD_TAG_ACTION_SETf)) {
            *sd_tag = soc_mem_field32_get(unit, nptr_view_id, nptr_entry, 
                                SD_TAG_ACTION_SETf);
        }
    }
    return SOC_E_NONE; 
}
   

             
/*
 * Function:
 *      _bcm_flow_nexthop_entry_set
 * Purpose:
 *      Modify Egress entry - flex views only
 * Parameters:
 *      unit - Device Number
 *      nh_index - Next Hop Index
 *      new_entry_type - New Entry type
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_nexthop_entry_set(int unit, int nh_index, int drop,
              bcm_flow_port_encap_t *pe_info, int is_network_port)
{
    uint32 egr_nh[SOC_MAX_MEM_WORDS];
    bcm_flow_encap_config_t einfo;
    uint32 sd_tag;
    int tpid_index = -1;
    int vp;
    source_vp_entry_t svp;
    int data_type;

    int rv = BCM_E_NONE;

    int i;
    uint32 action_set = 0;
    uint8 process_sd_tag = 0;
    uint8 process_higig_controls = 0;
    uint8 process_switching_controls = 0;
    uint8 process_next_ptr = 0;
    uint8 is_eline;

    uint32  view_id=0;
    uint32 flex_data_array[_BCM_FLOW_PDD_FIELD_MAX];
    uint32 flex_data_count;

    vp = _SHR_GPORT_FLOW_PORT_ID_GET(pe_info->flow_port);
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_lock(unit,EGR_L3_NEXT_HOPm); 
    rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL,
                      nh_index, egr_nh);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    data_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh, DATA_TYPEf);

    if (BCMI_L3_NH_FLEX_VIEW(data_type)) {
        /* Get view id corresponding to the data type. */
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 EGR_L3_NEXT_HOPm,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &view_id);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Get PDD field list corresponding to the view id. */
        rv = soc_flow_db_mem_view_field_list_get(unit,
                            view_id,
                            SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
                            _BCM_FLOW_PDD_FIELD_MAX,
                            flex_data_array,
                            &flex_data_count);

        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i=0; i<flex_data_count; i++) {
            switch (flex_data_array[i]) {
                case SD_TAG_ACTION_SETf:
                    process_sd_tag = 1;
                    break;
                case HIGIG_CONTROLS_ACTION_SETf:
                    process_higig_controls = 1;
                    break;
                case SWITCHING_CTRLS_ACTION_SETf:
                    process_switching_controls = 1;
                    break;
                case NEXT_PTR_ACTION_SETf:
                    process_next_ptr = 1;
                    break;
                default:
                    break;
            }
        }
    } 

    if (!is_network_port) {  /* access port side */

        if (BCMI_L3_NH_FLEX_VIEW(data_type)) {
            if (process_next_ptr) {
                action_set = 0;
                soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf,
                               BCMI_L3_NH_EGR_NEXT_PTR_TYPE_DVP);

                soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf,
                               vp);

                soc_mem_field32_set(unit, view_id, egr_nh,
                              NEXT_PTR_ACTION_SETf, action_set);
            }
        } else {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                /* should always be L2TAG view */
                if (data_type != _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
                    /* first time setup */
                    sal_memset(egr_nh, 0, sizeof(egr_nh));
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                DATA_TYPEf, _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW);

                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                L2_OTAG__FLOW_SELECT_CTRL_IDf, 0x8);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                L2_OTAG__NEXT_PTR_TYPEf, 0x1);
                    data_type = _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW;
                }
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                L2_OTAG__DVPf, vp);
                if (pe_info->valid_elements & BCM_FLOW_PORT_ENCAP_CLASS_ID_VALID) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            L2_OTAG__CLASS_IDf, pe_info->class_id);
                }
            } else {
                /* should always be SDTAG view */
                if (data_type != _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW) {
                    /* first time setup */
                    sal_memset(egr_nh, 0, sizeof(egr_nh));
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                DATA_TYPEf, _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW);
                }
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                SD_TAG__DVPf, vp);
            }
        } 

        if (pe_info->valid_elements & BCM_FLOW_PORT_ENCAP_FLAGS_VALID) {
            rv = bcmi_esw_flow_vp_is_eline(unit,vp, &is_eline);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
         
            if (!BCMI_L3_NH_FLEX_VIEW(data_type) ) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            L2_OTAG__BC_DROPf, drop ? 1 : 0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            L2_OTAG__UUC_DROPf, drop ? 1 : 0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            L2_OTAG__UMC_DROPf, drop ? 1 : 0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            L2_OTAG__HG_LEARN_OVERRIDEf, is_eline ? 1 : 0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                L2_OTAG__HG_MODIFY_ENABLEf, 0x0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                L2_OTAG__HG_HDR_SELf,
                            pe_info->flags & BCM_FLOW_PORT_ENCAP_FLAG_LOCAL? 0: 1);
                } else {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            SD_TAG__BC_DROPf, drop ? 1 : 0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            SD_TAG__UUC_DROPf, drop ? 1 : 0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            SD_TAG__UMC_DROPf, drop ? 1 : 0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                            SD_TAG__HG_LEARN_OVERRIDEf, is_eline ? 1 : 0);
                    /* HG_MODIFY_ENABLE must be 0x0 for Ingress and Egress Chip */
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                SD_TAG__HG_MODIFY_ENABLEf, 0x0);
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                SD_TAG__HG_HDR_SELf,
                            pe_info->flags & BCM_FLOW_PORT_ENCAP_FLAG_LOCAL? 0: 1);
                }
            } else {
                if (process_switching_controls) {
                    action_set = 0;

                    action_set = soc_mem_field32_get(unit, view_id, egr_nh,
                                  SWITCHING_CTRLS_ACTION_SETf);
                    soc_format_field32_set(unit, SWITCHING_CTRLS_ACTION_SETfmt,
                                   &action_set, L2_DROPf,
                                   drop ? 1: 0 );
                    soc_mem_field32_set(unit, view_id, egr_nh,
                                  SWITCHING_CTRLS_ACTION_SETf, action_set);
                }

                if (process_higig_controls) {
                    action_set = 0;
                    action_set = soc_mem_field32_get(unit, view_id, egr_nh,
                                  HIGIG_CONTROLS_ACTION_SETf);
                    soc_format_field32_set(unit, HIGIG_CONTROLS_ACTION_SETfmt,
                                           &action_set, HG_LEARN_OVERRIDEf,
                                       is_eline ? 1 : 0);
                    soc_mem_field32_set(unit, view_id, egr_nh, 
                                 HIGIG_CONTROLS_ACTION_SETf, action_set);
                }
            }
        }
    } else {   /* network port side. Should only have L3 view  */
        if (!BCMI_L3_NH_FLEX_VIEW(data_type)) {
            if (data_type == _BCM_VXLAN_EGR_NEXT_HOP_L3_VIEW) {
                if (soc_feature(unit, soc_feature_egr_modport_to_nhi)) {
                    if (pe_info->flags & BCM_FLOW_PORT_ENCAP_FLAG_LOCAL) {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                        L3__HG_L2_TUNNEL_ENCAP_ENABLEf, 1);
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                        L3__HG_HDR_SELf, 0);
                    } else {
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                        L3__HG_L2_TUNNEL_ENCAP_ENABLEf, 0);
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                        L3__HG_HDR_SELf, 1);
                    }
                } else {
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                    L3__HG_HDR_SELf, 1);
                }
            }
        } else {  /* flex view */
            if (process_higig_controls) {
                action_set = 0;
                action_set = soc_mem_field32_get(unit, view_id, egr_nh,
                                  HIGIG_CONTROLS_ACTION_SETf);
                soc_format_field32_set(unit, HIGIG_CONTROLS_ACTION_SETfmt,
                     &action_set, HG_HDR_SELf,
                     (pe_info->flags & BCM_FLOW_PORT_ENCAP_FLAG_LOCAL)? 0 : 1);
                soc_mem_field32_set(unit, view_id, egr_nh,
                             HIGIG_CONTROLS_ACTION_SETf, action_set);
            }
        }
    }

    if (pe_info->valid_elements & BCM_FLOW_PORT_ENCAP_FLAGS_VALID) {
        /* SD_TAG handling */
        bcm_flow_encap_config_t_init(&einfo);

        if (pe_info->flags & BCM_FLOW_PORT_ENCAP_FLAG_SERVICE_TAGGED) {
            rv = _bcm_flow_port_encap_info_convert(pe_info, &einfo, TRUE);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
            if (BCMI_L3_NH_FLEX_VIEW(data_type)) {
                /* just to indicate the flex flow */
                einfo.flow_handle = SOC_FLOW_DB_FLOW_ID_START + 1;
            }
            if (data_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
                rv = _bcm_flow_sd_tag_set_to_entry(unit, &einfo,
                            EGR_L3_NEXT_HOPm, egr_nh, &tpid_index);
            } else {
                rv = _bcm_flow_sd_tag_set(unit, &einfo, &sd_tag, &tpid_index);
            }
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
            if (BCMI_L3_NH_FLEX_VIEW(data_type)) {
                if (process_sd_tag) {
                    soc_mem_field32_set(unit, view_id, egr_nh,
                              SD_TAG_ACTION_SETf, sd_tag);
                } else {
                /* check sd-tag should be set in a different table */
                    rv = _bcm_flow_linked_tbl_sd_tag_set(unit, view_id,
                            egr_nh, sd_tag);
                    if (rv != SOC_E_NONE && rv != SOC_E_NOT_FOUND) {
                        if (SOC_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                }
            } else {
                if (!is_network_port) {  /* access port side */
                    if (data_type != _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
                        _bcm_flow_egr_nh_sd_tag_set(unit, EGR_L3_NEXT_HOPm, egr_nh,
                                            sd_tag);
                    }
                }
            }
        }
    }
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, egr_nh);
    soc_mem_unlock(unit,EGR_L3_NEXT_HOPm); 
    return rv;

cleanup:
    soc_mem_unlock(unit,EGR_L3_NEXT_HOPm); 
    if (tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    return rv;    
}

/*
 * Function:
 *      _bcm_flow_port_egr_nh_add
 * Purpose:
 *      Add FLOW Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_port_egr_nh_add(int unit, bcm_flow_port_encap_t *info, 
                          int is_network_port, int *nh_idx, uint32 *mflag)    
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv=BCM_E_NONE;
    uint32 mpath_flag=0;
    int vp_nh_ecmp_index=-1;
    int ref_count = 0;
    int vp;
    int drop;
    int nh_ecmp_index = -1;
    ing_dvp_table_entry_t dvp;

    vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    drop = (info->flags & BCM_FLOW_PORT_ENCAP_FLAG_DROP) ? 1 : 0;
    *nh_idx = 0;
    *mflag  = 0;

    if (info->options & BCM_FLOW_PORT_ENCAP_OPTION_REPLACE) {
        SOC_IF_ERROR_RETURN(
            READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  NEXT_HOP_INDEXf);
        if (nh_ecmp_index) {
            BCM_IF_ERROR_RETURN(_bcm_flow_nh_cnt_dec(unit, vp));
        }
    }

    /*
     * Get egress next-hop index from egress object and
     * increment egress object reference count. multicast will be entirely
     * handled in bcm_l3_egress_create API context.
     */
    rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egress_if,
                                     &mpath_flag, 1, &vp_nh_ecmp_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Read the existing egress next_hop entry */
    if (mpath_flag == 0) {
        if (is_network_port) {
            int l3_flag = BCM_XGS3_L3_ENT_FLAG(
               BCM_XGS3_L3_TBL_PTR(unit, next_hop), vp_nh_ecmp_index);
            if (l3_flag != _BCM_L3_FLOW_ONLY) {
                rv = BCM_E_CONFIG;
                _BCM_FLOW_CLEANUP(rv)
            }
        }
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                          vp_nh_ecmp_index, &egr_nh);
        _BCM_FLOW_CLEANUP(rv)
    }
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    else {
        /*
         * When Multi-level ECMP is enabled, ECMP group pointed
         * to by DVP must be an underlay group.
         */
        if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
                if (vp_nh_ecmp_index >= BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {
                    return BCM_E_PARAM;
                }
            } else
#endif
                if (vp_nh_ecmp_index < BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {
                    return BCM_E_PARAM;
                }
        }
    }
#endif

    *nh_idx = vp_nh_ecmp_index;
    *mflag  = mpath_flag;
    if (is_network_port) {
        if (mpath_flag == 0) {
            rv = _bcm_flow_nexthop_entry_set(unit, vp_nh_ecmp_index, drop,
              info, is_network_port);
            _BCM_FLOW_CLEANUP(rv)
        } else {
            rv = _bcm_flow_ecmp_nexthop_entry_set(unit, info->egress_if, 
                                 info, is_network_port);
            _BCM_FLOW_CLEANUP(rv)
        }
    } else {    /* Egress into Access - Non Tunnel */
        rv = _bcm_flow_nexthop_entry_set(unit, vp_nh_ecmp_index, drop,
              info, is_network_port);
        _BCM_FLOW_CLEANUP(rv)
    }

#ifdef BCM_RIOT_SUPPORT
    if (info->options & BCM_FLOW_PORT_ENCAP_OPTION_REPLACE) {
        rv = bcmi_l3_nh_assoc_ol_ul_link_replace(unit, nh_ecmp_index,
            vp_nh_ecmp_index);
    }
#endif /* BCM_RIOT_SUPPORT */
    return rv;

cleanup:
    if (vp_nh_ecmp_index != -1) {
        /* Decrement reference count */
        (void) bcm_xgs3_get_ref_count_from_nhi(
            unit, mpath_flag, &ref_count, vp_nh_ecmp_index);
    }
    return rv;
}

/*
 * Function:
 *           _bcm_flow_port_egr_nh_reset
 * Purpose:
 *           Reset VXLAN Egress NextHop
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

STATIC int
_bcm_flow_port_egr_nh_reset(int unit, int nh_index, int vp_type)
{
    egr_l3_next_hop_entry_t egr_nh;
    int rv=BCM_E_NONE;
    int action_present=0, action_not_present=0, old_tpid_idx = -1;
    uint32 view_id;
    uint32 action_set;
    uint32 entry_type;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                 MEM_BLOCK_ANY, nh_index, &egr_nh));
    entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, DATA_TYPEf);

    if (BCMI_L3_NH_FLEX_VIEW(entry_type)) {
        /* Get view id corresponding to the entry type. */
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 EGR_L3_NEXT_HOPm,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 entry_type,
                                 0,
                                 NULL,
                                 &view_id);

        if (!SOC_MEM_FIELD_VALID(unit, view_id, SD_TAG_ACTION_SETf)) {
            return rv;
        }

        action_set =  soc_mem_field32_get(unit, view_id, &egr_nh,
                              SD_TAG_ACTION_SETf);

        if (vp_type == _BCM_FLOW_DEST_VP_TYPE_ACCESS) {
            action_present =
                soc_format_field32_get(unit, SD_TAG_ACTION_SETfmt,
                           &action_set, SD_TAG_ACTION_IF_PRESENTf);
            action_not_present =
                soc_format_field32_get(unit, SD_TAG_ACTION_SETfmt,
                           &action_set, SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1) ||
                (action_present == 0x4) || (action_present == 0x7)) {
                old_tpid_idx =
                     soc_format_field32_get(unit, SD_TAG_ACTION_SETfmt,
                           &action_set, SD_TAG_TPID_INDEXf);
                BCM_IF_ERROR_RETURN(
                    _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx));
            }
            soc_mem_field32_set(unit, view_id, &egr_nh, SD_TAG_ACTION_SETf,0);
        } else {  /* XXXX reset HG_HDR */
        }
    } else {  /* fixed view */
        if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_SDTAG_VIEW) {
            action_present =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
            action_not_present =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1) ||
                (action_present == 0x4) || (action_present == 0x7)) {
                old_tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf);
                BCM_IF_ERROR_RETURN(
                    _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx));
            }
            /* Normalize Next-hop Entry to L3 View */
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, DATA_TYPEf, 0);
        } else if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
            uint32 profile_index;

            profile_index = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, L2_OTAG__TAG_ACTION_PROFILE_PTRf);
            BCM_IF_ERROR_RETURN(_bcm_td3_sd_tag_action_get(unit, profile_index,
                    &action_present, &action_not_present));
            if (profile_index != 0) {
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                        profile_index));
            }
            if ((action_not_present == 0x1) || (action_present == 0x1) ||
                (action_present == 0x4) || (action_present == 0x7)) {
                old_tpid_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit,
                    &egr_nh, L2_OTAG__TPID_INDEXf);
                BCM_IF_ERROR_RETURN(
                    _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx));
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, DATA_TYPEf, 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L2_OTAG__FLOW_SELECT_CTRL_IDf, 0x0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                        L2_OTAG__NEXT_PTR_TYPEf, 0x0);
        } else { /* L3 view */
        }
    }

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                 MEM_BLOCK_ANY, nh_index, &egr_nh));

    return rv;
}

/*
 * Function:
 *      _bcm_flow_port_egr_nh_delete
 * Purpose:
 *      Delete FLOW Next Hop entry
 * Parameters:
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_port_egr_nh_delete(int unit, int vp)
{
    int rv=BCM_E_NONE;
    int nh_ecmp_index=-1;
    ing_dvp_table_entry_t dvp;
    uint32  vp_type=0;
    uint32  flags=0;
    int  ref_count=0;
    int ecmp =-1;

    BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    vp_type = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf);
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (ecmp) {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
        flags = BCM_L3_MULTIPATH;
        /* Decrement reference count */
        BCM_IF_ERROR_RETURN(bcm_xgs3_get_ref_count_from_nhi(unit, flags, 
                                 &ref_count, nh_ecmp_index));
        if (ref_count == 1) {
            if (nh_ecmp_index &&
                ((vp_type == _BCM_FLOW_VXLAN_INGRESS_DEST_VP_TYPE) ||
                 (vp_type == _BCM_FLOW_L2GRE_INGRESS_DEST_VP_TYPE))) {
                /* support ecmp on network port only 
                 * only deal with the field in L3_VIEW changed by this API
                 * That is the HG_HDR XXXX
                 */
            }
        }
    } else {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
        if(nh_ecmp_index != 0) {
             /* Decrement reference count */
             BCM_IF_ERROR_RETURN(bcm_xgs3_get_ref_count_from_nhi(unit, flags, 
                                  &ref_count, nh_ecmp_index));
        }
        if (ref_count == 1) {
            if (nh_ecmp_index) {
                BCM_IF_ERROR_RETURN(_bcm_flow_port_egr_nh_reset(
                                  unit, nh_ecmp_index, vp_type));
            }
        }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_flow_ingress_dvp_reset
 * Purpose:
 *     Reset Ingress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_flow_ingress_dvp_reset(int unit, int vp)
{
    int rv=BCM_E_UNAVAIL;
    ing_dvp_table_entry_t dvp;
    ing_dvp_2_table_entry_t dvp_2;


    sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    rv =  WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);
    sal_memset(&dvp_2, 0, sizeof(ing_dvp_2_table_entry_t));
    rv =  WRITE_ING_DVP_2_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_2);

    return rv;
}

/*
 * Function:
 *      _bcm_flow_egress_dvp_reset
 * Purpose:
 *      Reet Egress DVP tables
 * Parameters:
 *      IN :  Unit
 *           IN :  vp
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_egress_dvp_reset(int unit, int vp)
{
    uint32 dvp[SOC_MAX_MEM_WORDS];

    sal_memset(dvp, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit,
            EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ALL, vp, dvp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_port_encap_set
 * Purpose:
 *      bind the tunnel object and egress object to the given dvp and cofigure
 * the dvp's tunnel attributes if it is L2 tunnel. For L3 tunnel, bind the
 * egress object with the given tunnel.
 * 
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow egress encap descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_port_encap_set(
    int unit,
    bcm_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int dvp;
    int rv;
    int network_port = 0;
    _bcm_vp_info_t vp_info;
    int nh_idx = 0;
    uint32 mpath_flag = 0;
    int num_vp;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if ((dvp <= 0) || (dvp >= num_vp)) {
        return BCM_E_PARAM;
    } 
    bcmi_esw_flow_lock(unit);
    if (info->options & BCM_FLOW_PORT_ENCAP_OPTION_CLEAR) {
        if (_BCM_FLOW_EGR_DVP_USED_GET(unit,dvp)) {
            _BCM_FLOW_EGR_DVP_USED_CLR(unit,dvp);
            rv = _bcm_flow_port_egr_nh_delete(unit,dvp);
            if (BCM_SUCCESS(rv)) {
                rv = _bcm_flow_egress_dvp_reset(unit, dvp);
                if (BCM_SUCCESS(rv)) {
                    rv = _bcm_flow_ingress_dvp_reset(unit, dvp);
                }
            }
        } else {
            rv = BCM_E_NOT_FOUND;
        }
        bcmi_esw_flow_unlock(unit);
        SOC_IF_ERROR_RETURN(rv);
        return rv;
    }
 
    if (!_bcm_vp_used_get(unit, dvp, _bcmVpTypeFlow)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
          "DVP %d is not flow port, create flow port first\n"), dvp));
        bcmi_esw_flow_unlock(unit);
        return BCM_E_NOT_FOUND;
    }

    /* Check for Network-Port */
    rv = _bcm_vp_info_get(unit, dvp, &vp_info);
    _BCM_FLOW_CLEANUP(rv);
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port = TRUE;
    } else {
        network_port = FALSE;
    }

    if (info->valid_elements & BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID) {
        rv = _bcm_flow_port_egr_nh_add(unit, info, network_port, &nh_idx, 
                                   &mpath_flag);
        _BCM_FLOW_CLEANUP(rv);
    }
    if (network_port) {
        rv = _bcm_flow_egress_network_dvp_set(unit, info, nh_idx, field, 
                    num_of_fields);
        _BCM_FLOW_CLEANUP(rv);

        /* Program DVP entry  - Ingress  */
        rv = _bcm_flow_ingress_dvp_set(unit, dvp, mpath_flag,
                     nh_idx, info, network_port);
        _BCM_FLOW_CLEANUP(rv);

        rv = _bcm_flow_ingress_dvp_2_set(unit, dvp, mpath_flag,
                    nh_idx, info, network_port);
        _BCM_FLOW_CLEANUP(rv);

    } else {
        rv = _bcm_flow_egress_access_dvp_set(unit, info, field, num_of_fields);
        _BCM_FLOW_CLEANUP(rv);

        /* Program DVP entry  with ECMP / Next Hop Index */
        rv = _bcm_flow_ingress_dvp_set(unit, dvp, mpath_flag,
                       nh_idx, info, network_port);
        _BCM_FLOW_CLEANUP(rv);

        rv = _bcm_flow_ingress_dvp_2_set(unit, dvp, mpath_flag,
                               nh_idx, info, network_port);
        _BCM_FLOW_CLEANUP(rv);
    }

    if (BCM_SUCCESS(rv)) {
        /* indicates the dvp is set up */
        _BCM_FLOW_EGR_DVP_USED_SET(unit,dvp);
    }

cleanup:
    bcmi_esw_flow_unlock(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_port_encap_get
 * Purpose:
 *      Get the user configuration parameters from the given object. For L2
   tunnel, it is dvp. For L3 tunnel, it is egress object id. 
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) Flow egress encap descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN/OUT) logical field array
 * Returns:
 *      BCM_E_XXX
 */


int bcmi_esw_flow_port_encap_get(
    int unit,
    bcm_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int dvp;
    int num_vp;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if ((dvp <= 0) || (dvp >= num_vp)) {
        return BCM_E_PARAM;
    } 
    if (!_bcm_vp_used_get(unit, dvp, _bcmVpTypeFlow)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
          "DVP %d is not flow port, create flow port first\n"), dvp));
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_flow_port_egr_nh_get(unit, info));
    BCM_IF_ERROR_RETURN(_bcm_flow_egress_dvp_get(unit,
                       info, field, num_of_fields));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_encap_fixed_view_entry_init
 * Purpose:
 *      initialize the fixed view memory entry
 * Parameters:
 *      unit - (IN) Device Number
 *      mem  - (IN) memory id
 *      info - (IN) flow encap configuration info
 *      key_type - (IN) the fixed key type
 *      entry    - (OUT) point to the entry to be initialized
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_encap_fixed_view_entry_init(int unit,
        soc_mem_t mem,
        uint32 key_type,
        uint32 *entry)
{
    int rv = BCM_E_NONE;

    soc_mem_field32_set(unit,mem, entry,  KEY_TYPEf,  key_type);
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        if ((key_type == _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI) ||
            (key_type == _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI_DVP)) {
                soc_mem_field32_set(unit, mem, entry, DATA_TYPEf, 23);
        } else if (key_type == _BCM_FLOW_ENCAP_KEY_TYPE_VFI_DVP_GROUP) {
            soc_mem_field32_set(unit, mem, entry, DATA_TYPEf, 21);
        } else {
            soc_mem_field32_set(unit, mem, entry, DATA_TYPEf, key_type);
        }
    } else {
        soc_mem_field32_set(unit, mem, entry, DATA_TYPEf, key_type);
    }
    if (soc_mem_field_valid(unit, mem, BASE_VALIDf)) {
        soc_mem_field32_set(unit, mem, entry, BASE_VALIDf, 1);
    } else {
        soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_flow_encap_entry_key_set
 * Purpose:
 *      Set flow encap Entry Key
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN) flow_match configuration info
 *      field - (IN) array of logical fields
 *      num_of_fields -(IN) the size of the above array
 *      mem - (IN) memory id
 *      entry - (IN) point to the match entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC
int _bcm_flow_encap_entry_key_set(int unit,
    bcm_flow_encap_config_t *info,
    bcm_flow_logical_field_t *field,
    int num_of_fields,
    soc_mem_t mem_id,
    uint32 *entry,
    int network_port)
{
    int i;
    int j;
    soc_field_t vfi_f = 0;
    soc_field_t vrf_f = 0;
    soc_field_t dvp_f = 0;
    soc_field_t dvp_grp_f = 0;
    soc_field_t oif_grp_f = 0;
    int rv;
    uint32 dvp = 0;
    uint32 vfi = 0;
    uint32 vrf = 0;

#if defined(INCLUDE_XFLOW_MACSEC) || defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    int                mod_port_index;
#endif
    /*
     * BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP
     *   if network_port 
     *      VFI_DVP_GROUP__DATA_TYPE = 0 tunnel data
     *   else access DVP
     *      VFI_DVP_GROUP__DATA_TYPE = 1 vlan_xlate
     *
     * BCM_FLOW_ENCAP_CRITERIA_VFI_DVP
     *   if network_port 
     *     regular match
     *   else access DVP
     *     VLAN_XLATE_VFI  key_type = 12
     */

    /* validate parameters */

    if (info->valid_elements & BCM_FLOW_ENCAP_FLOW_PORT_VALID) {
        if (!BCM_GPORT_IS_MODPORT(info->flow_port)) {
            dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        }
    }
    if (info->valid_elements & BCM_FLOW_ENCAP_VPN_VALID) {
        if (_BCM_VPN_IS_L3(info->vpn)) {
            _BCM_FLOW_VPN_GET(vrf, _BCM_VPN_TYPE_L3, info->vpn);
        } else {
            BCM_IF_ERROR_RETURN(
                  (bcmi_esw_flow_vpn_is_valid(unit, info->vpn)));
             _BCM_FLOW_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, info->vpn);
        }
    }

    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_DVP) {
        if (!dvp) {
             return BCM_E_PARAM;
        }
        /* only apply to flex view */
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            return BCM_E_PARAM;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem_id,DVPf)) {
            soc_mem_field32_set(unit,mem_id, entry, DVPf, dvp);
        } else {
            LOG_VERBOSE(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
             "DVPf not in memory view definition\n")));
            return BCM_E_PARAM;
        }

    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_L3_INTF) {
        
        if (info->valid_elements & BCM_FLOW_ENCAP_INTF_VALID) {
        
            /* only apply to flex view */
            if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                return BCM_E_PARAM;
            }
            if (SOC_MEM_FIELD_VALID(unit,mem_id,L3_OIFf)) {
                soc_mem_field32_set(unit,mem_id, entry, L3_OIFf, info->intf_id);
            } else {
                LOG_VERBOSE(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                 "L3_OIFf not in memory view definition\n")));
                return BCM_E_PARAM;
            }
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
        /* KEY : VRF,VT_DVP_GROUP_ID,VT_L3_OIF_GROUP_ID
         * VT_DVP_GROUP_ID    : from EGR_DVP_ATTRIBUTEm
         * VT_L3_OIF_GROUP_ID : from EGR_L3_INTFm
         */
        _BCM_FLOW_VPN_GET(vrf, _BCM_VPN_TYPE_L3, info->vpn);
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            vrf_f = VRFf;
            dvp_grp_f = VT_DVP_GROUP_IDf;
            oif_grp_f = VT_L3OIF_GROUP_IDf;
        } else {
            vrf_f = VRF_MAPPING__VRFf;
            dvp_grp_f = VRF_MAPPING__VT_DVP_GROUP_IDf;
            oif_grp_f = VRF_MAPPING__VT_L3OIF_GROUP_IDf;
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_VPN_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, vrf_f) ||
                    (vrf > SOC_VRF_MAX(unit))) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, vrf_f, vrf);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_DVP_GROUP_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, dvp_grp_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, dvp_grp_f, info->dvp_group);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_OIF_GROUP_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, oif_grp_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, oif_grp_f, info->oif_group);
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            vfi_f = VFIf;
            dvp_grp_f = VT_DVP_GROUP_IDf;
        } else {
            vfi_f = VFI_DVP_GROUP__VFIf;
            dvp_grp_f = VFI_DVP_GROUP__VT_DVP_GROUP_IDf;
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_VPN_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, vfi_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, vfi_f, vfi);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_DVP_GROUP_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, dvp_grp_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, dvp_grp_f, info->dvp_group);
        }
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, 
                                 VFI_DVP_GROUP__DATA_OVERLAY_TYPEf)) {
                return BCM_E_PARAM;
            }
            if (network_port) {
                soc_mem_field32_set(unit,mem_id, entry, 
                                 VFI_DVP_GROUP__DATA_OVERLAY_TYPEf, 0);
            } else { /* access DVP */                
                soc_mem_field32_set(unit,mem_id, entry, 
                                 VFI_DVP_GROUP__DATA_OVERLAY_TYPEf, 1);
            }
        }    
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            vfi_f = VFIf;
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    vfi_f = VXLAN_VFI_FLEX__VFIf;
                } else {
                    vfi_f = VXLAN_VFI__VFIf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vfi_f  = L2GRE_VFI__VFIf;
            } else {
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_VPN_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, vfi_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, vfi_f, vfi);
        }
    }  else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {

        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            vfi_f = VFIf;
            dvp_f = DVPf;
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    vfi_f = VXLAN_VFI_FLEX__VFIf;
                    dvp_f = VXLAN_VFI_FLEX__DVPf;
                } else {
                    vfi_f = VXLAN_VFI__VFIf;
                    dvp_f = VXLAN_VFI__DVPf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vfi_f = L2GRE_VFI__VFIf;
                dvp_f = L2GRE_VFI__DVPf;
            } else {
                vfi_f = VLAN_XLATE_VFI__VFIf;
                dvp_f = VLAN_XLATE_VFI__DVPf;
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_VPN_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,vfi_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem_id, entry, vfi_f, vfi);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_FLOW_PORT_VALID) {
            if (!dvp) {
                return BCM_E_PARAM;
            }
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,dvp_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem_id, entry, dvp_f, dvp);
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_FLEX) {
        uint32 key_ids[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
        uint32 cnt;

        rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem_id,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                      _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, key_ids, &cnt);
        BCM_IF_ERROR_RETURN(rv);
        for (i = 0; i < num_of_fields; i++) {
            for (j = 0; j < cnt; j++) {
                if (field[i].id == key_ids[j]) {
                    soc_mem_field32_set(unit,mem_id, entry,
                          field[i].id, field[i].value);
                    break;
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_FLOW_PORT_VALID) {
            if (BCM_GPORT_IS_MODPORT(info->flow_port)) {
                if (!SOC_MEM_FIELD_VALID(unit, mem_id, DGPPf)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit,mem_id, entry, DGPPf, 
                  (_SHR_GPORT_MODPORT_MODID_GET(info->flow_port) << 
                   _BCM_FLOW_MODID_SHIFTER) |
                  _SHR_GPORT_MODPORT_PORT_GET(info->flow_port));
            } else { 
                if (!dvp) {
                    return BCM_E_PARAM;
                }
                if (!SOC_MEM_FIELD_VALID(unit, mem_id, DVPf)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit,mem_id, entry, DVPf, dvp);
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_INT_PRI_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem_id, INT_PRIf)) {
                soc_mem_field32_set(unit, mem_id, entry, INT_PRIf, info->pri);
            } else {
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_VPN_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem_id, VFIf)) {
                soc_mem_field32_set(unit,mem_id, entry, VFIf, vfi);
            } else if (SOC_MEM_FIELD_VALID(unit, mem_id, VRF_IDf)) {
                soc_mem_field32_set(unit,mem_id, entry, VRF_IDf, vrf);
            } else {
                return BCM_E_PARAM;
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_INTF_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, L3_OIFf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, L3_OIFf, info->intf_id);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_DVP_GROUP_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, VT_DVP_GROUP_IDf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, VT_DVP_GROUP_IDf, 
                    info->dvp_group);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_OIF_GROUP_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, VT_L3OIF_GROUP_IDf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, VT_L3OIF_GROUP_IDf, 
                    info->oif_group);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_SRC_FLOW_PORT_VALID) {
            uint32 svp = 0;
            if (BCM_GPORT_IS_FLOW_PORT(info->src_flow_port)) {
                svp = _SHR_GPORT_FLOW_PORT_ID_GET(info->src_flow_port);
            } else {
                return BCM_E_PARAM;
            }
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, SVPf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, SVPf, svp);
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_CLASS_ID_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, mem_id, CLASS_IDf)) {
                soc_mem_field32_set(unit, mem_id, entry, CLASS_IDf, info->class_id);
            } else if (SOC_MEM_FIELD_VALID(unit, mem_id, CLASS_ID_ACTION_SETf)) {
                soc_mem_field32_set(unit, mem_id, entry, CLASS_ID_ACTION_SETf, info->class_id);
            } else {
                return BCM_E_PARAM;
            }
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_SVP_DVP) {
        if (soc_feature(unit, soc_feature_vxlan_evpn)) {
            if (!dvp) {
                return BCM_E_PARAM;
            }
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, DVPf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, DVPf, dvp);

            if (info->valid_elements & BCM_FLOW_ENCAP_SRC_FLOW_PORT_VALID) {
                uint32 svp = 0;

                if (BCM_GPORT_IS_FLOW_PORT(info->src_flow_port)) {
                    svp = _SHR_GPORT_FLOW_PORT_ID_GET(info->src_flow_port);
                } else {
                    return BCM_E_PARAM;
                }

                if (!SOC_MEM_FIELD_VALID(unit, mem_id, SVPf)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit,mem_id, entry, SVPf, svp);
            }
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_DGPP_INT_PRI) {
#if defined(INCLUDE_XFLOW_MACSEC) || defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if (soc_feature(unit, soc_feature_xflow_macsec) ||
            (soc_feature(unit, soc_feature_channelized_switching))) {
            /* only apply to flex view */
            if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                return BCM_E_PARAM;
            }
            if (info->valid_elements & BCM_FLOW_ENCAP_FLOW_PORT_VALID) {
                if ((BCM_GPORT_IS_MODPORT(info->flow_port)) ||
                     (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port))) {
                    if (!SOC_MEM_FIELD_VALID(unit, mem_id, DGPPf)) {
                        return BCM_E_PARAM;
                    }
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
                    if (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port)) {
                        soc_mem_field32_set(unit,mem_id, entry, DGPPf,
                            (_BCM_SUBPORT_COE_PORT_ID_MOD_GET(info->flow_port) <<
                             _BCM_FLOW_MODID_SHIFTER) |
                            _BCM_SUBPORT_COE_PORT_ID_PORT_GET(info->flow_port));

                    } else
#endif
                    {
                        soc_mem_field32_set(unit,mem_id, entry, DGPPf,
                            (_SHR_GPORT_MODPORT_MODID_GET(info->flow_port) <<
                             _BCM_FLOW_MODID_SHIFTER) |
                            _SHR_GPORT_MODPORT_PORT_GET(info->flow_port));
                   }
                }
            }
            if (info->valid_elements & BCM_FLOW_ENCAP_INT_PRI_VALID) {
                if (SOC_MEM_FIELD_VALID(unit,mem_id,INT_PRIf)) {
                    soc_mem_field32_set(unit,mem_id, entry, INT_PRIf, info->pri);
                } else {
                    return BCM_E_PARAM;
                }
            }
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
            if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port))) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_src_mod_port_table_index_get_from_port(
                        unit, info->flow_port, &mod_port_index));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_src_mod_port_table_index_get_from_port(
                        unit, (_SHR_GPORT_MODPORT_MODID_GET(info->flow_port) <<
                            _BCM_FLOW_MODID_SHIFTER) |
                        _SHR_GPORT_MODPORT_PORT_GET(info->flow_port), &mod_port_index));
            }

            /* check CMH_FORMAT for  MACSEC_ENCRYPT_RX_KEY_DGPP_INTPRI */
            SOC_IF_ERROR_RETURN(
                    soc_mem_field32_modify(unit, EGR_GPP_ATTRIBUTESm, mod_port_index,
                        VXLT_CTRL_IDf, 64));
        } else
#endif
        {
            return BCM_E_UNAVAIL;
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI) {
#if defined (INCLUDE_XFLOW_MACSEC) || defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if ((soc_feature(unit, soc_feature_xflow_macsec)) ||
            (soc_feature(unit, soc_feature_channelized_switching))) {
            /* only apply to flex view */
            if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                return BCM_E_PARAM;
            }
            if (info->valid_elements & BCM_FLOW_ENCAP_FLOW_PORT_VALID) {
                if ((BCM_GPORT_IS_MODPORT(info->flow_port)) ||
                     (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port))) {
                    if (!SOC_MEM_FIELD_VALID(unit, mem_id, DGPPf)) {
                        return BCM_E_PARAM;
                    }
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
                    if (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port)) {
                        soc_mem_field32_set(unit,mem_id, entry, DGPPf,
                            (_BCM_SUBPORT_COE_PORT_ID_MOD_GET(info->flow_port) <<
                             _BCM_FLOW_MODID_SHIFTER) |
                            _BCM_SUBPORT_COE_PORT_ID_PORT_GET(info->flow_port));

                    } else
#endif
                    {
                        soc_mem_field32_set(unit,mem_id, entry, DGPPf,
                            (_SHR_GPORT_MODPORT_MODID_GET(info->flow_port) <<
                             _BCM_FLOW_MODID_SHIFTER) |
                            _SHR_GPORT_MODPORT_PORT_GET(info->flow_port));
                    }
                }
            }
            if (info->valid_elements & BCM_FLOW_ENCAP_VLAN_VALID) {
                if (info->vlan >= BCM_VLAN_INVALID) {
                    return  BCM_E_PARAM;
                }
                if (SOC_MEM_FIELD_VALID(unit,mem_id,OUTER_VLAN_IDf)) {
                    soc_mem_field32_set(unit,mem_id, entry, OUTER_VLAN_IDf, info->vlan);
                } else {
                    return BCM_E_PARAM;
                }
                if (SOC_MEM_FIELD_VALID(unit,mem_id,OTAG_STATUSf)) {
                    soc_mem_field32_set(unit,mem_id, entry, OTAG_STATUSf, 1);
                } else {
                    return BCM_E_PARAM;
                }
            }
            if (info->valid_elements & BCM_FLOW_ENCAP_INT_PRI_VALID) {
                if (SOC_MEM_FIELD_VALID(unit,mem_id,INT_PRIf)) {
                    soc_mem_field32_set(unit,mem_id, entry, INT_PRIf, info->pri);
                } else {
                    return BCM_E_PARAM;
                }
            }
#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT
            if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port))) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_src_mod_port_table_index_get_from_port(
                        unit, info->flow_port, &mod_port_index));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_src_mod_port_table_index_get_from_port(
                        unit, (_SHR_GPORT_MODPORT_MODID_GET(info->flow_port) <<
                            _BCM_FLOW_MODID_SHIFTER) |
                        _SHR_GPORT_MODPORT_PORT_GET(info->flow_port), &mod_port_index));
            }
            /* check CMH_FORMAT for  MACSEC_ENCRYPT_RX_KEY_DGPP_VLAN_INTPRI */
            SOC_IF_ERROR_RETURN(
                    soc_mem_field32_modify(unit, EGR_GPP_ATTRIBUTESm, mod_port_index,
                        VXLT_CTRL_IDf, 128));
        } else
#endif
        {
            return BCM_E_UNAVAIL;
        }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_flow_encap_key_set
 * Purpose:
 *      Set the key for a flow encap entry
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN) flow_match configuration info
 *      field - (IN) array of logical fields
 *      num_of_fields -(IN) the size of the above array
 *      entry - (IN) point to the match entry
 *      mem - (OUT) memory id
 *      mem_view - (OUT) memory view id
 * Returns:
 *      BCM_E_XXX
 */

STATIC
int _bcm_flow_encap_key_set(
    int unit,
    bcm_flow_encap_config_t *info,
    bcm_flow_logical_field_t *field,
    uint32 num_of_fields,
    uint32 *entry,
    soc_mem_t *mem,
    soc_mem_t *mem_view,
    int *network_port)
{
    int rv;
    soc_mem_t mem_view_id = 0;
    uint32 key_type = 0;
    soc_mem_t mem_id = 0;
    soc_flow_db_mem_view_info_t vinfo;

    *network_port = FALSE;
    /* validate parameters */

    /* flow port based encap */
    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_DVP ||
        info->criteria == BCM_FLOW_ENCAP_CRITERIA_L3_INTF) {
        /* no fixed view supported */
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit, 
                  "encap criteria not supported in legacy flow\n")));
            return BCM_E_PARAM;
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
        mem_id = EGR_VLAN_XLATE_1_DOUBLEm;
        key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VRF_MAPPING;
        *network_port = TRUE;
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
        mem_id = EGR_VLAN_XLATE_1_DOUBLEm;  /* fixed view */
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem_id = EGR_VLAN_XLATE_1_SINGLEm;
            }
            key_type = _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI;
            *network_port = TRUE;
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            key_type = _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI;
            *network_port = TRUE;
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
        mem_id = EGR_VLAN_XLATE_1_DOUBLEm;  /* fixed view */
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            key_type = _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI_DVP;
            *network_port = TRUE;
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem_id = EGR_VLAN_XLATE_1_SINGLEm;
            }
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            key_type = _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI_DVP;
            *network_port = TRUE;
        } else if (!(info->flow_handle)) {  /* access DVP */
            key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VLAN_XLATE_VFI;
        }
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
        key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VFI_DVP_GROUP;
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN || 
            info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            *network_port = TRUE;
        } else if (!(info->flow_handle)) {  /* access DVP */
            key_type = _BCM_FLOW_ENCAP_KEY_TYPE_VLAN_XLATE_VFI;
        }

        mem_id = EGR_VLAN_XLATE_1_DOUBLEm;  /* fixed view */
    } 
    *mem = mem_id;

    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
        if (!(*mem)) {
            return BCM_E_PARAM;
        }
        rv = _bcm_flow_encap_fixed_view_entry_init(unit, mem_id,
                        key_type, entry);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* get the physical memory view */
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,info->flow_handle,
             info->flow_option,
             SOC_FLOW_DB_FUNC_ENCAP_ID, (uint32 *)&mem_view_id);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo);
        BCM_IF_ERROR_RETURN(rv);
        if (mem_view != NULL) {
            *mem_view = mem_view_id;
        }
        *mem = vinfo.mem;

        /* flex view, initialize all control fields such as data_type */
        soc_flow_db_mem_view_entry_init(unit,mem_view_id,entry);
        mem_id = mem_view_id;
        *network_port = TRUE;
    }

    rv = _bcm_flow_encap_entry_key_set(unit,info,field,num_of_fields,
                        mem_id, entry,*network_port);
    return rv;
}

STATIC int
_bcm_flow_encap_vlan_set(int unit, bcm_flow_encap_config_t *info,
                         soc_mem_t mem,
                         uint32 *entry,
                         uint32 *otag,
                         uint32 *xlate_data)
{
    int xlate_format;
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    xlate_format = EGR_VLAN_XLATE_XLATE_DATA_DOUBLEfmt;

    /* access facing port. Derive vlan tag from VFI/DVP. If VFI is valid
     * in EGR_L3_INTF, then the packet is treated as un-tagged
     */ 
    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP ||
        info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
        if (info->valid_elements & BCM_FLOW_ENCAP_VLAN_VALID) {
            if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
                soc_format_field32_set(unit,OTAG_ACTION_SETfmt,&otag,
                                VIDf,info->vlan);
            } else {
                if (soc_mem_field_valid(unit, mem, VXLAN_VFI_FLEX__OVIDf) &&
                    (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
                    soc_mem_field32_set(unit, mem, entry, VXLAN_VFI_FLEX__OVIDf, info->vlan);
                } else {
                    soc_format_field32_set(unit,xlate_format,
                            xlate_data, NEW_OVIDf,info->vlan);
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_PKT_PRI_VALID) {
            if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
                soc_format_field32_set(unit,OTAG_ACTION_SETfmt,&otag,
                                PCPf,info->pri);
            } else {
                if (soc_mem_field_valid(unit, mem, VXLAN_VFI_FLEX__OPRIf) &&
                    (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
                    soc_mem_field32_set(unit, mem, entry, VXLAN_VFI_FLEX__OPRIf, info->pri);
                } else {
                    soc_format_field32_set(unit,xlate_format,
                        xlate_data, NEW_OPRIf,info->pri);
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_PKT_CFI_VALID) {
            if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
                soc_format_field32_set(unit,OTAG_ACTION_SETfmt,&otag,
                                DEf,info->cfi);
            } else {
                if (soc_mem_field_valid(unit, mem, VXLAN_VFI_FLEX__OCFIf) &&
                    (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
                    soc_mem_field32_set(unit, mem, entry, VXLAN_VFI_FLEX__OCFIf,info->cfi);
                } else {
                soc_format_field32_set(unit,xlate_format,
                        xlate_data, NEW_OCFIf,info->cfi);
                }
            }
        }
        if (info->valid_elements & (BCM_FLOW_ENCAP_VLAN_VALID |
                                    BCM_FLOW_ENCAP_PKT_PRI_VALID |
                                    BCM_FLOW_ENCAP_PKT_CFI_VALID) ) {

            /* action profile */
            bcm_vlan_action_set_t_init(&action);
            action.ut_outer          = bcmVlanActionAdd;
            action.ut_outer_pkt_prio = bcmVlanActionAdd;
            action.ut_outer_cfi      = bcmVlanActionAdd;
            
            BCM_IF_ERROR_RETURN(_bcm_trx_egr_vlan_action_profile_entry_add(
                     unit, &action, &profile_idx));
            if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
                soc_mem_field32_set(unit,mem, entry,
                            TAG_ACTION_PROFILE_ACTION_SETf,profile_idx);
            } else {
                if (soc_mem_field_valid(unit, mem,
                        VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf) &&
                    (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
                    soc_mem_field32_set(unit, mem, entry,
                        VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf, profile_idx);
                } else {
                    soc_format_field32_set(unit,xlate_format,
                            xlate_data, TAG_ACTION_PROFILE_PTRf,profile_idx);
                }
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_encap_vlan_get(int unit, bcm_flow_encap_config_t *info,
                         soc_mem_t hw_mem,
                         uint32 *entry,
                         uint32 *otag,
                         uint32 *xlate_data)
{
    int xlate_format;
    xlate_format = EGR_VLAN_XLATE_XLATE_DATA_DOUBLEfmt;

    /* access facing port. Derive vlan tag from VFI/DVP. If VFI is valid
     * in EGR_L3_INTF, then the packet is treated as un-tagged
     */ 
    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP ||
        info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            info->vlan = soc_format_field32_get(unit,OTAG_ACTION_SETfmt,&otag,
                                                VIDf);
            info->pri = soc_format_field32_get(unit,OTAG_ACTION_SETfmt,&otag,
                                               PCPf);
            info->cfi = soc_format_field32_get(unit,OTAG_ACTION_SETfmt,&otag,
                                               DEf);
        } else {
            if (soc_mem_field_valid(unit, hw_mem, OVIDf)) {
                info->vlan = soc_mem_field32_get(unit,hw_mem, entry, OVIDf);
            } else {
                info->vlan = soc_format_field32_get(unit,xlate_format,
                                         xlate_data, NEW_OVIDf);
            }
            if (soc_mem_field_valid(unit, hw_mem, OPRIf)) {
                info->pri = soc_mem_field32_get(unit,hw_mem, entry, OPRIf);
            } else {
                info->pri = soc_format_field32_get(unit,xlate_format,
                                         xlate_data, NEW_OPRIf);
            }
            if (soc_mem_field_valid(unit, hw_mem, OCFIf)) {
                info->cfi = soc_mem_field32_get(unit,hw_mem, entry, OCFIf);
            } else {
                info->cfi = soc_format_field32_get(unit,xlate_format,
                                         xlate_data, NEW_OCFIf);
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_sd_tag_set(int unit, bcm_flow_encap_config_t *info,
                         uint32 *sd_tag,
                         int *tpid_index)
{
    int action_present;
    int action_not_present;
    int sd_tag_remark_cfi_set = 0;
    int tag_format;
    int old_tpid_inx;

    if (_BCM_FLOW_IS_FLEX_VIEW(info) ||
        (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP)) {
        tag_format = SD_TAG_ACTION_SETfmt;
    } else {
        tag_format = SD_TAG_DATA_FORMAT_DOUBLEfmt;
    }

    action_present = 0;
    action_not_present = 0;

    /* only these two fixed views use specific field names
     * Others use sd_tag format 
     */
    if (info->valid_elements & BCM_FLOW_ENCAP_VLAN_VALID) {
        if (info->vlan >= BCM_VLAN_INVALID) {
            return  BCM_E_PARAM;
        }
        soc_format_field32_set(unit,tag_format,sd_tag,
                            SD_TAG_VIDf,info->vlan);
    }
    if (info->valid_elements & BCM_FLOW_ENCAP_PKT_PRI_VALID) {
        if (info->pri > BCM_PRIO_MAX) {
            return  BCM_E_PARAM;
        }
        soc_format_field32_set(unit,tag_format,sd_tag,
                            SD_TAG_NEW_PRIf,info->pri);
    }
    if (info->valid_elements & BCM_FLOW_ENCAP_PKT_CFI_VALID) {
        if (info->cfi > 1) {
           return  BCM_E_PARAM;
        }
        soc_format_field32_set(unit,tag_format,sd_tag,
                            SD_TAG_NEW_CFIf,info->cfi);
    }

    if (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) {
        sd_tag_remark_cfi_set = TRUE;
        action_not_present = 0x1;  /* ADD */ 
    }

    if (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) {
        action_present = 0x1; /* REPLACE_VID_TPID */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE) {
        action_present = 0x2;  /* REPLACE_VID_ONLY */ 

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_DELETE) {
        action_present = 0x3;  /* DELETE */ 

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x4;  /* REPLACE_VID_PRI_TPID */ 

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_REPLACE){
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x5;  /* REPLACE_VID_PRI */ 

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_PRI_REPLACE ) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x6;  /*  REPLACE_PRI_ONLY */ 

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) {
        action_present = 0x7;  /* REPLACE_TPID_ONLY */ 
    }
    if (sd_tag_remark_cfi_set) {
        soc_format_field32_set(unit,tag_format,sd_tag,
                            SD_TAG_REMARK_CFIf,info->cfi);
    }
    if (action_not_present) {
        soc_format_field32_set(unit,tag_format,sd_tag,
                            SD_TAG_ACTION_IF_NOT_PRESENTf,action_not_present);
    }
    if (action_present) {
        soc_format_field32_set(unit,tag_format,sd_tag,
                            SD_TAG_ACTION_IF_PRESENTf,action_present);
    }

    if ((info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE)) {
        if (info->options & BCM_FLOW_ENCAP_OPTION_REPLACE) {
            old_tpid_inx = soc_format_field32_get(unit,tag_format,sd_tag,
                                SD_TAG_TPID_INDEXf);
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit, 
                                old_tpid_inx));
        }
    
        /* TPID value is used */
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit, info->tpid, 
                             tpid_index));
        soc_format_field32_set(unit,tag_format,sd_tag,
                            SD_TAG_TPID_INDEXf,*tpid_index);
    }
    return  BCM_E_NONE;
}

STATIC int
_bcm_flow_sd_tag_get(int unit, bcm_flow_encap_config_t *info, uint32 sd_tag,
                     int  *tpid_idx)
{
    int action_present;
    int action_not_present;
    int tag_format;
    int tpid_index;
    int rv;
 
    if (_BCM_FLOW_IS_FLEX_VIEW(info) ||
        (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP)) {
        tag_format = SD_TAG_ACTION_SETfmt;
    } else {
        tag_format = SD_TAG_DATA_FORMAT_DOUBLEfmt;
    }
    action_present = 0;
    action_not_present = 0;

    info->vlan = soc_format_field32_get(unit,tag_format,&sd_tag,
                            SD_TAG_VIDf);

    info->pri = soc_format_field32_get(unit,tag_format,&sd_tag,
                            SD_TAG_NEW_PRIf);
    info->cfi = soc_format_field32_get(unit,tag_format,&sd_tag,
                            SD_TAG_NEW_CFIf);
    action_not_present = soc_format_field32_get(unit,tag_format,&sd_tag,
                                   SD_TAG_ACTION_IF_NOT_PRESENTf);
    action_present = soc_format_field32_get(unit,tag_format,&sd_tag,
                                   SD_TAG_ACTION_IF_PRESENTf);
    
    if (action_not_present || action_present) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED;
    }
    if (action_not_present) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD;
    }

    if (action_present == 0x1) {  
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE;

    } else if (action_present == 0x2) {
        info->flags  |= BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE;

    } else if (action_present == 0x3) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_DELETE;

    } else if (action_present == 4) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE;

    } else if (action_present == 0x5) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_PRI_REPLACE;

    } else if (action_present == 0x6) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_PRI_REPLACE;

    } else if (action_present == 0x7) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_TPID_REPLACE;
    }

    if ((info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE)) {

        tpid_index = soc_format_field32_get(unit,tag_format,&sd_tag,
                            SD_TAG_TPID_INDEXf);
        rv = _bcm_fb2_outer_tpid_entry_get(unit,&info->tpid,tpid_index);
        if (BCM_FAILURE(rv)) {
            info->flags |= _BCM_FLOW_ENCAP_FLAG_TPID_INDEX;
            info->tpid = tpid_index;
        }
        if (tpid_idx != NULL) {
            *tpid_idx = tpid_index;
        }
    }
    return  BCM_E_NONE;
}

STATIC int
_bcm_flow_sd_tag_set_to_entry(
        int unit,
        bcm_flow_encap_config_t *info,
        soc_mem_t mem,
        uint32 *entry,
        int *tpid_index)
{
    int action_present = 0;
    int action_not_present = 0;
    int sd_tag_remark_cfi_set = 0;
    int old_tpid_inx = 0;
    uint32 profile_index = 0;
    soc_field_t vlan_f = INVALIDf;
    soc_field_t pri_f = INVALIDf;
    soc_field_t cfi_f = INVALIDf;
    soc_field_t pri_cfi_sel_f = INVALIDf;
    soc_field_t tag_act_f = INVALIDf;
    soc_field_t tpid_f = INVALIDf;

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
           "Fixed view only\n")));
        return BCM_E_PARAM;
    }

    if (mem == EGR_VLAN_XLATE_1_SINGLEm) {
        /* View VXLAN_VFI_FLEX */
        vlan_f = VXLAN_VFI_FLEX__OVIDf;
        pri_f = VXLAN_VFI_FLEX__OPRIf;
        cfi_f = VXLAN_VFI_FLEX__OCFIf;
        pri_cfi_sel_f = VXLAN_VFI_FLEX__OPRI_OCFI_SELf;
        tag_act_f = VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf;
        tpid_f = VXLAN_VFI_FLEX__OUTER_TPID_INDEXf;
    } else if (mem == EGR_L3_NEXT_HOPm) {
        /* View L2_OTAG */
        vlan_f = L2_OTAG__VIDf;
        pri_f = L2_OTAG__PCPf;
        cfi_f = L2_OTAG__DEf;
        pri_cfi_sel_f = L2_OTAG__PCP_DE_SELf;
        tag_act_f = L2_OTAG__TAG_ACTION_PROFILE_PTRf;
        tpid_f = L2_OTAG__TPID_INDEXf;
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
           "Unsupported mem or view: %s\n"), SOC_MEM_UFNAME(unit, mem)));
        return BCM_E_PARAM;
    }

    if (info->valid_elements & BCM_FLOW_ENCAP_VLAN_VALID) {
        if (info->vlan >= BCM_VLAN_INVALID) {
            return  BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, mem, entry, vlan_f, info->vlan);
    }
    if (info->valid_elements & BCM_FLOW_ENCAP_PKT_PRI_VALID) {
        if (info->pri > BCM_PRIO_MAX) {
            return  BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, mem, entry, pri_f, info->pri);
    }
    if (info->valid_elements & BCM_FLOW_ENCAP_PKT_CFI_VALID) {
        if (info->cfi > 1) {
           return  BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, mem, entry, cfi_f, info->cfi);
    }

    if (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) {
        sd_tag_remark_cfi_set = TRUE;
        action_not_present = 0x1;  /* ADD */
    }

    if (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) {
        action_present = 0x1; /* REPLACE_VID_TPID */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE) {
        action_present = 0x2;  /* REPLACE_VID_ONLY */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_DELETE) {
        action_present = 0x3;  /* DELETE */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x4;  /* REPLACE_VID_PRI_TPID */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_REPLACE){
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x5;  /* REPLACE_VID_PRI */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_PRI_REPLACE ) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x6;  /*  REPLACE_PRI_ONLY */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) {
        action_present = 0x7;  /* REPLACE_TPID_ONLY */
    }

    soc_mem_field32_set(unit, mem, entry, pri_cfi_sel_f, sd_tag_remark_cfi_set);
    profile_index = soc_mem_field32_get(unit, mem, entry, tag_act_f);
    _bcm_td3_sd_tag_action_profile_get(unit, action_present,
            action_not_present, &profile_index);
    soc_mem_field32_set(unit, mem, entry, tag_act_f, profile_index);

    if ((info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE)) {
        if (info->options & BCM_FLOW_ENCAP_OPTION_REPLACE) {
            old_tpid_inx = soc_mem_field32_get(unit, mem, entry, tpid_f);
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit,
                                old_tpid_inx));
        }

        /* TPID value is used */
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit, info->tpid,
                             tpid_index));
        soc_mem_field32_set(unit, mem, entry, tpid_f, *tpid_index);
    }
    return  BCM_E_NONE;
}


STATIC int
_bcm_flow_sd_tag_get_from_entry(
        int unit,
        bcm_flow_encap_config_t *info,
        soc_mem_t mem,
        uint32 *entry,
        int  *tpid_idx)
{
    int action_present = 0;
    int action_not_present = 0;
    int tpid_index = 0;
    uint32 profile_index = 0;
    int rv = BCM_E_NONE;
    soc_field_t vlan_f = INVALIDf;
    soc_field_t pri_f = INVALIDf;
    soc_field_t cfi_f = INVALIDf;
    soc_field_t tag_act_f = INVALIDf;
    soc_field_t tpid_f = INVALIDf;

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
           "Fixed view only\n")));
        return BCM_E_PARAM;
    }

    if (mem == EGR_VLAN_XLATE_1_SINGLEm) {
        /* View VXLAN_VFI_FLEX */
        vlan_f = VXLAN_VFI_FLEX__OVIDf;
        pri_f = VXLAN_VFI_FLEX__OPRIf;
        cfi_f = VXLAN_VFI_FLEX__OCFIf;
        tag_act_f = VXLAN_VFI_FLEX__TAG_ACTION_PROFILE_PTRf;
        tpid_f = VXLAN_VFI_FLEX__OUTER_TPID_INDEXf;
    } else if (mem == EGR_L3_NEXT_HOPm) {
        /* View L2_OTAG */
        vlan_f = L2_OTAG__VIDf;
        pri_f = L2_OTAG__PCPf;
        cfi_f = L2_OTAG__DEf;
        tag_act_f = L2_OTAG__TAG_ACTION_PROFILE_PTRf;
        tpid_f = L2_OTAG__TPID_INDEXf;
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
           "Unsupported mem or view: %s\n"), SOC_MEM_UFNAME(unit, mem)));
        return BCM_E_PARAM;
    }

    info->vlan = soc_mem_field32_get(unit, mem, entry, vlan_f);
    info->pri = soc_mem_field32_get(unit, mem, entry, pri_f);
    info->cfi = soc_mem_field32_get(unit, mem, entry, cfi_f);

    profile_index = soc_mem_field32_get(unit, mem, entry, tag_act_f);
    (void)_bcm_td3_sd_tag_action_get(unit, profile_index, &action_present,
                    &action_not_present);

    if (action_not_present || action_present) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED;
    }
    if (action_not_present) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD;
    }

    if (action_present == 0x1) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE;

    } else if (action_present == 0x2) {
        info->flags  |= BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE;

    } else if (action_present == 0x3) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_DELETE;

    } else if (action_present == 4) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE;

    } else if (action_present == 0x5) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_PRI_REPLACE;

    } else if (action_present == 0x6) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_PRI_REPLACE;

    } else if (action_present == 0x7) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_TPID_REPLACE;
    }

    if ((info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE)) {
        tpid_index = soc_mem_field32_get(unit, mem, entry, tpid_f);

        rv = _bcm_fb2_outer_tpid_entry_get(unit, &info->tpid, tpid_index);
        if (BCM_FAILURE(rv)) {
            info->flags |= _BCM_FLOW_ENCAP_FLAG_TPID_INDEX;
            info->tpid = tpid_index;
        }
        if (tpid_idx != NULL) {
            *tpid_idx = tpid_index;
        }
    }
    return  BCM_E_NONE;
}

STATIC int
_bcm_flow_sd_tag_set_to_format(
        int unit,
        bcm_flow_encap_config_t *info,
        soc_mem_t mem,
        uint32 *entry,
        int *tpid_index)
{
    int action_present = 0;
    int action_not_present = 0;
    int sd_tag_remark_cfi_set = 0;
    int old_tpid_inx = 0;
    uint32 profile_index = 0;
    soc_format_t tag_profile_fmt = TAG_ACTION_PROFILE_ACTION_SETfmt;
    soc_format_t tag_act_fmt = OTAG_ACTION_SETfmt;
    soc_field_t tag_profile_fmt_f = TAG_ACTION_PROFILE_ACTION_SETf;
    soc_field_t tag_act_fmt_f = OTAG_ACTION_SETf;
    uint32 tag_profile_set = 0;
    uint32 tag_act_set = 0;
    soc_field_t vlan_f = VIDf;
    soc_field_t pri_f = PCPf;
    soc_field_t cfi_f = DEf;
    soc_field_t pri_cfi_sel_f = PCP_DE_SELf;
    soc_field_t tag_act_f = TAG_ACTION_PROFILE_PTRf;
    soc_field_t tpid_f = TPID_INDEXf;

    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
           "Flex view only\n")));
        return BCM_E_PARAM;
    }

    if (info->valid_elements & BCM_FLOW_ENCAP_VLAN_VALID) {
        if (info->vlan >= BCM_VLAN_INVALID) {
            return  BCM_E_PARAM;
        }
        soc_format_field32_set(unit, tag_act_fmt, &tag_act_set, vlan_f, info->vlan);
    }
    if (info->valid_elements & BCM_FLOW_ENCAP_PKT_PRI_VALID) {
        if (info->pri > BCM_PRIO_MAX) {
            return  BCM_E_PARAM;
        }
        soc_format_field32_set(unit, tag_act_fmt, &tag_act_set, pri_f, info->pri);
    }
    if (info->valid_elements & BCM_FLOW_ENCAP_PKT_CFI_VALID) {
        if (info->cfi > 1) {
           return  BCM_E_PARAM;
        }
        soc_format_field32_set(unit, tag_act_fmt, &tag_act_set, cfi_f, info->cfi);
    }

    if (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) {
        sd_tag_remark_cfi_set = TRUE;
        action_not_present = 0x1;  /* ADD */
    }

    if (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) {
        action_present = 0x1; /* REPLACE_VID_TPID */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE) {
        action_present = 0x2;  /* REPLACE_VID_ONLY */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_DELETE) {
        action_present = 0x3;  /* DELETE */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x4;  /* REPLACE_VID_PRI_TPID */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_REPLACE){
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x5;  /* REPLACE_VID_PRI */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_PRI_REPLACE ) {
        sd_tag_remark_cfi_set = TRUE;
        action_present = 0x6;  /*  REPLACE_PRI_ONLY */

    } else if (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) {
        action_present = 0x7;  /* REPLACE_TPID_ONLY */
    }

    soc_format_field32_set(unit, tag_act_fmt, &tag_act_set, pri_cfi_sel_f, sd_tag_remark_cfi_set);
    soc_mem_field32_set(unit, mem, entry, tag_act_fmt_f, tag_act_set);

    tag_profile_set = soc_mem_field32_get(unit, mem, entry, tag_profile_fmt_f);

    profile_index = soc_format_field32_get(unit, tag_profile_fmt, &tag_profile_set, tag_act_f);
    _bcm_td3_sd_tag_action_profile_get(unit, action_present,
            action_not_present, &profile_index);
    soc_format_field32_set(unit, tag_profile_fmt, &tag_profile_set, tag_act_f, profile_index);
    soc_mem_field32_set(unit, mem, entry, tag_profile_fmt_f, tag_profile_set);

    if ((info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE)) {
        if (info->options & BCM_FLOW_ENCAP_OPTION_REPLACE) {
            tag_act_set = soc_mem_field32_get(unit, mem, entry, tag_act_fmt_f);
            old_tpid_inx = soc_format_field32_get(unit, tag_act_fmt, &tag_act_set, tpid_f);
            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit,
                                old_tpid_inx));
        }

        /* TPID value is used */
        BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_add(unit, info->tpid,
                             tpid_index));

        soc_format_field32_set(unit, tag_act_fmt, &tag_act_set, tpid_f, *tpid_index);
        soc_mem_field32_set(unit, mem, entry, tag_act_fmt_f, tag_act_set);
    }
    return  BCM_E_NONE;
}


STATIC int
_bcm_flow_sd_tag_get_from_format(
        int unit,
        bcm_flow_encap_config_t *info,
        soc_mem_t mem,
        uint32 *entry,
        int  *tpid_idx)
{
    int action_present = 0;
    int action_not_present = 0;
    int tpid_index = 0;
    uint32 profile_index = 0;
    int rv = BCM_E_NONE;
    soc_format_t tag_profile_fmt = TAG_ACTION_PROFILE_ACTION_SETfmt;
    soc_format_t tag_act_fmt = OTAG_ACTION_SETfmt;
    soc_field_t tag_profile_fmt_f = TAG_ACTION_PROFILE_ACTION_SETf;
    soc_field_t tag_act_fmt_f = OTAG_ACTION_SETf;
    uint32 tag_profile_set = 0;
    uint32 tag_act_set = 0;
    soc_field_t vlan_f = VIDf;
    soc_field_t pri_f = PCPf;
    soc_field_t cfi_f = DEf;
    soc_field_t tag_act_f = TAG_ACTION_PROFILE_PTRf;
    soc_field_t tpid_f = TPID_INDEXf;


    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
           "Flex view only\n")));
        return BCM_E_PARAM;
    }

    tag_act_set = soc_mem_field32_get(unit, mem, entry, tag_act_fmt_f);

    info->vlan = soc_format_field32_get(unit, tag_act_fmt, &tag_act_set, vlan_f);
    info->pri = soc_format_field32_get(unit, tag_act_fmt, &tag_act_set, pri_f);
    info->cfi = soc_format_field32_get(unit, tag_act_fmt, &tag_act_set, cfi_f);

    tag_profile_set = soc_mem_field32_get(unit, mem, entry, tag_profile_fmt_f);

    profile_index = soc_format_field32_get(unit, tag_profile_fmt, &tag_profile_set, tag_act_f);
    (void)_bcm_td3_sd_tag_action_get(unit, profile_index, &action_present,
                    &action_not_present);

    if (action_not_present || action_present) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED;
    }
    if (action_not_present) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD;
    }

    if (action_present == 0x1) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE;

    } else if (action_present == 0x2) {
        info->flags  |= BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE;

    } else if (action_present == 0x3) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_DELETE;

    } else if (action_present == 4) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE;

    } else if (action_present == 0x5) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_VLAN_PRI_REPLACE;

    } else if (action_present == 0x6) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_PRI_REPLACE;

    } else if (action_present == 0x7) {
        info->flags |= BCM_FLOW_ENCAP_FLAG_TPID_REPLACE;
    }

    if ((info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_TPID_REPLACE) ||
        (info->flags & BCM_FLOW_ENCAP_FLAG_VLAN_PRI_TPID_REPLACE)) {
        tpid_index = soc_format_field32_get(unit, tag_act_fmt, &tag_act_set, tpid_f);

        rv = _bcm_fb2_outer_tpid_entry_get(unit, &info->tpid, tpid_index);
        if (BCM_FAILURE(rv)) {
            info->flags |= _BCM_FLOW_ENCAP_FLAG_TPID_INDEX;
            info->tpid = tpid_index;
        }
        if (tpid_idx != NULL) {
            *tpid_idx = tpid_index;
        }
    }
    return  BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_encap_profile_delete
 * Purpose:
 *      Delete a profile in flow encap entry based on the given info
 * Parameters:
 *      unit     - (IN) Device Number
 *      info    - (IN) Flow encap config descriptor
 *      mem      - (IN) mempru ID
 *      mem_view - (IN) memory view ID
 *      key_data - (IN) entry key
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_encap_profile_delete(
        int unit,
        bcm_flow_encap_config_t *info,
        soc_mem_t mem,
        soc_mem_t mem_view,
        void *key_data)
{
    soc_format_t tag_profile_fmt = TAG_ACTION_PROFILE_ACTION_SETfmt;
    soc_field_t tag_profile_fmt_f = TAG_ACTION_PROFILE_ACTION_SETf;
    soc_field_t tag_act_f = TAG_ACTION_PROFILE_PTRf;
    uint32 tag_profile_set = 0;
    uint32 profile_index = 0;
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    int index = 0;
    int rv = BCM_E_NONE;

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
             key_data, return_entry, 0);
    if (rv != BCM_E_NONE) {
        return BCM_E_NONE;
    }
    if (soc_flow_db_mem_view_is_valid(unit, mem_view)){
        if (soc_mem_field_valid(unit, mem_view, tag_profile_fmt_f)){
            tag_profile_set = soc_mem_field32_get(unit, mem_view, return_entry,
                            tag_profile_fmt_f);
            profile_index = soc_format_field32_get(unit, tag_profile_fmt,
                            &tag_profile_set, tag_act_f);
        }
    } else if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
    (soc_mem_field_valid(unit, mem, tag_act_f))) {
        profile_index = soc_mem_field32_get(unit, mem, return_entry,
                            tag_act_f);
    }

    if (profile_index != 0) {
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                    profile_index);
    }
    return rv;
}


/*
 * Function:
 *      _bcm_flow_encap_entry_set
 * Purpose:
 *      Set flow Match Entry
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN) flow_match configuration info
 *      field - (IN) array of logical fields
 *      num_of_fields -(IN) the size of the above array
 *      mem  - (IN) memory id
 *      entry - (IN) point to the match entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_encap_entry_set(int unit,
        bcm_flow_encap_config_t *info,
        bcm_flow_logical_field_t *field,
        int num_of_fields,
        soc_mem_t mem,
        uint32 *entry,
        int network_port)
{
    int rv = BCM_E_NONE;
    int index = -1;
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    int i;
    int j;
    soc_field_t vnid_f;
    uint32 sd_tag;
    int tpid_index;
    uint32 tunnel_data[3];
    uint32 otag;
    uint32 xlate_data[3];
    int tunnel_format;
    int type = 0;
    int new_entry_add = FALSE;
    tunnel_format = EGR_VLAN_XLATE_TUNNEL_DATA_DOUBLEfmt;

    /* data fields: flex view uses PDD */
    sal_memset(tunnel_data, 0, sizeof(tunnel_data));

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, entry,
                        return_entry, 0);
    if (BCM_E_NONE == rv) {
        /* found memory entry */
        if (info->options & BCM_FLOW_ENCAP_OPTION_REPLACE) {
            sal_memcpy(entry,return_entry,SOC_MAX_MEM_WORDS * 4);
        } else {
            return BCM_E_EXISTS;
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        if (info->options & BCM_FLOW_ENCAP_OPTION_REPLACE) {
            return BCM_E_NOT_FOUND;
        } else {
            new_entry_add = TRUE;
        }
    } else {
        return rv;
    }

    if (network_port) {
        sd_tag = 0;
        if (info->options & BCM_FLOW_ENCAP_OPTION_REPLACE) {
            if (_BCM_FLOW_SD_TAG_FORMAT(info)) {
                if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
                        soc_mem_field_get(unit, mem, entry,
                             VRF_MAPPING__TUNNEL_DATAf, tunnel_data);
                        sd_tag = soc_format_field32_get(unit,tunnel_format,
                                       tunnel_data, SD_TAG_DATAf);
                    } else {
                        if (soc_mem_field_valid(unit, mem, VFI_DVP_GROUP__TUNNEL_DATAf)) {
                            soc_mem_field_get(unit, mem, entry,
                                 VFI_DVP_GROUP__TUNNEL_DATAf, tunnel_data);
                            sd_tag = soc_format_field32_get(unit,tunnel_format,
                                           tunnel_data, SD_TAG_DATAf);
                        }
                    }
                } else {
                    if (SOC_MEM_FIELD_VALID(unit,mem,SD_TAG_ACTION_SETf)) {
                        sd_tag = soc_mem_field32_get(unit, mem, entry,
                                          SD_TAG_ACTION_SETf);
                    }
                }
            } else {
                if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
                    if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                        if (soc_mem_field_valid(unit, mem, VXLAN_VFI__SD_TAG_DATAf)) {
                            sd_tag = soc_mem_field32_get(unit, mem, entry,
                                              VXLAN_VFI__SD_TAG_DATAf);
                        }
                    } else if (info->flow_handle == 
                                         SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                        sd_tag = soc_mem_field32_get(unit, mem, entry,
                                          L2GRE_VFI__SD_TAG_DATAf);
                    }
                }
            }
        }
        if ((info->valid_elements & BCM_FLOW_ENCAP_FLAGS_VALID) &&
            (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED)) {
            if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
                soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                rv = _bcm_flow_sd_tag_set_to_entry(unit, info, mem, entry,
                                      &tpid_index);
            } else if (soc_mem_field_valid(unit, mem, OTAG_ACTION_SETf) &&
                soc_mem_field_valid(unit, mem, TAG_ACTION_PROFILE_ACTION_SETf)) {
                rv = _bcm_flow_sd_tag_set_to_format(unit, info, mem, entry,
                                      &tpid_index);
            } else {
                rv = _bcm_flow_sd_tag_set(unit, info, &sd_tag,
                                      &tpid_index);
            }
            BCM_IF_ERROR_RETURN(rv);
        }

        if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP &&
            (!(_BCM_FLOW_IS_FLEX_VIEW(info)))) {
            soc_mem_field32_set(unit, mem, entry, 
                 VFI_DVP_GROUP__DATA_OVERLAY_TYPEf, 0);  /* tunnel data */
        }
        if (info->valid_elements & BCM_FLOW_ENCAP_VNID_VALID) {
            if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
                vnid_f = DIRECT_ASSIGNMENT_22_ACTION_SETf; /* VNID */
            } else {
                if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                        vnid_f = VXLAN_VFI_FLEX__VNIDf;
                    } else {
                        vnid_f = VXLAN_VFI__VN_IDf;
                    }
                    type = 2;
                    } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                    vnid_f = L2GRE_VFI__VPNIDf;
                    type = 1;
                } else {
                    return BCM_E_PARAM;
                }
            }
            if (_BCM_FLOW_TUNNEL_DATA_FORMAT(info) && 
                (!_BCM_FLOW_IS_FLEX_VIEW(info))) {
                soc_format_field32_set(unit,tunnel_format,tunnel_data,
                                TUNNEL_HEADER_DATA_TYPEf,type);
                if (type == 1) {
                    soc_format_field32_set(unit,tunnel_format,tunnel_data,
                                VPNIDf,info->vnid);
                } else if (type == 2) {
                    if ((info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN)
                        && soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                        if (soc_mem_field_valid(unit, mem, vnid_f)) {
                            soc_mem_field32_set(unit, mem, entry, vnid_f, info->vnid);
                        }
                    } else {
                        soc_format_field32_set(unit,tunnel_format,tunnel_data,
                                    VN_IDf,info->vnid);
                    }
                }
                soc_mem_field_set(unit, mem, entry,
                             VFI_DVP_GROUP__TUNNEL_DATAf, tunnel_data);
            } else {
                if (!SOC_MEM_FIELD_VALID(unit, mem, vnid_f)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, mem, entry, vnid_f, info->vnid);
            }
        }
        if ((info->valid_elements & BCM_FLOW_ENCAP_FLAGS_VALID) &&
            (info->flags & BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED)) {
            if (_BCM_FLOW_SD_TAG_FORMAT(info)) {
                if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                    soc_format_field32_set(unit,tunnel_format,tunnel_data,
                                SD_TAG_DATAf,sd_tag);
                    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
                        soc_mem_field_set(unit, mem, entry, 
                             VRF_MAPPING__TUNNEL_DATAf, tunnel_data);
                    } else {
                        if (soc_mem_field_valid(unit, mem, VFI_DVP_GROUP__TUNNEL_DATAf)) {
                            soc_mem_field_set(unit, mem, entry,
                                 VFI_DVP_GROUP__TUNNEL_DATAf, tunnel_data);
                        }
                    }
                } else {
                    if (!(soc_mem_field_valid(unit, mem, OTAG_ACTION_SETf) &&
                        soc_mem_field_valid(unit, mem, TAG_ACTION_PROFILE_ACTION_SETf))) {
                        if (!SOC_MEM_FIELD_VALID(unit, mem, SD_TAG_ACTION_SETf)) {
                            return BCM_E_PARAM;
                        }
                        soc_mem_field32_set(unit, mem, entry,
                                                  SD_TAG_ACTION_SETf, sd_tag);
                    }
                }
            } else {
                if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
                    if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                        /* VXLAN_VFI__SD_TAG_DATAf defined in regsfile */
                    } else if (info->flow_handle == 
                                            SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                        soc_mem_field32_set(unit, mem, entry,
                                          L2GRE_VFI__SD_TAG_DATAf,sd_tag);
                    }
                }
            }
        }
    } else {  /* if (network_port)  */
        if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP ||
            info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {

            sal_memset(xlate_data, 0, sizeof(xlate_data));
            rv = _bcm_flow_encap_vlan_set(unit, info, mem, entry,
                           &otag,xlate_data);
            BCM_IF_ERROR_RETURN(rv);
            if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP &&
                (!(_BCM_FLOW_IS_FLEX_VIEW(info)))) {
                if (soc_mem_field_valid(unit, mem, VFI_DVP_GROUP__DATA_OVERLAY_TYPEf)) {
                    soc_mem_field32_set(unit, mem, entry,
                         VFI_DVP_GROUP__DATA_OVERLAY_TYPEf, 1);  /* vlan xlate data */
                }
            }
            if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
                    if (soc_mem_field_valid(unit, mem, VFI_DVP_GROUP__XLATE_DATAf)) {
                        soc_mem_field_set(unit, mem, entry,
                             VFI_DVP_GROUP__XLATE_DATAf, xlate_data);
                    }
                } else { /* VLAN_XLATE_VFI */
                    soc_mem_field_set(unit, mem, entry,
                         VLAN_XLATE_VFI__XLATE_DATAf, xlate_data);
                }
            } else {
                if (!SOC_MEM_FIELD_VALID(unit, mem, OTAG_ACTION_SETf)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, mem, entry, OTAG_ACTION_SETf, otag);
            }
        }
    }

    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_SVP_DVP) {
        if (soc_feature(unit, soc_feature_vxlan_evpn)) {
            if (info->valid_elements & BCM_FLOW_ENCAP_CLASS_ID_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem, CLASS_ID_ACTION_SETf)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit,mem, entry, CLASS_ID_ACTION_SETf, info->class_id);
            }
        }
    }

#if defined(INCLUDE_XFLOW_MACSEC) || defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_xflow_macsec)) ||
         (soc_feature(unit, soc_feature_channelized_switching))) {
        if ((info->criteria == BCM_FLOW_ENCAP_CRITERIA_DGPP_INT_PRI) ||
            (info->criteria == BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI)) {
            if (info->valid_elements & BCM_FLOW_ENCAP_CLASS_ID_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem, CLASS_ID_ACTION_SETf)) {
                    return BCM_E_PARAM;
                }
                if ((soc_feature(unit, soc_feature_channelized_switching)) &&
                    (info->flags & BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT)) {


                    /* 10th bit is used to drive macsec encryption. */
                    info->class_id |= (1 << 10);
                }
                soc_mem_field32_set(unit,mem, entry, CLASS_ID_ACTION_SETf, info->class_id);
            }
        }
    }
#endif /* (INCLUDE_XFLOW_MACSEC || BCM_CHANNELIZED_SWITCHING_SUPPORT)*/

    if (info->valid_elements & BCM_FLOW_ENCAP_FLEX_DATA_VALID) {
        uint32 data_ids[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
        uint32 cnt;
        rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, data_ids, &cnt);
        BCM_IF_ERROR_RETURN(rv);
        for (i = 0; i < num_of_fields; i++) {
            for (j = 0; j < cnt; j++) {
                if (field[i].id == data_ids[j]) {
                    soc_mem_field32_set(unit,mem, entry,
                          field[i].id, field[i].value);
                    break;
                }
            }
        }
    }

    if (new_entry_add) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
    } else { /* replace action */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    }

    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

/*
 * Function:
 *      _bcm_flow_encap_entry_data_get
 * Purpose:
 *      Convert the hardware entry data to the flow encap user data 
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN) flow_encap configuration info
 *      field - (IN) array of logical fields
 *      num_of_fields -(IN) the size of the above array
 *      mem  - (IN) memory id
 *      entry - (IN) point to the encap entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_encap_entry_data_get(int unit,
        bcm_flow_encap_config_t *info,
        bcm_flow_logical_field_t *field,
        int num_of_fields,
        soc_mem_t mem,
        uint32 *entry,
        int network_port)
{
    int rv = BCM_E_NONE;
    int i;
    int j;
    soc_field_t vnid_f;
    uint32 tunnel_data[3];
    uint32 otag;
    uint32 xlate_data[3];
    int tunnel_format;
    int type;
    uint32 sd_tag = 0;
    
    tunnel_format = EGR_VLAN_XLATE_TUNNEL_DATA_DOUBLEfmt;

    /* data fields: flex view uses PDD */
    sal_memset(tunnel_data, 0, sizeof(tunnel_data));

    if (network_port) {
        if (_BCM_FLOW_SD_TAG_FORMAT(info)) {
            if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
                    soc_mem_field_get(unit, mem, entry,
                                VRF_MAPPING__TUNNEL_DATAf, tunnel_data);
                } else {
                    soc_mem_field_get(unit, mem, entry,
                                VFI_DVP_GROUP__TUNNEL_DATAf, tunnel_data);
                }
                sd_tag = soc_format_field32_get(unit,tunnel_format,
                                   tunnel_data, SD_TAG_DATAf);
            } else {
                if (SOC_MEM_FIELD_VALID(unit,mem,SD_TAG_ACTION_SETf)) {
                    sd_tag = soc_mem_field32_get(unit, mem, entry,
                                          SD_TAG_ACTION_SETf);
                }
            }
        } else {
            if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
                if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                    if (!soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                        sd_tag = soc_mem_field32_get(unit, mem, entry,
                                          VXLAN_VFI__SD_TAG_DATAf);
                    }
                } else if (info->flow_handle == 
                                     SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                    sd_tag = soc_mem_field32_get(unit, mem, entry,
                                      L2GRE_VFI__SD_TAG_DATAf);
                }
            }
        }
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
            (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) &&
            (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
            rv = _bcm_flow_sd_tag_get_from_entry(unit, info, mem, entry, NULL);
        } else if (soc_mem_field_valid(unit, mem, OTAG_ACTION_SETf) &&
            soc_mem_field_valid(unit, mem, TAG_ACTION_PROFILE_ACTION_SETf)) {
            rv = _bcm_flow_sd_tag_get_from_format(unit, info, mem, entry, NULL);
        } else {
            rv = _bcm_flow_sd_tag_get(unit, info, sd_tag, NULL);
        }
        BCM_IF_ERROR_RETURN(rv);
        
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            vnid_f = DIRECT_ASSIGNMENT_22_ACTION_SETf; /* VNID */
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    vnid_f = VXLAN_VFI_FLEX__VNIDf;
                } else {
                    vnid_f = VXLAN_VFI__VN_IDf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vnid_f = L2GRE_VFI__VPNIDf;
            } else {
                return BCM_E_PARAM;
            }
        }
        if (_BCM_FLOW_TUNNEL_DATA_FORMAT(info) && 
            (!_BCM_FLOW_IS_FLEX_VIEW(info))) {
            type = soc_format_field32_get(unit,tunnel_format,tunnel_data,
                                TUNNEL_HEADER_DATA_TYPEf);
            if (type == 1) {
                info->vnid = soc_format_field32_get(unit,tunnel_format,
                                    tunnel_data, VPNIDf);
            } else if (type == 2) {
                info->vnid = soc_format_field32_get(unit,tunnel_format,
                                   tunnel_data, VN_IDf);
            }
 
        } else {
            if (SOC_MEM_FIELD_VALID(unit,mem,vnid_f)) {
                info->vnid = soc_mem_field32_get(unit, mem, entry, vnid_f);
            }
        }
        
    } else {  /* if (network_port) */
        if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP ||
            info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
            
            if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
                if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
                    if (soc_mem_field_valid(unit, mem, VFI_DVP_GROUP__XLATE_DATAf)) {
                        soc_mem_field_get(unit, mem, entry,
                             VFI_DVP_GROUP__XLATE_DATAf, xlate_data);
                    }
                } else { /* VLAN_XLATE_VFI */
                    soc_mem_field_get(unit, mem, entry,
                         VLAN_XLATE_VFI__XLATE_DATAf, xlate_data);
                }
            } else {
                otag = soc_mem_field32_get(unit, mem, entry, OTAG_ACTION_SETf);
            }
            rv = _bcm_flow_encap_vlan_get(unit, info, mem, entry, 
                           &otag,xlate_data);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_SVP_DVP) {
        if (soc_feature(unit, soc_feature_vxlan_evpn)) {
            if (info->valid_elements & BCM_FLOW_ENCAP_SRC_FLOW_PORT_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem, CLASS_ID_ACTION_SETf)) {
                    return BCM_E_PARAM;
                }
                info->class_id = soc_mem_field32_get(unit,mem, entry, CLASS_ID_ACTION_SETf);
            }
        }
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        if ((info->criteria == BCM_FLOW_ENCAP_CRITERIA_DGPP_INT_PRI) ||
            (info->criteria == BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI)) {
            if (info->valid_elements & BCM_FLOW_ENCAP_CLASS_ID_VALID) {
                if (!SOC_MEM_FIELD_VALID(unit, mem, CLASS_ID_ACTION_SETf)) {
                    return BCM_E_PARAM;
                }
                info->class_id = soc_mem_field32_get(unit,mem, entry,
                    CLASS_ID_ACTION_SETf);

                if (info->class_id & (1 << 10)) {
                    info->flags |= BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT;
                    info->class_id = (info->class_id & 0xff);
                }
            }
        }
    }
#endif /* (BCM_CHANNELIZED_SWITCHING_SUPPORT) */

    if (_BCM_FLOW_IS_FLEX_VIEW(info) && (field != NULL)) {
        uint32 data_ids[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
        uint32 cnt;
        rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, data_ids, &cnt);
        BCM_IF_ERROR_RETURN(rv);
        for (i = 0; i < num_of_fields; i++) {
            for (j = 0; j < cnt; j++) {
                if (field[i].id == data_ids[j]) {
                    field[i].value = soc_mem_field32_get(unit,mem, entry,
                          field[i].id);
                    break;
                }
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_encap_add
 * Purpose:
 *      Add a flow encap entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow encap config descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_encap_add(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    soc_mem_t mem;
    soc_mem_t mem_view;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int rv;
    int network_port;
    _bcm_vp_info_t vp_info;
    int dvp;
    _bcm_flow_bookkeeping_t *flow_info;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    if (info->valid_elements & BCM_FLOW_ENCAP_CLASS_ID_VALID) {
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if ((soc_feature(unit, soc_feature_channelized_switching)) &&
            (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port))) {

            if  (info->class_id > 0xff) {
                return BCM_E_PARAM;
            }
        } else
#endif
        if ((info->class_id == 0) || (info->class_id > 0xfff)) {
            return BCM_E_PARAM;
        }
    }

    flow_info = FLOW_INFO(unit);

    sal_memset(entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(_bcm_flow_encap_key_set(unit,info,field,
                        num_of_fields,entry,&mem,&mem_view,&network_port));

    /* validate the dvp */
    if ((info->valid_elements & BCM_FLOW_ENCAP_FLOW_PORT_VALID) &&
        (!(BCM_GPORT_IS_MODPORT(info->flow_port) ||
          (BCM_GPORT_IS_SUBPORT_PORT(info->flow_port))))) {
        dvp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        if (!_bcm_vp_used_get(unit, dvp, _bcmVpTypeFlow)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
               "DVP %d is not the flow port\n"), dvp));
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, dvp, &vp_info));
        if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
            if (!network_port) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "DVP %d is a network port, incorrect encap criteria\n"), 
                    dvp));
                return BCM_E_PARAM;
            }
        }
    }

    soc_mem_lock(unit,mem);
    /* set the match object and attributes and write to memory */
    rv = _bcm_flow_encap_entry_set(unit,info,field,
              num_of_fields,
              _BCM_FLOW_IS_FLEX_VIEW(info)? mem_view: mem,
              entry,network_port);
    soc_mem_unlock(unit,mem);

    if (BCM_SUCCESS(rv)) {
        if (!(info->options & BCM_FLOW_ENCAP_OPTION_REPLACE)) {
            bcmi_esw_flow_lock(unit);            
            if ((mem == EGR_VLAN_XLATE_1_DOUBLEm) || (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
                flow_info->mem_use_cnt[_BCM_FLOW_EVXLT1_INX]++;
            } else if ((mem == EGR_VLAN_XLATE_2_DOUBLEm) || (mem == EGR_VLAN_XLATE_2_SINGLEm)) {
                flow_info->mem_use_cnt[_BCM_FLOW_EVXLT2_INX]++;
            }
            bcmi_esw_flow_unlock(unit);
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_flow_encap_entry_get
 * Purpose:
 *      Get the flow Encap Entry based on the given key
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN/OUT) flow_encap configuration info
 *      field - (IN/OUT) array of logical fields
 *      num_of_fields -(IN) the size of the above array
 *      mem  - (IN) memory id
 *      entry - (IN) point to the encap entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_encap_entry_get(int unit,
        bcm_flow_encap_config_t *info,
        bcm_flow_logical_field_t *field,
        int num_of_fields,
        soc_mem_t mem,
        uint32 *entry,
        int network_port)
{
    int index = -1;
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    int rv = BCM_E_NONE;

    SOC_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
              entry, return_entry, 0));
    rv = _bcm_flow_encap_entry_data_get(unit,info,field,num_of_fields,
                    mem,return_entry,network_port);
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

STATIC int
_bcm_flow_encap_entry_valid(int unit, soc_mem_t mem, uint32 *entry, int *valid)
{
    uint32 base_valid_0 = 0;
    uint32 base_valid_1 = 0;
    uint32 base_valid = 0;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if ((NULL == entry) || (NULL == valid)) {
        return BCM_E_PARAM;
    }
    if (soc_mem_field_valid(unit, mem, BASE_VALID_0f)) {
        base_valid_0 = soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f);
    }
    if (soc_mem_field_valid(unit, mem, BASE_VALID_1f)) {
        base_valid_1 = soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f);
    }
    if (soc_mem_field_valid(unit, mem, BASE_VALIDf)) {
        base_valid = soc_mem_field32_get(unit, mem, entry, BASE_VALIDf);
    }

    if ((base_valid != 0x1) && ((base_valid_0 == 0) || (base_valid_1 == 0)) ) {
        *valid = FALSE;
    } else {
        *valid = TRUE;
    }
    return BCM_E_NONE;
}

int
_bcm_flow_key_is_valid(int unit, soc_mem_t mem_view_id, 
                       uint32 *key, int num_keys) 
{
    int rv;
    uint32 key_ids[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
    uint32 cnt = 0;
    int i,j;
    int key_match;
    int num_key_fields;

    rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem_view_id,
                      SOC_FLOW_DB_FIELD_TYPE_KEY,
                      _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, key_ids, &cnt);
    if (BCM_FAILURE(rv)) {
        return FALSE;
    }

    /* don't count the overlapped hash key fields */
    num_key_fields = cnt;
    for (i = 0; i < cnt; i++) {
        if ((key_ids[i] == KEY_0f) || (key_ids[i] == KEY_1f)) {
            num_key_fields--;
        }
    }

    if (num_keys != num_key_fields) {
        return FALSE;
    }

    key_match = 0;
    for (i = 0; i < num_keys; i++) {
        for (j = 0; j < cnt; j++) {
            if (key[i] == key_ids[j]) {
                key_match++;
                break;
            }
        }
    }
    if (key_match != num_key_fields) {
        return FALSE;
    }
    return TRUE;
}

/*
 * Function   :
 *      _bcm_flow_encap_entry_parse
 * Description   :
 *      Convert the memory entry field data to the flow match configuration
 *      parameters.
 * Parameters   :
 *      unit    - (IN) Device Number
 *      mem     - (IN)  memory id.
 *      entry   - (IN) HW entry.
 *      info    - (OUT) Flow match config descriptor
 *      num_of_fields - (OUT) number of logical fields
 *      field         - (OUT) logical field array
 */
int
_bcm_flow_encap_entry_parse(int unit, soc_mem_t mem, uint32 *entry,
                             bcm_flow_encap_config_t *info,
                             uint32 *num_of_fields,
                             bcm_flow_logical_field_t *field)
{
    int data_type;  
    int key_type;
    soc_field_t vfi_f = 0;
    soc_field_t vrf_f = 0;
    soc_field_t dvp_f = 0;
    soc_field_t dvp_grp_f = 0;
    soc_field_t oif_grp_f = 0;
    uint32 dgpp;

    uint32 field_id[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];
    uint32 cnt;
    uint32 data_cnt;
    int i;
    int rv;
    soc_mem_t mem_view_id;
    uint32 tunnel_data[3];
    int tunnel_format;
    int type;
    uint32 dvp = 0;
    uint32 vfi = 0;
    uint32 vrf = 0;
    int network_port;
    uint32 svp = 0;
    
    tunnel_format = EGR_VLAN_XLATE_TUNNEL_DATA_DOUBLEfmt;
    key_type  = 0;
    data_type = 0; 
    network_port = FALSE;
   
    if (SOC_MEM_FIELD_VALID(unit,mem,KEY_TYPEf)) {
        key_type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,DATA_TYPEf)) {
        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
    }  

    /* find the memory view based on the memory and key type */
    rv = soc_flow_db_mem_to_view_id_get (unit, mem, key_type, data_type,
                    0, NULL, (uint32 *)&mem_view_id);

    if (rv != SOC_E_NONE && rv != SOC_E_NOT_FOUND) {
        return rv;
    }

    if (rv == SOC_E_NOT_FOUND) {  /* fixed view */
        rv = SOC_E_NONE;
        switch (key_type) {
            case _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI:
                info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
                info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID;
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                network_port = TRUE;
                break;

            case _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI_DVP:
                info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP;
                info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID |
                                        BCM_FLOW_ENCAP_FLOW_PORT_VALID;
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                network_port = TRUE;
                break;

            case _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI:
                info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
                info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID;
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                network_port = TRUE;
                break;

            case _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI_DVP:
                info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP;
                info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID |
                                        BCM_FLOW_ENCAP_FLOW_PORT_VALID;
                info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                network_port = TRUE;
                break;
            case _BCM_FLOW_ENCAP_KEY_TYPE_VFI_DVP_GROUP:
                info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP;
                info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID |
                                        BCM_FLOW_ENCAP_DVP_GROUP_VALID;
                if (soc_mem_field32_get(unit, mem, entry, 
                                         VFI_DVP_GROUP__DATA_OVERLAY_TYPEf)) {
                    /* xlate data */
                    
                } else {  /* tunnal data */
                    soc_mem_field_get(unit, mem, entry,  
                              VFI_DVP_GROUP__TUNNEL_DATAf, tunnel_data);
                    type = soc_format_field32_get(unit,tunnel_format,
                                    tunnel_data, TUNNEL_HEADER_DATA_TYPEf);
                    if (type == 1) {
                        info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                        network_port = TRUE;
                        info->vnid = soc_format_field32_get(unit,tunnel_format,
                                        tunnel_data, VPNIDf);
                    } else if (type == 2) {
                        info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                        network_port = TRUE;
                        info->vnid = soc_format_field32_get(unit,tunnel_format,
                                   tunnel_data, VN_IDf);
                    }
                }
                break;

            case _BCM_FLOW_ENCAP_KEY_TYPE_VRF_MAPPING:
                info->criteria = BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING;
                info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID;
                network_port = TRUE;
                soc_mem_field_get(unit, mem, entry, 
                            VRF_MAPPING__TUNNEL_DATAf, tunnel_data);
                type = soc_format_field32_get(unit,tunnel_format,tunnel_data,
                                TUNNEL_HEADER_DATA_TYPEf);
                if (type == 1) {
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                    info->vnid = soc_format_field32_get(unit,tunnel_format,
                                    tunnel_data, VPNIDf);
                } else if (type == 2) {
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                    info->vnid = soc_format_field32_get(unit,tunnel_format,
                                   tunnel_data, VN_IDf);
                }
                break;

            case _BCM_FLOW_ENCAP_KEY_TYPE_VLAN_XLATE_VFI:
                info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP;
                info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID |
                                        BCM_FLOW_ENCAP_FLOW_PORT_VALID;
                break;

            default:
                /* not used by the flow_match functions */
                return BCM_E_NOT_FOUND;
                break;
         }
    } else {  /* flex view found */
        soc_flow_db_mem_view_info_t vinfo;
        uint32 vfi_dvp_ids[2];
        uint32 vfi_grp_ids[2];
        uint32 vfi_id[1]; 
        uint32 dvp_id[1]; 
        uint32 oif_id[1]; 
        uint32 vrf_id[1];
        uint32 svp_dvp_ids[2];

        /* find the memory view info */
        rv = soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo);
        SOC_IF_ERROR_RETURN(rv);

        mem = mem_view_id;
        vfi_dvp_ids[0] = VFIf;
        vfi_dvp_ids[1] = DVPf;

        vfi_grp_ids[0] = VFIf;
        vfi_grp_ids[1] = VT_DVP_GROUP_IDf;

        svp_dvp_ids[0] = SVPf;
        svp_dvp_ids[1] = DVPf;

        vfi_id[0] = VFIf; 
        dvp_id[0] = DVPf; 
        oif_id[0] = L3_OIFf; 
        vrf_id[0] = VRFf; 
        if (_bcm_flow_key_is_valid(unit, mem_view_id, vfi_dvp_ids, 2)) { 
            info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP;
            info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID |
                                    BCM_FLOW_ENCAP_FLOW_PORT_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem_view_id, vfi_grp_ids, 2)) { 
             info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP;
            info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID |
                                    BCM_FLOW_ENCAP_DVP_GROUP_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem_view_id, vfi_id, 1)) { 
            info->criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
            info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem_view_id, dvp_id, 1)) { 
            info->criteria = BCM_FLOW_ENCAP_CRITERIA_DVP;
            info->valid_elements |= BCM_FLOW_ENCAP_FLOW_PORT_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem_view_id, oif_id, 1)) { 
            info->criteria = BCM_FLOW_ENCAP_CRITERIA_L3_INTF;
            info->valid_elements |= BCM_FLOW_ENCAP_INTF_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem_view_id, vrf_id, 1)) { 
            info->criteria = BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING;
            info->valid_elements |= BCM_FLOW_ENCAP_VPN_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem_view_id, svp_dvp_ids, 2)) { 
            info->criteria = BCM_FLOW_ENCAP_CRITERIA_SVP_DVP;
            info->valid_elements |= BCM_FLOW_ENCAP_FLOW_PORT_VALID |
                                    BCM_FLOW_ENCAP_SRC_FLOW_PORT_VALID;
        } else {
            info->criteria = BCM_FLOW_ENCAP_CRITERIA_FLEX;
            info->valid_elements |= BCM_FLOW_ENCAP_FLEX_KEY_VALID;
        }
        rv = _bcm_flow_mem_view_to_flow_info_get(unit, mem_view_id,
               SOC_FLOW_DB_FUNC_ENCAP_ID,
               &info->flow_handle, &info->flow_option, 1);
        SOC_IF_ERROR_RETURN(rv);

        network_port = TRUE;
    }

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        vfi_f     = VFIf;
        dvp_f     = DVPf;
        dvp_grp_f = VT_DVP_GROUP_IDf;
        vrf_f     = VRFf;
        oif_grp_f = VT_L3OIF_GROUP_IDf;
    } else {
        dvp_grp_f = VFI_DVP_GROUP__VT_DVP_GROUP_IDf;
        oif_grp_f = VRF_MAPPING__VT_L3OIF_GROUP_IDf;
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode) &&
                (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
                vfi_f = VXLAN_VFI_FLEX__VFIf;
                dvp_f = VXLAN_VFI_FLEX__DVPf;
            } else {
                vfi_f = VXLAN_VFI__VFIf;
                dvp_f = VXLAN_VFI__DVPf;
            }
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            vfi_f = L2GRE_VFI__VFIf;
            dvp_f = L2GRE_VFI__DVPf;
        } else {
            /* access side: VLAN_XLATE_VFI */
            vfi_f = VLAN_XLATE_VFI__VFIf;
            dvp_f = VLAN_XLATE_VFI__DVPf;
        }
    }

    /* retrieve the key fields */
    if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI) {
        vfi = soc_mem_field32_get(unit,mem, entry, vfi_f);
        _BCM_FLOW_VPN_SET(info->vpn,_BCM_VPN_TYPE_VFI,vfi);

    }  else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP) {
        vfi = soc_mem_field32_get(unit, mem, entry, vfi_f);
        _BCM_FLOW_VPN_SET(info->vpn,_BCM_VPN_TYPE_VFI,vfi);
        dvp = soc_mem_field32_get(unit, mem, entry, dvp_f);
        _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port,dvp);

    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP) {
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            vfi_f = VFI_DVP_GROUP__VFIf;
        }
        vfi = soc_mem_field32_get(unit,mem, entry, vfi_f);
        _BCM_FLOW_VPN_SET(info->vpn,_BCM_VPN_TYPE_VFI,vfi);
        info->dvp_group = soc_mem_field32_get(unit,mem, entry, 
                                                 dvp_grp_f);
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) {
        /* KEY : VRF,VT_DVP_GROUP_ID,VT_L3_OIF_GROUP_ID
         * VT_DVP_GROUP_ID    : from EGR_DVP_ATTRIBUTEm
         * VT_L3_OIF_GROUP_ID : from EGR_L3_INTFm
         */
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            vrf_f = VRF_MAPPING__VRFf;
            dvp_grp_f = VRF_MAPPING__VT_DVP_GROUP_IDf;
            oif_grp_f = VRF_MAPPING__VT_L3OIF_GROUP_IDf;
        }
        vrf = soc_mem_field32_get(unit,mem, entry, vrf_f);
        _BCM_FLOW_VPN_SET(info->vpn,_BCM_VPN_TYPE_L3,vrf);
        info->dvp_group = soc_mem_field32_get(unit,mem, entry,dvp_grp_f);
        info->oif_group = soc_mem_field32_get(unit,mem, entry,oif_grp_f);
    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_L3_INTF) {
        /* only apply to flex view */
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            return BCM_E_PARAM;
        }
        info->intf_id = soc_mem_field32_get(unit,mem, entry, L3_OIFf);

    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_DVP) {
        /* only apply to flex view */
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            return BCM_E_PARAM;
        }
        dvp = soc_mem_field32_get(unit,mem, entry, dvp_f);
        _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port,dvp);

    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_SVP_DVP) {
        /* only apply to flex view */
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            return BCM_E_PARAM;
        }
        dvp = soc_mem_field32_get(unit,mem, entry, dvp_f);
        _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port, dvp);
        svp = soc_mem_field32_get(unit,mem, entry, SVPf);
        _SHR_GPORT_FLOW_PORT_ID_SET(info->src_flow_port, svp);

    } else if (info->criteria == BCM_FLOW_ENCAP_CRITERIA_FLEX) {
        rv = soc_flow_db_mem_view_field_list_get(unit,
                             mem,
                             SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                             _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX, field_id, &cnt);
        SOC_IF_ERROR_RETURN(rv);
        if (cnt >= _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX) {
            return BCM_E_PARAM;
        }
        for (i = 0; i < cnt; i++) {
            field[i].id = field_id[i];
            field[i].value = soc_mem_field32_get(unit,mem, entry,
                     field[i].id);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,vfi_f)) {
            vfi = soc_mem_field32_get(unit,mem, entry, vfi_f);
            _BCM_FLOW_VPN_SET(info->vpn,_BCM_VPN_TYPE_VFI,vfi);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,dvp_f)) {
            dvp = soc_mem_field32_get(unit,mem, entry, dvp_f);
            _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port,dvp);
        } else {
            if (SOC_MEM_FIELD_VALID(unit,mem,DGPPf)) {
                dgpp = soc_mem_field32_get(unit,mem, entry, DGPPf);
                _SHR_GPORT_MODPORT_SET(info->flow_port,
                      dgpp >> _BCM_FLOW_MODID_SHIFTER, 
                      dgpp & _BCM_FLOW_PORT_MASK);
            }
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,dvp_grp_f)) {
            info->dvp_group = soc_mem_field32_get(unit,mem, entry, 
                                                     dvp_grp_f);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,oif_grp_f)) {
            info->oif_group = soc_mem_field32_get(unit,mem, entry, 
                                                     oif_grp_f);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,vrf_f)) {
            vrf = soc_mem_field32_get(unit,mem, entry, vrf_f);
            _BCM_FLOW_VPN_SET(info->vpn,_BCM_VPN_TYPE_L3,vrf);
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,L3_OIFf)) {
            info->intf_id = soc_mem_field32_get(unit,mem, entry, L3_OIFf);
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_IDf)) {
            info->class_id = soc_mem_field32_get(unit, mem, entry, CLASS_IDf);
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, SVPf)) {
            svp = soc_mem_field32_get(unit, mem, entry, SVPf);
            _SHR_GPORT_FLOW_PORT_ID_SET(info->src_flow_port, svp);
        }
    }

    if (info->criteria != BCM_FLOW_ENCAP_CRITERIA_FLEX) {
        cnt = 0;
    }

    data_cnt = 0;
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        /* retrieve the data fields, cnt: number of key fields */
        rv = soc_flow_db_mem_view_field_list_get(unit,
                   mem,
                   SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                   _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX - cnt, field_id, &data_cnt);
        BCM_IF_ERROR_RETURN(rv);
        if (data_cnt >= _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX - cnt) {
            return BCM_E_PARAM;
        }
        for (i = 0; i < data_cnt; i++) {
            field[i+cnt].id = field_id[i];
        }
    }
    rv = _bcm_flow_encap_entry_data_get(unit, info, field+cnt,
                    data_cnt, mem, entry,network_port);
    BCM_IF_ERROR_RETURN(rv);
    *num_of_fields = cnt + data_cnt;
    if (data_cnt) {
        info->valid_elements |= BCM_FLOW_ENCAP_FLEX_DATA_VALID;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_match_traverse_mem
 * Purpose:
 *      Read the given memory table and call the user callback function on
 *      each entry.
 * Parameters:
 *      unit    - (IN) Device Number
 *      mem     - (IN) the memory id
 *      cb      - (IN) user callback function
 *      user_data     - (IN) user context data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_encap_traverse_mem(int unit, soc_mem_t mem,
                             bcm_flow_encap_traverse_cb cb,
                             void *user_data)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of table we currently work on */
    uint32          *vt_tbl_chnk;
    uint32          *entry;
    int             valid, rv = BCM_E_NONE;
    bcm_flow_encap_config_t info;
    uint32 num_of_fields = _BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX;
    bcm_flow_logical_field_t field[_BCM_FLOW_ENCAP_LOGICAL_FIELD_MAX];

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    vt_tbl_chnk = soc_cm_salloc(unit, buf_size, "flow encap traverse");
    if (NULL == vt_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem);
         chnk_idx <= mem_idx_max;
         chnk_idx += chunksize) {
        sal_memset((void *)vt_tbl_chnk, 0, buf_size);

        chnk_idx_max =
            ((chnk_idx + chunksize) <= mem_idx_max) ?
            chnk_idx + chunksize - 1: mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, vt_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            entry =
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             vt_tbl_chnk, ent_idx);
            rv = _bcm_flow_encap_entry_valid(unit, mem, entry, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (!valid) {
                continue;
            }
            bcm_flow_encap_config_t_init(&info);
            rv = _bcm_flow_encap_entry_parse(unit, mem, entry, &info,
                    &num_of_fields,field);
            if (rv == BCM_E_NOT_FOUND) { /* for entries not exposed to user */
                continue;
            } else if (BCM_FAILURE(rv)) {
                break;
            }
            rv = cb(unit, &info, num_of_fields,field,user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }
    soc_cm_sfree(unit, vt_tbl_chnk);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_encap_delete
 * Purpose:
 *      Delete a flow encap entry based on the given key
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow encap config descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_encap_delete(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int network_port;
    _bcm_flow_bookkeeping_t *flow_info;
    soc_mem_t mem_view;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    flow_info = FLOW_INFO(unit);

    sal_memset(entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(_bcm_flow_encap_key_set(unit,info,field,
                        num_of_fields,entry,&mem, &mem_view, &network_port));
    BCM_IF_ERROR_RETURN(_bcm_flow_encap_profile_delete(unit, info, mem, mem_view, entry));
    BCM_IF_ERROR_RETURN(soc_mem_delete(unit, mem, MEM_BLOCK_ANY, entry));

    bcmi_esw_flow_lock(unit);
    if ((mem == EGR_VLAN_XLATE_1_DOUBLEm) || (mem == EGR_VLAN_XLATE_1_SINGLEm)) {
        if (flow_info->mem_use_cnt[_BCM_FLOW_EVXLT1_INX]) {
            flow_info->mem_use_cnt[_BCM_FLOW_EVXLT1_INX]--;
        }
    } else if ((mem == EGR_VLAN_XLATE_2_DOUBLEm)  || (mem == EGR_VLAN_XLATE_2_SINGLEm)) {
        if (flow_info->mem_use_cnt[_BCM_FLOW_EVXLT2_INX]) {
            flow_info->mem_use_cnt[_BCM_FLOW_EVXLT2_INX]--;
        }
    }
    bcmi_esw_flow_unlock(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_encap_get
 * Purpose:
 *      Delete a flow encap entry based on the given key
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow encap config descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_encap_get(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    soc_mem_t mem;
    soc_mem_t mem_view;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int network_port;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(_bcm_flow_encap_key_set(unit,info,field,
                        num_of_fields,entry,&mem,&mem_view,&network_port));

    BCM_IF_ERROR_RETURN(_bcm_flow_encap_entry_get(unit,info,field,
              num_of_fields,
              _BCM_FLOW_IS_FLEX_VIEW(info)? mem_view: mem,
              entry,network_port));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_encap_traverse
 * Purpose:
 *      Traverse flow encap entries. Call the user callback function on each 
 *      entry. 
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow encap config descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 *      cb            - (IN) user callback function
 *      user_data     - (IN) user context data
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_encap_traverse(
    int unit,
    bcm_flow_encap_traverse_cb cb,   
    void *user_data)
{
    soc_mem_t mem;
    int rv;
    _bcm_flow_bookkeeping_t *flow_info;

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    flow_info = FLOW_INFO(unit);

    /* traverse the first vlan_xlate */
    if (flow_info->mem_use_cnt[_BCM_FLOW_EVXLT1_INX]) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
        rv = _bcm_flow_encap_traverse_mem(unit, mem, cb, user_data);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (flow_info->mem_use_cnt[_BCM_FLOW_EVXLT1_INX]) {
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            mem = EGR_VLAN_XLATE_1_SINGLEm;
            rv = _bcm_flow_encap_traverse_mem(unit, mem, cb, user_data);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    /* traverse the second vlan_xlate */
    if (flow_info->mem_use_cnt[_BCM_FLOW_EVXLT2_INX]) {
        mem = EGR_VLAN_XLATE_2_DOUBLEm;
        rv = _bcm_flow_encap_traverse_mem(unit, mem, cb, user_data);
        BCM_IF_ERROR_RETURN(rv);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_flow_mshg_prune_egress_set
 * Parameters:
 *    unit          (IN)device number
 *    src_group_id  (IN) source split horizon group ID
 *    dst_group_id  (IN) destination split horizon group ID
 *    prune_enable  (IN) enable or disable pruning
 * Purpose
 *    This API is used to enable/disable egress pruning for Ipv6 VxLAN flow
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_flow_mshg_prune_egress_set(
        int unit,
        bcm_switch_network_group_t src_group_id,
        bcm_switch_network_group_t dst_group_id,
        int prune_enable)
{
    bcm_flow_handle_t flow_handle = 0;
    bcm_flow_option_id_t flow_option = 0;
    soc_mem_t mem_view_id = 0;
    soc_mem_t mem_id = 0;
    int index = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    soc_flow_db_mem_view_info_t mem_view_info;
    uint32 src_group_id_f = 0;
    uint32 dst_group_id_f = 0;
    int entry_exist = 0;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(soc_flow_db_flow_handle_get(unit ,"VXLAN", &flow_handle));
    BCM_IF_ERROR_RETURN(soc_flow_db_flow_option_id_get(unit, flow_handle,
        "LOOKUP_SPLIT_HORIZON_GROUPS_ASSIGN_PASS_ACTION", &flow_option));
    BCM_IF_ERROR_RETURN(soc_flow_db_ffo_to_mem_view_id_get(unit, flow_handle,
             flow_option,
             SOC_FLOW_DB_FUNC_ENCAP_ID, (uint32 *)&mem_view_id));
    BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(unit,
            flow_handle, "SVP_NETWORK_GROUP", &src_group_id_f));
    BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_logical_field_id_get(unit,
            flow_handle, "DVP_NETWORK_GROUP", &dst_group_id_f));

    sal_memset(entry, 0, sizeof(entry));
    soc_flow_db_mem_view_entry_init(unit, mem_view_id, entry);
    soc_mem_field32_set(unit, mem_view_id, entry, src_group_id_f, src_group_id);
    soc_mem_field32_set(unit, mem_view_id, entry, dst_group_id_f, dst_group_id);
    sal_memset(&mem_view_info, 0, sizeof(mem_view_info));
    BCM_IF_ERROR_RETURN(soc_flow_db_mem_view_info_get(unit, mem_view_id,
                            &mem_view_info));
    mem_id = mem_view_info.mem;
    rv = soc_mem_search(unit, mem_id, MEM_BLOCK_ANY, &index, entry,
                            return_entry, 0);
    if (BCM_E_NONE == rv) {
        entry_exist = 1;
    } else if (BCM_E_NOT_FOUND == rv) {
        entry_exist = 0;
    } else {
        return rv;
    }

    if (prune_enable) {
        sal_memset(entry, 0, sizeof(entry));
    }
    if (entry_exist) {
        rv = soc_mem_write(unit, mem_id, MEM_BLOCK_ALL, index, entry);
    } else {
        rv = soc_mem_insert(unit, mem_id, MEM_BLOCK_ALL, entry);
    }

    return rv;
}

/*
 * Function:
 *    bcmi_esw_flow_mshg_prune_egress_init
 * Parameters:
 *    unit          (IN)device number
 * Purpose
 *    Egress pruning for Ipv6 VxLAN flow initialization.
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_flow_mshg_prune_egress_init(int unit)
{
    bcm_switch_network_group_t src_group_id;
    bcm_switch_network_group_t dst_group_id;
    int rv = BCM_E_NONE;

    if (soc_reg_field_valid(unit, VXLAN_IPV6_CONTROLr, MSHG_ENABLEf)) {
        soc_reg_field32_modify(unit, VXLAN_IPV6_CONTROLr, 0, MSHG_ENABLEf, 0x1);
    }

    for (dst_group_id = 0; dst_group_id < 8; dst_group_id++) {
        for (src_group_id = 0; src_group_id < 8; src_group_id++) {
            if ((dst_group_id != src_group_id) ||
                ((src_group_id == 0) && (dst_group_id == 0))) {
                rv = bcmi_esw_flow_mshg_prune_egress_set(unit, src_group_id,
                        dst_group_id, 0x0);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                            "Ipv6 VxLAN flow: %s\n"), bcm_errmsg(rv)));
                    return rv;
                }
            }
        }
    }

    return rv;
}


#endif  /* INCLUDE_L3 */

