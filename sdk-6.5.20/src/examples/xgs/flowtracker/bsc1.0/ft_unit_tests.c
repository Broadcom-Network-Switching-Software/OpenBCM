/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

cint_reset();

int unit = 0;

#include "ft_collector_config.c"

int i = 0;
int j=0;
bcm_flowtracker_ipfix_observation_domain_t ipfix_observation_domain_id = 10;
bcm_flowtracker_group_info_t flow_group_info;
bcm_flowtracker_tracking_param_info_t info[50];
int rv = 0, rv2=0;
bcm_flowtracker_group_stat_t group_stats;
bcm_flowtracker_collector_copy_info_t cinfo;
/* UPDATE port based on send_packet() */
int port = 49;
int test_number = 0;
int sw_managed_group = 0;

bcm_flowtracker_collector_copy_info_t_init(&cinfo);

int max_groups = 1;
int max_checks = 1;
int max_ipv6_checks = 1;
int max_vxlan_checks = 2;
int max_alu32 =2;
int max_load16 = 0;
int max_load8=0;
int alu_ts_0 = 0;
int alu_ts_1 = 0;
int new_learn_ts = 0;
int flow_start_ts = 0;
int flow_end_ts = 0;
int second_check = 0;
int options=0;
int fuzzy_flow_check_data = 0;
/* Group : flow_group2 */
int flow_group_id2[1000];
int flow_group_id3; /* IPv6 */
int flow_group_id4; /* IPv4-Vxlan */
bcm_field_entry_t ftfp_entry[1000];
bcm_flowtracker_check_t check_id[10000];
bcm_flowtracker_check_t second_check_id[10000];
bcm_flowtracker_check_t ipv6_check_id[10000];
bcm_flowtracker_check_t vxlan_check_id[10000];
bcm_flowtracker_check_info_t check_info;

int collector_id = 0;

flow_group_id3 = -1;
flow_group_id4 = -1;
for (j=0; j<1000; j++) {

    flow_group_id2[j] = -1;

        check_id[j] = -1;
        second_check_id[j] = -1;
        ipv6_check_id[j] = -1;
        vxlan_check_id[j] = -1;
}


bshell(0, "debug bcm flowtracker  info");
bshell(0, "debug bcm fp debug");

bcm_flowtracker_group_stat_t_init(&group_stats);

int print_traverse_check(int unit,  bcm_flowtracker_check_t check_id,
    bcm_flowtracker_check_info_t *check_info, void *user_data)
{
    printf("[CINT] :: \ncheck_id = 0x%x\n", check_id);
    printf("[CINT] :: min_value = %d\n", check_info->min_value);
    printf("[CINT] :: min_value = %d\n", check_info->max_value);

    return 1;
}


int set_get_time(unsigned int sec, unsigned int nsecs)
{

    bcm_time_tod_t tod;

    bcm_time_tod_t_init(&tod);
    unsigned int stages;

    stages = BCM_TIME_STAGE_ALL;

    tod.time_format = bcmTimeFormatPTP;
    tod.ts.nanoseconds = nsecs;
    COMPILER_64_SET(tod.ts.seconds, 0, sec);
    COMPILER_64_SET(tod.ts_adjustment_counter_ns, 0, 0);

    print bcm_time_tod_set(unit, stages, &tod);
    bshell(0, "d chg EGR_TS_UTC_CONVERSION");
    bshell(0, "d chg BSC_TS_UTC_CONVERSION");

    bcm_time_tod_t_init(&tod);
    print bcm_time_tod_get(unit, stages, &tod);

    print tod;

    return 0;
}

printf("[CINT] :: *******************************\n");
printf("[CINT] :: Before any configuration : Dumping the memory. \n");
bshell(0, "salprofile");
printf("[CINT] :: *******************************\n");


rv = 0;
rv2 = 0;


/*****************************************************************************/
/**** FTFP Configurations ****************************************************/
/*****************************************************************************/

int ftfp_group1 = -1; /* IPv4 */
bcm_field_presel_t ftfp_presel_id1[3];
int ftfp_group2 = -1; /* IPv6 */
bcm_field_presel_t ftfp_presel_id2[3];
int ftfp_group3 = -1; /* IPv4-Vxlan */
bcm_field_presel_t ftfp_presel_id3[3];
int ftfp_group4 = -1; /* IPv4 with Dos Attack */
bcm_field_presel_t ftfp_presel_id4[3];

for (i = 0; i < 3; i++) {
    ftfp_presel_id1[i] = -1;
    ftfp_presel_id2[i] = -1;
    ftfp_presel_id3[i] = -1;
    ftfp_presel_id4[i] = -1;
}

/* Create configurations IPv4 FTFP Group */
int cc_ftfp_group_ipv4()
{
    bcm_field_entry_t presel_eid;
    bcm_field_group_config_t ftfp_group_config;

    if (ftfp_group1 != -1) {
        return 0;
    }

    /* 1. Presel to select Unicast L2 and Unicast IPv4 packet formats */
    print bcm_field_presel_create(unit, &ftfp_presel_id1[0]);
    presel_eid = ftfp_presel_id1[0] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
    print bcm_field_qualify_PktDstAddrType(unit, presel_eid, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
    print bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv4NoOpts);
    printf ("FTFP Presel Created : %d\n", ftfp_presel_id1[0]);

    /* 2. Presel to select Multicast L2 and unicast IPv4 Packet formats */
    print bcm_field_presel_create(unit, &ftfp_presel_id1[1]);
    presel_eid = ftfp_presel_id1[1] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
    print bcm_field_qualify_PktDstAddrType(unit, presel_eid, BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
    print bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv4NoOpts);
    printf ("FTFP Presel Created : %d\n", ftfp_presel_id1[1]);

    /* 3. Presel to select Unicast L2 and Multicast IPv4 Packet formats */
    print bcm_field_presel_create(unit, &ftfp_presel_id1[2]);
    presel_eid = ftfp_presel_id1[2] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
    print bcm_field_qualify_PktDstAddrType(unit, presel_eid, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP);
    print bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv4NoOpts);
    printf ("FTFP Presel Created : %d\n", ftfp_presel_id1[2]);

    /* FTFP Group for IPV4 packets Identify
     * - Unicast L2 and Unicast IPV4
     * - Multicast L2 and Unicast IPV4
     * - Unicast L2 and Multicast IPV4
     * - both TCP and UDP Protocols.
     */
    bcm_field_group_config_t_init(&ftfp_group_config);
    ftfp_group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id1[0]);
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id1[1]);
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id1[2]);
    BCM_FIELD_QSET_INIT(ftfp_group_config.qset);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyStageIngressFlowtracker);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInPort);
    ftfp_group_config.mode = bcmFieldGroupModeAuto;
    print bcm_field_group_config_create(unit, &ftfp_group_config);
    ftfp_group1 = ftfp_group_config.group;
    printf("[CINT] :: ftfp group created : %d\n", ftfp_group1);

    return 0;
}

/* Delete configurations for IPv4 FTFP Group */
int dc_ftfp_group_ipv4()
{
    /* Destroy FTFP Group */
    if (ftfp_group1 != -1) {
        print bcm_field_group_destroy(unit, ftfp_group1);
        ftfp_group1 = -1;
    }

    /* Destroy Presel */
    for (i = 0; i < 3; i++) {
        if (ftfp_presel_id1[i] != -1) {
            print bcm_field_presel_destroy(unit, ftfp_presel_id1[i]);
            ftfp_presel_id1[i] = -1;
        }
    }

    return 0;
}

/* Create configurations for IPv6 FTFP Group */
int cc_ftfp_group_ipv6()
{
    bcm_field_entry_t presel_eid;
    bcm_field_group_config_t ftfp_group_config;

    if (ftfp_group2 != -1) {
        return 0;
    }

    /* 1. Presel to select Unicast L2 and Unicast IPv6 Packet formats */
    print bcm_field_presel_create(unit, &ftfp_presel_id2[0]);
    presel_eid = ftfp_presel_id2[0] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
    print bcm_field_qualify_PktDstAddrType(unit, presel_eid, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
    print bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv6NoExtHdr);
    printf("[CINT] :: FTFP presel created Id : %d\n", ftfp_presel_id2[0]);

    /* 2. Presel to select Multicast L2 and Unicast IPv6 Packet formats */
    print bcm_field_presel_create(unit, &ftfp_presel_id2[1]);
    presel_eid = ftfp_presel_id2[1] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
    print bcm_field_qualify_PktDstAddrType(unit, presel_eid, BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
    print bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv6NoExtHdr);
    printf("[CINT] :: FTFP presel created Id : %d\n", ftfp_presel_id2[1]);


    /* 3. Presel to select Unicast L2 and Multicast IPv6 Packet formats */
    print bcm_field_presel_create(unit, &ftfp_presel_id2[2]);
    presel_eid = ftfp_presel_id2[2] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
    print bcm_field_qualify_PktDstAddrType(unit, presel_eid, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP);
    print bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv6NoExtHdr);
    printf("[CINT] :: FTFP presel created Id : %d\n", ftfp_presel_id2[2]);

    /* FTFP Group Create */
    bcm_field_group_config_t_init(&ftfp_group_config);
    ftfp_group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id2[0]);
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id2[1]);
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id2[2]);
    BCM_FIELD_QSET_INIT(ftfp_group_config.qset);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyStageIngressFlowtracker);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyIp6PktNextHeader);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyIp6PktHopLimit);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyIp6PktTrafficClass);
    ftfp_group_config.mode = bcmFieldGroupModeAuto;
    print bcm_field_group_config_create(unit, &ftfp_group_config);
    ftfp_group2 = ftfp_group_config.group;
    printf("[CINT] :: FTFP Group created with Id : %d\n", ftfp_group2);

    return 0;
}

