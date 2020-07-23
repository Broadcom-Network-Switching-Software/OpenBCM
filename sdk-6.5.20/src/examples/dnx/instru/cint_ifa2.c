/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *  This file contains examples for IFA 2.0 node configurations.
 *
 *
 *                       IFA Zone Framework
 *
 *                             +----------+
 *                             |          |
 *                             |Collector |--------------+
 *                             |          |              |
 *                             +----------+              |
 *                                                       |
 *                                                       |
 *                                                       |
 *                                                       |
 *                                                       |
 *                                                       |
 *                                                       |
 *                                                       |
 *    +--------------+        +------------+        +----+-----------+
 *    |Initiator Node|        |Transit Node|        |Terminating Node|
 *    |   +------+   |        |  +------+  |        |     +------+   |
 *    |   | IFA  |   |------->|  | IFA  |  |------->|     | IFA  |   |
 *    |   +------+   |IFA flow|  +------+  |IFA flow|     +------+   |
 *    +--------------+        +------------+        +----------------+
 *
 *
 *
 *
 *  Initiator Node:
 *  Samples packets and adds IFA 2.0 encapsulation (IFA 2.0 Header, IFA 2.0 metadata header and intiator node's metadata):
 *
 *                         IFA Packet Format
 *
 *      0                   1                   2                   3
 *      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                        IP Header                              |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                       IFA Header                              |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                     Layer 4 Header                            |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                    IFA Metadata Stack                         |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     ~                                                               ~
 *     |                     Layer 4 Header                            |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 *
 *  Intermediate (Transit) Node:
 *  Adds IFA Metadata of current node to IFA Metadata Stack
 *
 *  Terminating Node:
 *  1. Terminates IFA 2.0 session by removing IFA 2.0 headers from packet and forwarding the
 *     original packet.
 *  2. Forwards copy of the IFA 2.0 encapsulated packet to collector.
 */
/*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/internal/cint_route_internal.c
 * cint
 * ifa_route_config(0,201,202,52430);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=201 data=0x000c00020000000007000100080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a81000064080045000057000000007f06fb1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/field/cint_field_ip_snoop.c
 * cint ../../src/./examples/dnx/instru/cint_instru_ipt.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/instru/cint_ifa.c
 * cint ../../src/./examples/dnx/instru/cint_ifa2.c
 * cint
 * cint_instru_ifa2_initiator(0);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=201 data=0x000c00020000000007000100080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a8100006408004500007f000000007ffdf9fec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00ff042000ffff4000800067000009010d0ef7610018729c43000000c900ca00000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # IPT packet
 * tx 1 psrc=201 data=0x000c000200000000070001009100000081000000080045000057000000008006fa1dc08001017fffff023a98000000000000000000000010001000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                    |  ETH                                        |   IP                                  | TCP                                   |  payload
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a8100006408004500007f000000007ffdf9fec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00ff042000ffff4000800067000009010d0ef7610009cbe66f000000c900ca00000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *          |  ETH                               |   IP                                  |IFA 2.0|    TCP                                |IFA 2.0|             IFA 2.0 Metadata                                  |    payload
 *                                                                                       |Header |                                       |MD     |                                                               |
 *                                                                                       |       |                                       |Header |                                                               |
 */

/* Transit Node*/
/*
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/internal/cint_route_internal.c
 * cint
 * ifa_route_config(0,200,203,52430);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=200 data=0x000c000200000000070001008100000008004500007f0000000080fdf8fec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00ff082000ffff40008000000000000000000000000000000000000000000000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a8100006408004500007f000000007ffdf9fec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00ff082000ffff40008000000000000000000000000000000000000000000000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/field/cint_field_ip_snoop.c
 * cint ../../src/./examples/dnx/instru/cint_instru_ipt.c
 * cint ../../src/./examples/dnx/instru/cint_field_IFA_datapath.c
 * cint ../../src/./examples/dnx/instru/cint_ifa.c
 * cint ../../src/./examples/dnx/instru/cint_ifa2.c
 * cint
 * cint_instru_ifa2_intermediate(0);
 * exit;
 *
 * # IPT packet
 * tx 1 psrc=200 data=0x000c000200000000070001008100000008004500007f0000000080fdf8fec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00ff082000ffff40008000000000000000000000000000000000000000000000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *                    |  ETH                               |   IP                                  |IFA 2.0|    TCP                                 |IFA 2.0|             IFA 2.0 Metadata                                  |    payload
 *                                                                                                 |Header |                                        |MD     |                                                               |
 *                                                                                                 |       |                                        |Header |                                                               |
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a8100006408004500009f000000007ffdf9dec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00fe102000ffff4000800067800009ff4436639a00252e1b83000000c800cb000000002000ffff40008000000000000000000000000000000000000000000000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *         |  ETH                               |   IP                                  |IFA 2.0|    TCP                                 |IFA 2.0|             IFA 2.0 Metadata transit node                     |             IFA 2.0 Metadata  1st node                        |    payload
 *                                                                                      |Header |                                        |MD     |                                                               |                                                               |
 *                                                                                      |       |                                        |Header |                                                               |                                                               |
 *
 * # IPT packet
 * tx 1 psrc=200 data=0x000c00020000000007000100910000008100000008004500007f0000000080fdf8fec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00fe102000ffff40008000000000000000000000000000000000000000000000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * # Received packets on unit 0 should be:
 * # Source port: 0, Destination port: 0
 * # Data: 0x00000000cd1d00123456789a8100006408004500009f000000007ffdf9dec08001017fffff022f0600ff3a98000000000000000000000010001000000000ff00fd182000ffff4000800067800009ff460f2e0600233b3cc7000000c800cb000000002000ffff40008000000000000000000000000000000000000000000000000000000000000111000000000222ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */


int DEFAULT_DEVICE_ID = 0xFFFF;
int DEFAULT_HOP_LIMIT = 0x7f;
int DEFAULT_MAX_IFA_LENGTH = 0x1000;
int DEFAULT_IFA_SENDER = 0xFFFF;
int DEFAULT_IFA_TEMPLATE_ID = 0x2;

int IPT_PROFILE_IFA2_INITIATOR = 5; /* Used for initiator node */
int IPT_PROFILE_IFA2_TERMINATION = 7; /* Used for termination node (both with and without md) */
int IPT_PROFILE_IFA2_UDP_GPE = 6; /* Used for intermediate and termination with md nodes */
int DEFAULT_IP_PROTOCOL = 253;
int DEFAULT_IFA2_MAX_SIZE = 0xff;

/* Trap id for second passs of termination with_md*/
int ifa2_trap_id = -1;
uint32 cint_field_IFA_datapath_egress_ace_entry_handle;
bcm_field_qualify_t cint_field_IFA_datapath_egress_snooped_qual;
bcm_field_group_t cint_field_IFA_datapath_egress_fg_id=0;
bcm_field_context_t cint_field_IFA_datapath_egress_context_id_ipv4;
bcm_field_presel_t cint_field_IFA_datapath_egress_presel_ipv4 = 6;
bcm_field_context_t cint_field_IFA_datapath_egress_context_id_2nd_pass;
bcm_field_group_t cint_field_IFA_datapath_egress_fg_id_2nd_pass=0;
int cint_field_IFA_2nd_p_fhei_trap_id;

int cint_instru_ipt_init(int unit)
{
    int rv;
    uint32 ipt_profile;
    bcm_instru_ipt_t config;
    uint32 supported_flags_per_device = 0;

    ipt_profile = IPT_PROFILE_IFA2_INITIATOR;
    bcm_instru_ipt_t_init(&config);

    config.node_type = bcmInstruIptNodeTypeTail;
    config.metadata_flags = supported_flags_per_device;

    rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_ipt_profile_set failed for Tail Edit\n");
        return rv;
    }

    ipt_profile = IPT_PROFILE_IFA2_TERMINATION;
    bcm_instru_ipt_t_init(&config);

    config.node_type = bcmInstruIptNodeTypeTail;
    config.metadata_flags = supported_flags_per_device;

    rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_ipt_profile_set failed for Tail Edit\n");
        return rv;
    }

    return rv;
}

