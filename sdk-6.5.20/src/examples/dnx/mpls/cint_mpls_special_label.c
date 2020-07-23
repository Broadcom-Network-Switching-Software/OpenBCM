/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_mpls_special_label.c Purpose: special label termination and encapsulation example. 
 */

/*
 * 
 * Configuration:

 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dnx/mpls/cint_mpls_special_label.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int pwe_flag = 0;
 * int mpls_tunnel_flag = 0;
 * int mpls_tunnel_num = 1;
 * int update_hw = 1;
 * rv = mpls_special_label_main(unit,pwe_flag,mpls_tunnel_flag,mpls_tunnel_num,update_hw);
 * print rv; 
 * 
 * At ingress, special label could be terminated along with general labels, and indicating some additional property of label
 * At egress, special label could be encapsulated as additional label along with main mpls label
 
 *  Scenarios configured in this cint:
 *  In each cint itteration only 1 scenario is configured, based on defined pwe_flag and mpls_tunnel_flag, mpls_tunnel_num
 * pwe_flag: which special label should be along with PWE label at ingress ,and should be encapsulated with PWE label at egress;
 * mpls_tunnel_flag: which special label should be along with tunnel label at ingress ,and should be encapsulated with tunnel label at egress;
 * mpls_tunnel_num: tunnel number should be added after the PWE label, up to 3.
 * for simplication, when multiple mpls tunnels is added, they will carry the same special label set defined by mpls_tunnel_flag.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Packet from AC side:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan      | tpid
 *   |    | 0c:00:02:00:00       |11:00:00:01:12         || 5         | 0x8100
 *   |    |                      |                       ||          ||               
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+--+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  packet from PWE side(termination or encapsulation)
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS       ||   MPLS       ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01  |00:00:00:cd:1d || tunne-3    ||  tunnel-2   ||   tunnel-1  ||                 || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+--+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      MPLS-tunnel-format
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+--+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      after-tunnel-special-label ||  FRR      ||  MPLS        || before-tunnel-special-labels
 *   |    | optional:ELI,EL                 || optional  ||              || optional:    Explicit NULL,
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+--+-+-+-+-+-+-+-++-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      PWE-format
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |   CW       ||   after-tunnel-special-label   ||  FRR      ||  PWE        || before-tunnel-special-labels
 *   |    | optional   ||   optional:ELI,EL              || optional  ||             || optional:    Explicit NULL,
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+--+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

enum MPLS_SPECIAL_LABEL_E{
    NO_LABEL = 0x0,
    NO_SPECIAL_LABEL = 0x001,
    MPLS_SPECIAL_LABEL_ELI = 0x002,
    MPLS_SPECIAL_LABEL_EL =   0x004,
    MPLS_SPECIAL_LABEL_CW =  0x008,
    MPLS_SPECIAL_LABEL_IPv4_NULL = 0x010,
    MPLS_SPECIAL_LABEL_IPv6_NULL = 0x020,
    MPLS_SPECIAL_LABEL_RA = 0x040,
    MPLS_SPECIAL_LABEL_OAM_ALERT = 0x080,
    MPLS_SPECIAL_LABEL_IMPLICIT_NULL= 0x100,
    MPLS_SPECIAL_LABEL_Double_IPv4_NULL = 0x200,
    MPLS_SPECIAL_LABEL_Triple_IPv4_NULL =  0x400,
    MPLS_SPECIAL_LABEL_GAL = 0x800
};

/**
 * Initiate the meaningful label information according to mpls tunnel number
 */
int
mpls_special_label_init_params (
    int unit)
{
    int rv = BCM_E_NONE;
    int i = 0;

    init_default_vpls_params(unit);

    for (i = 0;i <= outer_mpls_tunnel_index;i++) {
        mpls_encap_tunnel[i].label[0] = 3333 + i*1000;
        mpls_encap_tunnel[i].num_labels = 1;
        mpls_encap_tunnel[i].encap_access = bcmEncapAccessTunnel2+i;
        mpls_encap_tunnel[i].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_encap_tunnel[i].tunnel_id = 8194 + i ;

        /* mpls tunnel termination parameters */
        mpls_term_tunnel[i].action = BCM_MPLS_SWITCH_ACTION_POP;
        mpls_term_tunnel[i].label = mpls_encap_tunnel[i].label[0];
    }

        return rv;
}

/**
 * Configure the mpls/pwe encap/termination property.
 * INPUT:
 *   pwe_flag   - label property of PWE
 *   mpls_tunnel_flag -label property of MPLS tunnel
 *  mpls_tunnel_num  - number of mpls tunnel(1-3 for vpls service)
 */
int
mpls_special_label_tunnel_property_set (
    int unit,
    int pwe_flag,
    int mpls_tunnel_flag,
    int mpls_tunnel_num)
{
    int rv = BCM_E_NONE;
    int i = 0;


    if (pwe_flag & MPLS_SPECIAL_LABEL_ELI) {
        pwe_encap.label_flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    } else if (pwe_flag & MPLS_SPECIAL_LABEL_EL) {
        pwe_encap.label_flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
        pwe_term.flags |=  BCM_MPLS_PORT_ENTROPY_ENABLE;
    }

    if (pwe_flag & MPLS_SPECIAL_LABEL_CW) {
        pwe_encap.flags |= BCM_MPLS_PORT_CONTROL_WORD;
        pwe_term.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    
    for (i =  0;i < mpls_tunnel_num+1;i++) {
        if (mpls_tunnel_flag & MPLS_SPECIAL_LABEL_ELI) {
            mpls_encap_tunnel[i].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
        } 
    }        

        return rv;
}


/**
 * Main entrance for mpls/pwe encap/termination property configuration.
 * INPUT:
 *   pwe_flag   - label property of PWE
 *   mpls_tunnel_flag -label property of MPLS tunnel
 *   mpls_tunnel_num  - number of mpls tunnel(1-3 for vpls service)
 *   update_hw    flag to update the HW according to SW configuration
 */
int
mpls_special_label_main (
    int unit,
    int pwe_flag,
    int mpls_tunnel_flag,
    int mpls_tunnel_num,
    int update_hw)
{
    int rv = 0;

    outer_mpls_tunnel_index = mpls_tunnel_num;

    rv = mpls_special_label_init_params(unit);
    if (rv != 0) {
        printf("mpls_special_label_init_params failed %d",rv);
        return rv;
    }

    rv = mpls_special_label_tunnel_property_set(unit,pwe_flag,mpls_tunnel_flag,mpls_tunnel_num);
    if (rv != 0) {
        printf("mpls_special_label_tunnel_property_set %d",rv);
        return rv;
    }

    if (update_hw) {
        rv = vpls_main(unit,pwe_port.port,ac_port.port);
        if (rv != 0) {
            printf("vpls_main %d",rv);
            return rv;
        }
    }

    return rv;

}