/* Delete configurations for IPv6 FTFP Group */
int dc_ftfp_group_ipv6()
{
    /* Destroy FTFP Group */
    if (ftfp_group2 != -1) {
        print bcm_field_group_destroy(unit, ftfp_group2);
        ftfp_group2 = -1;
    }

    /* Destroy Presel */
    for (i = 0; i < 3; i++) {
        if (ftfp_presel_id2[i] != -1) {
            print bcm_field_presel_destroy(unit, ftfp_presel_id2[i]);
            ftfp_presel_id2[i] = -1;
        }
    }

    return 0;
}

/* Create configurations for IPv4-Vxlan FTFP Group */
int cc_ftfp_group_ipv4_vxlan()
{
    bcm_field_entry_t presel_eid;
    bcm_field_group_config_t ftfp_group_config;

    if (ftfp_group3 != -1) {
        return 0;
    }

    /* Presel */
    print bcm_field_presel_create(unit, &ftfp_presel_id3[0]);
    presel_eid = ftfp_presel_id3[0] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_PktType(unit, presel_eid, bcmFieldPktTypeIp4Vxlan);
    printf("[CINT] :: FTFP presel created Id : %d\n", ftfp_presel_id3[0]);

    /* Configure FTFP group */
    bcm_field_group_config_t_init(&ftfp_group_config);
    ftfp_group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id3[0]);
    BCM_FIELD_QSET_INIT(ftfp_group_config.qset);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyStageIngressFlowtracker);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVxlanNetworkId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVxlanHeaderBits8_31);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVxlanHeaderBits56_63);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVxlanFlags);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerSrcIp);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerIpProtocol);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerTtl);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerL4DstPort);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerL4SrcPort);
    ftfp_group_config.mode = bcmFieldGroupModeAuto;
    print bcm_field_group_config_create(unit, &ftfp_group_config);
    ftfp_group3 = ftfp_group_config.group;

    printf("[CINT] :: VXLAN ftfp group created : %d\n", ftfp_group3);
    return 0;
}

/* Delete configurations for IPv4-Vxlan FTFP Group */
int dc_ftfp_group_ipv4_vxlan()
{
    /* Destroy FTFP Group */
    if (ftfp_group3 != -1) {
        print bcm_field_group_destroy(unit, ftfp_group3);
        ftfp_group3 = -1;
    }

    /* Destroy Presel */
    for (i = 0; i < 3; i++) {
        if (ftfp_presel_id3[i] != -1) {
            print bcm_field_presel_destroy(unit, ftfp_presel_id3[i]);
            ftfp_presel_id3[i] = -1;
        }
    }

    return 0;
}

/* Create Dos Attack configuration. */
int cc_ftfp_group_ipv4_dos()
{
    bcm_field_entry_t presel_eid;
    bcm_field_group_config_t ftfp_group_config;

    if (ftfp_group4 != -1) {
        return 0;
    }

    /* 1. Presel to select Unicast L2 and Unicast IPv4 packet formats */
    print bcm_field_presel_create(unit, &ftfp_presel_id4[0]);
    presel_eid = ftfp_presel_id4[0] | BCM_FIELD_QUALIFY_PRESEL;
    print bcm_field_qualify_Stage(unit, presel_eid, bcmFieldStageIngressFlowtracker);
    print bcm_field_qualify_L4Ports(unit, presel_eid, 1, 1);
    print bcm_field_qualify_DosAttack(unit, presel_eid, 1, 1);
    print bcm_field_qualify_PktDstAddrType(unit, presel_eid, BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC | BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP);
    print bcm_field_qualify_IpType(unit, presel_eid, bcmFieldIpTypeIpv4NoOpts);

    printf ("FTFP Presel Created : %d\n", ftfp_presel_id4[0]);

    /* FTFP Group for IPV4 packets Identify
     * - Unicast L2 and Unicast IPV4
     * - Multicast L2 and Unicast IPV4
     * - Unicast L2 and Multicast IPV4
     * - both TCP and UDP Protocols.
     */

    bcm_field_hintid_t hint_id;
    print bcm_field_hints_create(unit, &hint_id);

    bcm_field_hint_t hint;
    bcm_field_hint_t_init(&hint);
    hint.hint_type = bcmFieldHintTypeExtraction;
    hint.qual = bcmFieldQualifyDosAttackEvents;
    hint.dosattack_event_flags = BCM_FIELD_DOSATTACK_MACSA_DA_SAME | BCM_FIELD_DOSATTACK_TCPPORTS_EQUAL;

    /* Associating the above configured hints to hint id */
    print bcm_field_hints_add(unit, hint_id, &hint);

    bcm_field_group_config_t_init(&ftfp_group_config);
    ftfp_group_config.hintid = hint_id;
    ftfp_group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    BCM_FIELD_PRESEL_ADD(ftfp_group_config.preselset, ftfp_presel_id4[0]);
    BCM_FIELD_QSET_INIT(ftfp_group_config.qset);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyStageIngressFlowtracker);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInnerVlanId);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(ftfp_group_config.qset, bcmFieldQualifyDosAttackEvents);
    ftfp_group_config.mode = bcmFieldGroupModeAuto;
    print bcm_field_group_config_create(unit, &ftfp_group_config);
    ftfp_group1 = ftfp_group_config.group;


    printf("Dos attack ftfp group created : %d\n", ftfp_group1);

    return 0;
}

/* Delete configurations for IPv4 with Dos FTFP Group */
int dc_ftfp_group_ipv4_dos()
{
    /* Destroy FTFP Group */
    if (ftfp_group4 != -1) {
        print bcm_field_group_destroy(unit, ftfp_group4);
        ftfp_group4 = -1;
    }

    /* Destroy Presel */
    for (i = 0; i < 3; i++) {
        if (ftfp_presel_id4[i] != -1) {
            print bcm_field_presel_destroy(unit, ftfp_presel_id4[i]);
            ftfp_presel_id4[i] = -1;
        }
    }

    return 0;
}

/******* FTFP Entry *********/

/* Create configurations for IPv4 FTFP Entries */
int cc_ftfp_entries_ipv4()
{
    int vlans = 3;
    if (test_number > 60) {
        vlans = 2;
    }

    for (j=0; j<max_groups; j++) {

        if (flow_group_id2[j] == -1) {
            continue;
        }

         printf("[CINT] :: j = %d, FT_Group = %d ftfp entry 1_3 = %d\n", j, flow_group_id2[j], ftfp_entry[j]);

/* traffic : traffic_L2_QQ_IP_TCP_DATA__U_L2_x_U_IP__V30_46_TCP_SYN.t
 * 22 33 44 55 66 77 10 20 30 40 50 60 81 00 00 1E 81 00 00 2E 08 00 45 08 00 38 00 00 40 00 40 06 49 98 0A 0A 01 01 0A 0A 01 02 33 44 55 66 00 A1 A2 A3 00 B1 B2 B3 50 02 11 F5 F8 C3 00 00 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5 F5
 */
        print bcm_field_entry_create(unit, ftfp_group1, &ftfp_entry[j]);
        print bcm_field_action_add(unit, ftfp_entry[j], bcmFieldActionFlowtrackerGroupId, flow_group_id2[j], 0);
        print bcm_field_qualify_IpProtocol(unit, ftfp_entry[j], 6, 0xff); /* TCP */
        print bcm_field_qualify_InPort(unit, ftfp_entry[j], port, 0xff); /* xe0 */
        print bcm_field_qualify_OuterVlanId(unit, ftfp_entry[j], 30, 0xfff); /* outervlan=30 */
        print bcm_field_qualify_VlanFormat(unit, ftfp_entry[j], vlans, 0xf); /* vlans based on different packets.*/
        rv2 =  bcm_field_entry_install(unit, ftfp_entry[j]);
        if (rv2) {
             printf("[CINT] ::  \n :::::   ALERT ::::: \n Entry install failed at Check Create failed at i = %d, j = %d, checks : %d, rv2 = %d\n", i, j, max_checks, rv2);
             break;
        }

         printf("[CINT] :: j = %d, Group = %d ftfp entry = %d\n", j, flow_group_id2[j], ftfp_entry[j]);
    }

    return 0;
}

/* Create configurations for IPv6 FTFP Entries */
int cc_ftfp_entries_ipv6()
{
    j = 0;
    print bcm_field_entry_create(unit, ftfp_group2, &ftfp_entry[j]);
    print bcm_field_action_add(unit, ftfp_entry[j], bcmFieldActionFlowtrackerGroupId, flow_group_id3, 0);
    print bcm_field_qualify_Ip6PktNextHeader(unit, ftfp_entry[j], 6, 0xff); /* TCP*/
    print bcm_field_qualify_VlanFormat(unit, ftfp_entry[j], 0, 0xf); /* vlanformat=untagged */
    print bcm_field_qualify_InPort(unit, ftfp_entry[j], port, 0xff); /* xe0 */
    rv2 = bcm_field_entry_install(unit, ftfp_entry[j]);


    if (rv2) {
        printf("[CINT] ::  \n :::::   ALERT ::::: \n Entry install failed at Check Create failed at i = %d, j = %d, checks : %d, rv2 = %d\n", i, j, max_checks, rv2);
        break;
    }

    return 0;
}