/*
 * Example IFA 2.0  Node configuration.
 * This example configures all node parameters through bcm_ifa_config_info_set.
 * The IFA header and metadata will be built according to this configuration.
 * This configuration should be performed for all node types (initiator, transit
 * and terminator).
 */
int cint_ifa2_node_parameters_config(int unit)
{
    int rv = BCM_E_NONE;
    bcm_ifa_config_info_t config_data;

    config_data.device_id = DEFAULT_DEVICE_ID;
    config_data.hop_limit = DEFAULT_HOP_LIMIT;
    config_data.max_payload_length = DEFAULT_MAX_IFA_LENGTH;
    config_data.template_id = DEFAULT_IFA_TEMPLATE_ID;

    /** Set ifa node */
    rv = bcm_ifa_config_info_set(unit , 0, &config_data);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_ifa_config_info_set failed\n");
        printf("rv %d\n",rv);
        return rv;
    }

    return rv;
}

/* Example of configuring IFA 2.0 header fields */
int cint_config_IFA2_header(int unit)
{
    int rv = BCM_E_NONE;
    bcm_ifa_header_t header;

    header.max_length = DEFAULT_IFA2_MAX_SIZE;
    header.hop_limit = DEFAULT_HOP_LIMIT;
    header.ip_protocol = DEFAULT_IP_PROTOCOL;

    rv = bcm_ifa_header_create(unit,0,0,&header);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_ifa_header_create failed\n");
        printf("rv %d\n",rv);
        return rv;
    }
    return rv;
}

/* Example of field configuration for IFA 2.0, to be used for for intiator node.
 * iPMF1 traps packet by 5-tupple, sets IPT profile for context selection and uses container
 * to flag iPMF3 to set IPTCommand */
