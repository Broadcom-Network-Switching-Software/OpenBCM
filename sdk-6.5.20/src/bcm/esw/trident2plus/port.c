/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port driver.
 */

#include <soc/defs.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/td2_td2p.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/trx.h>
#include <bcm/error.h>
#include <bcm/port.h>


/*
 * FlexPort Operations Changes
 *
 * Flags to be used to determine the type of operations required
 * when the FlexPort API multi_set() is called.
 *
 * OP_NONE  - No changes.
 * OP_PMAP  - Change in port mapping.  It requires FlexPort sequence.
 * OP_LANES - Change in lanes.  It requires FlexPort sequence.
 * OP_SPEED - Change in speed.  This is covered in a FlexPort sequence.
 *            A change in speed may require a FlexPort sequence.
 *            If it is not required, then calling the 'speed' set
 *            function is sufficient.
 * OP_ENCAP - Change in encap mode.  It is NOT covered in a FlexPort
 *            sequence, so an explicit call must be made to change
 *            the encap mode.
 * OP_ALL   - All the above.  It requires FlexPort sequence.  
 */
#define BCM_TD2P_PORT_RESOURCE_OP_NONE     0
#define BCM_TD2P_PORT_RESOURCE_OP_PMAP    (1 << 0)
#define BCM_TD2P_PORT_RESOURCE_OP_LANES   (1 << 1)
#define BCM_TD2P_PORT_RESOURCE_OP_SPEED   (1 << 2)
#define BCM_TD2P_PORT_RESOURCE_OP_ENCAP   (1 << 3)
#define BCM_TD2P_PORT_RESOURCE_OP_ALL           \
    (BCM_TD2P_PORT_RESOURCE_OP_PMAP |           \
     BCM_TD2P_PORT_RESOURCE_OP_LANES |          \
     BCM_TD2P_PORT_RESOURCE_OP_SPEED |          \
     BCM_TD2P_PORT_RESOURCE_OP_ENCAP) 

soc_profile_mem_t *egr_pri_cng_profile[BCM_MAX_NUM_UNITS] = {NULL};

/*
 *  DSCP
 */
#define DSCP_CODE_POINT_CNT 64


/*
 * Internal BCM Port Resource Configuration
 */
typedef struct bcm_td2p_port_resource_s {
    uint32 flags;              /* SOC_PORT_RESOURCE_XXX */
    bcm_gport_t port;          /* Local logical port number to connect to
                                  the given physical port */
    int physical_port;         /* Local physical port, -1 if the logical to
                                  physical mapping is to be deleted */
    int speed;                 /* Initial speed after FlexPort operation */
    int lanes;                 /* Number of PHY lanes for this physical port */
    bcm_port_encap_t encap;    /* Encapsulation mode for port */
} bcm_td2p_port_resource_t;


/*
 * Forward static function declaration
 */
STATIC int
_bcm_td2p_port_resource_multi_set(int unit, 
                                  int nport, bcm_port_resource_t *resource);
STATIC int
_bcm_td2p_port_resource_speed_set(int unit, bcm_port_t port, int speed);

/*
 * Function Vector
 */
static bcm_esw_port_drv_t bcm_td2p_port_drv = {
    /* fn_drv_init                */ bcmi_td2p_port_fn_drv_init,
    /* resource_set               */ bcmi_xgs5_port_resource_set,
    /* resource_get               */ bcmi_xgs5_port_resource_get,
    /* resource_multi_set         */ _bcm_td2p_port_resource_multi_set,
    /* resource_speed_set         */ _bcm_td2p_port_resource_speed_set,
    /* resource_traverse          */ bcmi_xgs5_port_resource_traverse,
    /* port_redirect_set          */ NULL,
    /* port_redirect_get          */ NULL,
    /* port_enable_set            */ NULL,
    /* encap_speed_check          */ bcmi_xgs5_port_encap_speed_check,
    /* force_lb_set               */ NULL,
    /* force_lb_check             */ NULL,
    /* resource_status_update     */ bcmi_xgs5_port_resource_status_update
};


/*
 * Function:
 *      bcmi_td2p_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_td2p_port_fn_drv_init(int unit)
{
    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit, &bcm_td2p_port_drv, NULL, NULL));

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_td2p_port_trunk_src_trunk_map_table_entry_init
 * Purpose:
 *     This function is used by Flexport feature. It programs
 *     SOURCE_TRUNK_MAP_TABLE entries for a port to the values programmed
 *     during chip initialization.
 * Parameters:
 *     unit  - (IN) SOC unit number.
 *     port  - (IN) Source Port num.
 *     index - (OUT) Source mod port table index. Can be NULL if the caller is
 *                   not interested in the table's index corresponding to the
 *                   port. May be used for debugging.
 * Returns:
 *     BCM_E_xxx
 */
STATIC int
_bcm_td2p_port_trunk_src_trunk_map_table_entry_init(int unit, bcm_port_t port,
                                                    int *index)
{
    int my_modid;
    int src_trk_idx = 0;
    int rv;
    source_trunk_map_table_entry_t trunk_map_entry;

    /* Get module id for unit. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Get index to source trunk map table */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                              port,
                                                              &src_trk_idx));

    /* Read source trunk map table */
    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);
    rv = soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ALL,
                      src_trk_idx, &trunk_map_entry);

    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    /* Initialize relevant fields */
    soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &trunk_map_entry,
                                            PORT_TYPEf, 0);

    soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &trunk_map_entry,
                                            TGIDf, 0);

    /* Write entry to hw. */
    rv = soc_mem_write(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ALL,
                       src_trk_idx, &trunk_map_entry);

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);

    if ((index != NULL) && SOC_SUCCESS(rv)) {
        *index = src_trk_idx;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_td2p_port_trunk_port_attach
 * Purpose:
 *     This function initializes trunk tables for newly added port. It is
 *     used for Flexport feature.
 * Parameters:
 *     unit     - (IN) Unit number.
 *     port     - (IN) Logical port, BCM format.
 * Returns:
 *     BCM_E_NONE              Success.
 *     BCM_E_XXX
 */

STATIC int
_bcm_td2p_port_trunk_port_attach(int unit, bcm_port_t port)
{

    bcm_pbmp_t pbmp;
    higig_trunk_control_entry_t entry;
    int rv = BCM_E_NONE;
    int index = -1;

    BCM_IF_ERROR_RETURN(_bcm_td2p_port_trunk_src_trunk_map_table_entry_init(
                        unit, port, &index));

    LOG_INFO(BSL_LS_BCM_PORT,
             (BSL_META_U(unit,
                         "table_entry_init: unit=%d port=%d index=%d\n"),
                         unit, port, index));

    soc_mem_lock(unit, HIGIG_TRUNK_CONTROLm);

    rv = soc_mem_read(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ANY, 0, &entry);

    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);
        return (rv);
    }

    BCM_PBMP_CLEAR(pbmp);

    /* Read port bmap from hardware, and add new port to the bitmap */
    soc_mem_pbmp_field_get(unit, HIGIG_TRUNK_CONTROLm, &entry,
                           ACTIVE_PORT_BITMAPf, &pbmp);

    BCM_PBMP_PORT_ADD(pbmp, port);

    soc_mem_pbmp_field_set(unit, HIGIG_TRUNK_CONTROLm, &entry,
                           ACTIVE_PORT_BITMAPf, &pbmp);

    rv = soc_mem_write(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ALL, 0,
                       &entry);

    soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);

    return rv;
}

/*
 * Function:
 *     _bcm_td2p_port_trunk_port_detach
 * Purpose:
 *     This function updates the trunk tables for a deleted port. It is
 *     used for Flexport feature.
 * Parameters:
 *     unit     - (IN) Unit number.
 *     port     - (IN) Logical port, BCM format.
 * Returns:
 *     BCM_E_NONE              Success.
 *     BCM_E_XXX
 * Notes:
 *     The internal databases will be updated when a port is removed from trunk
 *     group, or when the trunk itself is destroyed
 */

STATIC int
_bcm_td2p_port_trunk_port_detach(int unit, bcm_port_t port)
{
    bcm_pbmp_t pbmp;
    higig_trunk_control_entry_t entry;
    int rv = BCM_E_NONE;

    soc_mem_lock(unit, HIGIG_TRUNK_CONTROLm);

    rv = soc_mem_read(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ANY, 0, &entry);

    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);
        return (rv);
    }

    BCM_PBMP_CLEAR(pbmp);

    /* Read port bmap from hardware, and remove port from the bitmap */
    soc_mem_pbmp_field_get(unit, HIGIG_TRUNK_CONTROLm, &entry,
                           ACTIVE_PORT_BITMAPf, &pbmp);

    BCM_PBMP_PORT_REMOVE(pbmp, port);

    soc_mem_pbmp_field_set(unit, HIGIG_TRUNK_CONTROLm, &entry,
                           ACTIVE_PORT_BITMAPf, &pbmp);

    rv = soc_mem_write(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ALL, 0,
                       &entry);

    soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);

    return rv;
}

