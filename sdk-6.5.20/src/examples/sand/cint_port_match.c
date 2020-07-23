/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_port_match.c
 * Purpose: Example of the port match application
 *
 *  The purpose of the port match application is to configure several mappings over the same gport.
 *  It has two modes: ingress match and egress match.
 *  Egress match allows you to set several <vlan_domain, vid> combinations on the same egress vlan port.
 *  Ingress match allows you to set several <vlan_domain, vid> combinations on the same ingress vlan port.
 *      * Ingress match is also divided into two modes: with initial match, and without initial match.
 *          * With initial match means that bcm_vlan_port_create will also add the first <vlan_domain, vid> mapping
 *          * Without initial match means that bcm_vlan_port_create will not add the first <vlan_domain, vid> mapping.
 *              As a side effect, it will also not add the vlan translation configuration. The vlan will remain the same,
 *              and the translation will need to be added manuall
 *  The example supports also Asymmetric LIFs.
 *      * Ingress match - No EVE operation is applied as the default EVE configuration during bcm_vlan_port_create
 *        isn't applicable for an Ingress only LIF.
 *      * Egress match - Both Ingress only and Egress only VLAN ports are created, and the matching for the
 *        Egress ports is attached to the Egress only LIF. An additional Egress LIF is created in order to associate
 *        physical ports using Port match and discard packets to the these ports. In this way, some of the ports
 *        receive the packets with no VLAN edit, while packets to the other ports are dropped.
 *  
 * 
 *  To run in ingress match mode:
 *  BCM.0> cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
 *  BCM.0> cint ../../../../src/examples/dpp/cint_port_tpid.c
 *  BCM.0> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 *  BCM.0> cint ../../../../src/examples/dpp/cint_multi_device_utils.c
 *  BCM.0> cint ../../../../src/examples/dpp/cint_port_match.c
 *  BCM.0> cint
 *  cint> int sysport1, sysport2, sysport3;
 *  cint> print port_to_system_port(<local unit for port 1>, <port1>, &sysport1);
 *  cint> print port_to_system_port(<local unit for port 2>, <port2>, &sysport2);
 *  cint> print port_to_system_port(<local unit for port 3>, <port3>, &sysport3);
 *  cint> port_match_info_init_default(<sysport1>, <sysport2>, <sysport3>, -1);
 *  cint> int nof_units = <nof_units>;
 *  cint> int units[<nof_units>] = {<first_unit>, <second unit>...};
 *  cint> print port_match_run_ingress_match(units, nof_units, <no-initial-match>, <is-asymmetric-lif>);
 * 
 *  *Note - you can skip the port_to_system_port calls if you're running on single device
 * 
 *  After this, the chip will be configured with the following <port, vid> -> <vid> matches:
 *          <port1, 10>    ->   <20>
 *          <port1, 30>    ->   <20>
 *  * Note - when choosing no_initial_match != 0, vlan translation mapping will not work, only port mapping.
 *  * Note 2 - This matching won't be applicable for Asymmetric LIFs as no default EVE oprtation is defined for the LIF.
 * 
 *  Port mapping will be by dst mac:
 *      00:00:00:00:00:01   ->  port2
 *      00:00:00:00:00:02   ->  port3
 * 
 * 
 *  To test traffic:
 *  * The received packet will have an identical VLAN if the CINT is ran without initial match or
 *  * with Asymmetric LIFs
 *  1) Send to port1:
 *      An ethernet packet , dst mac 00:00:00:00:00:01, src mac any, vlan 10, tpid 8100
 *      Get in port 2:
 *      An ethernet packet, identical src mac and dst mac , vlan 20, tpid 8100
 * 
 *  2) Send to port1:
 *      An ethernet packet , dst mac 00:00:00:00:00:02, src mac any, vlan 10, tpid 8100
 *      Get in port 3:
 *      An ethernet packet, identical src mac and dst mac , vlan 20, tpid 8100
 * 
 *  3) Send to port 1:
 *      An ethernet packet, dst mac 00:00:00:00:00:01, src mac any, vlan 30, tpid 8100
 *      Get in port 2:
 *      An ethernet packet, identical src mac and dst mac , vlan 20, tpid 8100
 * 
 *  4) Send to port 1:
 *      An ethernet packet, dst mac 00:00:00:00:00:02, src mac any, vlan 30, tpid 8100
 *      Get in port 3:
 *      An ethernet packet, identical src mac and dst mac , vlan 20, tpid 8100
 * 
 *  To run in egress match mode:
 *  BCM.0> cint ../../../../src/examples/dpp/cint_port_tpid.c
 *  BCM.0> cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 *  BCM.0> cint ../../../../src/examples/dpp/cint_multi_device_utils.c
 *  BCM.0> cint ../../../../src/examples/dpp/cint_port_match.c
 *  BCM.0> cint
 *  cint> int sysport1, sysport2, sysport3, sysport4;
 *  cint> print port_to_system_port(<local unit for port 1>, <port1>, &sysport1);
 *  cint> print port_to_system_port(<local unit for port 2>, <port2>, &sysport2);
 *  cint> print port_to_system_port(<local unit for port 3>, <port3>, &sysport3);
 *  cint> print port_to_system_port(<local unit for port 4>, <port4>, &sysport4);
 *  cint> port_match_info_init_default(<sysport1>, <sysport2>, <sysport3>, <sysport4>);
 *  cint> int nof_units = <nof_units>;
 *  cint> int units[<nof_units>] = {<first_unit>, <second unit>...};
 *  cint> print port_match_run_egress_match(units, nof_units, <is-asymmetric-lif>);
 * 
 *  *Note - you can skip the port_to_system_port calls if you're running on single device
 *  *Note - on multi device systems, keep ports 2-4 (the egress ports) on the same device. That's because the if egress vlan port is
 *          created on a different unit than the port, the port will not be attached to it. 
 *
 *  After this, the chip will be configured with the following <port, vid> -> <vid> matches:
 *          <port1, 50>    ->   <40>
 *  Note - This matching won't be applicable for Asymmetric LIFs as no default EVE operation is defined when the
 *         match mode is NONE. More ever, packets for port 4 will be dropped.
 *
 *  Port mapping will be by dst mac:
 *      00:00:00:00:00:01   ->  port2
 *      00:00:00:00:00:02   ->  port3
 *      00:00:00:00:00:03   ->  port4
 * 
 *  To test traffic:
 *  * The recieved packet will have an identical vlan if the cint is ran with Asymmetric LIFs
 *  1) Send to port1:
 *      An ethernet packet , dst mac 00:00:00:00:00:01, src mac any, vlan 50, tpid 8100
 *      Get in port 2:
 *      An ethernet packet, identical src mac and dst mac , vlan 40, tpid 8100
 * 
 *  2) Send to port 1:
 *      An ethernet packet, dst mac 00:00:00:00:00:02, src mac any, vlan 50, tpid 8100
 *      Get in port 3:
 *      An ethernet packet, identical src mac and dst mac , vlan 40, tpid 8100
 * 
 *  2) Send to port 1:
 *      An ethernet packet, dst mac 00:00:00:00:00:03, src mac any, vlan 50, tpid 8100
 *      Get in port 4:
 *      An ethernet packet, identical src mac and dst mac , vlan 40, tpid 8100
 * 
 * 
 *  Notes:
 *      * In both Ingress & Egress modes, after calling port_match_info_init_default, you can change the members of the
 *      port_match_info_1 struct if you want different mappings.
 *      * This cint can work on multi device systems. 
 */