int cint_field_IFA2_datapath_initiator_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_get_t qual_info_get_offset;
    bcm_field_entry_info_t ent_info;

    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_a_cont_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = bcmFieldQualifyDstMac;
    /* Set actions */
    fg_info.nof_actions = 2;
    /* Tail Edit profile */
    fg_info.action_types[0] = bcmFieldActionIPTProfile;
    /* INT command*/
    fg_info.action_types[1] = cint_field_IFA_datapath_ingress_act_container_1b;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_datapath", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_ingress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPv4", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /* Add flow_id in field */
    rv = appl_dnx_field_flow_id_init(unit,cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in appl_dnx_field_flow_id_init\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    /* Set Layer for Qualifier to Layer 0 = ETH */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id, cint_field_IFA_datapath_ingress_context_id_ipv4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_ingress_fg_id,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipv4;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_transit_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    /*iPMF3 Configuration*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_q_cont_1b", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_qual_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 9;
    action_info.stage = bcmFieldStageIngressPMF3;
    action_info.action_type = bcmFieldActionIPTCommand;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPT_c_1b", sizeof(action_info.name));
    bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_IPTCommand_1b);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_qual_container_1b;
    /* Set actions */
    fg_info.nof_actions = 1;
    /* INT command*/
    fg_info.action_types[0] = cint_field_IFA_datapath_ingress_act_IPTCommand_1b;
    fg_info.action_with_valid_bit[0]=FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_dp_IPT_c", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_ingress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_iPMF3", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
    printf("Error (%d), in bcm_field_context_create\n", rv);
    return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    bcm_field_qualifier_info_get_t_init(& qual_info_get_offset);
    bcm_field_qualifier_info_get(unit,bcmFieldQualifyContainer,bcmFieldStageIngressPMF3,&qual_info_get_offset);
    attach_info.key_info.qual_info[0].offset = qual_info_get_offset.offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id_ipmf3, cint_field_IFA_datapath_ingress_context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipmf3;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF3) context(%d) fwd_layer(Ipv4) \n",
    cint_field_IFA_datapath_transit_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority_initiator;

    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp4;
    ent_info.entry_qual[0].mask[0] = 0x1F;
    ent_info.entry_qual[1].type = bcmFieldQualifyDstMac;
    ent_info.entry_qual[1].value[0] = 0x00020000;
    ent_info.entry_qual[1].value[1] = 0x000c;
    ent_info.entry_qual[1].mask[0] = -1;
    ent_info.entry_qual[1].mask[1] = -1;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = bcmFieldActionIPTProfile;
    ent_info.entry_action[0].value[0] = IPT_PROFILE_IFA2_INITIATOR;
    ent_info.entry_action[1].type = cint_field_IFA_datapath_ingress_act_container_1b;
    ent_info.entry_action[1].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_fg_id, &ent_info, &cint_field_ip_snoop_ent_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Tail_Profile (3) \n", cint_field_ip_snoop_ent_id);

    return 0;
}

/*
 * Example cint for IFA 2.0 Initiator Node.
 * iPMF1 will be used to sample packets for IFA 2.0 . This is done by 5 tupple.
 * The PMF will set IPTProfile.
 */

int cint_instru_ifa2_initiator(int unit)
{
    bcm_field_entry_t entry_handle;
    int rv = BCM_E_NONE;

    if (*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
      /* devices require ipt configuration */
      rv = cint_instru_ipt_init(unit);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in ePMF rule creation \n", rv);
         return rv;
      }
    }

    entry_handle = 0;

    /* Configure Initiator Node Parameters */
    rv = cint_ifa2_node_parameters_config(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }

    /* Configure IFA 2.0 header Parameters */
    rv = cint_config_IFA2_header(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }

    /* Configure Field */
    rv = cint_field_IFA2_datapath_initiator_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }


    return rv;
}


/* Example of field configuration for IFA 2.0, to be used for for intermediate node.
 * iPMF1 traps packet by IP-Protocol, sets IPT profile for context selection and uses container
 * to flag iPMF3 to set IPTCommand */
int cint_field_IFA2_datapath_intermediate_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_qualifier_info_get_t qual_info_get_offset;
    bcm_field_entry_info_t ent_info;

    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_a_cont_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = bcmFieldQualifyIp4Protocol;
    /* Set actions */
    fg_info.nof_actions = 2;
    /* Tail Edit profile */
    fg_info.action_types[0] = bcmFieldActionIPTProfile;
    /* INT command*/
    fg_info.action_types[1] = cint_field_IFA_datapath_ingress_act_container_1b;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_datapath", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_ingress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPv4", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /* Add flow_id in field */
    rv = appl_dnx_field_flow_id_init(unit,cint_field_IFA_datapath_ingress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in appl_dnx_field_flow_id_init\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    /* Set Layer for Qualifier to Layer 0 = ETH */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id, cint_field_IFA_datapath_ingress_context_id_ipv4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_ingress_fg_id,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipv4;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_transit_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    /*iPMF3 Configuration*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_q_cont_1b", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_qual_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 9;
    action_info.stage = bcmFieldStageIngressPMF3;
    action_info.action_type = bcmFieldActionIPTCommand;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPT_c_1b", sizeof(action_info.name));
    bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_IPTCommand_1b);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_qual_container_1b;
    /* Set actions */
    fg_info.nof_actions = 1;
    /* INT command*/
    fg_info.action_types[0] = cint_field_IFA_datapath_ingress_act_IPTCommand_1b;
    fg_info.action_with_valid_bit[0]=FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_dp_IPT_c", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_ingress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_iPMF3", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
    printf("Error (%d), in bcm_field_context_create\n", rv);
    return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    bcm_field_qualifier_info_get_t_init(& qual_info_get_offset);
    bcm_field_qualifier_info_get(unit,bcmFieldQualifyContainer,bcmFieldStageIngressPMF3,&qual_info_get_offset);
    attach_info.key_info.qual_info[0].offset = qual_info_get_offset.offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id_ipmf3, cint_field_IFA_datapath_ingress_context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipmf3;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF3) context(%d) fwd_layer(Ipv4) \n",
    cint_field_IFA_datapath_transit_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority_initiator;

    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp4;
    ent_info.entry_qual[0].mask[0] = 0x1F;
    ent_info.entry_qual[1].type = bcmFieldQualifyIp4Protocol;
    ent_info.entry_qual[1].value[0] = 253;
    ent_info.entry_qual[1].mask[0] = -1;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = bcmFieldActionIPTProfile;
    ent_info.entry_action[0].value[0] = IPT_PROFILE_IFA2_UDP_GPE;
    ent_info.entry_action[1].type = cint_field_IFA_datapath_ingress_act_container_1b;
    ent_info.entry_action[1].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_fg_id, &ent_info, &cint_field_ip_snoop_ent_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Tail_Profile (3) \n", cint_field_ip_snoop_ent_id);

    return 0;
}