/*
 * Function:
 *     _bcm_td2p_port_ipmc_port_state_change
 * Purpose:
 *     This function updates ipmc related fields in hardware tables/registers,
 *     for a specified port. It is used for Flexport feature.
 * Parameters:
 *     unit     - (IN) Unit number.
 *     port     - (IN) Logical port, BCM format.
 *     enable   - (IN) Port specific enable/disable of IPMC config and related
 *                     features. Set TRUE to enable them, FALSE to disable them
 * Returns:
 *     BCM_E_NONE              Success.
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_td2p_port_ipmc_port_state_change(int unit, bcm_port_t port, int enable)
{
    int do_vlan = soc_property_get(unit, spn_IPMC_DO_VLAN, 1);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit, port,
                            _bcmPortIpmcV4Enable, enable));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit, port,
                            _bcmPortIpmcV6Enable, enable));
    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit, port,
                            _bcmPortIpmcVlanKey, (enable && do_vlan) ? 1 : 0));

    /* Update LPORT Profile Table */
    BCM_IF_ERROR_RETURN(_bcm_lport_profile_field32_modify(unit,
                            LPORT_PROFILE_LPORT_TAB, V4IPMC_ENABLEf, enable));
    BCM_IF_ERROR_RETURN(_bcm_lport_profile_field32_modify(unit,
                            LPORT_PROFILE_LPORT_TAB, V6IPMC_ENABLEf, enable));
    BCM_IF_ERROR_RETURN(_bcm_lport_profile_field32_modify(unit,
                            LPORT_PROFILE_LPORT_TAB, IPMC_DO_VLANf,
                            (enable && do_vlan) ? 1 : 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_port_ipmc_port_attach
 * Purpose:
 *     This function programs hardware tables and registers for enabling IPMC
 *     functionality and related features. It is used for Flexport feature.
 * Parameters:
 *     unit     - (IN) Unit number.
 *     port     - (IN) Logical port, BCM format.
 * Returns:
 *     BCM_E_NONE              Success.
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_port_ipmc_port_attach(int unit, bcm_port_t port)
{
    if (soc_feature(unit, soc_feature_ip_mcast)) {
        BCM_IF_ERROR_RETURN(_bcm_td2p_port_ipmc_port_state_change(unit,
                                port, TRUE));

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
            BCM_IF_ERROR_RETURN(bcm_tr3_ipmc_port_attach_repl_init(unit, port));
        }

        if (IS_E_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_ipmc_egress_port_set(unit, port,
                                    _soc_mac_all_zeroes, 0, 0, 0));
        }
#endif
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_port_ipmc_port_detach
 * Purpose:
 *     This function clears hardware tables and registers in IPMC
 *     functionality and related features. It is used for Flexport feature.
 * Parameters:
 *     unit     - (IN) Unit number.
 *     port     - (IN) Logical port, BCM format.
 * Returns:
 *     BCM_E_NONE              Success.
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_port_ipmc_port_detach(int unit, bcm_port_t port)
{
    if (soc_feature(unit, soc_feature_ip_mcast)) {
        BCM_IF_ERROR_RETURN(_bcm_td2p_port_ipmc_port_state_change(unit,
                                port, FALSE));

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
            /* See bcm_tr_ipmc_detach, bcm_tr3_ipmc_repl_detach for more
             * information
             */
            BCM_IF_ERROR_RETURN(bcm_tr3_ipmc_port_detach_repl_deinit(unit, port));
        }
#endif
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_rate_port_attach
 * Purpose:
 *      Initialize port related information in the Rate module.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_port_rate_port_attach(int unit, bcm_port_t port)
{
    soc_field_t fields[] = {PACKET_QUANTUMf, BYTE_MODEf};
    uint32 values[] = {0x100, 1};


#ifdef BCM_FIREBOLT6_SUPPORT
    if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {


        SOC_IF_ERROR_RETURN(soc_mem_write(unit,
           STORM_CONTROL_METER_CONFIGm, MEM_BLOCK_ALL, port,
           soc_mem_entry_null(unit, STORM_CONTROL_METER_CONFIGm)));


        SOC_IF_ERROR_RETURN(soc_mem_fields32_modify(unit,
            STORM_CONTROL_METER_CONFIGm, port, COUNTOF(fields),
            fields, values));
    } else
#endif
    {

        SOC_IF_ERROR_RETURN
            (WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, 0));

        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit,
                 STORM_CONTROL_METER_CONFIGr,
                 port, COUNTOF(fields),
                 fields, values));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_rate_port_detach
 * Purpose:
 *      Clear port related information in the Rate module.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_port_rate_port_detach(int unit, bcm_port_t port)
{
    SOC_IF_ERROR_RETURN
        (WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, 0));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_stack_port_attach
 * Purpose:
 *      Initialize port related information in the Stack module.
 *      Set my_modid and other modid related initialization.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes port is valid.
 */
int
_bcm_td2p_port_stack_port_attach(int unit, bcm_port_t port)
{
    modport_map_subport_entry_t subport_entry;
    modport_map_subport_mirror_entry_t subport_m_entry;

    /* Configure logical subport numbers */
    sal_memset(&subport_entry, 0, sizeof(subport_entry));
    soc_MODPORT_MAP_SUBPORTm_field32_set(unit, &subport_entry, ENABLEf, 1);
    soc_MODPORT_MAP_SUBPORTm_field32_set(unit, &subport_entry, DESTf, port);
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ALL,
                                    port, &subport_entry));

    /* Configure logical subport mirror numbers */
    sal_memset(&subport_m_entry, 0, sizeof(subport_m_entry));
    soc_MODPORT_MAP_SUBPORT_M0m_field32_set(unit, &subport_m_entry,
                                            ENABLEf, 1);
    soc_MODPORT_MAP_SUBPORT_M0m_field32_set(unit, &subport_m_entry,
                                            DESTf, port);
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORT_MIRRORm(unit, MEM_BLOCK_ALL,
                                           port, &subport_m_entry));

    /* Initialize modport map profiles ptr on port.*/
    if (soc_feature(unit, soc_feature_modport_map_dest_is_port_or_trunk)) {
        BCM_IF_ERROR_RETURN
            (bcm_td_stk_modport_map_port_attach(unit, port));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_stack_port_detach
 * Purpose:
 *      Clear port related information in the Stack module.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes port is valid.
 */
int
_bcm_td2p_port_stack_port_detach(int unit, bcm_port_t port)
{
    modport_map_subport_entry_t subport_entry;
    modport_map_subport_mirror_entry_t subport_m_entry;

    /* Clear logical subport numbers */
    sal_memset(&subport_entry, 0, sizeof(subport_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORTm(unit, MEM_BLOCK_ALL,
                                    port, &subport_entry));

    /* Clear logical subport mirror numbers */
    sal_memset(&subport_m_entry, 0, sizeof(subport_m_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_MODPORT_MAP_SUBPORT_MIRRORm(unit, MEM_BLOCK_ALL,
                                       port, &subport_m_entry));

    /* detach port from modport map profiles */
    if (soc_feature(unit, soc_feature_modport_map_dest_is_port_or_trunk)) {
        BCM_IF_ERROR_RETURN
            (bcm_td_stk_modport_map_port_detach(unit, port));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_dscp_table_default_add
 * Purpose:
 *      Add DSCP default profile entry to given port.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes port is valid.
 */
STATIC int
_bcm_td2p_port_dscp_table_default_add(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_tab_set(unit, port,
                               _BCM_CPU_TABS_ETHER,
                               TRUST_DSCP_PTRf,
                               0));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_outer_tpid_ref_count_update
 * Purpose:
 *      Update TPID reference count.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 *      add    - (IN) Indicates whether port is added or deleted from BCM.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_td2p_port_outer_tpid_ref_count_update(int unit, bcm_port_t port, int add)
{
    egr_vlan_control_1_entry_t entry;
    int index;
    int enabled;

    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_field_get(unit, port,
            EGR_VLAN_CONTROL_1m, OUTER_TPID_INDEXf, (void *)&index));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &entry));
        index = soc_EGR_VLAN_CONTROL_1m_field32_get(unit, &entry,
                                                    OUTER_TPID_INDEXf);
    }
    if (add) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1));
    } else {
        /* Ignore error when deleting */
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, index);
    }

    enabled = 0;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_config_get(unit, port,
                                  _bcmPortOuterTpidEnables, &enabled));
    index = 0;
    while (enabled) {
        if (enabled & 1) {
            if (add) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1));
            } else {
                /* Ignore error when deleting */
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, index);
            }
        }
        enabled = enabled >> 1;
        index++;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_priority_map_init
 * Purpose:
 *      Initialize priority map for given port.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_td2p_port_priority_map_init(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;

    if (soc_feature(unit, soc_feature_egr_qos_combo_profile)) {
       rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_QOS_PROFILE_INDEXf, 0);
       if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else if (SOC_IS_TRIDENT3X(unit)) {
       rv = _bcm_esw_egr_port_tab_set(unit, port, EGR_QOS_PROFILE_INDEXf, 0);
       if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_port_software_free
 * Purpose:
 *      Deallocates memory for a given port from the BCM PORT module
 *      data structure.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes main BCM PORT module initialization has been executed.
 *      - Assumes port is valid.
 */
STATIC int
_bcm_td2p_port_software_free(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        return BCM_E_INTERNAL;
    }

    /* Proto-based VLAN_DATA state */
    if (pinfo->p_vd_pbvl != NULL) {
        sal_free(pinfo->p_vd_pbvl);
        pinfo->p_vd_pbvl = NULL;
    }

    /* End-to-end congestion control configuration */
    if (pinfo->e2ecc_config != NULL) {
        sal_free(pinfo->e2ecc_config);
        pinfo->e2ecc_config = NULL;
    }

    /* Reset all fields */
    sal_memset(pinfo, 0, sizeof(_bcm_port_info_t));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_software_alloc
 * Purpose:
 *      Allocates software memory in the BCM PORT module for given port.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes main BCM PORT module initialization has been executed.
 *      - Assumes port is valid.
 */
STATIC int
_bcm_td2p_port_software_alloc(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;
    int idxmax = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    int inds_bytes = (idxmax + (_BCM_PORT_VD_PBVL_ESIZE -  1)) / \
        _BCM_PORT_VD_PBVL_ESIZE; /* Round to the next entry */

    /* Free memory in case this is already allocated */
    BCM_IF_ERROR_RETURN(_bcm_td2p_port_software_free(unit, port));

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to get PORT info data for "
                              "unit=%d port=%d\n"),
                   unit, port));
        return BCM_E_INTERNAL;
    }

    /* Reset all fields */
    sal_memset(pinfo, 0, sizeof(_bcm_port_info_t));

    /* Proto-based VLAN_DATA state */
    pinfo->p_vd_pbvl = sal_alloc(inds_bytes, "vdv_info");
    if (pinfo->p_vd_pbvl == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for PORT vlan "
                              "unit=%d port=%d\n"),
                   unit, port));
        return BCM_E_MEMORY;
    }
    sal_memset(pinfo->p_vd_pbvl, 0, inds_bytes);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_bridge_port_init
 * Purpose:
 *      This function initializes PORT_BRIDGE_BMAP memory (for a normal port),
 *      and/or PORT_BRIDGE_MIRROR_BMAP (for a Higig port). The initialization is
 *      required for ensuring that port bit in one of the above memories
 *      (depending on port type), and PORT_TAB.PORT_BRIDGE bit have the same
 *      value. See notes.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 *      value  - 1 Set bit in PORT_BRIDGE_BMAP and/or PORT_BRIDGE_MIRROR_BMAP
 *                 memory.
 *               0 Reset bit in PORT_BRIDGE_BMAP and/or PORT_BRIDGE_MIRROR_BMAP
 *                 memory.
 *                 See Notes.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - PORT_TAB.PORT_BRIDGE is reset in function
 *        mbcm_driver[unit]->mbcm_port_cfg_init()
 *      - If this function is called outside of port attach and detach
 *        functions then the caller must make sure that PORT_TAB.PORT_BRIDGE is
 *        programmed to the same value as in these memories, for a given port
 *        Refer to memory description of these two memories for more information
 *      - Assumes port is valid.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_td2p_port_bridge_port_init(int unit, bcm_port_t port, int value)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t  pbmp;

    if (IS_E_PORT(unit, port)
        || IS_CPU_PORT(unit, port)
        || IS_HG_PORT(unit, port)) {
        port_bridge_bmap_entry_t entry;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_BRIDGE_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
        soc_mem_pbmp_field_get(unit, PORT_BRIDGE_BMAPm, &entry,
                               BITMAPf, &pbmp);

        if (value) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        } else {
            SOC_PBMP_PORT_REMOVE(pbmp, port);
        }

        soc_mem_pbmp_field_set(unit, PORT_BRIDGE_BMAPm, &entry,
                               BITMAPf, &pbmp);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_BRIDGE_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
    }

    if (IS_HG_PORT(unit, port)) {
        port_bridge_mirror_bmap_entry_t entry;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_BRIDGE_MIRROR_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
        soc_mem_pbmp_field_get(unit, PORT_BRIDGE_MIRROR_BMAPm, &entry,
                               BITMAPf, &pbmp);
        if (value) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        } else {
            SOC_PBMP_PORT_REMOVE(pbmp, port);
        }
        soc_mem_pbmp_field_set(unit, PORT_BRIDGE_MIRROR_BMAPm, &entry,
                               BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_BRIDGE_MIRROR_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2_port_basic_init
 * Purpose:
 *      Initialize the BCM PORT module portion (basic) for a given port
 *      (bcm_esw_port_init).
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes main BCM PORT module initialization has been executed.
 *      - Assumes port is valid.
 *      - Caller must call other corresponding module routines
 *        that need to initialize any port related information.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_td2_port_basic_init(int unit, bcm_port_t port)
{
    int rv;
    _bcm_port_info_t *pinfo;
    bcm_vlan_action_set_t action;
    bcm_pbmp_t port_pbmp;
    bcm_vlan_data_t vd;
    int length_check;
    uint32 addr;
    int block;
    uint8 acc_type;
    uint32 rval;
    uint32 profile_index;
    int blk;
    int blk_num = -1;
    int bindex = 0;
    int tx_pbm = 0;
    uint32 rx_los;
    int enable;
    int value;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "BCM Port Basic Init unit=%d port=%d(%s)\n"),
                 unit, port, SOC_PORT_NAME(unit, port)));

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to get PORT info data for "
                              "unit=%d port=%d\n"),
                   unit, port));
        return BCM_E_INTERNAL;
    }


    /**********
     * VLAN TPID reference count
     * Priority Map
     * Double-Tag mode
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_outer_tpid_ref_count_update(unit, port, 1));
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_priority_map_init(unit, port));

    pinfo->dtag_mode = BCM_PORT_DTAG_MODE_NONE;

    /**********
     * VLAN Action
     * soc_feature_vlan_action
     */
    bcm_vlan_action_set_t_init(&action);
    pinfo->vp_count = 0;
    if (!IS_LB_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_port_vlan_prot_index_alloc(unit, &(pinfo->vlan_prot_ptr)));
    }

    /* Update Egress VLAN profile table count */
    _bcm_trx_egr_vlan_action_profile_entry_increment
        (unit, BCM_TRX_EGR_VLAN_ACTION_PROFILE_DEFAULT);

    if (IS_HG_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_egress_default_action_get(unit,
                                                      port, &action));
        /* Backward compatible defaults */
        action.ot_outer = bcmVlanActionDelete;
        action.dt_outer = bcmVlanActionDelete;
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_egress_default_action_set(unit,
                                                          port, &action));
    }

    /**********
     * Write port configuration tables to contain the Initial System
     * Configuration (see init.c).
     *
     * Note:  Logically, it would make more sense to call mbcm_port_cfg_init()
     * and bcm_port_settings_init() first.  However, we are maintaining
     * the same order sequence as in bcm_esw_port_init() to avoid
     * introducing dependencies issues on code above. 
     */
    SOC_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_ASSIGN(port_pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(port_pbmp, PBMP_TDM(unit));

    vd.vlan_tag = BCM_VLAN_DEFAULT;
    BCM_PBMP_ASSIGN(vd.port_bitmap, port_pbmp);
    BCM_PBMP_ASSIGN(vd.ut_port_bitmap, port_pbmp);
    BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_init(unit, port, &vd));

    BCM_IF_ERROR_RETURN(_bcm_td2p_port_bridge_port_init(unit, port, 0));

    BCM_IF_ERROR_RETURN(bcm_port_settings_init(unit, port));

    /**********
     * DSCP Profile
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_dscp_table_default_add(unit, port));

    /**********
     * COS_MAP Profile
     */
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_td2p_port_cosq_config_set(unit, port, TRUE));
    }

    if (soc_feature(unit, soc_feature_pgw_mac_rsv_mask_remap)) {
        /**********
         * Frame Length Check
         */
        length_check = soc_property_get(unit, spn_MAC_LENGTH_CHECK_ENABLE, 0);
        addr = soc_reg_addr_get(unit, PGW_MAC_RSV_MASKr, port, 0, FALSE, &block,
                                &acc_type);
        addr &= 0xffffff00;
        addr |= (SOC_INFO(unit).port_l2p_mapping[port] - 1) & 0xf;
        SOC_IF_ERROR_RETURN
            (_soc_reg32_get(unit, block, acc_type, addr, &rval));
        if (length_check) {
            rval |= 0x20;  /* Set bit 5 to enable frame length check */
        } else {
            rval &= ~0x20; /* Clear bit 5 to disable frame length check */
        }
        rval |= 1 << 18;   /* Set PFC frame detected indicator */
        SOC_IF_ERROR_RETURN
            (_soc_reg32_set(unit, block, acc_type, addr, rval));
    }