int nof_macs = 2;
int verbose = 3;
int port_id_mask = 0xff;

struct port_match_info_s {
    int in_vsi;
    int eg_vsi;
    int in_vids[3];
    int eg_vids[2];
    int ports[4];
    bcm_mac_t dmac_1;
    bcm_mac_t dmac_2;
    bcm_mac_t dmac_3;
    int tpid;
    bcm_gport_t in_gport;
    bcm_gport_t eg_gport_ingress;
    bcm_gport_t eg_gport_egress;
    bcm_gport_t vlan_port_egress_default;
} ;

port_match_info_s port_match_info_1;


int 
port_match_info_init_default(bcm_gport_t port_1, bcm_gport_t port_2, bcm_gport_t port_3, bcm_gport_t port_4){
    bcm_mac_t dmac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    bcm_mac_t dmac_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_mac_t dmac_3 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
    
    port_match_info_1.tpid = 0x8100;
    port_match_info_1.in_vsi = 500;
    port_match_info_1.eg_vsi = 600;

    port_match_info_1.in_vids[0] = 10;
    port_match_info_1.eg_vids[0] = 20;
    port_match_info_1.in_vids[1] = 30;
    port_match_info_1.eg_vids[1] = 40;
    port_match_info_1.in_vids[2] = 50;

    sal_memcpy(port_match_info_1.dmac_1, dmac_1, 6);

    sal_memcpy(port_match_info_1.dmac_2, dmac_2, 6);

    sal_memcpy(port_match_info_1.dmac_3, dmac_3, 6);

    port_match_info_1.ports[0] = port_1;
    port_match_info_1.ports[1] = port_2;
    port_match_info_1.ports[2] = port_3;
    port_match_info_1.ports[3] = port_4;
    port_match_info_1.in_gport = -1;   
    port_match_info_1.eg_gport_ingress = -1;  
    port_match_info_1.eg_gport_egress = -1;
    port_match_info_1.vlan_port_egress_default = -1;
    return BCM_E_NONE;
}
            