/*
 * Example cint for IFA 2.0 Intermediate Node.
 * iPMF1 will be used to sample IFA 2.0 packets . This is done by detecting IP-protocol type.
 * The PMF will set IPTProfile.
 */

int cint_instru_ifa2_intermediate(int unit)
{
    int rv = BCM_E_NONE;

    if (*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
      /* J2P and above devices require ipt configuration */
      rv = cint_instru_ipt_init(unit);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in ePMF rule creation \n", rv);
         return rv;
      }
    }

    /* Configure Initiator Node Parameters */
    rv = cint_ifa2_node_parameters_config(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }
    /* Configure IFA 2.0 header Parameters */
    rv = cint_config_IFA2_header(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }

    /* Configure Field */
    rv = cint_field_IFA2_datapath_intermediate_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }

    return rv;
}

/**
*  Example to configure IFA 2.0 Termination node.
*    - This example configures IFA 2.0 Termination node, that terminated IFA flow and snoops IFA packet to collector.
*
*  Operation principle:
*      The IFA 2.0 traffic is trapped by IP protocol type, and
*      IFA 2.0 packets are snooped. One copy is forwarded to the
*      collector according to user defined trap and the IFA
*      headers are stripped from the header stack on the other
*      copy and it is forwarded to its original destination 
*
*  Configuration:
*    - Snooping configuration with tail-edit-profile = IPT_PROFILE_IFA2_TERMINATION.
*    - iPMF1 configuration
*        a. Set field group with 2 qualifiers and 4 actions
*           Qualifiers:
*             - IP protocol type = IFA 2.0
*            Actions:
*             0- Snoop Packet
*             1- Set Tail Edit Profile = 7
*    - ePMF configuration
*         a. Qualifier: Destination Port is Collector
*            Action: Set Tail Edit Profile = 0
 */
