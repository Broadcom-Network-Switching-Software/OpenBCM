/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flexflow_match.c
 * Purpose:     Flex flow match APIs.
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
#include <bcm_int/esw/virtual.h> 
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#include <include/soc/tnl_term.h>
#include <bcm_int/esw/trident3.h>

#define _BCM_FLOW_FFO_ENTRY_MAX  20
#define _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE_WIDE 2
#define _BCM_FLOW_TD3_L3_TUNNEL_QUAD_WIDE   1
#define _QUAD_ENTRY_MAX   (4 * SOC_MAX_MEM_WORDS)

int _bcm_flow_mem_view_to_flow_info_get(int unit, 
               uint32 mem_view_id,
               uint32 func_id, 
               uint32 *flow_hndl,
               uint32 *flow_opt,
               int    instance)
{
    int i;
    soc_flow_db_ffo_t ffo[_BCM_FLOW_FFO_ENTRY_MAX];
    uint32 num_ffo;
    uint32 match_num = 0;
    int rv;

    rv = soc_flow_db_mem_view_to_ffo_get(unit,mem_view_id, 
                          _BCM_FLOW_FFO_ENTRY_MAX, ffo, &num_ffo);
    SOC_IF_ERROR_RETURN(rv);

    rv = SOC_E_NOT_FOUND;    
    for (i = 0; i < num_ffo; i++) {
        if (ffo[i].function_id == func_id) {
            match_num++;     
            if (match_num == instance) {
                *flow_hndl = ffo[i].flow_handle;
                *flow_opt  = ffo[i].option_id;
                rv = SOC_E_NONE;
                break;
            }
        }
    }  
    return rv;
}

STATIC int
_bcm_flow_match_term_entry_to_mem_entry(int unit, soc_mem_t mem,
                               soc_tunnel_term_t *tnl_entry,
                               uint32 *entry)
{

    int entry_width = sizeof(tunnel_entry_t);
    int mode;
    int i;

    mode = soc_mem_field32_get(unit, mem,
                 (uint32 *)&tnl_entry->entry_arr[0], MODEf);
    sal_memcpy((uint8 *)entry, &tnl_entry->entry_arr[0],entry_width);
    if (mode == _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE_WIDE) {
        sal_memcpy((uint8 *)entry + entry_width,
                   &tnl_entry->entry_arr[1],entry_width);

    } else if (mode == _BCM_FLOW_TD3_L3_TUNNEL_QUAD_WIDE) {
        for (i = 1; i < 4; i++) {
            if (i < COUNTOF(tnl_entry->entry_arr)) {
                sal_memcpy((uint8 *)entry + entry_width * i,
                       &tnl_entry->entry_arr[i],entry_width);
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_match_mem_entry_to_term_entry(int unit, soc_mem_t mem,
                               soc_tunnel_term_t *tnl_entry,
                               uint32 *entry)
{

    int entry_width = sizeof(tunnel_entry_t);
    int mode;
    int i;

    sal_memcpy(&tnl_entry->entry_arr[0], (uint8 *)entry,entry_width);
    mode = soc_mem_field32_get(unit, mem,
                 (uint32 *)&tnl_entry->entry_arr[0], MODEf);
    if (mode == _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE_WIDE) {
        sal_memcpy(&tnl_entry->entry_arr[1],
                   (uint8 *)entry + entry_width, entry_width);

    } else if (mode == _BCM_FLOW_TD3_L3_TUNNEL_QUAD_WIDE) {
        for (i = 1; i < 4; i++) {
            if (i < COUNTOF(tnl_entry->entry_arr)) {
                sal_memcpy(&tnl_entry->entry_arr[i],
                       (uint8 *)entry + entry_width * i,entry_width);
            }
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_flow_match_untagged_profile_set
 * Purpose:
 *      Set  VLan profile per Physical port entry
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port  - (IN) Physical port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_untagged_profile_set (int unit, bcm_port_t port)
{
    int rv=BCM_E_NONE;
    bcm_vlan_action_set_t action;
    uint32 ing_profile_idx = 0xffffffff;
    int old_ing_profile_idx = 0xffffffff;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        return BCM_E_NONE;
    }
#endif

    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = 0x0; /* No Op */
    action.ut_inner = 0x0; /* No Op */

    rv = _bcm_trx_vlan_action_profile_entry_add(unit, &action, &ing_profile_idx);
    if (rv < 0) {
        return rv;
    }

    SOC_IF_ERROR_RETURN(
        _bcm_esw_port_tab_get(unit, port, TAG_ACTION_PROFILE_PTRf,
                              &old_ing_profile_idx));

    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                               TAG_ACTION_PROFILE_PTRf, ing_profile_idx);

    if (old_ing_profile_idx != 0x1) {
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                old_ing_profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_untagged_profile_reset
 * Purpose:
 *      Reset  VLan profile per Physical port entry
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port  - (IN) Physical port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_untagged_profile_reset(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    int ing_profile_idx = -1;

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
        return BCM_E_NONE;
    }
#endif

    SOC_IF_ERROR_RETURN(
        _bcm_esw_port_tab_get(unit, port, TAG_ACTION_PROFILE_PTRf,
                              &ing_profile_idx));
    rv = _bcm_trx_vlan_action_profile_entry_delete(unit, ing_profile_idx);
    if (rv < 0) {
        return rv;
    }

    rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                               TAG_ACTION_PROFILE_PTRf, 0x1);

    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_trunk_table_set
 * Purpose:
 *      Configure Flow Trunk port entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      port    - (IN) Trunk member port
 *      tgid    - (IN) Trunk group Id
 *      info    - (IN) flow_match configuration info 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_trunk_table_set(int unit, bcm_port_t port, bcm_trunk_t tgid, 
          bcm_flow_match_config_t *info, int *replaced_vp,int *replaced_iif)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    bcm_module_t my_modid;
    int     src_trk_idx = -1;
    int     port_type;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    int     rv = BCM_E_NONE;
    int old_vp;
    int old_iif;

    if (tgid != BCM_TRUNK_INVALID) {
        port_type = 1; /* TRUNK_PORT_TYPE */
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit, "Invalid Trunk ID\n")));
        return BCM_E_PARAM;
    }

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                port, &src_trk_idx));

    soc_mem_lock(unit, mem);

    /* Read source trunk map table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                        src_trk_idx, &trunk_map_entry);

    if (rv == SOC_E_NONE) {

        /* Set trunk group id. */
        soc_mem_field32_set(unit, mem, &trunk_map_entry, SRC_TGIDf, tgid);

        /* Set port is part of Trunk group */
        soc_mem_field32_set(unit, mem, &trunk_map_entry, PORT_TYPEf, port_type);

        if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
            int vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);

            if (replaced_vp && (!(*replaced_vp))) {
                old_vp = soc_mem_field32_get(unit, mem, &trunk_map_entry, 
                                         SOURCE_VPf);
                if (old_vp && (old_vp != vp)) {
                    *replaced_vp = old_vp;
                }
            }
            /* Set SVP */
            soc_mem_field32_set(unit, mem, &trunk_map_entry, SOURCE_VPf, vp);
        }

        if (info->valid_elements & BCM_FLOW_MATCH_IIF_VALID) {
            if (replaced_iif && (!(*replaced_iif))) {
                old_iif = soc_mem_field32_get(unit, mem, &trunk_map_entry, 
                                         L3_IIFf);
                if (old_iif && (old_iif != info->intf_id)) {
                    *replaced_iif = old_iif;
                }
            }
            /* Set IIF */
            soc_mem_field32_set(unit, mem, &trunk_map_entry, L3_IIFf,
                                    info->intf_id);
        }

        /* Write entry to hw. */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, src_trk_idx, 
                            &trunk_map_entry);
    }
    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_trunk_table_get
 * Purpose:
 *      Configure flow Trunk port entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      port   - (IN) Trunk member port
 *      tgid - (IN) Trunk group Id
 *      info - (IN) flow_match configuration info 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_trunk_table_get(int unit, bcm_port_t port, bcm_trunk_t tgid, 
          bcm_flow_match_config_t *info)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    bcm_module_t my_modid;
    int     src_trk_idx = -1;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    int     rv = BCM_E_NONE;
    uint32 vp;

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                port, &src_trk_idx));

    /* Read source trunk map table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                        src_trk_idx, &trunk_map_entry);
    SOC_IF_ERROR_RETURN(rv);

    /* Get SVP */
    vp = soc_mem_field32_get(unit, mem, &trunk_map_entry, SOURCE_VPf);
    _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port,vp);

    /* Get IIF */
    info->intf_id = soc_mem_field32_get(unit, mem, &trunk_map_entry, L3_IIFf);

    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_trunk_table_reset
 * Purpose:
 *      Reset flow Trunk port entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      port   - (IN) Trunk member port
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_trunk_table_reset(int unit, bcm_port_t port, 
                int *deleted_vp,int *deleted_iif)
{
    source_trunk_map_table_entry_t trunk_map_entry;/*Trunk table entry buffer.*/
    bcm_module_t my_modid;
    int    src_trk_idx = -1;
    int rv = BCM_E_NONE;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    int vp;
    int iif;
 
    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                        port, &src_trk_idx));

    soc_mem_lock(unit, mem);

    /* Read source trunk map table. */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     src_trk_idx, &trunk_map_entry);
    if (rv == SOC_E_NONE) {

        /* Set trunk group id. */
        soc_mem_field32_set(unit, mem, &trunk_map_entry, SRC_TGIDf, 0);

        vp = soc_mem_field32_get(unit, mem, &trunk_map_entry, SOURCE_VPf);
        if (deleted_vp && (!(*deleted_vp))) {
            vp = soc_mem_field32_get(unit, mem, &trunk_map_entry, SOURCE_VPf);
            if (vp) {
                *deleted_vp = vp;
            }
        }

        /* Set SVP */
        soc_mem_field32_set(unit, mem, &trunk_map_entry, SOURCE_VPf, 0);

        if (deleted_iif && (!(*deleted_iif))) {
            iif = soc_mem_field32_get(unit, mem, &trunk_map_entry, L3_IIFf);
            if (iif) {
                *deleted_iif = iif;
            }
        }
        soc_mem_field32_set(unit, mem, &trunk_map_entry, L3_IIFf, 0);

        /* Write entry to hw. */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, src_trk_idx, 
                           &trunk_map_entry);
    }
    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_trunk_add
 * Purpose:
 *      Assign SVP of  flow Trunk port
 * Parameters:
 *      unit - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      info - (IN) flow_match configuration info 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
 _bcm_flow_match_trunk_add(int unit, 
          bcm_trunk_t tgid, 
          bcm_flow_match_config_t *info,
          int *replaced_vp,
          int *replaced_iif)
{
    int port_idx=0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_port_out, &local_member_count));
    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = _bcm_flow_match_trunk_table_set(unit, 
                            local_port_out[port_idx], tgid, info,
                            replaced_vp,replaced_iif);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = _bcm_esw_port_tab_set(
                 unit, local_port_out[port_idx],
                 _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 1); /* L2_SVP */
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }

        rv = _bcm_flow_match_untagged_profile_set(unit, 
                                     local_port_out[port_idx]);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
    }

    return BCM_E_NONE;

 trunk_cleanup:
    for (; port_idx >= 0; port_idx--) {
        (void) _bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                 _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0);
        (void) _bcm_flow_match_trunk_table_reset(unit, 
                               local_port_out[port_idx],NULL,NULL);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_trunk_delete
 * Purpose:
 *      Remove SVP of flow Trunk port
 * Parameters:
 *      unit - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      info - (IN) flow_match configuration info 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_trunk_delete(int unit, 
          bcm_trunk_t tgid,
          bcm_flow_match_config_t *info,
          int *deleted_vp,
          int *deleted_iif)
{
    int port_idx=0;
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_port_out, &local_member_count));

    for (port_idx = 0; port_idx < local_member_count; port_idx++) {
        rv = _bcm_flow_match_trunk_table_reset(unit, local_port_out[port_idx],
                    deleted_vp,deleted_iif);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
        rv = _bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                   _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }

        rv = _bcm_flow_match_untagged_profile_reset(unit, 
                                  local_port_out[port_idx]);
        if (BCM_FAILURE(rv)) {
            goto trunk_cleanup;
        }
    }

    return BCM_E_NONE;

 trunk_cleanup:
    for (;port_idx >= 0; port_idx--) {
        (void)_bcm_esw_port_tab_set(unit, local_port_out[port_idx],
                                    _BCM_CPU_TABS_NONE, PORT_OPERATIONf, 0x1);
        (void) _bcm_flow_match_trunk_table_set(unit, local_port_out[port_idx], 
                            tgid, info,NULL,NULL);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_trunk_get
 * Purpose:
 *      Assign SVP of  flow Trunk port
 * Parameters:
 *      unit - (IN) Device Number
 *      tgid - (IN) Trunk group Id
 *      info - (IN) flow_match configuration info 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
 _bcm_flow_match_trunk_get(int unit, 
          bcm_trunk_t tgid, 
          bcm_flow_match_config_t *info)
{
    int local_port_out[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_port_out, &local_member_count));
    
    /* get from the first port */
    rv = _bcm_flow_match_trunk_table_get(unit, 
                            local_port_out[0], tgid, info);
    BCM_IF_ERROR_RETURN(rv);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_match_fixed_view_entry_init
 * Purpose:
 *      initialize the fixed view memory entry
 * Parameters:
 *      unit - (IN) Device Number
 *      mem  - (IN) memory id
 *      info - (IN) flow_match configuration info 
 *      key_type - (IN) the fixed key type
 *      entry    - (OUT) point to the entry to be initialized
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_match_fixed_view_entry_init(int unit,
        soc_mem_t mem,
        bcm_flow_match_config_t *info,
        uint32 key_type,
        uint32 *entry)
{
    int rv = BCM_E_NONE;

    soc_mem_field32_set(unit,mem, entry,  KEY_TYPEf,  key_type);
    if (key_type == _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_VNID) {
        soc_mem_field32_set(unit, mem, entry, DATA_TYPEf,
            _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_SIP_VNID);
    } else if (key_type == _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP) {
        soc_mem_field32_set(unit, mem, entry, DATA_TYPEf,
            _BCM_VXLAN_FLEX_DATA_TYPE_IPV4_SIP);
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

STATIC int
_bcm_flow_match_sgpp_ctrl_set(int unit,
        bcm_flow_match_config_t *info,
        uint32 num_of_fields,
        bcm_flow_logical_field_t *field,
        int clear_action)
{
    soc_mem_t sgpp_view_id;
    int rv, i, j;
    uint32 entry[SOC_MAX_MEM_WORDS];
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    int    gport_id=-1;
    int    src_trk_idx=0;  /*Source Trunk table index.*/
    soc_mem_t mem;
    int ctrl_profile1 = 0, ctrl_profile0 = 0;
    int edit_ctrl_id = 0, vxlt_ctrl_id = 0;
    int gbp_sel = 0;
    uint32  data_array[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32  data_count;
    uint32 values[4];
    soc_field_t fields[4];
    int field_cnt = 0;
    soc_flow_db_mem_view_info_t vinfo;

    /* currently don't handle trunk port yet */
    if (BCM_GPORT_IS_TRUNK(info->port)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(entry, 0, sizeof(entry));
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,info->flow_handle, info->flow_option,
                 SOC_FLOW_DB_FUNC_SGPP_CONTROL_ID, (uint32 *)&sgpp_view_id);
    SOC_IF_ERROR_RETURN(rv);

    soc_flow_db_mem_view_entry_init(unit,sgpp_view_id,entry);
    if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, CTRL_PROFILE_IDX_1f)) {
        ctrl_profile1 = soc_mem_field32_get(unit, sgpp_view_id, entry,
                                            CTRL_PROFILE_IDX_1f);
    }
    if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, CTRL_PROFILE_IDX_0f)) {
        ctrl_profile0 = soc_mem_field32_get(unit, sgpp_view_id, entry,
                                            CTRL_PROFILE_IDX_0f);
    }
    if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, EDIT_CTRL_IDf)) {
        edit_ctrl_id = soc_mem_field32_get(unit, sgpp_view_id, entry,
                                           EDIT_CTRL_IDf);
        if (clear_action) {
            edit_ctrl_id = 0;  /* clear to default */
        }
        fields[field_cnt] = EDIT_CTRL_IDf;
        values[field_cnt++] = edit_ctrl_id;

    }
    if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, VXLT_CTRL_IDf)) {
        vxlt_ctrl_id = soc_mem_field32_get(unit, sgpp_view_id, entry,
                                           VXLT_CTRL_IDf);
        if (clear_action) {
            vxlt_ctrl_id = 0;  /* clear to default */
        }
        fields[field_cnt] = VXLT_CTRL_IDf;
        values[field_cnt++] = vxlt_ctrl_id;

    }
    if (info->valid_elements & BCM_FLOW_MATCH_FLEX_DATA_VALID) {
        if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, IDf)) {
            gbp_sel = soc_mem_field32_get(unit, sgpp_view_id, entry,
                    IDf);
        }
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, info->port,
                           &mod_out, &port_out, &trunk_id, &gport_id));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                 port_out, &src_trk_idx));

    rv = soc_flow_db_mem_view_info_get(unit, sgpp_view_id, &vinfo);
    BCM_IF_ERROR_RETURN(rv);

    mem = vinfo.mem;
    soc_mem_lock(unit,mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, src_trk_idx, entry);
    if (SOC_SUCCESS(rv)) {
        if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, CTRL_PROFILE_IDX_1f)) {
            if (clear_action) {
                ctrl_profile1 = 0;  /* clear to default */
            }
            soc_mem_field32_set(unit, sgpp_view_id, entry, CTRL_PROFILE_IDX_1f,
                                ctrl_profile1);
        }
        if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, CTRL_PROFILE_IDX_0f)) {
            if (clear_action) {
                ctrl_profile0 = 0;  /* clear to default */
            }
            soc_mem_field32_set(unit, sgpp_view_id, entry, CTRL_PROFILE_IDX_0f,
                                ctrl_profile0);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_FLEX_DATA_VALID) {
            if (SOC_MEM_FIELD_VALID(unit, sgpp_view_id, IDf)) {
                if (clear_action) {
                    gbp_sel = 0;  /* clear to default */
                }
                soc_mem_field32_set(unit, sgpp_view_id, entry, IDf,
                                    gbp_sel);
            }
            /* Get logical PDD field list for this view id. */
            rv = soc_flow_db_mem_view_field_list_get(unit, sgpp_view_id,
                    SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                    _BCM_FLOW_LOGICAL_FIELD_MAX,
                    data_array, &data_count);
            BCM_IF_ERROR_RETURN(rv);
            for (j = 0; j < num_of_fields; j++) {
                for (i = 0; i < data_count; i++) {
                    if (field[j].id == data_array[i]) {
                        if (clear_action) {
                            field[j].value = 0;  /* clear to default */
                        }
                        soc_mem_field32_set(unit, sgpp_view_id, entry, field[j].id,
                                            field[j].value);
                        break;
                    }
                }
            }
        }
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, src_trk_idx, entry);
    }
            
    soc_mem_unlock(unit,mem);

    mem = EGR_LPORT_PROFILEm;
    soc_mem_lock(unit,mem);
    if (soc_feature(unit, soc_feature_egr_lport_tab_profile)) {
        rv = bcm_esw_port_egr_lport_fields_set(unit, info->port,
                                    mem, field_cnt, fields, values);
    }
    soc_mem_unlock(unit,mem);
    return rv;
}

