/*
 * Cint demonstrates SA-discard feature in MACT.
 * MACT entry include SA-discard indication.
 * when lookup SA lookup in MACT result entry with this indication set,
 * then packet is dropped/trapped/forwarded depends on in-port configuration.
 *
 * this feature require:
 *  - ingress learning.
 *
 *   demonstrate SA-Discard feature,
 *   Brdiged packet with SA exists in MACT with SA_DSICARD will be dropped.
 *
 * Parameters:
 *    - in_port_1: incomming port, send packet from this port with SA = sa_discard will be dropped.
 *    - in_port_2: incomming port, send packet from this port with SA = sa_discard will be trapped to CPU.
 *      must be != in_port_1, for test for one port set in_port_2 = -1.
 *    - dest_port: egress port.
 *
 *  Traffic1:
 *   in-port: in_port_1
 *   SA: 00:00:07:00:01:01
 *   DA: 00:00:00:00:00:ab
 * --> packet is dropped
 *
 *  Traffic2:
 *   in-port: in_port_2
 *   SA: 00:00:07:00:01:01
 *   DA: 00:00:00:00:00:ab
 * --> packet is trapped to CPU
 *
 *  Traffic3:
 *   in-port: *
 *   SA: 00:00:07:00:01:02, or any MAC different than 00:00:07:00:01:01
 *   DA: 00:00:00:00:00:ab
 * --> packet is forwarded to out_port
 *
 * send L2 packet with SA
 * - dest_port destination port.
 * - nof_entries: number of entries to add to MACT
 * - add entries to VLAN 1 mac-address is inc over mac_address
 */


/* how to run:
 * load relevant Cints.
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../src/./examples/dnx/cint_dnx_l2_learning.c
 * cint ../../src/./examples/dnx/cint_dnx_l2_mact_sa_discard.c
 * cint
 * l2_sa_discard_example_run_with_dvapi(0,200,201,202,203);
 * exit;
 */

/* print level, high results more prints */
int print_level = 2;
/* MAC addresses used in the cint exmaple */
/* DA for static forwarding */
bcm_mac_t da  =          {0x00, 0x00, 0x00, 0x00, 0x00, 0xab};
/* packets with this SA to be dropped/trapped according to in-port config*/
bcm_mac_t sa_discard  = {0x00, 0x00, 0x07, 0x00, 0x01, 0x01};
/* packets with this SA to be forwarded as SA-discard is not set in MAC */
bcm_mac_t sa_pass  = {0x00, 0x00, 0x07, 0x00, 0x01, 0x02};
/* packets with SA different than above to be forwarded as not exist in MACT*/
/* trap ids to control the configuration of
 *  bcmRxTrapL2DiscardMacsaDrop: by default drop packet.
 *  bcmRxTrapL2DiscardMacsaTrap: by default trap packet to CPU.
 */
int drop_trap_id = 0;
int redirect_trap_id = 0;

/*
 * example
 *   demonstrate SA-Discard feature,
 *   Brdiged packet with SA exists in MACT with SA_DSICARD will be dropped.
 * Parameters:
 *    - in_port_1: incomming port, send packet from this port with SA = sa_discard will be dropped.
 *    - in_port_2: incomming port, send packet from this port with SA = sa_discard will be trapped to CPU.
 *      must be != in_port_1, for test for one port set in_port_2 = -1.
 *    - dest_port: egress port.
 *
 *  Traffic1:
 *   in-port: in_port_1
 *   SA: 00:00:07:00:01:01
 *   DA: 00:00:00:00:00:ab
 * --> packet is dropped
 *
 *  Traffic2:
 *   in-port: in_port_2
 *   SA: 00:00:07:00:01:01
 *   DA: 00:00:00:00:00:ab
 * --> packet is trapped to CPU
 *
 *  Traffic3:
 *   in-port: *
 *   SA: 00:00:07:00:01:02, or any MAC different than 00:00:07:00:01:01
 *   DA: 00:00:00:00:00:ab
 * --> packet is forwarded to out_port
 */

 /*  send L2 packet with SA
 * - dest_port destination port.
 * - nof_entries: number of entries to add to MACT
 * - add entries to VLAN 1 mac-address is inc over mac_address
 */
