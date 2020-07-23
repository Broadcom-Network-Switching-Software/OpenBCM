/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Priority Over Port Example (Ardon) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_tm_prio_o_port_reduce_resources.c
 * Purpose: Example for Ardon application. Includes Priority Over Port Cosq Application.
 * Reference: BCM88650/BCM88660 TM Device Driver (ATMF/Ardon)
 *
 *  Provide an example of building scheduling scheme for "Priority Over Port" with 8 priorities.
 *  Means, to connect VOQ connectors to the appropriate HR according to the HRs TC.
 *  This cint directed to system with reduced resources (Ardon).    
 *  Also includes examples for: adding ports, MC, ISQ, CGM and other configurations.    
 *  
 * Creating Scheduling Scheme done by the following steps: 
 * - Egress: Create for each local port: following CLs, VOQ connectors. 
 * - Ingress: Create VOQs for each system port. 
 * - Connect Ingress VOQs <=> Egress VOQ connectors. 
 * 
 * This application example assumes Main port configuration via SOC property. 
 * Main port number should be given as parameter (tm_internal_pkt_port) to the Cint, This port need to be configured with TM header type (tm_port_header_type).
 * In Ardon this port Interface should be TM_INTERNAL_PKT.
 * The Cint will create duplicated system ports attached to the Main port, the number of duplicated system ports will be according to cint parameter nof_local_ports.
 * Means, if tm_internal_pkt_port=2 and  nof_local_ports=4 the ports in the system will be: 2,3,4,5.
 * 
 * Notice that the scheduling scheme assumes static IDs for VOQ connectors, VOQs ... 
 * The mapping depended on the local and system ports in the system.
 * The static mapping is done using utility functions: convert_modport_to_sysport_id, convert_sysport_id_to_base_voq_id, convert_modport_to_base_voq_connector.
 * The mapping can adjust by changing the above functions and setting OFFSET_START_OF_VOQ, OFFSET_START_OF_VOQ_CONNECTORS.
 * Port - VOQ - VOQ connectors mapping can be verified using diag shell command:
     gport
 *
 *
 * Adjust to different system configuration:
 *  1. Change relevant soc properties.
 *   - Adjust (if needed) SOC property in 88202 section (uncomment PriorityOPort lines and comment 'Default SDK Application' lines).
 *   - Disable 88650 ucode configuration.
 *   - Disable 88650 Dram selection. 
 *  2. Call ardon_appl_init() with the relevant system configuration, reference can be found in ardon_appl_exmaple():
       (this is not a specific example for a relevant system, but a general reference)
         cd ../../../../src/examples/dpp
         cint cint_tm_prio_o_port_reduce_resources.c
         cint
         int unit = 0;
         int nof_local_ports = 6;
         int mc_group_num = 5;
         int nof_isqs = 512;
         int cgm_thr_enable = 1;
         ardon_appl_example(unit, nof_local_ports, mc_group_num, nof_isqs, cgm_thr_enable);
         quit;

 *  2a. Specific example for mgmt system can be found in ardon_mgmt_appl_example().
 *     The system is controlled by a central cpu which controlls the following devices:
 *     ---------   --------   --------- 
 *     | Fap_0 |---|  Fe  |---| Fap_1 |
 *     ---------   --------   --------- 
 *      unit 0      unit 1     unit 2
 *
         cd ../../../../src/examples/dpp
         cint cint_tm_prio_o_port_reduce_resources.c
         cint
         int unit = 0;
         int nof_local_ports = 6;
         int mc_group_num = 5;
         int nof_isqs = 512;
         int cgm_thr_enable = 1;
         ardon_mgmt_appl_example(unit, nof_local_ports, mc_group_num, nof_isqs, cgm_thr_enable);
         quit;

 *
 *  Note: In global_info_init() we assume there is a central CPU.
 *        If this is not the case, change g_info.central_cpu to 0.
 *

 *
 * Multicast Configuration:
 * ========================
 *   The function ardon_appl_mc() demonstrate the use of BCM Multicast API. This function open several MC groups , according to mc_group_num. 
 *   The group type (ING/EGR/BOTH) is configured according to mc_type. The first group MC id is set according to mc_id and all the other groups will be allocated in increasing order.
 *   One port is added to each MC group, to the first group the port mc_port_member is added and to the other groups the allocated port will be in increasing order.
 *   If we are not using fabric (as in EMUL) the following Configuration is needed:
 *     s IPT_FORCE_LOCAL_OR_FABRIC FORCE_LOCAL=1
 *   To use MC-ID in the range of < 255:
 *     gress_multicast_direct_bitmap_max=255
 *
 *   Send packet MC Packet:
       tr 40 Tagged=true LengthStart=65 LengthEnd=128 Chains=1 PktsPerChain=1 LengthInc=100 MacDest=00:00:01:00:00:00 MacSrc=00:00:00:e3:00:e3 # MC-id = 256 = 0x100 port 2
       tr 40 Tagged=true LengthStart=65 LengthEnd=128 Chains=1 PktsPerChain=1 LengthInc=100 MacDest=00:00:01:01:00:00 MacSrc=00:00:00:e3:00:e3 # MC-id = 257 = 0x101 port 3
       tr 40 Tagged=true LengthStart=65 LengthEnd=128 Chains=1 PktsPerChain=1 LengthInc=100 MacDest=00:00:01:09:00:00 MacSrc=00:00:00:e3:00:e3 # MC-id = 265 = 0x109 port 11
 *
 * Running the Application on Ardon pcid:
 * ======================================
 *
 *   Application:
         cd ../../../src/examples/dpp
         cint cint_tm_prio_o_port_reduce_resources.c
         cint
         int unit = 0;
         int nof_local_ports = 6;
         int mc_group_num = 5;
         int nof_isqs = 512;
         int cgm_thr_enable = 1;
         ardon_appl_example(unit, nof_local_ports, mc_group_num, nof_isqs, cgm_thr_enable);
         quit;
  
 * Running the Application on Arad/Arad+:
 * ======================================
 *   Following example for Main port = 1
 *   SOC Property:
 *     diag_cosq_disable=1
 *     ucode_port_1.BCM88650=10GBase-R12 # All other ports are disabled
 *     fap_device_mode.BCM88650=TM
 *     egress_multicast_direct_bitmap_max.BCM88650=255
 *     dtm_flow_mapping_mode_region_65.BCM88650=1
 *     dtm_flow_mapping_mode_region_66.BCM88650=1
 *     tm_port_header_type_in_0.BCM88650=TM     # for all CPU ports
 *     tm_port_header_type_out_0.BCM88650=TM    # for all CPU ports
 *     stack_enable.BCM88650=0
 *
 *
 *   Application:
       cd ../../../../src/examples/dpp
       cint cint_tm_prio_o_port_reduce_resources.c
       cint
       int unit = 0;
       int nof_local_ports = 28;
       int mc_group_num = 20;
       int nof_isqs = 512;
       int cgm_thr_enable = 1;
       ardon_appl_example(unit, nof_local_ports, mc_group_num, nof_isqs, cgm_thr_enable);
       quit;

 *
 *   Traffic:
 *     Packet header according to ITMH (tm_port_header_type_2.BCM88650=TM). MacDest[19:0]=dest,   MacDest[24:22]=tc.
 *     1. Send traffic to port=5 tc=3 with Packet DMA:
         tr 40 Tagged=true LengthStart=65 LengthEnd=128 Chains=1 PktsPerChain=1 LengthInc=100 MacDest=00:cc:00:05:00:00 MacSrc=00:00:00:e3:00:e3
 *       
 *     2. Send traffic to port=3 tc=0 with IRE Packet GEN:
         s IRE_REGI_PKT_DATA REGI_PKT_DATA=0x000C0003123456C4900112233445566778899009876543210123456789987654
         s IRE_REG_FAP_PORT_CONFIGURATION REG_REASSEMBLY_CONTEXT=1 REG_PORT_TERMINATION_CONTEXT=1
         s IRE_REGISTER_INTERFACE_PACKET_CONTROL REGI_PKT_SEND_DATA=0x0 REGI_PKT_ERR=0x0 REGI_PKT_SOP=1 REGI_PKT_EOP=0 REGI_PKT_BE=31
         s IRE_REGISTER_INTERFACE_PACKET_CONTROL REGI_PKT_SEND_DATA=0x1 REGI_PKT_ERR=0x0 REGI_PKT_SOP=1 REGI_PKT_EOP=0 REGI_PKT_BE=31
         s IRE_REGI_PKT_DATA REGI_PKT_DATA=0x00007FFFFF03003F003F03DAA1C9877367494287118008711805000000010000
         s IRE_REGISTER_INTERFACE_PACKET_CONTROL REGI_PKT_SEND_DATA=0x0 REGI_PKT_ERR=0x0 REGI_PKT_SOP=0 REGI_PKT_EOP=0 REGI_PKT_BE=31
         s IRE_REGISTER_INTERFACE_PACKET_CONTROL REGI_PKT_SEND_DATA=0x1 REGI_PKT_ERR=0x0 REGI_PKT_SOP=0 REGI_PKT_EOP=0 REGI_PKT_BE=31
         s IRE_REGI_PKT_DATA REGI_PKT_DATA=0xAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
         s IRE_REGISTER_INTERFACE_PACKET_CONTROL REGI_PKT_SEND_DATA=0x0 REGI_PKT_ERR=0x0 REGI_PKT_SOP=0 REGI_PKT_EOP=1 REGI_PKT_BE=31
         s IRE_REGISTER_INTERFACE_PACKET_CONTROL REGI_PKT_SEND_DATA=0x1 REGI_PKT_ERR=0x0 REGI_PKT_SOP=0 REGI_PKT_EOP=1 REGI_PKT_BE=31
 *      
 *     3. using IXIA:
 *       - use Z:\projects\NTSW_LAB\SOFTWARE_VERSION\Arad\in_work\ixia_cfgs\PrioOverPort.prt to load IXIA port property.
 *       - Limit Bandwidth to 1G:
             c
             bcm_gport_t e2e_gport;
             BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport,1);
             print bcm_cosq_gport_bandwidth_set(0, e2e_gport, 0, 0, 1000000,0);
             quit;
 *       - Make "Packet Group statistic view" for the IXIA port you are using.
 *       - Send traffic with different priorities.
 *       - verify Traffic is according to Port Over Priority (At the statistic view you should see only the groups with the hi priority traffic).
 *
 *   Diag:
       show c
       diag pp PKT_associated_TM_info
       diag pp fdt
       diag count g nz
 *       
 * OCB only mode (ocb enabled + No dram's):
 * ========================================
 * SOC property:
 *   bcm886xx_ocb_databuffer_size.BCM88650=1024 ==> dram budder size need to be equal or lower than ocb buffer size
 *   ext_ram_present.BCM88650=0                 ==> or ext_ram_present.BCM88202=0
 *   bcm886xx_ocb_enable.BCM88650=1             ==> this is the default, just validate
 *
 *
 * Ardon EMUL setup:
 * ===========
 * Enable to following SOC property:
 *   diag_emulator_partial_init.BCM88202=1
 *   schan_timeout_usec.BCM88202=100000000000
 *   tdma_timeout_usec.BCM88202=100000000000
 *   tslam_timeout_usec.BCM88202=100000000000
 *   phy_null.BCM88202=0
 *
 * When running ardon on emulation it is advised to use the following(to save time due to the slow pace of the emulator) :
    cint cint_tm_prio_o_port_reduce_resources.c
    cint
    int unit = 0;
    int nof_local_ports = 6;
    int mc_group_num = 5;
    int nof_isqs = 0; 
    int cgm_thr_enable = 0;
    ardon_appl_example(unit, nof_local_ports, mc_group_num, nof_isqs, cgm_thr_enable);
    quit;
 *
 * Diag shell:
    g NBI_TX_BYTE_TOTAL_CNT
    g NBI_TX_EOP_PKT_TOTAL_CNT
    g NBI_TX_SOP_PKT_TOTAL_CNT
    g NBI_TX_ERR_PKT_TOTAL_CNT

    g NBI_RX_BYTE_TOTAL_CNT
    g NBI_RX_EOP_PKT_TOTAL_CNT
    g NBI_RX_SOP_PKT_TOTAL_CNT
    g NBI_RX_ERR_PKT_TOTAL_CNT

    g NBI_TX_BYTE_CNT
    g NBI_TX_EOP_PKT_CNT
    g NBI_TX_SOP_PKT_CNT
    g NBI_TX_ERR_PKT_CNT

    g NBI_RX_BYTE_CNT
    g NBI_RX_EOP_PKT_CNT
    g NBI_RX_SOP_PKT_CNT
    g NBI_RX_ERR_PKT_CNT
 *
 *
 * Ardon FC:
 * =========
 * Ardon recieves FCV (Flow Control Vector) with the size of 256 bit.
 * FCV vector can be mapped only to CL's 0-255.
 * During Ardon init the table CFC_NIF_SHR_MAP is configured.
 *
 * In this priotiry over port schedualing scheme the CL are connected according to the formula: (port * g_info.num_cos + cos).
 * Means, port0,priority0 is connected to CL_index0; port0,priority1 is connected to CL_index1; port1,priority1 is connected to CL_index9.
 *
 * Default FCV to CL SCH mapping is: FCV[i] = port * cos_num + cos. 
 * Means that bits[7:0] in FCV are all TCs at port0, and bits[15:8] are all TCs at port1 and so on.
 * 
 * Using BCM API each bit in FCV can be mapped to a d different CL (0-255). example can be found in:\
       cd ../../../../src/examples/dpp
       cint cint_tm_prio_o_port_reduce_resources.c
       cint
       int unit = 0;
       int num_tm_ports = 28;
       int num_cos = 8;
       ardon_fc_shr_example(unit, num_tm_ports, num_cos);
       quit;
 *
 * CL's indexes can be calculated according to : sch_elem_index = (64 * 1024) + cl_index * 4. Meand cl_index=1 is sch_elem_index=65540.
 * SOC property configuration for working with CL (in Priority over port configuration) is:
    custom_feature_cl_scheduler_fc=1
    dtm_flow_mapping_mode_region_65.BCM88202=1
    dtm_flow_mapping_mode_region_66.BCM88202=1
 * Notice that enableing the SOC Property will disable the HR FC mapping.
 *
 * Default FCV to CL SCH mapping is: FCV[i] = port * cos_num + cos. 
 * Means that bits[7:0] in FCV are all TCs at port0, and bits[15:8] are all TCs at port1 and so on.
 * Relevant Registers/Memories:
    g CFC_CFC_ENABLERS
    d CFC_NIF_PFC_MAP
    g SCH_REG_1BC
    d CFC_NIF_SHR_MAP
 
    g CFC_NIF_FCV_STATUS_SEL
    g CFC_NIF_FCV_STATUS

    g CFC_FRC_NIF_SHR
 *
 *
 * Note:
 *  - This Application example run on top of Default application (Set diag_cosq_disable=1 and not diag_disable=1). Missing:
 *      - RX enable
 *      - internal port (ERP) configuration across units.
 *      - My Modid set (bcm_stk_my_modid_set), Module enable (bcm_stk_module_enable).
 *      - Also configure Main port (add gport)
 *      - bcmCosqControlPacketLengthAdjust
 *
 * Ardon shaping example:
 * ======================
 * This application defines 512 shaping queues (ISQs) (8 priorities * 64 ISQ ports ) starting queue number 3072.
 * These queues has BW of 2.5Gb/s).
 * in order to test this configuration frames with the following header need to be send:
        isq_flow            isq base number + tc
        0x80000             indicates flow id header + extention
        0xc0000             indicates unicast
        set itmh_hdr        [expr ( ($tc<<22) | $isq_flow | 0x80000 )]
        set isq_hdr_ext     [expr (0xc0000 | $dest_port)]
 * call to the application
 * ISQ configuration:
 *   isq_bw = 2500000;
 *   rv = ardon_shaping_configuration(unit, nof_isqs, MAX_COS, isq_bw);
 *   if (BCM_FAILURE(rv)) {
 *       printf("ardon_shaping_configuration: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
 *       return rv;
 *   }
 * 
 
 * Ardon GCM example:
 * ==================
 * This function responsible for configuring Ardon per port thresholds according ardon_egq_thresholds.xls.
 * This function configures only the 1 port 100G/120G with 8 priority example. 
 * egrs_thr_t structure defined in order to save threshold values. we can see the different thresholds that are configured at this example:
 *              unicast flow control packet descriptors threshold
 *              unicast flow control data buffers threshold
 *              unicast drop packet descriptors threshold
 *              unicast drop data buffers threshold
 *              multicast drop packet descriptors threshold
 *              multicast drop data buffers threshold
 *              unicast flow control packet descriptors interface threshold
 *              multicast flow control packet descriptors interface threshold
 *              unicast flow control packet descriptors per traffic class threshol
 *              unicast flow control data buffers per traffic class threshold
 *              unicast drop packet descriptors per traffic class threshold
 *              unicast drop data buffers per traffic class threshold
 *              multicast drop packet descriptors per traffic class per color threshold
 *              multicast drop data buffers per traffic class per color threshold
 *              multicast reserved packet discriptors per traffic class threshold
 * thresholds are differ by pd/db, uc/mc, fc/drop, tc, color, reserved & interface cariteria.
 * set_eqr_thresholds help function defined to receive these parameters and configure proper values.
 * call to the application
 * rv = ardon_thr_example(unit, tm_internal_pkt_port, MAX_COS);
 * if (BCM_FAILURE(rv)) {
 *      printf("ardon_thr_example: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
 *      return rv;
 *  }
 */
int MAX_COS = 8;
int MAX_NUM_PORTS = 256;
int MAX_NUM_CPU_PORTS = 8;
int MAX_NUM_DEVICE = 8;
int MAX_MODIDS_PER_DEVICE = 2;

int OFFSET_START_OF_VOQ = 32;
int OFFSET_START_OF_VOQ_CONNECTORS = 32;

int ISQ_QUEUE_START = 5120;
int ISQ_QUEUE_END   = 6143;

struct info_t {
    int         my_modid_idx[MAX_NUM_DEVICE];
    /* Number of devices in the system */
    int         nof_devices;
    /* Main application port */
    bcm_port_t tm_internal_pkt_port;
    /* Number of physical ports per device */
    int         num_ports;
    /* Number of internal ports (OLP,ERP) per device */
    int         num_internal_ports;
    /* Number of CPU ports per device */
    int         num_cpu_ports;
    /* Number of cos levels (COSQ) */
    int         num_cos;
    /* CPU system port */
    int         cpu_sys_port;
    int         cpu_ports[MAX_NUM_CPU_PORTS];
    int         internal_ports[MAX_NUM_DEVICE*MAX_NUM_PORTS];
    int         offset_start_of_internal_ports;
    /* Offset given for a system port on each device */
    int         offset_start_of_sys_port;
    /* Offset given for a voq */
    int         offset_start_of_voq;
    /* Offset given for a voq connector */    
    int         offset_start_of_voq_connector;
    /* Present local ports on each device */
    bcm_gport_t dest_local_gport[MAX_NUM_PORTS];    
    /* 
     * Present all VOQ connectors on egress device 
     * For each dest local port create 2 voq connectors, for each device 
     */
    bcm_gport_t gport_ucast_voq_connector_group[MAX_NUM_DEVICE][MAX_NUM_PORTS];
    /* 
     * Present all VOQ on ingress device, 
     * For each system port create VOQ 
     */
    bcm_gport_t gport_ucast_queue_group[MAX_NUM_DEVICE][MAX_NUM_PORTS];
    /* Present all FQ scheduling on egress device, one for each local port */
    bcm_gport_t gport_ucast_scheduler[MAX_NUM_PORTS][MAX_COS];    
    
    /* If true, a single CPU control two units. Otherwise, this script runs on two CPUs. */
    int central_cpu;
    int cfg_2nd_unit;
    
};
info_t g_info; /* global info */

/* 
 * egress threshold parameters
 */
typedef struct egrs_thr_t{
    int uc_fc_pds_th;                   /* unicast flow control packet descriptors threshold*/
    int uc_fc_dbs_th;                   /* unicast flow control data buffers threshold*/
    int uc_drop_pds_th;                 /* unicast drop packet descriptors threshold*/
    int uc_drop_dbs_th;                 /* unicast drop data buffers threshold*/
    int mc_drop_pds_th;                 /* multicast drop packet descriptors threshold*/
    int mc_drop_dbs_th;                 /* multicast drop data buffers threshold*/
    int uc_fc_pd_if_th;                 /* unicast flow control packet descriptors interface threshold*/
    int mc_fc_pd_if_th;                 /* multicast flow control packet descriptors interface threshold*/
    int tc_uc_fc_pds_th[MAX_COS];       /* unicast flow control packet descriptors per traffic class threshold*/
    int tc_uc_fc_dbs_th[MAX_COS];       /* unicast flow control data buffers per traffic class threshold*/
    int tc_uc_drop_pds_th[MAX_COS];     /* unicast drop packet descriptors per traffic class threshold*/
    int tc_uc_drop_dbs_th[MAX_COS];     /* unicast drop data buffers per traffic class threshold*/
    int tc_mc_drop_pds_th[MAX_COS][4];  /* multicast drop packet descriptors per traffic class per color threshold*/
    int tc_mc_drop_dbs_th[MAX_COS][4];  /* multicast drop data buffers per traffic class per color threshold*/
    int mc_pd_reserved[MAX_COS];        /* multicast reserved packet discriptors per traffic class threshold*/        
};

/* 
 * Utils function: Converting between IDs
 */
/* return the index for the internal port stored in the internal_ports[] array */
int port_internal_id_get(int modid, int port)
{
    int port_i;
    for (port_i=0; port_i<g_info.num_internal_ports; port_i++) {
        if (g_info.internal_ports[modid * MAX_NUM_PORTS + port_i] == port) {
            return port_i;
        }
    }
    return -1;
}

/* Convert (module id,port) id to sysport id */
int convert_modport_to_sysport_id(int modid,int port)
{
    /* In current configuration local port=0 is CPU port and system port=255 */
    int internal_port_idx;
    if (port == 0) {
        port = g_info.cpu_sys_port;
    }
    internal_port_idx = port_internal_id_get(modid, port);
    if (internal_port_idx != -1) {
        return (g_info.offset_start_of_sys_port * modid + internal_port_idx + g_info.offset_start_of_internal_ports);
    }
    return g_info.offset_start_of_sys_port * modid + port;
}

/* 
 * Convert sysport to base VOQ id 
 * The conversation includes: 
 * VOQ ID = offset + (sysport-1)*num_cos. 
 * offset specify the start of the VOQ range (right after the FMQ range) by default 4.
 */
int convert_sysport_id_to_base_voq_id(int sysport)
{
    /* Assume, no system port 0 */
    if (sysport == 0) {
        return -1;
    }
    return g_info.offset_start_of_voq + (sysport - 1) * g_info.num_cos;
}

/* 
 * Convert (local port,ingress_modid) to VOQ connector base 
 * VOQ connector ID  = offset + egress_local_port * nof_devices * max_cos + ingress_modid*max_cos
 * specify offset start of the VOQ connector ID, by default 32. 
 * max_cos is 8
 */

int convert_modport_to_base_voq_connector(int local_port,int ingress_modid)
{
    /* In case of internal ports (higher than 255) have a port of max + (local_port - 255) */
    if (local_port > 255) {
        local_port = 256 + (local_port % 256);
    }
    return g_info.offset_start_of_voq_connector + local_port * g_info.nof_devices * MAX_MODIDS_PER_DEVICE * MAX_COS + ingress_modid * MAX_COS;
}

/* 
 * Purpose: Initializes Looks at current configuration and populates g_info based on that
 */
int global_info_init(int unit, int nof_devices, bcm_port_t tm_internal_pkt_port, int nof_local_ports)
{
    int 
        rv = BCM_E_NONE,
        port_i = 0,
        modid = 0,
        idx = 0,
        port = 0,
        num_cpu_ports = 0;
    bcm_port_config_t port_config;    
    bcm_pbmp_t pbmp;
  
    for (modid = 0; modid < nof_devices; modid++) {
        g_info.my_modid_idx[modid] = modid;
    }
    g_info.num_cos = MAX_COS;
    g_info.nof_devices = nof_devices;    
    g_info.tm_internal_pkt_port = tm_internal_pkt_port;
    g_info.num_ports = nof_local_ports;

    /* set num_ports in stk_init */
    idx = 0;
    BCM_GPORT_LOCAL_SET(g_info.dest_local_gport[idx], g_info.tm_internal_pkt_port);
    for (idx = 1; idx < g_info.num_ports; idx++ ) {
        BCM_GPORT_LOCAL_SET(g_info.dest_local_gport[idx], g_info.tm_internal_pkt_port + idx);
    }
    
    /* Add CPU ports */
    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv)) {
        printf("Error, in bcm_port_config_get.\n");
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_OR(pbmp, port_config.cpu);
    idx = g_info.num_ports;
    print pbmp;
    BCM_PBMP_ITER(pbmp, port) {
        printf("idx=%d, port=%d\n", idx, port);
        BCM_GPORT_LOCAL_SET(g_info.dest_local_gport[idx], port);
        g_info.cpu_ports[idx - g_info.num_ports] = port;
        idx++;
    }
    
    BCM_PBMP_COUNT(pbmp, num_cpu_ports);
    printf("num_cpu_ports=%d\n", num_cpu_ports);
    g_info.num_ports += num_cpu_ports;
    g_info.num_cpu_ports = num_cpu_ports;
    
    /* print all ports array */
    for (idx = 0; idx < g_info.num_ports; idx++ ) {
        printf("g_info.dest_local_gport[%d]=0x%x\n", idx, g_info.dest_local_gport[idx]);
    }
    
    
    g_info.cpu_sys_port = 255;
    /* Offset sysport per device */
    g_info.offset_start_of_sys_port = 256;
    /* Offset VOQ */
    g_info.offset_start_of_voq = OFFSET_START_OF_VOQ;
    /* Offset VOQ connector */
    g_info.offset_start_of_voq_connector = OFFSET_START_OF_VOQ_CONNECTORS;

    /* on local device internal ports will start at 240 */
    g_info.offset_start_of_internal_ports = 240;
    g_info.num_internal_ports = 0;

    for (modid = 0; modid < nof_devices; modid++) {
        for (port_i = 0; port_i < MAX_NUM_PORTS; port_i++) {
            g_info.internal_ports[modid*MAX_NUM_PORTS+port_i] = -1;
        }
    }
    g_info.central_cpu = 1;
    g_info.cfg_2nd_unit = 0;

    return rv;
}

int appl_ardon_stk_init(int unit)
{
    int 
        rv = BCM_E_NONE,
        idx,
        port, 
        sys_port, 
        modid,
        config_all_ports,
        nof_ports;
    bcm_gport_t 
        modport_gport,
        sysport_gport;
    bcm_pbmp_t pbmp;        
    
    /* 
     * Creation of system ports in the system Iterate over port. 
     * on mgmt device, port 1 and cpu ports are already defined, so we need to skip them.
     * on single device they are also defined, so we need to skip them.
     * on single device on a multi device system, they are defined only for this device,
     * so we need to define them for other devices (i.e. other modids).
     */
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        if ((g_info.cfg_2nd_unit) || (!g_info.cfg_2nd_unit && modid == g_info.my_modid_idx[unit/MAX_MODIDS_PER_DEVICE])) {
            config_all_ports = 0;
            nof_ports = g_info.num_ports + g_info.num_internal_ports - g_info.num_cpu_ports;
        } else {
            config_all_ports = 1;
            nof_ports = g_info.num_ports + g_info.num_internal_ports;
        }

        for (idx = 0; idx < nof_ports; idx++) {
            if (idx == 0 && !config_all_ports) {
                continue;
            }
            
            if (config_all_ports && idx >= nof_ports - g_info.num_cpu_ports) {
                port = g_info.cpu_ports[idx - (nof_ports - g_info.num_cpu_ports)];
            } else {
                port = g_info.tm_internal_pkt_port + idx;
            }

            /* System port id depends on modid + port id */ 
            sys_port = convert_modport_to_sysport_id(modid, port);
            
            BCM_GPORT_MODPORT_SET(modport_gport, modid * MAX_MODIDS_PER_DEVICE, port);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
            /* printf("idx(%d) modid(%d) port(%d) sysport(%d) dest_local(0x%08x)\n", idx, modid, port, sys_port, g_info.dest_local_gport[idx]); */
            
            /*
             * get q-pair for this port, set to egq_dsp_ptr.OutTmPort= q-pair of tm_internal_pkt_port
             */
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_PORT_ADD(pbmp, port);
            /* print pbmp; */
            if (modid == g_info.my_modid_idx[unit/MAX_MODIDS_PER_DEVICE]) {
                rv = bcm_port_egress_set(unit,  g_info.tm_internal_pkt_port, modid*MAX_MODIDS_PER_DEVICE,  pbmp);
                if (BCM_FAILURE(rv)) {
                    printf("Error, in bcm_port_egress_set, unit %d, modid_idx %d\n", unit, modid);
                    return rv;
                }
            }
            
            rv = bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
            if (BCM_FAILURE(rv)) {
                printf("bcm_stk_sysport_gport_set(%d, %d, %d) failed. Error:%d\n", unit, sysport_gport, modport_gport, rv);
                return rv;
            }           
        }
    }
    
    return rv;
}