/* Create configurations for IPv4-Vxlan FTFP Entries */
int cc_ftfp_entries_ipv4_vxlan()
{
    j = 0;
    print bcm_field_entry_create(unit, ftfp_group3, &ftfp_entry[j]);
    print bcm_field_action_add(unit, ftfp_entry[j], bcmFieldActionFlowtrackerGroupId, flow_group_id4, 0);
    print bcm_field_qualify_IpProtocol(unit, ftfp_entry[j], 17, 0xff); /* UDP */
    print bcm_field_qualify_VxlanNetworkId(unit, ftfp_entry[j], 0x345667, 0xffffff);

    rv2 = bcm_field_entry_install(unit, ftfp_entry[j]);
    if (rv2) {
        printf("[CINT] ::  \n :::::   ALERT ::::: \n Entry install failed at Check Create failed at i = %d, j = %d, checks : %d, rv2 = %d\n", i, j, max_checks, rv2);
        break;
    }

    return 0;
}

/* Delete configurations for FTFP Entries */
int dc_ftfp_entries()
{
    for (j=0; j<max_groups; j++) {

        if ((flow_group_id2[j] != -1) || (flow_group_id3 != -1) || (flow_group_id4 != -1)) {

            if (ftfp_entry[j] != 0) {
                printf("[CINT] :: j = %d, Destroying ftfp entry = %d\n", j, ftfp_entry[j]);
                print bcm_field_entry_destroy(unit, ftfp_entry[j]);
            }
            ftfp_entry[j] = 0;
        }
    }

    return 0;
}

/**** FTFP Configurations Ends ***********************************************/


/*****************************************************************************/
/**** FT Configurations ******************************************************/
/*****************************************************************************/

int cc_ipv4(int groups, bcm_flowtracker_tracking_param_type_t param,
            int alu32, int checks, int load16, int load8,
            bcm_flowtracker_tracking_param_type_t check_param,
            int min, int max, bcm_flowtracker_check_operation_t opr, int offset)
{

    options = 0;


    int num =0;/* Number of tracking params.*/
    max_groups = groups;
    max_checks = checks;
    max_alu32 = alu32;
    max_load16 = load16;
    max_load8=  load8;


    for (j=0; j<max_groups; j++) {
        print "Create FT Group";
        bcm_flowtracker_group_info_t_init(&flow_group_info);
        if (sw_managed_group == 1) {
            flow_group_info.group_flags = BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY;
        }
        print bcm_flowtracker_group_create (unit, 0, &flow_group_id2[j], &flow_group_info);
        print bcm_flowtracker_group_flow_limit_set(0, flow_group_id2[j], 1);

        for (i=0; i<50; i++) { info[i].flags = 0; info[i].param = 0; }

        num = 0;
        info[num].flags = 1;
        info[num].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
        num++;
        info[num].flags = 1;
        info[num].param = bcmFlowtrackerTrackingParamTypeDstIPv4;
        num++;
        info[num].flags = 1;
        info[num].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
        num++;
        info[num].flags = 1;
        info[num].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
        num++;
        info[num].flags = 1;
        info[num].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
        num++;

        if (param) {
            info[num].flags = 0;
            info[num].param = param;
            num++;
        }

        if (new_learn_ts) {
            info[num].flags = 0;
            info[num].param = bcmFlowtrackerTrackingParamTypeTimestampNewLearn;
            num++;
        }

        if (flow_start_ts) {
            info[num].flags = 0;
            info[num].param = bcmFlowtrackerTrackingParamTypeTimestampFlowStart;
            num++;
        }

        if (flow_end_ts) {
            info[num].flags = 0;
            info[num].param = bcmFlowtrackerTrackingParamTypeTimestampFlowEnd;
            num++;
        }

        if (alu_ts_0) {
            info[num].flags = 0;
            info[num].param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
            num++;
        }

        if (alu_ts_1) {
            info[num].flags = 0;
            info[num].param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
            num++;
        }

        for (i=0; i<max_alu32; i++) {
            info[num].param = bcmFlowtrackerTrackingParamTypePktCount;
            num++;
        }

        for (i=0; i<max_load8; i++) {
            info[num].flags = 0;
            info[num].param = bcmFlowtrackerTrackingParamTypeTTL;
            num++;
        }


        for (i=0; i<max_load16; i++) {
            info[num].flags = 0;
            if (i == 1) {
                info[num].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
            } else {
                info[num].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            }
            num++;
        }



        printf("[CINT] :: Tracking params (count = %d) are being added to Group with Id = %d \n", num, flow_group_id2[j]);
        print bcm_flowtracker_group_tracking_params_set(unit, flow_group_id2[j], num, info);


        bcm_flowtracker_collector_copy_info_t_init(&cinfo);

        /*cinfo.num_pkts_skip_for_next_sample=2;*/
        cinfo.num_pkts_initial_samples=3;

        printf("[CINT] ::  INITIAL Setting the collector copy profile profile entry. in group = %d, \n", flow_group_id2[j]);
        /* Update the same group with new age. */
        /*print bcm_flowtracker_group_collector_copy_info_set(0, flow_group_id2[j], cinfo);*/

        bshell(0, "d chg BSC_KG_GROUP_TABLE 0 4");
        bshell(0, "d chg BSC_DG_GROUP_TABLE 0 4");

        printf("[CINT] :: SET :  info.num_pkts_skip_for_next_sample = %d,  info.num_pkts_initial_samples = %d\n", cinfo.num_pkts_skip_for_next_sample, cinfo.num_pkts_initial_samples);
        printf("[CINT] ::  Setting the collector copy profile profile entry.\n");
        bshell(0, "d chg BSC_DG_GROUP_COPY_TO_COLLECTOR_PROFILE");


        printf("[CINT] :: ============== Now modify Group counters tp 0xffffffff(overflow) and print. ==============\n");
        /*bshell(0, "modify BSC_KG_FLOW_EXCEED_COUNTER 0 4 FLOW_COUNT=0xffffffff");
          bshell(0, "modify BSC_KG_FLOW_AGE_OUT_COUNTER 0 4 FLOW_COUNT=0xffffffff");
          bshell(0, "modify BSC_KG_FLOW_LEARNED_COUNTER 0 1 FLOW_COUNT=0xffffffff");*/
        bshell(0, "d chg BSC_KG_FLOW_EXCEED_COUNTER  ");
        bshell(0, "d chg BSC_KG_FLOW_AGE_OUT_COUNTER ");
        bshell(0, "d chg BSC_KG_FLOW_LEARNED_COUNTER");
        for (i=0; i<max_checks; i++) {
            /* Local_offset. */
            int l_off = 0;

            bcm_flowtracker_check_info_t_init(&check_info);
            check_info.param = check_param;
            if (offset == -1) {
                l_off = 0;
            } else {
                l_off = offset;
            }
            check_info.min_value = min + i*l_off;
            check_info.max_value = max + i*l_off;
            check_info.operation = opr;
            if (alu_ts_0) {
                check_info.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
            }

            check_info.flags |= BCM_FLOWTRACKER_CHECK_DATA_CLEAR_ON_EXPORT;
            check_info.flags |= BCM_FLOWTRACKER_CHECK_EXPORT_MODE_THRESHOLD;

            printf("[CINT] ::  Check Create information getting printed. \n");
            print check_info;
            printf("[CINT] ::  Check Create information getting printed. Done \n");

            rv =  bcm_flowtracker_check_create(unit, 0, check_info, &check_id[i+(j*max_checks)]);

            if (rv) {
                printf("[CINT] ::  Check Create failed at index : %d\n", i+(j*max_checks));
                break;
            }

            printf("[CINT] ::  flowtracker check is created with Id = 0x%x \n", check_id[i+(j*max_checks)]);

            if (second_check) {
                bcm_flowtracker_check_info_t_init(&check_info);
                check_info.param = bcmFlowtrackerTrackingParamTypeIPProtocol;
                check_info.min_value = 1;
                check_info.max_value = 100+i+j;
                /*check_info.operation = 1+i;*/
                if (alu_ts_1) {
                    check_info.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2;
                }

                check_info.operation = bcmFlowtrackerCheckOpSmaller;
                options = BCM_FLOWTRACKER_CHECK_SECOND;
                check_info.primary_check_id = check_id[i+(j*max_checks)];

                rv =  bcm_flowtracker_check_create(unit, options, check_info, &second_check_id[i+(j*max_checks)]);

                if (rv) {
                    printf(" Secondary check creation failed. ar primary index : %x\n", check_id[i+(j*max_checks)]);
                    break;
                } else {

                    printf(" Secondary check created with index = : %x\n", second_check_id[i+(j*max_checks)]);
                }

            }


            bcm_flowtracker_check_action_info_t c_info;
            bcm_flowtracker_check_action_info_t_init(&c_info);
            bcm_flowtracker_check_export_info_t e_info;
            bcm_flowtracker_check_export_info_t_init(&e_info);


            c_info.param = bcmFlowtrackerTrackingParamTypeIPProtocol;
            if (offset == -1) {
                c_info.param = bcmFlowtrackerTrackingParamTypeTcpWindowSize;
                c_info.action = bcmFlowtrackerCheckActionUpdateValue;
            } else {
                c_info.action = bcmFlowtrackerCheckActionCounterIncr;
            }

            print bcm_flowtracker_check_action_info_set(unit, check_id[i+(j*max_checks)], c_info);

            if (test_number == 19) {
                e_info.export_check_threshold = 4597;
                e_info.operation = bcmFlowtrackerCheckOpGreaterEqual;
            } else {
                e_info.export_check_threshold = 2;
                e_info.operation = bcmFlowtrackerCheckOpGreaterEqual;
            }
            print bcm_flowtracker_check_export_info_set(unit, check_id[i+(j*max_checks)], e_info);

            print bcm_flowtracker_group_check_add(unit, flow_group_id2[j], check_id[i+(max_checks*j)]);
            printf("[CINT] ::  flowtracker check 0x%x is added to group %d \n", check_id[i+(j*max_checks)], flow_group_id2[j]);

            /*
               printf("[CINT] ::  Now trying to delete this check id(0x%x) when it is associated with group \n", check_id[i+(j*max_checks)]);
               print bcm_flowtracker_check_destroy(unit, check_id[i+(max_checks*j)]);
               printf("[CINT] ::  Now trying to delete All the flowcheckers \n");
               print bcm_flowtracker_check_destroy_all(unit);
               printf("[CINT] ::  Now trying to traverse through all the checks. \n");
               print bcm_flowtracker_check_traverse(unit, (&print_traverse_check), NULL);
             */

            bcm_flowtracker_check_info_t_init(&check_info);
            printf("[CINT] :: Get created check information. \n");
            rv =  bcm_flowtracker_check_get(unit, check_id[i+(j*max_checks)], &check_info);

            if (rv) {
                printf("[CINT] ::  Check get failed at check index : %d\n", check_id[i+(j*max_checks)]);
                break;
            }
            printf("[CINT] ::  Check Get information getting printed. \n");
            print check_info;
            printf("[CINT] ::  Check Get information getting printed. Done \n");

        }
        /*
           if (rv) {
           break;
           }
         */
    }

}

