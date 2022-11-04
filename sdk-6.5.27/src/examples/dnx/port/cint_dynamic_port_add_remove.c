/*~~~~~~~~~~~~~~~~~~~~~~~~~~NIF: Interface Configuration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_dynamic_port_add_remove.c
 * Purpose:     Example of adding and removing ports
 *
 * Example includes:
 *  o     Removing one port configuration
 *  o     Adding one port configuration
 *  o     Removing several ports and Adding several new ports configuration
 *  o     Changing number of lanes for a given port
 *
 * It is assumed diag_init is executed.
 *

 *
 * Usage Example:
 * The example below will remove port 13 and then will add it back as XE on link 88.
 * This example is for JR2.
 * For Q2A use lane 18 (i.e. BCM_PBMP_PORT_SET(phy_pbmp, 18);)
 *
 * Test Scenario - start
    nif sts
    *c
    *cint_reset();
    *exit;
    cint ../../../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
    cint ../../../../src/examples/sand/cint_sand_nif_phy_loopback.c


    c
    print "remove port 13";
    int unit;
    bcm_port_t port;
    uint32 flags;
    unit = 0;
    port = 13;
    flags = 0;
    print cint_dyn_port_remove_port_full_example(unit, port, flags);
    exit;
    nif sts

    c
    print "Add port 13 10G on XE88";
    int unit;
    bcm_port_t port;
    bcm_pbmp_t phy_pbmp;
    int rate_mhz;
    BCM_PBMP_PORT_SET(phy_pbmp, 88);
    unit = 0;
    port = 13;
    rate_mhz = 10000;
    print cint_dyn_port_add_ixia_port(unit, port , phy_pbmp, rate_mhz, 0);
    exit;
    nif sts

    c
    print "Configure local snake";
    print bcm_port_force_forward_set(0,13,13,1);
    exit;
 * Test Scenario - end

 * The example below will remove port 1 and then will add it back as CDG on links 0-7:
 * This example is for JR2.
 * For Q2A use lane 8-15.
 *

 * Test Scenario - start
    nif sts
    *c
    *cint_reset();
    *exit;
    cint ../../../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
    cint ../../../../src/examples/sand/cint_sand_nif_phy_loopback.c

    c
    print "remove port 1";
    int unit;
    bcm_port_t port;
    uint32 flags;
    unit = 0;
    port = 1;
    flags = 0;
    print cint_dyn_port_remove_port_full_example(unit, port, flags);
    exit;
     nif sts

    c
    print "Add port 1 400G on CDG0";
    int unit;
    int rate_mhz;
    bcm_port_t port;
    bcm_pbmp_t phy_pbmp;
    BCM_PBMP_PORT_SET(phy_pbmp, 0);
    BCM_PBMP_PORT_ADD (phy_pbmp, 1);
    BCM_PBMP_PORT_ADD (phy_pbmp, 2);
    BCM_PBMP_PORT_ADD (phy_pbmp, 3);
    BCM_PBMP_PORT_ADD (phy_pbmp, 4);
    BCM_PBMP_PORT_ADD (phy_pbmp, 5);
    BCM_PBMP_PORT_ADD (phy_pbmp, 6);
    BCM_PBMP_PORT_ADD (phy_pbmp, 7);
    unit = 0;
    port = 1;
    rate_mhz = 400000;
    print cint_dyn_port_add_ixia_port(unit, port , phy_pbmp, rate_mhz, 0);
    exit;
    nif sts

    c
    print "Configure local snake";
    print bcm_port_force_forward_set(0,1,1,1);
    exit;
 * Test Scenario - end

 * The example below will remove port 7 and then will add it back as 2 ILKN channels (port 7 - channel 0 and
 * port 8 - channel 1), on ilkn_id 3 and links 40-51:
 * This example is for Q2A.
 * JR2 doesn't support dynamic port add/remove for ILKN (since it is ELK only)

 * Test Scenario - start
    nif sts
    nif ilkn lane_map
    *c
    *cint_reset();
    *exit;
    cint ../../../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
    cint ../../../../src/examples/sand/cint_sand_nif_phy_loopback.c

    c
    print "remove port 7";
    int unit;
    bcm_port_t port;
    uint32 flags;
    unit = 0;
    port = 7;
    flags = 0;
    print cint_dyn_port_remove_port_full_example(unit, port, flags);
    exit;
    nif sts
    nif ilkn lane_map

    c
    print "Add port 7 as ILKN channel 0, on ilkn_id 3 and lanes 40-51";
    int i;
    int unit;
    uint32 channel;
    uint32 ilkn_id;
    int rate_mhz;
    bcm_port_t port;
    bcm_pbmp_t phy_pbmp;
    int first_phy;
    int last_phy;

    first_phy = 40;
    last_phy=51;
    BCM_PBMP_PORT_SET(phy_pbmp, first_phy);
    for (i=first_phy+1; i<=last_phy; i++)
    {
        BCM_PBMP_PORT_ADD (phy_pbmp, i);
    }
    ilkn_id = 3;
    unit = 0;
    port = 7;
    channel = 0;
    rate_mhz = 25781;
    print cint_dyn_port_add_ilkn_ports(unit, port , phy_pbmp, ilkn_id, channel, rate_mhz, BCM_PORT_ADD_CONFIG_CHANNELIZED);

    print "Add port 8 as ILKN channel 1, on ilkn_id 3 and lanes 40-51";
    port = 8;
    channel = 1;
    print cint_dyn_port_add_ilkn_ports(unit, port , phy_pbmp, ilkn_id, channel, rate_mhz, BCM_PORT_ADD_CONFIG_CHANNELIZED);

    exit;
    nif sts
    nif ilkn lane_map
 * Test Scenario - end

 */

int random_lane_num;
int serdes_allocation_temp[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
int serdes_allocation_final[12];
/* Flag for adding 20G port with the dummy tvco 26G */
int tvco_prioritize_26p562g_flag = 0;
/* Enable all the ports after adding port sequences */
int enable_port_at_last = 1;

int DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE = 0;
int DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG = 2;
int DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG = 4;
int DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG = 8;
int DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG = 16;
int DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG = 24;
int DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG = 28;

/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame (IEEE 802.1Q)*/
int DNX_TPID_VALUE_C_8100_TAG = 0x8100;
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1Q)*/
int DNX_TPID_VALUE_S_9100_TAG = 0x9100;
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1ad, a.k.a QinQ)*/
int DNX_TPID_VALUE_S_88A8_TAG = 0x88A8;
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1ah, a.k.a MAC-IN-MAC)*/
int DNX_TPID_VALUE_MAC_IN_MAC_TAG = 0x88E7;

/* FLR firmware support while creating Flexe PHY */
int flexe_phy_dpp_with_flr = 0;
int flexe_phy_fec_type = 0;
int flexe_phy_instance = -1;

typedef enum dfe_mode_t
{
    dfe_on,
    dfe_lp,
    dfe_none
};

typedef enum medium_type_t
{
    backplane,
    copper_cable,
    optic
};

typedef enum port_mapping_resource_t
{
    logical_ports,
    tm_ports,
    port_base_q_pairs,
    port_base_hrs,
    nif_phy_lanes
};

struct cint_dyn_port_global_configurations_t {
    /** Indication if we want to skip compensation for port */
    int skip_port_compensation;
    /**  if true, set header type only and skip other PP properties */
    int pp_set_header_only;
};

/* Function:
 *      cint_dyn_port_config_ports_properties
 * Purpose:
 *      Reconfigure NIF
 */

int
cint_dyn_port_resource_fill(
    int unit,
    bcm_gport_t port,
    dfe_mode_t dfe_mode,
    medium_type_t medium_type,
    bcm_port_resource_t * resource)
{
    char error_msg[200]={0,};

    /*
     * Set resource
     */
    snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_default_get(unit, port, 0, resource), error_msg);
    /*
     * Fill the lane_config word
     */
    switch (medium_type)
    {
        case backplane:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                         BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE);
            break;
        case copper_cable:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                         BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE);
            break;
        case optic:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                         BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS);
            break;
        default:
            printf("Unsupported medium type: $medium_type\n");
            return BCM_E_PARAM;
            break;
    }

    switch (dfe_mode)
    {
        case dfe_on:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
            break;
        case dfe_lp:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
            break;
        case dfe_none:
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_CLEAR(resource->phy_lane_config);
            break;
        default:
            printf("Unsupported dfe node: $dfe_mode\n");
            return BCM_E_PARAM;
            break;
    }

    return BCM_E_NONE;
}