int port_match_run_ingress_match(int *units_ids, int nof_units, int no_initial_match, int is_asymmetric_lifs){
    int rv, i, j;
    int unit, port;
    bcm_l2_addr_t l2_addr;
    uint32 flags;
    bcm_port_class_t portClass  = bcmPortClassId;
    bcm_gport_t vlan_port_1;

    /* Add first l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_1, port_match_info_1.in_vsi);
    l2_addr.port =port_match_info_1.ports[1];
    units_array_make_local_first(units_ids, nof_units,port_match_info_1.ports[1]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Add second l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_2, port_match_info_1.in_vsi);
    l2_addr.port =port_match_info_1.ports[2];
    units_array_make_local_first(units_ids, nof_units,port_match_info_1.ports[2]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /***********************/
    /** Create vlan port **/
    /*********************/

    /* If we're in advanced vlan translation mode, we have to define tpid and init
       vlan translation configurations*/
    if (advanced_vlan_translation_mode && !no_initial_match) {
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            port_match_init_vlan_translation(unit, 1);
        }
    }

    units_array_make_local_first(units_ids, nof_units, port_match_info_1.ports[0]);

    /* In asymmetric mode, no default EVE will be configured. */
    flags = 0;
    if (is_asymmetric_lifs) {
        flags |= BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    }

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if(is_system_port_is_local(unit, port_match_info_1.ports[0])) {
            if (no_initial_match) {
                rv = vlan_port_create(unit, 
                                      0, 
                                      BCM_VLAN_PORT_MATCH_NONE, 
                                      &vlan_port_1, 
                                      0,
                                      port_match_info_1.eg_vids[0],
                                      port_match_info_1.in_vsi,
                                      flags);
                if (rv != 0){
                    printf("Error, in vlan_port_create\n");
                    return rv;
                }
            } else {    /* Create */
                rv = vlan_port_create(unit, 
                                      port_match_info_1.ports[0], 
                                      is_device_or_above(unit, JERICHO2) ? BCM_VLAN_PORT_MATCH_PORT_CVLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN, 
                                      &vlan_port_1, 
                                      port_match_info_1.in_vids[0],
                                      port_match_info_1.eg_vids[0],
                                      port_match_info_1.in_vsi,
                                      flags);
                if (rv != 0){
                    printf("Error, in vlan_port_create\n");
                    return rv;
                }
            }
        }
        flags |= BCM_VLAN_PORT_WITH_ID;
    }
    port_match_info_1.in_gport = vlan_port_1;

    if (no_initial_match) {
        /* Add the match criteria we've missed by setting the vlan port on _PORT_MATCH_NONE */
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            rv = port_match_add_remove(unit, 
                           is_device_or_above(unit, JERICHO2) ? BCM_PORT_MATCH_PORT_CVLAN : BCM_PORT_MATCH_PORT_VLAN, 
                           BCM_PORT_MATCH_INGRESS_ONLY, 
                           port_match_info_1.in_vids[0],
                           port_match_info_1.ports[0],
                           vlan_port_1,
                           1);
        }
    }

    /* Add multiple match critreia for the existing vlan port */
    /* Local unit is irrelevant */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if(is_system_port_is_local(unit, port_match_info_1.ports[0])) {
            rv = port_match_add_remove(unit, 
                           is_device_or_above(unit, JERICHO2) ? BCM_PORT_MATCH_PORT_CVLAN : BCM_PORT_MATCH_PORT_VLAN, 
                           BCM_PORT_MATCH_INGRESS_ONLY, 
                           port_match_info_1.in_vids[1],
                           port_match_info_1.ports[0],
                           vlan_port_1,
                           1);
            if (rv != 0){
                printf("Error, in port_match_add_remove\n");
                return rv;
            }
        }
    }

    /* Add port in vlan membership */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];        
        if (is_system_port_is_local(unit, port_match_info_1.ports[0]))
        {
            rv = bcm_vlan_gport_add(unit, port_match_info_1.in_vids[0], port_match_info_1.ports[0], 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_vlan_gport_add\n");
                return rv;
            }

            rv = bcm_vlan_gport_add(unit, port_match_info_1.in_vids[1], port_match_info_1.ports[0], 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_vlan_gport_add\n");
                return rv;
            }
        }
    }

    printf("Done configuring ingress port match\n");
    return rv;
}