/* This function create the scheduling scheme */
int cosq_init_ing_egr_alloc(int unit) 
{
    int                         rv = BCM_E_NONE;
    int                         idx, cos, port, tm_port;
    uint32                      flags;
    int queue_range = 0;
    int modid, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t sysport_gport,modport_gport, e2e_tc_gport, tc_gport;
    bcm_cosq_gport_info_t g_port = {0};
    int int_flags;
    int local_erp_port[2] = {-1, -1};
    int num_erp_ports = 0;
    int core, core_idx ;
    bcm_cosq_voq_connector_gport_t config ;
    int queues_in_region_div_2, halves_in_voq_connector_id ;
    int dpp_device_cosq_nbr_queues_in_region ;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info ;
    bcm_port_mapping_info_t mapping_info ;

    /* 
     * Before creating VOQs, User must specify the range of the FMQs in the device.
     * In most cases, where fabric multicast is only defined by packet tc, range should set between 0-3. 
     * Set range that is different than 0-3, need to change fabric scheduler mode. 
     */     
    queue_range = 0;
    rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMin,queue_range);
    if (BCM_FAILURE(rv)) {
        printf("bcm_fabric_control_set queue_id :%d failed. Error:%d (%s)\n", queue_range, rv, bcm_errmsg(rv));
        return rv;
    }
    
    queue_range = 3;
    rv = bcm_fabric_control_set(unit,bcmFabricMulticastQueueMax,queue_range);
    if (BCM_FAILURE(rv)) {
        printf("bcm_fabric_control_set queue_id :%d failed. Error:%d (%s)\n", queue_range, rv, bcm_errmsg(rv));
        return rv;
    }
    /* Get ERP port */
    int_flags =
    BCM_PORT_INTERNAL_EGRESS_REPLICATION;
    rv = bcm_port_internal_get(unit, int_flags, MAX_NUM_PORTS, local_erp_port, &num_erp_ports);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_internal_get failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    /* 
     * Creating Schedling Scheme 
     * This is done by the following steps: 
     * - Create num_cos priorities for Main Port. Replace HR mode to enhance and set HR to TCG. Create SP between the HR queues.
     * - Egress: Create for each local port: following FQs, VOQ connectors. 
     * - Ingress: Create VOQs for each system port. 
     * - Connect Ingress VOQs <=> Egress VOQ connectors (done in cosq_init_ing_egr_connect). 
     * Pay attention the scheduling scheme assumes static IDs for VOQ connectors,VOQs. 
     * This is depended by the local and system ports in the system.
     * Conversion is done static using utils functions. 
     */
     
    /* Create num_cos priorities for Main Port */
    printf("Create num_cos priorities for Main Port\n");
    for (cos = 0; cos < g_info.num_cos; cos++) {
        /* Replace HR mode to enhance */
        flags = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED | BCM_COSQ_GPORT_REPLACE);
        
        BCM_COSQ_GPORT_E2E_PORT_TC_SET(e2e_tc_gport, BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[0]));
        rv = bcm_cosq_gport_add(unit, 0, cos, flags, &e2e_tc_gport);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_add replace 0hr scheduler cos:%d failed. Error:%d (%s)\n", cos, rv, bcm_errmsg(rv));
            return rv;
        }

        /* HR to TCG */
        rv = bcm_cosq_gport_sched_set(unit, e2e_tc_gport, cos, BCM_COSQ_SP0, 0);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_sched_set replace e2e_tc_gport scheduler cos:%d failed. Error:%d (%s)\n", cos, rv, bcm_errmsg(rv));
            return rv;
        }

        BCM_COSQ_GPORT_PORT_TC_SET(tc_gport, BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[0]));
        rv = bcm_cosq_gport_sched_set(unit, tc_gport, cos, BCM_COSQ_SP0, 0);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_sched_set replace tc_gport scheduler cos:%d failed. Error:%d (%s)\n", cos, rv, bcm_errmsg(rv));
            return rv;
        }
    }  
     
    /* Stage I: Egress Create CL */
    printf("Stage I: Egress Create CL\n");
    for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
        /* Create CL per traffic class i, attach it to HR SPi */
        for (cos = 0; cos < g_info.num_cos; cos++) {
            
            BCM_COSQ_GPORT_E2E_PORT_TC_SET(e2e_tc_gport, BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[0])); /* Alwayes get the HR for Main port */
            g_port.in_gport = e2e_tc_gport;
            g_port.cosq = cos;
            rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &g_port);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_handle_get cos:%d failed. Error:%d\n", cos, rv);
                return rv;
            }
            
            flags = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP | BCM_COSQ_GPORT_WITH_ID);
            BCM_GPORT_SCHEDULER_SET(g_info.gport_ucast_scheduler[idx][cos], (80 * 1024) + (idx * g_info.num_cos + cos) * 4);

            /* printf("bcm_cosq_gport_add: idx=%d, cos=%d, idx=%d\n", idx, cos, idx); */
            rv = bcm_cosq_gport_add(unit, g_port.out_gport, 1, flags, g_info.gport_ucast_scheduler[idx][cos]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add cl scheduler idx:%d cos:%d failed. Error:%d\n", idx, cos, rv);
                return rv;
            }

            /* printf("bcm_cosq_gport_add: idx=%d, cos=%d, idx=%d, gport_ucast_scheduler[idx][cos]=0x%x\n", idx, cos, idx, g_info.gport_ucast_scheduler[idx][cos]); */
            rv = bcm_cosq_gport_sched_set(unit, g_info.gport_ucast_scheduler[idx][cos], 0, BCM_COSQ_SP0 + cos, 0);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_sched_set cos scheduler-cl idx:%d, cos:%d failed. Error:%d\n", idx, cos, rv);
                return rv;
            } 

            /* attach hr scheduler to cl */
            /* printf("bcm_cosq_gport_attach: idx=%d, cos=%d, idx=%d\n", idx, cos, idx); */
            rv = bcm_cosq_gport_attach(unit, g_port.out_gport, g_info.gport_ucast_scheduler[idx][cos], 0  );
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_attach cos scheduler-cl idx:%d, cos:%d failed. Error:%d\n", idx, cos, rv);
                return rv;
            } 
        } /* cos */
    } /* idx. dest_local_gport[idx] = port */

    /* Stage I: Egress Create VOQ connectors */
    printf("Stage I: Egress Create VOQ connectors\n");
    /* printf("Stage I: g_info.central_cpu %d g_info.nof_devices %d g_info.num_ports %d g_info.num_internal_ports %d\n",g_info.central_cpu, g_info.nof_devices, g_info.num_ports, g_info.num_internal_ports); */

    dpp_device_cosq_nbr_queues_in_region = 1024 ;
    queues_in_region_div_2 = dpp_device_cosq_nbr_queues_in_region/2 ;

    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            /* printf("Stage I: idx %d modid %d\n",idx, modid); */
            /* create voq connector - with ID */
            flags = BCM_COSQ_GPORT_VOQ_CONNECTOR; 

            port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);

            /* Skip ERP port if not far end on ingress */
            for(core_idx = 0 ; core_idx < num_erp_ports ; core_idx++) {
                if ((local_erp_port[core_idx] == port) && (modid == g_info.my_modid_idx[modid])) {
                    continue;
                }
            }

            rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
            if (BCM_FAILURE(rv)) {
                printf("bcm_port_get port %d failed. Error %d (%s)\n", port, rv, bcm_errmsg(rv));
                return rv;
            }

            flags |= BCM_COSQ_GPORT_WITH_ID; 
            voq_connector_id  = convert_modport_to_base_voq_connector(port, modid*MAX_MODIDS_PER_DEVICE);
            /*
             * We only allow flow ids which are odd multiples of (DPP_DEVICE_COSQ_NBR_QUEUES_IN_REGION/2)
             * So each block of 512 connectors is shifted to offset twice as large as the original.
             */
            halves_in_voq_connector_id = voq_connector_id / queues_in_region_div_2 ;

            /* printf("voq_connector_id at start=%d\n", voq_connector_id) ; */

            voq_connector_id = voq_connector_id % queues_in_region_div_2 ;
            voq_connector_id = (halves_in_voq_connector_id * queues_in_region_div_2 * 2) + voq_connector_id ;

            /* printf("voq_connector_id at end  =%d\n", voq_connector_id) ; */

            core = mapping_info.core ;
            BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(g_info.gport_ucast_voq_connector_group[modid][idx], voq_connector_id,core);

            BCM_COSQ_GPORT_E2E_PORT_SET(config.port, port);
            config.flags = flags ;
            config.numq = g_info.num_cos ;
            config.remote_modid = modid * MAX_MODIDS_PER_DEVICE;
            /*
             * from Arad on, all setups assume 2 cores on ingress.
             */
            config.nof_remote_cores = MAX_MODIDS_PER_DEVICE ;
                            
            /* printf("modid=%d, idx=%d, port=%d, voq_connector_id=0x%lX\n", modid, idx, port, voq_connector_id) ; */

            rv = bcm_cosq_voq_connector_gport_add(unit, &config, &g_info.gport_ucast_voq_connector_group[modid][idx]);
    
            if (rv != BCM_E_NONE) {
                printf("cosq_init_ing_egr_alloc(): bcm_cosq_voq_connector_gport_add() failed: $rv\n");
                return rv;
            }
            
            for (cos = 0; cos < g_info.num_cos; cos++) {
                /* Each VOQ connector attach suitable CL traffic class */
                /* printf("bcm_cosq_gport_sched_set: idx=%d, g_info.gport_ucast_voq_connector_group[modid][idx]=0x%x, cos=%d\n", idx, g_info.gport_ucast_voq_connector_group[modid][idx], cos); */
                rv = bcm_cosq_gport_sched_set(unit, g_info.gport_ucast_voq_connector_group[modid][idx], cos, BCM_COSQ_SP0,0);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_sched_set connector idx:%d cos:%d failed. Error:%d(%s)\n", idx, cos, rv, bcm_errmsg(rv));
                    return rv;
                }
               /*  printf("bcm_cosq_gport_sched_set: idx=%d, gport_ucast_voq_connector_group[modid][idx]=0x%x, g_info.gport_ucast_scheduler[idx][cos]=0x%x\n", idx, g_info.gport_ucast_voq_connector_group[modid][idx], g_info.gport_ucast_scheduler[idx][cos]); */
                rv = bcm_cosq_gport_attach(unit, g_info.gport_ucast_scheduler[idx][cos], g_info.gport_ucast_voq_connector_group[modid][idx], cos);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_attach fq scheduler-connector idx:%d cos:%d failed. Error:%d (%s)\n", idx, cos, rv, bcm_errmsg(rv));
                    return rv;
                }
            } /* cos */
        } /* idx */
    } /* modid */
    
    /* Stage 2: Ingress: Create VOQs for each system port. */
    printf("Stage 2: Ingress: Create VOQs for each system port.\n");
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {

            /* printf("Stage 2: idx %d modid %d\n",idx, modid); */

            flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
             port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);
            /* Skip ERP port if not far end on ingress */
            for (core_idx = 0; core_idx < num_erp_ports; core_idx++) {
                if ((local_erp_port[core_idx] == port) && (modid == g_info.my_modid_idx[modid])) {
                    continue;
                }
            }

            rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
            if (BCM_FAILURE(rv)) {
                printf("bcm_port_get port %d failed. Error %d (%s)\n", port, rv, bcm_errmsg(rv));
                return rv;
            }
                           
            tm_port = mapping_info.tm_port;
            BCM_GPORT_MODPORT_SET(modport_gport, modid * MAX_MODIDS_PER_DEVICE + mapping_info.core, tm_port);

            flags |= BCM_COSQ_GPORT_WITH_ID;
            /* printf("idx=%d, modid=%d, port=%d, modport_gport=0x%x,\n", idx, modid, port, modport_gport); */
            rv = bcm_stk_gport_sysport_get(unit, modport_gport, &sysport_gport);
            if (BCM_FAILURE(rv)) {
               printf("bcm_stk_gport_sysport_get get sys port failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
               return rv;
            }
            
            sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
            voq_base_id = convert_sysport_id_to_base_voq_id(sysport_id);

            /* printf("Stage 2: voq_base_id %d\n",voq_base_id);  */

            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(g_info.gport_ucast_queue_group[modid][idx], voq_base_id);

            /*printf("idx=%d, modid=%d, port=%d, modport_gport=0x%x, sysport_gport=0x%x, sysport_id=0x%x, voq_base_id=0x%x, gport_ucast_queue_group[modid][idx]=0x%x\n", 
                    idx, modid, port, modport_gport, sysport_gport, sysport_id, voq_base_id, g_info.gport_ucast_queue_group[modid][idx]); */
            rv = bcm_cosq_gport_add(unit, modport_gport, g_info.num_cos, flags, &g_info.gport_ucast_queue_group[modid][idx]);
            if (BCM_FAILURE(rv)) {
                printf("bcm_cosq_gport_add UC queue failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
               return rv;
            }
            for (cos = 0; cos < g_info.num_cos ; cos++){
                rv = bcm_cosq_gport_sched_set(unit, g_info.gport_ucast_queue_group[modid][idx], cos, BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY, 0);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_sched_set() BCM_COSQ_DELAY_TOLERANCE_100G_LOW_DELAY failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }
    
    /* printf("cosq_init_ing_egr_alloc: Exit\n"); */

    return rv;
}

int cosq_init_ing_egr_connect(int unit, int mymodid) 
{
    int rv = BCM_E_NONE;
    int idx, port, core_idx;    
    bcm_cosq_gport_connection_t connection;
    int modid, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t voq_connector_gport, sysport_gport, modport_gport, voq_base_gport;    
    int local_erp_port[2] = {-1, -1}, num_erp_ports;
    int int_flags;
    int cos;

    /* Get ERP port */
    int_flags = BCM_PORT_INTERNAL_EGRESS_REPLICATION;
    rv = bcm_port_internal_get(unit, int_flags, MAX_NUM_PORTS, local_erp_port, &num_erp_ports);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_internal_get failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* Stage 3: Connect Ingress VOQs <=> Egress VOQ connectors. */
    /* Ingress: connect voqs to voq connectors */
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            for (cos = 0; cos < g_info.num_cos; cos++) {
                connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
                connection.remote_modid = modid * MAX_MODIDS_PER_DEVICE;
                connection.voq = g_info.gport_ucast_queue_group[modid][idx];
                port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);

                /* Skip ERP port if not far end on ingress */
                for (core_idx = 0; core_idx < num_erp_ports; core_idx++) {
                    if ((local_erp_port[core_idx] == port) && (modid == g_info.my_modid_idx[modid])) {
                        continue;
                    }
                }
                connection.voq_connector = g_info.gport_ucast_voq_connector_group[mymodid][idx];
                
                rv = bcm_cosq_gport_connection_set(unit, &connection);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_connection_set ingress modid: %d, idx:%d failed. Error:%d (%s)\n", modid, idx, rv, bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }            

    /* Egress: connect voq connectors to voqs */
    for (modid = 0; modid < g_info.nof_devices; modid++) {
        for (idx = 0; idx < g_info.num_ports + g_info.num_internal_ports; idx++) {
            for (cos = 0; cos < g_info.num_cos; cos++) {
                connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
                connection.remote_modid = modid * MAX_MODIDS_PER_DEVICE;
                connection.voq_connector = g_info.gport_ucast_voq_connector_group[modid][idx];
                connection.voq = g_info.gport_ucast_queue_group[mymodid][idx];

                port = BCM_GPORT_LOCAL_GET(g_info.dest_local_gport[idx]);
                /* Skip ERP port if not far end on ingress */
                for (core_idx = 0; core_idx < num_erp_ports; core_idx++) {
                    if ((local_erp_port[core_idx] == port) && (modid == g_info.my_modid_idx[modid])) {
                        continue;
                    }
                }
                
                rv = bcm_cosq_gport_connection_set(unit, &connection);
                if (BCM_FAILURE(rv)) {
                    printf("bcm_cosq_gport_connection_set egress modid: %d, idx:%d failed. Error:%d (%s)\n", modid, idx, rv, bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }       

    return rv;
}

/*
 * Purpose: Configure COSQ Application.
 * - Run the Cosq Application (Scheduling scheme) (cosq_init)
 * Parameters:
 *  - unit:
 *  - modid:
 *  - nof_units: Number of devices in the system. cannot be above 8
 *  - unit_arr: An array with unit ids of the rest of units controlled by this CPU. 
 *  - modid_arr: An array with mod ids of the rest of units controlled by this CPU. 
 */
int appl_ardon_cosq_init(int unit, int modid, int nof_units, int* unit_arr, int* modid_arr)
{
    int rv = BCM_E_NONE;
    int unit_idx;   

    /*
     * COSQ Init
     */
    
    rv = cosq_init_ing_egr_alloc(unit);
    if (BCM_FAILURE(rv)) {
        printf("cosq_init_ing_egr_alloc: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    if (g_info.cfg_2nd_unit) {
        for (unit_idx = 0; unit_idx < nof_units - 1; unit_idx++) {
            rv = cosq_init_ing_egr_alloc(unit_arr[unit_idx]);
            if (BCM_FAILURE(rv)) {
                printf("cosq_init_ing_egr_alloc: failed. unit: %d, Error:%d (%s) \n", unit_arr[unit_idx], rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }
    
    rv = cosq_init_ing_egr_connect(unit, modid);
    if (BCM_FAILURE(rv)) {
        printf("cosq_init_ing_egr_connect: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    if (g_info.cfg_2nd_unit) {
        for (unit_idx = 0; unit_idx < nof_units - 1; unit_idx++) {
            rv = cosq_init_ing_egr_connect(unit_arr[unit_idx], modid_arr[unit_idx]);
            if (BCM_FAILURE(rv)) {
                printf("cosq_init_ing_egr_connect: failed. unit: %d, Error:%d (%s) \n", unit_arr[unit_idx], rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }
    return rv;
}

/*
 * Purpose: Initialize Ardon Application.
 * This is the main function of this cint example. The following steps are being done:
 * - Setup the diag init structure. (global_info_init)
 * - Run the Cosq Application (Scheduling scheme) (cosq_init)
 * - Add Ports to the main Ardon interface (appl_ardon_stk_diag_init)
 * Parameters:
 *  - unit:
 *  - modid:
 *  - tm_internal_pkt_port - Main Ardon interface local port number
 *  - nof_local_ports - Number of local ports map to the Main Ardon port 
 *  - nof_units: Number of devices in the system. cannot be above 8
 *  - unit_arr: An array with unit ids of the rest of units controlled by this CPU. 
 *  - modid_arr: An array with mod ids of the rest of units controlled by this CPU. 
 */
int ardon_appl_init(int unit, bcm_port_t tm_internal_pkt_port, int nof_local_ports, int modid, int nof_units, int* unit_arr, int* modid_arr)
{
    int rv = BCM_E_NONE;
    int unit_idx;
    
    if (nof_units > MAX_NUM_DEVICE) {
        printf("Number of units (%d) is more than %d, is negative ", nof_units, MAX_NUM_DEVICE);
        return BCM_E_PARAM;
    }
    
    /* misc init should be called first */
    printf("global_info_init\n");
    rv = global_info_init(unit, nof_units, tm_internal_pkt_port, nof_local_ports);
    if (BCM_FAILURE(rv)) {
        printf("global_info_init: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }    
    
    g_info.cfg_2nd_unit = (g_info.central_cpu && (nof_units > 1));
    printf("g_info.cfg_2nd_unit = %d\n", g_info.cfg_2nd_unit);

    printf("appl_ardon_stk_init\n");
    rv = appl_ardon_stk_init(unit);
    if (BCM_FAILURE(rv)) {
        printf("appl_ardon_stk_init: failed. unit %d Error:%d (%s) \n", unit, rv, bcm_errmsg(rv));
        return rv;
    }
    if (g_info.cfg_2nd_unit) {
        for (unit_idx = 0; unit_idx < nof_units - 1; unit_idx++) {
            rv = appl_ardon_stk_init(unit_arr[unit_idx]);
            if (BCM_FAILURE(rv)) {
                printf("appl_ardon_stk_init: failed. Error:%d (%s) \n", unit, rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }
    
    printf("appl_ardon_cosq_init\n");    
    rv = appl_ardon_cosq_init(unit, modid, nof_units, unit_arr, modid_arr);
    if (BCM_FAILURE(rv)) {
        printf("appl_ardon_cosq_init: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    return rv;
}



/*
 * Multicast Utility Func
 */
int open_multicast_group(int unit, int mc_id, int is_egress) {
    bcm_error_t rv = BCM_E_NONE;
  int         flags;

  if(is_egress == 0) {
      /* Ingress MC */
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_INGRESS_GROUP;
  } else if (is_egress == 1) {
      /* Egress MC */
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP;
  } else {
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP|BCM_MULTICAST_INGRESS_GROUP;
  }  

  rv = bcm_multicast_create(unit, flags, &mc_id);
  if (rv != BCM_E_NONE) {
    printf("Error, in _multicast_create, mc_id mc_id=%d\n", mc_id);
    return rv;
  }

  return rv;
}

int add_multicast_replication(int unit, int mc_id, bcm_gport_t gport, int cud, int is_egress) {
  bcm_error_t rv = BCM_E_NONE;

  if(is_egress == 0) {
    rv = bcm_multicast_ingress_add(unit, mc_id, gport, cud);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_multicast_ingress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, gport);
      return rv;
    }
  } else {
    rv = bcm_multicast_egress_add(unit, mc_id, gport, cud);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_multicast_egress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, gport);
      return rv;
    }
  }

  return rv;
}

int ardon_appl_mc(int unit, int mc_group_num, int mc_type, int mc_id, int nxt_mc_id, int mc_port_member)
{
    int 
        rv = BCM_E_NONE,
        i;
    bcm_gport_t mc_gport_member;
    
    for (i = 0; i < mc_group_num; i++) {
        
        /* Open Multicast Group  */
        printf("ardon_appl_mc(): Open Multicast Group, mc_id=%d, mc_type=%d\n", mc_id + i, mc_type);        
        rv = open_multicast_group(unit, mc_id + i, mc_type);
        if (rv != BCM_E_NONE) {
            printf("Error, open_multicast_group(). mc_id=%d\n", mc_id + i);
            return rv;
        }
            
        /* Add MC members */
        printf("ardon_appl_mc():Add MC member. mc_port_member=0x%x, nxt_mc_id=0x%x\n", mc_port_member + i, nxt_mc_id + i);
        BCM_GPORT_SYSTEM_PORT_ID_SET(mc_gport_member, mc_port_member + i);
        rv = add_multicast_replication(unit, mc_id + i, mc_gport_member, nxt_mc_id + i, mc_type);
        if (rv != BCM_E_NONE) {
            printf("Error, add_multicast_replication(). i=%d, mc_gport_member=0x%x\n", i, mc_port_member + i);
            return rv;
        }
    }

    return rv;
}

/* 
 * Ardon FC configuration Example 
 * The following configuration set FCV[i] = port + (num_tm_ports * cos).
 * Means, that FCV bits[27:0] are for all 28 ports (num_tm_ports) with priority 0, and bits[55:28] are for all 28 ports of priority 1 and so on.
 */
int ardon_fc_shr_example(int unit, int num_tm_ports, int num_cos) 
{
    int 
        rv = BCM_E_NONE,
        port,
        cos;
    bcm_gport_t gport_cl_sch;
    bcm_cosq_fc_endpoint_t
        source,
        target;

    printf("ardon_fc_shr_example(): num_tm_ports=%d, num_cos=%d\n", num_tm_ports, num_cos);
    for (cos = 0; cos < num_cos; cos++) {
        for (port = 0; port < num_tm_ports; port++) {
            BCM_GPORT_SCHEDULER_SET(gport_cl_sch, (64 * 1024) + (port * num_cos + cos) * 4);

            BCM_GPORT_CONGESTION_SET(source.port, 0x0); /* port number does not have any meanning */

            source.calender_index = port + cos * num_tm_ports;
            target.port = gport_cl_sch;

            printf("ardon_fc_shr_example(): port=%d, cos=%d, source.calender_index=%d, target.port=0x%x\n", 
                   port, cos, source.calender_index, target.port);
            rv = bcm_cosq_fc_path_add(unit,bcmCosqFlowControlReception, &source, &target);
            if (rv != BCM_E_NONE) {
                printf("ardon_fc_shr_example(): bcm_cosq_fc_path_add() Fail !!!\n");
                return rv;
            }
        }
    }

    /* Testing: Remove FCV[255] */
    printf("ardon_fc_shr_example(): Disable FCV[255].\n");
    BCM_GPORT_SCHEDULER_SET(gport_cl_sch, (64 * 1024) + (port + cos * num_tm_ports) * 4);
    BCM_GPORT_CONGESTION_SET(source.port, 0x0); /* port number does not have any meanning */
    source.calender_index = 255;
    target.port = gport_cl_sch;
    rv = bcm_cosq_fc_path_delete(unit,bcmCosqFlowControlReception, &source, &target);
    if (rv != BCM_E_NONE) {
        printf("ardon_fc_shr_example(): bcm_cosq_fc_path_delete() Fail !!!\n");
        return rv;
    }

    return rv;

}

int scheduler_queue_config(int unit, bcm_gport_t cl_element, bcm_gport_t queue, int nof_cos) {
 
    bcm_gport_t voq_con, mod_port;
    bcm_error_t rv = BCM_E_NONE;
    int modid, i;

    rv = bcm_stk_modid_get(unit, &modid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stk_modid_get\n");
        return rv;
    }
    BCM_GPORT_MODPORT_SET(mod_port, modid, 0); /*required only for passing modid to API*/
    rv = bcm_cosq_gport_add(unit, mod_port, nof_cos, BCM_COSQ_GPORT_VOQ_CONNECTOR, &voq_con);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_add mod_port=0x%x, nof_cos=%d\n", mod_port, nof_cos);
        return rv;
    }
    
    /*attch VOQ conn to sched scheme*/
    for (i = 0; i < nof_cos; i++) {
        rv = bcm_cosq_gport_attach(unit, cl_element,  voq_con, i);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_cosq_gport_attach cl_element=0x%x, voq_con=0x%x\n", cl_element, voq_con);
            return rv;
        }
    }
    
    /*set connection*/
    bcm_cosq_gport_connection_t connection;
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = modid;
    connection.voq = queue;
    connection.voq_connector = voq_con;
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_connection_set connection.remote_modid=0x%x, connection.voq=0x%x, connection.voq_connector=0x%x\n",
                                                        connection.remote_modid, connection.voq, connection.voq_connector);
        return rv;
    }
    
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_connection_set connection.remote_modid=0x%x, connection.voq=0x%x, connection.voq_connector=0x%x\n",
                                                        connection.remote_modid, connection.voq, connection.voq_connector);
        return rv;
    }
    return rv;
}

/* 
 * Ardon shaping configuration Example 
 * This function defines nof_isqs and define isq_bw for them all together. this is done by connecting 
 * all of the isqs to one cl and define bw it. 
 */
int ardon_shaping_configuration(int unit, int nof_isqs, int nof_cos, int isq_bw) {
    
    bcm_error_t           rv = BCM_E_NONE;
    bcm_gport_t queue;
    bcm_gport_t sys_port;
    bcm_gport_t cl_element;
    bcm_gport_t isq_root;
    int i;
    
    /*set ISQs range*/

    /* printf("ardon_shaping_configuration. nof_isqs %d nof_cos %d isq_bw %d\n",nof_isqs, nof_cos, isq_bw); */

    rv = bcm_fabric_control_set(unit, bcmFabricShaperQueueMin, ISQ_QUEUE_START);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_control_set bcmFabricShaperQueueMin\n");
        return rv;
    }
    rv = bcm_fabric_control_set(unit, bcmFabricShaperQueueMax, ISQ_QUEUE_END);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_control_set bcmFabricShaperQueueMax\n");
        return rv;
    }
            
    /*set ISQ root*/
    BCM_COSQ_GPORT_ISQ_ROOT_SET(isq_root);
        
    /*set sys port to ISQ root*/
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, 0);
    rv = bcm_stk_sysport_gport_set(unit, sys_port, isq_root);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stk_sysport_gport_set sys_port=0x%x, isq_root=0x%x\n", sys_port, isq_root);
        return rv;
    }
    
    /* create scheduler */
    rv = bcm_cosq_gport_add(unit, 0, 1, BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ, &cl_element);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_add cl_element=0x%x\n", cl_element);
        return rv;
    }
    rv = bcm_cosq_gport_sched_set(unit, cl_element, 0, BCM_COSQ_SP0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_sched_set cl_element=0x%x\n", cl_element);
        return rv;
    }

    rv = bcm_cosq_gport_attach(unit, isq_root, cl_element, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_attach isq_root=0x%x, cl_element=0x%x\n", isq_root, cl_element);
        return rv;
    }

    rv = bcm_cosq_gport_bandwidth_set(unit, isq_root, 0, 0, isq_bw, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_bandwidth_set cl_element=0x%x, isq_bw=%d\n", cl_element, isq_bw);
        return rv;
    }

    rv = bcm_cosq_control_set(unit, cl_element, 0, bcmCosqControlBandwidthBurstMax, 3000);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_control_set cl_element=0x%x, bcmCosqControlBandwidthBurstMax, 3000\n", cl_element);
        return rv;
    }

    for (i = 0; i < nof_isqs/nof_cos; i++) {
        /*create queue*/
        rv = bcm_cosq_gport_add(unit, isq_root, nof_cos, BCM_COSQ_GPORT_ISQ | BCM_COSQ_GPORT_TM_FLOW_ID, &queue);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_cosq_gport_add isq_root=0x%x, queue=0x%x, nof_cos=%d\n", isq_root, queue, nof_cos);
            return rv;
        }
        /*create sched scheme*/
        rv = scheduler_queue_config(unit, cl_element, queue, nof_cos);
        if (rv != BCM_E_NONE) {
            printf("Error, in scheduler_queue_config\n");
            return rv;
        }
    }

    /* printf("ardon_shaping_configuration. Exit\n"); */

    return rv;
}

/* 
 * Ardon THR assiatance function 
 * thresholds are differ by pd/db, uc/mc, fc/drop, tc, color & reserved cariteria. 
 * set_eqr_thresholds receives these parameters and configure proper values.
 */
int set_eqr_thresholds(int unit, int local_port_id, int is_mc, int pd_db, int tc, int color, int drop_thr, int fc_thr, int mc_pd_rsv, int if_thr)
{
    bcm_error_t           rv = BCM_E_NONE;
    bcm_cos_queue_t       cosq  ;
    bcm_cosq_threshold_t  threshold;
    bcm_cosq_gport_type_t gport_type;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t           out_gport;
    int                   flags = 0;
    cosq = tc;

    printf("set_eqr_thresholds, is_mc=%d, pd_db=%d, tc=%d, color=%d, drop_thr=%d, fc_thr=%d mc_pd_rsv=%d\n", is_mc, pd_db, tc, color, drop_thr, fc_thr, mc_pd_rsv );
    /* Set GPORT according to the given local_port_id */
    BCM_GPORT_LOCAL_SET(gport_info.in_gport,local_port_id); 

    if(is_mc == 0) {
        gport_type = bcmCosqGportTypeUnicastEgress; 
        threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST;
    } else {
        gport_type = bcmCosqGportTypeMulticastEgress; 
        threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST;
        threshold.dp    = color;
        if (color != -1 ) {
            threshold.flags |= BCM_COSQ_THRESHOLD_PER_DP;
        }
    }
        
    if (pd_db) {
        threshold.type = bcmCosqThresholdPacketDescriptors;
    } else {
        threshold.type = bcmCosqThresholdDataBuffers;
    }
    rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in handle get, gport_info.in_gport $gport_info.in_gport\n");
        return rv;
    }
    out_gport = gport_info.out_gport;

    if (drop_thr != -1) {
        threshold.flags |= BCM_COSQ_THRESHOLD_DROP;
        threshold.value = drop_thr;
        
        rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Error, in set_eqr_thresholds port=%d, is_mc=%d, pd_dp=%d, gport_type=%d, drop_thr=%d\n",
                                                 local_port_id, is_mc, pd_db, gport_type, drop_thr, fc_thr);
            return rv;
        }
    }

    if(is_mc == 0) {
        threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST;
    } else {
        threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST;
        if (color != -1 ) {
            threshold.flags |= BCM_COSQ_THRESHOLD_PER_DP;
        }
    }

    if(fc_thr != -1) {
        threshold.flags |= BCM_COSQ_THRESHOLD_FLOW_CONTROL;
        threshold.value = fc_thr;
        
        rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Error, in set_eqr_thresholds port=%d, is_mc=%d, pd_dp=%d, gport_type=%d, drop_thr=%d\n",
                                                 local_port_id, is_mc, pd_db, gport_type, drop_thr, fc_thr);
            return rv;
        }
    }

    if(mc_pd_rsv != -1) {
        gport_type = bcmCosqGportTypeMulticastEgress; 
        threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_MULTICAST | BCM_COSQ_THRESHOLD_DROP;
        BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport_info.in_gport,local_port_id);
        rv = bcm_cosq_gport_handle_get(unit, gport_type, &gport_info); 
        if (rv != BCM_E_NONE) {
            printf("Error, in handle get, gport_info.in_gport $gport_info.in_gport\n");
            return rv;
        }
        out_gport = gport_info.out_gport;
        threshold.value = mc_pd_rsv;
        threshold.type = bcmCosqThresholdAvailablePacketDescriptors;
        
        rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Error, in set_eqr_thresholds port=%d, is_mc=%d, pd_dp=%d, drop_thr=%d\n",
                                                 local_port_id, is_mc, pd_db, drop_thr, fc_thr);
            return rv;
        }
    }

    if(if_thr != -1) {
        local_port_id += 1024;
        threshold.flags |= BCM_COSQ_THRESHOLD_FLOW_CONTROL;
        BCM_GPORT_LOCAL_SET(out_gport,local_port_id); 
        threshold.value = if_thr;
        threshold.type = bcmCosqThresholdPacketDescriptors;
        
        rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, &threshold);
        if (rv != BCM_E_NONE) {
            printf("Error, in set_eqr_thresholds port=%d, is_mc=%d, pd_dp=%d, drop_thr=%d\n",
                                                 local_port_id, is_mc, pd_db, drop_thr, fc_thr);
            return rv;
        }
        if (is_mc == 0) {
            threshold.type = bcmCosqThresholdBytes;
            rv = bcm_cosq_gport_threshold_set(unit, out_gport, cosq, &threshold);
            if (rv != BCM_E_NONE) {
                printf("Error, in set_eqr_thresholds port=%d, is_mc=%d, pd_dp=%d, drop_thr=%d\n",
                                                     local_port_id, is_mc, pd_db, drop_thr, fc_thr);
                return rv;
            }
        }
    }

    /* printf("set_eqr_thresholds: Exit\n"); */
    return rv;
}

