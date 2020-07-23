/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_port_packet_processing_example.c
 * Purpose:     Example of adding a port and applying pp properties to it
 *
 * Example includes:
 *      Adding one port configuration including port header type configuration, applying packet processing properties
 *
 * Usage Example:
    cd ../../../../regress/bcm
    c
    cint_reset();
    exit;
    c ../../src/examples/dnx/port/cint_dynamic_port_add_remove.c
    c ../../src/examples/dnx/port/cint_port_packet_processing_example.c
 
    c
    print "add port 18";
    int unit;
    bcm_port_t port;
    int rate_mhz;
    int fec_type;
    unit = 0;
    port = 18;
    bcm_pbmp_t phy_pbmp;
    BCM_PBMP_CLEAR(phy_pbmp);
    BCM_PBMP_PORT_SET(phy_pbmp,port);
    rate_mhz = 10000;
    fec_type = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    int port_header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    int port_header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    print cint_port_packet_processing_add_eth_port(unit,port,phy_pbmp,rate_mhz,fec_type,port_header_type_in,port_header_type_out);
 
    c
    print "Set port header type";
    int unit;
    bcm_port_t port;
    int port_header_type_in;
    int port_header_type_out;
    unit = 0;
    port = 18;
    port_header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    port_header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    print cint_port_packet_processing_header_type_set(unit,port,port_header_type_in,port_header_type_out);
    exit;
 
    c
    print "Set packet processing propeties to removed trunk member"
    int unit;
    bcm_port_t port;
    bcm_vlan_t vlan;
    bcm_gport_t vlan_port_id; 
    unit = 0;
    port = 18;
    vlan = 5;
    print cint_port_packet_processing_removed_trunk_member_pp_properties_set(unit,port,vlan,&vlan_port_id);
    exit;
 
    c
    print ""
    int unit;
    bcm_port_t port;
    bcm_vlan_t vlan;
    unit = 0;
    port = 18;
    vlan = 5;
    print cint_port_packet_processing_pp_properties_unset(unit,port,vlan);
    exit;

 */

/**
 * \brief - Set incoming and outgoing port header types
 *
 * \param [in] unit - Unit number
 * \param [in] port - Logical port
 * \param [in] header_type_in - Incoming port header type according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 * \param [in] header_type_out - Outgoing port header type according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_port_packet_processing_header_type_set(
    int unit,
    bcm_port_t port,
    int header_type_in,
    int header_type_out)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    int rv = BCM_E_NONE;

    /** Set incoming pot header type */
    key.type = bcmSwitchPortHeaderType;
    /** 1 means direction IN */
    key.index = 1;
    value.value = header_type_in;
    rv = bcm_switch_control_indexed_port_set(unit, port, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_indexed_port_set when setting incoming header type\n");
        return rv;
    }

    /** Set outgoing port header type */
    key.type = bcmSwitchPortHeaderType;
    /** 2 means direction OUT */
    key.index = 2;   
    value.value = header_type_out;
    rv = bcm_switch_control_indexed_port_set(unit, port, key, value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_indexed_port_set when setting outgoing header type\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - Configure pp properties to removed trunk member
 * Configure vlan domain, set VLAN-membership-namespace, set vlan port membership,
 * configure vlan for untagged cases, configure in_lif same interface
 * enable learning, set port default-LIF to simple bridge and set stp state to FORWARD,
 * Create vlan port with match criteria the port itself 
 *  
 * \param [in] unit - Unit ID
 * \param [in] port - Logical port
 * \param [in] vlan - Vlan ID 
 * \param [out] vlan_port_id - Vlan port ID  
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_port_packet_processing_removed_trunk_member_pp_properties_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vlan,
    bcm_gport_t  *vlan_port_id)
{
    uint32 class_id;
    uint32 flags;
    bcm_vlan_port_t vlan_port;
    int rv = BCM_E_NONE;

    flags = BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD;
    class_id = port;

    /** Configure vlan domain per port*/
    rv = bcm_port_class_set(unit, port, bcmPortClassId, class_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error, in bcm_port_class_set\n");
      return rv;
    }

    /** Set VLAN-membership-namespace */
    rv = bcm_port_class_set(unit, port, bcmPortClassVlanMember, class_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error, in bcm_port_class_set\n");
      return rv;
    }

    /** Set vlan port membership */
    rv = bcm_vlan_gport_add(unit, BCM_VLAN_DEFAULT, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    /** Configure inlif same interface*/
    rv = bcm_port_control_set(unit, port, bcmPortControlBridge, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    /** Configure vlan for untag cases*/
    rv = bcm_port_untagged_vlan_set(unit, port, BCM_VLAN_DEFAULT);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    /** Enable learning */
    rv = bcm_port_learn_set(unit, port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_learn_set\n");
        return rv;
    }

    /** Configure default stp state as FORWARD per ethernet port */
    rv = bcm_port_stp_set(unit, port, BCM_STG_STP_FORWARD);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_stp_set\n");
        return rv;
    }

    /** Set port default-LIF to simple bridge:  */
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressMissDrop, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_port_set\n");
        return rv;
    }

    /** Create VLAN-Port with criteria BCM_VLAN_PORT_MATCH_PORT */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = port;
    vlan_port.vsi = vlan;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, failed during bcm_vlan_port_create for port %d, vsi - %d, rv - %d\n", port, vsi, rv);
        return rv;
    }
    *vlan_port_id = vlan_port.vlan_port_id;

    return BCM_E_NONE;
}