#ifdef BCM_RCPU_SUPPORT
    if ((uint32)port == soc_property_get(unit, spn_RCPU_PORT, -1)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_frame_max_set(unit, port, BCM_PORT_JUMBO_MAXSZ));
    }
#endif /* BCM_RCPU_SUPPORT */

    /**********
     * Outer TPID
     * soc_feature_vlan_ctrl
     */
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_tpid_set(unit, port,
                               _bcm_fb2_outer_tpid_default_get(unit)));

    /**********
     * Profile Indexes
     */
    /* Initialize egress block profile pointer to invalid value */
    BCM_IF_ERROR_RETURN
        (bcmi_esw_port_egr_prof_ptr_set(unit, port, -1));

    /* Program the VLAN_PROTOCOL_DATA / FP_PORT_FIELD_SEL_INDEX
       / PROTOCOL_PKT_INDEX pointers */
    if (!IS_LB_PORT(unit, port) && !IS_TDM_PORT(unit, port)) {
        if (soc_mem_index_max(unit, VLAN_PROTOCOLm) > 0) {
            value = pinfo->vlan_prot_ptr /
            soc_mem_index_count(unit, VLAN_PROTOCOLm);

            BCM_IF_ERROR_RETURN
                (_bcm_esw_port_tab_set(unit, port,
                                       _BCM_CPU_TABS_ETHER,
                                       VLAN_PROTOCOL_DATA_INDEXf,
                                       value));
        }

        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port,
                                   _BCM_CPU_TABS_ETHER,
                                   FP_PORT_FIELD_SEL_INDEXf,
                                   port));
        BCM_IF_ERROR_RETURN
            (_bcm_prot_pkt_ctrl_add(unit, 0, 0, &profile_index));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port,
                                   _BCM_CPU_TABS_ETHER,
                                   PROTOCOL_PKT_INDEXf,
                                   profile_index));
    }

    /********
     * E2ECC
     *
     * Ports are enabled to send E2ECC messages by default.
     * Disable E2ECC message TX here until congestion config
     * set enables the port.
     */
    blk = SOC_PORT_BLOCK(unit, SOC_INFO(unit).port_l2p_mapping[port]);
    bindex = SOC_PORT_BINDEX(unit, SOC_INFO(unit).port_l2p_mapping[port]);
    blk_num = SOC_BLOCK_INFO(unit, blk).number;

    SOC_IF_ERROR_RETURN(READ_E2ECC_TX_ENABLE_BMPr(unit, blk_num, &rval));
    tx_pbm = soc_reg_field_get(unit, E2ECC_TX_ENABLE_BMPr, rval,
                               TX_ENABLE_BMPf);
    tx_pbm &= ~(1 << bindex);
    soc_reg_field_set(unit, E2ECC_TX_ENABLE_BMPr, &rval,
                      TX_ENABLE_BMPf, tx_pbm);
    SOC_IF_ERROR_RETURN(WRITE_E2ECC_TX_ENABLE_BMPr(unit, blk_num, rval));

    /**********
     * EEE Config
     * soc_feature_eee
     */
    if (!IS_LB_PORT(unit, port) && !IS_CPU_PORT (unit, port) &&
        !IS_TDM_PORT(unit, port)) {
        /* EEE standard compliance Work Around:
         * Initialize the software state of native eee in MAC
         */
        BCM_IF_ERROR_RETURN
            (bcmi_esw_port_eee_cfg_set(unit, port, 0));
        /* Check if MAC supports Native EEE and set the value of
         * eee_cfg by reading the EEE status from MAC*/
        rv = bcmi_esw_portctrl_eee_enable_get(unit, port, &value);
        if (rv != BCM_E_UNAVAIL) {
            BCM_IF_ERROR_RETURN
                (bcmi_esw_port_eee_cfg_set(unit, port, value));
        }
    }

    /**********
     * HIGIG
     */
    if (IS_HG_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (bcmi_esw_portctrl_higig_mode_set(unit, port, TRUE));
#ifdef BCM_HIGIG2_SUPPORT
        /* soc_feature_higig2 */
        BCM_IF_ERROR_RETURN
            (bcmi_esw_portctrl_higig2_mode_set(unit, port, TRUE));
        SOC_HG2_ENABLED_PORT_ADD(unit, port);
#endif /* BCM_HIGIG2_SUPPORT */
    } else {  /* IEEE */
        BCM_IF_ERROR_RETURN
            (bcmi_esw_portctrl_higig_mode_set(unit, port, FALSE));
#ifdef BCM_HIGIG2_SUPPORT
        /* soc_feature_higig2 */
        BCM_IF_ERROR_RETURN
            (bcmi_esw_portctrl_higig2_mode_set(unit, port, FALSE));
        SOC_HG2_ENABLED_PORT_REMOVE(unit, port);
#endif /* BCM_HIGIG2_SUPPORT */
    }

    /***********
     * SW RX LOS configuration from PHY driver
     */
    if (IS_E_PORT(unit, port)) {
        rv = bcm_esw_port_phy_control_get(unit, port,
                                          BCM_PORT_PHY_CONTROL_SOFTWARE_RX_LOS,
                                          &rx_los);
        if (BCM_SUCCESS(rv)) {
            pinfo->rx_los = rx_los;
        }
    }

    /***********
     * SW port enable status from HW
     */
    rv = bcm_esw_port_enable_get(unit, port, &enable);
    if (BCM_SUCCESS(rv)) {
        pinfo->enable = enable;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_port_basic_attach
 * Purpose:
 *      Initialize the BCM PORT module portion (basic) for a given port
 *      (bcm_esw_port_init) and allocates any memory needed by the BCM PORT
 *      module.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes main BCM PORT module initialization has been executed.
 *      - Assumes port is valid.
 *      - Caller must call other corresponding module routines
 *        that need to initialize any port related information.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_td2_port_basic_attach(int unit, bcm_port_t port)
{
    int rv, rv1;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "BCM Attach Basic unit=%d port=%d(%s)\n"),
                 unit, port, SOC_PORT_NAME(unit, port)));

    /* Allocate SW memory needed by PORT module */
    BCM_IF_ERROR_RETURN(_bcm_td2p_port_software_alloc(unit, port));

    /* Initialize PORT module */
    rv = _bcm_td2_port_basic_init(unit, port);

    /*
     * Only free memory during failure.
     * Memory is used by PORT module while port exists (attached).
     */
    if (BCM_FAILURE(rv)) {
        rv1 = _bcm_td2p_port_software_free(unit, port);
        if (BCM_FAILURE(rv1)) {
            rv = rv1;
        }
    }

    return rv;
}


