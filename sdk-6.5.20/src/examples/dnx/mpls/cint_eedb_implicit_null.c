/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_eedb_implicit_null.c Purpose: test MPLS implicit NULL EEDB entry
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
 * cint ../../../../src/examples/dnx/mpls/cint_eedb_implicit_null.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int port1 = 200; 
 * int port2 = 201;
 * int nof_encap_stages = 0;
 * int set_el_eli =0;
 * rv = eedb_implicit_null_main(unit,port1,port2,nof_encap_stages,set_el_eli);
 * print rv; 
 * 
 * Use cint_dnx_utility_mpls.c to configure MPLS/IPv4/IPv6 forwarding
 * Configure 0-4 MPLS encapsulations of various formats of PHP and PUSH
 * Perform ETH termination and requested forwarding
 * Perform MPLS encapsulation based on configuration where for IP forwarding IMLICIT_NUL tunnels does not add or remove MPLS PHP tunnels, MPLS forwarding results in POP of forwarding tunnel.
 * Add ARP
 */

int
eedb_implicit_null_main(
    int unit,
    int port1,
    int port2,
    int nof_encap_stages,
    int encap_stages_format)
{
    int rv = BCM_E_NONE;
    int encap_access_array[4] = {bcmEncapAccessTunnel1, bcmEncapAccessTunnel2, bcmEncapAccessTunnel3, bcmEncapAccessTunnel4};
    int encap_access_idx = 0;
    int mpls_tunnel_id = 0;
    bcm_mpls_label_t stage_enc_label = 3333;

    MPLS_UTIL_MAX_NOF_ENTITIES = 2;

    init_default_mpls_params(unit, port1, port2);

    mpls_tunnel_initiator_create_s_init(mpls_util_entity[0].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
    
    /* encapsulation entries */
    for (mpls_tunnel_id = 0; mpls_tunnel_id < nof_encap_stages; mpls_tunnel_id++, encap_access_idx++)
    {
        if ((encap_stages_format >> mpls_tunnel_id) & 0x1)
        {
            /* configure MPLS encap tunnel */
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = stage_enc_label;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
            stage_enc_label++;
        }
        else
        {
            /* configure MPLS PHP tunnel */
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].label[0] = BCM_MPLS_LABEL_INVALID;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].num_labels = 1;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].action = BCM_MPLS_EGRESS_ACTION_PHP;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].flags |= (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)?0:BCM_MPLS_EGRESS_LABEL_STAT_ENABLE;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].l3_intf_id = &mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id+1].tunnel_id;
            mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id].encap_access = encap_access_array[encap_access_idx];
        }
    }

    mpls_util_entity[0].mpls_encap_tunnel[mpls_tunnel_id-1].l3_intf_id = &mpls_util_entity[1].arps[0].arp;

    rv = mpls_util_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_util_main\n", rv);
        return rv;
    }

    return rv;

}
