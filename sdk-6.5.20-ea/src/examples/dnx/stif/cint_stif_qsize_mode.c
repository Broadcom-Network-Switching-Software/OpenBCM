/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * File:        cint_stif_qsize_mode.c
 * Purpose:     example for External Statistics configurations 
 * in QSIZE report mode 
 * 
 * *
 * The cint include:
 *  - sequence example of configuring extenral statistics in
 *    QueueSize report mode.
 *  - first - configure soc properties - enable STIF;choose
 *    ports for STIF; select the report mode - QSIZE; select the
 *    STIF mode - single mode 1*400G/ dual mode 2*200G/ quad
 *    mode 4*100G; select the packet size - 512/1024/1536;
 *    select the scrubber rate range - 0-65535; scrubber sram
 *    buffers/sram pdbs/ dram bdb thresholds (0-15); selective
 *    report queue ranges; scrubber queue ranges
 *  - second - use APIs sequence to connect the sources to the
 *    selected ports, to enable the flow control; set scrubber
 *    and queue ranges via API
 *   
 *  
 *  SOC property configuration:
 *  ----------------------------------------------------------------------------------------------------------------------------------------------
 *  | init soc properties relevant for QSIZE report mode                                                                                         |
 *  ----------------------------------------------------------------------------------------------------------------------------------------------
 *  |ucode_port_<logical_port[0]>=<interface_type>[<interface_id>][_<channel_num>]:core<_core_id>:stat |
 *  |                                                                                                                                                                         |
 *  |                                                                                                                                                                         |
 *  |stat_if_enable = 1                                                                                                                        |
 *  |stat_if_report_mode = QSIZE                                                                                                               |
 *  |stat_if_idle_reports_present = 1                                                                                                          |
 *  |stat_if_pkt_size = 512                                                                                                                    |
 *  |stat_if_scrubber_rate_min = 0                                                                                                             |
 *  |stat_if_scrubber_rate_max = 65535                                                                                                         |
 *  |stat_if_scrubber_sram_buffers_th_(0-14) = 2048                                                                                            |
 *  |stat_if_scrubber_sram_pdbs_th_(0-14) = 1024                                                                                               |
 *  |stat_if_scrubber_bdb_th_(0-14) = 1024                                                                                                     |
 *  |stat_if_selective_report_queue_min_0/1 = 0                                                                                                |
 *  |stat_if_selective_report_queue_max_0/1 = 65535                                                                                            |
 *  |stat_if_scrubber_queue_min = 0                                                                                                            |
 *  |stat_if_scrubber_queue_max = 65535                                                                                                        |
 *  -----------------------------------------------------------------------------------------------------------------------------------------------
 */

/** logical port set in the DVAPI test */
bcm_port_t stif_logical_port;

/**
 * \brief
 *      cint example of configrung STIF for QueueSize mode.
 *      The sequence is:
 *      a. configure STIF soc properties 
 *      b. configure source mapping according to the selected
 *      logical ports
 *      c. enable flow control - optional - relevant only for
 *      source IngressDequeue
 *      d. set queue ranges - optional - can be done via API or
 *      via soc properties
 *      e. set scrubber ranges - optional - can be done via API
 *      or soc properties
 * \param [in] unit -unit id 
 * \param [in] gport - gport 
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
stif_qsize_configurations_example(
    int unit,
    bcm_gport_t gport)
{
    int rv;
    int i, flags = 0;
    bcm_stat_stif_source_t source;
    bcm_cosq_range_t queue_range, scrubber_range;
    int max_queue = *dnxc_data_get(unit, "ipq", "queues", "nof_queues", NULL) - 1;

    /** step 0: enable STIF and configure the given soc
     *  properties */

    /** step 1: configure the source mapping  to the selected
     *  logical ports */
    /** For this example is chosen STIF mode = single mode 1*400G
     *  - use one logical port - use core 0 and
     *    connect all sources to this port
     *  
     *  Logical ports are set to the global variable
     *  stif_logical_port via tcl
 */

    /** step 1.1 connect IngressEnqueue source */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressEnqueue;
    rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    /** step 1.2 connect IngressDequeue source */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressDequeue;
    rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_stif_source_mapping_set\n");
        return rv;
    }
    /** step 1.3 connect IngressScrubber source */
    source.core = 0;
    source.src_type = bcmStatStifSourceIngressScrubber;
    rv = bcm_stat_stif_source_mapping_set(unit, flags, source, stif_logical_port);
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

    /** step 3: set Queue ranges */
    queue_range.range_start = 0;
    queue_range.range_end = max_queue;
    rv = bcm_cosq_control_range_set(unit, gport, flags, bcmCosqStatIfQueues , queue_range);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_cosq_control_range_set with control bcmCosqStatIfQueue \n");
        return rv;
    }
    /** step 4: set Scrubber queue ranges */
    scrubber_range.range_start = 100;
    scrubber_range.range_end = max_queue/16;
    rv = bcm_cosq_control_range_set(unit, gport, flags, bcmCosqStatIfScrubberQueues , scrubber_range);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_cosq_control_range_set with control bcmCosqStatIfScrubberQueues \n");
        return rv;
    }

    return BCM_E_NONE;
}