/*
 * Function:
 *      _bcm_td2_port_basic_detach
 * Purpose:
 *      Clear the BCM PORT module portion (basic) for a given port.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes port is valid.
 *      - Caller must call other corresponding module routines
 *        where any port related information needs to be cleared.
 */
STATIC int
_bcm_td2_port_basic_detach(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;
    bcm_pbmp_t port_pbmp;
    bcm_vlan_data_t vd;
    int value;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "BCM Detach Basic unit=%d port=%d(%s)\n"),
                 unit, port, SOC_PORT_NAME(unit, port)));

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to get PORT info data for "
                              "unit=%d port=%d\n"),
                   unit, port));
        return BCM_E_INTERNAL;
    }

    /**********
     * COS_MAP Profile
     */
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        BCM_IF_ERROR_RETURN
            (bcm_td2p_port_cosq_config_set(unit, port, FALSE));
    }

    /**********
     * HIGIG
     */
    BCM_IF_ERROR_RETURN
        (bcmi_esw_portctrl_higig_mode_set(unit, port, FALSE));
#ifdef BCM_HIGIG2_SUPPORT
    /* soc_feature_higig2 */
    BCM_IF_ERROR_RETURN
        (bcmi_esw_portctrl_higig2_mode_set(unit, port, FALSE));
    SOC_HG2_ENABLED_PORT_REMOVE(unit, port);
#endif /* BCM_HIGIG2_SUPPORT */

    /*******
     * EEE Config
     */
    if (!IS_LB_PORT(unit, port) && !IS_CPU_PORT (unit, port) &&
        !IS_TDM_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (bcmi_esw_port_eee_cfg_set(unit, port, 0));
    }

    /**********
     * Profile Indexes
     */
    /* Program the VLAN_PROTOCOL_DATA / FP_PORT_FIELD_SEL_INDEX
       / PROTOCOL_PKT_INDEX pointers */
    if (!IS_LB_PORT(unit, port) && !IS_TDM_PORT(unit, port)) {
        if (soc_mem_index_max(unit, VLAN_PROTOCOLm) > 0) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_port_tab_set(unit, port,
                                       _BCM_CPU_TABS_ETHER,
                                       VLAN_PROTOCOL_DATA_INDEXf,
                                       0));
        }

        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_set(unit, port,
                                   _BCM_CPU_TABS_ETHER,
                                   FP_PORT_FIELD_SEL_INDEXf,
                                   0));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_tab_get(unit, port,
                                   PROTOCOL_PKT_INDEXf,
                                   &value));
        BCM_IF_ERROR_RETURN
            (_bcm_prot_pkt_ctrl_delete(unit, value));
    }
    /* Initialize egress block profile pointer to invalid value */
    BCM_IF_ERROR_RETURN
        (bcmi_esw_port_egr_prof_ptr_set(unit, port, -1));

    /**********
     * VLAN Action
     * soc_feature_vlan_action
     */
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_port_egress_default_action_delete(unit, port));

    if (!IS_LB_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_port_vlan_prot_index_free(unit, pinfo->vlan_prot_ptr));
    }

    /**********
     * VLAN TPID reference count
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_outer_tpid_ref_count_update(unit, port, 0));

    /**********
     * Init port configuration tables
     */
    SOC_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_ASSIGN(port_pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(port_pbmp, PBMP_TDM(unit));

    vd.vlan_tag = BCM_VLAN_NONE;
    BCM_PBMP_ASSIGN(vd.port_bitmap, port_pbmp);
    BCM_PBMP_ASSIGN(vd.ut_port_bitmap, port_pbmp);
    BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_init(unit, port, &vd));

    BCM_IF_ERROR_RETURN(_bcm_td2p_port_bridge_port_init(unit, port, 0));

    BCM_IF_ERROR_RETURN(bcm_port_settings_init(unit, port));

    /**********
     * Reset fields
     * Deallocate SW memory
     */
    BCM_IF_ERROR_RETURN(_bcm_td2p_port_software_free(unit, port));

    return BCM_E_NONE;
}

#ifdef INCLUDE_L3

/*
 * Function:
 *      _bcm_td2p_vfi_membership_change
 * Purpose:
 *  Add/remove a port to/from the vlan_vfi_membership profile at the given
 *  index. This is mainly used for adding a port to the default profile 1
 *  for VFI membership. By default, the default porfile should contain all
 *  port numbers.  Normally profile should be accessed through 
 *  soc_profile_mem_xxx functions.
 *  
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vfi_membership_change(int unit, int profile_inx,bcm_port_t port, int add)
{
    bcm_pbmp_t pbmp;
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    egr_vlan_vfi_membership_entry_t egr_vlan_vfi;

    BCM_PBMP_CLEAR(pbmp);
    BCM_IF_ERROR_RETURN(READ_ING_VLAN_VFI_MEMBERSHIPm(unit, 
          MEM_BLOCK_ANY, profile_inx, &ing_vlan_vfi));

    soc_mem_pbmp_field_get(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &pbmp);
     if (add) {
         BCM_PBMP_PORT_ADD(pbmp, port);
     } else {
         BCM_PBMP_PORT_REMOVE(pbmp, port);
     }
     soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &pbmp);
     BCM_IF_ERROR_RETURN(WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, 
               MEM_BLOCK_ALL, profile_inx, &ing_vlan_vfi));

    BCM_PBMP_CLEAR(pbmp);
    BCM_IF_ERROR_RETURN(READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, 
          MEM_BLOCK_ANY, profile_inx, &egr_vlan_vfi));

    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &pbmp);
     if (add) {
         BCM_PBMP_PORT_ADD(pbmp, port);
     } else {
         BCM_PBMP_PORT_REMOVE(pbmp, port);
     }

     soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &pbmp);
     BCM_IF_ERROR_RETURN(WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit,
               MEM_BLOCK_ALL, profile_inx, &egr_vlan_vfi));
     return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vfi_membership_port_attach
 * Purpose:
 *  Add a port to the default vfi membership profile
 * 
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2p_vfi_membership_port_attach(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(_bcm_td2p_vfi_membership_change(unit,1, port,TRUE));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vfi_membership_port_detach
 * Purpose:
 *  Remove a port from the default vfi membership profile
 *
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2p_vfi_membership_port_detach(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(_bcm_td2p_vfi_membership_change(unit,1, port,FALSE));
    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_td2p_port_attach
 * Purpose:
 *      Attach given port to the BCM layer and initialize it
 *      to its default state.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes port is a valid BCM logical port.
 *      - Assumes function is called when a new port is added into the system,
 *        so it is not called during WarmBoot.
 *      - Assumes BCM PORT module initialization routine has
 *        been called.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_td2p_port_attach(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
#ifndef BCM_VLAN_NO_DEFAULT_ETHER
    bcm_pbmp_t pbmp;
#endif

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "BCM Attach unit=%d port=%d(%s)\n"),
                 unit, port, SOC_PORT_NAME(unit, port)));

    /*
     * Initialization of port logic in BCM modules
     *
     * There are order dependencies among the BCM modules.
     * This follows the current order of execution in _bcm_modules_init()
     * which is well established.
     */

    /* PORT */
    rv = _bcm_td2_port_basic_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize basic port "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* STG */
    rv = bcm_esw_vlan_stp_set(unit, BCM_VLAN_DEFAULT,
                              port, BCM_STG_STP_FORWARD);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to set port STP for default VLAN "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* VLAN */
#ifndef BCM_VLAN_NO_DEFAULT_ETHER
    /* Add ports to default VLAN only if vlan policy allows it */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_esw_vlan_port_add(unit, BCM_VLAN_DEFAULT, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to add port to default VLAN "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
#endif


    /* TRUNK */
    rv = _bcm_td2p_port_trunk_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize port "
                              "Source Trunk Map table "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* LINKSCAN */
    rv = _soc_linkscan_hw_port_init(unit, port);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize port Linkscan "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* STACK */
    rv = _bcm_td2p_port_stack_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize port Stack "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* STAT */
    rv = bcmi_td2p_stat_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize port Stat "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* RATE */
    rv = _bcm_td2p_port_rate_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize port Rate "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

#ifdef BCM_FIELD_SUPPORT
    /* FIELD */
    rv = bcm_esw_port_control_set(unit, port, bcmPortControlFilterLookup, 1);
    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterIngress, 1);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterEgress, 1);
    }
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize port Field "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
#if defined(BCM_TRX_SUPPORT)
    rv = _bcm_field_flex_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to attach port in Field Port Based Groups"
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
#endif
#endif /* BCM_FIELD_SUPPORT */

    /* MIRROR */
    rv = bcmi_esw_mirror_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize port Mirror "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