int l2_sa_discard_example(int unit, int in_port_1, int in_port_2, int dest_port, int trap_dist_port) {
    int rv = 0;
    bcm_l2_addr_t l2addr;

    /* port has to be different */
    if(in_port_1 == in_port_2){
        printf("port_1 and port_2 has to in-equal\n");
        return BCM_E_PARAM;
    }

    /* configure traps for drop/trap */
    rv = l2_sa_discard_config_traps(unit, trap_dist_port);
    if (rv != BCM_E_NONE) {
         printf("Error, l2_sa_discard_config_traps\n");
         return rv;
    }

    /* for port_1 drop packet for SA-discard */
    rv = bcm_port_control_set(unit, in_port_1, bcmPortControlDiscardMacSaAction, BCM_PORT_CONTROL_DISCARD_MACSA_DROP);
    if (rv != BCM_E_NONE) {
         printf("Error, bcm_port_control_set 1\n");
         return rv;
    }

   /* for port_2 trap packet for SA-discard */
    if(in_port_2 != -1){
        rv = bcm_port_control_set(unit, in_port_2, bcmPortControlDiscardMacSaAction, BCM_PORT_CONTROL_DISCARD_MACSA_TRAP);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set 2\n");
            return rv;
        }
    }

    /* add L2 entry with SA-discard set */
    bcm_l2_addr_t_init(&l2addr, sa_discard, 1);
    l2addr.port = dest_port;
    l2addr.vid = 1;
    l2addr.flags = BCM_L2_STATIC|BCM_L2_DISCARD_SRC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    /* add L2 entry with SA-discard unset */
    bcm_l2_addr_t_init(&l2addr, sa_pass, 1);
    l2addr.port = dest_port;
    l2addr.vid = 1;
    l2addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

   /* add L2 for DA lookup */
    bcm_l2_addr_t_init(&l2addr, da, 1);
    l2addr.port = dest_port;
    l2addr.vid = 1;
    l2addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

int l2_sa_discard_config_traps(int unit, int dest_port) {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;

    /* change trap to redirect dest-port */
    if(drop_trap_id == 0){
        rv = bcm_rx_trap_type_create(unit,0,bcmRxTrapL2DiscardMacsaTrap,&drop_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in trap create, trap id $drop_trap_id \n");
            return rv;
        }
    }

    /*change dest port for trap */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 5;
    config.dest_port=dest_port;
    rv = bcm_rx_trap_set(unit,drop_trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    /* set drop to drop packet, this is also the driver default if filter is enabled*/
    if(redirect_trap_id == 0){
        rv = bcm_rx_trap_type_create(unit,0,bcmRxTrapL2DiscardMacsaDrop,&redirect_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in trap create, trap id $redirect_trap_id  2\n");
            return rv;
        }
    }

    /*change dest port for trap */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 5;
    config.dest_port=BCM_GPORT_BLACK_HOLE;
    rv = bcm_rx_trap_set(unit,redirect_trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set 2n");
        return rv;
    }

    return rv;
}

int l2_sa_discard_example_run_with_dvapi(int unit, int in_port_1, int in_port_2, int out_port, int trap_dest_port) {
    int rv = 0;

    rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_switch_control_set \n");
        return rv;
    }

    /* run SA-discard example */
    rv = l2_sa_discard_example(unit,in_port_1,in_port_2,out_port,trap_dest_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in l2_sa_discard_example \n");
        return rv;
    }

    return rv;
}

/*
 * This function is used for cleaup. It destroys both traps created in  l2_sa_discard_config_traps
 *  (drop_trap_id and redirect_trap_id).
 */

int l2_sa_discard_destroy_traps(int unit) {
    int rv;

    rv = bcm_rx_trap_type_destroy(unit, drop_trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_type_destroy\n");
        return rv;
    }

    rv = bcm_rx_trap_type_destroy(unit, redirect_trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_type_destroy\n");
        return rv;
    }

    return rv;
}