/*
* Function:
*      cint_dyn_port_vlan_llvp_ingress
* Purpose:
*      This function initializes Ingress LLVP
*      classification tables with default values.
*      The configuration is the same as the Application reference code -
*      see appl_dnx_vlan_llvp_init_ingress.
* Parameters:
*      unit                             - (IN)  Unit number.
*      port                             - (IN)  Logical port.
* Returns:
*      BCM_E_NONE     No Error
*      BCM_E_XXX      Error occurred
*/
int
cint_dyn_port_vlan_llvp_ingress(
    int unit,
    bcm_port_t port)
{
    bcm_port_tpid_class_t tpid_class;
    uint32 is_ext_vcp_enable;
    uint32 flags;
    bcm_port_tag_format_class_t tag_format_class_id;
    bcm_port_tpid_class_t_init(&tpid_class);

    is_ext_vcp_enable = *(dnxc_data_get(unit, "vlan", "llvp", "ext_vcp_en", NULL));

    if(is_ext_vcp_enable)
    {
        flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    else
    {
        flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    }

    /** Set mapping:Frame has no VLAN TAGs */
    tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has C-VLAN TAG */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has C-VLAN TAG with priority (VID=0) - set to UNTAG! */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        tpid_class.port = port;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");
    }

    /** Set mapping: Frame has S-VLAN TAG */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has S-VLAN TAG with priority (VID=0) - set to UNTAG! */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        tpid_class.port = port;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");
    }

    /** Set mapping:Frame has S-VLAN TAG */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has S-VLAN TAG with priority (VID=0) - set to UNTAG! */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        tpid_class.port = port;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");
    }

    /** Set mapping: Frame has S-Priority-C-VLAN TAGs */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
        tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
        tpid_class.port = port;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");
    }

    /** Set mapping: Frame has S-Priority-C-VLAN TAGs */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
        tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
        tpid_class.port = port;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");
    }

    /** Set mapping: Frame has S-C-VLAN TAGs */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has S-C-VLAN TAGs */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has C-C-VLAN TAG*/
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has S-S-VLAN TAG */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has S-S-VLAN TAG */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    return BCM_E_NONE;
}
/*
* Function:
*      cint_dyn_port_vlan_llvp_egress
* Purpose:
*      This function initializes Egress LLVP
*      classification tables with default values.
*      The configuration is the same as the Application reference code -
*      see appl_dnx_vlan_llvp_init_egress.
* Parameters:
*      unit                             - (IN)  Unit number.
*      port                             - (IN)  Logical port.
* Returns:
*      BCM_E_NONE     No Error
*      BCM_E_XXX      Error occurred
*/
int
cint_dyn_port_vlan_llvp_egress(
    int unit,
    bcm_port_t port)
{
    uint32 flags;
    bcm_port_tpid_class_t tpid_class;
    bcm_port_tag_format_class_t tag_format_class_id;
    bcm_port_tpid_class_t_init(&tpid_class);

    /** Set mapping: Frame has no VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has C-VLAN TAG */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has S-VLAN TAG*/
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has S-VLAN TAG */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has S-Priority-C-VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has S-Priority-C-VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has S-C-VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has S-C-VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has C-C-VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping:Frame has S-S-VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    /** Set mapping: Frame has S-S-VLAN TAGs */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.port = port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &tpid_class), "");

    return BCM_E_NONE;
}

/**
 * \brief - Take PP port soc-properties and call the right BCM PP port APIs accordingly.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_dyn_port_add_packet_processing_properties_flexible_set(
    int unit,
    bcm_port_t port,
    int header_type_in,
    int header_type_out,
    int header_type_only)
{
    uint32 class_id;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_port_config_t port_config;
    bcm_pbmp_t pbmp_eth;
    uint32 flags;

    flags = BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD;
    class_id = port;

    /*
     * Set incoming header type 
     */
    key.type = bcmSwitchPortHeaderType;
    key.index = 1;      /* 0 = Both, 1 = direction IN, 2 = direction OUT */
    value.value = header_type_in;

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit, port, key, value),
        "when setting incoming header type");

    /*
     * Set outgoing header type 
     */
    key.type = bcmSwitchPortHeaderType;
    key.index = 2;      /* 0 = Both, 1 = direction IN, 2 = direction OUT */
    value.value = header_type_out;

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit, port, key, value),
        "when setting outgoing header type");

    if (header_type_only)
    {
        /** Nothing else is required under header_type_only flag */
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &port_config), "");

    /** Below configurations are relevant only for Ethernet ports */
    BCM_PBMP_CLEAR(pbmp_eth);
    BCM_PBMP_OR(pbmp_eth, port_config.e);
    BCM_PBMP_REMOVE(pbmp_eth, port_config.rcy_mirror);
    if (BCM_PBMP_MEMBER(pbmp_eth, port))
    {
        /** Configure VLAN domain per port*/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port, bcmPortClassId, class_id), "");

        /** Set VLAN-membership-namespace per PP port */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port, bcmPortClassVlanMember, class_id), "");

        /** Set vlan port membership */
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, BCM_VLAN_DEFAULT, port, 0), "");

        /** Configure inlif same interface*/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, port, bcmPortControlBridge, TRUE), "");

        /** Configure vlan for untag cases*/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, port, BCM_VLAN_DEFAULT), "");

        /** Enable learning */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_learn_set(unit, port, flags), "");

        if (BCM_PBMP_MEMBER(port_config.nif, port))
        {
            /** Configure default STP state as FORWARD per Ethernet port */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_stp_set(unit, port, BCM_STG_STP_FORWARD), "");
        }

        /** Set port default-LIF to simple bridge:  */
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressMissDrop, 0), "");

        /** Set ingress LLVP classification */
        BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_vlan_llvp_ingress(unit, port), "");

        /** Set egress LLVP classification */
        BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_vlan_llvp_egress(unit, port), "");
    }

    return BCM_E_NONE;
}

/**
 * \brief - Take PP port soc-properties and call the right BCM PP port APIs accordingly.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

int
cint_dyn_port_dnx_port_dyanamic_add_packet_processing_properties_set(
    int unit,
    bcm_port_t port,
    int header_type_in,
    int header_type_out)
{
    return cint_dyn_port_add_packet_processing_properties_flexible_set(unit, port, header_type_in, header_type_out, 0);
}

/*
* Function:
*      cint_dyn_port_remove_port_full_example
* Purpose:
*      Reconfigure single port
* Parameters:
*      unit            - (IN)  Unit number.
*      port            - (IN)  Logical port # - must be a port which already defined by 'ucode_port' soc property.
*      flags           - (IN)  flags
* Returns:
*      BCM_E_NONE     No Error
*      BCM_E_XXX      Error occurred
*
* Assumes port is valid and initialized
* Stages ID is according to the ID when adding port:
* stage 6: disable the port
* stage 5: distroy application reference
* stage 3: configure egr and sch shapers back to zero
* stage 1: remove the port
*
*/
int
cint_dyn_port_remove_port_full_example(
    int unit,
    int port,
    uint32 flags)
{
    char error_msg[200]={0,};
    int commit_changes = 1;
    int is_remove = 1;
    int is_last_in_if = 1;
    int loopback;
    int port_is_nif_dynamic;
    int port_is_flexe_dynamic;
    int port_is_rcy_dynamic;
    int port_is_scheduler_required;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags_get;
    int pp_tm_context_en;
    int flags_mirror_port = 0;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    int first_phy;

    /*
     * Input argument validation: This cint is relevant only for NIF ports (excluding ELK and STIF) and flexe clients
     *
     */
    /* Get port info */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &flags_get, &interface_info, &mapping_info), "");
    pp_tm_context_en = *(dnxc_data_get(unit, "ingr_reassembly", "context", "pp_tm_context", NULL));

    /** Input argument validation: This cint is relevant only for NIF ports (excluding ELK and STIF) and flexe clients */
    BCM_PBMP_ITER(interface_info.phy_pbmp, first_phy)
    {
        break;
    }
    port_is_nif_dynamic = is_port_nif_dynamic (unit, port, interface_info.interface, flags_get, first_phy);
    port_is_flexe_dynamic = is_port_flexe_dynamic (unit, port, interface_info.interface);
    port_is_rcy_dynamic = is_port_rcy_dynamic (unit, port, interface_info.interface);
    if (!port_is_nif_dynamic && !port_is_flexe_dynamic && !port_is_rcy_dynamic)
    {
        printf("Error, cint_dyn_port_remove_port_full_example supports only NIF ports (excluding ELK / STIF / MGMT), FlexE clients and Recycle ports. port=%d\n", port);
        return BCM_E_PORT;
    }

    /*
     * stage 6, disable port (not relevant for RCY port)
     */
    if (!port_is_rcy_dynamic)
    {
        /*
         * Disable port only if port is last_in_if (i.e. not sharing the interface with other channelized port
         * For FlexE clients, it is always single channel port.
         */
        if (port_is_nif_dynamic)
        {
            snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port);
            BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_is_last_in_if(unit, port, &is_last_in_if), error_msg);
        }

        if (is_last_in_if)
        {
            /** first remove the mapped rcy_mirror port in case last channel is being removed */
            if (pp_tm_context_en)
            {
                snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port);
                BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_get(unit, flags_mirror_port, port, &rcy_map_info), error_msg);
                if (rcy_map_info.rcy_mirror_port != BCM_PORT_INVALID)
                {
                    snprintf(error_msg, sizeof(error_msg), "for the mapped rcy_mirror port=%d of forward port %d",
                        rcy_map_info.rcy_mirror_port, port);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_remove(unit, rcy_map_info.rcy_mirror_port, 0), error_msg);
                }
            }
            if (*(dnxc_data_get(unit, "nif", "global", "not_implemented", NULL)) == 0)
            {
                snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port);
                if (port_is_nif_dynamic)
                {
                    /*If phy loopback is enabled - need to first open loopback and restore lane swap*/
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_get (unit, port, &loopback), error_msg);
                    if (loopback == BCM_PORT_LOOPBACK_PHY)
                    {
                        /*open loopback and restore lane swap*/
                        BCM_IF_ERROR_RETURN_MSG(dnx_port_phy_loopback_clear(unit, port), error_msg);
                    }
                }
                BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port, 0), error_msg);
            }

            /* sleep 100 msec to make sure all link status change interrupts for this port are handled */
            sal_usleep(100000);

        } else {
            if (*(dnxc_data_get(unit, "nif", "global", "not_implemented", NULL)) == 0)
            {
                snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port);
                /*
                 * Clear PHY LB DB for the port
                 */
                BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_get (unit, port, &loopback), error_msg);
                if (loopback == BCM_PORT_LOOPBACK_PHY)
                {
                    BCM_IF_ERROR_RETURN_MSG(dnx_port_phy_loopback_db_clear(unit, port), error_msg);
                }
            }
        }
    }
    /** Check if scheduler is required */
    port_is_scheduler_required = is_port_scheduler_required(unit, flags_get, interface_info);
    if (port_is_scheduler_required)
    {   /*
         * stage 5, application reference (JR2 and beyond)
         */
        snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port);
        BCM_IF_ERROR_RETURN_MSG(appl_dnx_dynamic_port_destroy(unit, port), error_msg);

        /*
         * stage 3, before removing the port - set shapers rates to 0 
         */
        {
            /*
             * setting egr shapers rates to zero 
             */
            
            int is_last_shared_otm;
            BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_is_last_shared_otm(unit, port, &is_last_shared_otm), error_msg);
            if (is_last_shared_otm)
            {
                BCM_IF_ERROR_RETURN_MSG(dnx_tune_egr_ofp_rate_set(unit, port, 0, commit_changes, is_remove), error_msg); /* egr expects speed in Kbits/sec */

                /*
                 * setting scheduler rates back to zero
                 */
                if (!(flags_get & (BCM_PORT_ADD_FLEXE_SAR | BCM_PORT_ADD_OTN_SAR)))
                {
                    BCM_IF_ERROR_RETURN_MSG(dnx_tune_scheduler_port_rates_set(unit, port, 0, 0, commit_changes, is_remove), error_msg); /* sch expects speed in Kbits/sec */
                }
            }
        }
    }
    /*
     * stage 1, remove port
     */
    snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_remove(unit, port, flags), error_msg);
    return BCM_E_NONE;
}