#ifdef INCLUDE_L3
    /* L3 */
    if (!IS_ST_PORT(unit, port)) {
        rv = bcm_esw_port_control_set(unit, port, bcmPortControlIP4, 1);
        if (BCM_SUCCESS(rv)) {
            rv = bcm_esw_port_control_set(unit, port, bcmPortControlIP6, 1);
        }
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to enable port L3 "
                                  "unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }

    /* IPMC */
    rv = _bcm_td2p_port_ipmc_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to enable port IPMC "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

#ifdef BCM_MPLS_SUPPORT
    /* MPLS */
    if (!IS_ST_PORT(unit, port)) {
        rv = bcm_esw_port_control_set(unit, port, bcmPortControlMpls, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to enable port MPLS "
                                  "unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }
#endif /* BCM_MPLS_SUPPORT */

    /* MIM */
    if (!IS_ST_PORT(unit, port) && soc_feature(unit, soc_feature_mim)) {
        rv = bcm_esw_port_control_set(unit, port, bcmPortControlMacInMac, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to enable port MIM "
                                  "unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }

    /* VFI */
    rv = _bcm_td2p_vfi_membership_port_attach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to add port to default VFI membership "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
#endif /* INCLUDE_L3 */

    /* PFC */
    rv = bcm_esw_port_control_set(unit, port, bcmPortControlPFCPassFrames, 0);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to initialize PFC settings "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    if (SOC_IS_TRIDENT2PLUS(unit)) {
        rv = bcm_td2_cosq_default_llfc_profile_attach(unit, port);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to add default entries for PRIO2COS_PROFILE"
                                  "register profile unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_detach
 * Purpose:
 *      Detach given port from the BCM layer and clear any data
 *      that is normally configured when a port is initialized.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes port is a valid BCM logical port.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_td2p_port_detach(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "BCM Detach unit=%d port=%d(%s)\n"),
                 unit, port, SOC_PORT_NAME(unit, port)));

    /*
     * Clear of port logic in BCM modules
     *
     * For the most part, this follows the reverse logic
     * of its counter part function _bcm_td2p_port_attach().
     */

#ifdef INCLUDE_L3
    /* MIM */
    if (!IS_ST_PORT(unit, port) && soc_feature(unit, soc_feature_mim)) {
        rv = bcm_esw_port_control_set(unit, port, bcmPortControlMacInMac, 0);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to clear port MIM "
                                  "unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }

#ifdef BCM_MPLS_SUPPORT
    /* MPLS */
    if (!IS_ST_PORT(unit, port)) {
        rv = bcm_esw_port_control_set(unit, port, bcmPortControlMpls, 0);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to clear port MPLS "
                                  "unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }
#endif /* BCM_MPLS_SUPPORT */

    /* IPMC */
    rv = _bcm_td2p_port_ipmc_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port IPMC "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* L3 */
    if (!IS_ST_PORT(unit, port)) {
        rv = bcm_esw_port_control_set(unit, port, bcmPortControlIP4, 0);
        if (BCM_SUCCESS(rv)) {
            rv = bcm_esw_port_control_set(unit, port, bcmPortControlIP6, 0);
        }
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to clear port L3 "
                                  "unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }
#endif /* INCLUDE_L3 */

    /* MIRROR */
    rv = bcmi_esw_mirror_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port Mirror "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

#ifdef BCM_FIELD_SUPPORT
    /* FIELD */
    rv = bcm_esw_port_control_set(unit, port, bcmPortControlFilterLookup, 0);
    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterIngress, 0);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterEgress, 0);
    }
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable clear port Field "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
#if defined(BCM_TRX_SUPPORT)
    rv = _bcm_field_flex_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port in Field Port Based Groups"
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
#endif
#endif /* BCM_FIELD_SUPPORT */

    /* RATE */
    rv = _bcm_td2p_port_rate_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port Rate "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* STAT*/
    rv = bcmi_td2p_stat_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port Stat "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* STACK */
    rv = _bcm_td2p_port_stack_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port Stack "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* TRUNK */
    rv = _bcm_td2p_port_trunk_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port Source Trunk Map table "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* VLAN */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_esw_vlan_port_remove(unit, BCM_VLAN_DEFAULT, pbmp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to remove port from default VLAN "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* STG */
    rv = bcm_esw_vlan_stp_set(unit, BCM_VLAN_DEFAULT,
                              port, BCM_STG_STP_DISABLE);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port STP for default VLAN "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    /* PORT */
    rv = _bcm_td2_port_basic_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to detach basic port "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

#ifdef INCLUDE_L3
    /* VFI */
    rv = _bcm_td2p_vfi_membership_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to clear port in default VFI membership "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
#endif /* INCLUDE_L3 */
    /* PFC */
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        rv = bcm_td2_cosq_llfc_profile_detach(unit, port);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Unable to delete default entries for PRIO2COS_PROFILE"
                                  "register profile unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *         bcm_esw_td2p_flexport_port_attach
 * Purpose:
 *         External wrapper for _bcm_td2p_port_attach
 */
int
bcm_esw_td2p_flexport_port_attach(int unit, bcm_port_t port)
{
    return _bcm_td2p_port_attach(unit, port);
}


/*
 * Function:
 *         bcm_esw_td2p_flexport_port_detach
 * Purpose:
 *         External wrapper for _bcm_td2p_port_detach
 */
int
bcm_esw_td2p_flexport_port_detach(int unit, bcm_port_t port)
{
    return _bcm_td2p_port_detach(unit, port);
}


/*
 * Function:
 *      _bcm_td2p_port_resource_attach
 * Purpose:
 *      Attach ports to the BCM layer and initialize them
 *      to the default state.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes caller have detached any ports that will be attached
 *        in this routine.
 *      - Assumes this is NOT called during Warmboot.
 *      - Set current MY_MODID for all newly attached ports.
 */
STATIC int
_bcm_td2p_port_resource_attach(int unit,
                               int nport,
                               bcm_td2p_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_td2p_port_resource_t *pr;
    int i, my_modid;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- BCM Attach ---\n")));

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_ATTACH) {
            rv = _bcm_td2p_port_attach(unit, pr->port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Error: Unable to attach BCM port "
                                      "unit=%d port=%d rv=%d\n"),
                           unit, pr->port, rv));
                return rv;
            }
        }
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_set(unit, my_modid));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_detach
 * Purpose:
 *      Detach ports from the BCM layer.
 *
 *      It clears any HW or SW state that was programmed
 *      by the SDK initialization routines.  It does NOT clear
 *      references to logical ports that have been programmed by the
 *      application.  Applications are responsible for doing that.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_td2p_port_resource_detach(int unit,
                               int nport,
                               bcm_td2p_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_td2p_port_resource_t *pr;
    int i;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- BCM Detach ---\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_DETACH) {
            rv = _bcm_td2p_port_detach(unit, pr->port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Error: Unable to detach BCM port "
                                      "unit=%d port=%d rv=%d\n"),
                           unit, pr->port, rv));
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_resolve
 * Purpose:
 *      Convert logical port number GPORT to BCM local port format and
 *      validate port numbers:
 *      - Logical and physical port numbers are within the valid range.
 *      - Ports must not be CPU or Loopback ports.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 */
STATIC int
_bcm_td2p_port_resource_resolve(int unit, 
                                int nport,
                                bcm_td2p_port_resource_t *resource)
{
    int i;
    bcm_td2p_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check that logical port number is addressable and convert */
        BCM_IF_ERROR_RETURN
            (bcmi_xgs5_port_addressable_local_get(unit,
                                                  pr->port,
                                                  &pr->port));

        /*
         * Check that logical port number is valid for TD2+
         * (above check is generic)
         */
        SOC_IF_ERROR_RETURN
            (soc_td2p_port_addressable(unit, pr->port));

        /* Check that physical port is within the valid range */
        if (pr->physical_port != -1) {
            SOC_IF_ERROR_RETURN
                (soc_td2p_phy_port_addressable(unit, pr->physical_port));
        }

        /* Check that ports, logical and physical, are not a management port */
        if ((si->port_l2p_mapping[pr->port] == TD2P_PHY_PORT_CPU) ||
            (si->port_l2p_mapping[pr->port] == TD2P_PHY_PORT_LB) ||
            (pr->physical_port == TD2P_PHY_PORT_CPU) ||
            (pr->physical_port == TD2P_PHY_PORT_LB) ||
            SOC_PBMP_MEMBER(si->management_pbm, pr->port)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Port cannot be CPU, LB, or management "
                                  "port "
                                  "unit=%d port=%d\n"),
                       unit, pr->port));
            return BCM_E_PORT;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_input_validate
 * Purpose:
 *      Validate function input requirements.
 *
 *      This routine checks for function semantics and guidelines
 *      from the API perspective:
 *      - Check the order of elements in array, 'delete' operations must
 *        be placed first in array.
 *      - Ports must not be CPU or Loopback ports.
 *      - Ports must be disabled.
 *      - Ports linkscan mode must be NONE.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 *      - Assumes port values has been validated.
 */
STATIC int
_bcm_td2p_port_resource_input_validate(int unit, 
                                       int nport,
                                       bcm_td2p_port_resource_t *resource)
{
    int i;
    bcm_td2p_port_resource_t *pr;
    int delete = 1;
    int enable;
    soc_info_t *si = &SOC_INFO(unit);


    /* Check array order and port state disable */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Skip check for speed change */
        if (pr->flags & SOC_PORT_RESOURCE_SPEED) {
            continue;
        }

        /* Check that delete operations are first */
        if (pr->physical_port != -1) {  /* First non-delete found */
            delete = 0;
        } else if ((pr->physical_port == -1) && !delete) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Delete operations must be "
                                  "first in array\n")));
            return BCM_E_CONFIG;
        }

        if ((pr->encap != BCM_PORT_ENCAP_IEEE) &&
            (pr->encap != BCM_PORT_ENCAP_HIGIG2)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Encap modes can only be IEEE or HIGIG2 "
                                  "unit=%d port=%d\n"),
                       unit, pr->port));
            return BCM_E_CONFIG;
        }

        /* Skip ports that are not currently defined */
        if (si->port_l2p_mapping[pr->port] == -1) {
            continue;
        }

        /* Check that ports are disabled */
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_enable_get(unit, pr->port, &enable));
        if (enable) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Port %d needs to be disabled\n"),
                       pr->port));
            return BCM_E_BUSY;
        }

        /*
         * Check that ports to be 'detached' do not
         * have linkscan mode set.  See _bcm_td2p_port_resource_pmap_get().
         */
        if (pr->flags & SOC_PORT_RESOURCE_DETACH) {
            if (bcm_esw_linkscan_enable_port_get(unit, pr->port) !=
                BCM_E_DISABLED) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Linkscan mode needs to be "
                                      "disabled on ports to be detached, "
                                      "port=%d\n"),
                           pr->port));
                return BCM_E_BUSY;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_pmap_get
 * Purpose:
 *      Determine the type of port mapping changes, if any,
 *      as a result of the FlexPort operation.  In addition, it
 *      determines whether the port needs to go through the attach
 *      and/or detach process.
 *
 *      All ports that are being flexed will be detached except for those
 *      that will become inactive (this is only for legacy mode).
 *      All ports that are active after flexed are always attached (this is
 *      for new and legacy mode).
 *
 *      When a port is 'marked' as 'delete' (physical port is -1),
 *      this does not mean the logical port will be destroyed.  Depending
 *      on the rest of the information in the array, the port mapping
 *      may be the same, remapped, or it may no longer exist after FlexPort.
 *
 *      Likewise, when a valid port mapping is given, the port may
 *      not be necessarily new.
 *
 *      Example:
 *
 *      Main FlexPort array ('resource') (application input)
 *               L1 --> -1 (current mapped to P1), mapping is SAME
 *               L2 --> -1 (current mapped to P2), mapping is REMAP
 *               L3 --> -1 (current mapped to P3), mapping is DESTROY
 *               L1 --> P1                       , mapping is SAME
 *               L2 --> P5                       , mapping is REMAP
 *               L9 --> P9                       , mapping is NEW
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN/OUT) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes all 'delete' elements are first in array.
 */