rv = rv2 == 0;

printf("[CINT] :: *******************************");
printf("[CINT] :: *******************************\n");
printf("[CINT] :: *******************************");
printf("[CINT] :: *******************************\n");
printf("[CINT] :: After Configuring configuration : Dumping the memory. \n");
bshell(0, "salprofile");
printf("[CINT] :: *******************************");
printf("[CINT] :: *******************************\n");
printf("[CINT] :: *******************************\n");
printf("[CINT] :: *******************************\n");

int dc_ft_group_flows_clear()
{
    printf("[CINT] ::::::::::::::: Clear Active flows ::::::::::::::::: \n");
    for (j=0;j<max_groups;j++) {

        if (flow_group_id2[j] == -1) {
            continue;
        }

        print bcm_flowtracker_group_clear(0, flow_group_id2[j], BCM_FLOWTRACKER_GROUP_CLEAR_ALL);
    }

    if (flow_group_id3 != -1) {
        print bcm_flowtracker_group_clear(0, flow_group_id3, BCM_FLOWTRACKER_GROUP_CLEAR_ALL);
    }

    if (flow_group_id4 != -1) {
        print bcm_flowtracker_group_clear(0, flow_group_id4, BCM_FLOWTRACKER_GROUP_CLEAR_ALL);
    }

    return 0;
}

int dc_ft_group_ipv4()
{
    printf("[CINT] :: \n :::::::::::::::: Destroying the state now IPv4 :::::::::::::::: \n");
    for (j=0; j<max_groups; j++) {

        if (flow_group_id2[j] == -1) {
            continue;
        }

        printf("[CINT] :: ***************** J = %d ***************** \n  Deleting group = %d \n",j,  flow_group_id2[j]);
        for (i=0; i<max_checks; i++) {

            if (check_id[i+(j*max_checks)] == -1) {
                continue;
            }

            printf("[CINT] :: i = %d , Deleting check id 0x%x from FT Group = %d\n", i,  check_id[i+(j*max_checks)], flow_group_id2[j]);
            print bcm_flowtracker_group_check_delete(unit, flow_group_id2[j], check_id[i+(j*max_checks)]);

            if ((second_check_id[i+(j*max_checks)]) != -1) {
                printf("[CINT] :: i = %d , Deleting second check id with number 0x%x \n", i,  second_check_id[i+(j*max_checks)]);
                print bcm_flowtracker_group_check_delete(unit, flow_group_id2[j], second_check_id[i+(j*max_checks)]);
                rv =  bcm_flowtracker_check_destroy(unit, second_check_id[i+(j*max_checks)]);
                if (!rv) {
                    printf("[CINT] :: i = %d, Failed to delete second check id 0x%x \n", i,  second_check_id[i+(j*max_checks)]);
                } else {
                    second_check_id[i+(j*max_checks)] = -1;
                }
            }
            printf("[CINT] :: Destroying check with id 0x%x \n", check_id[i+(j*max_checks)]);
            rv = bcm_flowtracker_check_destroy(unit, check_id[i+(j*max_checks)]);
            if (rv) {
                printf("[CINT] :: i = %d, Failed to delete check id 0x%x rv=%d\n", i, check_id[i+(j*max_checks)], rv);
            } else {
                check_id[i+(j*max_checks)] = -1;
            }
        }

        print bcm_flowtracker_group_flow_limit_set(0, flow_group_id2[j], 0);
        printf("[CINT] :: ***************** J = %d ***************** \n  Deleting group = %d \n",j, flow_group_id2[j]);
        rv = bcm_flowtracker_group_destroy (unit, flow_group_id2[j]);
        if (rv) {
            printf("[CINT] :: Failed to destroy j = %d FT Group = %d \n", j, flow_group_id2[j]);
        } else {
            flow_group_id2[j] = -1;
        }
    }

    return 0;
}

int cc_ipv6()
{

    options=0;
    int num =0;/* Number of tracking params.*/
    bcm_flowtracker_group_info_t_init(&flow_group_info);
    print bcm_flowtracker_group_create (unit, 0, &flow_group_id3, &flow_group_info);
    print bcm_flowtracker_group_flow_limit_set(0, flow_group_id3, 10);

    for (i=0; i<20; i++) { info[i].flags = 0; info[i].param = 0; }


    for (i=0; i<20; i++) { info[i].flags = 0; info[i].param = 0; }

    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeSrcIPv6;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeDstIPv6;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    num++;
    /*
       info[num].flags = 0;
       info[num].param = bcmFlowtrackerTrackingParamTypeHopLimit;
       num++;

       info[num].flags = 0;
       info[num].param = bcmFlowtrackerTrackingParamTypeNextHeader;
       num++;
     */
    if (alu_ts_1) {
        info[num].flags = 0;
        info[num].param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2;
        num++;
    }

    if (new_learn_ts) {
        info[num].flags = 0;
        info[num].param = bcmFlowtrackerTrackingParamTypeTimestampNewLearn;
        num++;
    }

    if (alu_ts_0) {
        info[num].flags = 0;
        info[num].param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
        num++;
    }

    for (i=0; i<max_alu32; i++) {
        info[num].flags = 0;
        info[num].param = bcmFlowtrackerTrackingParamTypePktCount;
        num++;
    }


    j =0;
    print bcm_flowtracker_group_tracking_params_set(unit, flow_group_id3, num, &info);

    printf("[CINT] ::  FT Group with Id = %d, num_tracking_parameters = %d \n", flow_group_id3, num);

    for (i=0; i<max_ipv6_checks; i++) {
        bcm_flowtracker_check_action_info_t c_info;
        bcm_flowtracker_check_action_info_t_init(&c_info);
        bcm_flowtracker_check_export_info_t e_info;
        bcm_flowtracker_check_export_info_t_init(&e_info);
        bcm_flowtracker_check_info_t_init(&check_info);

        if (i == 0) {
            check_info.param = bcmFlowtrackerTrackingParamTypeHopLimit;
            c_info.param = bcmFlowtrackerTrackingParamTypeHopLimit;
        } else {
            check_info.param = bcmFlowtrackerTrackingParamTypeNextHeader;
            c_info.param = bcmFlowtrackerTrackingParamTypeNextHeader;
        }

        if (alu_ts_0) {
            check_info.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
        }

        check_info.min_value = 1+j+i;
        check_info.max_value = 2+i+j;
        check_info.operation = bcmFlowtrackerCheckOpInRange;
        rv =  bcm_flowtracker_check_create(unit, 0, check_info, &ipv6_check_id[i+(j*max_ipv6_checks)]);

        if (rv) {
            printf("[CINT] ::  Check Create failed at index : %d\n", i+(j*max_ipv6_checks));
            break;
        }


        if (second_check) {
            bcm_flowtracker_check_info_t_init(&check_info);
            check_info.param = bcmFlowtrackerTrackingParamTypeIPProtocol;
            check_info.min_value = 1;
            check_info.max_value = 100+i+j;
            /*check_info.operation = 1+i;*/
            if (alu_ts_1) {
                check_info.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT2;
            }
            check_info.operation = bcmFlowtrackerCheckOpSmaller;
            options = BCM_FLOWTRACKER_CHECK_SECOND;
            check_info.primary_check_id = ipv6_check_id[i+(j*max_ipv6_checks)];

            rv =  bcm_flowtracker_check_create(unit, options, check_info, &second_check_id[i+(j*max_ipv6_checks)]);

            if (rv) {
                printf(" Secondary check creation failed. ar primary index : %x\n", check_id[i+(j*max_ipv6_checks)]);
                break;
            } else {

                printf(" Secondary check created with index = : %x\n", second_check_id[i+(j*max_ipv6_checks)]);
            }
        }



        c_info.action = bcmFlowtrackerCheckActionCounterIncr;

        print bcm_flowtracker_check_action_info_set(unit, ipv6_check_id[i+(j*max_ipv6_checks)], c_info);

        e_info.export_check_threshold = 1;
        e_info.operation = bcmFlowtrackerCheckOpGreaterEqual;
        print bcm_flowtracker_check_export_info_set(unit, ipv6_check_id[i+(j*max_ipv6_checks)], e_info);

        print bcm_flowtracker_group_check_add(unit, flow_group_id3, ipv6_check_id[i+(max_ipv6_checks*j)]);

    }

    printf("[CINT] :: FT Group Created = %d\n", flow_group_id3);

    return 0;

}