/*
* Function:
*      cint_dyn_port_interface_has_single_channel
* Purpose:
*      checks if given port is the first port in the interface
* Parameters:
*      unit                             - (IN)  Unit number.
*      port                             - (IN)  Logical port.
*      interface_has_single_channel     - (OUT) indication if this interface has a single channel
* Returns:
*      BCM_E_NONE     No Error
*      BCM_E_XXX      Error occurred
*/
int
cint_dyn_port_interface_has_single_channel(
    int unit,
    bcm_port_t port,
    int *interface_has_single_channel)
{
    bcm_port_config_t ports_config;
    uint32 port_add_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t ref_phy_port;
    bcm_port_t port_iter;
    uint32 nof_channels = 0;

    /** set first channel indication to true */
    *interface_has_single_channel = TRUE;

    /** if channelized, get phy port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &port_add_flags, &interface_info, &mapping_info), "");
    if(!(port_add_flags & BCM_PORT_ADD_CONFIG_CHANNELIZED))
    {
        /** non channelized port, first and only channel by definition */
        *interface_has_single_channel = TRUE;
        return BCM_E_NONE;
    }
    else
    {
        /** get reference physical port to check if port is first channel */
        ref_phy_port = interface_info.phy_port;
    }

    /** get ports bitmap */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &ports_config), "");

    /** iterate on nif ports only and check if any of them has the same phy port as input port, once found more than 1 break and return FALSE */
    BCM_PBMP_ITER(ports_config.nif, port_iter)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port_iter, &port_add_flags, &interface_info, &mapping_info), "");
        if(interface_info.phy_port == ref_phy_port)
        {
            if ((interface_info.interface == BCM_PORT_IF_ILKN) && (port_add_flags & BCM_PORT_ADD_CROSS_CONNECT))
            {
                /** Skip L1 ILKN port since the first L2 ILKN port needs to call resource_set API to configure resouces */
                continue;
            }
            ++nof_channels;
        }
        if(nof_channels > 1)
        {
            *interface_has_single_channel = FALSE;
            break;
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      cint_dyn_port_add_port_full_example
 * Purpose:
 *      Full example to add port and set relevant shapers
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  Logical port to add.
 *      mapping_info    - (IN)  Logical port mapping info
 *      interface_info  - (IN)  physical port info
 *      resource        - (IN)  NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 *      dfe_mode        - (IN)  dfe mode: on, low power or off
 *      medium_type     - (IN)  medium type: backplane, copper cable or optic
 *      header_type_in  - (IN)  incomming header type
 *      header_type_out - (IN)  outgoing header type
 *      flags           - (IN)
 *
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * stage 1: Add new port
 * stage 2: Configure NIF
 * stage 3: configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: configure application reference
 * stage 6: enable the port
 *
 */

int
cint_dyn_port_add_port_full_example(
    int unit,
    bcm_port_t port,
    bcm_port_mapping_info_t mapping_info,
    bcm_port_interface_info_t interface_info,
    bcm_port_resource_t resource,
    dfe_mode_t dfe_mode,
    medium_type_t medium_type,
    int header_type_in,
    int header_type_out,
    uint32 flags)
{

    appl_dnx_sys_device_t *sys_params;
    int device_id;

    /** Get a pointer to the system parameters */
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_sys_device_params_get(unit, &sys_params), "");
    /** always assume local port */
    device_id = sys_params->unit_to_device_index[unit];

    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_ports_full_example(unit, 1, &port, &device_id, &mapping_info, &interface_info, &resource, &dfe_mode,
                                                &medium_type, &header_type_in, &header_type_out, &flags), "");
    return BCM_E_NONE;
}




/*
 * Function:
 *      cint_dyn_port_add_port_full_example
 * Purpose:
 *      Full example to add port and set relevant shapers accross a system
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  Logical port to add.
 *      device_id       - (IN)  device id.
 *      mapping_info    - (IN)  Logical port mapping info
 *      interface_info  - (IN)  physical port info
 *      resource        - (IN)  NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 *      dfe_mode        - (IN)  dfe mode: on, low power or off
 *      medium_type     - (IN)  medium type: backplane, copper cable or optic
 *      header_type_in  - (IN)  incomming header type
 *      header_type_out - (IN)  outgoing header type
 *      flags           - (IN)
 *
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * Notes:
 *      when the device ID that is connected to this unit is not the device ID recieved in input -
 *      only the relevant parts of configuration from app ref is done - meaning system ports, VOQs creation and connection.
 *
 * stage 1: Add new port
 * stage 2: Configure NIF
 * stage 3: configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: configure application reference
 * stage 6: enable the port
 *
 */

int
cint_dyn_port_add_port_full_example_multi_faps(
    int unit,
    bcm_port_t port,
    int device_id,
    bcm_port_mapping_info_t mapping_info,
    bcm_port_interface_info_t interface_info,
    bcm_port_resource_t resource,
    dfe_mode_t dfe_mode,
    medium_type_t medium_type,
    int header_type_in,
    int header_type_out,
    uint32 flags)
{
    return cint_dyn_port_add_ports_full_example(unit, 1, &port, &device_id, &mapping_info, &interface_info, &resource, &dfe_mode,
                                                &medium_type, &header_type_in, &header_type_out, &flags);
}

/*
 * Function:
 *      cint_dyn_port_add_ports_full_example_extended
 * Purpose:
 *      Full example to add ports and set relevant shapers
 *      This example is used to configure port resouces for multi ports
 *      simultaneously using API bcm_port_resource_multi_set. This example
 *      can provide better port VCO assignment when configuring multi ports
 *      on one PM.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      nof_ports       - (IN)  Number of ports
 *      port_list       - (IN)  Ports array
 *      device_id_list  - (IN)  devices IDs array
 *      mapping_info    - (IN)  Logical port mapping info
 *      interface_info  - (IN)  physical port info
 *      resource        - (IN)  NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 *      dfe_mode        - (IN)  dfe mode: on, low power or off
 *      medium_type     - (IN)  medium type: backplane, copper cable or optic
 *      header_type_in  - (IN)  incomming header type
 *      header_type_out - (IN)  outgoing header type
 *      flags           - (IN)  flags for bcm_port_add API
 *      g_config   - (IN) structure that holds configurations
 *
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * stage 1: Add new ports
 * stage 2: Configure NIF resources
 * stage 3: Configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: Configure application reference
 * stage 6: Enable the ports
 *
 */