int cint_field_IFA2_datapath_termination_main(int unit)
{
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_context_t context_id = cint_field_IFA_datapath_ingress_context_id_ipv4;
    bcm_field_context_t ipmf1_context_id=0;
    bcm_field_action_t action_type = bcmFieldActionSnoop;
    char * name = "IFA_const_initiator";

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    bcm_field_entry_info_t ent_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t qual_info_get_offset;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    bcm_field_context_param_info_t param_info;
    int system_collector_port;
    void *dest_char;
    int id;

    int rv = BCM_E_NONE;

    rv = bcm_field_action_info_get(unit,action_type,stage,&action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    bcm_gport_t snoop_trap_gport_id;
    int mirror_dest_id;
    bcm_port_config_t port_config;
    int cpu_port;

    /*
     * Configure snooping
     */
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_ITER(port_config.cpu, cpu_port)
    {
        mirror_dest.gport = cpu_port;
        break;
    }

    rv = cint_field_ip_snoop_set(unit, cpu_port, 1, 0, 0, &snoop_trap_gport_id, &mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;

    /* Set quals */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = bcmFieldQualifyIp4Protocol;

    /* Set actions */
    fg_info.nof_actions = 2;
    /* INT command*/
    fg_info.action_types[0] = bcmFieldActionSnoop;
    /* Tail Edit profile */
    fg_info.action_types[1] = bcmFieldActionIPTProfile;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id);

    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    id = cint_field_IFA_datapath_ingress_fg_id;
    printf("FG (%s) created ID: %d \n", name,id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;

    /* Set Layer for Qualifier to Layer 0 = ETH */
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("FG (%d) Attached to context: %d \n",id,context_id);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_ingress_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);


    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_ingress_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 1;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;


    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_ingress_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority_initiator;

    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp4;
    ent_info.entry_qual[0].mask[0] = 0x1F;
    ent_info.entry_qual[1].type = bcmFieldQualifyIp4Protocol;
    ent_info.entry_qual[1].value[0] = 253;
    ent_info.entry_qual[1].mask[0] = -1;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = snoop_trap_gport_id;

    ent_info.entry_action[1].type = bcmFieldActionIPTProfile;
    ent_info.entry_action[1].value[0] = IPT_PROFILE_IFA2_TERMINATION;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_fg_id, &ent_info, &cint_field_ip_snoop_ent_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Tail_Profile (3) \n", cint_field_ip_snoop_ent_id);

    id = cint_field_IFA_ipmf2_fg_id;
    printf("FG (%s) created ID: %d \n", name,id);

    /* *********************** */
    /* ePMF for terminator Node */
    /* *********************** */

     /*
      * Configure the ACE format.
      */
      bcm_field_ace_format_info_t_init(&ace_format_info);

      ace_format_info.nof_actions = 1;

      ace_format_info.action_types[0] = bcmFieldActionIPTProfile;

      dest_char = &(ace_format_info.name[0]);
      sal_strncpy_s(dest_char, "AM_ACE_INIT", sizeof(ace_format_info.name));
      rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_field_ace_format_add \n", rv);
         return rv;
      }

      bcm_field_ace_entry_info_t_init(&ace_entry_info);

      ace_entry_info.nof_entry_actions = 1;
      ace_entry_info.entry_action[0].type = bcmFieldActionIPTProfile;
      /* tail edit profile = 0 - normal contexts for snooped copy */
      ace_entry_info.entry_action[0].value[0] = 0;

      rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &cint_field_IFA_datapath_ingress_ace_entry_handle);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_field_ace_entry_add \n", rv);
         return rv;
      }
      printf("ACE Format (%d) and ACE entry 0x(%x) created \n", ace_format_id,cint_field_IFA_datapath_ingress_ace_entry_handle);

      bcm_field_group_info_t_init(&fg_info);
      fg_info.fg_type = bcmFieldGroupTypeTcam;
      fg_info.stage = bcmFieldStageEgress;

      /* Set quals */
      fg_info.nof_quals = 1;
      fg_info.qual_types[0] = bcmFieldQualifyDstPort;
      /* Set actions */
      fg_info.nof_actions = 1;
      fg_info.action_types[0] = bcmFieldActionAceEntryId;

      dest_char = &(fg_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_datapath_e_init", sizeof(fg_info.name));
      rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_node_egress_fg_id);
      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_group_add\n", rv);
          return rv;
      }
      printf("Created TCAM field group (%d) in ePMF \n", cint_field_IFA_datapath_ingress_node_egress_fg_id);

      /**Attach  context**/
      bcm_field_group_attach_info_t_init(&attach_info);
      bcm_field_context_info_t_init(&context_info);

      dest_char = &(context_info.name[0]);
      sal_strncpy_s(dest_char, "IFA_eg_init_IPv4", sizeof(context_info.name));
      rv = bcm_field_context_create(unit, 0, fg_info.stage, &context_info, &cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4);
      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_context_create\n", rv);
          return rv;
      }

      attach_info.key_info.nof_quals = fg_info.nof_quals;
      attach_info.payload_info.nof_actions = fg_info.nof_actions;
      attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
      attach_info.payload_info.action_types[0] = fg_info.action_types[0];

      attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

      rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_node_egress_fg_id, cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4, &attach_info);

      if (rv != BCM_E_NONE)
      {
          printf("Error (%d), in bcm_field_group_context_attach\n", rv);
          return rv;
      }

      printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_ingress_node_egress_fg_id,cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4);

      bcm_field_entry_info_t_init(&ent_info);
      ent_info.priority = priority_initiator;

      ent_info.nof_entry_quals = fg_info.nof_quals;
      ent_info.entry_qual[0].type = fg_info.qual_types[0];

      /**The value should be encoded as GPORT_SYTEM_PORT*/
      ent_info.entry_qual[0].value[0] = cpu_port;
      ent_info.entry_qual[0].mask[0] = -1;

      ent_info.nof_entry_actions = fg_info.nof_actions;
      ent_info.entry_action[0].type = fg_info.action_types[0];
      ent_info.entry_action[0].value[0] = cint_field_IFA_datapath_ingress_ace_entry_handle;

      rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_node_egress_fg_id, &ent_info, &cint_field_IFA_datapath_ingress_node_egress_entry_handle);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in bcm_field_entry_add\n", rv);
         return rv;
      }
      printf("FG ID: (%d) Entry add: id:(0x%x)  Ace_Entry_id (0x%x) \n",cint_field_IFA_datapath_ingress_node_egress_fg_id,
          cint_field_IFA_datapath_ingress_node_egress_entry_handle,cint_field_IFA_datapath_ingress_ace_entry_handle);


      bcm_field_presel_entry_id_info_init(&p_id);
      bcm_field_presel_entry_data_info_init(&p_data);

      p_id.presel_id = cint_field_IFA_datapath_ingress_node_egress_presel_ipv4;
      p_id.stage = fg_info.stage;
      p_data.entry_valid = TRUE;
      p_data.context_id = cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4;

      p_data.nof_qualifiers = 1;

      p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
      p_data.qual_data[0].qual_arg = 0;
      p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
      p_data.qual_data[0].qual_mask = 0x1F;

      rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in dnx_field_presel_set \n", rv);
         return rv;
      }

      printf("Presel (%d) was configured for stage(ePMF) context(%d) fwd_layer(Ipv4) \n",
              cint_field_IFA_datapath_ingress_node_egress_presel_ipv4,cint_field_IFA_datapath_ingress_node_egress_context_id_ipv4);

    return rv;
}

/*
 *  Configuration of trap for ifa 2.0 termination with MD node
 */
int cint_ifa2_termination_with_md_recycle_trap(int unit, int collector_port, int rcy_port)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    int ingress_trap_id = 0;
    int trap_id = 0;
    int flags = 0;

    /* ---------------------- */
    /* Configure Ingress Trap */
    /* ---------------------- */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.dest_port = collector_port;
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;

    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &ingress_trap_id);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    rv = bcm_rx_trap_set(unit, ingress_trap_id, trap_config);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    /* ---------------------- */
    /* Configure Egress Trap */
    /* ---------------------- */
    bcm_rx_trap_config_t_init(&trap_config);

    rv = bcm_rx_trap_type_create(unit, trap_config.flags, bcmRxTrapEgTxIfaEgressMetadata, &trap_id);
    if (BCM_FAILURE(rv))
    {
        SHR_ERR_EXIT(rv, "bcm_rx_trap_type_create failed.");
    }

    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, ingress_trap_id, /*rcy_strength*/15, 0);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (BCM_FAILURE(rv))
    {
        SHR_ERR_EXIT(rv, "bcm_rx_trap_set failed.");
    }

    /* Recycle port configuration */
    rv = bcm_instru_gport_control_set(unit, rcy_port,0,
                                                 bcmInstruGportControlIptTrapToRcyEnable, TRUE);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_instru_gport_control_set \n", rv);
       return rv;
    }

    return rv;

}