/**
 * \brief - Remove pp properties from a port
 * Remove vlan domain, VLAN-membership-namespace, vlan port membership,
 * vlan for untagged cases. Disable in_lif same interface and learning.
 * Set port default-LIF to drop-LIF and disable stp state.
 *  
 * \param [in] unit - Unit ID
 * \param [in] port - Logical port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_port_packet_processing_pp_properties_unset(
    int unit,
    bcm_port_t port)
{
    uint32 flags;
    bcm_vlan_port_t vlan_port;
    int rv = BCM_E_NONE;

    flags = 0;

    /** Remove vlan domain */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, 0);
    if (rv != BCM_E_NONE)
    {
      printf("Error, in bcm_port_class_set\n");
      return rv;
    }

    /** Remove VLAN-membership-namespace */
    rv = bcm_port_class_set(unit, port, bcmPortClassVlanMember, 0);
    if (rv != BCM_E_NONE)
    {
      printf("Error, in bcm_port_class_set\n");
      return rv;
    }

    /** Delete vlan port membership */
    rv = bcm_vlan_gport_delete(unit, BCM_VLAN_DEFAULT, port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    /** Disable inlif same interface*/
    rv = bcm_port_control_set(unit, port, bcmPortControlBridge, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    /** Remove vlan for untag cases*/
    rv = bcm_port_untagged_vlan_set(unit, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    /** Disable learning */
    rv = bcm_port_learn_set(unit, port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_learn_set\n");
        return rv;
    }

    /** Configure stp state to DISABLE */
    rv = bcm_port_stp_set(unit, port, BCM_STG_STP_DISABLE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_stp_set\n");
        return rv;
    }

    /** Enable drop using drop-LIF */
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressMissDrop, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_port_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief   Full example to add port and set relevant shapers
 *  
 * stage 1: Add new port
 * stage 2: Configure NIF
 * stage 3: configure egr and sch shapers
 * stage 4: Configure PP properties
 * stage 5: configure application reference
 * stage 6: enable the port 
 *  
 * \param [in] unit  - Unit number
 * \param [in] port  - Logical port to add
 * \param [in] mapping_info - Logical port mapping info
 * \param [in] interface_info  - physical port info
 * \param [in] resource - NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 * \param [in] dfe_mode - dfe mode: on, low power or off
 * \param [in] medium_type - medium type: backplane, copper cable or optic
 * \param [in] header_type_in  - incomming header type
 * \param [in] header_type_out - outgoing header type
 * \param [in] flags  - flags for bcm_port_add API (BCM_PORT_ADD_XX_PORT)
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
cint_port_packet_processing_add_port_full_example(
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

    return cint_port_packet_processing_add_ports_full_example(unit, 1, &port, &mapping_info, &interface_info, &resource, &dfe_mode, 
                                                &medium_type, &header_type_in, &header_type_out, &flags);
}

/**
 * \brief
 *      Full example to add ports and set relevant shapers
 *      This example is used to configure port resouces for multi ports
 *      simultaneously using API bcm_port_resource_multi_set. This example
 *      can provide better port VCO assignment when configuring multi ports
 *      on one PM.
 * stage 1: Add new ports
 * stage 2: Configure NIF resources
 * stage 3: Configure egr and sch shapers
 * stage 4: Configure switch port header types for both directions
 * stage 5: Configure application reference
 * stage 6: Enable the ports 
 *  
 * \param [in] unit - Unit number.
 * \param [in] nof_ports -  Number of ports
 * \param [in] port_list  -  Ports array
 * \param [in] mapping_info - Logical port mapping info
 * \param [in] interface_info - physical port info
 * \param [in] resource - NIF resource (speed, link training and FEC.
 *                              All other struct fields, including FW control word - are ignored
 * \param [in] dfe_mode - dfe mode: on, low power or off
 * \param [in] medium_type - medium type: backplane, copper cable or optic
 * \param [in] header_type_in - incomming header type
 * \param [in] header_type_out - outgoing header type
 * \param [in] flags - flags for bcm_port_add API
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

int
cint_port_packet_processing_add_ports_full_example(
    int unit,
    int nof_ports,
    bcm_port_t *port_list,
    bcm_port_mapping_info_t *mapping_info,
    bcm_port_interface_info_t *interface_info,
    bcm_port_resource_t *resource,
    dfe_mode_t *dfe_mode,
    medium_type_t *medium_type,
    int *header_type_in,
    int *header_type_out,
    uint32 *flags)
{
    int port_total_rate_mhz, nof_lanes;
    int rv = BCM_E_NONE;
    int nof_ports_resource_set = 0;
    bcm_port_resource_t resource_set_arr[nof_ports];
    bcm_gport_t egress_q_gport;
    bcm_pbmp_t phys_bmp;
    bcm_port_phy_tx_t tx_taps;
    int commit_changes = 1, port_index;
    int is_remove = 0;
    int port_is_nif_dynamic;
    int interface_has_single_channel;
    bcm_port_interface_info_t interface_info_tmp;
    bcm_port_mapping_info_t mapping_info_tmp;
    uint32 flags_tmp = 0;

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {

         /** Input argument validation: This cint is relevant only for NIF ports (excluding ELK and STIF) */
         port_is_nif_dynamic = is_port_nif_dynamic (unit, port_list[port_index], interface_info[port_index].interface, flags[port_index]);
         if (!port_is_nif_dynamic)
         {
             printf("Error, cint_port_packet_processing_add_ports_full_example supports only NIF ports (excluding ELK / STIF). port=%d\n", port_list[port_index]);
             return BCM_E_PORT;
         }

         /** stage 1 - adding the port */
        rv = bcm_port_add(unit, port_list[port_index], flags[port_index],
                          &interface_info[port_index], &mapping_info[port_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_add\n");
            return rv;
        }

        rv = cint_dyn_port_interface_has_single_channel(unit, port_list[port_index], &interface_has_single_channel);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_dyn_port_interface_has_single_channel\n");
            return rv;
        }

        /** stage 2 - configure NIF properties */
        /** Set resource  - this should be done only for first port on interface*/
        if (interface_has_single_channel)
        {
            sal_memcpy(&resource_set_arr[nof_ports_resource_set], &resource[port_index], sizeof(resource_set_arr[0]));
            rv = cint_dyn_port_resource_fill(unit, port_list[port_index],
                                             dfe_mode[port_index], medium_type[port_index], &resource_set_arr[nof_ports_resource_set]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, cint_dyn_port_resource_fill,unit $unit logical_port: %x, $rv\n", port_list[port_index]);
                return rv;
            }
            if (tvco_prioritize_26p562g_flag == 1) {
                resource_set_arr[nof_ports_resource_set].flags = BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G;
            }

            nof_ports_resource_set++;
        }
    }

    rv = bcm_port_resource_multi_set(unit, nof_ports_resource_set, resource_set_arr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_resource_multi_set, unit=%d", unit );
        return rv;
    }


    for (port_index = 0; port_index < nof_ports_resource_set; ++port_index)
    {
        /** If no link training set tx taps */
        if (resource_set_arr[port_index].link_training == 0)
        {
            /** For NRZ - default Tx taps are OK.Only in PAM4 need to set Tx taps for the Ixia 400G setup */
            rv = bcm_port_get(unit, resource_set_arr[port_index].port, &flags_tmp, &interface_info_tmp, &mapping_info_tmp);
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
                rv = bcm_port_phy_tx_set(unit, resource_set_arr[port_index].port, &tx_taps);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_port_phy_tx_set, unit=%d, port=%d\n", unit, resource_set_arr[port_index].port);
                    return rv;
                }

                /** ER channels need to enable TX precoding */
                if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource_set_arr[port_index].phy_lane_config))
                {
                    rv = bcm_port_phy_control_set(unit, resource_set_arr[port_index].port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, 1);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error, bcm_port_phy_control_set, unit=%d, port=%d\n", unit, resource_set_arr[port_index].port);
                        return rv;
                    }

                    rv = bcm_port_phy_control_set(unit, resource_set_arr[port_index].port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, 1);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error, bcm_port_phy_control_set, unit=%d, port=%d\n", unit, resource_set_arr[port_index].port);
                        return rv;
                    }
                }
            }
        }
    }

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        /** stage 3 - configure egr and sch shapers */
        rv = dnx_algo_port_interface_rate_get(unit, port_list[port_index], DNX_ALGO_PORT_SPEED_F_TX, &port_total_rate_mhz);
        if (rv != BCM_E_NONE)
        {
            printf("Error, dnx_algo_port_interface_rate_get, unit=%d, port=%d\n", unit, port_list[port_index]);
            return rv;
        }

        if (!(flags[port_index] & BCM_PORT_ADD_FLEXE_PHY))
        {
            /** No need for speedup, as it s done inside dnx_tune_egr_ofp_rate_set */
            /** egr expects speed in Kbits/sec */
            commit_changes = (port_index == nof_ports - 1) ? 1 : 0;
            rv = dnx_tune_egr_ofp_rate_set(unit, port_list[port_index], port_total_rate_mhz * 1000, commit_changes, is_remove);
            if (rv != BCM_E_NONE)
            {
                printf("Error, dnx_tune_egr_ofp_rate_set, unit=%d, port=%d\n", unit, port_list[port_index]);
                return rv;
            }
            /** No need for speedup, as it s done inside dnx_tune_scheduler_port_rates_set */
            /** sch expects speed in Kbits/sec */
            rv = dnx_tune_scheduler_port_rates_set(unit, port_list[port_index], 0, port_total_rate_mhz * 1000, commit_changes, is_remove);
            if (rv != BCM_E_NONE)
            {
                printf("Error, dnx_tune_scheduler_port_rates_set, unit=%d, port=%d\n", unit, port_list[port_index]);
                return rv;
            }
            
            /** stage 4 - PP properties - setting only incoming and outgoing port header type*/
            rv = cint_port_packet_processing_header_type_set(unit, port_list[port_index], header_type_in[port_index],
                                                                                    header_type_out[port_index]);
            if (rv != BCM_E_NONE)
           {
               printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port_list[port_index]);
               return rv;
            }
            
        }

        /** stage 5 - application reference */
        rv = appl_dnx_dynamic_port_create(unit, port_list[port_index]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port_list[port_index]);
            return rv;
        }

        rv = cint_dyn_port_interface_has_single_channel(unit, port_list[port_index], &interface_has_single_channel);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_dyn_port_interface_has_single_channel\n");
            return rv;
        }

        /** stage 6 - enable the port - this should be done only for first port on interface */
        if (interface_has_single_channel)
        {
            rv = bcm_port_enable_set(unit,  port_list[port_index], 1);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_dnx_port_enable_set: unit=%d, port=%d\n", unit, port_list[port_index]);
                return rv;
            }
        }
    }

    return BCM_E_NONE;

}