int dc_ft_group_ipv6()
{
    if (flow_group_id3 == -1) {
        return 0;
    }

    printf("[CINT] :: \n :::::::::::::::: Destroying the state now IPv6 :::::::::::::::: \n");

    j = 0;
    printf("[CINT] :: ***************** J = %d ***************** \n  Deleting group = %d \n",j,  flow_group_id3);
    for (i=0; i<max_ipv6_checks; i++) {

        if (ipv6_check_id[i+(j*max_ipv6_checks)] == -1) {
            continue;
        }

        printf("[CINT] :: i = %d , Deleting check id with number 0x%x \n", i,  ipv6_check_id[i+(j*max_ipv6_checks)]);
        print bcm_flowtracker_group_check_delete(unit, flow_group_id3, ipv6_check_id[i+(j*max_ipv6_checks)]);

        if ((second_check_id[i+(j*max_ipv6_checks)]) != -1) {
            printf("[CINT] :: i = %d , Deleting second check id with number 0x%x \n", i,  second_check_id[i+(j*max_ipv6_checks)]);
            print bcm_flowtracker_group_check_delete(unit, flow_group_id3, second_check_id[i+(j*max_ipv6_checks)]);
            rv =  bcm_flowtracker_check_destroy(unit, second_check_id[i+(j*max_ipv6_checks)]);
            if (rv) {
                printf("[CINT] :: i = %d , Failed to delete Second check id 0x%x \n", i,  second_check_id[i+(j*max_ipv6_checks)]);
            } else {
                second_check_id[i+(j*max_ipv6_checks)] = -1;
            }
        }
        printf("[CINT] :: Destroying check with id 0x%x \n", ipv6_check_id[i+(j*max_ipv6_checks)]);
        rv = bcm_flowtracker_check_destroy(unit, ipv6_check_id[i+(j*max_ipv6_checks)]);
        if (rv) {
            printf("[CINT] :: i = %d , Failed to delete check id 0x%x \n", i,  ipv6_check_id[i+(j*max_ipv6_checks)]);
        } else {
            ipv6_check_id[i+(j*max_ipv6_checks)] = -1;
        }

    }

    print bcm_flowtracker_group_flow_limit_set(0, flow_group_id3, 0);
    printf("[CINT] :: ***************** J = %d ***************** \n  Deleting group = %d \n",j,  flow_group_id3);
    rv =  bcm_flowtracker_group_destroy (unit, flow_group_id3);
    if (rv) {
        printf("[CINT] :: Failed to destroy j = %d FT Group = %d \n", j, flow_group_id3);
    } else {
        flow_group_id3 = -1;
    }

    return 0;
}

int cc_vxlan()
{

    /* Group : flow_group4 */
    int num =0;
    int max_alu32 = 1;

    bcm_flowtracker_group_info_t_init(&flow_group_info);
    print bcm_flowtracker_group_create (unit, 0, &flow_group_id4, &flow_group_info);
    print bcm_flowtracker_group_flow_limit_set(0, flow_group_id4, 10);
    printf("[CINT] ::  Tracking params are being added to Group with Id = %d \n", flow_group_id4);

    for (i=0; i<20; i++) { info[i].flags = 0; info[i].param = 0; }

    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeDstIPv4;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
    num++;
    info[num].flags = 1;
    info[num].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
    num++;

    if (new_learn_ts) {
        info[num].flags = 0;
        info[num].param = bcmFlowtrackerTrackingParamTypeTimestampNewLearn;
        num++;
    }

    if (alu_ts_0) {
        info[num].flags = 0;
        info[num].param = bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1;
        num++;
    }

    for (i=0; i<max_alu32; i++) {
        info[num].flags = 0;
        info[num].param = bcmFlowtrackerTrackingParamTypePktCount;
        num++;
    }

    print bcm_flowtracker_group_tracking_params_set(unit, flow_group_id4, num, &info);
    j = 0;

    for (i=0; i<max_vxlan_checks; i++) {

        bcm_flowtracker_check_info_t_init(&check_info);
        if (i == 0) {
            check_info.param = bcmFlowtrackerTrackingParamTypeVxlanNetworkId;
            check_info.min_value = 0x345550;
            check_info.max_value = 0x345770;

        } else {

            check_info.param = bcmFlowtrackerTrackingParamTypeVxlanNetworkId;
            check_info.min_value = 0x365660;
            check_info.max_value = 0x365670;
        }
        check_info.operation = bcmFlowtrackerCheckOpInRange;
        if (alu_ts_0) {
            check_info.flags = BCM_FLOWTRACKER_CHECK_TIMESTAMP_CHECK_EVENT1;
        }

        rv =  bcm_flowtracker_check_create(unit, 0, check_info, &vxlan_check_id[i+(j*max_vxlan_checks)]);

        if (rv) {
            printf("[CINT] ::  Check Create failed at index : %d\n", i+(j*max_vxlan_checks));
            break;
        }

        printf("[CINT] ::  flowtracker check is created with Id = 0x%x \n", vxlan_check_id[i+(j*max_vxlan_checks)]);

        bcm_flowtracker_check_action_info_t c_info;
        bcm_flowtracker_check_action_info_t_init(&c_info);
        bcm_flowtracker_check_export_info_t e_info;
        bcm_flowtracker_check_export_info_t_init(&e_info);


        c_info.param = bcmFlowtrackerTrackingParamTypeNone;
        c_info.action = bcmFlowtrackerCheckActionCounterIncr;

        print bcm_flowtracker_check_action_info_set(unit, vxlan_check_id[i+(j*max_vxlan_checks)], c_info);
        /*
           e_info.export_check_threshold = 1;
           e_info.operation = bcmFlowtrackerCheckOpGreater;
           print bcm_flowtracker_check_export_info_set(unit, vxlan_check_id[i+(j*max_vxlan_checks)], e_info);
         */
        print bcm_flowtracker_group_check_add(unit, flow_group_id4, vxlan_check_id[i+(max_vxlan_checks*j)]);

    }

    bshell(0, "modreg vxlan_control UDP_DEST_PORT=8472");

    return 0;
}

int dc_ft_group_ipv4_vxlan()
{
    if (flow_group_id4 == -1) {
        return 0;
    }
    printf("[CINT] :: \n :::::::::::::::: Destroying the state now IPv4-Vxlan :::::::::::::::: \n");

    j = 0;
    printf("[CINT] :: ***************** J = %d ***************** \n  Deleting group = %d \n",j,  flow_group_id4);
    for (i=0; i<max_vxlan_checks; i++) {

        if (vxlan_check_id[i+(j*max_vxlan_checks)] == -1) {
            continue;
        }

        printf("[CINT] :: i = %d , Deleting check id with number 0x%x \n", i, vxlan_check_id[i+(j*max_vxlan_checks)]);
        print bcm_flowtracker_group_check_delete(unit, flow_group_id4, vxlan_check_id[i+(j*max_vxlan_checks)]);

        printf("[CINT] :: Destroying check with id 0x%x \n", vxlan_check_id[i+(j*max_vxlan_checks)]);
        rv = bcm_flowtracker_check_destroy(unit, vxlan_check_id[i+(j*max_vxlan_checks)]);

        if (rv) {
            printf("\n[CINT] :: Failed to delete check id with number 0x%x \n", i, vxlan_check_id[i+(j*max_vxlan_checks)]);
        } else {
            vxlan_check_id[i+(j*max_vxlan_checks)] = -1;
        }

    }

    print bcm_flowtracker_group_flow_limit_set(0, flow_group_id4, 0);
    printf("[CINT] :: ***************** J = %d ***************** \n  Deleting group = %d \n",j,  flow_group_id4);
    rv = bcm_flowtracker_group_destroy (unit, flow_group_id4);
    if (rv) {
        printf("[CINT] :: Failed to destroy j = %d FT Group = %d \n", j, flow_group_id4);
    } else {
        flow_group_id4 = -1;
    }

    return 0;
}