int
cint_dyn_port_add_ports_full_example_extended(
    int unit,
    int nof_ports,
    bcm_port_t *port_list,
    int *device_id_list,
    bcm_port_mapping_info_t *mapping_info,
    bcm_port_interface_info_t *interface_info,
    bcm_port_resource_t *resource,
    dfe_mode_t *dfe_mode,
    medium_type_t *medium_type,
    int *header_type_in,
    int *header_type_out,
    uint32 *flags,
    cint_dyn_port_global_configurations_t *g_config)
{
    int port_total_rate_mhz, nof_lanes;
    char error_msg[200]={0,};
    int nof_ports_resource_set = 0;
    bcm_port_resource_t resource_set_arr[nof_ports];
    bcm_gport_t egress_q_gport;
    bcm_pbmp_t phys_bmp;
    bcm_port_phy_tx_t tx_taps;
    int commit_changes = 1, port_index;
    int is_remove = 0;
    int port_is_nif_dynamic;
    int port_is_flexe_dynamic;
    int port_is_rcy_dynamic;
    int port_is_scheduler_required;
    int interface_has_single_channel;
    bcm_port_interface_info_t interface_info_tmp;
    bcm_port_mapping_info_t mapping_info_tmp;
    uint32 flags_tmp = 0;
    uint32 appl_flags = 0;
    int ilkn_data_rate;
    system_port_mapping_t sys_port_map;
    int port_is_local;
    appl_dnx_sys_device_t *sys_params;
    int first_phy;
    /** Get a pointer to the system parameters */
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_sys_device_params_get(unit, &sys_params), "");

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /** check if port is local to this device */
        port_is_local = (sys_params->unit_to_device_index[unit] == device_id_list[port_index]);
        if (!port_is_local)
        {
            /** this step is required only for local ports */
            continue;
        }

        /** Input argument validation: This cint is relevant only for NIF ports (excluding ELK and STIF) and flexe clients */
        BCM_PBMP_ITER(interface_info[port_index].phy_pbmp, first_phy)
        {
            break;
        }
        port_is_nif_dynamic = is_port_nif_dynamic (unit, port_list[port_index], interface_info[port_index].interface, flags[port_index], first_phy);
        port_is_flexe_dynamic = is_port_flexe_dynamic (unit, port_list[port_index], interface_info[port_index].interface);
        port_is_rcy_dynamic = is_port_rcy_dynamic (unit, port_list[port_index], interface_info[port_index].interface);
        if (!port_is_nif_dynamic && !port_is_flexe_dynamic && !port_is_rcy_dynamic)
        {
            printf("Error, cint_dyn_port_add_ports_full_example supports only NIF ports (excluding ELK / STIF / MGMT), FlexE clients and Recycle ports. port=%d\n", port_list[port_index]);
            return BCM_E_PORT;
        }

        /** stage 1 - adding the port */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_add(unit, port_list[port_index], flags[port_index],
                          &interface_info[port_index], &mapping_info[port_index]), "");

        BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_interface_has_single_channel(unit, port_list[port_index], &interface_has_single_channel), "");

        /** stage 2 - configure NIF properties */
        /*
         * Set resource  - this should be done only for first port on interface
         * For FlexE client - no need to set the resource. The speed of FlexE is configured bcm_port_flexe_group_cal_slots_set.
         */
        if (interface_has_single_channel && (interface_info[port_index].interface != BCM_PORT_IF_FLEXE_CLIENT))
        {
            sal_memcpy(&resource_set_arr[nof_ports_resource_set], &resource[port_index], sizeof(resource_set_arr[0]));
            if ((interface_info[port_index].interface != BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT) && (interface_info[port_index].interface != BCM_PORT_IF_FLEXE_CLIENT)
                && (interface_info[port_index].interface != BCM_PORT_IF_RCY))
            {
                if (*(dnxc_data_get(unit, "nif", "global", "not_implemented", NULL)) == 0)
                {
                    snprintf(error_msg, sizeof(error_msg), "unit %d logical_port: %x", unit, port_list[port_index]);
                    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_resource_fill(unit, port_list[port_index],
                        dfe_mode[port_index], medium_type[port_index], &resource_set_arr[nof_ports_resource_set]), error_msg);
                    if (tvco_prioritize_26p562g_flag == 1) {
                        resource_set_arr[nof_ports_resource_set].flags = BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G;
                    }
                }
            }
            nof_ports_resource_set++;
        }
    }

    if (*(dnxc_data_get(unit, "nif", "global", "not_implemented", NULL)) == 0)
    {
        snprintf(error_msg, sizeof(error_msg), "unit=%d", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_multi_set(unit, nof_ports_resource_set, resource_set_arr), error_msg);
    }


    for (port_index = 0; port_index < nof_ports_resource_set; ++port_index)
    {
        /** check if port is local to this device */
        port_is_local = (sys_params->unit_to_device_index[unit] == device_id_list[port_index]);
        if (!port_is_local)
        {
            /** this step is required only for local ports */
            continue;
        }

        /** If no link training set tx taps */
        if (resource_set_arr[port_index].link_training == 0)
        {
            /** For NRZ - default Tx taps are OK.Only in PAM4 need to set Tx taps for the Ixia 400G setup */
            snprintf(error_msg, sizeof(error_msg), "default, unit=%d, port=%d", unit, resource_set_arr[port_index].port);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, resource_set_arr[port_index].port, &flags_tmp, &interface_info_tmp, &mapping_info_tmp), error_msg);
            if ((interface_info_tmp.interface != BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT) && (interface_info_tmp.interface != BCM_PORT_IF_RCY))
            {
                BCM_PBMP_COUNT(interface_info_tmp.phy_pbmp, nof_lanes);
                if (resource_set_arr[port_index].speed/nof_lanes > 28125)
                {
                    tx_taps.pre2 = 0;
                    tx_taps.pre = -24;
                    tx_taps.main = 132;
                    tx_taps.post = -12;
                    tx_taps.post2 = 0;
                    tx_taps.post3 = 0;
                    tx_taps.tx_tap_mode = bcmPortPhyTxTapMode6Tap;
                    tx_taps.signalling_mode = bcmPortPhySignallingModePAM4;
                    snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, resource_set_arr[port_index].port);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_phy_tx_set(unit, resource_set_arr[port_index].port, &tx_taps), error_msg);

                    /* ER channels need to enable TX precoding */
                    if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource_set_arr[port_index].phy_lane_config))
                    {
                        BCM_IF_ERROR_RETURN_MSG(bcm_port_phy_control_set(unit, resource_set_arr[port_index].port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, 1), error_msg);

                        BCM_IF_ERROR_RETURN_MSG(bcm_port_phy_control_set(unit, resource_set_arr[port_index].port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, 1), error_msg);
                    }
                }
            }
        }
    }

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /** check if port is local to this device */
        port_is_local = (sys_params->unit_to_device_index[unit] == device_id_list[port_index]);
        /** Check if scheduler is required */
        port_is_scheduler_required = is_port_scheduler_required(unit, flags[port_index], interface_info[port_index]);
        if (port_is_scheduler_required)
        {
            if (port_is_local)
            {
                /** stage 3 - configure egr and sch shapers */
                switch (interface_info[port_index].interface)
                {
                    case BCM_PORT_IF_NIF_ETH:
                        port_total_rate_mhz = resource->speed;       /* ETH: rate is port rate */
                        break;
                    case BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT:
                        port_total_rate_mhz = resource->speed;       /* Flexe MAC client: rate is port rate */
                        break;
                    case BCM_PORT_IF_ILKN:
                        snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port_list[port_index]);
                        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_get(unit, port_list[port_index], bcmPortControlDataRate, &ilkn_data_rate), error_msg);
                        port_total_rate_mhz = ilkn_data_rate; /* ILKN: rate is interface data rate */
                        break;
                    case BCM_PORT_IF_CPU:  /* 1G */
                        port_total_rate_mhz = 1000;
                        break;
                    case BCM_PORT_IF_RCY:
                        port_total_rate_mhz = resource->speed;       /* RCY: rate is interface rate */
                        break;
                    default:
                        printf("Unsupported interface: %d\n", interface_info[port_index].interface);
                        return BCM_E_PARAM;
                        break;
                }

                {
                    /** No need for speedup, as it is done inside dnx_tune_egr_ofp_rate_set */
                    /* egr expects speed in Kbits/sec */
                    commit_changes = (port_index == nof_ports - 1) ? 1 : 0;
                    snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port_list[port_index]);
                    BCM_IF_ERROR_RETURN_MSG(dnx_tune_egr_ofp_rate_set(unit, port_list[port_index], port_total_rate_mhz * 1000, commit_changes, is_remove), error_msg);
                }
                if (!(flags[port_index] & (BCM_PORT_ADD_FLEXE_SAR | BCM_PORT_ADD_OTN_SAR)))
                {
                    /** No need for speedup, as it is done inside dnx_tune_scheduler_port_rates_set */
                    /* sch expects speed in Kbits/sec */
                    snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port_list[port_index]);
                    BCM_IF_ERROR_RETURN_MSG(dnx_tune_scheduler_port_rates_set(unit, port_list[port_index], 0, port_total_rate_mhz * 1000, commit_changes, is_remove), error_msg);

                    /** stage 4 - PP properties */
                    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_packet_processing_properties_flexible_set(unit, port_list[port_index], header_type_in[port_index],
                        header_type_out[port_index], g_config->pp_set_header_only), error_msg);
                }
            }

            {
                /** stage 5 - application reference */
                sys_port_map.device_index = device_id_list[port_index];
                sys_port_map.local_port = port_list[port_index];
                sys_port_map.core = mapping_info[port_index].core;
                if(mapping_info[port_index].pp_dsp == -1)
                {
                    /** currently getting both TM port and pp_dsp as input is invalid,
                     * until this is changed, assume that if pp_dsp was -1 use TM port's value instead */
                    sys_port_map.pp_dsp = mapping_info[port_index].tm_port;
                }
                else
                {
                    sys_port_map.pp_dsp = mapping_info[port_index].pp_dsp;
                }
                sys_port_map.rate = resource[port_index].speed;
                /** Convert device and logical port to system port id */
                BCM_IF_ERROR_RETURN_MSG(appl_sand_device_to_sysport_convert(unit, sys_port_map.device_index, APPL_DNX_NOF_SYSPORTS_PER_DEVICE,
                    sys_port_map.local_port, &sys_port_map.system_port_id), "");
                /** Port might not be local to this FAP */
                appl_flags = APPL_DNX_DYN_PORT_USE_SYS_PORT_MAP;
                if(g_config->skip_port_compensation)
                {
                    appl_flags = appl_flags | APPL_DNX_DYN_PORT_SKIP_COMPENSATION_PER_PORT;
                }
                snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port_list[port_index]);
                BCM_IF_ERROR_RETURN_MSG(appl_dnx_dynamic_port_create(unit, appl_flags, &sys_port_map, flags[port_index], interface_info[port_index].flags, port_list[port_index]), error_msg);
            }
        }
        else
        {
            /*
             * Enable Linkscan for FlexE phy, eth L1 and ILKN L1.
             */
            if (port_is_local && ((interface_info[port_index].interface == BCM_PORT_IF_NIF_ETH) ||
                (interface_info[port_index].interface == BCM_PORT_IF_ILKN) ||
                (interface_info[port_index].interface == BCM_PORT_IF_OTN)))
            {
                snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port_list[port_index]);
                BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_mode_set(unit, port_list[port_index], BCM_LINKSCAN_MODE_SW), error_msg);
            }
        }
        if(port_is_local)
        {
            BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_interface_has_single_channel(unit, port_list[port_index], &interface_has_single_channel), "");

            /** stage 6 - enable the port - this should be done only for first port on interface */
            if ((interface_has_single_channel) && (interface_info[port_index].interface != BCM_PORT_IF_RCY))
            {
                if (*(dnxc_data_get(unit, "nif", "global", "not_implemented", NULL)) == 0)
                {
                    snprintf(error_msg, sizeof(error_msg), "unit=%d, port=%d", unit, port_list[port_index]);
                    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit,  port_list[port_index], enable_port_at_last), error_msg);
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      cint_dyn_port_add_ports_full_example_pp
 * Purpose:
 *      Full example to add ports and set relevant shapers
 *      This example is used to configure port resouces for multi ports
 *      simultaneously using API bcm_port_resource_multi_set. This example
 *      can provide better port VCO assignment when configuring multi ports
 *      on one PM.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      nof_ports       - (IN)  Number of ports
 *      port_list       - (IN)  Ports array
 *      device_id_list  - (IN)  devices IDs array
 *      mapping_info    - (IN)  Logical port mapping info
 *      interface_info  - (IN)  physical port info
 *      resource        - (IN)  NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 *      dfe_mode        - (IN)  dfe mode: on, low power or off
 *      medium_type     - (IN)  medium type: backplane, copper cable or optic
 *      header_type_in  - (IN)  incomming header type
 *      header_type_out - (IN)  outgoing header type
 *      flags           - (IN)  flags for bcm_port_add API
 *      pp_set_header_only - (IN) if true, set header type only and skip other PP properties
 *
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * stage 1: Add new ports
 * stage 2: Configure NIF resources
 * stage 3: Configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: Configure application reference
 * stage 6: Enable the ports
 *
 */