/*
 * Example cint for IFA 2.0 Termination Node without Metadata.
 * IFA 2.0 is configured on the device and then field configuration is set:
 *    - iPMF1 will be used to sample IFA 2.0 packets . This is done by detecting IP-protocol type.
 *    - The iPMF will snoop the packet to collector and set IPTProfile for the forwarded packet.
 *    - The ePMF will will reset IPTProfile for the snooped packet only.
 */

int cint_instru_ifa2_termination(int unit)
{
    int rv = BCM_E_NONE;

    if (*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
      /* J2P and above devices require ipt configuration */
      rv = cint_instru_ipt_init(unit);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in ePMF rule creation \n", rv);
         return rv;
      }
    }

    /* Configure Initiator Node Parameters */
    rv = cint_ifa2_node_parameters_config(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }
    /* Configure IFA 2.0 header Parameters */
    rv = cint_config_IFA2_header(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }

    /* Configure Field */
    rv = cint_field_IFA2_datapath_termination_main(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }

    return rv;
}

/**
*  Example to configure IFA 2.0 Termination node.
*    - This example configures IFA 2.0 Termination node, that terminates IFA flow and snoops IFA packet to collector.
*
*  Operation principle:
*      The IFA 2.0 traffic is trapped by IP protocol type, and IFA 2.0 packets are snooped.
*      The original packet is stripped from IFA 2.0 headers and metadata stack,
*      The snooped packet is encapsulated by egress node metadata and trapped to collector after recycling.
*
*  Configuration:
*    - Snooping configuration with tail-edit-profile = IPT_PROFILE_IFA2_TERMINATION.
*    - iPMF1 configuration
*        a. Set field group with 2 qualifiers and 4 actions
*           Qualifiers:
*             - IP protocol type = IFA 2.0
*            Actions:
*             0- Snoop Packet
*             1- Set Tail Edit Profile = IPT_PROFILE_IFA2_TERMINATION
*             2- Set Container for iPMF3 (for both copies)
*    - iPMF3 configuration
*            Qualifiers:
*              0- Container
*            Actions:
*              0- Set INT (for both copies)
*    - ePMF configuration (Configured in cint_field_IFA2_egress_datapath_group)
*         a. Qualifier: Snooped Copy
*            Action:
*             1- Set ACE (for trapping to recycle)
*             2- Set Tail Edit Profile = IPT_PROFILE_IFA2_UDP_GPE
 */