/* 
 * Ardon THR configuration Example 
 * This function responsible for configuring Ardon per port thresholds according ardon_egq_thresholds.xls.
 * This function configures only the 1 port 100G/120G with 8 priority example. 
 * egrs_thr_t structure defined in order to save threshold values. 
 * thresholds are differ by pd/db, uc/mc, fc/drop, tc, color, reserved & interface cariteria. 
 * set_eqr_thresholds help function defined to receive these parameters and configure proper values.
 */
int ardon_thr_example(int unit, int local_port_id, int nof_cos) {
    int rv = BCM_E_NONE;
    int tc;
    int color;
    int is_mc;
    int pd_db;
    int invalid = -1;

    /* printf("ardon_thr_example, local_port_id=%d, nof_cos=%d\n", local_port_id, nof_cos ); */
    /* Thresholds example for 12 port of 10G */
    /*
    egrs_thr_t egrs_thr = {
        2850,
        2850,
        5700,
        5700,
        13000,
        130000,
        2500,
        10830,
        {950, 950, 950, 950, 950, 950, 950, 950},
        {950, 950, 950, 950, 950, 950, 950, 950},
        {4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000},
        {5700, 5700, 5700, 5700, 5700, 5700, 5700, 5700},
        { {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400} },
        { {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000} },
        { 400, 400, 400, 400, 400, 400, 400, 400}
    }; 
    */ 

    /* Thresholds example for 1 port of 100G/120G */
    egrs_thr_t egrs_thr = {
        2850,
        2850,
        5700,
        5700,
        13000,
        130000,
        2500,
        10830,
        {950, 950, 950, 950, 950, 950, 950, 950},
        {950, 950, 950, 950, 950, 950, 950, 950},
        {4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000},
        {5700, 5700, 5700, 5700, 5700, 5700, 5700, 5700},
        { {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400}, {4000, 2000, 2000, 400} },
        { {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000}, {130000, 65000, 65000, 13000} },
        { 400, 400, 400, 400, 400, 400, 400, 400}
    };

    is_mc = 0;
    pd_db = 1;
    tc = -1;
    color = -1;
    set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, invalid, invalid, invalid, egrs_thr.uc_fc_pd_if_th );
    is_mc = 1;
    set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, invalid, invalid, invalid, egrs_thr.mc_fc_pd_if_th );
    is_mc = 0;
    set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.uc_drop_pds_th, egrs_thr.uc_fc_pds_th, invalid, invalid );
    pd_db = 0;
    set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.uc_drop_dbs_th, egrs_thr.uc_fc_dbs_th, invalid, invalid );
    is_mc = 1;
    pd_db = 1;
    set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.mc_drop_pds_th, invalid, invalid, invalid );
    pd_db = 0;
    set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.mc_drop_dbs_th, invalid, invalid, invalid );

    for (tc = 0; tc < nof_cos; tc++) {
        is_mc = 0;
        pd_db = 1;
        color = -1;
        set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.tc_uc_drop_pds_th[tc], egrs_thr.tc_uc_fc_pds_th[tc], invalid, invalid );
        pd_db = 0;
        set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.tc_uc_drop_dbs_th[tc], egrs_thr.tc_uc_fc_dbs_th[tc], invalid, invalid );
        is_mc = 1;
        for (color = 0; color < 4; color++) {
            pd_db = 1;
            set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.tc_mc_drop_pds_th[tc][color], invalid, invalid, invalid );
            pd_db = 0;
            set_eqr_thresholds(unit, local_port_id, is_mc, pd_db, tc, color, egrs_thr.tc_mc_drop_dbs_th[tc][color], invalid, invalid, invalid );
        }
    }

    /* printf("ardon_thr_example, Exit\n" ); */

    return rv;
}

