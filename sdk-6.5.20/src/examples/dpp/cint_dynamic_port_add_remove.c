/*~~~~~~~~~~~~~~~~~~~~~~~~~~NIF: Interface Configuration~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_dynamic_port_add_remove.c
 * Purpose:     Example of simple Egress transmit scheduling setup
 *
 * Example includes:
 *  o     Removing one port configuration
 *  o     Adding one port configuration
 *  o     Removing several ports and Adding several new ports configuration
 *  o     Changing number of lanes for a given port
 *
 * It is assumed diag_init is executed.
 *
 * To remove one port configuration follow following steps:
 *
 * 1) detach port
 * 2) remove port configuration
 * 3) add new port configuration
 * 4) probe new port
 *
 * NOTE: If you detach first lane port that was previously initialized and do not probe that port,
 *       the speeds of the following lanes may be effected unexpectedly.
 *
 * Usage Example:
 * The example below will remove port 100 and add Ethernet XFI port on first physical lane 20 with port id 101.
 *
 * int rv;
 * bcm_port_mapping_info_t mapping_info;
 * bcm_port_interface_info_t interface_info;
 *
 *
 * rv = remove_port_full_example(unit, 100, 0);
 * if (rv != BCM_E_NONE) {
 *     printf("Error, in remove_port_full_example\n");
 *     return rv;
 * }
 *
 * mapping_info.channel = 0;
 * mapping_info.core = 0;
 * mapping_info.tm_port = 100;
 * mapping_info.num_priorities = 8;
 * interface_info.phy_port = 20;
 * interface_info.interface = BCM_PORT_IF_XFI;
 *
 * rv = add_port_full_example(unit, 101 , mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_TM ,FALSE);
 * if (rv != BCM_E_NONE) {
 *     printf("Error, in add_port_full_example\n");
 *     return rv;
 * }
 */

int random_lane_num;
int serdes_allocation_temp[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int serdes_allocation_final[12];

 /*
 * Function:
 *      remove_port
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
 *
 * Assumes port is valid and initialized
 *
 * 1) detach given port
 * 2) remove port configuration
 */
int remove_port(int unit, int port, uint32 flags){
    int rv;

    /*Disable rx los application*/
    /*Remove ports from rx_los application - do not check error for case rx los application is not used*/
    rx_los_port_enable(unit, port, 0, 0);


    /* remove port configuration */
    rv = bcm_port_remove(unit, port, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_remove\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      add_port
 * Purpose:
 *      Reconfigure single port
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  Logical port # - must be a port which already defined by 'ucode_port' soc property.
 *	    mapping_info    - (IN)	Logical port mapping info
 *	    interface_info  - (IN)	physical port info
 *      flags           - (IN)  flags
 *      is_hg           - (IN)  Is higig port
 *      is_first_in_if  - (IN)  If port is master channel (first channel in interface) this param should be TRUE,
 *                              FALSE otherwise(another channel to an existing interface)
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 *
 * 1) remove port configuration
 * 2) add new port configuration
 * 3) probe new port
 * 4) enable rx los
 * 5) If first_packet_sw_bypass is active and port is ILKN -> set SW linkscan for this port
 */