int
port_match_run_egress_match(int *units_ids, int nof_units, int is_asymmetric_lifs){
    int unit, i, j, rv, port;
    bcm_l2_addr_t l2_addr;
    uint32 flags;
    bcm_port_class_t portClass  = bcmPortClassId;
    bcm_gport_t vlan_port_1, vlan_port_egress, vlan_port_egress_default;
    int nof_eg_ports = 3;
    int nof_dropped_ports = (is_asymmetric_lifs) ? 1 : 0;

    /* Set VLAN domain */
    for (j = 0 ; j < nof_eg_ports ; j++){
        port = port_match_info_1.ports[j + 1];
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            if (is_system_port_is_local(unit, port)) {
                rv = bcm_port_class_set(unit, port, portClass, port & port_id_mask);
                if (rv != BCM_E_NONE){
                    printf("Error, in bcm_port_class_set\n");
                    return rv;
                }
            }
        }
    }

    /* If we're in advanced vlan translation mode, we have to define tpid and init
    vlan translation configurations*/
    if (advanced_vlan_translation_mode) {
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            port_match_init_vlan_translation(unit, 0);
        }
    }

    /* Add first l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_1, port_match_info_1.eg_vsi);
    l2_addr.port =port_match_info_1.ports[1];
    units_array_make_local_first(units_ids, nof_units,port_match_info_1.ports[1]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Add second l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_2, port_match_info_1.eg_vsi);
    l2_addr.port =port_match_info_1.ports[2];
    units_array_make_local_first(units_ids, nof_units,port_match_info_1.ports[2]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    /* Add third l2 dest address */
    bcm_l2_addr_t_init(&l2_addr, port_match_info_1.dmac_3, port_match_info_1.eg_vsi);
    l2_addr.port =port_match_info_1.ports[3];
    units_array_make_local_first(units_ids, nof_units,port_match_info_1.ports[3]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_addr_add\n");
            return rv;
        }
    }

    
    /* Create a VLAN port for Simple bridge. In asymmetric mode, no default EVE
       is configured. */
    units_array_make_local_first(units_ids, nof_units, port_match_info_1.ports[0]);
    flags = 0;
    if (is_asymmetric_lifs) {
        flags |= BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    }

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if (is_system_port_is_local(unit, port_match_info_1.ports[0])) {
            rv = vlan_port_create(unit, 
                                  0, 
                                  is_device_or_above(unit, JERICHO2) ? BCM_VLAN_PORT_MATCH_PORT_CVLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN, 
                                  &vlan_port_1, 
                                  port_match_info_1.in_vids[2],
                                  port_match_info_1.eg_vids[1],
                                  port_match_info_1.eg_vsi,
                                  flags);
            if (rv != 0){
                printf("Error, in vlan_port_create\n");
                return rv;
            }
            flags |= BCM_VLAN_PORT_WITH_ID;
        }
    }
    port_match_info_1.eg_gport_ingress = vlan_port_1;
    port_match_info_1.eg_gport_egress = vlan_port_1;

    if (is_asymmetric_lifs) {
        /* In asymmetric mode, create an Egress only VLAN port as well,
           so that LIF matching for the various Egress ports can be added.
           The egress vlan port should be created first on the same unit as the
           egress ports. */
        units_array_make_local_first(units_ids, nof_units, port_match_info_1.ports[1]);

        flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            if (is_system_port_is_local(unit, port_match_info_1.ports[1])) {
                if (is_device_or_above(unit, JERICHO2))
                {
                    flags |= BCM_VLAN_PORT_DEFAULT;
                    flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
                }
                rv = vlan_port_create(unit,
                                      0,
                                      BCM_VLAN_PORT_MATCH_NONE,
                                      &vlan_port_egress,
                                      port_match_info_1.in_vids[2],
                                      port_match_info_1.eg_vids[1],
                                      0,
                                      flags);
                if (rv != 0){
                    printf("Error, in Egress only vlan_port_create\n");
                    return rv;
                }

                /* Create an additional Egress LIF in order to define port default behaviour */
                rv = vlan_port_create(unit,
                                      0,
                                      BCM_VLAN_PORT_MATCH_NONE,
                                      &vlan_port_egress_default,
                                      port_match_info_1.in_vids[2],
                                      port_match_info_1.eg_vids[1],
                                      0,
                                      flags);
                if (rv != 0){
                    printf("Error, in Egress only vlan_port_create\n");
                    return rv;
                }
                flags |= BCM_VLAN_PORT_WITH_ID;
            }
        }
        port_match_info_1.eg_gport_egress = vlan_port_egress;
        port_match_info_1.vlan_port_egress_default = vlan_port_egress_default;

    }

    /* Add multiple egress match critreia for the existing vlan port */
    for (j = 0 ; j < nof_eg_ports ; j++) {
        port = port_match_info_1.ports[j + 1];
        units_array_make_local_first(units_ids, nof_units, port);
        /* Only do it in local unit*/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            if (is_system_port_is_local(unit, port)) {
                if (j < (nof_eg_ports - nof_dropped_ports)) {
                    rv = port_match_add_remove(unit, 
                                   is_device_or_above(unit, JERICHO2) ? BCM_PORT_MATCH_PORT: BCM_PORT_MATCH_PORT_VLAN, 
                                   BCM_PORT_MATCH_EGRESS_ONLY, 
                                   port_match_info_1.eg_vsi,
                                   port,
                                   port_match_info_1.eg_gport_egress,
                                   1);
                    if (rv != 0){
                        printf("Error, in port_match_add_remove\n");
                        return rv;
                    }
                } else {
                    /* Add a port match to the Egress default LIF */
                    rv = port_match_add_remove(unit, 
                                   BCM_PORT_MATCH_PORT, 
                                   BCM_PORT_MATCH_EGRESS_ONLY, 
                                   -1,
                                   port,
                                   port_match_info_1.vlan_port_egress_default,
                                   1);
                    if (rv != 0){
                        printf("Error, in Default Egress port_match_add_remove\n");
                        return rv;
                    }
                }
            }
        }
    }

    /* Add port in vlan membership */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];        
        if (is_system_port_is_local(unit, port_match_info_1.ports[0]))
        {
            rv = bcm_vlan_gport_add(unit, port_match_info_1.in_vids[2], port_match_info_1.ports[0], 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_vlan_gport_add\n");
                return rv;
            }
        }
    }

    /* Set the behaviour for Default Egress to Drop only on its local unit */
    /* bcm_port_discard_set is not support on JR2 */
    if (is_asymmetric_lifs && !is_device_or_above(unit, JERICHO2)) {
        units_array_make_local_first(units_ids, nof_units, port_match_info_1.ports[1]);
        if (!is_device_or_above(units_ids[0], JERICHO2)) {
            rv = bcm_port_discard_set(units_ids[0], port_match_info_1.vlan_port_egress_default, BCM_PORT_DISCARD_EGRESS);
            if (rv != 0) {
                printf("Error, in Default Egress bcm_port_discard_set\n");
                return rv;
            }
        } else {
            bcm_rx_trap_lif_config_t lif_config;
            bcm_gport_t action_gport;
            BCM_GPORT_TRAP_SET(action_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 13, 0);
            lif_config.lif_type = bcmRxTrapLifTypeOutLif;
            lif_config.lif_gport = port_match_info_1.vlan_port_egress_default;
            lif_config.action_gport = action_gport;
            rv = bcm_rx_trap_lif_set(units_ids[0], 0, &lif_config);
            if (rv != 0) {
                printf("Error, in Default Egress bcm_rx_trap_lif_set\n");
                return rv;
            }
        }
    }

    return rv;
}

