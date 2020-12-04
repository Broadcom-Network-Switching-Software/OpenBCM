/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_protocol_traps.c
 * Purpose: Shows an example for configuration of protocol traps.
 *
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_protocol_traps.c
 *
 * Main Function:
 *      cint_rx_trap_protocol_traps_main(unit,port,trap_profile,protocol_type,trap_args,trap_gport,is_icmpv6);
 * Destroy Function:
 *      cint_rx_trap_protocol_traps_destroy(unit,protocol_type,trap_profile,trap_args);
 *
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_ingress_traps.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_protocol_traps.c
 * cint
 * bcm_gport_t action_gport;
 * cint_rx_trap_ingress_traps_main(0,0,bcmRxTrapUserDefine,NULL);
 * BCM_GPORT_TRAP_SET(action_gport, cint_rx_trap_ingress_traps_trap_id, 7, 0);
 * cint_rx_trap_protocol_traps_main(0,201,2,bcmRxTrapL2cpPeer,0x34,action_gport,0);
 */

bcm_rx_trap_protocol_key_t cint_rx_trap_protocol_traps_key = { 0 };
bcm_rx_trap_protocol_profiles_t cint_rx_trap_protocol_traps_protocol_profiles = { 0 };

/**
* \brief
*  Function that sets-up the desired protocol trap to be ready for use.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] port           - In-PP-Port
*   \param [in] intf           - LIF interface
*   \param [in] trap_profile   - Profile of the protocol trap(0-3)
*   \param [in] protocol_type  - protocol type
*   \param [in] protocol_args  - protocol arguments
*   \param [in] action_gport   - action gport encoding Trap-Action-Profile
*   \param [in] is_icmpv6      - relevant for ICMP protocol only, indicates if ICMPv6 or ICMPv4 type
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_protocol_traps_main(
    int unit,
    bcm_gport_t port,
    bcm_if_t intf,
    int trap_profile,
    bcm_rx_trap_t protocol_type,
    int trap_args,
    bcm_gport_t trap_gport,
	int is_icmpv6)
{
    int rv = BCM_E_NONE;
    cint_rx_trap_protocol_traps_key.protocol_trap_profile = trap_profile;
    cint_rx_trap_protocol_traps_key.protocol_type = protocol_type;

    rv = cint_rx_trap_protocol_traps_args_and_profiles_fill(unit, protocol_type, trap_profile, trap_args, is_icmpv6);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_protocol_traps_args_and_profiles_fill\n");
        return rv;
    }

    if(port != 0)
    {
        rv = bcm_rx_trap_protocol_profiles_set(unit, port, &cint_rx_trap_protocol_traps_protocol_profiles);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_protocol_profiles_set\n");
            return rv;
        }
    }
    else if(intf != 0)
    {
        rv = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &cint_rx_trap_protocol_traps_protocol_profiles);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_protocol_interface_profiles_set\n");
            return rv;
        }
    }

    rv = bcm_rx_trap_protocol_set(unit, &cint_rx_trap_protocol_traps_key, trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_protocol_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that fills configuration structures according to protocol type and profile.
* \par DIRECT INPUT:
*   \param [in] unit           - unit Id
*   \param [in] protocol_type  - protocol type
*   \param [in] trap_profile   - Profile of the protocol trap(0-3)
*   \param [in] protocol_args  - protocol arguments
*   \param [in] is_icmpv6      - relevant for ICMP protocol only, indicates if ICMPv6 or ICMPv4 type
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_protocol_traps_args_and_profiles_fill(
    int unit,
    bcm_rx_trap_t protocol_type,
    int trap_profile,
    int trap_args,
	int is_icmpv6)
{
    int rv = BCM_E_NONE;
    cint_rx_trap_protocol_traps_key.trap_args = 0;

    switch (protocol_type)
    {
        case bcmRxTrapL2cpPeer:
            /*
             * L2cp arguments varies from 0-63 according to mac 6 LSBs, example of 6 LSB = 001010
             */
            cint_rx_trap_protocol_traps_key.trap_args = trap_args;
            cint_rx_trap_protocol_traps_protocol_profiles.l2cp_trap_profile = trap_profile;
            break;
        case bcmRxTrapIcmpRedirect:
            /*
             * ICMP arguments varies from 0-255 according to ICMP types, example of type=8 echo request aka ping
             */
            cint_rx_trap_protocol_traps_key.trap_args = trap_args;
            if (is_icmpv6)
            {
            	cint_rx_trap_protocol_traps_protocol_profiles.icmpv6_trap_profile = trap_profile;
            }
            else
            {
            	cint_rx_trap_protocol_traps_protocol_profiles.icmpv4_trap_profile = trap_profile;
            }
            break;
        case bcmRxTrap8021xFail:
            /*
             * non authorized 802.1x has no arguments but its trap profile is composed of collision-resolution bit as
             * follows: * 00 - 802.1x authorized state, no trapping * 01 - 802.1x non-authorized state, Col-Res=0.
             * Other traps take precedence. * 1x - 802.1x non-authorized state, Col-Res=1. Take precedence over other
             * traps * example of trap_profile=1, other traps take precedence
             */
            cint_rx_trap_protocol_traps_protocol_profiles.non_auth_8021x_trap_profile = trap_profile;
            break;
        case bcmRxTrapIgmpMembershipQuery:
        case bcmRxTrapIgmpReportLeaveMsg:
        case bcmRxTrapIgmpUndefined:
            /*
             * IGMP traps have no arguments
             */
            cint_rx_trap_protocol_traps_protocol_profiles.igmp_trap_profile = trap_profile;
            break;
        case bcmRxTrapDhcpv4Server:
        case bcmRxTrapDhcpv4Client:
        case bcmRxTrapDhcpv6Client:
        case bcmRxTrapDhcpv6Server:
            /*
             * DHCP traps have no arguments
             */
            cint_rx_trap_protocol_traps_protocol_profiles.dhcp_trap_profile = trap_profile;
            break;
        case bcmRxTrapArp:
        case bcmRxTrapArpMyIp:
            /*
             * ARP traps have no arguments. notice my ARP trap needs additional configuration of IPs
             */
            cint_rx_trap_protocol_traps_protocol_profiles.arp_trap_profile = trap_profile;
            break;
        case bcmRxTrapNdpMyIp:
            /*
             * NDP traps have no arguments. notice my NDP trap needs additional configuration of IPs * For NDP trap
             * profile configuration icmpv6 profile needs to be configured
             */
            cint_rx_trap_protocol_traps_protocol_profiles.icmpv6_trap_profile = trap_profile;
            break;
        case bcmRxTrapNdp:
            /*
             * NDP traps have no arguments. notice my NDP trap needs additional configuration of IPs * For NDP trap
             * profile configuration icmpv6 profile needs to be configured
             */
            cint_rx_trap_protocol_traps_protocol_profiles.icmpv6_trap_profile = trap_profile;
            break;
        default:
            rv = BCM_E_PARAM;
            printf("Error, invalid protocol type\n");
            return rv;
    }

    return rv;
}

