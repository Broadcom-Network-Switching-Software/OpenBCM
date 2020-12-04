/*
 * Feature  : PFC(Priority Flow control) Receive & Trasmit
 *
 * Usage    : BCM.0> cint th4_hsdk_mmu_fc_pfc.c
 *
 * Config   : th4_hsdk_mmu_config.yml
 *
 * Log file : th4_hsdk_mmu_fc_pfc_log.txt
 *
 * Test Topology  :
 *  
 *       DMAC=0x1            +------------+         DMAC=0x1
 *   I   SMAC=0x2   +-------->   56980    +-------> SMAC=0x2         I
 *   X   VLAN=2, Pri=2 cd0(1)|            |cd1(2)   VLAN=2, Pri=2    X
 *   I                       | l2:mac=0x1 |                          I
 *   A                       |    vlan=2  |                          A
 *            <-------+      |    port=2  |     <-------+
 *         TX PFC FRAMES     |            |  RX PFC FRAMES
 *                           +------------+ 
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates how to enable PFC on TH4
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *  1) Step1 - Test Setup (Done in test_setup())
 *  ============================================
 *    a) Select one ingress and one egress port
 *    b) Setup a L2 forwarding path from ingress to egress port
 *
 *  2) Step2 - Configuration (Done in pfc_config())
 *  =================================================
 *    a) Set the PFC tx priotiry to PG mapping
 *    b) Disable generic Tx and Rx Pause
 *    c) Enable PFC Tx and Rx Pause
 *
 *  3) Step3 - Verification (Done in test_verify())
 *  ==========================================
 *    a) Transmit l2 packet (dmac=0x1 smac=0x2 vlan=2 prio=2)
 *       at line rate from IXIA to ingress port cd0
 *    b) Along with a), send PFC frames with pause-control class 2 enabled to port cd1
 *       (Pause frame: dmac=0x0180C2000001, smac=0x1, type=0x8808, opcode=0x0001, class 2)
 *
 *    c) Expected Result:
 *       ================
 *       For test case mentioned in a)
 *       On cd0 (Port-1): verify that PFC frames should be transmitted at regular intervals as
 *       egress port is congested.
 *       In 'show c' output, PFC frame Tx should be seen on cd0 (port-1)
 *
 *       On cd1 (Port-2): packets should egress at configured rate
 *
 *       For test case mentioned in b)
 *       Port cd1 should not egress any packets as PFC pasue frames are being
 *       received. In 'show c' output, there should not be any Tx on on cd1 (port-2)
 */
 
cint_reset();

int unit = 0;
bcm_port_t ing_port; /*Ingress port */
bcm_port_t egr_port; /*Egress port */
int cos = 2; /*Priority of incoming packet on which testing is done */

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0, port=0, rv=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if(BCM_FAILURE(rv)) {
       printf("\nError in retrieving port configuration: %s.\n", bcm_errmsg(rv));
       return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)&& (port < num_ports)) {
            port_list[port]=i;
            port++;
        }
    }

    if (( port == 0 ) || ( port != num_ports )) {
         printf("portNumbersGet() failed \n");
         return -1;
    }

    return BCM_E_NONE;
}

bcm_error_t data_path_setup(int unit)
{
    bcm_vlan_t vid = 2;
    bcm_mac_t dest_mac = "00:00:00:00:00:01"; /*Destination MAC */
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t l2addr;
    int rv = 0;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE) {
        printf("[bcm_vlan_create] returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, ing_port);
    BCM_PBMP_PORT_ADD(pbmp, egr_port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_add returned :%s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l2_addr_t_init(l2addr, dest_mac, vid);
    l2addr.port = egr_port;
    l2addr.flags |= BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_addr_add returned :'%s'\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* PFC tx priotiry to PG mapping */
bcm_error_t
pfc_priority_to_pg_setup(int unit)
{
    int pri_to_pg_array[8] = {0, 1, 2, 3, 4, 5, 6, 7}; 
    int profile_id = 1;
    int rv = 0;
  
    /* Set PFC-tx priority to PG mapping 1:1 mapping*/
    rv = bcm_cosq_priority_group_pfc_priority_mapping_profile_set(unit, profile_id, 8, pri_to_pg_array);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_priority_group_pfc_priority_mapping_profile_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t pfc_config(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
    uint32 limit_kbits_sec = 1000;    /* Bandwidth limit on egress queue - 1Mbps*/
    bcm_mac_t pause_smac = "00:11:22:33:44:55"; /*Source MAC on all PFC frames transmitted from a port */
    int enable = 1;
    int disable = 0;
    int rv = 0;

    /*
     * Set the bandwidth limit on particular queue of egr port 
     */
    rv = bcm_cosq_gport_bandwidth_set(unit, egr_port, cos, limit_kbits_sec, limit_kbits_sec, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Set PAUSE frame smac\n");

    /* Set the source MAC for PFC Transmit frames */
    rv = bcm_port_pause_addr_set(unit, ing_port, pause_smac);
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_pause_addr_set returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Disable regular PAUSE\n");

    /* Disable normal PAUSE on ingress port */
    rv = bcm_port_pause_set(unit, ing_port, disable, disable);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_pause_set~ingress returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Disable normal PAUSE on egress port */
    rv = bcm_port_pause_set(unit, egr_port, disable, disable);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_pause_set~egress returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Eenabling PFC PAUSE\n");

    /* PFC tx priotiry to PG mapping */
    rv = pfc_priority_to_pg_setup(unit);
    if (rv != BCM_E_NONE) {
        printf("pfc_priority_to_pg_setup returned '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable PFC Tx on ingress port and RX on egress port */
    rv = bcm_port_control_set(unit, ing_port, bcmPortControlPFCTransmit, 1);
    if (rv != BCM_E_NONE) {
         printf("bcm_port_control_set~Tx returned '%s'\n", bcm_errmsg(rv));
         return rv;
    }
    rv = bcm_port_control_set(unit, egr_port, bcmPortControlPFCReceive, 1);
    if (rv != BCM_E_NONE) {
         printf("bcm_port_control_set~Rx returned '%s'\n", bcm_errmsg(rv));
         return rv;
    }

    /* Modify PG limits to generate early PFC */
    bcm_cosq_control_set (unit, ing_port, 7,  bcmCosqControlIngressPortPGSharedDynamicEnable, 0);

    return BCM_E_NONE;
}

bcm_error_t test_setup(int unit)
{
    int rv = 0;

    int port_list[2], i;
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ing_port = port_list[0];
    egr_port = port_list[1];

    /* common data path setup */
    if (BCM_FAILURE((rv = data_path_setup(unit)))) {
        printf("data_path_setup() failed.: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Ingress port should send PFC frame towards IXIA
 * egress port should not egress anything as it receives PFC pause frames from Ixia.
 */
void test_verify(int unit)
{
    bshell(unit, "show c");
}

bcm_error_t execute()
{
    bcm_error_t rv;

    bshell(unit, "config show; attach; version");

    /* Select ports and set up L2 switching */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    /* Config and enable PFC */
    if (BCM_FAILURE((rv = pfc_config(unit, ing_port, egr_port)))) {
        printf("pfc_config() failed.\n");
        return -1;
    }

    /* verify the result */
    test_verify(unit);

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

