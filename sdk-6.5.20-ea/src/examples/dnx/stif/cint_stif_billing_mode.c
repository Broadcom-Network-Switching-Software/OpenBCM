/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * File:        cint_stif_billing_mode.c
 * Purpose:     example for External Statistics configurations 
 * in BILLING report mode 
 * 
 * *
 * The cint include:
 *  - sequence example of configuring extenral statistics in
 *    Billing report mode.
 *  - first - configure soc properties - enable STIF;choose
 *    ports for STIF; select the report mode - BILLING; select
 *    the STIF mode - single mode 1*400G/ dual mode 2*200G/ quad
 *    mode 4*100G; select the ingress and egress report size
 *    - 64b/72b/80b/96b; select the packet size
 *    - 512/1024/1536; select if the ingress will count the
 *    multicast copies as a single
 *  - second - use APIs sequence to connect the sources to the
 *    selected ports, to enable the flow control; select if
 *    packet size adjust will use HeaderTruncate or not; select
 *    the filter groups for drop reasons
 *  - third - use API to configure the record format for ingress
 *    and egress
 * 
 * 
 *   - ACL rule to generate ingress PP stat objects
 *  
 *  SOC property configuration:
 *  ----------------------------------------------------------------------------------------------------------------------------------------------
 *  | init soc properties relevant for BILLING report mode                                                                                      |
 *  ----------------------------------------------------------------------------------------------------------------------------------------------
 *  |ucode_port_<logical_port[0]>=<interface_type>[<interface_id>][_<channel_num>]:core<_core_id>:stat |
 *  |ucode_port_<logical_port[1]>=<interface_type>[<interface_id>][_<channel_num>]:core<_core_id>:stat |
 *  |                                                                                                                                                    |
 *  |                                                                                                                                                    |
 *  |stat_if_enable = 1                                                                                                                                  |
 *  |stat_if_report_mode = BILLING                                                                                                                       |
 *  |stat_if_report_size_ingress = 96                                                                                                                    |
 *  |stat_if_report_size_egress = 96                                                                                                                     |
 *  |stat_if_idle_reports_present = 1                                                                                                                    |
 *  |stat_if_pkt_size = 1536                                                                                                                             |
 *  |stat_if_report_multicast_single_copy = 1                                                                                                            |
 *  ----------------------------------------------------------------------------------------------------------------------------------------------
 *  
 *  Report format elements order:
 *  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  |RecordType (2b)    | PacketSize (14b)| Opcode (0-7)         |MetaData - TC(3b)/DP(2b)/Disposition(1b)/etc.|ElementObj3(20b)|ElementObj2(20b)|ElementObj1(20b)|ElementObj0(20b)|
 *  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  |ingress/egress/null| default element  | according to types|user selects                                                        |user selects            |user selects          |user selects           |user selects           |
 *  |                               | user configures   | user configures     |user configures                                                  |user configures       |user configures    |user configures     |user configures     |
 *  -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  
 *  Example configuration of the billing record format:
 *  INGRESS
 *  record size set to 96b
 *  filled are 87b
 *  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * |RecordType (2b)| PacketSize (14b)| Opcode (8b) | DropPrecedence (2b)| Disposition (1b)| ElementObj2(20b)|ElementObj1(20b)|ElementObj0(20b)|
 *  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  
 * EGRESS 
 * record size set to 96b 
 * filled are 88b
 * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * |RecordType (2b)| PacketSize (14b)| Opcode (8b) | TrafficClass (3b)| Disposition (1b)| ElementObj2(20b)|ElementObj1(20b)|ElementObj0(20b)|
 * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  
 * 
 * This cint invoke function of cint (used to create context when generating the acl rule creating the pp stat object):
 *  cint ../../../src/examples/dnx/field/cint_field_always_hit_context.c
 */

/** logical ports set in the DVAPI test */
bcm_port_t stif_logical_port_0, stif_logical_port_1, stif_logical_port_2, stif_logical_port_3;