int add_port(int unit, int port, bcm_port_mapping_info_t mapping_info, bcm_port_interface_info_t interface_info, uint32 flags, int header_type, int is_hg, int is_first_in_if){
    int rv;
    int first_packet_sw_bypass;

    /* add new port configuration */
    rv = bcm_port_add(unit, port, flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_add\n");
        return rv;
    }

    if (is_first_in_if)
    {
        /* enable port */
        rv = bcm_port_enable_set(unit, port, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_enable_set at port=%d\n", port);
            return rv;
        }
    }

    /*rx loss application*/
    /*Add ports to rx_los application - do not check error for case rx los application is not used*/
    rx_los_port_enable(unit, port, 1, 0);


    /*
     * If first_packet_sw_bypass is active, need to set SW linkscan for ILKN ports.
     */
    first_packet_sw_bypass = soc_property_get(unit, spn_ILKN_FIRST_PACKET_SW_BYPASS, 0);
    if (first_packet_sw_bypass){
        if (interface_info.interface = BCM_PORT_IF_ILKN){
            rv = bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_linkscan_mode_set at port=%d\n", port);
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}



/* Function:
 *      change_nof_lanes
 * Purpose:
 *      Reconfigure port number of lanes
 */
int change_nof_lanes(int unit, int port, int nof_lanes){
    bcm_pbmp_t pbmp, detached, okay;
    int rv, enable;

    /* save default enable mode */
    rv = bcm_port_enable_get(unit, port, &enable);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_enable_get. port=$port");
        return rv;
    }

    /* detach given port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(detached);
    BCM_PBMP_CLEAR(okay);
    BCM_PBMP_PORT_ADD(pbmp,port);

    /*Remove ports from rx_los application - do not check error for case rx los application is not used*/
    rx_los_port_enable(unit, port, 0, 0);

    rv = bcm_port_detach(unit, pbmp, &detached);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_detach. pbmp=$pbmp\n");
        return rv;
    }

    /* set number of lanes*/
    rv = bcm_port_control_set(unit, port, bcmPortControlLanes, nof_lanes);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set. type=bcmPortControlLanes, value=$nof_lanes");
        return rv;
    }

    /* probe port */
    rv = bcm_port_probe(unit, pbmp, &okay);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_probe. pbmp=$pbmp\n");
        return rv;
    }

    /* set port enable */
    rv = bcm_port_enable_set(unit, port, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_enable_get. port=$port");
        return rv;
    }

    /*Add ports to rx_los application - do not check error for case rx los application is not used*/
    rx_los_port_enable(unit, port, 1, 0);

    return BCM_E_NONE;
}

/* Function:
 *      config_ports_rate
 * Purpose:
 *      Reconfigure shapers, header_type is relevant only in case of is_port_add is TRUE
 */
