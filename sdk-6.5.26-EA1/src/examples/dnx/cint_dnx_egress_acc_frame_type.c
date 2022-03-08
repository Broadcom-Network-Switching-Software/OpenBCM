/*
 * $Id: cint_dnx_egress_acc_frame_type.c
 */

/*
 * Example of acceptable frame type filter at ERPP
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/cint_dnx_egress_acc_frame_type.c
 * cint
 * egress_acceptable_frame_type_run(0,201,202,203,1,0);
 * exit;
 *
 * Expecting 1 packets, accepatable frame type
 *   tx 1 psrc=201 data=0x00000000000200000000009981000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x00000000000200000000009981000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Expecting 1 packets, accepatable frame type
 *   tx 1 psrc=201 data=0x00000000000300000000009981000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x00000000000300000000009981000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Expecting 1 packets via l2 bridge
 *   tx 1 psrc=201 data=0x000000000002000000000099ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 *   Data: 0x00000000000200000000009981000001ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20000000000000000000000000000000
 *
 * Expecting 0 packets, because of ERPP trap for egress accepatable frame type filter
 *   tx 1 psrc=201 data=0x000000000003000000000099ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */

/*
 * trap id value which indicated whether corresponding trap has been created or not
 */
int egress_erpp_trap_id = -1;
int egress_etpp_trap_id = -1;

/*
 * Create trap and set drop action profile for ERPP application trap type
 */
int cint_acc_frame_type_erpp_trap_create_and_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_config_t_init(&config);

    config.dest_port = BCM_GPORT_BLACK_HOLE;
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;

    /** Action-Profile of ERPP application traps is configured using bcmRxTrapEgUserDefine */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &egress_erpp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_create %d for bcmRxTrapEgUserDefine\n", rv);
        return rv;
    }

    /** Set action of trap */
    rv = bcm_rx_trap_set(unit, egress_erpp_trap_id, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_set %d", rv);
        return rv;
    }

    /** Encode trap_id, forward strength and snoop strength as gport */
    BCM_GPORT_TRAP_SET(trap_gport, egress_erpp_trap_id, 7, 0);

    /** Set the Trap-Action-Profile for application trap type bcmRxTrapEgDiscardFrameTypeFilter */
    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgDiscardFrameTypeFilter, trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_action_profile_set: Couldn't update gprot for  trap %d\n", trap);
        return rv;
    }
    return rv;
}

/*
 * destroy ERPP trap
 */
int cint_acc_frame_type_erpp_trap_destroy(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;
    int trap_id;

    /** Get the Trap-Action-Profile for trap of specific type */
    rv = bcm_rx_trap_action_profile_get(unit, trap_type, &trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_get %d\n", rv);
        return rv;
    }

    /** Clear the Trap-Action-Profile for trap of specific type */
    rv = bcm_rx_trap_action_profile_clear(unit, trap_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_clear %d\n", rv);
        return rv;
    }

    /** Decode trap_id from trap gport */
    trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport);

    /** Destroy trap */
    rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_destroy %d\n", rv);
        return rv;
    }
    egress_erpp_trap_id = -1;

    return rv;
}

/*
 * Set drop action profile for ETPP application trap type
 */
int cint_acc_frame_type_etpp_trap_actions_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;

    /*
     * Do not call create and set for pre-defined DROP filters.
     * We just assign the pre-defined DROP profile as trap ID(profile), which is HW created and exists.
     */
    BCM_GPORT_TRAP_SET(trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 7, 0);

    /** Set the Trap-Action-Profile for application trap type bcmRxTrapEgDiscardFrameTypeFilter */
    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxDiscardFrameTypeFilter, trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_action_profile_set: Couldn't update gprot for  trap %d\n", trap);
        return rv;
    }

    /** Indicate etpp trap is configured */
    egress_etpp_trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DROP;

    return rv;
}

/*
 * Clear action profile for ETPP application trap type
 */
int cint_acc_frame_type_etpp_trap_actions_destroy(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;

    /** Clear the Trap-Action-Profile for trap of specific type */
    rv = bcm_rx_trap_action_profile_clear(unit, trap_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_clear %d\n", rv);
        return rv;
    }
    egress_etpp_trap_id = -1;

    return rv;
}

/*
 * create and condfigure trap to drop packets of unacceptable frame type
 */