/**
 * \brief
 *      cint example of configrung ACL rule to create pp stat
 *      objects
 *  
 *      if reports will include  PP stat objects its user
 *      responsibility to configure them
 *      pp stat objects can be generated via ACL rule
 *      in a record can be included up to 4 stat objects
 * \param [in] unit -unit id 
 * \param [in] core_id - core id 
 * \param [in] in_port - in port 
 * \param [in] stat_id - stat_id - pp stat object
 * \param [in] command_id - command_id - stat object id 
 * \param [in] type_id - type_id [0/1/2/3], default will be used 
 *        0 in this example
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
stif_billing_generate_pp_stat_object(
    int unit,
    int core_id,
    int in_port,
    int stat_id,
    int command_id,
    int type_id)
{
    bcm_field_presel_t cint_stif_presel_id = 10;
    bcm_field_context_t cint_stif_context_id_ipmf1 = 0;
    bcm_field_group_t fg_id;
    bcm_field_entry_t ent_id;
    int rv;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_qualify_t qual_type;
    uint32 qual_value;
    bcm_gport_t gport;

    rv = cint_field_always_hit_context_main(unit,cint_stif_presel_id,bcmFieldStageIngressPMF1,&cint_stif_context_id_ipmf1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_field_always_hit_context_main Ingress\n", rv);
        return rv;
    }
	
    qual_type = bcmFieldQualifyInPort;
    BCM_GPORT_LOCAL_SET(gport, in_port);
    qual_value = gport; 
    
    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /* Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionExtStat0 + command_id; /** according to command id 0/1/2/3 - pp stat object id*/
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }        

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, fg_id, cint_stif_context_id_ipmf1, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    
    /* Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = qual_value;
    ent_info.entry_qual[0].mask[0] = 0x1FF;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    /** The statistics is composed of data (stat_id and opcode (type_id). The first ExtStat action also has a valid bit.*/
    if (command_id == 0)
    {
        ent_info.entry_action[0].value[0] = stat_id;
        ent_info.entry_action[0].value[1] = type_id;
        ent_info.entry_action[0].value[2] = 1;
    }
    else
    {
        ent_info.entry_action[0].value[0] = stat_id;
        ent_info.entry_action[0].value[1] = type_id;
    }
    
    rv = bcm_field_entry_add(unit, 0, fg_id, &ent_info, &ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * \brief
 *      cint example of configrung STIF for Billing mode.
 *      The sequence is:
 *      a. configure STIF soc properties 
 *      b. configure source mapping according to the selected
 *      logical ports
 *      c. enable flow control - optional - relevant for
 *     source EgressDequeue
 *      d. set packet size adjust to use HeaderTruncate -
 *      optional
 *      e. select filter groups for drop reasons - optional
 *      f. configure ingress report format
 *      g. configure egress report format
 * \param [in] unit -unit id
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
stif_billing_configurations_example(
    int unit)
{
    int rv;
    int i, flags = 0, nof_elements_ingress, nof_elements_egress;
    bcm_stat_stif_source_t source;
    bcm_stat_counter_command_id_key_t key;
    bcm_stat_stif_record_format_element_t ingress_record_format_elements[7], egress_record_format_elements[7];
    int core_id = 0, in_port = 0, type_id = 0, command_id[3]={0,1,2}, stat_id[3]={100, 65535, 2048};
    int nof_cores;

    nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);

    /** step 0: enable STIF and configure the given soc
     *  properties */

    /** step 1: configure the source mapping  to the selected
     *  logical ports */
    /** For this example is chosen STIF mode = dual mode 2*200G
     *  - use two logical port - one core 0 and one core
     *    1; connect ingress and egress sources from each core to
     *    different port
     *  
     *   Logical ports are set to the global variable
     *  stif_logical_port via tcl
 */

    /** step 1.1 connect IngressEnqueue source */
    /** core 0 */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    /** source remapping not allowed */
    if (nof_cores > 1)
    {
        /** core 1 */
        source.core = nof_cores- 1;
        source.src_type = bcmStatStifSourceIngressEnqueue;
        rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_stif_source_mapping_set\n");
            return rv;
        }
    }
    
    /** step 1.2 connect EgressDequeue source */
    /** core 0 */
    source.core = 0;
    source.src_type = bcmStatStifSourceEgressDequeue;
    rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    /** source remapping not allowed */
    if (nof_cores > 1)
    {
        /** core 1 */
        source.core = nof_cores - 1;
        source.src_type = bcmStatStifSourceEgressDequeue;
        rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_stif_source_mapping_set\n");
            return rv;
        }
    }

    /** step 2 - OPTIONAL: enable Flow Control - disabled by
     *  default - relevant for IngressDequeue and EgressDequeue  */
    rv = bcm_stat_control_set(unit, flags, bcmStatControlStifFcEnable, 1 );
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_control_set\n");
        return rv;
    }

    /** step 3 - OPTIONAL: set packet size adjust to use
     *  HeaderTruncate flag=BCM_STAT_EXTERNAL to indicate STIF */
    key.core_id = BCM_CORE_ALL;
    rv = bcm_stat_pkt_size_adjust_select_set(unit, BCM_STAT_EXTERNAL, key, bcmStatPktSizeSelectCounterIngressHeaderTruncate, 1 );
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_pkt_size_adjust_select_set\n");
        return rv;
    }

    /** step 4 - OPTIONAL: activate filter groups - use
     *  flag=BCM_STAT_EXTERNAL to indicate STIF */
    /** step 4.1: activate filter group  GlobalReject drop
     *  reasons */
    rv = bcm_stat_counter_filter_group_set(unit, BCM_STAT_EXTERNAL, key, bcmStatCounterGroupFilterGlobalResourcesDrop, 1 );
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_filter_group_set\n");
        return rv;
    }
    /** step 4.2: activate filter group Wred drop reasons  */
    rv = bcm_stat_counter_filter_group_set(unit, BCM_STAT_EXTERNAL, key, bcmStatCounterGroupFilterWredDrop , 1 );
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_filter_group_set\n");
        return rv;
    }

    /** step 5: configure the record format for the ingress */
    /** first - as the format will include 3 pp stat objects ,
     *  they should be generated */
    /** this is example configuration using ACL rule */
    /** for this example - no traffic will be send, use core id
     *  0, in port 0, type id 0 */
    BCM_IF_ERROR_RETURN(stif_billing_generate_pp_stat_object(unit, core_id, in_port, stat_id[0], command_id[0], type_id));
    BCM_IF_ERROR_RETURN(stif_billing_generate_pp_stat_object(unit, core_id, in_port, stat_id[1], command_id[1], type_id));
    BCM_IF_ERROR_RETURN(stif_billing_generate_pp_stat_object(unit, core_id, in_port, stat_id[2], command_id[2], type_id));

    /**
     *  
     *  *  INGRESS
     *  record size set to 96b
     *  filled are 87b
     *  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     * |RecordType (2b)| PacketSize (14b)| Opcode (8b) | DropPrecedence (2b)| Disposition (1b)| ElementObj2(20b)|ElementObj1(20b)|ElementObj0(20b)|
     *  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
     */
    ingress_record_format_elements[0].element_type = bcmStatStifRecordElementObj0;
    ingress_record_format_elements[0].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[1].element_type = bcmStatStifRecordElementObj1;
    ingress_record_format_elements[1].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[2].element_type = bcmStatStifRecordElementObj2;
    ingress_record_format_elements[2].mask = BCM_STAT_FULL_MASK;
    /** for MetaData elements different masks can be set for
     *  example select element TrafficClass (3b) and mask=b'011
     *  can be selected */
    ingress_record_format_elements[3].element_type = bcmStatStifRecordElementIngressDispositionIsDrop; 
    ingress_record_format_elements[3].mask = BCM_STAT_FULL_MASK; 
    ingress_record_format_elements[4].element_type = bcmStatStifRecordElementIngressIncomingDropPrecedence; 
    ingress_record_format_elements[4].mask = BCM_STAT_FULL_MASK; 
    ingress_record_format_elements[5].element_type = bcmStatStifRecordElementOpCode; 
    ingress_record_format_elements[5].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[6].element_type = bcmStatStifRecordElementPacketSize; 
    ingress_record_format_elements[6].mask = BCM_STAT_FULL_MASK;

    nof_elements_ingress = 7;
    rv = bcm_stat_stif_record_format_set(unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_record_format_elements); 
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_record_format_set - INGRESS\n");
        return rv;
    }
    /** step 6: configure the record format for the egress */
    /**
     *  * EGRESS 
     * record size set to 80b 
     * filled are 68b
     * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     * |RecordType (2b)| PacketSize (14b)| Opcode (8b) | TrafficClass (3b)| Disposition (1b)|ElementObj1(20b)|ElementObj0(20b)|
     * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
     *  
     * Egress Objects are not generated in this example, they will have the value 0
     */
    egress_record_format_elements[0].element_type = bcmStatStifRecordElementObj0;
    egress_record_format_elements[0].mask = BCM_STAT_FULL_MASK;
    egress_record_format_elements[1].element_type = bcmStatStifRecordElementObj1;
    egress_record_format_elements[1].mask = BCM_STAT_FULL_MASK;
    /** For EGRESS - MetaData elements should always be used full
     *  - mask BCM_STAT_FULL_MASK should always be set */
    egress_record_format_elements[2].element_type = bcmStatStifRecordElementEgressMetaDataMultiCast;
    egress_record_format_elements[2].mask = BCM_STAT_FULL_MASK;
    egress_record_format_elements[3].element_type = bcmStatStifRecordElementEgressMetaDataTrafficClass;
    egress_record_format_elements[3].mask = BCM_STAT_FULL_MASK;
    egress_record_format_elements[4].element_type = bcmStatStifRecordElementOpCode; 
    egress_record_format_elements[4].mask = 0xFF; 
    egress_record_format_elements[5].element_type = bcmStatStifRecordElementPacketSize; 
    egress_record_format_elements[5].mask = BCM_STAT_FULL_MASK;

    nof_elements_egress = 6;
    rv = bcm_stat_stif_record_format_set(unit, BCM_STAT_EGRESS, nof_elements_egress, egress_record_format_elements);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_record_format_set - EGRESS\n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *      cint example of configrung STIF for Billing mode.
 *  
 *      The sequence is:
 *      a. configure STIF soc properties 
 *      b. configure source mapping according to the selected
 *      logical ports 4 ports* 200G
 *      other configurations are done according to
 *      stif_billing_configurations_example
 * \param [in] unit -unit id
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
stif_billing_configurations_quad_mode_example(
    int unit)
{
    int rv;
    int i, flags = 0, core_id = 0;
    bcm_stat_stif_source_t source;

    int feature_map_source_to_two_stif_ports = *dnxc_data_get(unit, "stif", "config", "map_source_to_two_stif_ports", NULL);
    
    /** verify the split source is supported */
    if (!feature_map_source_to_two_stif_ports)
    {
        printf("Error in stif_billing_configurations_quad_mode_example configuration - split sources to two stif ports is not supported\n", rv);
        return BCM_E_FAIL;
    }

    /** step 0: enable STIF and configure the given soc
     *  properties */

    /** step 1: configure the source mapping  to the selected
     *  logical ports */
    /** For this example is chosen STIF mode = quad mode 4*200G
     *  - use four logical port - two core 0 and two core
     *    1; connect ingress and egress sources from core 0 to
     *    ports from core 0 and core 1 - split one source to two ports
     *  
     *   Logical ports are set to the global variable
     *  stif_logical_port via tcl
 */

    /** step 1.1 connect IngressEnqueue source */
    /** core 0 */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_FIRST_PORT, source, stif_logical_port_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_SECOND_PORT, source, stif_logical_port_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    /** core 1 */
    source.core = 1;
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_FIRST_PORT, source, stif_logical_port_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_SECOND_PORT, source, stif_logical_port_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    
    /** step 1.2 connect EgressDequeue source */
    /** core 0 */
    source.core = 0;
    source.src_type = bcmStatStifSourceEgressDequeue;
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_FIRST_PORT, source, stif_logical_port_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_SECOND_PORT, source, stif_logical_port_3);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    /** core 1 */
    source.core = 1;
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_FIRST_PORT, source, stif_logical_port_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    rv = bcm_stat_stif_source_mapping_set(unit, BCM_STAT_SOURCE_MAPPING_SECOND_PORT, source, stif_logical_port_3);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }

    return BCM_E_NONE;
}