int
cint_dyn_port_add_ports_full_example_pp(
    int unit,
    int nof_ports,
    bcm_port_t *port_list,
    int *device_id_list,
    bcm_port_mapping_info_t *mapping_info,
    bcm_port_interface_info_t *interface_info,
    bcm_port_resource_t *resource,
    dfe_mode_t *dfe_mode,
    medium_type_t *medium_type,
    int *header_type_in,
    int *header_type_out,
    uint32 *flags,
    int pp_set_header_only)
{
    cint_dyn_port_global_configurations_t g_config;
    g_config.skip_port_compensation = 0;
    g_config.pp_set_header_only = pp_set_header_only;
    return cint_dyn_port_add_ports_full_example_extended(unit, nof_ports, port_list, device_id_list,
                                                         mapping_info, interface_info, resource,
                                                         dfe_mode, medium_type,
                                                         header_type_in, header_type_out,
                                                         flags, &g_config);
}

/*
 * Function:
 *      cint_dyn_port_add_ports_full_example
 * Purpose:
 *      Full example to add ports and set relevant shapers
 *      This example is used to configure port resouces for multi ports
 *      simultaneously using API bcm_port_resource_multi_set. This example
 *      can provide better port VCO assignment when configuring multi ports
 *      on one PM.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      nof_ports       - (IN)  Number of ports
 *      port_list       - (IN)  Ports array
 *      device_id_list  - (IN)  devices IDs array
 *      mapping_info    - (IN)  Logical port mapping info
 *      interface_info  - (IN)  physical port info
 *      resource        - (IN)  NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 *      dfe_mode        - (IN)  dfe mode: on, low power or off
 *      medium_type     - (IN)  medium type: backplane, copper cable or optic
 *      header_type_in  - (IN)  incomming header type
 *      header_type_out - (IN)  outgoing header type
 *      flags           - (IN)  flags for bcm_port_add API
 *
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * stage 1: Add new ports
 * stage 2: Configure NIF resources
 * stage 3: Configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: Configure application reference
 * stage 6: Enable the ports
 *
 */

int
cint_dyn_port_add_ports_full_example(
    int unit,
    int nof_ports,
    bcm_port_t *port_list,
    int *device_id_list,
    bcm_port_mapping_info_t *mapping_info,
    bcm_port_interface_info_t *interface_info,
    bcm_port_resource_t *resource,
    dfe_mode_t *dfe_mode,
    medium_type_t *medium_type,
    int *header_type_in,
    int *header_type_out,
    uint32 *flags)
{
    return cint_dyn_port_add_ports_full_example_pp(unit, nof_ports, port_list, device_id_list,
                                                   mapping_info, interface_info, resource,
                                                   dfe_mode, medium_type,
                                                   header_type_in, header_type_out,
                                                   flags, 0);
}
/* The below cint function is an example for adding Ixia port
 * Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - Ixia requirments:
 *     - Link training: 'off'
 *     - DFE: 'Off' for 10G. 'On' for 100G and 400G
 *     - medium type: 'optic' for 10G. 'copper' for 100G and 400G
 *     - NIF interface: ETH
 *     - Header type (in and out): ETH
 *  - FEC: default according to speed
 *  - TM port: same id as the logical port
 *  - Priorities: 2
 *
 * */
int
cint_dyn_port_add_ixia_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz,
    uint32 flags)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    int header_type_in, header_type_out;
    char error_msg[200]={0,};

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    bcm_port_resource_t_init(resource);

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_NIF_ETH;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);

    /** Configuring  mapping_info*/
    BCM_PBMP_ITER(phy_pbmp, first_phy)
    {
        break;
    }
    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;
    mapping_info.core = first_phy / nof_links_per_core;
    mapping_info.channel = 0;

    if (!(flags & BCM_PORT_ADD_CROSS_CONNECT))
    {
        mapping_info.tm_port = port;
        mapping_info.num_priorities = 2;

        /*
         * Configuring PP properties
         */
        header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    }

    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    resource.link_training = 0;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    /*
     * We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically
     */
    if (rate_mhz == 10 * 1000)  /* Ixia requirments for 10G: Optic, no DFE, no link training */
    {
        dfe_mode = dfe_none;
        medium_type = optic;
    }
    else if ((rate_mhz == 100 * 1000) || (rate_mhz == 400 * 1000))      /* Ixia requirments for 100G or 400G: DFE, no
                                                                         * link training */
    {
        dfe_mode = dfe_on;
        medium_type = copper_cable;
    }
    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             dfe_mode, medium_type, header_type_in, header_type_out, flags), error_msg);
    return BCM_E_NONE;

}

/* The below cint function is an example for adding normal eth port or L1 eth port
 *  Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - SerDes configuration to suite simple loopback or p2p cable connected port:
 *     - Link training: 'on'
 *     - DFE: 'on'
 *     - medium type: 'copper'
 *  - NIF interface: ETH
 *  - Header type (in and out): ETH
 *  - FEC: default according to speed
 *  - TM port: same id as the logical port
 *  - Priorities: 2
 *
 * */
int
cint_dyn_port_add_eth_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz,
    int fec_type,
    uint32 flags,
    int mgmt_channel)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    int header_type_in, header_type_out;
    char error_msg[200]={0,};

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    bcm_port_resource_t_init(resource);

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_NIF_ETH;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);

    /** Configuring  mapping_info*/
    BCM_PBMP_ITER(phy_pbmp, first_phy)
    {
        break;
    }
    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;
    mapping_info.core = first_phy / nof_links_per_core;

    if (!(flags & BCM_PORT_ADD_CROSS_CONNECT) && !(flags & BCM_PORT_ADD_IF_FRAMER_MGMT))
    {
        mapping_info.tm_port = port;
        mapping_info.num_priorities = 2;

        /*
         * Configuring PP properties
         */
        header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    }
    if (flags & BCM_PORT_ADD_IF_FRAMER_MGMT)
    {
        mapping_info.mgmt_channel = mgmt_channel;
    }
    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = fec_type;
    resource.link_training = (flags & BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT) ? 0 : 1;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    /*
     * We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically
     */
    dfe_mode = dfe_on;
    medium_type = copper_cable;
    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             dfe_mode, medium_type, header_type_in, header_type_out, flags), error_msg);
    return BCM_E_NONE;

}