STATIC
void _bcm_flow_match_default_option_get(
    int unit,
    bcm_flow_match_config_t *info,
    char **option_str)
{
    *option_str = NULL;
    
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
        if (info->valid_elements & (BCM_FLOW_MATCH_SIP_VALID |
                                    BCM_FLOW_MATCH_FLOW_PORT_VALID)) {
            *option_str = "LOOKUP_IPV4_SIP_ASSIGN_SVP";
        } else if (info->valid_elements & (BCM_FLOW_MATCH_SIP_V6_VALID |
                                    BCM_FLOW_MATCH_FLOW_PORT_VALID)) {
            *option_str = "LOOKUP_IPV6_SIP_ASSIGN_SVP";
        } 
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_VN_ID) {
        if (info->valid_elements & (BCM_FLOW_MATCH_VNID_VALID |
                                    BCM_FLOW_MATCH_VPN_VALID)) {
            *option_str = "LOOKUP_VNID_ASSIGN_VFI";
        } else if (info->valid_elements & (BCM_FLOW_MATCH_VNID_VALID |
                                    BCM_FLOW_MATCH_IIF_VALID)) {
            *option_str = "LOOKUP_VNID_ASSIGN_L3_IIF";
        } 
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_MPLS_LABEL) {
        *option_str = "LOOKUP_MPLS_LABEL_ASSIGN_SVP";
    }
}

