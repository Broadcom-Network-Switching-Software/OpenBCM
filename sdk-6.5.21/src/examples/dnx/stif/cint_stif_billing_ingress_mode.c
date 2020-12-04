/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * File:        cint_stif_billing_ingress_mode.c
 * Purpose:     example for External Statistics configurations 
 * in INGRESS_BILLING report mode 
 * 
 * *
 * The cint include:
 *  - sequence example of configuring extenral statistics in
 *    IngressBilling report mode.
 *  - first - configure soc properties - enable STIF;choose
 *    ports for STIF; select the report mode - INGRESS_BILLING;
 *    select the STIF mode - single mode 1*400G/ dual mode
 *    2*200G/ quad mode 4*100G; select the ingress 
 *    report size - 64b/72b/80b/96b; select the packet size
 *    - 512/1024/1536; select if the ingress will count the
 *    multicast copies as a single
 *  - second - use APIs sequence to connect the sources to the
 *    selected ports, to enable the flow control; select if
 *    packet size adjust will use HeaderTruncate or not; select
 *    the filter groups for drop reasons
 *  - third - use API to configure the record format for ingress
 *  
 *  
*  SOC property configuration:
 *  ----------------------------------------------------------------------------------------------------------------------------------------------
 *  | init soc properties for INGRESS BILLING report mode                                                                                     |
 *  ----------------------------------------------------------------------------------------------------------------------------------------------
 *  |ucode_port_<logical_port[0]>=<interface_type>[<interface_id>][_<channel_num>]:core<_core_id>:stat |
 *  |ucode_port_<logical_port[1]>=<interface_type>[<interface_id>][_<channel_num>]:core<_core_id>:stat |
 *  |ucode_port_<logical_port[2]>=<interface_type>[<interface_id>][_<channel_num>]:core<_core_id>:stat |
 *  |ucode_port_<logical_port[3]>=<interface_type>[<interface_id>][_<channel_num>]:core<_core_id>:stat |
 *  |                                                                                                                                                     |
 *  |                                                                                                                                                     |
 *  |stat_if_enable = 1                                                                                                                                   |
 *  |stat_if_report_mode = BILLING                                                                                                                        |
 *  |stat_if_report_size_ingress = 64                                                                                                                     |
 *  |stat_if_idle_reports_present = 1                                                                                                                     |
 *  |stat_if_pkt_size = 1024                                                                                                                              |
 *  |stat_if_report_multicast_single_copy = 1                                                                                                             |
 *  -----------------------------------------------------------------------------------------------------------------------------------------------
 *  
 *  Report format elements order:
 *  ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  |RecordType (2b)    | PacketSize (14b)| Opcode (0-7)      |MetaData - TC(3b)/DP(2b)/Disposition(1b)/etc.|ElementObj3(20b)|ElementObj2(20b)|ElementObj1(20b)|ElementObj0(20b)|
 *  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  |ingress/egress/null| default element | according to types|user selects                                                       |user selects            |user selects         |user selects            |user selects          |
 *  |                                | user configures | user configures     |user configures                                                 |user configures       |user configures   |user configures      |user configures    |
 *  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *  
 *  Example configuration of the billing record format:
 *  INGRESS
 *  record size set to 64b
 *  filled are 62b
 * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * |RecordType (2b)| PacketSize (14b)| TmDropReason (mask 1b) | Core (1b)| TrafficClass (mask 2b) | PpMetaData (mask 1b)| QueueNumber (mask 1b)|ElementObj1(20b)|ElementObj0(20b)|
 * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */

/** logical ports set in the DVAPI test */
bcm_port_t stif_logical_port_0, stif_logical_port_1, stif_logical_port_2, stif_logical_port_3;