int config_ports_rate(int unit, bcm_pbmp_t pbmp, int kbits_sec_max, int header_type ,int is_port_add)
{
    int                   rv;
    int                   port;
    bcm_gport_t           fap_port;
    int                   tc_i;
    bcm_cosq_gport_info_t gport_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    int                   new_interface_speed, interface_rate;
    int                   port_phy_speed, is_channelized, egq_rate, sch_rate;
    uint32                dummy1 = 0,dummy2 = 0;
    uint32                flags, local_port_speed, kbits_sec_max_get;


    sch_rate = kbits_sec_max + (kbits_sec_max / 100) * 5; /* 5% speedup */
    egq_rate = kbits_sec_max + (kbits_sec_max / 100) * 1; /* 1% speedup */

    BCM_PBMP_ITER(pbmp, port) /*configure each port in pbmp*/
    {
        gport_info.cosq=0;
        gport_info.in_gport=port;

        /* get port priorities */
        rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
            return rv;
        }

        is_channelized = (flags & BCM_PORT_ADD_CONFIG_CHANNELIZED) ? 1 : 0;

        if(mapping_info.num_priorities==8) {/*for 8 priorities ser TCG bandwidth*/
            /*EGQ TCG bandwidth*/
            rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPortTCG,gport_info);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port , gport_type %x, $rv\n", bcmCosqGportTypeLocalPortTCG);
                return rv;
            }
            rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,egq_rate,0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
                return rv;
            }
            /*sch TCG bandwidth */
            if(is_port_add && !(header_type == BCM_SWITCH_PORT_HEADER_TYPE_TDM || header_type==BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW))
            {
                rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPortTCG,gport_info);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeE2EPortTCG);
                    return rv;
                }
                rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,sch_rate,0);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
                    return rv;
                }
            }
        }
        /*EGQ TC bandwidth */
        rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPortTC,gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeLocalPortTC);
            return rv;
        }
        for (tc_i=0;tc_i<mapping_info.num_priorities;++tc_i)
        {/*for each tc set bandwidth*/
            rv = bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,tc_i,0,egq_rate,0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, tc: %d,$rv\n",gport_info.out_gport,tc_i);
                return rv;
            }
        }
        /*sch TC bandwidth */
        if(is_port_add && !(header_type == BCM_SWITCH_PORT_HEADER_TYPE_TDM || header_type==BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW))
        {
            rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPortTC,gport_info);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeE2EPortTC);
                return rv;
            }
            for (tc_i=0;tc_i<mapping_info.num_priorities;++tc_i) {
                /*for each tc set bandwidth*/
                rv = bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,tc_i,0,sch_rate,0);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, tc: %d,$rv\n",gport_info.out_gport,tc_i);
                    return rv;
                }
            }
        }

        /*set EGQ bandwidth */
        rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeLocalPort);
            return rv;
        }
        rv = bcm_cosq_gport_bandwidth_get(unit,gport_info.out_gport,0,&dummy1,&local_port_speed,&dummy2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_get,unit $unit fap gport: %x, $rv\n", fap_port);
            return rv;
        }
        /*set EGQ interface bandwidth*/
        rv = bcm_fabric_port_get(unit,gport_info.out_gport,0,&fap_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_port_get,unit $unit gport: %x, $rv\n",gport_info.out_gport);
            return rv;
        }

        /*set EGQ interface bandwidth: setting non-chanellized interface is done to set the whole calender, not to one interface*/
        /*translate to port rate*/
        rv = bcm_cosq_gport_bandwidth_get(unit,fap_port,0,&dummy1,&kbits_sec_max_get,&dummy2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_get,unit $unit fap gport: %x, $rv\n", fap_port);
            return rv;
        }


        if (is_port_add)
        {
            rv = bcm_port_speed_get(unit, port, &port_phy_speed);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_speed_set,unit $unit gport: %x, $rv\n", bcm_port_speed_get);
                return rv;
            }
            if (is_channelized) {
                if (interface_info.interface == BCM_PORT_IF_ILKN) {
                    interface_rate = port_phy_speed*interface_info.num_lanes*1000;
                } else {
                    interface_rate = port_phy_speed*1000;
                }
                if (( kbits_sec_max_get) <= 0) {
                    rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,(interface_rate /* maximal interface speed */),0);
                    if (rv != BCM_E_NONE) {
                        printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                        return rv;
                    }
                }
            }
        }
        /*set EGQ bandwidth */
        rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeLocalPort);
            return rv;
        }
        rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,egq_rate,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
            return rv;
        }

        /*set sch bandwidth*/
        if(is_port_add && !(header_type == BCM_SWITCH_PORT_HEADER_TYPE_TDM || header_type==BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW))
        {
            rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPort,gport_info);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeE2EPort);
                return rv;
            }
            rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,sch_rate,0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
                return rv;
            }
        }

        /*set sch interface bandwidth*/
        if(is_port_add && !(header_type == BCM_SWITCH_PORT_HEADER_TYPE_TDM || header_type==BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW))
        {
            rv = bcm_fabric_port_get(unit,gport_info.out_gport,0,&fap_port);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_fabric_port_get,unit $unit gport: %x, $rv\n",gport);
                return rv;
            }

            rv = bcm_port_speed_get(unit, port, &port_phy_speed);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_speed_set,unit $unit gport: %x, $rv\n", bcm_port_speed_get);
                return rv;
            }
            if (is_channelized) {
                if (interface_info.interface == BCM_PORT_IF_ILKN) {
                    interface_rate = port_phy_speed*interface_info.num_lanes*1000;
                } else {
                    interface_rate = port_phy_speed*1000;
                }
                if ((kbits_sec_max_get + sch_rate) <= (interface_rate /* maximal interface speed */)) {
                    rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,(kbits_sec_max_get + sch_rate),0);
                    if (rv != BCM_E_NONE) {
                        printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                        return rv;
                    }
                }
                else
                {
                    rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,(interface_rate /* maximal interface speed */),0);
                    if (rv != BCM_E_NONE) {
                        printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                        return rv;
                    }
                }
            }
        }
    }
    return BCM_E_NONE;
}



 /*
 * Function:
 *      remove_port_full_example
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
 *
 * Assumes port is valid and initialized
 *
 * 1) configure rates
 * 2) detach given port
 * 3) remove port configuration
 */