/*
 * Function:
 *      _bcm_flow_match_entry_set
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
_bcm_flow_match_entry_set(int unit, 
        bcm_flow_match_config_t *info, 
        bcm_flow_logical_field_t *field,
        int num_of_fields,
        soc_flow_db_mem_view_info_t *view_info,
        uint32 *entry,
        int *replaced_iif,
        int *replaced_vp)
{
    int rv = BCM_E_NONE;
    int i;
    int j;
    int index = -1;
    int action_type;
    uint32 return_entry[_QUAD_ENTRY_MAX];
    int vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    int new_entry_add = FALSE;
    soc_mem_t mem;
    soc_tunnel_term_t tnl_entry;
    soc_tunnel_term_t tnl_return_entry;
    uint32 action_set;

    sal_memset(&tnl_entry, 0, sizeof(soc_tunnel_term_t));
    sal_memset(&tnl_return_entry, 0, sizeof(soc_tunnel_term_t));

    mem = _BCM_FLOW_IS_FLEX_VIEW(info)? 
             view_info->mem_view_id: view_info->mem;

    if (replaced_iif != NULL) {
        *replaced_iif = 0;
    } 
    if (replaced_vp != NULL) {
        *replaced_vp = 0;
    } 
    if (view_info->mem == L3_TUNNELm) {  /* TCAM */
        rv = _bcm_flow_match_mem_entry_to_term_entry(unit, mem,
                        &tnl_entry, entry);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_tunnel_term_match(unit,&tnl_entry, 
                          &tnl_return_entry, &index);
    } else { /* hash table */
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, entry,
                        return_entry, 0);
    }
    if (BCM_E_NONE == rv) {
        /* found memory entry */
        if (info->options & BCM_FLOW_MATCH_OPTION_REPLACE) {
            if (view_info->mem == L3_TUNNELm) {
                BCM_IF_ERROR_RETURN(
                _bcm_flow_match_term_entry_to_mem_entry(unit, mem,
                        &tnl_return_entry, return_entry));
            }
            sal_memcpy(entry,return_entry,sizeof(return_entry));
        } else {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit, "Entry with same key already exists\n")));
            return BCM_E_EXISTS;
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        if (info->options & BCM_FLOW_MATCH_OPTION_REPLACE) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit, "Entry doesn't exist\n")));
            return BCM_E_NOT_FOUND;
        } else {
            new_entry_add = TRUE;
        }
    } else {
        return rv;
    }

    /* data fields: flex view uses PDD */
    action_type = 0;
    if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            if (!SOC_MEM_FIELD_VALID(unit, mem, SVP_ACTION_SETf)) {
                return BCM_E_PARAM;
            }
            if (new_entry_add == FALSE && replaced_vp != NULL) {
                action_set = soc_mem_field32_get(unit, mem, entry,
                                 SVP_ACTION_SETf);
                *replaced_vp = soc_format_field32_get(unit, SVP_ACTION_SETfmt, 
                                      &action_set, SVPf);
            }
            action_set = 0;
            soc_format_field32_set(unit, SVP_ACTION_SETfmt,
                                       &action_set, SVPf, vp);
            soc_mem_field32_set(unit, mem, entry, SVP_ACTION_SETf, action_set);
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                soc_field_t svp_f = VXLAN_SIP__SVPf;

                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    svp_f = VXLAN_FLEX_IPV4_SIP__SVPf;
                }
                if (!SOC_MEM_FIELD_VALID(unit, mem, svp_f)) {
                    return BCM_E_PARAM;
                }
                if (new_entry_add == FALSE && replaced_vp != NULL) {
                    *replaced_vp = soc_mem_field32_get(unit, mem, entry,
                                           svp_f);
                }
                soc_mem_field32_set(unit, mem, entry, svp_f, vp);
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                if (!SOC_MEM_FIELD_VALID(unit, mem, L2GRE_SIP__SVPf)) {
                    return BCM_E_PARAM;
                }
                if (new_entry_add == FALSE && replaced_vp != NULL) {
                    *replaced_vp = soc_mem_field32_get(unit, mem, entry, 
                                           L2GRE_SIP__SVPf);
                }
                soc_mem_field32_set(unit, mem, entry, L2GRE_SIP__SVPf, vp);
            } else {
                if (!SOC_MEM_FIELD_VALID(unit, mem, XLATE__SOURCE_VPf)) {
                    return BCM_E_PARAM;
                }
                if (new_entry_add == FALSE && replaced_vp != NULL) {
                    *replaced_vp = soc_mem_field32_get(unit, mem, entry, 
                                           XLATE__SOURCE_VPf);
                }
                action_type = 1; /*SVP */
                soc_mem_field32_set(unit, mem, entry, XLATE__MPLS_ACTIONf, 
                            action_type); 
                soc_mem_field32_set(unit, mem, entry, XLATE__SOURCE_VPf, vp);
            }
        }
    }

    /* get the replaced iif if replace action */
    if (info->valid_elements & BCM_FLOW_MATCH_IIF_VALID) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            if (!SOC_MEM_FIELD_VALID(unit, mem, L3_IIF_ACTION_SETf)) {
                return BCM_E_PARAM;
            }
            if (new_entry_add == FALSE && replaced_iif != NULL) {
                *replaced_iif = soc_mem_field32_get(unit, mem, entry, 
                                        L3_IIF_ACTION_SETf);
            }
            soc_mem_field32_set(unit, mem, entry, L3_IIF_ACTION_SETf, 
                   info->intf_id);
        } else {
            if (!SOC_MEM_FIELD_VALID(unit, mem, XLATE__L3_IIFf)) {
                return BCM_E_PARAM;
            }
            if (new_entry_add == FALSE && replaced_iif != NULL) {
                *replaced_iif = soc_mem_field32_get(unit, mem, entry, 
                                        XLATE__L3_IIFf);
            }
            action_type |= 0x2; /* IIF */
            soc_mem_field32_set(unit, mem, entry, XLATE__MPLS_ACTIONf, 
                                action_type); 
            soc_mem_field32_set(unit, mem, entry,XLATE__L3_IIFf, info->intf_id);
        }
    }
 
    if (info->valid_elements & BCM_FLOW_MATCH_VPN_VALID) {
        int vfi = 0;
        BCM_IF_ERROR_RETURN(
             (bcmi_esw_flow_vpn_is_valid(unit, info->vpn)));
        _BCM_FLOW_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, info->vpn);
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            if (!SOC_MEM_FIELD_VALID(unit, mem, VFI_ACTION_SETf)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, mem, entry, VFI_ACTION_SETf, vfi);
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    if (!SOC_MEM_FIELD_VALID(unit, mem, VXLAN_FLEX__VFI_ACTION_SETf)) {
                        return BCM_E_PARAM;
                    }
                    soc_mem_field32_set(unit, mem, entry, VXLAN_FLEX__VFI_ACTION_SETf, vfi);
                } else {
                    if (!SOC_MEM_FIELD_VALID(unit, mem, VXLAN_VN_ID__VFIf)) {
                        return BCM_E_PARAM;
                    }
                    soc_mem_field32_set(unit, mem, entry, VXLAN_VN_ID__VFIf, vfi);
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                if (!SOC_MEM_FIELD_VALID(unit, mem, L2GRE_VPNID__VFIf)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, mem, entry, L2GRE_VPNID__VFIf, vfi);
            } else {
                if (!SOC_MEM_FIELD_VALID(unit, mem, XLATE__VFIf)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, mem, entry, XLATE__VFIf, vfi);
            }
        }
    }

    if (info->valid_elements & BCM_FLOW_MATCH_VLAN_VALID) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            if (SOC_MEM_FIELD_VALID(unit, mem, TAG_ACTION_SETf)) {
                uint32 profile = 0;

                action_set = 0;
                action_set = soc_mem_field32_get(unit, mem, entry,
                                    TAG_ACTION_SETf);
                profile = soc_format_field32_get(unit, TAG_ACTION_SETfmt,
                                  &action_set, ING_TAG_ACTION_PROFILE_IDXf);
                _bcm_td3_def_vlan_profile_get(unit, &profile);
                soc_format_field32_set(unit, TAG_ACTION_SETfmt,
                        &action_set, ING_TAG_ACTION_PROFILE_IDXf, profile);
                soc_mem_field32_set(unit, mem, entry, TAG_ACTION_SETf,
                        action_set);

                action_set = 0;
                soc_format_field32_set(unit, OVLAN_ACTION_SETfmt,
                        &action_set, VLAN_IDf, info->vlan);
                soc_mem_field32_set(unit, mem, entry, OVLAN_ACTION_SETf,
                        action_set);
            }
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                if (soc_mem_field_valid(unit, mem, VXLAN_FLEX__OVIDf)) {
                    uint32 profile = 0;
                    soc_field_t vlan_act_f = VXLAN_FLEX__ING_TAG_ACTION_PROFILE_IDXf;

                    if (soc_mem_field_valid(unit, mem, vlan_act_f)) {
                        profile = soc_mem_field32_get(unit, mem, entry, vlan_act_f);
                        _bcm_td3_def_vlan_profile_get(unit, &profile);
                        soc_mem_field32_set(unit, mem, entry, vlan_act_f, profile);
                    }
                    soc_mem_field32_set(unit, mem, entry, VXLAN_FLEX__OVIDf, info->vlan);
                }
            }
        }
    }

    if (info->valid_elements & BCM_FLOW_MATCH_FLEX_DATA_VALID) {
        uint32 data_ids[_BCM_FLOW_LOGICAL_FIELD_MAX]; 
        uint32 cnt;
        rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_LOGICAL_FIELD_MAX, data_ids, &cnt);
        BCM_IF_ERROR_RETURN(rv);
        for (i = 0; i < num_of_fields; i++) {
            for (j = 0; j < cnt; j++) {
                if (field[i].id == data_ids[j]) {
                    soc_mem_field32_set(unit, mem, entry,
                         field[i].id, field[i].value);
                    break;
                }
            }
        }
    }

    if (view_info->mem == L3_TUNNELm) {
        rv = _bcm_flow_match_mem_entry_to_term_entry(unit, mem,
                        &tnl_entry, entry);
        if (BCM_SUCCESS(rv)) { 
            rv = soc_tunnel_term_insert(unit, &tnl_entry, (uint32 *)&index);
        }
    } else { /* hash */
        if (new_entry_add) {
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
        } else {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
        }
    }
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_entry_get
 * Purpose:
 *      Get the flow Match Entry based on the given key
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (OUT) flow_match configuration info 
 *      field - (OUT) array of logical fields 
 *      num_of_fields -(IN) the size of the above array 
 *      mem  - (IN) memory id
 *      entry - (IN) point to the match entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_entry_data_get(int unit, 
        bcm_flow_match_config_t *info, 
        bcm_flow_logical_field_t *field,
        int num_of_fields,
        soc_mem_t mem,
        uint32 *entry)
{
    int rv = BCM_E_NONE;
    int i;
    int j;
    int vp = 0;
    int vfi = 0;
    _bcm_flow_bookkeeping_t *flow_info;
    uint32 action_set;

    flow_info = FLOW_INFO(unit);

    /* data fields: flex view uses PDD */
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        if (SOC_MEM_FIELD_VALID(unit,mem,SVP_ACTION_SETf)) {
            action_set = soc_mem_field32_get(unit, mem, entry,
                                 SVP_ACTION_SETf);
            vp = soc_format_field32_get(unit, SVP_ACTION_SETfmt,
                                      &action_set, SVPf);
        }
        if (SOC_MEM_FIELD_VALID(unit, mem, OVLAN_ACTION_SETf)) {
            action_set = soc_mem_field32_get(unit, mem, entry,
                                 OVLAN_ACTION_SETf);
            info->vlan = soc_format_field32_get(unit, OVLAN_ACTION_SETfmt,
                                      &action_set, VLAN_IDf);
        }
    } else {
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_mem_field_valid(unit, mem, VXLAN_SIP__SVPf)) {
                vp = soc_mem_field32_get(unit, mem, entry, VXLAN_SIP__SVPf);
            }
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                if (soc_mem_field_valid(unit, mem, VXLAN_FLEX_IPV4_SIP__SVPf)) {
                    vp = soc_mem_field32_get(unit, mem, entry, VXLAN_FLEX_IPV4_SIP__SVPf);
                }
                if (soc_mem_field_valid(unit, mem, VXLAN_FLEX__OVIDf)) {
                   info->vlan = soc_mem_field32_get(unit, mem, entry, VXLAN_FLEX__OVIDf);
                }
            }
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            vp = soc_mem_field32_get(unit, mem, entry, L2GRE_SIP__SVPf);
        } else {
            if (SOC_MEM_FIELD_VALID(unit,mem,XLATE__SOURCE_VPf)) {
                vp = soc_mem_field32_get(unit, mem, entry, XLATE__SOURCE_VPf);
            } 
        }
    }
    if (flow_info->vp_ref_cnt[vp]) {
        _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port,vp);
        info->valid_elements |= BCM_FLOW_MATCH_FLOW_PORT_VALID;
    }
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        if (SOC_MEM_FIELD_VALID(unit,mem,L3_IIF_ACTION_SETf)) {
            info->intf_id = soc_mem_field32_get(unit, mem, entry, 
                                  L3_IIF_ACTION_SETf); 
        }
    } else {
        if (SOC_MEM_FIELD_VALID(unit,mem,XLATE__L3_IIFf)) {
            info->intf_id = soc_mem_field32_get(unit, mem, entry,
                                                XLATE__L3_IIFf);
        }
    }
    if (flow_info->iif_ref_cnt[info->intf_id]) {
        info->valid_elements |= BCM_FLOW_MATCH_IIF_VALID;
    }
  
    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        if (SOC_MEM_FIELD_VALID(unit,mem,VFI_ACTION_SETf)) {
            vfi = soc_mem_field32_get(unit, mem, entry, VFI_ACTION_SETf);
        }
    } else {
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                if (soc_mem_field_valid(unit, mem, VXLAN_FLEX__VFI_ACTION_SETf)) {
                    vfi = soc_mem_field32_get(unit, mem, entry, VXLAN_FLEX__VFI_ACTION_SETf);
                }
            }
            if (soc_mem_field_valid(unit, mem, VXLAN_VN_ID__VFIf)) {
                vfi = soc_mem_field32_get(unit, mem, entry, VXLAN_VN_ID__VFIf);
            }
        } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
            vfi = soc_mem_field32_get(unit, mem, entry, L2GRE_VPNID__VFIf);
        } else {
            if (SOC_MEM_FIELD_VALID(unit,mem,XLATE__VFIf)) {
                vfi = soc_mem_field32_get(unit, mem, entry, XLATE__VFIf);
            }
        }
    }
    if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeFlow)) { 
        info->valid_elements |= BCM_FLOW_MATCH_VPN_VALID; 
    }
    _BCM_FLOW_VPN_SET(info->vpn,_BCM_VPN_TYPE_VFI,vfi);

    if (_BCM_FLOW_IS_FLEX_VIEW(info) && (field != NULL)) {
        uint32 data_ids[_BCM_FLOW_LOGICAL_FIELD_MAX]; 
        uint32 cnt;
        rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                      _BCM_FLOW_LOGICAL_FIELD_MAX, data_ids, &cnt);
        BCM_IF_ERROR_RETURN(rv);
        for (i = 0; i < num_of_fields; i++) {
            for (j = 0; j < cnt; j++) {
                if (field[i].id == data_ids[j]) {
                    field[i].value = soc_mem_field32_get(unit, mem, entry,
                                     field[i].id);
                    break;
                }
            }
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_flow_match_entry_get
 * Purpose:
 *      Get the flow Match Entry based on the given key
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN/OUT) flow_match configuration info 
 *      field - (IN/OUT) array of logical fields 
 *      num_of_fields -(IN) the size of the above array 
 *      mem  - (IN) memory id
 *      entry - (IN) point to the match entry
 * Returns:
 *      BCM_E_XXX
 * Parameters:
 *  IN :  Unit
 *  IN :  entry
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_entry_get(int unit,
        bcm_flow_match_config_t *info,
        bcm_flow_logical_field_t *field,
        int num_of_fields,
        soc_flow_db_mem_view_info_t *view_info,
        uint32 *entry)
{
    int index = -1;
    uint32 return_entry[_QUAD_ENTRY_MAX];
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    soc_tunnel_term_t tnl_entry;
    soc_tunnel_term_t tnl_return_entry;

    if (view_info->mem == L3_TUNNELm) {
        rv = _bcm_flow_match_mem_entry_to_term_entry(unit, view_info->mem,
                        &tnl_entry, entry);
        BCM_IF_ERROR_RETURN(rv);
        rv = soc_tunnel_term_match(unit,&tnl_entry, &tnl_return_entry, &index);
        rv = _bcm_flow_match_term_entry_to_mem_entry(unit, view_info->mem,
                        &tnl_return_entry, return_entry);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        SOC_IF_ERROR_RETURN(soc_mem_search(unit, view_info->mem, 
                      MEM_BLOCK_ANY, &index, entry, return_entry, 0));
    }
    mem = _BCM_FLOW_IS_FLEX_VIEW(info)? view_info->mem_view_id: 
           view_info->mem;
    rv = _bcm_flow_match_entry_data_get(unit,info,field,num_of_fields,
                    mem,return_entry);
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_entry_key_set
 * Purpose:
 *      Set flow Match Entry Key
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
int _bcm_flow_match_entry_key_set(int unit,
    bcm_flow_match_config_t *info,
    bcm_flow_logical_field_t *field,
    int num_of_fields,
    soc_mem_t mem_id,
    uint32 *entry)
{
    int i;
    int j;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    bcm_port_t gport_id = -1;
    soc_field_t ovid_f;
    soc_field_t ivid_f;
    soc_field_t sip_f;
    soc_field_t vnid_f;
    soc_field_t t_f;
    soc_field_t tgid_f;
    soc_field_t module_id_f;
    soc_field_t port_num_f;
    soc_field_t lbl_f;
    int rv;

    /* validate parameters */

    /* XXX check if ELAN/ELINE handle differently */

    if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
        t_f         = Tf;
        tgid_f      = TGIDf;
        module_id_f = MODULE_IDf;
        port_num_f  = PORT_NUMf;
        ovid_f      = OVIDf;
        ivid_f      = IVIDf;
    } else {
        t_f         = XLATE__Tf;
        tgid_f      = XLATE__TGIDf;
        module_id_f = XLATE__MODULE_IDf;
        port_num_f  = XLATE__PORT_NUMf;
        ovid_f      = XLATE__OVIDf;
        ivid_f      = XLATE__IVIDf;
    }
    if (info->valid_elements & BCM_FLOW_MATCH_PORT_VALID) {
        rv = _bcm_esw_gport_resolve(unit, info->port, &mod_out,
                        &port_out, &trunk_id, &gport_id);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN ||
        info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN ||
        info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED ||
        info->criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {
        if (info->valid_elements & BCM_FLOW_MATCH_PORT_VALID) {
            if (BCM_GPORT_IS_TRUNK(info->port)) {
                if (!(SOC_MEM_FIELD_VALID(unit, mem_id, t_f) &&
                    SOC_MEM_FIELD_VALID(unit, mem_id, tgid_f))) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit,mem_id, entry, t_f, 1);
                soc_mem_field32_set(unit,mem_id, entry, tgid_f, trunk_id);
            } else {
                if (mod_out != -1) {
                    if (!SOC_MEM_FIELD_VALID(unit, mem_id, module_id_f)) {
                        return BCM_E_PARAM;
                    }
                
                    soc_mem_field32_set(unit, mem_id, entry, module_id_f, 
                                        mod_out);
                }
                if (!SOC_MEM_FIELD_VALID(unit, mem_id, port_num_f)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit,mem_id, entry, port_num_f, port_out);
            }
        }
        if (!SOC_MEM_FIELD_VALID(unit,mem_id,SOURCE_TYPEf)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, mem_id, entry, SOURCE_TYPEf, 1);
    }

    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN ) {
        if (!BCM_VLAN_VALID(info->vlan)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit, "Invalid vlan\n")));
             return BCM_E_PARAM;
        }
        if (!SOC_MEM_FIELD_VALID(unit, mem_id, ovid_f)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);

    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN) {
        if (!BCM_VLAN_VALID(info->inner_vlan)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit, "Invalid inner vlan\n")));
            return BCM_E_PARAM;
        }
        if (!SOC_MEM_FIELD_VALID(unit, mem_id, ivid_f)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit,mem_id, entry, ivid_f, info->inner_vlan);

    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED) {
        if (!BCM_VLAN_VALID(info->vlan) ||
                !BCM_VLAN_VALID(info->inner_vlan)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
             (BSL_META_U(unit, "Invalid either outer vlan or inner vlan\n")));
            return BCM_E_PARAM;
        }

        if (!(SOC_MEM_FIELD_VALID(unit, mem_id, ovid_f) &&
            SOC_MEM_FIELD_VALID(unit, mem_id, ivid_f))) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);
        soc_mem_field32_set(unit,mem_id, entry, ivid_f, info->inner_vlan);

    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {

        /* match_vlan : Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
        if (!SOC_MEM_FIELD_VALID(unit, mem_id, OTAGf)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit,mem_id, entry, OTAGf, info->vlan);

    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            ovid_f = OVIDf;
            if (SOC_MEM_FIELD_VALID(unit,mem_id,IPV4__SIPf)) {
                sip_f = IPV4__SIPf;
            } else {
                sip_f = SIPf;
            }
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                ovid_f = VXLAN_SIP__OVIDf;
                sip_f  = VXLAN_SIP__SIPf;
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    sip_f = VXLAN_FLEX_IPV4_SIP__SIPf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                sip_f  = L2GRE_SIP__SIPf;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid legacy flow handle\n")));
                return BCM_E_PARAM;
            }
        }
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination) &&
            (info->valid_elements & BCM_FLOW_MATCH_VLAN_VALID)) {
            VLAN_CHK_ID(unit,info->vlan);
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, ovid_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);
        }
        if (!SOC_MEM_FIELD_VALID(unit, mem_id, sip_f)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit,mem_id, entry, sip_f, info->sip);

    }  else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_VN_ID ||
                info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {

        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            ovid_f = OVIDf;
            vnid_f = VNIDf; 
            if (SOC_MEM_FIELD_VALID(unit,mem_id,IPV4__SIPf)) {
                sip_f = IPV4__SIPf;
            } else {
                sip_f = SIPf;
            }
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    ovid_f = VXLAN_FLEX__OVIDf;
                    sip_f  = VXLAN_FLEX__SIPf;
                    vnid_f = VXLAN_FLEX__VNIDf;
                } else {
                    ovid_f = VXLAN_VN_ID__OVIDf;
                    sip_f  = VXLAN_VN_ID__SIPf;
                    vnid_f = VXLAN_VN_ID__VN_IDf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vnid_f = L2GRE_VPNID__VPNIDf;
                sip_f   = L2GRE_VPNID__SIPf;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid legacy flow handle\n")));
                return BCM_E_PARAM;
            }
        }
        if (!SOC_MEM_FIELD_VALID(unit, mem_id, vnid_f)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, mem_id, entry, vnid_f, info->vnid);
        if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, sip_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, sip_f, info->sip);
        }

        if (soc_feature(unit,soc_feature_vrf_aware_vxlan_termination) &&
            (info->valid_elements & BCM_FLOW_MATCH_VLAN_VALID)) {
            VLAN_CHK_ID(unit, info->vlan);
            if (!SOC_MEM_FIELD_VALID(unit, mem_id, ovid_f)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);
        }
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_MPLS_LABEL) {
        if (_BCM_FLOW_IS_FLEX_VIEW(info)) {
            if (SOC_MEM_FIELD_VALID(unit, mem_id, MPLS_LABEL_1f)) {
                lbl_f = MPLS_LABEL_1f;
            } else if (SOC_MEM_FIELD_VALID(unit, mem_id, MPLS_LABEL_2f)) {
                lbl_f = MPLS_LABEL_2f;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                 (BSL_META_U(unit, "mpls_label field not found\n")));
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, lbl_f, info->mpls_label);
        } else {
            LOG_ERROR(BSL_LS_BCM_FLOW,
             (BSL_META_U(unit, "mpls_label only supported for flex view\n")));
            return BCM_E_PARAM;
        }
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_FLEX) {
        uint32 key_ids[_BCM_FLOW_LOGICAL_FIELD_MAX]; 
        uint32 cnt;

        ovid_f = OVIDf;
        vnid_f = VNIDf; 

        rv = soc_flow_db_mem_view_field_list_get(unit,
                      mem_id,
                      SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                      _BCM_FLOW_LOGICAL_FIELD_MAX, key_ids, &cnt);
        if (rv != SOC_E_NONE && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        if (rv == SOC_E_NONE) {
            for (i = 0; i < num_of_fields; i++) {
                for (j = 0; j < cnt; j++) {
                    if (field[i].id == key_ids[j]) {
                        soc_mem_field32_set(unit, mem_id, entry,
                             field[i].id, field[i].value);
                        break;
                    }
                }
            }
        }
        rv = SOC_E_NONE;
        if (info->valid_elements & BCM_FLOW_MATCH_VNID_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,vnid_f)) {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, vnid_f, info->vnid);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_VLAN_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,ovid_f)) {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, ovid_f, info->vlan);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_INNER_VLAN_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,ivid_f)) {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, ivid_f, info->inner_vlan);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_PORT_VALID) {
            if (BCM_GPORT_IS_TRUNK(info->port)) {
                if (!(SOC_MEM_FIELD_VALID(unit,mem_id,t_f) &&
                      SOC_MEM_FIELD_VALID(unit,mem_id,tgid_f))) {
                    if (SOC_MEM_FIELD_VALID(unit,mem_id,SGLP_16f &&
                        SOC_MEM_FIELD_VALID(unit,mem_id,SGLP_15_0f))) {
                        t_f = SGLP_16f;
                        tgid_f = SGLP_15_0f;
                    } else {
                        return SOC_E_PARAM;
                    }
                }
            
                soc_mem_field32_set(unit,mem_id, entry, t_f, 1);
                soc_mem_field32_set(unit,mem_id, entry, tgid_f, trunk_id);
            } else {
                if ((SOC_MEM_FIELD_VALID(unit,mem_id,module_id_f) &&
                     SOC_MEM_FIELD_VALID(unit,mem_id,port_num_f))) {
                    soc_mem_field32_set(unit,mem_id, entry, module_id_f, 
                                         mod_out);
                    soc_mem_field32_set(unit,mem_id, entry, port_num_f, 
                                         port_out);
                } else if (SOC_MEM_FIELD_VALID(unit,mem_id,SGPPf)) {
                    soc_mem_field32_set(unit,mem_id, entry, SGPPf, 
                                         port_out | (mod_out << 8));
                } else if (SOC_MEM_FIELD_VALID(unit,mem_id,SGLP_15_0f)) {
                    soc_mem_field32_set(unit,mem_id, entry, SGLP_15_0f,
                                         port_out | (mod_out << 8));
                } else {
                    return SOC_E_PARAM;
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_MATCH_SIP_VALID) {
            if (SOC_MEM_FIELD_VALID(unit,mem_id,SIPf)) {
                sip_f = SIPf;
            } else if (SOC_MEM_FIELD_VALID(unit,mem_id,IPV4__SIPf)) {
                sip_f = IPV4__SIPf;
            } else {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, sip_f, info->sip);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_SIP_V6_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,IPV6__SIPf)) {
                return SOC_E_PARAM;
            }
            soc_mem_ip6_addr_set(unit, mem_id, entry,IPV6__SIPf, 
                                      info->sip6, 0);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_DIP_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,IPV4__DIPf)) {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, IPV4__DIPf, info->dip);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_DIP_V6_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,IPV6__DIPf)) {
                return SOC_E_PARAM;
            }
            soc_mem_ip6_addr_set(unit, mem_id, entry,IPV6__DIPf, info->dip6, 0);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_PROTOCOL_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,IPV4__PROTOCOLf)) {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, IPV4__PROTOCOLf, 
                    info->protocol);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_UDP_SRC_PORT_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,L4_SRC_PORTf)) {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, L4_SRC_PORTf, 
                    info->udp_src_port);
        }
        if (info->valid_elements & BCM_FLOW_MATCH_UDP_DST_PORT_VALID) {
            if (!SOC_MEM_FIELD_VALID(unit,mem_id,L4_DST_PORTf)) {
                return SOC_E_PARAM;
            }
            soc_mem_field32_set(unit,mem_id, entry, L4_DST_PORTf, 
                    info->udp_dst_port);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_match_port_ctrl
 * Purpose:
 *      Perform SET/GET/DELETE actions for port match criteria 
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN) flow_match configuration info 
 *      action - (IN) action id 
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_flow_match_port_ctrl(
    int unit,
    bcm_flow_match_config_t *info,
    int action,
    int *replaced_vp,
    int *replaced_iif)
{
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    int    gport_id=-1;
    int    src_trk_idx=0;  /*Source Trunk table index.*/
    int vp = 0;
    int rv = BCM_E_NONE;
    uint32 port_op = 0;
    source_trunk_map_table_entry_t stm_entry;
    soc_mem_t mem;
    int old_vp = 0;
    int old_iif = 0;

    if (replaced_vp != NULL) {
        *replaced_vp = 0;
    }
    if (replaced_iif != NULL) {
        *replaced_iif = 0;
    }
   
    if (action == _BCM_FLOW_MATCH_PORT_CTRL_SET) {
        port_op = 1;
        vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, info->port,
                           &mod_out, &port_out, &trunk_id, &gport_id));
    if (BCM_GPORT_IS_TRUNK(info->port)) {
        if (action == _BCM_FLOW_MATCH_PORT_CTRL_SET) {
            BCM_IF_ERROR_RETURN(_bcm_flow_match_trunk_add(unit, trunk_id, 
                                     info,replaced_vp,replaced_iif));
        } else if (action == _BCM_FLOW_MATCH_PORT_CTRL_DELETE) {
            BCM_IF_ERROR_RETURN(_bcm_flow_match_trunk_delete(unit, trunk_id, 
                                     info,replaced_vp,replaced_iif));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_flow_match_trunk_get(unit, trunk_id, 
                                     info));
        }
    } else {
        int is_local;

        BCM_IF_ERROR_RETURN
            ( _bcm_esw_modid_is_local(unit, mod_out, &is_local));

        /* Get index to source trunk map table */
        BCM_IF_ERROR_RETURN(
               _bcm_esw_src_mod_port_table_index_get(unit, mod_out,
                 port_out, &src_trk_idx));

        mem = SOURCE_TRUNK_MAP_TABLEm;
        if (action != _BCM_FLOW_MATCH_PORT_CTRL_GET) {
            soc_mem_lock(unit,mem);
        }
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, src_trk_idx, &stm_entry);
        old_vp = soc_mem_field32_get(unit, mem, &stm_entry, SOURCE_VPf);
        old_iif = soc_mem_field32_get(unit, mem, &stm_entry, L3_IIFf);
        if (rv == SOC_E_NONE) {
            if (action == _BCM_FLOW_MATCH_PORT_CTRL_SET) {
                /* Enable SVP Mode */
                if  (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) { 
                    if (replaced_vp && old_vp && (old_vp != vp)) {
                        /* replace action */
                        *replaced_vp = old_vp;
                    }
                    soc_mem_field32_set(unit, mem, &stm_entry, SOURCE_VPf, 
                                             vp);
                }
                if (info->valid_elements & BCM_FLOW_MATCH_IIF_VALID) { 
                    if (replaced_iif && old_iif && (old_iif != info->intf_id)) {
                        /* replace action */
                        *replaced_iif = old_iif;
                    }
                    soc_mem_field32_set(unit, mem, &stm_entry, L3_IIFf, 
                                             info->intf_id);
                } 
            } else if (action == _BCM_FLOW_MATCH_PORT_CTRL_DELETE) {
                if (replaced_vp) {
                    *replaced_vp = old_vp;
                }
                if (replaced_iif) {
                    *replaced_iif = old_iif;
                }
                soc_mem_field32_set(unit, mem, &stm_entry, SOURCE_VPf, 0);
                soc_mem_field32_set(unit, mem, &stm_entry, L3_IIFf, 0);
            } else { /* _BCM_FLOW_MATCH_PORT_CTRL_GET */
                _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port,old_vp);
                info->intf_id = old_iif;
            }
        }
        if (action != _BCM_FLOW_MATCH_PORT_CTRL_GET) {
            if (rv == SOC_E_NONE) {
                rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, src_trk_idx, 
                               &stm_entry);
            }
            soc_mem_unlock(unit,mem);
        }
        BCM_IF_ERROR_RETURN(rv);

        if (is_local) {
            if (action != _BCM_FLOW_MATCH_PORT_CTRL_GET) {
                rv = _bcm_esw_port_tab_set(
                         unit, info->port, _BCM_CPU_TABS_NONE,
                         PORT_OPERATIONf, port_op); /* L2_SVP */
                BCM_IF_ERROR_RETURN(rv);
            }

            if (action == _BCM_FLOW_MATCH_PORT_CTRL_SET) {
                /* Set TAG_ACTION_PROFILE_PTR */
                rv = _bcm_flow_match_untagged_profile_set(
                         unit, info->port);
            } else if (action == _BCM_FLOW_MATCH_PORT_CTRL_DELETE) {
                rv = _bcm_flow_match_untagged_profile_reset(unit,info->port);
            }
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_key_set
 * Purpose:
 *      Set the key for a flow match entry 
 * Parameters:
 *      unit - (IN) Device Number
 *      info - (IN) flow_match configuration info 
 *      field - (IN) array of logical fields 
 *      num_of_fields -(IN) the size of the above array 
 *      entry - (IN) point to the match entry
 *      mem - (IN) memory id
 * Returns:
 *      BCM_E_XXX
 */

STATIC
int _bcm_flow_match_key_set(
    int unit,
    bcm_flow_match_config_t *info,
    bcm_flow_logical_field_t *field,
    uint32 num_of_fields,
    uint32 *entry,
    soc_flow_db_mem_view_info_t *view_info)
{
    int rv;
    soc_mem_t mem_view_id;
    uint32 key_type = 0;
    soc_mem_t mem_id = 0;
    soc_flow_db_mem_view_info_t vinfo;

    if (view_info == NULL) {
        return BCM_E_INTERNAL;
    }

    if (info->criteria <= BCM_FLOW_MATCH_CRITERIA_INVALID ||
            info->criteria >= BCM_FLOW_MATCH_CRITERIA_COUNT) {
        return BCM_E_PARAM;
    }

    /* validate parameters */

    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN) {
        mem_id = VLAN_XLATE_1_DOUBLEm;  /* fixed view */
        key_type = TR_VLXLT_HASH_KEY_TYPE_OVID;
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN) {
        mem_id = VLAN_XLATE_1_DOUBLEm;
        key_type = TR_VLXLT_HASH_KEY_TYPE_IVID;
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED) {
        mem_id = VLAN_XLATE_1_DOUBLEm;
        key_type = TR_VLXLT_HASH_KEY_TYPE_IVID_OVID;
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {
        mem_id = VLAN_XLATE_1_DOUBLEm;
        key_type = TR_VLXLT_HASH_KEY_TYPE_PRI_CFI;
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_VN_ID) {
        mem_id = MPLS_ENTRYm;  /* fixed view */
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem_id = VLAN_XLATE_1_DOUBLEm;
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_VNID;
            } else {
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_VNID;
            }
        } else {
            key_type = _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_VPNID;
        }
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
        mem_id = MPLS_ENTRYm;  /* fixed view */
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem_id = MPLS_ENTRY_SINGLEm;
            }
            key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP;
        } else {
            key_type = _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP;
        }
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {
        mem_id = MPLS_ENTRYm;  /* fixed view */
        if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
            if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                mem_id = VLAN_XLATE_1_DOUBLEm;
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_SIP_VNID;
            } else {
                key_type = _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP_VNID;
            }
        } else {
            key_type = _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP_VPNID;
        }
    }

    view_info->mem = mem_id;
    view_info->type = 0;
    view_info->mem_view_id = 0;

    /* get the physical memory view */
    if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
        rv = _bcm_flow_match_fixed_view_entry_init(unit, mem_id,
                       info, key_type, entry);
        BCM_IF_ERROR_RETURN(rv);      
    } else {
        char *option_str;
        uint32 dft_option_id;
        uint32 option_id;

        option_id = info->flow_option;
        if (!option_id) {
            _bcm_flow_match_default_option_get(unit,info, &option_str);
            if (option_str != NULL) {
                rv = soc_flow_db_flow_option_id_get(unit, info->flow_handle,
                                    option_str, &dft_option_id);
                if (rv == SOC_E_NONE) {
                    option_id = dft_option_id;
                } 
            }
        }
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,info->flow_handle,
             option_id,
             SOC_FLOW_DB_FUNC_MATCH_ID, (uint32 *)&mem_view_id);
        BCM_IF_ERROR_RETURN(rv);      
        rv = soc_flow_db_mem_view_info_get(unit, mem_view_id, &vinfo);
        BCM_IF_ERROR_RETURN(rv); 
        view_info->type        = vinfo.type;
        view_info->mem_view_id = mem_view_id;
        view_info->mem         = vinfo.mem;
     
        /* flex view, initialize all control fields such as data_type */ 
        soc_flow_db_mem_view_entry_init(unit,mem_view_id,entry);
        if (vinfo.mem == L3_TUNNELm) { 
            /* only available for flex */
            if ((info->criteria != BCM_FLOW_MATCH_CRITERIA_FLEX) ||
                (!_BCM_FLOW_IS_FLEX_VIEW(info))) {
                return BCM_E_PARAM;
            }
        } 
        mem_id = mem_view_id; 
    }

    rv = _bcm_flow_match_entry_key_set(unit,info,field,num_of_fields,
                        mem_id, entry); 
    
    /* BCM_FLOW_MATCH_CRITERIA_FLEX
     * flex key -- flex view
     * key comprises of logical fields and fixed fields selected
     * by valid_elements
     *
     * BCM_FLOW_MATCH_CRITERIA_XXX other than FLEX
     * fixed key -- fixed view or flex view. 
     * key comprises of fixed fields in the parameter structure. Should be 
     * determined by MATCH_CRITERIA. valid_elements can be used as parameter
     * checking in this case.
     *
     * The objects or attributes selected for the match should be determined by
     * the valid_elements
     */

    return rv;
    
}