STATIC int
_bcm_td2p_port_resource_pmap_get(int unit, 
                                 int nport, bcm_td2p_port_resource_t *resource)
{
    int i;
    int j;
    bcm_td2p_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        if (pr->physical_port == -1) {
            /*
             * Find if port is given in later elements.
             * If so, check if port is remapped.
             * If not found, port is marked to be destroyed
             * (port will no longer exist after FlexPort)
             *
             * Note that all the 'delete' ports must always first
             * in array.
             */
            for (j = i; j < nport; j++) {
                if (resource[j].physical_port == -1) {
                    continue;
                }
                if (resource[j].port == pr->port) {  /* Found */
                    /* Check if mapping is same */
                    if (resource[j].physical_port !=
                        si->port_l2p_mapping[pr->port]) {
                        pr->flags |= SOC_PORT_RESOURCE_REMAP;
                    }
                    break;
                }
            }

            if (j >= nport) {  /* Not found */
                pr->flags |= SOC_PORT_RESOURCE_DESTROY;

            }

            /*
             * Detach only if the port is not becoming inactive (legacy).
             *
             * A port that is becoming inactive after flex is indicated
             * when both of the following flags are set:
             *     DESTROY
             *     I_MAP
             */
            if (!(pr->flags & SOC_PORT_RESOURCE_DESTROY) ||
                !(pr->flags & SOC_PORT_RESOURCE_I_MAP)) {
                pr->flags |= SOC_PORT_RESOURCE_DETACH;
            }

        } else {

            /*
             * Check if logical port is new (port does exist), or
             * port is being remapped.
             */
            if (si->port_l2p_mapping[pr->port] == -1) {
                pr->flags |= SOC_PORT_RESOURCE_NEW;
            } else if (pr->physical_port != si->port_l2p_mapping[pr->port]) {
                pr->flags |= SOC_PORT_RESOURCE_REMAP;
            }

            /* All active ports after flexed are always attached */
            pr->flags |= SOC_PORT_RESOURCE_ATTACH;
        }

    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_op_get
 * Purpose:
 *      Get the type of FlexPort operations/changes:
 *          none, port-mapping, lanes, speed, encap.
 *
 *      Some operations only need speed or encap changes.  In this
 *      case, the actual FlexPort operation is not necessary and
 *      only calls to set to speed and/or encap mode are made.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      op       - (OUT) FlexPort operation BCM_TD2P_PORT_RESOURCE_OP_...
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Resource is not NULL.
 *      Logical port in 'resource' is in BCM port format (non GPORT).
 */
STATIC int
_bcm_td2p_port_resource_op_get(int unit, 
                               int nport, bcm_td2p_port_resource_t *resource,
                               int *op)
{
    int i;
    bcm_td2p_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);
    int speed = 0;
    int encap = 0;

    *op = BCM_TD2P_PORT_RESOURCE_OP_NONE;
    
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check port speed */
        if (pr->flags & SOC_PORT_RESOURCE_SPEED) {
            /*
             * NOTE: speed_set sequence
             *
             * pr->flags & SOC_PORT_RESOURCE_SPEED is set by speed_set API.
             * No logical-physical port mapping changes.
             * OP_PMAP flag doesn't need to be set in this case.
             * _bcm_td2p_port_resource_speed_execute will be called.
             */
            *op |= BCM_TD2P_PORT_RESOURCE_OP_SPEED;
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                                    "Speed change detected: "
                                    "logical=%d physical=%d\n"),
                         pr->port, pr->physical_port));
            continue;
        }

        /* Check port mapping change */
        if (pr->flags &
            (SOC_PORT_RESOURCE_NEW | SOC_PORT_RESOURCE_DESTROY |
             SOC_PORT_RESOURCE_REMAP)) {
            *op |= BCM_TD2P_PORT_RESOURCE_OP_PMAP;

            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                                    "Port map change detected: "
                                    "%s %s %s l=%d p=%d\n"),
                         (pr->flags & SOC_PORT_RESOURCE_NEW) ? "new" : "",
                         (pr->flags & SOC_PORT_RESOURCE_DESTROY) ?
                         "destroy" : "",
                         (pr->flags & SOC_PORT_RESOURCE_REMAP) ? "remap" : "",
                         pr->port, pr->physical_port));
        }

        /* Skip ports that are being deleted or not defined */
        if ((pr->physical_port == -1) ||
            (si->port_l2p_mapping[pr->port] == -1)) {
            continue;
        }

        /*
         * NOTE: Execute full flex operation if speed is different
         */
        if (!(*op & BCM_TD2P_PORT_RESOURCE_OP_SPEED)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, pr->port, &speed));
            if (pr->speed != speed) {
                *op |= BCM_TD2P_PORT_RESOURCE_OP_PMAP;
            }
        }

        /* Check port lanes */
        if (!(*op & BCM_TD2P_PORT_RESOURCE_OP_LANES)) {
            if (pr->lanes != si->port_num_lanes[pr->port]) {
                *op |= BCM_TD2P_PORT_RESOURCE_OP_LANES;
            }
        }

        /* Check port encap */
        if (!(*op & BCM_TD2P_PORT_RESOURCE_OP_ENCAP)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, pr->port, &encap));
            if (pr->encap != encap) {
                *op |= BCM_TD2P_PORT_RESOURCE_OP_ENCAP;
            }
        }

        /* If all are set, no more checking is necessary */
        if (*op == BCM_TD2P_PORT_RESOURCE_OP_ALL) {
            break;
        }
    }
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_loopback_clear
 * Purpose:
 *      Clear the loopback mode for the given set of ports.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes all 'delete' elements are first in array.
 */
STATIC int
_bcm_td2p_port_resource_loopback_clear(int unit,
                                       int nport,
                                       bcm_td2p_port_resource_t *resource)
{
    int i;
    int loopback;
    bcm_td2p_port_resource_t *pr;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_loopback_get(unit,
                                                pr->port, &loopback));
            if (loopback != BCM_PORT_LOOPBACK_NONE) {
                BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_loopback_set(unit,
                                                pr->port,
                                                BCM_PORT_LOOPBACK_NONE));
            }
        }
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_td2p_port_soc_resource_validate
 * Purpose:
 *      Perform soc resource validation.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Assumes BCM data has been validated (basic API semantics)
 */
STATIC int
_bcm_td2p_port_soc_resource_validate(int unit,
                                     int nport, bcm_td2p_port_resource_t *resource)
{
    int rv;
    int i;
    soc_port_resource_t *soc_resource;

    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "port_resource");
    if (soc_resource == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for SOC FlexPort "
                              "array\n")));
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = resource[i].port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
    }

    rv = soc_td2p_port_resource_validate(unit, nport, soc_resource);
    if (SOC_FAILURE(rv)) {
        sal_free(soc_resource);
        return rv;
    }

    sal_free(soc_resource);

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_port_resource_speed_execute
 * Purpose:
 *      Perform a speed set operation.
 *      This includes speed change only
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Assumes BCM data has been validated (basic API semantics)
 *
 * Details:
 *     This function is pretty similar as _bcm_td2p_port_resource_execute.
 *     But speed set doesn't change logical-physical port mapping.
 *     So this function should skip detach, attach function calls.
 *       _bcm_td2p_port_resource_detach
 *       _bcm_td2p_port_resource_attach
 *     Also this calls soc_td2p_port_resource_configure with
 *     the flag:SOC_PORT_RESOURCE_CONFIGURE_SPEED_SET to make slightly
 *     different sequence from full flex operation.
 */
STATIC int
_bcm_td2p_port_resource_speed_execute(int unit,
                                  int nport, bcm_td2p_port_resource_t *resource)
{
    int rv;
    int i;
    soc_port_resource_t *soc_resource;

    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "port_resource");
    if (soc_resource == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for SOC FlexPort "
                              "array\n")));
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = resource[i].port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
    }

    rv = soc_td2p_port_resource_validate(unit, nport, soc_resource);
    if (SOC_FAILURE(rv)) {
        sal_free(soc_resource);
        return rv;
    }

    /* SOC Port Resource Configure */
    rv = soc_td2p_port_resource_configure(unit, nport, soc_resource,
                        SOC_PORT_RESOURCE_CONFIGURE_SPEED_ONLY);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Changes to device may have been partially "
                              "executed.  System may be unstable.\n")));
    }

    sal_free(soc_resource);

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_port_resource_execute
 * Purpose:
 *      Perform a FlexPort operation.
 *      This includes changes that involve:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Assumes BCM data has been validated (basic API semantics)
 *
 * Details:
 *
 * Port Mapping
 * ------------
 * A logical to physical port mapping is given as part of the port resource
 * array.  This can lead to the following operations:
 * - Port Mapping Delete (or clear)
 *   This is indicated when the physical port is set to (-1) and
 *   the port to be flexed is considered an 'old' port.
 *   The logical port in the array entry must always be valid.
 *   A 'delete' may result in the following actions for a port mapping:
 *      (1) Destroyed
 *      (2) Same
 *      (3) Remapped
 *
 * - Port Mapping Add (or set)
 *   This is indicated when the physical port is set to a valid
 *   physical port number (>=0).
 *   The logical port in the array entry must always be valid.
 *   An 'add' may result in the following actions for a port mapping:
 *      (1) New
 *      (2) Same
 *      (3) Remapped
 *
 * Destroyed: Logical port no longer exist after FlexPort.
 * Remapped:  Logical port will continue to exist after FlexPort
 *            but is mapped to a different physical port.
 * New:       Logical port did not exist before FlexPort.
 * Same:      Logical to physical port mapping remains the same.
 */
STATIC int
_bcm_td2p_port_resource_execute(int unit, 
                                int nport, bcm_td2p_port_resource_t *resource,
                                int op)
{
    int rv;
    int i;
    int flag;
    soc_port_resource_t *soc_resource;

    /*
     * Main FlexPort Sequence
     *
     * - SOC Validation:
     *     + More validation takes place in the SOC layer.
     *     + These check for abilities and configurations based on
     *       the physical aspects of the device.
     *
     * NOTE: Up to this point, no changes has taken place in SW
     *       data structures or HW.
     *       From this point forward, HW and SW data will be modified,
     *       any errors will leave the system unstabled.
     *
     * - Clear any port related settings in BCM layer as needed.
     *
     * - Detach 'old' ports from BCM layer:
     *     + These are all ports to be cleared/deleted (indicated by physical
     *       port -1), EXCEPT those that will become inactive after the
     *       FlexPort operation (this is only true with legacy API).
     *     + The port state should be the same as the state of a
     *       port that was never configured during SDK initialization.
     *     + This involves SW and HW changes.
     *
     * - SOC Port Resource Configure:
     *     Takes place in the SOC layer.
     *     It consists primarliy of these steps:
     *       + Delete ports in SOC layer (SOC_INFO SW, HW update).
     *       + Add ports in SOC layer (SOC_INFO SW, HW update).
     *       + Execute FlexPort operation as defined by HW specs.
     *
     * - Attach 'new' ports to BCM layer:
     *     + These are all ports that are active after the FlexPort operation.
     *     + Ports are initialized to a default state
     *       (same state when a port is brought up after SDK is initialized).
     *     + This involves SW and HW changes.
     *
     * - Set BCM properties on flexed ports as needed.
     */

    /*
     * SOC Port Structure initialization
     */
    /* Allocate memory for SOC Port Resource array data structure */
    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "port_resource");
    if (soc_resource == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for SOC FlexPort "
                              "array\n")));
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = resource[i].port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
    }


    /* SOC Validation */
    rv = soc_td2p_port_resource_validate(unit, nport, soc_resource);
    if (SOC_FAILURE(rv)) {
        sal_free(soc_resource);
        return rv;
    }

    /* Detach ports from BCM layer */
    rv = _bcm_td2p_port_resource_detach(unit, nport, resource);

    /* SOC Port Resource Configure */
    if (op == BCM_TD2P_PORT_RESOURCE_OP_LANES) {
        flag = SOC_PORT_RESOURCE_CONFIGURE_LANES_ONLY;
    } else {
        flag = SOC_PORT_RESOURCE_CONFIGURE_FLEX;
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_td2p_port_resource_configure(unit, nport, soc_resource, flag);
    }

    /* Attach ports to BCM layer */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_td2p_port_resource_attach(unit, nport, resource);
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Changes to device may have been partially "
                              "executed.  System may be unstable.\n")));
    }

    sal_free(soc_resource);

    return rv;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_configure
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - The data is modified by the function.
 *      - It takes the internal bcm_td2p_port_resource_t data type.
 */
