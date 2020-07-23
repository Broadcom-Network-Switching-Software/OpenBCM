/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_jer_fc_coe_config_example.c
 * 
 * Background:
 *     Jericho can map flow control indications in COE FC packet to in-band FC vector. 
 *     The FC vector will be processed similar to ILKN Rx OOB or ILKN RX in-band. 
 * 
 * Purpose: 
 *     Example of COE flow control(FC) configration. 
 * 
 * Service Model:
 *     coe pp port(COE VID)<-----vlan 100-----> Local port
 *          4         11                            20
 *          8         12                            20
 * 
 *       FC indication            Calendar index                             Related traffic
 *     PFC_0 with VID 11 => Calendar  7(3 + channel_id(in pp port 4)) => priority 0 traffic on port 4
 *     PFC_1 with VID 11 => Calendar  6(2 + channel_id(in pp port 4)) => priority 1 traffic on port 4
 *     PFC_0 with VID 12 => Calendar 11(3 + channel_id(in pp port 8)) => priority 0 traffic on port 4
 *     PFC_1 with VID 12 => Calendar 10(2 + channel_id(in pp port 8)) => priority 1 traffic on port 4
 * 
 * To set port extender type between COE and regular port.
 *   1. Add below SOC properties:
 *     custom_feature_map_port_extr_enabled.BCM88675=2
 *     ucode_port_4.BCM88375_A0=10GBase-R66.0:core_0.4
 *     ucode_port_8.BCM88375_A0=10GBase-R66.1:core_0.8
 *     ucode_port_20.BCM88375_A0=10GBase-R69:core_0.20
 *   2. Run below 
 *     BCM> cint examples/dpp/cint_port_extender_dynamic_switching.c
 *     BCM> cint
 *     cint> print port_extender_dynamic_swithing_coe_info_init(0, 4, 11, 4);
 *     cint> print port_extender_dynamic_swithing_coe_info_init(0, 8, 12, 4);
 *     cint> print port_extender_dynamic_swithing_coe_vlan_init(100, 20);
 *     cint> print port_extender_dynamic_swithing_coe_service(unit);
 *
 * To drop FC packet, run:
 *     BCM> cint examples/dpp/cint_jer_fc_coe_config_example.c
 *     BCM> cint
 *     cint> print cint_jer_coe_fc_packet_drop_set_example(0);
 * 
 * To enable COE PFC and set COE PFC packet format, run:
 *   1. Add below SOC properties to enable COE PFC:
 *     fc_intlkn_indication_invert.port1.oob_rx=1
 *     fc_oob_type.port1=4
 *     fc_oob_mode.port1=0x1
 *     fc_oob_calender_length.port1.rx=256
 *     fc_calendar_coe_mode=PFC
 *     fc_calendar_pause_resolution=1
 *     ext_voltage_mode_oob=3.3V
 *     bcm886xx_pdm_mode.BCM88375=1
 *     fc_oob_calender_rep_count=1
 *   2. Add below SOC properties to set COE PFC packet format:
 *     fc_coe_mac_address=01:80:C2:00:00:01
 *     fc_coe_ethertype=0x8808
 *     fc_coe_data_offset=4
 *
 * To map FC indication in COE PFC packet to stop a port priority traffic, run:
 *     BCM> cint examples/dpp/cint_jer_fc_coe_config_example.c
 *     BCM> cint
 *     cint> print cint_jer_test_coe_fc_pfc_rec_set_example(/*unit*/0, /*target_port*/4, /*oob_port*/0, /*oob_calendar_idx*/ 7, /*target_port*/4, /*target_cosq*/0);
 *     cint> print cint_jer_test_coe_fc_pfc_rec_set_example(/*unit*/0, /*target_port*/4, /*oob_port*/0, /*oob_calendar_idx*/ 6, /*target_port*/4, /*target_cosq*/1);
 *     cint> print cint_jer_test_coe_fc_pfc_rec_set_example(/*unit*/0, /*target_port*/8, /*oob_port*/0, /*oob_calendar_idx*/11, /*target_port*/8, /*target_cosq*/0);
 *     cint> print cint_jer_test_coe_fc_pfc_rec_set_example(/*unit*/0, /*target_port*/8, /*oob_port*/0, /*oob_calendar_idx*/10, /*target_port*/8, /*target_cosq*/1);
 */