int dos_ipv4()
{

    max_checks = 1;

    for (j=0; j<max_groups; j++) {
        bcm_flowtracker_group_info_t_init(&flow_group_info);
        print bcm_flowtracker_group_create (unit, 0, &flow_group_id2[j], &flow_group_info);
        print bcm_flowtracker_group_flow_limit_set(0, flow_group_id2[j], 1);
        printf("***************** J = %d ***************** \n  Tracking params are being added to Group with Id = %d \n",j,  flow_group_id2[j]);

        for (i=0; i<7; i++) { info[i].flags = 0; info[i].param = 0; }

        info[0].flags = 1;
        info[0].param = bcmFlowtrackerTrackingParamTypeSrcIPv4;
        info[1].flags = 1;
        info[1].param = bcmFlowtrackerTrackingParamTypeDstIPv4;
        info[2].flags = 1;
        info[2].param = bcmFlowtrackerTrackingParamTypeIPProtocol;

        info[3].flags = 0;
        info[3].param = bcmFlowtrackerTrackingParamTypeDosAttack;

        for (i=0; i<max_alu32; i++) {
            info[4+i].flags = 0;
            info[4+i].param = bcmFlowtrackerTrackingParamTypePktCount;
        }

        for (i=0; i<max_load8; i++) {
            info[4+max_alu32+i].flags = 0;
            info[4+max_alu32+i].param = bcmFlowtrackerTrackingParamTypeIPProtocol;
        }


        for (i=0; i<max_load16; i++) {
            info[4+max_alu32+max_load8+i].flags = 0;
            if (i == 1) {
                info[4+max_alu32+max_load8+i].param = bcmFlowtrackerTrackingParamTypeL4DstPort;
            } else {
                info[4+max_alu32+max_load8+i].param = bcmFlowtrackerTrackingParamTypeL4SrcPort;
            }
        }
        print bcm_flowtracker_group_tracking_params_set(unit, flow_group_id2[j], 4+max_alu32+max_load8+max_load16, &info);


        for (i=0; i<max_checks; i++) {
            bcm_flowtracker_check_action_info_t c_info;
            bcm_flowtracker_check_action_info_t_init(&c_info);
            bcm_flowtracker_check_export_info_t e_info;
            bcm_flowtracker_check_export_info_t_init(&e_info);

            bcm_flowtracker_check_info_t_init(&check_info);
            check_info.param = bcmFlowtrackerTrackingParamTypeDosAttack;
            check_info.max_value = 0;
            if (test_number == 61) {
                check_info.min_value = 0x00c00001;
                check_info.min_value = 0x00c00000;
                check_info.max_value = 0x00ffffff;
                check_info.operation = bcmFlowtrackerCheckOpInRange;
                c_info.action = bcmFlowtrackerCheckActionCounterIncr;
                e_info.export_check_threshold = 1;
                e_info.operation = bcmFlowtrackerCheckOpGreaterEqual;
            } else {
                check_info.min_value = 0x000100c0;
                check_info.min_value = 0;
                check_info.min_value = 0x00c00000;
                check_info.operation = bcmFlowtrackerCheckOpEqual;
                check_info.operation = bcmFlowtrackerCheckOpNotEqual;

                c_info.action = bcmFlowtrackerCheckActionUpdateValue;
            }

            rv =  bcm_flowtracker_check_create(unit, 0, check_info, &check_id[i+(j*max_checks)]);

            if (rv) {
                printf(" Check Create failed at index : %d\n", i+(j*max_checks));
                break;
            }

            printf(" flowtracker check is created with Id = 0x%x \n", check_id[i+(j*max_checks)]);

            c_info.param = bcmFlowtrackerTrackingParamTypeDosAttack;

            print bcm_flowtracker_check_action_info_set(unit, check_id[i+(j*max_checks)], c_info);

            print bcm_flowtracker_check_export_info_set(unit, check_id[i+(j*max_checks)], e_info);

            print bcm_flowtracker_group_check_add(unit, flow_group_id2[j], check_id[i+(max_checks*j)]);
        }
    }
}

/**** FT Group configurations Ends ********************************************/

/*****************************************************************************/
/**** FT Collector Configurations ********************************************/
/*****************************************************************************/

int cc_ft_collector()
{
    collector_id = ut_fte_collector_create(1);
}

int cc_ft_collector_add_ipv4()
{
    for (j=0; j<max_groups; j++) {

        if (flow_group_id2[j] == -1) {
            continue;
        }


        printf("[CINT] :: Attach j = %d, Group = %d Collector = %d\n", j, flow_group_id2[j], collector_id);
        rv =  ut_fte_collector_add(flow_group_id2[j], collector_id);
        if (rv) {
            printf("[CINT] ::  \n :::::   ALERT ::::: \n Template Validate / Collector add Failed for Group at j = %d, rv = %d\n", j, rv);
            break;
        }
    }
}

int cc_ft_collector_add_ipv6()
{
    rv = ut_fte_collector_add(flow_group_id3, collector_id);
}

int cc_ft_collector_add_ipv4_vxlan()
{
    rv = ut_fte_collector_add(flow_group_id4, collector_id);
}

int dc_ft_collector()
{
    for (j=0; j<max_groups; j++) {

        if (flow_group_id2[j] == -1) {
            continue;
        }

        rv2 = ut_fte_collector_destroy(flow_group_id2[j], collector_id);
        if (rv2 != BCM_E_NONE) {
            printf("\n FAILED to Destroy Collector, j=%d, group=%d, col=%d, rv=%d\n",j, flow_group_id2[j], collector_id, rv2);
        }
    }

    if (flow_group_id3 != -1) {
        rv2 = ut_fte_collector_destroy(flow_group_id3, collector_id);
        if (rv2 != BCM_E_NONE) {
            printf("\n FAILED to Destroy Collector, j=%d, group=%d, col=%d, rv=%d\n",j, flow_group_id3, collector_id, rv2);
        }
    }

    if (flow_group_id4 != -1) {
        rv2 = ut_fte_collector_destroy(flow_group_id4, collector_id);
        if (rv2 != BCM_E_NONE) {
            printf("\n FAILED to Destroy Collector, group=%d, col=%d, rv=%d\n",j, flow_group_id4, collector_id, rv2);
        }
    }

    return rv;
}


/**** FT Collectors Configurations Ends **************************************/

/**** FT Group Controls ******************************************************/

int stats(bcm_flowtracker_group_t id)
{
    bcm_flowtracker_group_stat_t_init(&group_stats);
    print bcm_flowtracker_group_stat_get(unit, id, &group_stats);
    printf("[CINT] :: ////////////////////////////////////////////////////////////\n");
    printf("[CINT] :: Printing Group stats for Group Id : = %d\n", id);
    print group_stats;
    printf("[CINT] :: ////////////////////////////////////////////////////////////\n");
}


int set_direction(bcm_flowtracker_group_t id, int direction)
{

    printf("[CINT] :: Setting direciotn : = %d\n", direction);
    print bcm_flowtracker_group_control_set(0, id, bcmFlowtrackerGroupControlFlowDirection, direction);
    return 0;
}

int set_do_not_ft(bcm_flowtracker_group_t id, int enable)
{

    printf("[CINT] :: Setting Do not FT : = %d\n", enable);
    print bcm_flowtracker_group_control_set(0, id, bcmFlowtrackerGroupControlFlowtrackerEnable, enable);
    return 0;
}


int set_new_learn(bcm_flowtracker_group_t id, int enable)
{

    printf("[CINT] :: Setting New Learn : = %d\n", enable);
    print bcm_flowtracker_group_control_set(0, id, bcmFlowtrackerGroupControlNewLearnEnable, enable);
    return 0;
}

bcm_flowtracker_cpu_notification_info_t notification_info;

bcm_flowtracker_cpu_notification_info_t_init(&notification_info);

int set_not(bcm_flowtracker_group_t id, int notification)
{

    BCM_FLOWTRACKER_CPU_NOTIFICATION_SET(notification_info, notification);
    bcm_flowtracker_group_cpu_notification_set(unit, id, &notification_info);

    printf("[CINT] :: ============== Printing Group Table to check notifications .==============\n");
    bshell(0, "d chg BSC_KG_GROUP_TABLE 0");
    bshell(0, "d chg BSC_DG_GROUP_TABLE 0");
}

int clr_not(bcm_flowtracker_group_t id, int notification)
{

    bcm_flowtracker_cpu_notification_info_t_init(&notification_info);

    bcm_flowtracker_group_cpu_notification_get(unit, id, &notification_info);

    BCM_FLOWTRACKER_CPU_NOTIFICATION_CLR(notification_info, notification);
    bcm_flowtracker_group_cpu_notification_set(unit, id, &notification_info);

    printf("[CINT] :: ============== Printing Group Table to check notifications .==============\n");
    bshell(0, "d chg BSC_KG_GROUP_TABLE 0");
    bshell(0, "d chg BSC_DG_GROUP_TABLE 0");
}

int get_not(bcm_flowtracker_group_t id, int notification)
{

    bcm_flowtracker_cpu_notification_info_t_init(&notification_info);

    bcm_flowtracker_group_cpu_notification_get(unit, id, &notification_info);

    if (BCM_FLOWTRACKER_CPU_NOTIFICATION_GET(notification_info, notification)) {
        printf("[CINT] ::  Notification is set.\n");
    } else {
        printf("[CINT] ::  Notification is not Set.\n");
    }

}


bcm_flowtracker_export_trigger_info_t trig_info;

int set_trig(bcm_flowtracker_group_t id, int trigger)
{

    bcm_flowtracker_group_export_trigger_get(unit, id, &trig_info);
    BCM_FLOWTRACKER_TRIGGER_SET(trig_info, trigger);
    /* bcmFlowtrackerExportTriggerNewLearn */
    bcm_flowtracker_group_export_trigger_set(unit, id, &trig_info);


    printf("[CINT] :: ============== Printing Group Table.==============\n");
    bshell(0, "d chg BSC_KG_GROUP_TABLE 0");
    bshell(0, "d chg BSC_DG_GROUP_TABLE 0");
}

int get_trig(bcm_flowtracker_group_t id, int trigger)
{


    sal_memset(&trig_info, 0, sizeof(bcm_flowtracker_export_trigger_info_t));


    bcm_flowtracker_export_trigger_info_t_init(&trig_info);
    if (BCM_FLOWTRACKER_TRIGGER_GET(notification_info, trigger)) {
        printf("[CINT] ::  Trigger is set.\n");
    } else {
        printf("[CINT] ::  Trigger is not Set.\n");
    }
}