/*
 * Function:
 *      _bcm_flow_match_info_save
 * Purpose:
 *      Save flow Match info
 * Parameters:
 *      unit - (IN) Device Number
 *      vp   - (IN) flow port
 *      info - (IN) flow_match configuration info
 * Returns:
 *      BCM_E_XXX
 */

STATIC void
_bcm_flow_match_info_save(int unit,
        int vp,
        bcm_flow_match_config_t *info)
{
    _bcm_flow_bookkeeping_t *flow_info;
    bcm_flow_match_vp_t* match_key;

    flow_info = FLOW_INFO(unit);
    match_key = &flow_info->match_key[vp];
    match_key->vlan        = info->vlan;
    match_key->inner_vlan  = info->inner_vlan;
    match_key->port        = info->port;
    match_key->sip         = info->sip;
    sal_memcpy(match_key->sip6, info->sip6,
                   BCM_IP6_ADDRLEN);
    match_key->criteria    = info->criteria;
    match_key->flow_handle = info->flow_handle;
    match_key->flow_option = info->flow_option;

    match_key->mpls_label  = info->mpls_label;
    match_key->vnid        = info->vnid;
}

/*
 * Function:
 *      _bcm_flow_match_info_clear
 * Purpose:
 *      Clear flow Match Entry
 * Parameters:
 *      unit - (IN) Device Number
 *      VP - (IN) flow VP
 * Returns:
 *      BCM_E_XXX
 */