/* Map FC indication in COE FC packet to stop a port traffic */
int cint_jer_test_coe_fc_port_rec_set_example(int unit, int is_oob, int is_coe, int calendar_port, int calendar_idx, int target_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port;
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config calendar fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;

    if ((is_oob == 1) && (is_coe == 1)) {        /* OOB COE flow control */
        BCM_GPORT_CONGESTION_COE_SET(fc_reception_port.port, calendar_port);
    } else if ((is_oob == 1) && (is_coe == 0)) { /* OOB flow control */
        BCM_GPORT_CONGESTION_OOB_SET(fc_reception_port.port, calendar_port);
    } else if ((is_oob == 0) && (is_coe == 1)) { /* Inband COE flow control */
        BCM_COSQ_GPORT_INBAND_COE_SET(fc_reception_port.port, calendar_port);
    } else {                                     /* Inband flow control */
        BCM_GPORT_LOCAL_SET(fc_reception_port.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */
    fc_target.cosq = -1;         /* -1 for port reaction point */
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("calenar fc reception configuration failed(%d) in cint_jer_test_coe_fc_port_rec_set_example\n", rv);
    }
    /* Config calendar fc } */

    return rv;
}

int cint_jer_test_coe_fc_port_rec_unset_example(int unit, int is_oob, int is_coe, int calendar_port, int calendar_idx, int target_port)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port;
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config calendar fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port, target_port) /*Can be local, or modport etc..*/;

    if ((is_oob == 1) && (is_coe == 1)) {        /* OOB COE flow control */
        BCM_GPORT_CONGESTION_COE_SET(fc_reception_port.port, calendar_port);
    } else if ((is_oob == 1) && (is_coe == 0)) { /* OOB flow control */
        BCM_GPORT_CONGESTION_OOB_SET(fc_reception_port.port, calendar_port);
    } else if ((is_oob == 0) && (is_coe == 1)) { /* Inband COE flow control */
        BCM_COSQ_GPORT_INBAND_COE_SET(fc_reception_port.port, calendar_port);
    } else {                                     /* Inband flow control */
        BCM_GPORT_LOCAL_SET(fc_reception_port.port, calendar_port);
    }

    /* setting flags */
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */
    fc_target.cosq=-1;           /* -1 for port reaction point */
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("calendar fc reception configuration failed in cint_jer_test_coe_fc_port_rec_unset_example\n");
    }
    /* Config calendar fc } */
    
    return rv;
}

/* Map FC indication in COE FC packet to stop a port priority traffic */
int cint_jer_test_coe_fc_pfc_rec_set_example(int unit, int is_oob, int calendar_port, int calendar_idx, int target_port, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config calendar fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception_port.port, calendar_port);
    }
    else {
        BCM_GPORT_LOCAL_SET(fc_reception_port.port, calendar_port);
    }
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("calendar fc reception configuration failed(%d) in cint_jer_test_coe_fc_pfc_rec_set_example\n", rv);
    }
    /* Config calendar fc } */

    return rv;
}

int cint_jer_test_coe_fc_pfc_rec_unset_example(int unit,int is_oob, int calendar_port, int calendar_idx, int target_port, int target_cosq)
{
    int rv = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t fc_reception_port; 
    bcm_cosq_fc_endpoint_t fc_target;

    /* Config calendar fc { */
    /* setting port */
    BCM_GPORT_LOCAL_SET(fc_target.port,target_port) /*Can be local, or modport etc..*/;
    if (is_oob) {
        BCM_GPORT_CONGESTION_SET(fc_reception_port.port, calendar_port);
    }
    else {
        BCM_GPORT_LOCAL_SET(fc_reception_port.port, calendar_port);
    }
    /* setting flags */    
    fc_target.flags = fc_reception_port.flags = 0;
    /* setting cosq */    
    fc_target.cosq = target_cosq;
    fc_reception_port.cosq = -1; /* cosq for fc_reception can be 0 or -1, but -1 is recommended */
    /* setting calender index */
    fc_reception_port.calender_index = calendar_idx;

    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception,&fc_reception_port,&fc_target);
    if (rv != BCM_E_NONE) {
        printf("calendar fc reception configuration failed in cint_jer_test_coe_fc_pfc_rec_unset_example\n");
    }
    /* Config calendar fc } */
    
    return rv;
}

/* Add PMF entry to drop COE FC packet */
int cint_jer_coe_fc_packet_drop_set_example(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    
    rv = bcm_field_group_create(unit, qset, 5, &group);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_field_group_create\n", rv);
    }
    
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_field_group_action_set\n", rv);
    }

    rv = bcm_field_entry_create(unit, group, &entry);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_field_entry_create\n", rv);
    }
    
    macData[0] = 0x01;
    macData[1] = 0x80;
    macData[2] = 0xC2;
    macData[3] = 0x00;
    macData[4] = 0x00;
    macData[5] = 0x01;
    macMask[0] = 0xFF;
    macMask[1] = 0xFF;
    macMask[2] = 0xFF;
    macMask[3] = 0xFF;
    macMask[4] = 0xFF;
    macMask[5] = 0xFF;
    rv = bcm_field_qualify_DstMac(unit, entry, macData, macMask);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_field_qualify_DstMac\n", rv);
    }

    rv = bcm_field_qualify_EtherType(unit, entry, 0x8808, 0xFFFF);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_field_qualify_EtherType\n", rv);
    }
    
    rv = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_field_action_add\n", rv);
    }

    rv = bcm_field_entry_install(unit, entry);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) in bcm_field_entry_install\n", rv);
    }
    
    return rv;
}