/* The below cint function is an example for adding normal FlexE PHY port
 *  Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - SerDes configuration to suite simple loopback or p2p cable connected port:
 *     - Link training: according to input
 *     - DFE: 'on'
 *     - medium type: 'copper'
 *  - NIF interface: FlexE physical port
 *  - Header type (in and out): None
 *  - FEC: default according to speed
 *  - TM port: None
 *  - Priorities: None
 *
 * */
int
cint_dyn_port_add_flexe_phy_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz,
    int cl72_en,
    uint32 flags)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    int header_type_in, header_type_out;
    char error_msg[200]={0,};

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    bcm_port_resource_t_init(resource);

    /** Mark the port is flexe physical port */
    flags |= BCM_PORT_ADD_FLEXE_PHY;
    if (flexe_phy_dpp_with_flr)
    {
        cl72_en = 0; /* Link training should be disabled when FLR on */
        flags |= BCM_PORT_ADD_FLR_FIRMWARE_SUPPORT;
    }

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_NIF_ETH;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);

    /** Configuring  mapping_info*/
    BCM_PBMP_ITER(phy_pbmp, first_phy)
    {
        break;
    }
    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;
    mapping_info.core = first_phy / nof_links_per_core;

    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = flexe_phy_fec_type > 0 ? flexe_phy_fec_type : BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    resource.link_training = cl72_en;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    resource.base_flexe_instance = flexe_phy_instance;
    /*
     * We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically
     */
    dfe_mode = dfe_on;
    medium_type = copper_cable;

    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             dfe_mode, medium_type, 0, 0, flags), error_msg);
    return BCM_E_NONE;

}
/* The below cint function is an example for adding normal Tx feedback
 *  Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - SerDes configuration to suite simple loopback or p2p cable connected port:
 *     - Link training: according to input
 *     - DFE: 'on'
 *     - medium type: 'copper'
 *  - NIF interface: ETH TX feedback port
 *  - Header type (in and out): None
 *  - FEC: default according to speed
 *  - TM port: None
 *  - Priorities: None
 *
 * */
int
cint_dyn_port_add_tx_feedback_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    char error_msg[200] = { 0 };

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    bcm_port_resource_t_init(resource);

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_NIF_ETH;
    interface_info.flags |= BCM_PORT_ADD_TX_FEEDBACK_ONLY;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);

    /** Configuring  mapping_info*/
    BCM_PBMP_ITER(phy_pbmp, first_phy)
    {
        break;
    }
    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;
    mapping_info.core = first_phy / nof_links_per_core;

    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    resource.link_training = 1;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    /*
     * We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically
     */
    dfe_mode = dfe_on;
    medium_type = copper_cable;
    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                                                dfe_mode, medium_type, 0, 0, 0), error_msg);
    return BCM_E_NONE;

}

/* The below cint function is an example for adding FlexE unware port
 *  Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - SerDes configuration to suite simple loopback or p2p cable connected port:
 *     - Link training: according to input
 *     - DFE: 'on'
 *     - medium type: 'copper'
 *  - NIF interface: FlexE physical port
 *  - Header type (in and out): None
 *  - FEC: default according to speed
 *  - TM port: None
 *  - Priorities: None
 *
 * */
int
cint_dyn_port_add_flexe_unaware_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz,
    int cl72_en)
{
    char error_msg[200]={0,};
    uint32 flags = 0;

    /** Mark the port is flexe unaware port */
    flags |= BCM_PORT_ADD_FLEXE_UNAWARE;
    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_flexe_phy_port(unit, port, phy_pbmp, rate_mhz, cl72_en, flags), error_msg);
    return BCM_E_NONE;

}

/* The below cint function is an example for adding port (channel) of ILKN data.
 * Note: Device need to support ILKN data (JR2, for example, doesn't support that)
 * Port description:
 *  - Logical port, phys, channel id and rate - user decision (input arguments)
 *  - Defaults
 *     - Link training: 'on'
 *     - DFE: 'On'
 *     - medium type: 'copper'
 *     - NIF interface: ILKN
 *     - Header type (in and out): ETH
 *  - FEC: default according to speed
 *  - TM port: same id as the logical port
 *  - Priorities: 2
 *
 * */
int
cint_dyn_port_add_ilkn_ports(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    uint32 ilkn_id,
    uint32 channel,
    int rate_mhz,
    int fec_type,
    uint32 flags)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    int header_type_in, header_type_out;
    char error_msg[200]={0,};

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_mapping_info_t_init(&mapping_info);
    bcm_port_resource_t_init(resource);

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_ILKN;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);
    interface_info.interface_id = ilkn_id;

    /** Configuring  mapping_info*/
    BCM_PBMP_ITER(phy_pbmp, first_phy)
    {
        break;
    }
    nof_links = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
    nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    nof_links_per_core = nof_links / nof_cores;
    mapping_info.core = first_phy / nof_links_per_core;
    mapping_info.channel = channel;
    if (!(flags & BCM_PORT_ADD_CROSS_CONNECT))
    {
        mapping_info.tm_port = port;
        mapping_info.num_priorities = 2;

        /*
         * Configuring PP properties
         */
        header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    }

    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = fec_type;
    resource.link_training = 1;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    /*
     * We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically
     */
    dfe_mode = dfe_on;
    medium_type = copper_cable;
    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             dfe_mode, medium_type, header_type_in, header_type_out, flags), error_msg);
    return BCM_E_NONE;

}

/* The below cint function is an example for adding FlexE busa/busb/busc clients.
 * Note: Device need to support FlexE feature
 * Port description:
 *  - Logical port:user decision (input arguments)
 *  - if_type: BCM_PORT_IF_FLEXE_CLIENT/BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT
 *  - chanel: Set it to "0" for non OAM/PTP clients
 *  - rx_speed: Rx speed
 *  - tx_speed: Tx speed
 *  - flags: port add flags
 *  - if_flags: flags for interface_info
 *
 * */
int
cint_dyn_port_add_flexe_client(
    int unit,
    bcm_port_t port,
    bcm_port_if_t if_type,
    uint32 channel,
    int rx_speed,
    int tx_speed,
    uint32 flags,
    uint32 if_flags,
    uint32 mgmt_channel)
{

    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_resource_t resource;
    int header_type_in, header_type_out;
    int sar_is_tdm_port;
    char error_msg[200]={0,};

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    sal_memset(&resource, 0, sizeof(resource));
    bcm_port_mapping_info_t_init(&mapping_info);

    /** Configuring  interface_info*/
    interface_info.interface = if_type;
    interface_info.interface_id = -1;
    interface_info.flags = if_flags;

    /** Configuring mapping_info*/
    mapping_info.core = 0;
    mapping_info.channel = channel;

    if ((flags & BCM_PORT_ADD_FLEXE_MAC) &&
        !((flags & BCM_PORT_ADD_CROSS_CONNECT) || (if_flags & BCM_PORT_ADD_FLEXE_PROTECTION) || (flags & BCM_PORT_ADD_STAT_ONLY)))
    {
        mapping_info.tm_port = port;
        mapping_info.num_priorities = 2;

        /*
         * Configuring PP properties
         */
        header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    }
    if (flags & BCM_PORT_ADD_IF_FRAMER_MGMT)
    {
        mapping_info.mgmt_channel = mgmt_channel;
    }
    if (flags & (BCM_PORT_ADD_FLEXE_SAR | BCM_PORT_ADD_OTN_SAR))
    {
        sar_is_tdm_port = *(dnxc_data_get(unit, "nif", "sar_client", "is_tdm_port", NULL));
        if (sar_is_tdm_port)
        {
            mapping_info.tm_port = port;
            mapping_info.num_priorities = 2;
        }
    }
    /** Configuring flexe client resource*/
    resource.port = port;
    resource.speed = tx_speed;
    if (rx_speed != tx_speed)
    {
        resource.flags |= BCM_PORT_RESOURCE_ASYMMETRICAL;
        resource.rx_speed = rx_speed;
    }
    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             0, 0, header_type_in, header_type_out, flags), error_msg);
    return BCM_E_NONE;

}

/* The below cint function is an example for adding normal OTN PHY port
 *  Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - SerDes configuration to suite simple loopback or p2p cable connected port:
 *     - Link training: according to input
 *     - DFE: 'on'
 *     - medium type: 'copper'
 *  - NIF interface: OTN physical port
 *  - Header type (in and out): None
 *  - FEC: None
 *  - TM port: None
 *  - Priorities: None
 *  - logical_otn_lane: the first logical OTN lane.
 *
 * */
int
cint_dyn_port_add_otn_phy_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz,
    int cl72_en,
    int logical_otn_lane,
    uint32 flags)
{
    int i;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_resource_t resource;
    char error_msg[200]={0,};

    /** init vars */
    sal_memset(&interface_info, 0, sizeof(interface_info));
    bcm_port_resource_t_init(&resource);
    bcm_port_mapping_info_t_init(&mapping_info);

    /** Configuring  interface_info*/
    interface_info.interface = BCM_PORT_IF_OTN;
    BCM_PBMP_ASSIGN(interface_info.phy_pbmp, phy_pbmp);

    /** Configuring mapping_info*/
    mapping_info.core = 0;

    /** Configuring flexe client resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = bcmPortPhyFecNone;
    resource.link_training = cl72_en;
    resource.logical_otn_lane = logical_otn_lane;
    /*
     * Add the port
     */
    snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             0, 0, 0, 0, flags), error_msg);
    return BCM_E_NONE;
}