STATIC void
_bcm_flow_match_info_clear(int unit,
        int vp)
{
    _bcm_flow_bookkeeping_t *flow_info;
    bcm_flow_match_vp_t* match_key;

    flow_info = FLOW_INFO(unit);
    match_key = &flow_info->match_key[vp];

    if (match_key != NULL) {
        sal_memset(match_key, 0, sizeof(*match_key));
    }
}

/*
 * Function:
 *      bcmi_esw_flow_match_add
 * Purpose:
 *      Add a flow match entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow match config descriptor
 *      num_of_fields - (IN) number of logical fields 
 *      field         - (IN) logical field array 
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_match_add(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    /* L3_TUNNELm could support 4 entry wide view definition */
    uint32 entry[_QUAD_ENTRY_MAX];  
    int rv;
    _bcm_flow_bookkeeping_t *flow_info;
    int replaced_iif;
    int replaced_vp;
    int vp;
    soc_flow_db_mem_view_info_t view_info;

    if (info == NULL) {
        return BCM_E_PARAM;
    }

    flow_info = FLOW_INFO(unit);

    /* validate the vp */
    if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
        vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit, "flow port ID is not valid\n")));

            return BCM_E_PORT;
        }
    }
        
    /* port match doesn't use hash table */
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT) {
        /* check if this match set flow specific controls on the given port*/
        if (_BCM_FLOW_IS_FLEX_VIEW(info) &&
            info->valid_elements & BCM_FLOW_MATCH_PORT_VALID) {
            rv = _bcm_flow_match_sgpp_ctrl_set(unit,info,num_of_fields,field,FALSE);
            BCM_IF_ERROR_RETURN(rv);
        }

        BCM_IF_ERROR_RETURN(_bcm_flow_match_port_ctrl(unit,info,
                 _BCM_FLOW_MATCH_PORT_CTRL_SET,&replaced_vp,&replaced_iif));

        bcmi_esw_flow_lock(unit);
        /* update the iif reference counter */
        if (info->valid_elements & BCM_FLOW_MATCH_IIF_VALID) {
            if (info->intf_id) {
                flow_info->iif_ref_cnt[info->intf_id]++;
            }
            if (replaced_iif) {
                if (flow_info->iif_ref_cnt[replaced_iif]) {
                    flow_info->iif_ref_cnt[replaced_iif]--;
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
            vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            if (vp) {
                flow_info->vp_ref_cnt[vp]++;
            }
            _bcm_flow_match_info_save(unit, vp, info);
            if (replaced_vp) {
                if (flow_info->vp_ref_cnt[replaced_vp]) {
                    flow_info->vp_ref_cnt[replaced_vp]--;
                }
            }
        }
        bcmi_esw_flow_unlock(unit);
        return BCM_E_NONE;
    }

    sal_memset(entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(_bcm_flow_match_key_set(unit,info,field,
                        num_of_fields,entry,&view_info));

    soc_mem_lock(unit,view_info.mem);
    /* set the match object and attributes and write to memory */ 
    rv = _bcm_flow_match_entry_set(unit,info,field,
              num_of_fields,
              &view_info,
               entry,&replaced_iif,&replaced_vp); 
    soc_mem_unlock(unit,view_info.mem);

    bcmi_esw_flow_lock(unit);
    /* update the iif reference counter */
    if (BCM_SUCCESS(rv)) { 
        if (info->valid_elements & BCM_FLOW_MATCH_IIF_VALID) {
            flow_info->iif_ref_cnt[info->intf_id]++;
            if (info->options & BCM_FLOW_MATCH_OPTION_REPLACE) {
                if (flow_info->iif_ref_cnt[replaced_iif]) {
                    flow_info->iif_ref_cnt[replaced_iif]--;
                }
            }
        }
        if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
            vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            flow_info->vp_ref_cnt[vp]++;
             _bcm_flow_match_info_save(unit, vp, info);
            if (info->options & BCM_FLOW_MATCH_OPTION_REPLACE) {
                if (flow_info->vp_ref_cnt[replaced_vp]) {
                    flow_info->vp_ref_cnt[replaced_vp]--;
                }
            }
        }
    }
    if (!(info->options & BCM_FLOW_MATCH_OPTION_REPLACE)) {
        if (view_info.mem == VLAN_XLATE_1_DOUBLEm) {
            flow_info->mem_use_cnt[_BCM_FLOW_VXLT1_INX]++;
        } else if (view_info.mem == VLAN_XLATE_2_DOUBLEm) {
            flow_info->mem_use_cnt[_BCM_FLOW_VXLT2_INX]++;
        } else if ((view_info.mem == MPLS_ENTRYm) || (view_info.mem == MPLS_ENTRY_SINGLEm)) {
            flow_info->mem_use_cnt[_BCM_FLOW_MPLS_INX]++;
        } else if (view_info.mem == L3_TUNNELm) {
            flow_info->mem_use_cnt[_BCM_FLOW_TNL_INX]++;
        }
    }
    bcmi_esw_flow_unlock(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_flow_match_profile_delete
 * Purpose:
 *      Delete a profile in flow match entry based on the given info
 * Parameters:
 *      unit     - (IN) Device Number
 *      mem      - (IN) mempru ID
 *      mem_view - (IN) memory view ID
 *      key_data - (IN) entry key
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_match_profile_delete(
        int unit,
        soc_mem_t mem,
        soc_mem_t mem_view,
        void *key_data)
{
    soc_format_t tag_profile_fmt = TAG_ACTION_SETfmt;
    soc_field_t tag_profile_fmt_f = TAG_ACTION_SETf;
    soc_field_t tag_act_f = ING_TAG_ACTION_PROFILE_IDXf;
    uint32 tag_profile_set = 0;
    uint32 profile_index = 0;
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    int index = 0;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
        key_data, return_entry, 0));
    if (soc_flow_db_mem_view_is_valid(unit, mem_view) &&
        soc_mem_field_valid(unit, mem_view, tag_profile_fmt_f)) {
        tag_profile_set = soc_mem_field32_get(unit, mem_view, return_entry,
                            tag_profile_fmt_f);
        profile_index = soc_format_field32_get(unit, tag_profile_fmt,
                            &tag_profile_set, tag_act_f);
    } else if (soc_mem_field_valid(unit, mem, tag_act_f)) {
        tag_profile_set = soc_mem_field32_get(unit, mem, return_entry,
                            tag_act_f);
    }

    if ((profile_index != 0) && (profile_index != 1)) {
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                    profile_index);
    }
    return rv;
}


