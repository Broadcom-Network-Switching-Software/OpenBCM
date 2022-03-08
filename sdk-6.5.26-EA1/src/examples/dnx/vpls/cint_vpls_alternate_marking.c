/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_mpls_deep_stack.c Purpose: utility for MPLS deep encapsulation stack..
 */

/*
 * This cint is an example of Alternate Marking configuration (RFC321) of Ingress Node.
 * It contains L2 Packet forwarding into Vpls tunnel with Alternate Marking encapsulation.
 * This cint uses mpls and counter cint utilities and cint_field_alternate_marking.c that contains the
 * field configuration for alternate marking.
 *
 * Test Scenario (ingress node)
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/dnx/stat_pp/cint_stat_pp_config.c
 * cint ../../src/./examples/dnx/crps/cint_crps_db_utils.c
 * cint ../../src/./examples/dnx/crps/cint_crps_irpp_config.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/field/cint_field_alternate_marking.c
 * cint ../../src/./examples/dnx/mpls/cint_mpls_deep_stack_alternate_marking.c
 * cint ../../src/./examples/dnx/vpls/cint_vpls_alternate_marking.c
 * cint ../../src/./examples/dnx/vpls/cint_vpls_basic.c
 * cint
 * vpls_alternate_marking_encapsulation_example(0,200,201);
 *
 * cint_field_alternate_marking_period_change(0,0,0);
 * exit;
 *
 **** alternate marking, L=0, second packet = 0
 * tx 1 psrc=200 data=0x000c0002000000010203040591000005ffff00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 *  Should recieve 2 packets: forward copy + snoop copy. ToD timestamp in the snooped copy.
 * Next packet, counter should be >0, there will be no snoop copy.
 *
 ****** Switch to L=1
 * cint
 * cint_field_alternate_marking_period_change(0,0,1);
 * exit;
 *
 **** alternate marking, L=1, second packet = 0 *
 * tx 1 psrc=200 data=0x000c0002000000010203040591000005ffff00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 * Once again, should recieve 2 packets.
 *
 * Read L=0 counter:
 * read_counter_and_handle_eviction(unit,1,&counter)
 *
 * Time-of-Day in case D bit is on can come in one of 2 ways:
 * 1. Tail edit appended to the end of the packet which includes time of day
 *    On Jer2-B0 using this method only egress-TOD may be used.
 * 2.  64B of  mirror information appended to snooped packets.
 *      In this case the last 12 bytes will ingress-ToD (6B), egress-ToD (12B)
 *      Format of snooped packets in this case will be {FTMH+system+headers, 52B reserved, 6B ingress-ToD, 6B egress-TOD, network-headers}
 *              52B reserved includes original system headers. Existince of this stack is indicated by the FHEI of the first set of system headers (Usually trap code will be EgTxFieldSnoop1)
 *  Global variable tod_append_mode determines this.
 *  This applies to ingress node only.
 *  When set to 1, cint_field_IFA_datapath.c, cint_ifa.c cints must be loaded.
 *
 */

int vpls_am_special_label_valule = 10;
int vpls_am_flow_id = 0x1234;

int vpls_am_ioam_wo_l_encap_id = 0x7010;
int vpls_am_ioam_w_l_encap_id = 0x7012;

/** Add two encapsulation entries for IOAM Label, one with L bit, another without L bit */
int
vpls_alternate_marking_mpls_port_encapsulation_add (
    int unit,
    bcm_gport_t *am_mpls_port_l_off_id,
    bcm_gport_t *am_mpls_port_l_on_id)
{
    int rv;
    bcm_mpls_port_t mpls_port_egress;
    bcm_mpls_port_t mpls_port_egress_get;

    bcm_mpls_port_t_init(&mpls_port_egress);
    mpls_port_egress.flags = BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port_egress.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;

    /* qos info */
    mpls_port_egress.egress_label.ttl = 100;
    mpls_port_egress.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_port_egress.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    mpls_port_egress.egress_label.egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
    mpls_port_egress.egress_tunnel_label.exp = 5;

    /* ***** PWE with AM info (L bit off) **** */
    mpls_port_egress.egress_label.label = vpls_am_flow_id;
    mpls_port_egress.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING;
    mpls_port_egress.egress_label.encap_access = bcmEncapAccessTunnel3;
    mpls_port_egress.encap_id = vpls_am_ioam_wo_l_encap_id;
    /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
    BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_egress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_egress.encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.mpls_port_id);

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_port_add for secondary PWE\n");
        return rv;
    }
    *am_mpls_port_l_off_id = mpls_port_egress.mpls_port_id;

    bcm_mpls_port_t_init(&mpls_port_egress_get);
    mpls_port_egress_get.mpls_port_id = *am_mpls_port_l_off_id;
    print bcm_mpls_port_get(unit, 0, &mpls_port_egress_get);
    printf("Info of PWE with AM info (L bit off) id(%d):\n", *am_mpls_port_l_off_id);
    print mpls_port_egress_get;

    /* ***** PWE with AM info (L bit on)  **** */
    mpls_port_egress.egress_label.label = vpls_am_flow_id;
    mpls_port_egress.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING | BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET;
    mpls_port_egress.egress_label.encap_access = bcmEncapAccessTunnel3;
    mpls_port_egress.encap_id = vpls_am_ioam_w_l_encap_id;
    /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
    BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_egress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_egress.encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.mpls_port_id);

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_port_add for primary PWE\n");
        return rv;
    }
    *am_mpls_port_l_on_id = mpls_port_egress.mpls_port_id;

    bcm_mpls_port_t_init(&mpls_port_egress_get);
    mpls_port_egress_get.mpls_port_id = *am_mpls_port_l_on_id;
    print bcm_mpls_port_get(unit, 0, &mpls_port_egress_get);

    printf("Info of PWE with AM info (L bit on) id(%d):\n", *am_mpls_port_l_on_id);
    print mpls_port_egress_get;

    return rv;
}