int clr_trig(bcm_flowtracker_group_t id, int trigger)
{

    bcm_flowtracker_export_trigger_info_t_init(&trig_info);

    bcm_flowtracker_group_export_trigger_get(unit, id, &trig_info);
    BCM_FLOWTRACKER_TRIGGER_CLR(trig_info, trigger);
    bcm_flowtracker_group_export_trigger_set(unit, id, &trig_info);


    printf("[CINT] :: ============== Printing Group Table.==============\n");
    bshell(0, "d chg BSC_KG_GROUP_TABLE 0");
    bshell(0, "d chg BSC_DG_GROUP_TABLE 0");
}

/**** FT Group Controls Ends *************************************************/

/**** Traffic ****************************************************************/

int send_packet()
{
    bshell(unit, "port xe0 lb=mac");

    bshell(unit,"tx 1 pbm=xe0 File=pkt_L2_QinQ_TCP_SYN.dump");

    return 0;
}

int send_rev_packet()
{

    bshell(unit, "port xe0 lb=mac");

    bshell(unit, "tx 1 pbm=xe0  File=pkt_rev_dir.dump");

    return 0;
}


int send_vxlan_packet()
{
    bshell(unit, "port xe0 lb=mac");

    bshell(unit, "tx 1 pbm=xe0  File=pkt_vxlan_vnid345667.dump");
}


int send_ipv6_packet()
{
    bshell(unit, "port xe0 lb=mac");

    bshell(unit, "tx 1 pbm=xe0  File=pkt_ipv6_TCP_SYN.dump");
}


int send_dos_packet()
{
    bshell(unit, "port xe0 lb=mac");

    bshell(unit,"tx 1 pbm=xe0 File=pkt_dosattack_presel_macsa_da.dump");

    return 0;
}


int send_second_dos_packet()
{
    bshell(unit, "port xe0 lb=mac");

    bshell(unit,"tx 1 pbm=xe0 File=pkt_dosattack_presel_dst_src_port.dump");

    return 0;
}

/**** Traffic Ends ***********************************************************/




int testMenu()
{
    printf("[CINT] :: Displaying All the tests currently available\n");
    printf("[CINT] ::  ******************************************************************************************************************\n");
    printf("[CINT] :: Display All the tests currently available\n");
    printf("[CINT] :: 0 : Exit the testing utility. \n");
    printf("[CINT] :: Test 1 : Tracking Param = AT_IT_IPV4 5Tuple + IP proto || Session Data = pktCount+l4port+ipprotocol+TTL check\n");
    printf("[CINT] :: Test 2 : Tracking Param = AT_IT_IPV4 5Tuple + TTL || Session Data = 10 pktCount\n");
    printf("[CINT] :: Test 3 : Tracking Param = AT_IT_IPV4 5Tuple + TTL || Session Data =  pktCount + 2 TTL checks\n");
    printf("[CINT] :: Test 4 : Tracking Param = AT_IT_IPV4 5Tuple + Tcp Window || Session Data =  pktCount + 2 TTL checks\n");
    printf("[CINT] :: Test 5 : Tracking Param = AT_IT_IPV4 5Tuple || Session Data =  pktCount + TCP window checks\n");
    printf("[CINT] :: Test 6 : Tracking Param = AT_IT_IPV4 5Tuple || Session Data =  pktCount + 7 TCP window checks\n");
    printf("[CINT] :: Test 7 : Same as test 5 but send 5 packets and see ALU export for 3rd packet.\n");
    printf("[CINT] :: Test 8 : Tracking Param = AT_IT_IPV4 5Tuple + L4DestPort ||  TCP window checks(fail)\n");
    printf("[CINT] :: Test 9 : Tracking Param = AT_IT_IPV4 5Tuple + L4DestPort ||  9 checks fail (limit is 8)\n");
    printf("[CINT] :: Test 10 : Tracking Param = AT_IT_IPV4 5Tuple + L4DestPort || 2 tcp window checks one fail and one pass\n");
    printf("[CINT] :: Test 11 : Do Not Flowtrack \n");
    printf("[CINT] :: Test 12 : New Learn Timestamping. \n");
    printf("[CINT] :: Test 13 : New Learn Timestamping, ALU0 timestamp and ALu1 timestamp. \n");
    printf("[CINT] :: Test 14 : ALU0 timestamp and ALu1 timestamp. \n");
    printf("[CINT] :: Test 15 : Negative test : Create 9 ALU 16 checks. Limit is only 8. \n");
    printf("[CINT] :: Test 16 : Negative test : Create 13 ALU 32 element. Limit is only 12. \n");
    printf("[CINT] :: Test 17 : Negative test : Create 17 load 16 element. Limit is only 16. \n");
    printf("[CINT] :: Test 18 : Negative test : Create 9 load 8 element. Limit is only 8. \n");
    printf("[CINT] :: Test 19 : tcp window size in range check and export on matching tcp window size. \n");
    printf("[CINT] :: Test 20 : Flow start timestamping test. \n");
    printf("[CINT] :: Test 31 : IPV6 basic test.5 tuple + pkt_count, hop limit and nexthdr traking and check  \n");
    printf("[CINT] :: Test 32 : Same as test 31 + 3 timestamps.\n");
    printf("[CINT] :: Test 41 : Vxlan(v4) Basic test. 5_tuple + pkt count, 2 vnid range checks, one pass and one fail.\n");
    printf("[CINT] :: Test 61 : Dos attacks : Check with matching type dos attack and export the packet..\n");
    printf("[CINT] :: Test 62 : Dos attacks : Export for any new Dos attack..\n");
    printf("[CINT] :: Test 100 : Clean up the configuration\n");

    printf("[CINT] ::  ******************************************************************************************************************\n");

    return 0;
}

testMenu();