/* Delete the port matches and gports*/
int 
port_match_cleaup(int *units_ids, int nof_units, int ingress){
    int unit, i, j, rv, port;
    bcm_port_class_t portClass  = bcmPortClassId;
    uint32 match_flags = (ingress) ? BCM_PORT_MATCH_INGRESS_ONLY : BCM_PORT_MATCH_EGRESS_ONLY;
    int nof_eg_ports = (ingress) ? 1 : 3;
    int gport_match = (ingress) ? port_match_info_1.in_gport : port_match_info_1.eg_gport_egress;
    int gport_additional = (ingress) ? (-1) :
            ((port_match_info_1.eg_gport_ingress == port_match_info_1.eg_gport_egress) ? (-1) : port_match_info_1.eg_gport_ingress);
    int nof_dropped_ports = (ingress) ? 0 : ((gport_additional == -1) ? 0 : 1);
    int port_offset = (ingress) ? 0 : 1;
    int match_vid = (ingress) ? port_match_info_1.in_vids[1] : port_match_info_1.eg_vsi;
    int drop_match = 0;
    int match_criteria = 0;
    int sysport_is_local = 0;
    int local_port = 0;

    /* Delete multiple port match critreia from the existing vlan port */
    for (j = 0 ; j < nof_eg_ports ; j++){
        port = port_match_info_1.ports[j + port_offset];
        units_array_make_local_first(units_ids, nof_units, port);
        if ((j >= (nof_eg_ports - nof_dropped_ports)) && (!ingress)) {
            drop_match = 1;
        }
        /* Only do it in local unit*/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            if (is_system_port_is_local(unit, port)) {
                if (!drop_match) {
                    if (ingress) {
                        match_criteria = is_device_or_above(unit, JERICHO2) ? BCM_PORT_MATCH_PORT_CVLAN : BCM_PORT_MATCH_PORT_VLAN;
                    } else{
                        match_criteria = is_device_or_above(unit, JERICHO2) ? BCM_PORT_MATCH_PORT : BCM_PORT_MATCH_PORT_VLAN;
                    }
                    rv = port_match_add_remove(unit, 
                                   match_criteria, 
                                   match_flags, 
                                   match_vid,
                                   port,
                                   gport_match,
                                   0);
                    printf("port_match_add_remove for vid-port-gport %d-%d-%d\n", match_vid, port, gport_match);
                } else {
                    rv = port_match_add_remove(unit, 
                                   BCM_PORT_MATCH_PORT, 
                                   match_flags, 
                                   -1,
                                   port,
                                   port_match_info_1.vlan_port_egress_default,
                                   0);
                    printf("port_match_add_remove for port-gport %d-%d\n", port, port_match_info_1.vlan_port_egress_default);
                }
                if (rv != 0){
                    printf("Error, in port_match_add_remove\n");
                    return rv;
                }
            }
        }
    }

    /* Delete VLAN port for Simple bridge */
    units_array_make_local_first(units_ids, nof_units, port_match_info_1.ports[0]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        sysport_is_local = 0;
        rv = gport_is_local(unit, gport_match, &local_port, &sysport_is_local);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in gport_is_local(0x%x,0x%x).\n", unit, gport_match);
            return rv;
        }
        if(sysport_is_local ==1) {
            rv = bcm_vlan_port_destroy(unit, gport_match);
            if (rv != 0){
                printf("Error, in bcm_vlan_port_destroy\n");
                return rv;
            }
            if (gport_additional != -1) {
                rv = bcm_vlan_port_destroy(unit, gport_additional);
                if (rv != 0){
                    printf("Error, in additional bcm_vlan_port_destroy\n");
                    return rv;
                }

                rv = bcm_vlan_port_destroy(unit, port_match_info_1.vlan_port_egress_default);
                if (rv != 0){
                    printf("Error, in another additional bcm_vlan_port_destroy\n");
                    return rv;
                }
            }
        }
    }


    /* Restore VLAN domain */
    for (j = 0 ; j < 4 ; j++){
        port = port_match_info_1.ports[j];
        if (port > 0){
            for (i = 0 ; i < nof_units ; i++){
                unit = units_ids[i];
                if (is_system_port_is_local(unit, port)) {
                    rv = bcm_port_class_set(unit, port, portClass, 0);
                    if (rv != 0){
                        printf("Error, in bcm_port_class_set\n");
                        return rv;
                    }
                }
            }
        }
    }

    for (i = 0 ; i < nof_units ; i++) {
        unit = units_ids[i];
        rv = vlan_translation_default_mode_destroy(unit);
        if (rv != 0) {
            printf("Error, in vlan_translation_default_mode_destroy\n");
            return rv;
        }
    }

    return rv;
}