/** Example of Alternate Marking over VPLS */
int
vpls_alternate_marking_encapsulation_example (
    int unit,
    int in_port,
    int out_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t vlan_port_id_to_l_off = 0x1238;
    bcm_gport_t vlan_port_id_to_l_on = 0x1234;
    int native_ac_encap_id_to_l_off;
    int native_ac_encap_id_to_l_on;
    bcm_gport_t am_mpls_port_l_off_id;
    bcm_gport_t am_mpls_port_l_on_id;
    bcm_gport_t gport_tunnel;
    bcm_mac_t pwe_mac       = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};

    encap_access_pwe = bcmEncapAccessTunnel2;
    encap_access_mpls_tunnel = bcmEncapAccessTunnel4;

    rv = vpls_basic_example(unit,in_port,out_port, 1, 1);
    if (rv) {
        printf("Error, vpls_basic_example\n");
        return rv;
    }

    /* Set Alternate Marking special label switch control */
    if (verbose >= 1)
    {
        printf("Set alternate marking encapsulation special label switch control\n");
    }
    rv = bcm_switch_control_set(unit, bcmSwitchMplsAlternateMarkingSpecialLabel, vpls_am_special_label_valule);
    if (rv) {
        printf("Error, bcm_switch_control_set bcmSwitchMplsAlternateMarkingSpecialLabel\n");
        return rv;
    }

    rv = vpls_alternate_marking_mpls_port_encapsulation_add(unit, am_mpls_port_l_off_id, am_mpls_port_l_on_id);
    if (rv) {
        printf("Error, vpls_alternate_marking_mpls_port_encapsulation_add\n");
        return rv;
    }

    /*
     * Add Alternate Marking encapsulation: LOSS bit off
     */
    gport_tunnel = am_mpls_port_l_off_id;
    cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_no_loss = vpls_am_ioam_wo_l_encap_id;

    /*
     * Add alternate marking encapsulation: LOSS bit on
     */
    gport_tunnel = am_mpls_port_l_on_id;
    cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss = vpls_am_ioam_w_l_encap_id;
    printf("cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss:%d\n/n", cint_mpls_deep_stack_alternate_marking_params.alternate_marking_if_loss);

    /*
     * Add CRPS configuration
     */
    rv = mpls_deep_stack_alternate_marking_crps(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_deep_stack_alternate_marking_crps\n");
        return rv;
    }

    /*
     * Configure PMF rules
     */
    g_cint_field_am_ing_ent.service_type = 1; /* vpls */
    sal_memcpy(g_cint_field_am_ing_ent.DstMac, pwe_mac, 6);
    g_cint_field_am_ing_ent.idx_of_outlif_to_update = 1;
    rv = mpls_deep_stack_alternate_marking_ingress_field(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_deep_stack_alternate_marking_ingress_field\n");
        return rv;
    }

    rv = mpls_deep_stack_alternate_marking_snoop(unit, cint_field_am_egress_snoop_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in mpls_deep_stack_alternate_marking_field\n");
        return rv;
    }


    if (tod_append_mode ==2)
    {
        /*
         * Time of day gotten through mirror additional info.
         * This requires initializing through a cint from cint_ifa.c
         * Only when the in_port is a NIF port.
         */
        if (in_port_is_nif)
        {
            BCM_IF_ERROR_RETURN( cint_instru_config_nif_rx_timestamp(unit,in_port,0));
        }
    }
    else if (tod_append_mode ==1)
    {
        /*
        * Timestamp information when D bit is set is gotten through tail-edit meta data.
        * Switch id parameter is the node ID field.
        */
        BCM_IF_ERROR_RETURN(cint_instru_ipt_main(unit, 0xab, 0));
    }
    else
    {
        print "ERROR: tod_append_mode must be either 1 (use tail-edit) or 2 (use mirror-additional-info)";
        return  123;
    }

    return rv;
}