STATIC int
_bcm_td2p_port_resource_configure(int unit, 
                                  int nport,
                                  bcm_td2p_port_resource_t *resource)
{
    int op;
    int i;

    /* Convert port format and validate logical and physical port numbers */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_resolve(unit,
                                         nport, resource));

    /* Get port mapping change */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_pmap_get(unit, nport, resource));

    /* clear loopback */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_loopback_clear(unit, nport, resource));

    /*
     * Validate function input requirements
     *
     * This steps checks for function semantics and guidelines
     * from the API perspective.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_input_validate(unit, nport, resource));

    /*
     * FlexPort operations types:
     *     NONE                             => Nothing to do
     *     PMAP or LANES                    => FlexPort sequence
     *     ENCAP                            => Encap set
     *     SPEED, but not PMAP or LANES     => Speed set
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_op_get(unit, nport, resource, &op));

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "FlexPort operations 0x%x: %s %s %s %s\n"),
                 op,
                 (op & BCM_TD2P_PORT_RESOURCE_OP_PMAP) ? "pmap" : "",
                 (op & BCM_TD2P_PORT_RESOURCE_OP_SPEED) ? "speed" : "",
                 (op & BCM_TD2P_PORT_RESOURCE_OP_LANES) ? "lanes" : "",
                 (op & BCM_TD2P_PORT_RESOURCE_OP_ENCAP) ? "encap" : ""));

    if (op == BCM_TD2P_PORT_RESOURCE_OP_NONE) {
        return BCM_E_NONE;     /* No changes, just return */
    }

    /*
     * Port Mapping / Lane
     *
     * These changes always require a FlexPort operation.
     */
    if ((op & BCM_TD2P_PORT_RESOURCE_OP_PMAP) ||
        (op & BCM_TD2P_PORT_RESOURCE_OP_LANES)) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "--- Execute FlexPort sequence ---\n")));
        BCM_IF_ERROR_RETURN
            (_bcm_td2p_port_resource_execute(unit,
                                             nport, resource, op));
        /*
         * Bring new ports down to ensure port state is proper
         */
        for (i = 0; i < nport; i++) {
            if (resource[i].physical_port == -1) {
                continue;
            }
            BCM_IF_ERROR_RETURN
                (bcmi_esw_portctrl_enable_set(unit, resource[i].port, 0));
        }
    } else if (op & BCM_TD2P_PORT_RESOURCE_OP_SPEED) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "--- Execute Speed Set sequence ---\n")));
        BCM_IF_ERROR_RETURN
            (_bcm_td2p_port_resource_speed_execute(unit,
                                               nport, resource));
    } else if (op & BCM_TD2P_PORT_RESOURCE_OP_ENCAP) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2p_port_soc_resource_validate(unit, nport, resource));
    }

    /*
     * Encap
     */
    if (op & BCM_TD2P_PORT_RESOURCE_OP_ENCAP) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "Execute Encap change\n")));

        for (i = 0; i < nport; i++) {
            if (resource[i].physical_port == -1) {
                continue;
            }

            BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_encap_set_execute(unit,
                                    resource[i].port,
                                    resource[i].encap, TRUE));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_port_resource_multi_set
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - This is to be called by the API dispatch driver.
 *      - It takes the public bcm_port_resource_t data type.
 */
STATIC int
_bcm_td2p_port_resource_multi_set(int unit, 
                                  int nport,
                                  bcm_port_resource_t *resource)
{
    int rv;
    int i;
    bcm_td2p_port_resource_t *port_resource;

    if (resource == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * Use internal port resource structure type.
     * Called routines will be modifying the data, so
     * having a copy avoids modifying user's data.
     */
    port_resource = sal_alloc(sizeof(bcm_td2p_port_resource_t) * nport,
                              "port_resource");
    if (port_resource == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for internal "
                              "FlexPort array\n")));
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(port_resource, 0, sizeof(bcm_td2p_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        /*
         * No current flags defined in bcm_port_resource_t, otherwise
         * need to translate from public to internal flags.
         */
        port_resource[i].flags = 0;
        port_resource[i].port = resource[i].port;
        port_resource[i].physical_port = resource[i].physical_port;
        port_resource[i].speed = resource[i].speed;
        port_resource[i].lanes = resource[i].lanes;
        port_resource[i].encap = resource[i].encap;
    }

    rv = _bcm_td2p_port_resource_configure(unit, nport, port_resource);

    sal_free(port_resource);

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_port_core_chip_speed_get
 * Purpose:
 *      This API is to obtain core chip speed
 * Parameters:
 *      unit     - (IN) Unit number
 *      port     - (IN) Logical port number(bcm_port_t, not bcm_gport_t)
 *      speed    - (IN) Port speed
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - This is to be called by the API dispatch driver.
 */
STATIC int
_bcm_td2p_port_core_chip_speed_get(int unit, bcm_port_t port, int *speed)
{
    SOC_IF_ERROR_RETURN(soc_td2p_port_speed_get(unit, port, speed));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_resource_speed_set
 * Purpose:
 *      Changes the speed but keeps the same number of lanes
 * Parameters:
 *      unit     - (IN) Unit number
 *      port     - (IN) Logical port number(bcm_port_t, not bcm_gport_t)
 *      speed    - (IN) Port speed
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - This is to be called by the API dispatch driver.
 */
STATIC int
_bcm_td2p_port_resource_speed_set(int unit, bcm_port_t port, int speed)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_td2p_port_resource_t resource[2]; /* resource for pre and post */
    int cur_lanes, cur_speed, cur_encap;
    int phy_port;

    /* Get current core chip port speed */
    BCM_IF_ERROR_RETURN(_bcm_td2p_port_core_chip_speed_get(
                            unit, port, &cur_speed));
    if (cur_speed == 0) {
        /* port is detached */
        cur_speed = SOC_INFO(unit).port_speed_max[port];
    }

    if (cur_speed == speed) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                    "Speed is already configured as %d\n"), cur_speed));
        return BCM_E_NONE;
    }

    /* Check current number of lanes */
    BCM_IF_ERROR_RETURN(bcmi_td2p_port_lanes_get(unit, port, &cur_lanes));

    /* Get current port encap */
    BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, port, &cur_encap));

    /*
     * Clear mapping for physical ports involved in FlexPort operation.
     */
    phy_port = si->port_l2p_mapping[port];
    /* Check valid physical port */
    if (phy_port == -1) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port "
                              "for logical port %d\n"),
                   port));
        return BCM_E_INTERNAL;
    }

    resource[0].flags = SOC_PORT_RESOURCE_I_MAP | SOC_PORT_RESOURCE_SPEED;
    resource[0].port = port;
    resource[0].physical_port = -1;

    resource[1].flags = SOC_PORT_RESOURCE_I_MAP | SOC_PORT_RESOURCE_SPEED;
    resource[1].port = port;
    resource[1].physical_port = phy_port;
    resource[1].speed = speed;
    resource[1].lanes = cur_lanes;
    resource[1].encap = cur_encap;

    BCM_IF_ERROR_RETURN(_bcm_td2p_port_resource_configure(unit, 2, resource));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_port_lanes_get
 * Purpose:
 *      Get the number of PHY lanes for the given port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (OUT) Returns number of lanes for port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_td2p_port_lanes_get(int unit, bcm_port_t port, int *lanes)
{
    bcm_port_resource_t resource;

    BCM_IF_ERROR_RETURN(bcm_esw_port_resource_get(unit, port, &resource));

    *lanes = resource.lanes;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_td2p_port_lanes_set
 * Purpose:
 *      Set the number of PHY lanes for the given port.
 *
 *      This function should only be used to support the legacy
 *      FlexPort API bcm_port_control_set(... , bcmPortControlLanes).
 *
 *      The legacy API does not delete the port mappings when flexing
 *      to fewer lanes.  Ports which lanes are used by the base
 *      physical port becomes inactive.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (IN) Number of lanes to set on given port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Only to be used by legacy API to support TD2 behavior
 *        bcm_port_control_set(... , bcmPortControlLanes).
 *      - Supports only up to 4 lanes (TSC-4).
 *      - 'port' must be in BCM port format (non-GPORT) and mapped to
 *        the base physical port.
 *      - Several calls to legacy API may be required to
 *        achieve desired configuration.
 */
int
bcmi_td2p_port_lanes_set(int unit, bcm_port_t port, int lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_td2p_port_resource_t resource[8]; /* Max sz to support up to 4 lanes */
    int max_array_cnt;
    int i;
    int cur_lanes;
    int cur_speed;
    int cur_encap = 0;
    int phy_port;
    int speed;
    int num_ports_clear;
    int num_ports_new;

    /* Support legacy API up to 4 lanes */
    if ((lanes != 1) && (lanes != 2) && (lanes != 4)) {
        return BCM_E_PARAM;
    }

    /* Check current number of lanes */
    BCM_IF_ERROR_RETURN(bcmi_td2p_port_lanes_get(unit, port, &cur_lanes));
    if (cur_lanes == lanes) {
        return BCM_E_NONE;
    }

    /* Check that port can support number of lanes specified */
    SOC_IF_ERROR_RETURN(soc_td2p_port_lanes_valid(unit, port, lanes));

    /* Get current port speed */
    BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, port, &cur_speed));

    /* Get current port encap */
    BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, port, &cur_encap));

    /* Clear array */
    max_array_cnt = COUNTOF(resource);
    sal_memset(resource, 0, sizeof(resource));

    /*
     * Build port resource array
     * Certain information is derived from 'best' guess,
     * similar to the legacy behaviour.
     */

    /*
     * Select speed based on number of lanes
     *
     * For HG, if the current HG speed is the higher speed
     * (i.e. 42 vs 40) select the higher HG speed for the number of lanes.
     * Example
     *    1xHG[42] --> 4xHG[11]
     *    1xHG[40] --> 4xHG[10]
     *
     * Notes:  Assumes encap is HIGIG2 if it is not IEEE
     */
    switch(lanes) {
    case 1:
        if (cur_encap == BCM_PORT_ENCAP_IEEE) {    /* IEEE */
            speed = 10000;
        } else {                                   /* Assume HG */
            if (((cur_lanes == 4) && (cur_speed > 40000)) ||
                ((cur_lanes == 2) && (cur_speed > 20000))) {
                speed = 11000;
            } else {
                speed = 10000;
            }
        }
        break;
    case 2:
        if (cur_encap == BCM_PORT_ENCAP_IEEE) {    /* IEEE */
            speed = 20000;
        } else {                                   /* Assume HG */
            if (((cur_lanes == 4) && (cur_speed > 40000)) ||
                ((cur_lanes == 1) && (cur_speed > 10000))) {
                speed = 21000;
            } else {
                speed = 20000;
            }
        }
        break;
    case 4:
        if (cur_encap == BCM_PORT_ENCAP_IEEE) {    /* IEEE */
            speed = 40000;
        } else {                                   /* Assume HG */
            if (((cur_lanes == 2) && (cur_speed > 20000)) ||
                ((cur_lanes == 1) && (cur_speed > 10000))) {
                speed = 42000;
            } else {
                speed = 40000;
            }
        }
        break;
    /*
     * COVERITY
     *
     * Case for default is intentional as a defensive check.
     */
    /* coverity[dead_error_begin] */
    default:
        return BCM_E_PARAM;
        break;
    }

    switch (cur_lanes) {
    case 1:
        /*
         * 2x10 flex to: 2 lanes -> 1x20
         * 4x10 flex to: 4 lanes -> 1x40
         */
        if (lanes == 2) {
            int first_phy_port;
            int physical_port = si->port_l2p_mapping[port];

            SOC_IF_ERROR_RETURN (soc_td2p_port_macro_first_phy_port_get(unit,
                                 physical_port, &first_phy_port));

            /* In tri-mode, either first two lanes of a tsc (1,2) can be
             * selected for 20G speed, or the last two lanes (3, 4). In the
             * second case, we should create only one new 20G port
             */
            if (physical_port == first_phy_port) {
                num_ports_clear = 4;
                num_ports_new = 2;
            } else {
                num_ports_clear = 2;
                num_ports_new = 1;
            }
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 2:
        /*
         * 1x20 flex to: 1 lane  -> 2x10
         * 2x20 flex to: 4 lanes -> 1x40
         */
        if (lanes == 1) {
            num_ports_clear = 2;
            num_ports_new = 2;
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 4:
        /*
         * 1x40 flex to: 1 lane  -> 4x10
         *               2 lanes -> 2x20
         */
        if (lanes == 1) {
            num_ports_clear = 4;
            num_ports_new = 4;
        } else {
            num_ports_clear = 4;
            num_ports_new = 2;
        }
        break;
    default:
        return BCM_E_CONFIG;
        break;
    }

    if ((num_ports_clear + num_ports_new) > max_array_cnt) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid array FlexPort calculation "
                              "num_ports_clear=%d num_ports_new=%d "
                              "max_array=%d\n"),
                   num_ports_clear, num_ports_new, max_array_cnt));
        return BCM_E_INTERNAL;
    }

    /*
     * Clear mapping for physical ports involved in FlexPort operation.
     * Assume physical ports are numbered consecutively in device.
     */
    phy_port = si->port_l2p_mapping[port];
    for (i = 0; i < num_ports_clear; i++) {
        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port++];
        resource[i].physical_port = -1;
    }

    /*
     * Map new ports
     *
     * Legacy API requires that logical-physical port mapping
     * is valid in the SOC_INFO SW data.
     */
    phy_port = si->port_l2p_mapping[port];
    for (; i < (num_ports_clear + num_ports_new); i++) {
        /* Check valid physical port */
        if (phy_port == -1) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid physical port "
                                  "for logical port %d\n"),
                       port));
            return BCM_E_INTERNAL;
        }

        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port];
        resource[i].physical_port = phy_port;
        resource[i].lanes = lanes;
        resource[i].speed = speed;
        resource[i].encap = cur_encap;
        phy_port += lanes;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_resource_configure(unit,
                                           (num_ports_clear + num_ports_new),
                                           resource));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_flexport_pbmp_update
 * Description:
 *      given a pbmp, add flexport pbmp
 * Parameters:
 *      unit    - (IN) BCM device number
 *      pbmp    - (IN/OUT) port bit map
 */

