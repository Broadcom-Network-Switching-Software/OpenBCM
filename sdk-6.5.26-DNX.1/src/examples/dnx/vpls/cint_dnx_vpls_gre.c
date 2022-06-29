/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_vpls_gre.c Purpose: configure egress PWE pointing to GRE. 
 */

/*
 *  
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_vpls_advanced.c
 * cint ../../../../src/examples/dnx/vpls/cint_dnx_vpls_gre.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = vpls_gre_main(unit,in_port,out_port);
 * print rv; 
 *
 * cint configures GRE tunnel pointed by PWE tunnel
 */

void program_gre_tunnel(
    int       unit,
    bcm_if_t *tunnel_id)
{
    bcm_tunnel_initiator_t tunnel_info;
    bcm_l3_intf_t          l3_intf;
    bcm_ip_t               dip = 0xB5000001; /* 181.0.0.1 */
    bcm_ip_t               sip = 0xB6000001; /* 182.0.0.1 */
    int                    dscp = 10;
    int                    ttl = 64;
    int                    rv = BCM_E_NONE;

    bcm_tunnel_initiator_t_init(&tunnel_info);
    bcm_l3_intf_t_init(&l3_intf);
    tunnel_info.type = bcmTunnelTypeGreAnyIn4;
    tunnel_info.dip                     = dip;
    tunnel_info.sip                     = sip;
    tunnel_info.dscp                    = dscp;
    tunnel_info.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_info.ttl                     = ttl;
    tunnel_info.encap_access            = bcmEncapAccessTunnel4;

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%s), bcm_tunnel_initiator_create tunnel_info.tunnel_id 0x%x flags 0x%x\n",
        bcm_errmsg(rv), tunnel_info.tunnel_id, tunnel_info.flags);
        return rv;
    }
    *tunnel_id = tunnel_info.tunnel_id;
    printf("%s() tunnel_info.tunnel_id 0x%x intf.l3a_intf_id 0x%x flags 0x%x\n",
    "program_gre_tunnel", tunnel_info.tunnel_id,            l3_intf.l3a_intf_id, tunnel_info.flags);
    return rv;
}


 /*
  * Main function for ingress, egress VPLS configuration based on MPLS utility: 
  */
int
vpls_gre_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char *proc_name = "vpls_util_advanced_main";
    bcm_if_t tunnel_id;

    if (!vpls_util_advanced_params_set)
    {
        rv = init_default_vpls_util_advanced_params(unit, port1, port2);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in init_default_vpls_util_advanced_params\n", rv);
            return rv;
        }
    }
    mpls_util_entity[0].fecs[0].tunnel_gport = &mpls_util_entity[1].arps[0].arp;

    rv = program_gre_tunnel(unit, &tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in program_gre_tunnel\n", rv);
        return rv;
    }

    vpls_util_use_non_protected_vpls = 1;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_id, tunnel_id);
    printf("tunnel_id = 0x%x\n", tunnel_id);
    vpls_util_entity[0].vpls_egress[0].egress_tunnel_if = &tunnel_id;

    rv = vpls_util_advanced_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_util_advanced_main\n", rv);
        return rv;
    }

    return rv;
}