/* select the serdes lane randomly */
/* serdes_allocation_final is the final selected serdes array and the length is random_lane_num */
int
cint_dyn_port_select_random_serdes(
    int unit)
{
    int count = 12;
    int random_lane_index;
    int i, j, k;

    random_lane_num = sal_rand() % 12;
    if (random_lane_num == 0)
    {
        random_lane_num = 1;
    }
    printf("The random_lane_num %d is \n", random_lane_num);

    for (i = 0; i < random_lane_num; i++)
    {
        random_lane_index = sal_rand() % count;
        printf("The random_lane_index %d is selected\n", random_lane_index);

        serdes_allocation_final[i] = serdes_allocation_temp[random_lane_index];
        for (j = random_lane_index; j < count - 1; j++)
        {
            serdes_allocation_temp[j] = serdes_allocation_temp[j + 1];
        }

        count--;
    }

    for (k = 0; k < random_lane_num; k++)
    {
        printf("Each serdes in serdes_allocation_final is %d\n", serdes_allocation_final[k]);
    }

    return BCM_E_NONE;
}

/* add the channelized ILKN port */
int
cint_dyn_port_add_channelized_ilkn_port(
    int unit,
    int core_id,
    int phy_id,
    int local_port,
    int tm_port,
    int interface_id,
    int channel_num,
    int num_priorities,
    int num_lanes,
    int is_phy_pbmp)
{
    char error_msg[200]={0,};
    int i, flag = 0, logical_port;
    int is_first = 1;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_pbmp_t pbmp_add;
    bcm_pbmp_t pbmp;
    int rate_mhz;

    bcm_port_mapping_info_t_init(&mapping_info);
    BCM_PBMP_CLEAR(pbmp);
    for (i = 0; i < num_lanes; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, serdes_allocation_final[i]);
    }

    for (i = 0; i <= (channel_num - 1); i++)
    {
        logical_port = local_port + i;

        mapping_info.channel = i;
        mapping_info.core = core_id;
        mapping_info.num_priorities = num_priorities;
        mapping_info.tm_port = logical_port;

        interface_info.interface = BCM_PORT_IF_ILKN;
        interface_info.interface_id = interface_id;

        if (is_phy_pbmp)
        {
            interface_info.phy_pbmp = pbmp;
            flag = BCM_PORT_ADD_CONFIG_CHANNELIZED | BCM_PORT_ADD_USE_PHY_PBMP;
        }
        else
        {
            interface_info.phy_port = phy_id;
            interface_info.num_lanes = num_lanes;
            flag = BCM_PORT_ADD_CONFIG_CHANNELIZED;
        }

        snprintf(error_msg, sizeof(error_msg), "unit: %d, logical_port: %d", unit, port);
        BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_add_port_full_example(unit, logical_port, mapping_info, interface_info, flag, rate_mhz), error_msg);

        /*
         * stage 2 - rates
         */
        BCM_PBMP_CLEAR(pbmp_add);
        BCM_PBMP_PORT_ADD(pbmp_add, logical_port);
        rate_mhz = (is_first ? 125 * 100 : 1 * 1000);

        snprintf(error_msg, sizeof(error_msg), "rate: %d", rate_mhz);
        BCM_IF_ERROR_RETURN_MSG(config_ports_rate(unit, pbmp_add, rate_mhz, BCM_SWITCH_PORT_HEADER_TYPE_ETH, TRUE), error_msg);
                                                                                                        /* After
                                                                                                         * reconfiguring 
                                                                                                         * * the port:
                                                                                                         * set the
                                                                                                         * added port
                                                                                                         * rate to its
                                                                                                         * speed */
        is_first = 0;
    }

    return BCM_E_NONE;
}

/* delete all ports */
int
cint_dyn_port_remove_all(
    int unit)
{
    char error_msg[200]={0,};
    int port, flag = 0;
    bcm_port_config_t config;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags_get;
    int first_phy;
    
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), "");

    BCM_PBMP_ITER(config.nif, port)
    {
        /* Get port info */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &flags_get, &interface_info, &mapping_info), "");

        BCM_PBMP_ITER(interface_info.phy_pbmp, first_phy)
        {
            break;
        }
        if (is_port_nif_dynamic (unit, port, interface_info.interface, flags_get, first_phy))
        {
            snprintf(error_msg, sizeof(error_msg), "remove port fail port=%d", port);
            BCM_IF_ERROR_RETURN_MSG(cint_dyn_port_remove_port_full_example(unit, port, flag), error_msg);
        }
    }

    return BCM_E_NONE;
}


/* *
 * The below cint function is an example for checking if port is last in if.
 * is_last_in_if is relevant only for channelized port. Still, the cint will work also for non-channelized ports
 * */
int
cint_dyn_port_is_last_in_if(
    int unit,
    bcm_port_t port,
    int * is_last_in_if)
{
    unsigned int inquired_port_first_phy, other_port_first_phy;
    bcm_port_config_t config;
    bcm_port_t logical_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags = 0;

    *is_last_in_if = 1;

    /* Get the first phy of the inquired port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info), "");
    inquired_port_first_phy = interface_info.phy_port;

    /* Get all NIF ports and remove the inquired port from all-nif-ports list*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), "");
    BCM_PBMP_PORT_REMOVE(config.nif, port);

    BCM_PBMP_ITER(config.nif, logical_port)
    {
        /* Get the first phy of the other port */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info), "");
        other_port_first_phy = interface_info.phy_port;

        /* Check if port is last_in_if */
        if (other_port_first_phy == inquired_port_first_phy)
        {
            /* Port is not last in if */
            *is_last_in_if = 0;
            break;
        }
    }
    return BCM_E_NONE;
}

/* *
 * Check if another port has the same out TM port as the provided port
 * */
int
cint_dyn_port_is_last_shared_otm(
    int unit,
    bcm_port_t port,
    int * is_last_shared_otm)
{
    int out_tm_port;
    bcm_port_config_t config;
    bcm_port_t logical_port;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_core_t core;
    uint32 flags = 0;

    *is_last_shared_otm = 1;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info), "");

    if (interface_info.interface != BCM_PORT_IF_RCY)
    {
        /** only RCY can have shared OTM port */
        return BCM_E_NONE;
    }

    out_tm_port = mapping_info.out_tm_port;
    core = mapping_info.core;

    /* Get all RCY ports and remove the input port from all-rcy-ports list*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &config), "");
    BCM_PBMP_PORT_REMOVE(config.rcy, port);

    BCM_PBMP_ITER(config.rcy, logical_port)
    {
        /* Get out TM port of the other port */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info), "");

        /* Check if port shares the same out tm port */
        if (core == mapping_info.core && out_tm_port == mapping_info.out_tm_port)
        {
            /* Port is not last shared out tm port */
            *is_last_shared_otm = 0;
            break;
        }
    }
    return BCM_E_NONE;
}

/**
 * Purpose: To get all free port mapping resource of a provided type.
 * Possible types are as in the enum port_mapping_resource_t:
 *   - logical_ports,
 *   - tm_ports,
 *   - port_base_q_pairs,
 *   - nif_phy_lanes
 */