int
bcm_td2p_flexport_pbmp_update(int unit, bcm_pbmp_t *pbmp)
{
    bcm_port_t port;

    if (NULL == pbmp) {
        return BCM_E_PARAM;
    }
    for (port = 0; port < TD2P_MAX_NUM_PORTS; port++) {
        BCM_PBMP_PORT_ADD(*pbmp, port);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_control_class_select_set
 * Description:
 *      Function to add an entry to MISC_PORT_PROFILE table.
 * Parameters:
 *      unit     - (IN) Device number
 *      port     - (IN) Port number
 *      table_id - (IN) 0 for FP_I2E_CLASSID_SELECTm
 *                      1 for FP_HG_CLASSID_SELECT
 *      mem      - (IN) FP_I2E_CLASSID_SELECTm or FP_HG_CLASSID_SELECTm
 *      field    - (IN) Field to modify.
 *      value    - (IN) New field value.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_port_control_class_select_set(int unit, bcm_port_t port, int table_id,
                                        soc_mem_t mem, soc_field_t field, uint32 value)
{
    fp_i2e_classid_select_entry_t fp_ent;
    fp_hg_classid_select_entry_t hg_ent;
    void *entries[2];
    uint32 profile_index = 0;
    int cur_profile_index = 0;
    int cpu_tabs = _BCM_CPU_TABS_NONE;

    entries[0] = &fp_ent;
    entries[1] = &hg_ent;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_tab_get(unit, port, MISC_PORT_PROFILE_INDEXf, &cur_profile_index));

    BCM_IF_ERROR_RETURN
        (_bcm_misc_port_profile_entry_get(unit, cur_profile_index, 1, entries));

    soc_mem_field32_set(unit, mem, entries[table_id], field, value);

    BCM_IF_ERROR_RETURN
        (_bcm_misc_port_profile_entry_add(unit, entries, 1, &profile_index));

    if (IS_CPU_PORT(unit, port) && table_id == 1) {
        cpu_tabs = _BCM_CPU_TABS_BOTH;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_tab_set(unit, port, cpu_tabs,
                               MISC_PORT_PROFILE_INDEXf, profile_index));

    if (cur_profile_index != 0) {
        BCM_IF_ERROR_RETURN
            (_bcm_misc_port_profile_entry_delete(unit, cur_profile_index));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_control_class_select_get
 * Description:
 *      Funtion to get an entry from MISC_PORT_PROFILE table.
 * Parameters:
 *      unit     - (IN) Device number
 *      port     - (IN) Port number
 *      table_id - (IN) 0 for FP_I2E_CLASSID_SELECTm
 *                      1 for FP_HG_CLASSID_SELECT
 *      mem      - (IN) FP_I2E_CLASSID_SELECTm or FP_HG_CLASSID_SELECTm
 *      field    - (IN) Field to read.
 *      value    - (OUT) Field value.
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_port_control_class_select_get(int unit, bcm_port_t port, int table_id,
                                        soc_mem_t mem, soc_field_t field, uint32* value)
{
    fp_i2e_classid_select_entry_t fp_ent;
    fp_hg_classid_select_entry_t hg_ent;
    void *entries[2];
    int profile_index;

    entries[0] = &fp_ent;
    entries[1] = &hg_ent;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_tab_get(unit, port, MISC_PORT_PROFILE_INDEXf, &profile_index));

    BCM_IF_ERROR_RETURN(_bcm_misc_port_profile_entry_get(unit, profile_index, 1, entries));

    *value = soc_mem_field32_get(unit, mem, entries[table_id], field);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_control_egress_class_select_set
 * Description:
 *      Helper funtion to modify fields in FP_I2E_CLASSID_SELECTm memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (IN) New field value
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td2p_port_control_egress_class_select_set(int unit, bcm_port_t port, uint32 value)
{
    soc_mem_t mem = FP_I2E_CLASSID_SELECTm;
    soc_field_t field = PORT_SELf;
    /* table_id = 0 for FP_I2E_CLASSID_SELECTm in MISC_PORT_PROFILE_TABLE */
    int table_id = 0;
    /* Check for valid inputs for FP_I2E_CLASSID_SELECT */
    /* coverity[unsigned_compare] : FALSE */
    if ((value < bcmPortEgressClassSelectNone) ||
        (value >= bcmPortEgressClassSelectFieldIngress)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_control_class_select_set(unit, port, table_id, mem, field, value));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_control_egress_class_select_get
 * Description:
 *      Helper funtion to get fields in FP_I2E_CLASSID_SELECTm memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (OUT) New field value
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td2p_port_control_egress_class_select_get(int unit, bcm_port_t port, uint32 *value)
{
    soc_mem_t mem = FP_I2E_CLASSID_SELECTm;
    soc_field_t field = PORT_SELf;
    /* table_id = 0 for FP_I2E_CLASSID_SELECTm in MISC_PORT_PROFILE_TABLE */
    int table_id = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_control_class_select_get(unit, port, table_id, mem, field, value));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_control_higig_class_select_set
 * Description:
 *      Helper funtion to modify fields in FP_HG_CLASSID_SELECT memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (IN) New field value
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td2p_port_control_higig_class_select_set(int unit, bcm_port_t port, uint32 value)
{
    soc_mem_t mem = FP_HG_CLASSID_SELECTm;
    soc_field_t field = PORT_SELf;
    /* table_id = 1 for FP_HG_CLASSID_SELECTm in MISC_PORT_PROFILE_TABLE */
    int table_id = 1;
    /* Check for valid inputs for FP_HG_CLASSID_SELECT */
    /* coverity[unsigned_compare] : FALSE */
    if ((value < bcmPortFieldHiGigClassSelectNone) ||
        (value >= bcmPortFieldHiGigClassSelectCount) ||
        (value == bcmPortFieldHiGigClassSelectL3Egress) ||
        (value == bcmPortFieldHiGigClassSelectL3EgressIntf)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_control_class_select_set(unit, port, table_id, mem, field, value));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_port_control_higig_class_select_get
 * Description:
 *      Helper funtion to get fields in FP_HG_CLASSID_SELECT memory.
 * Parameters:
 *      unit  - (IN) Device number
 *      port  - (IN) Port number
 *      value - (OUT) New field value
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_td2p_port_control_higig_class_select_get(int unit, bcm_port_t port, uint32 *value)
{
    soc_mem_t mem = FP_HG_CLASSID_SELECTm;
    soc_field_t field = PORT_SELf;
    /* table_id = 1 for FP_HG_CLASSID_SELECTm in MISC_PORT_PROFILE_TABLE */
    int table_id = 1;

    BCM_IF_ERROR_RETURN
        (_bcm_td2p_port_control_class_select_get(unit, port, table_id, mem, field, value));

    return BCM_E_NONE;
}

#else /* BCM_TRIDENT2PLUS_SUPPORT */
typedef int bcm_esw_td2p_port_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