int set_trap_to_drop(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;

    if (trap_type == bcmRxTrapEgDiscardFrameTypeFilter)
    {
        rv = cint_acc_frame_type_erpp_trap_create_and_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_acc_frame_type_erpp_trap_create_and_set: %d\n", rv);
            return rv;
        }
    }
    else if (trap_type == bcmRxTrapEgTxDiscardFrameTypeFilter)
    {
        rv = cint_acc_frame_type_etpp_trap_actions_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_acc_frame_type_etpp_trap_actions_set: %d\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * destory trap to drop packets of unacceptable frame type
 */
int clear_trap_to_drop(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;

    if (trap_type == bcmRxTrapEgDiscardFrameTypeFilter)
    {
        rv = cint_acc_frame_type_erpp_trap_destroy(unit, trap_type);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_acc_frame_type_erpp_trap_destroy %d\n", rv);
            return rv;
        }
    }
    else if (trap_type == bcmRxTrapEgTxDiscardFrameTypeFilter)
    {
        rv = cint_acc_frame_type_etpp_trap_actions_destroy(unit, trap_type);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_acc_frame_type_etpp_trap_actions_destroy %d\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * 1. out_port_1: configure tpid class: accept all ethernet tags.
 *    out_port_2: configure tpid class: accept all ethernet tags.
 *              Except untags.
 * 2. use basic bridge to forward, add mac entries to out_port_1 and out_port_2
 * 3. configure acceptable frame type filter at egress without any trap
 * 4. send tag packet to out_port_1, packets should be received
 * 5. send tag packet to out_port_2, packets should be received
 * 6. send untag packet to out_port_1, packets should be received
 * 7. send untag packet to out_port_2, packets should be received
 * 8. configure trap for acceptable frame type at ERPP
 * 9. repeat 4~7, at step 7, packet should be discarded
 * 10. configure trap for acceptable frame type at ETPP
 * 11. repeat 4~7, at step 7, packet should be discarded
 */
int egress_acceptable_frame_type_run(
    int unit,
    int in_port,
    int out_port_1,
    int out_port_2,
    int enable_egress_erpp_trap,
    int enable_egress_etpp_trap)
{
    int rv = BCM_E_NONE;
    int vid = 20;

    bcm_port_tpid_class_t tpid_class;

    /** mac table variables */
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mac;
    int fid_untag = 1;
    int fid_tag = 20;

    /*
     * Add VLAN membership
     */
    rv = bcm_vlan_gport_add(unit, vid, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add vid\n");
        return rv;
    }

    /** discard untagged packets for both ingress and egress */
    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.port = out_port_2;
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tag_format_class_id = 0;
    tpid_class.flags = BCM_PORT_TPID_CLASS_DISCARD;
    tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set\n");
        return rv;
    }

    /** set erpp trap */
    if (enable_egress_erpp_trap)
    {
        /** if bcmRxTrapEgDiscardFrameTypeFilter has not been created, create it */
        if (egress_erpp_trap_id == -1)
        {
            set_trap_to_drop(unit, bcmRxTrapEgDiscardFrameTypeFilter);
        }
    }
    else
    {
        /** if bcmRxTrapEgDiscardFrameTypeFilter has been created, destory it */
        if (egress_erpp_trap_id != -1)
        {
            clear_trap_to_drop(unit, bcmRxTrapEgDiscardFrameTypeFilter);
        }
    }

    /** set etpp trap */
    if (enable_egress_etpp_trap)
    {
        /** if bcmRxTrapEgTxDiscardFrameTypeFilter has not been created, create it */
        if (egress_etpp_trap_id == -1)
        {
            set_trap_to_drop(unit, bcmRxTrapEgTxDiscardFrameTypeFilter);
        }
    }
    else
    {
        /** if bcmRxTrapEgTxDiscardFrameTypeFilter has been created, destory it */
        if (egress_etpp_trap_id != -1)
        {
            clear_trap_to_drop(unit, bcmRxTrapEgTxDiscardFrameTypeFilter);
        }
    }

    /*
     * add mact entries
     * fid      mac                port
     *   1    00:00:00:00:02       out_port_1
     *  20    00:00:00:00:02       out_port_1
     *   1    00:00:00:00:03       out_port_2
     *  20    00:00:00:00:03       out_port_2
     */
    mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
    mac[5] = 2;

    /* Add l2 address */
    bcm_l2_addr_t_init(&l2_addr, mac, fid_untag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = out_port_1;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_addr_add\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, mac, fid_tag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = out_port_1;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_addr_add\n");
        return rv;
    }

    mac[5] = 3;
    bcm_l2_addr_t_init(&l2_addr, mac, fid_untag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = out_port_2;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_addr_add\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, mac, fid_tag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = out_port_2;
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

int egress_acceptable_frame_type_clear(
    int unit,
    int out_port_1,
    int out_port_2)
{
    int rv = BCM_E_NONE;
    bcm_port_tpid_class_t tpid_class;

    /** remove the discard of untagged packets */
    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.port = out_port_2;
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tag_format_class_id = 0;
    tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set\n");
        return rv;
    }

    /** if bcmRxTrapEgDiscardFrameTypeFilter has been created, destory it */
    if (egress_erpp_trap_id != -1)
    {
        clear_trap_to_drop(unit, bcmRxTrapEgDiscardFrameTypeFilter);
    }
    /** if bcmRxTrapEgTxDiscardFrameTypeFilter has been created, destory it */
    if (egress_etpp_trap_id != -1)
    {
        clear_trap_to_drop(unit, bcmRxTrapEgTxDiscardFrameTypeFilter);
    }

    return rv;
}
