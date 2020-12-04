/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_mpls_tandem.c Purpose: utility for MPLS TANDEM over VPLS. 
 */

/*
 * 
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dnx/mpls/cint_mpls_tandem.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int port1 = 200; 
 * int port2 = 201;
 * int nof_mpls = 1;
 *int nof_tandem = 0;
 * int special_label = 0;
 * rv = tandem_main(unit,port1,port2,nof_mpls, nof_tandem, special_label);
 * print rv; 
 * 
 * 
 
 *  Scenarios configured in this cint:
 *  In each cint itteration only 1 scenario is configured, based on defined 'nof_mpls' + 'nof_tandem' + 'special_label'
 *  If 'special_label' is set ELIoEL labels will be set on the BOS MPLS label, just before the PWE label
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  basic with 1 MPLS label
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 1 MPLS and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||   MPLS   ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0     ||                  ||                          ||                      ||
 *   |    |                         |                       ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  basic with 2 MPLS labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 2 MPLS and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS   ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x4444||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0         ||Exp:0     ||                  ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  basic with 1 MPLS with 1 TANDEM labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 1 MPLS, 1 TANDEM and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS       ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x5656||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0         ||Exp:0         ||                  ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  basic with 1 MPLS with 2 TANDEM labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 1 MPLS, 2 TANDEM and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||  MPLS        ||   MPLS   ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x4545||Label:0x4444||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0         ||Exp:0         ||Exp:0     ||                  ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  basic with 2 MPLS with 1 TANDEM labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 2 MPLS, 1 TANDEM and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS       ||   MPLS       ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x5656||Label:0x4444||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0         ||Exp:0         ||Exp:0         ||                  ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  basic with 2 MPLS with 2 TANDEM labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 2 MPLS, 2 TANDEM and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS       ||  MPLS        ||   MPLS       ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x4545||Label:0x5656||Label:0x4444||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0         ||Exp:0         ||Exp:0         ||Exp:0         ||                  ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 */

int
tandem_main(
    int unit,
    int port1,
    int port2,
    int nof_mpls,
    int nof_tandem,
    int special_label)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "tandem_main";
    /* printf("%s(): ENTER. port1 %d,port2 %d, mode %d \r\n", proc_name, port1, port2, mode); */
    init_default_vpls_params(unit);

    if(special_label)
    {
        mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }

    if(nof_tandem == 0)
    {
        outer_mpls_tunnel_index = 0;
    }
    else
    {
        outer_mpls_tunnel_index = 1;
    }

    if(nof_mpls ==2)
    {
        mpls_encap_tunnel[0].num_labels = 2;
        mpls_encap_tunnel[0].label[1] = 0x4444;
    }
    /* configure TANDEM tunnel */
    if(nof_tandem == 1)
    {
        mpls_encap_tunnel[1].label[0] = 0x5656;
        mpls_encap_tunnel[1].num_labels = 1;
        mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel3;;
        mpls_encap_tunnel[1].tunnel_id = 8196;
        mpls_encap_tunnel[1].flags = mpls_encap_tunnel[0].flags | BCM_MPLS_EGRESS_LABEL_TANDEM;
    }
    else if(nof_tandem == 2)
    {
        mpls_encap_tunnel[1].label[0] = 0x5656;
        mpls_encap_tunnel[1].label[1] = 0x4545;
        mpls_encap_tunnel[1].num_labels = 2;
        mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel3;;
        mpls_encap_tunnel[1].tunnel_id = 8196;
        mpls_encap_tunnel[1].flags = mpls_encap_tunnel[0].flags | BCM_MPLS_EGRESS_LABEL_TANDEM;
    }

    rv = vpls_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error(%d), in vpls_mp_check_support\n", proc_name, rv);
        return rv;
    }

    /* printf("%s(): EXIT. port1 %d,port2 %d, mode %d \r\n", proc_name, port1, port2, mode); */
    return rv;
}