/*
 * Function:
 *      bcmi_esw_flow_match_delete
 * Purpose:
 *      Delete a flow match entry based on the given key
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow match config descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_match_delete(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    uint32 entry[_QUAD_ENTRY_MAX];
    _bcm_flow_bookkeeping_t *flow_info;
    int vp;
    int deleted_vp;
    int deleted_iif;
    soc_flow_db_mem_view_info_t view_info;
    soc_tunnel_term_t key;
    int rv;

    if (info == NULL) {
        return BCM_E_PARAM;
    }
    flow_info = FLOW_INFO(unit);

    /* port match doesn't use hash table */
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT) {
        /* Check if delete the flow specfic sgpp control on a specfic port */
        if (_BCM_FLOW_IS_FLEX_VIEW(info) &&
            info->valid_elements & BCM_FLOW_MATCH_PORT_VALID) {
            rv = _bcm_flow_match_sgpp_ctrl_set(unit,info,num_of_fields,field,TRUE);
            BCM_IF_ERROR_RETURN(rv);
        }
        /* perform delete action */
        BCM_IF_ERROR_RETURN(_bcm_flow_match_port_ctrl(unit,info,
                      _BCM_FLOW_MATCH_PORT_CTRL_DELETE,&deleted_vp,
                       &deleted_iif));
        bcmi_esw_flow_lock(unit);
        if (info->intf_id) {
            if (flow_info->iif_ref_cnt[info->intf_id]) {
                flow_info->iif_ref_cnt[info->intf_id]--; 
            }
        }
        vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
        if (vp) {
            if (flow_info->vp_ref_cnt[vp]) {
                flow_info->vp_ref_cnt[vp]--; 
            }
        }
        bcmi_esw_flow_unlock(unit);
        if (deleted_vp) {
            _bcm_flow_match_info_clear(unit, deleted_vp);
        }
        return BCM_E_NONE;
    }

    sal_memset(entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(_bcm_flow_match_key_set(unit,info,field,
                        num_of_fields,entry,&view_info));
    if (view_info.mem == L3_TUNNELm) {
        rv = _bcm_flow_match_mem_entry_to_term_entry(unit, view_info.mem,
                    &key, entry);
        BCM_IF_ERROR_RETURN(rv);
    }
    BCM_IF_ERROR_RETURN(_bcm_flow_match_entry_get(unit,info,field,
              num_of_fields,
              &view_info,
              entry));

    if (view_info.mem == L3_TUNNELm) {
        BCM_IF_ERROR_RETURN(soc_tunnel_term_delete(unit, &key)); 
    } else {
        BCM_IF_ERROR_RETURN(_bcm_flow_match_profile_delete(unit, view_info.mem,
                    view_info.mem_view_id, entry));
        BCM_IF_ERROR_RETURN(soc_mem_delete(unit, view_info.mem, 
                        MEM_BLOCK_ANY, entry));
    }

    bcmi_esw_flow_lock(unit);
    if (info->intf_id) {
        if (flow_info->iif_ref_cnt[info->intf_id]) {
            flow_info->iif_ref_cnt[info->intf_id]--; 
        }
    }
    vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    if (vp) {
        if (flow_info->vp_ref_cnt[vp]) {
            flow_info->vp_ref_cnt[vp]--; 
        }
    }
    if (view_info.mem == VLAN_XLATE_1_DOUBLEm) {
        if (flow_info->mem_use_cnt[_BCM_FLOW_VXLT1_INX]) {
            flow_info->mem_use_cnt[_BCM_FLOW_VXLT1_INX]--;
        }
    } else if (view_info.mem == VLAN_XLATE_2_DOUBLEm) {
        if (flow_info->mem_use_cnt[_BCM_FLOW_VXLT2_INX]) {
            flow_info->mem_use_cnt[_BCM_FLOW_VXLT2_INX]--;
        }
    } else if ((view_info.mem == MPLS_ENTRYm) || (view_info.mem == MPLS_ENTRY_SINGLEm)) {
        if (flow_info->mem_use_cnt[_BCM_FLOW_MPLS_INX]) {
            flow_info->mem_use_cnt[_BCM_FLOW_MPLS_INX]--;
        }
    } else if (view_info.mem == L3_TUNNELm) {
        if (flow_info->mem_use_cnt[_BCM_FLOW_TNL_INX]) {
            flow_info->mem_use_cnt[_BCM_FLOW_TNL_INX]--;
        }
    }
    bcmi_esw_flow_unlock(unit);
    if (vp) {
        _bcm_flow_match_info_clear(unit, vp);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_match_get
 * Purpose:
 *      Delete a flow match entry based on the given key
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow match config descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_match_get(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    uint32 entry[_QUAD_ENTRY_MAX];
    soc_flow_db_mem_view_info_t view_info;

    if (info == NULL) {
        return BCM_E_PARAM;
    }
    /* port match doesn't use hash table */
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT) {
        /* perform delete action */
        BCM_IF_ERROR_RETURN(_bcm_flow_match_port_ctrl(unit,info,
                            _BCM_FLOW_MATCH_PORT_CTRL_GET,NULL,NULL));
        return BCM_E_NONE;
    }

    sal_memset(entry, 0, sizeof(entry));
    BCM_IF_ERROR_RETURN(_bcm_flow_match_key_set(unit,info,field,
                        num_of_fields,entry,&view_info));

    BCM_IF_ERROR_RETURN(_bcm_flow_match_entry_get(unit,info,field,
              num_of_fields,
              &view_info,
              entry)); 
    return BCM_E_NONE;
}

STATIC int
_bcm_flow_match_entry_valid(int unit, soc_mem_t mem, uint32 *entry, int *valid)
{
    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if ((NULL == entry) || (NULL == valid)) {
        return BCM_E_PARAM;
    }
    *valid = TRUE;
    if (SOC_MEM_FIELD_VALID(unit,mem,BASE_VALID_0f)) {
        if (!soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f)) {
            *valid = FALSE;
        }
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,BASE_VALID_1f)) {
        if (!soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f)) {
            *valid = FALSE;
        }
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,VALIDf)) {
        if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
            *valid = FALSE;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_flow_match_entry_parse
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
_bcm_flow_match_entry_parse(int unit, soc_mem_t mem, uint32 *entry,
                             bcm_flow_match_config_t *info,
                             uint32 *num_of_fields,
                             bcm_flow_logical_field_t *field)
{
    int data_type;  
    int key_type;
    bcm_module_t mod_out = -1;
    bcm_port_t port_out = -1;
    bcm_trunk_t trunk_id = -1;
    uint32 trunk_bit = 0;
    soc_field_t ovid_f;
    soc_field_t otag_f;
    soc_field_t ivid_f;
    soc_field_t sip_f;
    soc_field_t dip_f;
    soc_field_t vnid_f;
    soc_field_t t_f;
    soc_field_t tgid_f;
    soc_field_t module_id_f;
    soc_field_t port_num_f;
    uint32 field_id[_BCM_FLOW_LOGICAL_FIELD_MAX];
    uint32 cnt;
    uint32 data_cnt;
    int i;
    int rv;
    soc_mem_t mem_view_id;
    int is_flex_view;

    key_type  = 0;
    data_type = 0; 
    vnid_f = VXLAN_VN_ID__VN_IDf; 
    sip_f  = SIPf;
    is_flex_view = FALSE;

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
        /* match action in L3_TUNNELm only available for flex view */
        if (mem == L3_TUNNELm) {
            return SOC_E_NOT_FOUND;
        }
        rv = SOC_E_NONE;
        if (mem == VLAN_XLATE_1_DOUBLEm || mem == VLAN_XLATE_2_DOUBLEm) {
            switch (key_type) {
                case TR_VLXLT_HASH_KEY_TYPE_OVID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                            BCM_FLOW_MATCH_VLAN_VALID;
                    break;

                case TR_VLXLT_HASH_KEY_TYPE_IVID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                            BCM_FLOW_MATCH_INNER_VLAN_VALID;
                    break;

                case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED;
                    info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                            BCM_FLOW_MATCH_VLAN_VALID |
                                            BCM_FLOW_MATCH_INNER_VLAN_VALID;
                    break;

                case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_VLAN_PRI;
                    info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                            BCM_FLOW_MATCH_VLAN_VALID;
                    break;
                case _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_VNID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_VN_ID;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_VNID_VALID;
                    break;
                case _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_SIP_VNID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP_VNID;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID |
                                            BCM_FLOW_MATCH_VNID_VALID;
                    break;
                default:
                    /* not used by the flow_match functions */
                    return BCM_E_NOT_FOUND;
                    break;
             }
        } else if (mem == MPLS_ENTRYm) {
            switch (key_type) {
                case _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_VNID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_VN_ID;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_VNID_VALID;
                    break;
                case _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_VPNID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_VN_ID;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                    info->valid_elements |= BCM_FLOW_MATCH_VNID_VALID;
                    break;
                case _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
                    info->vlan = soc_mem_field32_get(unit, mem,
                           entry, VXLAN_SIP__OVIDf);
                    break;
                case _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                    info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
                    break;
                case _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP_VNID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP_VNID;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID |
                                            BCM_FLOW_MATCH_VNID_VALID;
                    break;
                case _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP_VPNID:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP_VNID;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE;
                    info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID |
                                            BCM_FLOW_MATCH_VNID_VALID;
                    break;

                default:
                    /* not used by the flow_match functions */
                    return BCM_E_NOT_FOUND;
                    break;
             }
        } else if (mem == MPLS_ENTRY_SINGLEm) {
            switch (key_type) {
                case _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP:
                    info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
                    info->flow_handle = SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN;
                    info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
                    break;
                default:
                    /* not used by the flow_match functions */
                    return BCM_E_NOT_FOUND;
                    break;
            }
        }
    } else {  /* flex view found */      
        uint32 port_ovid_ids[3];
        uint32 port_ivid_ids[3];
        uint32 port_dtag_ids[4];
        uint32 otag_id[1];
        uint32 sip_id[1];
        uint32 vnid_id[1];
        uint32 vnid_sip_ids[2];

        is_flex_view = TRUE;
        port_ovid_ids[0] = MODULE_IDf;
        port_ovid_ids[1] = PORT_NUMf;
        port_ovid_ids[2] = OVIDf;

        port_ivid_ids[0] = MODULE_IDf;
        port_ivid_ids[1] = PORT_NUMf;
        port_ivid_ids[2] = IVIDf;

        port_dtag_ids[0] = MODULE_IDf;
        port_dtag_ids[1] = PORT_NUMf;
        port_dtag_ids[2] = OVIDf;
        port_dtag_ids[3] = IVIDf;

        otag_id[0] = OTAGf;
        vnid_id[0] = VNIDf;
        vnid_sip_ids[1] = VNIDf;
        if (SOC_MEM_FIELD_VALID(unit,mem_view_id,IPV4__SIPf)) {
            sip_id[0]  = IPV4__SIPf;
            vnid_sip_ids[0] = IPV4__SIPf;
        } else {
            sip_id[0]  = SIPf;
            vnid_sip_ids[0] = SIPf;
        }
        rv = _bcm_flow_mem_view_to_flow_info_get(unit, mem_view_id, 
               SOC_FLOW_DB_FUNC_MATCH_ID, 
               &info->flow_handle, &info->flow_option, 1);
        SOC_IF_ERROR_RETURN(rv);

        mem = mem_view_id;
        if (_bcm_flow_key_is_valid(unit, mem, port_ovid_ids, 3)) {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN;
            info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                    BCM_FLOW_MATCH_VLAN_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem, port_ivid_ids,3)) {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_PORT_INNER_VLAN;
            info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                    BCM_FLOW_MATCH_INNER_VLAN_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem, port_dtag_ids,4)) {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN_STACKED;
            info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                    BCM_FLOW_MATCH_VLAN_VALID |
                                    BCM_FLOW_MATCH_INNER_VLAN_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem, otag_id, 1)) {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_VLAN_PRI;
            info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID |
                                    BCM_FLOW_MATCH_VLAN_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem, vnid_id, 1)) {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_VN_ID;
            info->valid_elements |= BCM_FLOW_MATCH_VNID_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem, sip_id, 1)) {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
            info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
        } else if (_bcm_flow_key_is_valid(unit, mem, vnid_sip_ids, 1)) {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_SIP_VNID;
            info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID |
                                    BCM_FLOW_MATCH_VNID_VALID;
        } else {
            info->criteria = BCM_FLOW_MATCH_CRITERIA_FLEX;
            info->valid_elements |= BCM_FLOW_MATCH_FLEX_KEY_VALID;
        }
    }

    /* retrieve the key fields */
    if (is_flex_view) { /* flex view */
        t_f         = Tf;
        tgid_f      = TGIDf;
        module_id_f = MODULE_IDf;
        port_num_f  = PORT_NUMf;
        ovid_f      = OVIDf;
        ivid_f      = IVIDf;
        otag_f      = OTAGf;
    } else {  /* fix key type */
        t_f         = XLATE__Tf;
        tgid_f      = XLATE__TGIDf;
        module_id_f = XLATE__MODULE_IDf;
        port_num_f  = XLATE__PORT_NUMf;
        ovid_f      = XLATE__OVIDf;
        ivid_f      = XLATE__IVIDf;
        otag_f      = XLATE__OTAGf;
    }
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_VLAN_PRI) {
        if (SOC_MEM_FIELD_VALID(unit,mem,otag_f)) { 
            info->vlan = soc_mem_field32_get(unit, mem, entry, otag_f);
        }
    } else {
        if (SOC_MEM_FIELD_VALID(unit,mem,ovid_f)) { 
            info->vlan = soc_mem_field32_get(unit, mem, entry, ovid_f);
        }
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,ivid_f)) { 
        info->inner_vlan = soc_mem_field32_get(unit, mem, entry, ivid_f);
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,t_f)) { 
        trunk_bit = soc_mem_field32_get(unit,mem, entry, t_f);
    }
    if (trunk_bit) {
        if (SOC_MEM_FIELD_VALID(unit,mem,tgid_f)) { 
            trunk_id = soc_mem_field32_get(unit,mem, entry, tgid_f);
            BCM_GPORT_TRUNK_SET(info->port,trunk_id);
            info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID;
        }
    } else {
        if (SOC_MEM_FIELD_VALID(unit,mem,module_id_f)) { 
            mod_out = soc_mem_field32_get(unit,mem, entry, module_id_f);
            if (SOC_MEM_FIELD_VALID(unit,mem,port_num_f)) { 
                port_out= soc_mem_field32_get(unit,mem, entry, port_num_f);
            }
            if (mod_out != -1 && port_out != 1) { 
                BCM_GPORT_MODPORT_SET(info->port,mod_out,port_out);
                info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID;
            }
        }
    }
    if (SOC_MEM_FIELD_VALID(unit,mem,SGPPf)) {
        uint32 sgpp;

        sgpp = soc_mem_field32_get(unit,mem, entry, SGPPf); 
        port_out = sgpp & 0xff;
        mod_out  = (sgpp >> 8) & 0xff;
        BCM_GPORT_MODPORT_SET(info->port,mod_out,port_out);
        info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID;
    }

    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP) {
        if (is_flex_view) {
            ovid_f = OVIDf;
            vnid_f = VNIDf; 
            if (SOC_MEM_FIELD_VALID(unit,mem,IPV4__SIPf)) {
                sip_f  = IPV4__SIPf;
            } else {
                sip_f  = SIPf;
            }
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    sip_f  = VXLAN_FLEX_IPV4_SIP__SIPf;
                } else {
                    ovid_f = VXLAN_SIP__OVIDf;
                    sip_f  = VXLAN_SIP__SIPf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                sip_f  = L2GRE_SIP__SIPf;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid legacy flow handle\n")));
                return BCM_E_PARAM;
            }
        }
        if (soc_feature(unit, soc_feature_vrf_aware_vxlan_termination)) { 
            if (SOC_MEM_FIELD_VALID(unit,mem,ovid_f)) {
                info->vlan = soc_mem_field32_get(unit,mem, entry, ovid_f);
                info->valid_elements |= BCM_FLOW_MATCH_VLAN_VALID;
            }
        }
        info->sip = soc_mem_field32_get(unit,mem, entry, sip_f);
        info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_VN_ID ||
            info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {
        if (is_flex_view) {
            ovid_f = OVIDf;
            vnid_f = VNIDf; 
            if (SOC_MEM_FIELD_VALID(unit,mem,IPV4__SIPf)) {
                sip_f  = IPV4__SIPf;
            } else {
                sip_f  = SIPf;
            }
        } else {
            if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) {
                if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
                    ovid_f = VXLAN_FLEX__OVIDf;
                    sip_f  = VXLAN_FLEX__SIPf;
                    vnid_f = VXLAN_FLEX__VNIDf;
                } else {
                    ovid_f = VXLAN_VN_ID__OVIDf;
                    sip_f  = VXLAN_VN_ID__SIPf;
                    vnid_f = VXLAN_VN_ID__VN_IDf;
                }
            } else if (info->flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE) {
                vnid_f = L2GRE_VPNID__VPNIDf;
                sip_f   = L2GRE_VPNID__SIPf;
            } else {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, "Invalid legacy flow handle\n")));
                return BCM_E_PARAM;
            }
        }
        info->vnid = soc_mem_field32_get(unit, mem, entry, vnid_f);
        info->valid_elements |= BCM_FLOW_MATCH_VNID_VALID;
        if (info->criteria == BCM_FLOW_MATCH_CRITERIA_SIP_VNID) {
            info->sip = soc_mem_field32_get(unit,mem, entry, sip_f);
            info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,ovid_f)) {
            info->vlan = soc_mem_field32_get(unit,mem, entry, ovid_f);
            info->valid_elements |= BCM_FLOW_MATCH_VLAN_VALID;
        } else if (SOC_MEM_FIELD_VALID(unit, mem, OVLAN_ACTION_SETf)) {
                uint32 action_set = soc_mem_field32_get(unit, mem, entry, OVLAN_ACTION_SETf);
                info->vlan = soc_format_field32_get(unit, OVLAN_ACTION_SETfmt, &action_set, VLAN_IDf);
                info->valid_elements |= BCM_FLOW_MATCH_VLAN_VALID;
            }

    } else if (info->criteria == BCM_FLOW_MATCH_CRITERIA_FLEX) {
        rv = soc_flow_db_mem_view_field_list_get(unit,
                         mem,
                         SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
                         _BCM_FLOW_LOGICAL_FIELD_MAX, field_id, &cnt);
        SOC_IF_ERROR_RETURN(rv);
        if (cnt >= _BCM_FLOW_LOGICAL_FIELD_MAX) {
                LOG_ERROR(BSL_LS_BCM_FLOW,
                      (BSL_META_U(unit, 
                       "Exceed max logical array size: %d\n"),cnt));
            return BCM_E_PARAM;
        }
        for (i = 0; i < cnt; i++) {
            field[i].id = field_id[i]; 
            field[i].value = soc_mem_field32_get(unit,mem, entry,
                     field[i].id);
        }
           
        if (SOC_MEM_FIELD_VALID(unit,mem,vnid_f)) { 
            info->vnid = soc_mem_field32_get(unit,mem, entry, vnid_f);
            info->valid_elements |= BCM_FLOW_MATCH_VNID_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,ovid_f)) { 
            info->vlan = soc_mem_field32_get(unit,mem, entry, ovid_f);
            info->valid_elements |= BCM_FLOW_MATCH_VLAN_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,ivid_f)) { 
            info->inner_vlan = soc_mem_field32_get(unit,mem, entry, ivid_f);
            info->valid_elements |= BCM_FLOW_MATCH_INNER_VLAN_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,IPV4__SIPf)) {
            sip_f  = IPV4__SIPf;
        } else {
            sip_f  = SIPf;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,sip_f)) { 
            info->sip = soc_mem_field32_get(unit,mem, entry, sip_f);
            info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,IPV6__SIPf)) { 
            soc_mem_ip6_addr_get(unit, mem, entry,IPV6__SIPf, info->sip6, 0);
            info->valid_elements |= BCM_FLOW_MATCH_SIP_V6_VALID;
        }

        if (SOC_MEM_FIELD_VALID(unit,mem,IPV4__DIPf)) {
            dip_f  = IPV4__DIPf;
        } else {
            dip_f  = DIPf;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,dip_f)) { 
            info->dip = soc_mem_field32_get(unit,mem, entry, dip_f);
            info->valid_elements |= BCM_FLOW_MATCH_DIP_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,IPV6__DIPf)) { 
            soc_mem_ip6_addr_get(unit, mem, entry,IPV6__DIPf, info->dip6, 0);
            info->valid_elements |= BCM_FLOW_MATCH_DIP_V6_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,IPV4__PROTOCOLf)) {
            info->protocol = soc_mem_field32_get(unit,mem, entry, 
                                   IPV4__PROTOCOLf);
            info->valid_elements |= BCM_FLOW_MATCH_PROTOCOL_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,L4_SRC_PORTf)) {
            info->udp_src_port = soc_mem_field32_get(unit,mem, entry, 
                        L4_SRC_PORTf);
            info->valid_elements |= BCM_FLOW_MATCH_UDP_SRC_PORT_VALID;
        }
        if (SOC_MEM_FIELD_VALID(unit,mem,L4_DST_PORTf)) {
            info->udp_dst_port = soc_mem_field32_get(unit,mem, entry, 
                                   L4_DST_PORTf);
            info->valid_elements |= BCM_FLOW_MATCH_UDP_DST_PORT_VALID;
        }
    }     

    if (info->criteria != BCM_FLOW_MATCH_CRITERIA_FLEX) {
        cnt = 0;
    }

    data_cnt = 0;
    if (is_flex_view) { /* flex view */
        /* retrieve the data fields, cnt: number of key fields */
        rv = soc_flow_db_mem_view_field_list_get(unit,
                   mem,
                   SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
                   _BCM_FLOW_LOGICAL_FIELD_MAX - cnt, field_id, &data_cnt);
        if (rv != SOC_E_NONE && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
    
        if (data_cnt >= _BCM_FLOW_LOGICAL_FIELD_MAX - cnt) {
            LOG_ERROR(BSL_LS_BCM_FLOW,
                (BSL_META_U(unit, 
                "Exceed default logical array size: %d\n"),cnt + data_cnt));
            return BCM_E_PARAM;
        }
        for (i = 0; i < data_cnt; i++) {
            field[i+cnt].id = field_id[i];
        }
        if (!_BCM_FLOW_IS_FLEX_VIEW(info)) {
            /* flow_handle is not properly set, use default flex handle*/
            info->flow_handle = SOC_FLOW_DB_FLOW_ID_START;
        }
    }
    rv = _bcm_flow_match_entry_data_get(unit, info, field+cnt, 
                    data_cnt, mem, entry);
    BCM_IF_ERROR_RETURN(rv);
    *num_of_fields = cnt + data_cnt;
    if (data_cnt) {
        info->valid_elements |= BCM_FLOW_MATCH_FLEX_DATA_VALID;
    }

    /* non-flow match APIs could also use the same key type to add the entries
     * to this table. Make sure the right entries are traversed by validating
     * the objects.
     */
    if (!(info->valid_elements & (BCM_FLOW_MATCH_FLOW_PORT_VALID |
                                BCM_FLOW_MATCH_IIF_VALID |
                                BCM_FLOW_MATCH_VPN_VALID |
                                BCM_FLOW_MATCH_FLEX_DATA_VALID))) {
        return BCM_E_NOT_FOUND;
    } 
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_match_trunk_mem_traverse
 * Purpose:
 *      Read the source trunk memory table and call the user callback function 
 *      on each entry.
 * Parameters:
 *      unit    - (IN) Device Number
 *      cb      - (IN) user callback function
 *      user_data     - (IN) user context data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_match_trunk_mem_traverse(int unit, 
                             bcm_flow_match_traverse_cb cb,
                             void *user_data)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             ent_idx;
    int             buf_size, cfg_buf_size;
    uint32          *trunk_map_tbl;
    source_trunk_map_table_entry_t *entry;
    bcm_module_t local_modid;
    int mod_base;
    bcm_flow_match_config_t *info;
    uint32 port_type;
    uint32 tgid;
    uint32 svp;
    int i,j,cnt;
    int tid_found;
    int rv = BCM_E_NONE;
    soc_mem_t mem = SOURCE_TRUNK_MAP_TABLEm;
    uint32 iif;
    _bcm_flow_bookkeeping_t *flow_info;

    flow_info = FLOW_INFO(unit);

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    /* Get local module id. */
    rv = bcm_esw_stk_my_modid_get(unit, &local_modid);
    BCM_IF_ERROR_RETURN(rv);
    rv = _bcm_esw_src_modid_base_index_get(unit,local_modid,TRUE,&mod_base);
    BCM_IF_ERROR_RETURN(rv);

    /* validate the port index range */
    if ((mod_base + SOC_MODPORT_MAX(unit)) > 
               (soc_mem_index_max(unit, mem) + 1)) {
        return BCM_E_PARAM;
    }
    buf_size = sizeof(source_trunk_map_table_entry_t) * SOC_MODPORT_MAX(unit);
    trunk_map_tbl = soc_cm_salloc(unit, buf_size, 
                                  "flow match trunk map traverse");
    if (NULL == trunk_map_tbl) {
        return BCM_E_MEMORY;
    }

    /* allocate match config buffer */
    cfg_buf_size  = sizeof(bcm_flow_match_config_t) * SOC_MODPORT_MAX(unit);
    info = sal_alloc(cfg_buf_size, 
                                  "flow match trunk map traverse config");
    if (NULL == info) {
        soc_cm_sfree(unit, trunk_map_tbl);
        return BCM_E_MEMORY;
    }

    sal_memset((void *)trunk_map_tbl, 0, buf_size);
    sal_memset((void *)info, 0, cfg_buf_size);

    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, mod_base, 
                     mod_base + SOC_MODPORT_MAX(unit) - 1, trunk_map_tbl);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, trunk_map_tbl);
        sal_free(info);
        return BCM_E_MEMORY;
    }
    cnt = 0;
    for (ent_idx = 0; ent_idx < SOC_MODPORT_MAX(unit); ent_idx ++) {
        entry = soc_mem_table_idx_to_pointer(unit, mem, 
                      source_trunk_map_table_entry_t *,
                       trunk_map_tbl, ent_idx);
        port_type = soc_mem_field32_get(unit, mem, entry, PORT_TYPEf);
        tgid      = soc_mem_field32_get(unit, mem, entry, SRC_TGIDf);
        svp       = soc_mem_field32_get(unit, mem, entry, SOURCE_VPf);
        iif       = soc_mem_field32_get(unit, mem, entry, L3_IIFf);

        /* skip the entries not added by flow api */
        if (flow_info->iif_ref_cnt[iif] || flow_info->vp_ref_cnt[svp]) {
            if (port_type == 1) { /* trunk port type */
                BCM_GPORT_TRUNK_SET((info + cnt)->port,tgid);
            } else {
                BCM_GPORT_MODPORT_SET((info + cnt)->port,local_modid,ent_idx);
            }
            _SHR_GPORT_FLOW_PORT_ID_SET((info + cnt)->flow_port,svp);
            (info + cnt)->criteria = BCM_FLOW_MATCH_CRITERIA_PORT;
            cnt++;
        }
    }

    /* invoke the callback functions on each entry */
    for (i = 0; i < cnt; i++) {
        /* skip the same trunk id */
        tid_found = FALSE;
        for (j = 0; j < i; j++) {
            if (info[j].port == info[i].port) {
                tid_found = TRUE;
                break;
            }
        }
        if (tid_found) {
            continue;
        }
        rv = cb(unit,info + i,0,NULL,user_data);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, trunk_map_tbl);
            sal_free(info);
            return rv;
        }
    }       
    
    soc_cm_sfree(unit, trunk_map_tbl);
    sal_free(info);
    return rv;
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
_bcm_flow_match_traverse_mem(int unit, soc_mem_t mem,
                             bcm_flow_match_traverse_cb cb,
                             void *user_data)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of table we currently work on */
    uint32          *vt_tbl_chnk;
    uint32          *entry;
    int             valid, rv = BCM_E_NONE;
    bcm_flow_match_config_t info;
    uint32 num_of_fields = _BCM_FLOW_LOGICAL_FIELD_MAX;
    bcm_flow_logical_field_t field[_BCM_FLOW_LOGICAL_FIELD_MAX];

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    vt_tbl_chnk = soc_cm_salloc(unit, buf_size, "flow match traverse");
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
            rv = _bcm_flow_match_entry_valid(unit, mem, entry, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (!valid) {
                continue;
            }
            bcm_flow_match_config_t_init(&info);
            rv = _bcm_flow_match_entry_parse(unit, mem, entry, &info,
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
 *      _bcm_flow_match_traverse_tcam
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
_bcm_flow_match_traverse_tcam(int unit, soc_mem_t mem,
                             bcm_flow_match_traverse_cb cb,
                             void *user_data)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             idx,mem_idx_max;
    int i;
    uint32          entry[_QUAD_ENTRY_MAX];
    int             valid, rv = BCM_E_NONE;
    bcm_flow_match_config_t info;
    uint32 num_of_fields = _BCM_FLOW_LOGICAL_FIELD_MAX;
    bcm_flow_logical_field_t field[_BCM_FLOW_LOGICAL_FIELD_MAX];
    soc_tunnel_term_t tnl_entry;
    int entry_width = sizeof(tunnel_entry_t);
    int mode;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    idx = soc_mem_index_min(unit, mem);
    mem_idx_max = soc_mem_index_max(unit, mem);
    if ((idx < 0) || (mem_idx_max < 0)) {
        return BCM_E_MEMORY;
    }
    while (idx <= mem_idx_max) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                            (uint32 *)&tnl_entry.entry_arr[0]);
        SOC_IF_ERROR_RETURN(rv);

        rv = _bcm_flow_match_entry_valid(unit, mem, 
                    (uint32 *)&tnl_entry.entry_arr[0], &valid);
        if (BCM_FAILURE(rv)) {
            break;
        }
        if (!valid) {
            idx++;
            continue;
        }
        /* copy to the memory view format */
        sal_memcpy(entry,&tnl_entry.entry_arr[0],entry_width);

        /* mode:
         * 0 -- single wide
         * 2 -- double wide
         * 1 -- quad wide
         */
        mode = soc_mem_field32_get(unit, mem,
                     (uint32 *)&tnl_entry.entry_arr[0], MODEf);

        if (mode == _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE_WIDE) {
            idx++;
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                            (uint32 *)&tnl_entry.entry_arr[1]);
            SOC_IF_ERROR_RETURN(rv);
            sal_memcpy((uint8 *)entry + entry_width,
                       &tnl_entry.entry_arr[1],entry_width);

        } else if (mode == _BCM_FLOW_TD3_L3_TUNNEL_QUAD_WIDE) {
            for (i = 1; (i < COUNTOF(tnl_entry.entry_arr)) && 
                        (i < SOC_TNL_TERM_ENTRY_WIDTH_QUAD); i++) {
                idx++;
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                            (uint32 *)&tnl_entry.entry_arr[i]);
                SOC_IF_ERROR_RETURN(rv);
                sal_memcpy((uint8 *)entry + entry_width * i,
                       &tnl_entry.entry_arr[i],entry_width);
            }
        }
        idx++;

        bcm_flow_match_config_t_init(&info);
        rv = _bcm_flow_match_entry_parse(unit, mem, entry, &info,
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
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_match_traverse
 * Purpose:
 *      Traverse flow match entries and call the user callback function on 
 *      each entry. 
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN) Flow match config descriptor
 *      num_of_fields - (IN) number of logical fields
 *      field         - (IN) logical field array
 *      cb            - (IN) user callback function
 *      user_data     - (IN) user context data
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_match_traverse(
    int unit,
    bcm_flow_match_traverse_cb cb,
    void *user_data)
{
    /* XXX remove info, num_of_fields, field parameters. Add option field
     * to select traverse the port table or hash table: vxlate/mpls
     */
    soc_mem_t mem;
    int rv;
    _bcm_flow_bookkeeping_t *flow_info;

    if (cb == NULL) {
        return BCM_E_PARAM;
    }
    flow_info = FLOW_INFO(unit);

    /* traverse the port table */
    rv = _bcm_flow_match_trunk_mem_traverse(unit, cb, user_data);
    BCM_IF_ERROR_RETURN(rv);
   
    /* traverse the vlan_xlate_1 first */
    if (flow_info->mem_use_cnt[_BCM_FLOW_VXLT1_INX]) {
        mem = VLAN_XLATE_1_DOUBLEm;
        rv = _bcm_flow_match_traverse_mem(unit, mem, cb, user_data);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* traverse the vlan_xlate_2 */
    if (flow_info->mem_use_cnt[_BCM_FLOW_VXLT2_INX]) {
        mem = VLAN_XLATE_2_DOUBLEm;
        rv = _bcm_flow_match_traverse_mem(unit, mem, cb, user_data);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* traverse the mpls_entry next */
    if (flow_info->mem_use_cnt[_BCM_FLOW_MPLS_INX]) {
        mem = MPLS_ENTRYm;
        rv = _bcm_flow_match_traverse_mem(unit, mem, cb, user_data);
        BCM_IF_ERROR_RETURN(rv);

        mem = MPLS_ENTRY_SINGLEm;
        rv = _bcm_flow_match_traverse_mem(unit, mem, cb, user_data);
        BCM_IF_ERROR_RETURN(rv);
    }
    
    /* traverse the l3_tunnel  */
    if (flow_info->mem_use_cnt[_BCM_FLOW_TNL_INX]) {
        mem = L3_TUNNELm;
        rv = _bcm_flow_match_traverse_tcam(unit, mem, cb, user_data);
        BCM_IF_ERROR_RETURN(rv);
    }
    
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_match_svp_replace
 * Purpose:
 *      replace SVP in a flow match
 * Parameters:
 *      unit    - (IN) Device Number
 *      gport    - (IN) Flow gport
 *      new_vp - (IN) new VP used to repalce
 * Returns:
 *      BCM_E_XXX
 */
int bcmi_esw_flow_match_svp_replace(
    int unit,
    int gport,
    int new_vp)
{
    /* L3_TUNNELm could support 4 entry wide view definition */
    uint32 entry[_QUAD_ENTRY_MAX];
    int rv;
    _bcm_flow_bookkeeping_t *flow_info;
    int replaced_vp;
    int vp;
    soc_flow_db_mem_view_info_t view_info;
    bcm_flow_match_vp_t* match_key;
    bcm_flow_match_config_t *info;

    flow_info = FLOW_INFO(unit);

    /* validate the vp */
    vp = _SHR_GPORT_FLOW_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        LOG_ERROR(BSL_LS_BCM_FLOW,
              (BSL_META_U(unit, "flow port ID is not valid\n")));

        return BCM_E_PORT;
    }

    match_key = &flow_info->match_key[vp];
    info = sal_alloc(sizeof(*info),
                            "flow match svp replace");
    if (NULL == info) {
        return BCM_E_MEMORY;
    }
    sal_memset(info, 0, sizeof(*info));
    if (match_key->vlan) {
        info->valid_elements |= BCM_FLOW_MATCH_VLAN_VALID;
        info->vlan = match_key->vlan;
    }
    if (match_key->inner_vlan) {
        info->valid_elements |= BCM_FLOW_MATCH_INNER_VLAN_VALID;
        info->inner_vlan= match_key->inner_vlan;
    }
    if (match_key->port) {
        info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID;
        info->port = match_key->port;
    }
    if (match_key->sip) {
        info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
        info->sip = match_key->sip;
    }
    if (bcmi_flow_ip6_addr_is_valid(match_key->sip6)) {
        info->valid_elements |= BCM_FLOW_MATCH_SIP_V6_VALID;
        sal_memcpy(info->sip6, match_key->sip6, BCM_IP6_ADDRLEN);
    }
    info->criteria = match_key->criteria;
    info->flow_handle = match_key->flow_handle;
    info->flow_option = match_key->flow_option;

    _SHR_GPORT_FLOW_PORT_ID_SET(info->flow_port,new_vp);
    info->valid_elements |= BCM_FLOW_MATCH_FLOW_PORT_VALID;
    info->options = BCM_FLOW_MATCH_OPTION_REPLACE;

    /* port match doesn't use hash table */
    if (info->criteria == BCM_FLOW_MATCH_CRITERIA_PORT) {
        rv = _bcm_flow_match_port_ctrl(unit,info,
                 _BCM_FLOW_MATCH_PORT_CTRL_SET,&replaced_vp,NULL);
        if (SOC_FAILURE(rv)) {
            sal_free(info);
            return rv;
        }
        bcmi_esw_flow_lock(unit);
        if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
            vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            if (vp) {
                flow_info->vp_ref_cnt[vp]++;
            }
            if (replaced_vp) {
                if (flow_info->vp_ref_cnt[replaced_vp]) {
                    flow_info->vp_ref_cnt[replaced_vp]--;
                }
            }
        }
        bcmi_esw_flow_unlock(unit);
        sal_free(info);
        return BCM_E_NONE;
    }

    sal_memset(entry, 0, sizeof(entry));
    rv = _bcm_flow_match_key_set(unit,info,NULL,
                        0,entry,&view_info);
    if (SOC_FAILURE(rv)) {
        sal_free(info);
        return rv;
    }

    soc_mem_lock(unit,view_info.mem);
    /* set the match object and attributes and write to memory */
    rv = _bcm_flow_match_entry_set(unit,info,NULL,
              0,
              &view_info,
               entry, NULL ,&replaced_vp);
    soc_mem_unlock(unit,view_info.mem);

    bcmi_esw_flow_lock(unit);
    if (BCM_SUCCESS(rv)) {
        if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
            vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
            flow_info->vp_ref_cnt[vp]++;
            if (info->options & BCM_FLOW_MATCH_OPTION_REPLACE) {
                if (flow_info->vp_ref_cnt[replaced_vp]) {
                    flow_info->vp_ref_cnt[replaced_vp]--;
                }
            }
        }
    }
    bcmi_esw_flow_unlock(unit);
    sal_free(info);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_match_get
 * Purpose:
 *      get VP_LAG SVP from a flow match entry
 * Parameters:
 *      unit    - (IN) Device Number
 *      gport    - (IN) Flow gport
 *      vp_lag_vp - (IN) ptr of vp lag VP
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_match_svp_get(
    int unit,
    bcm_gport_t gport,
    int* vp_lag_vp)
{
    _bcm_flow_bookkeeping_t *flow_info;
    bcm_flow_match_vp_t* match_key;
    bcm_flow_match_config_t *info;
    int vp;
    int rv;

    flow_info = FLOW_INFO(unit);

    /* validate the vp */
    vp = _SHR_GPORT_FLOW_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeFlow)) {
        LOG_ERROR(BSL_LS_BCM_FLOW,
              (BSL_META_U(unit, "flow port ID is not valid\n")));
        return BCM_E_PORT;
    }

    match_key = &flow_info->match_key[vp];
    info = sal_alloc(sizeof(*info),
                            "flow match svp get");
    if (NULL == info) {
        return BCM_E_MEMORY;
    }
    sal_memset(info, 0, sizeof(*info));
    if (match_key->vlan) {
        info->valid_elements |= BCM_FLOW_MATCH_VLAN_VALID;
        info->vlan = match_key->vlan;
    }
    if (match_key->inner_vlan) {
        info->valid_elements |= BCM_FLOW_MATCH_INNER_VLAN_VALID;
        info->inner_vlan= match_key->inner_vlan;
    }
    if (match_key->port) {
        info->valid_elements |= BCM_FLOW_MATCH_PORT_VALID;
        info->port = match_key->port;
    }
    if (match_key->sip) {
        info->valid_elements |= BCM_FLOW_MATCH_SIP_VALID;
        info->sip = match_key->sip;
    }
    if (bcmi_flow_ip6_addr_is_valid(match_key->sip6)) {
        info->valid_elements |= BCM_FLOW_MATCH_SIP_V6_VALID;
        sal_memcpy(info->sip6, match_key->sip6, BCM_IP6_ADDRLEN);
    }
    info->criteria = match_key->criteria;
    info->flow_handle = match_key->flow_handle;
    info->flow_option = match_key->flow_option;

    rv = bcmi_esw_flow_match_get(unit, info, 0, NULL);
    if (SOC_FAILURE(rv)) {
        sal_free(info);
        return rv;
    }

    if (info->valid_elements & BCM_FLOW_MATCH_FLOW_PORT_VALID) {
        *vp_lag_vp = _SHR_GPORT_FLOW_PORT_ID_GET(info->flow_port);
    } else {
        sal_free(info);
        return BCM_E_NOT_FOUND;
    }

    sal_free(info);
    return BCM_E_NONE;
}


#endif  /* INCLUDE_L3 */