/**
* \brief
* Adittional configuration for My-ARP and My-NDP protocols
* ARP/NDP packets are traped by My-ARP/My-NDP traps the packets dip matches one of the ips configured 
* \par DIRECT INPUT:
*   \param [in] unit      -  unit Id
*   \param [in] protocol_type  - protocol type
*   \param [in] ndp_ip_index   - NDPMyIP 32b chunk index from IPv6
*   \param [in] my_ip1  - ip1 to configure
*   \param [in] my_ip2  - ip2 to configure
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_protocol_traps_arp_and_ndp_my_ip_config(
    int unit,
    bcm_rx_trap_t protocol_type,
    int ndp_ip_index,
    int my_ip1,
    int my_ip2)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_t switch_control_ip1, switch_control_ip2;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;

    if (protocol_type == bcmRxTrapArpMyIp)
    {
        switch_control_ip1 = bcmSwitchArpMyIp1;
        switch_control_ip2 = bcmSwitchArpMyIp2;

        rv = bcm_switch_control_set(unit, switch_control_ip1, my_ip1);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_switch_control_set with IP1\n");
            return rv;
        }

        rv = bcm_switch_control_set(unit, switch_control_ip2, my_ip2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_switch_control_set with IP2\n");
            return rv;
        }
    }
    else if (protocol_type == bcmRxTrapNdpMyIp)
    {
        switch_control_ip1 = bcmSwitchNdpMyIp1;
        switch_control_ip2 = bcmSwitchNdpMyIp2;

        key.type = switch_control_ip1;
        key.index = ndp_ip_index;

        info.value = my_ip1;
        rv = bcm_switch_control_indexed_set(unit, key, info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_switch_control_set with IP1\n");
            return rv;
        }

        key.type = switch_control_ip2;
        key.index = ndp_ip_index;

        info.value = my_ip2;
        rv = bcm_switch_control_indexed_set(unit, key, info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_switch_control_set with IP1\n");
            return rv;
        }
    }
    else
    {
        printf("Invalid protocol type, configurating My-IP is only relevant for My-ARP and My-NDP protocols\n");
        return BCM_E_PARAM;        
    }



    return rv;
}

/**
* \brief
* Adittional configuration for ARP and ICMP protocols
* indication if DA is ignored in evaluating the trap condition
* \par DIRECT INPUT:
*   \param [in] unit      -  unit Id
*   \param [in] protocol_type  - protocol type
*   \param [in] ignore_da - TRUE/FALSE value indication if DA is ignored
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
cint_rx_trap_protocol_traps_arp_and_icmp_ignore_da_config(
    int unit,
    bcm_rx_trap_t protocol_type,
    int ignore_da)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_t switch_control_ignore_da;
    
    if (protocol_type == bcmRxTrapArp)
    {
        switch_control_ignore_da = bcmSwitchArpIgnoreDa;
    }
    else if (protocol_type == bcmRxTrapIcmpRedirect)
    {
        switch_control_ignore_da = bcmSwitchIcmpIgnoreDa;
    }
    else
    {
        printf("Invalid protocol type, configurating ignore_da is only relevant for ARP and ICMP protocols\n");
        return BCM_E_PARAM;
    }

    rv = bcm_switch_control_set(unit, switch_control_ignore_da, ignore_da);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_switch_control_set with ignore DA\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Function that sets-up the desired protocol trap to be ready for use.
* \par DIRECT INPUT:
*   \param [in] unit                    - unit Id
*   \param [in] port                    - In-PP-Port
*   \param [in] intf                    - LIF interface
*   \param [in] protocol_type           - protocol type
*   \param [in] protocol_trap_profile   - Profile of the protocol trap(0-3)
*   \param [in] protocol_args           - protocol arguments
*   \param [in] is_icmpv6               - relevant for ICMP protocol only, indicates if ICMPv6 or ICMPv4 type
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_protocol_traps_destroy(
    int unit,
	bcm_gport_t port,
	bcm_if_t intf,
    bcm_rx_trap_t protocol_type,
    int trap_profile,
    int trap_args,
	int is_icmpv6)
{
    int rv = BCM_E_NONE;
    cint_rx_trap_protocol_traps_key.protocol_trap_profile = trap_profile;
    cint_rx_trap_protocol_traps_key.protocol_type = protocol_type;

    rv = cint_rx_trap_protocol_traps_args_and_profiles_fill(unit, protocol_type, 0, trap_args, is_icmpv6);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_rx_trap_protocol_traps_args_and_profiles_fill\n");
        return rv;
    }

    if(port != 0)
    {
        rv = bcm_rx_trap_protocol_profiles_set(unit, port, &cint_rx_trap_protocol_traps_protocol_profiles);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_protocol_profiles_set\n");
            return rv;
        }
    }
    else if(intf != 0)
    {
        rv = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &cint_rx_trap_protocol_traps_protocol_profiles);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_protocol_interface_profiles_set\n");
            return rv;
        }
    }

    rv = bcm_rx_trap_protocol_clear(unit,&cint_rx_trap_protocol_traps_key);
    if (rv != BCM_E_NONE)
   {
       printf("Error in bcm_rx_trap_protocol_clear\n");
       return rv;
   }

    return rv;
}