/**
 * \brief
 *      cint example of configrung STIF for Ingress Billing
 *      mode. The sequence is:
 *      a. configure STIF soc properties 
 *      b. configure source mapping according to the selected
 *      logical ports
 *      c. enable flow control - optional - relevant only for
 *      IngressDequeue
 *      d. set packet size adjust to use HeaderTruncate -
 *      optional
 *      e. select filter groups for drop reasons - optional
 *      f. configure ingress report format
 * \param [in] unit -unit id
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
stif_ingress_billing_configurations_example(
    int unit)
{
    int rv;
    int i, flags = 0, nof_elements_ingress;
    bcm_stat_stif_source_t source;
    bcm_stat_counter_command_id_key_t key;
    bcm_stat_stif_record_format_element_t ingress_record_format_elements[8];
    int nof_cores;
    nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
    /** step 0: enable STIF and configure the given soc
     *  properties */

    /** step 1: configure the source mapping  to the selected
     *  logical ports */
    /** For this example is chosen STIF mode = quad mode 4*100G
     *  - use four logical ports - two on core 0 and two on core 1;
     *    connect ingress sources from each core to different
     *    port
     *  
     *  Logical ports are set to the global variable
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
    /** devices with one core support less instances */
    if (nof_cores > 1)
    {
        /** core 1 */
        source.core = nof_cores - 1;
        source.src_type = bcmStatStifSourceIngressEnqueue;
        rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port_3);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_stif_source_mapping_set\n");
            return rv;
        }
        /** step 1.2 connect IngressDequeue source */
        /** core 0 */
        source.core = 0;
        source.src_type = bcmStatStifSourceIngressDequeue;
        rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_stif_source_mapping_set\n");
            return rv;
        }
    }
    /** core 1 */
    source.core = nof_cores - 1;
    source.src_type = bcmStatStifSourceIngressDequeue;
    rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    
    /** step 2: enable Flow Control - disabled by default -
     *  relevant only for source IngressDequeue */
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
    /** step 4.1: activate filter group  Error, drop reasons */
    rv = bcm_stat_counter_filter_group_set(unit, BCM_STAT_EXTERNAL, key, bcmStatCounterGroupFilterMiscDrop, 1 );
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_filter_group_set\n");
        return rv;
    }
    
    /** step 5: configure the record format for the ingress */
    /** 
     * *  INGRESS
     *  record size set to 64b
     *  filled are 62b
     * ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     * |RecordType (2b)| PacketSize (14b)| TmDropReason (mask 1b) | Core (1b)| TrafficClass (mask 2b) | PpMetaData (mask 1b)| QueueNumber (mask 1b)|ElementObj1(20b)|ElementObj0(20b)|
     * --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
     *  
     *  
     * For 1 core devices: Instead Core have DispositionIsDrop
 */ 
    ingress_record_format_elements[0].element_type = bcmStatStifRecordElementObj0;
    ingress_record_format_elements[0].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[1].element_type = bcmStatStifRecordElementObj1;
    ingress_record_format_elements[1].mask = BCM_STAT_FULL_MASK;
    ingress_record_format_elements[2].element_type = bcmStatStifRecordElementIngressQueueNumber;
    ingress_record_format_elements[2].mask = 1;
    ingress_record_format_elements[3].element_type = bcmStatStifRecordElementIngressPpMetaData;
    ingress_record_format_elements[3].mask = 2;
    ingress_record_format_elements[4].element_type = bcmStatStifRecordElementIngressTrafficClass;
    ingress_record_format_elements[4].mask = 6;
    ingress_record_format_elements[5].element_type = (nof_cores > 1) ? bcmStatStifRecordElementIngressCore : bcmStatStifRecordElementIngressDispositionIsDrop;
    ingress_record_format_elements[5].mask = BCM_STAT_FULL_MASK; 
    ingress_record_format_elements[6].element_type = bcmStatStifRecordElementIngressTmDropReason;
    ingress_record_format_elements[6].mask = 1; 
    ingress_record_format_elements[7].element_type = bcmStatStifRecordElementPacketSize; 
    ingress_record_format_elements[7].mask = BCM_STAT_FULL_MASK;

    nof_elements_ingress = 8;
    rv = bcm_stat_stif_record_format_set(unit, BCM_STAT_INGRESS, nof_elements_ingress, ingress_record_format_elements); 
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_record_format_set - INGRESS\n");
        return rv;
    }
    
    return BCM_E_NONE;
}