int cint_dyn_port_mapping_free_resources_get (
    int unit,
    port_mapping_resource_t free_resource_type_requested,
    int *free_resources_array,
    int *nof_free_resources
)
{
    uint32 array_size, flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_config_t port_config;
    bcm_pbmp_t pbmp, phy_pbmp, *supported_phys_pbmp;
    int logical_port, phy, is_found = 0;
    int is_ilkn_over_fabric = 0;
    int resource, occupied_resource, free_resource_index = 0;
    int i;

    switch (free_resource_type_requested) {
        case logical_ports:
            array_size = *(dnxc_data_get(unit, "port", "general", "nof_out_tm_ports", NULL));
            printf(" Get free LOGICAL ports \n");
            break;
        case tm_ports:
            array_size = *(dnxc_data_get(unit, "port", "general", "nof_out_tm_ports", NULL));
            printf(" Get free TM ports \n");
            break;
        case port_base_q_pairs:
            array_size = *(dnxc_data_get(unit, "egr_queuing", "params", "nof_q_pairs", NULL));
            printf(" Get free Base Q_Pairs \n");
            break;
        case port_base_hrs:
            array_size = *(dnxc_data_get(unit, "sch", "flow", "nof_hr", NULL));
            printf(" Get free Base HRs \n");
            break;
        case nif_phy_lanes:
            array_size = *(dnxc_data_get(unit, "nif", "phys", "nof_phys", NULL));
            printf(" Get free NIF PHYs \n");
            break;
        default:
            printf("Invalid resource type required %d \n", res);
            return  BCM_E_PARAM;
    }

    int occupied_resource_array[array_size];

    /**
     * Get device port bit maps
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &port_config), "");

    if (free_resource_type_requested == nif_phy_lanes) {
        BCM_PBMP_ASSIGN(pbmp , port_config.port);
    } else {
        BCM_PBMP_ASSIGN(pbmp , port_config.all);
        BCM_PBMP_REMOVE(pbmp, port_config.sfi);
    }

    /**
     * clear occupied array
     */
    for (i = 0; i < array_size; i++) {
        occupied_resource_array[i] = 0;
    }


    /**
     * Go over all TM ports
     */
    int resource_index;
    resource_index = 0;
    int prio = 0;
    BCM_PBMP_ITER(pbmp, logical_port) {

        BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info), "");
        switch (free_resource_type_requested) {
            case logical_ports:
                occupied_resource_array[logical_port] = 1;
                break;
            case tm_ports:
                if(mapping_info.tm_port == -1)
                {
                    continue;
                }
                occupied_resource_array[mapping_info.tm_port] =  1;
                break;
            case port_base_q_pairs:
                /** egress q_pairs have special allocation logic for octets.
                 *  mark the whole octet as occupied to avoid failures due to an attempt to make a bad allocation with ID that breaks those rules */
                int octet_base = mapping_info.base_q_pair / 8 * 8;
                for(prio = 0; prio < 8; ++prio)
                {
                    if(occupied_resource_array[octet_base + prio] == 0)
                    {
                        occupied_resource_array[octet_base + prio] =  1;
                        resource_index++;
                    }
                }
                break;
            case port_base_hrs:
                if(mapping_info.base_hr == -1)
                {
                    continue;
                }
                /** base_hrs have special allocation logic for octets.
                 *  mark the whole octet as occupied to avoid failures due to an attempt to make a bad allocation with ID that breaks those rules */
                int octet_base = mapping_info.base_hr / 8 * 8;
                for(prio = 0; prio < 8; ++prio)
                {
                    if(occupied_resource_array[octet_base + prio] == 0)
                    {
                        occupied_resource_array[octet_base + prio] =  1;
                        resource_index++;
                    }
                }
                break;
            case nif_phy_lanes:
                if (flags & BCM_PORT_ADD_ILKN_OVER_FABRIC_PORT) {
                    continue;
                }

                BCM_PBMP_ITER(interface_info.phy_pbmp, phy) {
                    occupied_resource_array[phy] = 1;
                }
                break;
            default:
                printf("Invalid resource type required %d \n", free_resource_type_requested);
                return  BCM_E_PARAM;
        }

    }

    /**
     * There is a reserved TM port
     */
    if (free_resource_type_requested == tm_ports) {
        uint32 reserved_tm_port = *(dnxc_data_get(unit, "port", "general", "reserved_pp_dsp", NULL));
        if (reserved_tm_port != -1) {
           occupied_resource_array[reserved_tm_port] = 1;
        }
    }

    /** check if there are reserved egress queues */
    if(free_resource_type_requested == port_base_q_pairs)
    {
        if(*(dnxc_data_get(unit, "egr_queuing_v1", "general", "v1_supported", NULL)) && *(dnxc_data_get(unit, "egr_queuing_v1", "egress_queues", "rcy_reserved_queues", NULL)))
        {
            uint32 reserved_egress_queues = *(dnxc_data_get(unit, "egr_queuing_v1", "egress_queues", "rcy_reserved_queue", NULL));
            for (i = reserved_egress_queues; i < array_size; i++)
            {
                occupied_resource_array[i] = 1;
            }
        }
    }

    /**
     * Some NIF lanes may not be supported for SKUs
     * Set the unsupported lanes as occupied to remove from free resource list
     */
    if (free_resource_type_requested == nif_phy_lanes)
    {
        supported_phys_pbmp = (void *)(dnxc_data_1d_get(unit, "nif", "phys", "general", "supported_phys", 0));
        for (resource = 0; resource < array_size; resource++)
        {
            if (!BCM_PBMP_MEMBER(*supported_phys_pbmp, resource))
            {
                occupied_resource_array[resource] = 1;
            }
        }
    }

    int nof_occupied_resources;
    nof_occupied_resources = resource_index;
    /**
     * Go over all possible resources
     * If not found in the list of occupied resources THAN
     * add to the list of free resources
     */
    free_resource_index = 0;
    for(resource = 0; resource < array_size; resource++) {
        if (occupied_resource_array[resource] == 0) {
            free_resources_array[free_resource_index] = resource;
            free_resource_index++;

        }

    }
    *nof_free_resources = free_resource_index;

    return BCM_E_NONE;
}

/**
 * Purpose: To check if port is nif and supports dynamic add / remove.
 * The exceptions that this function is checking are:
 *   - NIF ports STIP and ELK are not supporting dynamic add / remove.
 *   - Ryc mirror port does support dynamic add / remove. However, the add / remove sequences that
 *     were implemented in cint_dyn_port_add_port_full_example and cint_dyn_port_remove_port_full_example
 *     are not covering ryc-mirror port.
 */
int is_port_nif_dynamic (
        int unit,
        bcm_port_t port,
        bcm_port_if_t interface_type,
        uint32 flags,
        int first_phy)
{

    int port_is_nif_dynamic = 0;

    if (interface_type == BCM_PORT_IF_ILKN)
    {
        if (flags & BCM_PORT_ADD_KBP_PORT)
        {
            port_is_nif_dynamic = 0;
        }
        else
        {
            port_is_nif_dynamic = 1;
        }
    }
    else if ((interface_type == BCM_PORT_IF_XFI) || (interface_type == BCM_PORT_IF_XLAUI) || (interface_type == BCM_PORT_IF_XLAUI2)
        || (interface_type == BCM_PORT_IF_CAUI) || (interface_type == BCM_PORT_IF_NIF_ETH))
    {
        if (flags & (BCM_PORT_ADD_STIF_PORT | BCM_PORT_ADD_STIF_AND_DATA_PORT))
        {
            port_is_nif_dynamic = 0;
        }
        else
        {
            port_is_nif_dynamic = 1;
            
            if(*(dnxc_data_get(unit, "nif", "global", "mgmt_port_supported", NULL)))
            {
                if(first_phy >= *(dnxc_data_get(unit, "nif", "phys", "first_mgmt_phy", NULL)))
                {
                    port_is_nif_dynamic = 0;
                }

            }

        }
    }
    else if (interface_type == BCM_PORT_IF_OTN)
    {
        port_is_nif_dynamic = 1;
    }
    else
    {
        port_is_nif_dynamic = 0;
    }
    return port_is_nif_dynamic;
}

/**
 * Purpose: To check if port is flexe client and supports dynamic add / remove.
 */
int is_port_flexe_dynamic (
        int unit,
        bcm_port_t port,
        bcm_port_if_t interface_type)
{

    int port_is_flexe_dynamic = 0;

    if ((interface_type == BCM_PORT_IF_FLEXE_CLIENT) || (interface_type == BCM_PORT_IF_VIRTUAL_FLEXE_CLIENT))
    {
        port_is_flexe_dynamic = 1;
    }
    else
    {
        port_is_flexe_dynamic = 0;
    }
    return port_is_flexe_dynamic;
}

/**
 * Purpose: To check if port is Recycle and supports dynamic add / remove.
 */
int is_port_rcy_dynamic (
        int unit,
        bcm_port_t port,
        bcm_port_if_t interface_type)
{

    int port_is_rcy_dynamic = 0;

    if (interface_type == BCM_PORT_IF_RCY)
    {
        port_is_rcy_dynamic = 1;
    }
    else
    {
        port_is_rcy_dynamic = 0;
    }
    return port_is_rcy_dynamic;
}
/**
 * Purpose: To check if port requires scheduler.
 */
int is_port_scheduler_required(
        int unit,
        uint32 flags,
        bcm_port_interface_info_t interface_info)
{
    int port_is_scheduler_required = 0;
    int sar_is_egress_bypass_port = 0;

    if (flags & (BCM_PORT_ADD_FLEXE_SAR | BCM_PORT_ADD_OTN_SAR))
    {
        if (*(dnxc_data_get(unit, "nif", "sar_client", "is_tdm_port", NULL)))
        {
            if (*(dnxc_data_get(unit, "tdm", "params", "egress_bypass_enable", NULL)))
            {
                sar_is_egress_bypass_port = 1;
            }
            else
            {
                sar_is_egress_bypass_port = 0;
            }
        }
        else
        {
            sar_is_egress_bypass_port = 1;
        }
    }
    if (!(flags & BCM_PORT_ADD_FLEXE_PHY) &&
        !(flags & BCM_PORT_ADD_CROSS_CONNECT) &&
        !(flags & BCM_PORT_ADD_STAT_ONLY) &&
        !(interface_info.flags & BCM_PORT_ADD_FLEXE_PROTECTION) &&
        !(interface_info.flags & BCM_PORT_ADD_TX_FEEDBACK_ONLY) &&
        (interface_info.interface != BCM_PORT_IF_FLEXE_CLIENT) &&
        (interface_info.interface != BCM_PORT_IF_OTN) && !sar_is_egress_bypass_port &&
        !((interface_info.interface == BCM_PORT_IF_NIF_ETH) && (flags & BCM_PORT_ADD_IF_FRAMER_MGMT)))
    {
        port_is_scheduler_required = 1;
    }

    return port_is_scheduler_required;
}

/**
 * \brief - Get pp port from logical port
 *
 * \param [in] unit - Relevant unit
 * \param [in] logical_port - Logical port
 * \param [out] pp_port - PP port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int pp_port_from_logical_get (
        int unit,
        bcm_port_t logical_port,
        uint32* pp_port)
{
    uint32 flags;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info), "when getting pp port");

    *pp_port = mapping_info.pp_port;
    return BCM_E_NONE;
}