int ardon_appl_example(int unit, int nof_local_ports, int mc_group_num, int nof_isqs, int cgm_thr_enable) {
    int 
        rv,
        i,
        modid, 
        nof_units,
        unit_arr[MAX_NUM_DEVICE], 
        modid_arr[MAX_NUM_DEVICE],
        mc_type,
        mc_id,
        nxt_mc_id,
        mc_port_member,
        isq_bw;
    bcm_port_t tm_internal_pkt_port;
    
    /* printf("ardon_appl_example, nof_local_ports=%d, mc_group_num=%d, nof_isqs=%d, cgm_thr_enable=%d\n", nof_local_ports, mc_group_num, nof_isqs, cgm_thr_enable ); */

    tm_internal_pkt_port = 1;
    modid = 0;
    nof_units = 1;
    unit_arr[0] = 0;
    modid_arr[0] = 0;    
    rv = ardon_appl_init(unit, tm_internal_pkt_port, nof_local_ports, modid, nof_units, unit_arr, modid_arr);
    if (BCM_FAILURE(rv)) {
        printf("ardon_appl_init: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    mc_type = 0x1; /* BCM_MULTICAST_EGRESS_GROUP = 1 */
    mc_id = 256;
    nxt_mc_id = 0;
    mc_port_member = 1;
    rv = ardon_appl_mc(unit, mc_group_num, mc_type, mc_id, nxt_mc_id, mc_port_member);
    if (BCM_FAILURE(rv)) {
        printf("ardon_appl_mc: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }


    if (cgm_thr_enable == 0x1) {
        /* CGM configuration */
        rv = ardon_thr_example(unit, tm_internal_pkt_port, MAX_COS);
        if (BCM_FAILURE(rv)) {
            printf("ardon_thr_example: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
            return rv;
        }
    }

    if (nof_isqs != 0x0) {
        /* ISQ configuration to 2.5Gb/s */
        isq_bw = 2500000;

        /* printf("ardon_appl_example, Call ardon_shaping_configuration. isq_bw %d\n",isq_bw); */

        rv = ardon_shaping_configuration(unit, nof_isqs, MAX_COS, isq_bw);
        if (BCM_FAILURE(rv)) {
            printf("ardon_shaping_configuration: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
            return rv;
        }
    }

    /* Enable traffic */
    printf("Enable Traffic\n");
    rv = bcm_stk_module_enable(unit, modid, g_info.num_ports, 0x1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_module_enable: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    /* printf("ardon_appl_example, Exit\n"); */

    return rv;
}

/*
 * Ardon on mgmt device application example.
 *
 * This function is pretty similar to ardon_appl_example(), only it was written specifically to work 
 * for mgmt system, i.e. 1 cpu which controls a system with 2 faps and 1 fe.
 * For more information see notes at the begining of this file.
 *
 * Note: there are some hardcoded variables inside the function, so you may need to 
 * change them if your system is different.
 */
int ardon_mgmt_appl_example(int unit, int nof_local_ports, int mc_group_num, int nof_isqs, int cgm_thr_enable) {
    int 
        rv,
        i,
        modid, 
        nof_units,
        unit_arr[MAX_NUM_DEVICE], 
        modid_arr[MAX_NUM_DEVICE],
        mc_type,
        mc_id,
        nxt_mc_id,
        mc_port_member,
        isq_bw,
        unit_idx;
    bcm_port_t tm_internal_pkt_port;
    
    /* printf("ardon_mgmt_appl_example, nof_local_ports=%d, mc_group_num=%d, nof_isqs=%d, cgm_thr_enable=%d\n", nof_local_ports, mc_group_num, nof_isqs, cgm_thr_enable ); */

    tm_internal_pkt_port = 1;
    modid = 0; /* modid idx of this fap */
    nof_units = 2; /* number of faps in the system */
    unit_arr[0] = 2; /* unit of the second fap */
    modid_arr[0] = 1; /* modid idx of the second fap */ 
    rv = ardon_appl_init(unit, tm_internal_pkt_port, nof_local_ports, modid, nof_units, unit_arr, modid_arr);
    if (BCM_FAILURE(rv)) {
        printf("ardon_appl_init: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    g_info.cfg_2nd_unit = (g_info.central_cpu && (nof_units > 1));
    printf("g_info.cfg_2nd_unit = %d\n", g_info.cfg_2nd_unit);

    mc_type = 0x0; /* BCM_MULTICAST_INGRESS_GROUP = 1 */
    mc_id = 256;
    nxt_mc_id = 0;
    mc_port_member = MAX_NUM_PORTS + 1;
    rv = ardon_appl_mc(unit, mc_group_num, mc_type, mc_id, nxt_mc_id, mc_port_member);
    if (BCM_FAILURE(rv)) {
        printf("ardon_appl_mc: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    if (g_info.cfg_2nd_unit) {
        for (unit_idx = 0; unit_idx < nof_units - 1; ++unit_idx) {
            mc_type = 0x1; /* BCM_MULTICAST_EGRESS_GROUP = 1 */
            rv = ardon_appl_mc(unit_arr[unit_idx], mc_group_num, mc_type, mc_id, nxt_mc_id, mc_port_member);
            if (BCM_FAILURE(rv)) {
                printf("ardon_appl_mc: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }

    if (cgm_thr_enable == 0x1) {
        /* CGM configuration */
        rv = ardon_thr_example(unit, tm_internal_pkt_port, MAX_COS);
        if (BCM_FAILURE(rv)) {
            printf("ardon_thr_example: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
            return rv;
        }
        if (g_info.cfg_2nd_unit) {
            for (unit_idx = 0; unit_idx < nof_units - 1; ++unit_idx) {
                rv = ardon_thr_example(unit_arr[unit_idx], tm_internal_pkt_port, MAX_COS);
                if (BCM_FAILURE(rv)) {
                    printf("ardon_thr_example: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

    if (nof_isqs != 0x0) {
        /* ISQ configuration to 2.5Gb/s */
        isq_bw = 2500000;

        /* printf("ardon_mgmt_appl_example, Call ardon_shaping_configuration. isq_bw %d\n",isq_bw); */

        rv = ardon_shaping_configuration(unit, nof_isqs, MAX_COS, isq_bw);
        if (BCM_FAILURE(rv)) {
            printf("ardon_shaping_configuration: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
            return rv;
        }
        if (g_info.cfg_2nd_unit) {
            for (unit_idx = 0; unit_idx < nof_units - 1; ++unit_idx) {
                rv = ardon_shaping_configuration(unit_arr[unit_idx], nof_isqs, MAX_COS, isq_bw);
                if (BCM_FAILURE(rv)) {
                    printf("ardon_shaping_configuration: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

    /* Enable traffic */
    printf("Enable Traffic\n");
    rv = bcm_stk_module_enable(unit, modid, g_info.num_ports, 0x1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_module_enable: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    if (g_info.cfg_2nd_unit) {
        for (unit_idx = 0; unit_idx < nof_units - 1; ++unit_idx) {
            rv = bcm_stk_module_enable(unit_arr[unit_idx], modid_arr[unit_idx]*MAX_MODIDS_PER_DEVICE, g_info.num_ports, 0x1);
            if (BCM_FAILURE(rv)) {
                printf("bcm_stk_module_enable: failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
                return rv;
            }
        }
    }
    
    /* printf("ardon_mgmt_appl_example, Exit\n"); */

    return rv;
}