/** 
 * \brief - The below cint function is an example for adding normal eth port
 *  Port description:
 *  - Logical port, phys and rate - user decision (input arguments)
 *  - SerDes configuration to suite simple loopback or p2p cable connected port:
 *     - Link training: 'on'
 *     - DFE: 'on'
 *     - medium type: 'copper'
 *  - NIF interface: ETH
 *  - Header type (in and out): user decision
 *  - FEC: default according to speed
 *  - TM port: same id as the logical port
 *  - Priorities: 2
 */
int
cint_port_packet_processing_add_eth_port(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t phy_pbmp,
    int rate_mhz,
    int fec_type,
    int header_type_in,
    int header_type_out)
{
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int first_phy;
    int nof_links_per_core;
    uint32 nof_cores, nof_links;
    bcm_port_resource_t resource;
    dfe_mode_t dfe_mode;
    medium_type_t medium_type;
    uint32 flags = 0;
    int rv = 0;

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
    mapping_info.tm_port = port;
    mapping_info.num_priorities = 2;

    /** Configuring NIF resource*/
    resource.port = port;
    resource.speed = rate_mhz;
    resource.fec_type = fec_type;
    resource.link_training = 1;
    resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    /** We can't pass default inside phy_lane_config so we pass 2 of them (dfe_mode and medium type) - specifically */
    dfe_mode = dfe_on;
    medium_type = copper_cable;

    /** Add the port */
    rv = cint_port_packet_processing_add_port_full_example(unit, port, mapping_info, interface_info, resource,
                                             dfe_mode, medium_type, header_type_in, header_type_out, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_port_packet_processing_add_port_full_example, unit: %d, logical_port: %d\n", unit, port);
        return rv;
    }

    return BCM_E_NONE;
}