/* create vlan_port (Logical interface identified by port-vlan) */
int
vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_port_match_t match,
    bcm_gport_t *gport,
    bcm_vlan_t in_vid, 
    bcm_vlan_t egr_vid,
    int vsi,
    int flags
    ) {

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = match;
    vp.port = port_id;
    if (match != BCM_VLAN_PORT_MATCH_NONE) {
       vp.match_vlan = in_vid;
    }
    vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : egr_vid; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.vsi = vsi; 
    vp.flags = flags; 
    vp.vlan_port_id = *gport;    
    
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    if (verbose) {
        printf("Unit %d: Done creating vlan-port.\n\tInitial match is <port,vid> -> <egr_vid> = <%d,%d> -> <%d>\n", unit, port_id, in_vid, egr_vid, vp.vlan_port_id);
    }
    *gport = vp.vlan_port_id;  
    return BCM_E_NONE;
}


/* Add a new match criteria to an existing gport. Packets with this <port, vid> combinations will be routed same as the other packets in the gport. */
int
port_match_add_remove(int unit, bcm_port_match_t match, uint32 flags, int match_vlan, bcm_gport_t match_port, bcm_gport_t gport, int add){
    bcm_port_match_info_t matchInfo;
    int rv;

    bcm_port_match_info_t_init(&matchInfo);
    matchInfo.match = match;
    matchInfo.flags = flags;
    matchInfo.match_vlan = match_vlan;  
    matchInfo.port = match_port;
    if (add){
        rv = bcm_port_match_add(unit, gport, &matchInfo);
        if (rv != 0){
            printf("Error, in bcm_port_match_add. Unit %d, match_port: %d, match_vid is: %d\n", unit, match_port, match_vlan);
            return rv;
        }
        printf("Unit %d: Added port match <port, vid> = <%d,%d> to gport 0x%x\n", unit, match_port, match_vlan, gport);
    } else {
        rv = bcm_port_match_delete(unit, gport, &matchInfo);
        if (rv != 0){
            printf("Error, in bcm_port_match_delete.\n");
            return rv;
        }
    }
    
    return rv;
}

int
port_match_init_vlan_translation(int unit, int is_ingress){
    int rv, j, port;
    int nof_ports = (is_ingress) ? 3 : 4;

    for (j = 0 ; j < nof_ports; j++) {
        port = port_match_info_1.ports[j];
        port_tpid_init(port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }
    }

    rv = vlan_translation_default_mode_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translation_default_mode_init\n");
        return rv;
    }
    return rv;
}