int cint_field_IFA2_datapath_termination_with_md(int unit, int out_port)
{
    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_context_t context_id = cint_field_IFA_datapath_ingress_context_id_ipv4;
    bcm_field_context_t ipmf1_context_id=0;
    bcm_field_action_t action_type = bcmFieldActionSnoop;
    char * name = "IFA_const_initiator";

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t action_info_get_size;
    bcm_field_entry_info_t ent_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualifier_info_get_t qual_info_get_offset;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    bcm_field_context_param_info_t param_info;
    int system_collector_port;
    void *dest_char;
    int id;
    bcm_gport_t snoop_trap_gport_id;
    int mirror_dest_id;
    bcm_port_config_t port_config;
    int cpu_port;
    int system_out_port;
    int rv = BCM_E_NONE;

    /* --------------------*/
    /* iPMF1 configuration */
    /* --------------------*/

    rv = bcm_field_action_info_get(unit,action_type,stage,&action_info_get_size);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_info_get cint_const_action\n", rv);
        return rv;
    }

    /* Action for container */
    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF1;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_a_cont_1b", sizeof(action_info.name));
    rv = bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_create\n", rv);
        return rv;
    }

    /*
     * Configure snooping
     */

    BCM_GPORT_SYSTEM_PORT_ID_SET(system_out_port, out_port);
    rv = cint_field_ip_snoop_set(unit, system_out_port, 1, 0, 0, &snoop_trap_gport_id, &mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d) in cint_field_ip_snoop_set\n", rv);
        return rv;
    }

    /*
     * Create and attach TCAM group in iPMF1
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;

    /* Set quals */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = bcmFieldQualifyIp4Protocol;

    /* Set actions */
    fg_info.nof_actions = 3;
    /* INT command*/
    fg_info.action_types[0] = bcmFieldActionSnoop;
    /* Tail Edit profile */
    fg_info.action_types[1] = bcmFieldActionIPTProfile;
    /* INT command*/
    fg_info.action_types[2] = cint_field_IFA_datapath_ingress_act_container_1b;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    id = cint_field_IFA_datapath_ingress_fg_id;
    printf("FG (%s) created ID: %d \n", name,id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, name, sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    /*
     * Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;

    /* Set Layer for Qualifier to Layer 0 = ETH */
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];

    printf("Attaching field group\n");
    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    printf("FG (%d) Attached to context: %d \n",id,context_id);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_ingress_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_ingress_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;

    /* For termination, fwd layer is 0 so we need to look at layer+1 to look for ipv4 */
    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 1;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF1) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_ingress_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority_initiator;

    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp4;
    ent_info.entry_qual[0].mask[0] = 0x1F;
    ent_info.entry_qual[1].type = bcmFieldQualifyIp4Protocol;
    ent_info.entry_qual[1].value[0] = DEFAULT_IP_PROTOCOL;
    ent_info.entry_qual[1].mask[0] = -1;

    ent_info.nof_entry_actions = 3;
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = snoop_trap_gport_id;

    ent_info.entry_action[1].type = bcmFieldActionIPTProfile;
    ent_info.entry_action[1].value[0] = IPT_PROFILE_IFA2_TERMINATION;
    ent_info.entry_action[2].type = cint_field_IFA_datapath_ingress_act_container_1b;
    ent_info.entry_action[2].value[0] = 1;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_ingress_fg_id, &ent_info, &cint_field_ip_snoop_ent_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  Tail_Profile (3) \n", cint_field_ip_snoop_ent_id);

    id = cint_field_IFA_ipmf2_fg_id;
    printf("FG (%s) created ID: %d \n", name,id);

    /* --------------------*/
    /* iPMF3 configuration */
    /* --------------------*/

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=1;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_q_cont_1b", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_ingress_qual_container_1b);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_action_info_t_init(&action_info);
    action_info.size=1;
    action_info.prefix_size = 9;
    action_info.stage = bcmFieldStageIngressPMF3;
    action_info.action_type = bcmFieldActionIPTCommand;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPT_c_1b", sizeof(action_info.name));
    bcm_field_action_create(unit,0,&action_info,&cint_field_IFA_datapath_ingress_act_IPTCommand_1b);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_IFA_datapath_ingress_qual_container_1b;
    /* Set actions */
    fg_info.nof_actions = 1;
    /* INT command*/
    fg_info.action_types[0] = cint_field_IFA_datapath_ingress_act_IPTCommand_1b;
    fg_info.action_with_valid_bit[0]=FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_dp_IPT_c", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_ingress_fg_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_field_IFA_datapath_ingress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_iPMF3", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info, &cint_field_IFA_datapath_ingress_context_id_ipmf3);
    if (rv != BCM_E_NONE)
    {
    printf("Error (%d), in bcm_field_context_create\n", rv);
    return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    bcm_field_qualifier_info_get_t_init(& qual_info_get_offset);
    bcm_field_qualifier_info_get(unit,bcmFieldQualifyContainer,bcmFieldStageIngressPMF3,&qual_info_get_offset);
    attach_info.key_info.qual_info[0].offset = qual_info_get_offset.offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_ingress_fg_id_ipmf3, cint_field_IFA_datapath_ingress_context_id_ipmf3, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_transit_presel_ipv4;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_ingress_context_id_ipmf3;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set \n", rv);
        return rv;
    }

    printf("Presel (%d) was configured for stage(iPMF3) context(%d) fwd_layer(Ipv4) \n",
    cint_field_IFA_datapath_transit_presel_ipv4,cint_field_IFA_datapath_ingress_context_id_ipv4);

    return rv;
}

/* Example of egress field configuration for IFA 2.0, to be used for for egress node with metadata:
 *   ePMF traps packet by probe_headers and sets ePMF context bits to "IFA-Trap-Context"
 */
int cint_field_IFA2_egress_datapath_group(int unit,int IFA2_IP_protocol)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_t ace_format_id;
    void *dest_char;
    int rv = BCM_E_NONE;

    /*
    * Configure the ACE format.
    */
    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 2;

    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;
    ace_format_info.action_types[1] = bcmFieldActionIPTProfile;

    dest_char = &(ace_format_info.name[0]);
    sal_strncpy_s(dest_char, "AM_ACE", sizeof(ace_format_info.name));
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_format_add \n", rv);
       return rv;
    }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = 2;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextIFATrap;

    ace_entry_info.entry_action[1].type = bcmFieldActionIPTProfile;
    ace_entry_info.entry_action[1].value[0] = IPT_PROFILE_IFA2_UDP_GPE;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &cint_field_IFA_datapath_egress_ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_ace_entry_add \n", rv);
       return rv;
    }
    printf("ACE Format (%d) and ACE entry 0x(%x) created \n", ace_format_id,cint_field_IFA_datapath_egress_ace_entry_handle);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=8;
    dest_char = &(qual_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_eg_probe_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit,0,&qual_info,&cint_field_IFA_datapath_egress_snooped_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;

    /* Set quals */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = cint_field_IFA_datapath_egress_snooped_qual;
    fg_info.qual_types[1] = bcmFieldQualifyRepCopy;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionAceEntryId;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_datapath_egress", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_IFA_datapath_egress_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in ePMF \n", cint_field_IFA_datapath_egress_fg_id);

    /**Attach  context**/
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IFA_IPv4", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, fg_info.stage, &context_info, &cint_field_IFA_datapath_egress_context_id_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    /* Look for the IFA2_type_id at Fwd layer with offset */
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 72;

    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_IFA_datapath_egress_fg_id, cint_field_IFA_datapath_egress_context_id_ipv4, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("Attached  FG (%d) to context (%d)\n",cint_field_IFA_datapath_egress_fg_id,cint_field_IFA_datapath_egress_context_id_ipv4);

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = cint_field_IFA_datapath_egress_presel_ipv4;
    p_id.stage = fg_info.stage;
    p_data.entry_valid = TRUE;
    p_data.context_id = cint_field_IFA_datapath_egress_context_id_ipv4;

    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    p_data.qual_data[0].qual_mask = 0x1F;

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in dnx_field_presel_set \n", rv);
       return rv;
    }

    printf("Presel (%d) was configured for stage(ePMF) context(%d) fwd_layer(Ipv4) \n",
            cint_field_IFA_datapath_egress_presel_ipv4,cint_field_IFA_datapath_egress_context_id_ipv4);

    return rv;
}