int remove_port_full_example(int unit, int port, uint32 flags)
{
    int                   rv = BCM_E_NONE;
    bcm_pbmp_t 			  pbmp_remove;

    BCM_PBMP_CLEAR(pbmp_remove);
    BCM_PBMP_PORT_ADD(pbmp_remove,port);

    /* stage 1, set rates to 0 */
    rv = config_ports_rate(unit,pbmp_remove,0,0,FALSE); /*Before removing the port: set all the ports to be deleted rates to zero */
    if (rv != BCM_E_NONE) {
        printf("Error, in config_ports_rate\n");
        return rv;
    }

    /* stage 2, detach and remove port configuration */
    rv = remove_port(unit, port, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in remove_port\n");
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      add_port_full_example
 * Purpose:
 *      Full example to add port and set relevant shapers
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  Logical port to add.
 *      flags           - (IN)
 *      mapping_info    - (IN)	Logical port mapping info
 *      interface_info  - (IN)	physical port info
 *      is_first_in_if  - (IN)  If port is master channel (first channel in interface) this param should be TRUE,
 *                              FALSE otherwise(another channel to an existing interface)
 * Returns:
 *      BCM_E_NONE     No Error
 *      BCM_E_XXX      Error occurred
 *
 * stage 1: Add  new port configuration
 * stage 2: Set port rate
 */

int
add_port_full_example(int unit, bcm_port_t port ,bcm_port_mapping_info_t mapping_info,  bcm_port_interface_info_t interface_info, int flags, int header_type ,int is_hg ,int is_first_in_if)
{
    bcm_pbmp_t            pbmp_add;
    int                   max_rate_to_set, nof_lanes;
    int                   rv = BCM_E_NONE;
    bcm_gport_t           egress_q_gport;

    BCM_PBMP_CLEAR(pbmp_add);
    BCM_PBMP_PORT_ADD(pbmp_add,port);

    /* stage 1 */
    rv = add_port(unit, port, mapping_info, interface_info, flags, header_type, is_hg, is_first_in_if);
    if (rv != BCM_E_NONE) {
        printf("Error, add_port\n");
        return rv;
    }

    switch (interface_info.interface)
    {
        case BCM_PORT_IF_SGMII:/*1G*/
            max_rate_to_set=1*1000000;
            break;
       case BCM_PORT_IF_QSGMII:/*1G*/
            max_rate_to_set=1*1000000;
            break;
        case BCM_PORT_IF_CAUI:/*100G*/
            max_rate_to_set=100*1000000;
            break;
        case BCM_PORT_IF_RXAUI:/*10G*/
            max_rate_to_set=10*1000000;
            break;
        case BCM_PORT_IF_XFI:/*10G*/
            max_rate_to_set=10*1000000;
            break;
        case BCM_PORT_IF_XLAUI:/*40G*/
            max_rate_to_set=40*1000000;
            break;
        case BCM_PORT_IF_ILKN:/*nof_lanes*10G*/
            rv = bcm_port_control_get(unit, port, bcmPortControlLanes, nof_lanes);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_control_set. type=bcmPortControlLanes, value=$nof_lanes");
                return rv;
            }
            max_rate_to_set=nof_lanes*10*1000000;
            break;
        case BCM_PORT_IF_XAUI:/*10G*/
            max_rate_to_set=10*1000000;
            break;
        case BCM_PORT_IF_CPU:/*1G*/
            max_rate_to_set=1*1000000;
            break;
        default:
            printf("Unsupported interface: $interface_info.interface\n");
            return BCM_E_PARAM;
            break;
    }

    /* stage 2 - rates*/
    rv = config_ports_rate(unit,pbmp_add,max_rate_to_set,header_type,TRUE);/*After reconfiguring the port: set the added port rate to its speed*/
    if (rv != BCM_E_NONE) {
        printf("Error, config_ports_rate, rate: $max_rate_to_set\n");
        return rv;
    }

    /* stage 3 compensation */
    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(egress_q_gport, port);
    rv = bcm_cosq_control_set(unit, egress_q_gport, 0, bcmCosqControlPacketLengthAdjust, -2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_control_set: bcmCosqControlPacketLengthAdjust\n");
        return rv;
    }

    return BCM_E_NONE;
}
int add_eth_port(int unit, bcm_port_t port ,bcm_port_t phy_port) {
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = port;
    mapping_info.num_priorities = 2;
    interface_info.phy_port = phy_port;
    interface_info.interface = BCM_PORT_IF_XFI;
    interface_info.num_lanes = 1;
    interface_info.interface_id = 0;
    BCM_PBMP_CLEAR(interface_info.phy_pbmp);
    print add_port_full_example(unit, port, mapping_info, interface_info, 0, BCM_SWITCH_PORT_HEADER_TYPE_ETH, 0, TRUE);
}

/* select the serdes lane randomly */
/* serdes_allocation_final is the final selected serdes array and the length is random_lane_num */
int select_random_serdes(int unit) {
    int count = 12;
    int random_lane_index;
    int i, j, k;

    random_lane_num = sal_rand()%12;
    if (random_lane_num == 0) {
        random_lane_num = 1;
    }
    printf("The random_lane_num %d is \n", random_lane_num);

    for (i = 0; i < random_lane_num; i++) {
        random_lane_index = sal_rand()%count;
        printf("The random_lane_index %d is selected\n", random_lane_index);

        serdes_allocation_final[i] = serdes_allocation_temp[random_lane_index];
        for(j = random_lane_index; j < count - 1; j++) {
            serdes_allocation_temp[j] = serdes_allocation_temp[j+1];
        }

        count--;
    }

    for (k = 0;k < random_lane_num; k++) {
        printf("Each serdes in serdes_allocation_final is %d\n", serdes_allocation_final[k]);
    }

    return BCM_E_NONE;
}

/* add the channelized ILKN port */
int add_channelized_ilkn_port(int unit, int core_id, int phy_id, int local_port, int tm_port, int interface_id, int channel_num, int num_priorities, int num_lanes, int is_phy_pbmp)
{
    int rv = 0;
    int i, flag = 0, logical_port;
    int is_first = 1;
    bcm_port_mapping_info_t   mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_pbmp_t       pbmp_add;
    bcm_pbmp_t       pbmp;
    int              max_rate_to_set;

    BCM_PBMP_CLEAR(pbmp);
    for (i = 0; i < num_lanes; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, serdes_allocation_final[i]);
    }

    for(i = 0; i <= (channel_num - 1); i++) {
        logical_port = local_port + i;

        mapping_info.channel = i;
        mapping_info.core = core_id;
        mapping_info.num_priorities = num_priorities;
        mapping_info.tm_port = logical_port;

        interface_info.interface = BCM_PORT_IF_ILKN;
        interface_info.interface_id = interface_id;

        if (is_phy_pbmp) {
            interface_info.phy_pbmp = pbmp;
            flag = BCM_PORT_ADD_CONFIG_CHANNELIZED | BCM_PORT_ADD_USE_PHY_PBMP;
        } else {
            interface_info.phy_port = phy_id;
            interface_info.num_lanes = num_lanes;
            flag = BCM_PORT_ADD_CONFIG_CHANNELIZED;
        }

        rv = add_port_full_example(unit,logical_port, mapping_info, interface_info, flag, BCM_SWITCH_PORT_HEADER_TYPE_ETH, 0, is_first);
        if (rv != BCM_E_NONE) {
            printf("Error, add_port_full_example,unit $unit logical_port: %x, $rv\n", logical_port);
            return rv;
        }

        /* stage 2 - rates*/
        BCM_PBMP_CLEAR(pbmp_add);
        BCM_PBMP_PORT_ADD(pbmp_add,logical_port);
        max_rate_to_set = (is_first ? 125*100000: 1*1000000);

        rv = config_ports_rate(unit,pbmp_add,max_rate_to_set,BCM_SWITCH_PORT_HEADER_TYPE_ETH,TRUE);/*After reconfiguring the port: set the added port rate to its speed*/
        if (rv != BCM_E_NONE) {
            printf("Error, config_ports_rate, rate: $max_rate_to_set\n");
            return rv;
        }
        is_first = 0;
    }

    return rv;
}

/* delete all nif ports */
int
cint_dyn_port_remove_all_dpp(
    int unit)
{
    int rv = 0;
    int port, flag = 0;
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_config_get\n");
        return rv;
    }

    BCM_PBMP_ITER(config.nif, port)
    {
        rv = remove_port_full_example(unit, port, flag);
        if (rv)
        {
            printf("remove port fail port=%d \n", port);
            return rv;
        }
    }

    return rv;
}