int test_run(int test_num)
{
    char command[200];

    if (test_num == 0) {
        printf("[CINT] :: ====== Existing ======\n");
    } else if (test_num < 30) {
        printf("[CINT] :: Adding IPv4 preselection\n");
        cc_ftfp_group_ipv4();
    } else if ((test_num > 30) && (test_num <= 40)) {
        printf("[CINT] :: Adding IPv6 preselection\n");
        cc_ftfp_group_ipv6();
    } else if ((test_num > 40) && (test_num <= 50)) {
        printf("[CINT] :: Adding VXLAN preselection\n");
        cc_ftfp_group_ipv4_vxlan();
    } else if ((test_num > 60) && (test_num <= 70)) {
        printf("[CINT] :: Adding dos preselection\n");
        cc_ftfp_group_ipv4_dos();
    } else {
        testMenu();
    }

    /*
       int cc_ipv4(
       int num_groups =>  num groups to be created,
       bcm_flowtracker_tracking_param_type_t param, => tracking parameters in tracking list as data.

       int num_alu32, int num_checks, int num_load16, int num_load8, ==> number of various memories.

       Check Params ===>  bcm_flowtracker_tracking_param_type_t check_param,
       int min, int max, bcm_flowtracker_check_operation_t opr)

     */
    printf("[CINT] :: Starting ftmon\n");
    bshell(0, "ftrmon stop");
    bshell(0, "sleep 1");
    bshell(0, "ftrmon start");
    bshell(0, "sleep 1");
    bshell(0, "ftrmon report +decode");
    bshell(0, "sleep 1");


    switch(test_num) {
        case 0:
            return 0;

//cc_ipv4(int groups, bcm_flowtracker_tracking_param_type_t param,
//       int alu32, int checks, int load16, int load8,
//       bcm_flowtracker_tracking_param_type_t check_param,
//       int min, int max, bcm_flowtracker_check_operation_t opr, int offset)

        case 1:
            printf("[CINT] :: Collector is getting created now.\n");
            fuzzy_flow_check_data = 1;
            sw_managed_group = 0;
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeIPProtocol, 1, 1, 1, 1, bcmFlowtrackerTrackingParamTypeTTL, 1, 200, bcmFlowtrackerCheckOpInRange, 0);
            //cc_ipv4(500, bcmFlowtrackerTrackingParamTypeIPProtocol, 2, 2, 2, 4, bcmFlowtrackerTrackingParamTypeTTL, 1, 1, bcmFlowtrackerCheckOpInRange, 1);
            sal_sleep(1);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            printf("[CINT] :: Sending Packet Now.\n");
            send_packet();

            printf("[CINT] :: Checking Group Stats for New learn.\n");
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            fuzzy_flow_check_data = 0;

            break;

        case 2:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeTTL, 10, 0, 0, 0, bcmFlowtrackerTrackingParamTypeTTL, 1, 200, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            printf("[CINT] :: Sending Packet Now.\n");
            send_packet();

            printf("[CINT] :: Checking Group Stats for New learn.\n");
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 3:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeTTL, 1, 2, 0, 0, bcmFlowtrackerTrackingParamTypeTTL, 1, 200, bcmFlowtrackerCheckOpInRange, 0);

            sal_sleep(1);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            printf("[CINT] :: Sending Packet Now.\n");
            send_packet();

            printf("[CINT] :: Checking Group Stats for New learn.\n");
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 4:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 1, 1, 0, 0, bcmFlowtrackerTrackingParamTypeTTL, 1, 200, bcmFlowtrackerCheckOpInRange, 0);

            sal_sleep(1);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            printf("[CINT] :: Sending Packet Now.\n");
            send_packet();

            printf("[CINT] :: Checking Group Stats for New learn.\n");
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 5:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 1, 0, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 1, 200, bcmFlowtrackerCheckOpInRange, 0);

            sal_sleep(1);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            printf("[CINT] :: Sending Packet Now.\n");
            send_packet();

            printf("[CINT] :: Checking Group Stats for New learn.\n");
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 6:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 7, 0, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 1, 200, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 7:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 1, 1, 0, 0, bcmFlowtrackerTrackingParamTypeTTL, 1, 100, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            sal_sleep(1);
            sal_sleep(1);
            /*            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);
                          send_packet();
                          stats(0);
                          bshell(0, "d chg bsc_ipfix_stats");
                          bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
                          sal_sleep(1);
                          printf("[CINT] :: Dumping Session table State\n.\n");
                          sal_sleep(1);
                          bshell(0, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=0");
             */
            sal_sleep(1);
            sal_sleep(1);
            printf("[CINT] :: Send 1 packets again to check ALU threshold based check\n");


            send_packet();

            sal_sleep(1);
            sal_sleep(1);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 8:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 1, 1, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 1, 200, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 9:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 2, 1, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 1, 200, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 10:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 2, 2, 1, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 0x1100, 0x12f0, bcmFlowtrackerCheckOpInRange, 0x100);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            /*            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);*/
            send_packet();
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);

            break;

        case 11:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 2, 2, 1, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 0x11ff, 0x12f0, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            send_packet();
            printf("[CINT] :: Check that new learn is not done.\n");
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            break;
            /*
               int cc_ipv4(
               int num_groups =>  num groups to be created,
               bcm_flowtracker_tracking_param_type_t param, => tracking parameters in tracking list as data.

               int num_alu32, int num_checks, int num_load16, int num_load8, ==> number of various memories.

               Check Params ===>  bcm_flowtracker_tracking_param_type_t check_param,
               int min, int max, bcm_flowtracker_check_operation_t opr)
             */
        case 12:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            printf("[CINT] :: Enable New learn timestamping.\n");
            new_learn_ts = 1;
            cc_ipv4(1, 0, 1, 1, 1, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 0x11f0, 0x12f0, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);

            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            new_learn_ts = 0;
            break;

        case 13:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            printf("[CINT] :: Enable New learn timestamping.\n");
            new_learn_ts = 1;
            alu_ts_0 = 1;
            alu_ts_1 = 1;
            second_check=1;
            cc_ipv4(1, 0, 1, 1, 1, 0, bcmFlowtrackerTrackingParamTypeTTL, 0x1, 0xff, bcmFlowtrackerCheckOpGreater, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            bshell(0, "modify BSC_TS_UTC_CONVERSION 0 1 TIMESTAMP_TOD_SEC=65538 TIMESTAMP_TOD_NSEC=65538");

            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            new_learn_ts = 0;
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 14:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            printf("[CINT] :: Enable New learn timestamping.\n");
            second_check=1;
            alu_ts_0 = 1;
            alu_ts_1 = 1;
            cc_ipv4(1, 0, 1, 1, 1, 0, bcmFlowtrackerTrackingParamTypeTTL, 0x1, 0xff, bcmFlowtrackerCheckOpGreater, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            bshell(0, "modify BSC_TS_UTC_CONVERSION 0 1 TIMESTAMP_TOD_SEC=65538 TIMESTAMP_TOD_NSEC=65538");

            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            new_learn_ts = 0;
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 15:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 9, 1, 0, bcmFlowtrackerTrackingParamTypeTTL, 0x1, 0xff, bcmFlowtrackerCheckOpGreater, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;


        case 16:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 13, 1, 1, 0, bcmFlowtrackerTrackingParamTypeTTL, 0x1, 0xff, bcmFlowtrackerCheckOpGreater, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 17:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 1, 17, 1, bcmFlowtrackerTrackingParamTypeTTL, 0x1, 0xff, bcmFlowtrackerCheckOpGreater, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;


        case 18:
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 1, 1, 9, bcmFlowtrackerTrackingParamTypeTTL, 0x1, 0xff, bcmFlowtrackerCheckOpGreater, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 19:
            printf("[CINT] :: Collector is getting created now.\n");
            test_number = test_num;
            cc_ft_collector();
            cc_ipv4(1, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 2, 1, 1, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 0x1100, 0x12f0, bcmFlowtrackerCheckOpInRange, -1);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            /*            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);*/
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            break;

        case 20:
            printf("[CINT] :: Enable Flow start timestamping.\n");
            flow_start_ts = 1;
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            cc_ipv4(1, 0, 1, 1, 1, 0, bcmFlowtrackerTrackingParamTypeTcpWindowSize, 0x1000, 0x12f0, bcmFlowtrackerCheckOpInRange, 0);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            set_get_time(10, 20);

            print set_trig(flow_group_id2[0], bcmFlowtrackerExportTriggerNewLearn);
            send_packet();
            stats(flow_group_id2[0]);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            sal_memset(command, 0, 200);
            sprintf(command, "ft show session_data DIP=0x0a0a0202 SIP=0x0a0a0203 L4DestPort=0x5566 L4SrcPort=0x3344 PROTO=0x6 Group=%d", flow_group_id2[0]);
            bshell(0, command);
            new_learn_ts = 0;
            break;


        case 31:
            printf("[CINT] :: Testing basic IPV6 flow.\n");
            cc_ft_collector();
            cc_ipv6();
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv6();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv6();
            bshell(0, "modify BSC_TS_UTC_CONVERSION 0 1 TIMESTAMP_TOD_SEC=65538 TIMESTAMP_TOD_NSEC=65538");

            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);
            send_ipv6_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            new_learn_ts = 0;
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 32:
            printf("[CINT] :: Testing IPV6 packet with 3 timestamps.\n");
            new_learn_ts = 1;
            second_check=1;
            alu_ts_0 = 1;
            alu_ts_1 = 1;
            cc_ft_collector();
            cc_ipv6();
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv6();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv6();
            bshell(0, "modify BSC_TS_UTC_CONVERSION 0 1 TIMESTAMP_TOD_SEC=65538 TIMESTAMP_TOD_NSEC=65538");

            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);
            send_ipv6_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            new_learn_ts = 0;
            break;

        case 41:
            printf("[CINT] :: Testing basic Vxlan Flow.\n");
            cc_ft_collector();
            cc_vxlan();
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4_vxlan();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4_vxlan();

            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);
            send_vxlan_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            new_learn_ts = 0;
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 42:
            printf("[CINT] :: Testing basic Vxlan Flow.\n");
            new_learn_ts = 1;
            cc_ft_collector();
            cc_vxlan();
            sal_sleep(1);
            set_get_time(10, 20);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4_vxlan();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4_vxlan();

            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);
            send_vxlan_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            new_learn_ts = 0;
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 43:
            printf("[CINT] :: Testing basic Vxlan Flow.\n");
            new_learn_ts = 1;
            alu_ts_0 = 1;
            cc_ft_collector();
            cc_vxlan();
            sal_sleep(1);
            set_get_time(10, 20);
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4_vxlan();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4_vxlan();

            print set_trig(0, bcmFlowtrackerExportTriggerNewLearn);
            send_vxlan_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            new_learn_ts = 0;
            alu_ts_1 = 0;
            alu_ts_0 = 0;
            break;

        case 61:
            test_number = test_num;
            printf("[CINT] :: Check DoS attacks.\n");
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            dos_ipv4();
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            sal_sleep(1);
            send_dos_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            bshell(0, "ft show session_data DIP=0x0a0a0101 SIP=0x0a0a0102 L4DestPort=0x0 L4SrcPort=0x0 PROTO=0x6 Group=0");
            sal_sleep(20);

            send_second_dos_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            bshell(0, "ft show session_data DIP=0x0a0a0101 SIP=0x0a0a0102 L4DestPort=0x0 L4SrcPort=0x0 PROTO=0x6 Group=0");


            new_learn_ts = 0;
            break;

        case 62:
            test_number = test_num;
            printf("[CINT] :: Check DoS attacks.\n");
            printf("[CINT] :: Collector is getting created now.\n");
            cc_ft_collector();
            dos_ipv4();
            sal_sleep(1);
            printf("[CINT] :: Collector is getting added to Flowtracker Group.\n");
            cc_ft_collector_add_ipv4();
            printf("[CINT] :: Associate with FTFP Entries.\n");
            cc_ftfp_entries_ipv4();
            sal_sleep(1);
            send_dos_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            bshell(0, "ft show session_data DIP=0x0a0a0101 SIP=0x0a0a0102 L4DestPort=0x0 L4SrcPort=0x0 PROTO=0x6 Group=0");

            sal_sleep(20);
            send_second_dos_packet();
            stats(0);
            bshell(0, "d chg bsc_ipfix_stats");
            bshell(0, "setreg BSC_EX_EXPORT_TRIGGER EXPORT_ENABLE=1");
            bshell(0, "ft show session_data DIP=0x0a0a0101 SIP=0x0a0a0102 L4DestPort=0x0 L4SrcPort=0x0 PROTO=0x6 Group=0");

            new_learn_ts = 0;

            break;


        case 100:
            /* Delete FTFP Entries */
            dc_ftfp_entries();

            /* Delete FTFP Groups */
            dc_ftfp_group_ipv4();
            dc_ftfp_group_ipv6();
            dc_ftfp_group_ipv4_vxlan();
            dc_ftfp_group_ipv4_dos();

            /* Remove active flows */
            dc_ft_group_flows_clear();

            /* Delete FT Collector */
            dc_ft_collector();

           /* Delete FT Groups */
            dc_ft_group_ipv4();
            dc_ft_group_ipv6();
            dc_ft_group_ipv4_vxlan();
            //dc_ft_group_ipv4_dos();

            break;

        default :
            /* Invalid config. */
            testMenu();
            break;
    }

    return 0;
}