/* Example of PMF Egress Entry creation for IFA 2.0, to be used for for Egress nodes
 * with metadata.
 * Snooped IFA 2.0 packets need to be trapped for egress node proccess and recycling.
 * Qualifier is on replica.
 * The Actions set ePMF context bits to "IFA-Trap-Context" for trap context
 */
int cint_field_IFA2_datapath_egress_entry_add(int unit, bcm_field_entry_t *entry_handle)
{

    bcm_field_entry_info_t ent_info;
    int rv=BCM_E_NONE;

    /**Its batter to add entries after attach, for batter bank allocation*/
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 2;
    ent_info.entry_qual[0].type = cint_field_IFA_datapath_egress_snooped_qual;
    ent_info.entry_qual[0].value[0] = DEFAULT_IP_PROTOCOL;
    ent_info.entry_qual[0].mask[0] = 0xff;
    ent_info.entry_qual[1].type = bcmFieldQualifyRepCopy;
    ent_info.entry_qual[1].value[0] = 1;
    ent_info.entry_qual[1].mask[0] = 3;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionAceEntryId;
    ent_info.entry_action[0].value[0] = cint_field_IFA_datapath_egress_ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, cint_field_IFA_datapath_egress_fg_id, &ent_info, entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x) FG_ID (%d) \n Probe_1:(0x%x) Probe_2:(0x%x) Ace_Entry(0x%x)\n",
            *entry_handle,
            cint_field_IFA_datapath_egress_fg_id,
            ent_info.entry_qual[0].value[0],
            ent_info.entry_qual[0].value[1],cint_field_IFA_datapath_egress_ace_entry_handle);

    return rv;
}

/*
 * Example cint for configuring recycle for IFA 2.0 Egress Node with metadata.
 * 1. ePMF will be added for trapping and recycling on egress side:
 *            Qualifier: Snooped Copy
 *            Action:
 *             1- Set ACE (for trapping to recycle)
 *             2- Set Tail Edit Profile = IPT_PROFILE_IFA2_UDP_GPE
 * 2. Trap configuration
 */
int cint_instru_ifa2_egress_recycle(
        int unit,
        int out_port,
        int collector_port)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t entry_handle = 0;
    bcm_instru_ifa_info_t ifa_info;

    /* Configure field */
    rv = cint_field_IFA2_egress_datapath_group(unit,DEFAULT_IP_PROTOCOL);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in ePMF rule creation \n", rv);
       return rv;
    }

    /* Set ePMF to trap IFA packet */
    rv = cint_field_IFA2_datapath_egress_entry_add(unit,&entry_handle);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding ePMF entry \n", rv);
       return rv;
    }

    /* Ports configuration */
   rv = cint_ifa2_termination_with_md_recycle_trap(unit,collector_port,out_port);
   if (rv != BCM_E_NONE)
   {
	  printf("Error (%d), in cint_ifa2_termination_with_md_recycle_trap \n", rv);
	  return rv;
   }
    return rv;
}

/*
 * Example cint for IFA 2.0 Termination Node with Metadata.
 * IFA 2.0 is configured on the device and then field configuration is set:
 *    - iPMF1,3 and ePMF will be used to sample IFA 2.0 packets and set context selection for them.
 *      This is done by detecting IP-protocol type.
 *      Field configuration is detailed in cint_field_IFA2_datapath_termination_with_md
 *    - Incoming IFA 2.0 packet will be snooped in ingress.
 *        Original copy will be peeled from IFA 2.0 header, IFA 2.0 metadata header and metadata
 *        stack and will be forwarded.
 *        Snooped copy will be encapsulated by termination node's metadata, recycled and traped to
 *        collector.
 */
int cint_instru_ifa2_termination_with_md(int unit, int in_port, int recycle_port_0, int recycle_port_1, int out_port)
{
    int rv = BCM_E_NONE;
    int collector_port = 0;

    if (*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
      /* J2P and above devices require ipt configuration */
      rv = cint_instru_ipt_init(unit);
      if (rv != BCM_E_NONE)
      {
         printf("Error (%d), in ePMF rule creation \n", rv);
         return rv;
      }
    }

    /* Configure Node Parameters */
    rv = cint_ifa2_node_parameters_config(unit);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in configuration IFA node parameters \n", rv);
       return rv;
    }

    /* Configure Field */
    rv = cint_field_IFA2_datapath_termination_with_md(unit,out_port);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in adding iPMF1 rule \n", rv);
       return rv;
    }

    /* Configure recycling of ifa packet and forwarding to collector */
    rv = cint_instru_ifa2_egress_recycle(unit, out_port, collector_port);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in cint_instru_ifa2_egress_recycle \n", rv);
       return rv;
    }
    return rv;
}
