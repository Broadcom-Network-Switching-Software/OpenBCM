/*~~~~~~~~~~~~~~~~~~~~~~~~~~ RCPU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * File: cint_dnx_cmic_rcpu.c
 * Purpose: Example for RCPU using
 *
 * Environment:
 * The following example matches the following system configuration:
 *
 *                                     CPU_local_port=0, CPU_RCPU_port=201.
 *                                           --------
 *                       in_port=13, CPU     |      |    out_port=14
 *                                    -------|      |-----
 *                                           |      |
 *                                           --------
 *
 * Example Packet flow:
 *   1. ETH RCPU Packet inserted via in_port, assuming tm_port_header_type of in_port is ETH.
 *   2. ETH forwarding to CPU_RCPU_port.
 *   3. CPU_local_port transmit ETH RCPU replay.
 *   4. ETH RCPU replay forward to out_port.
 *
 * All the configuration example is in cmic_rcpu_appl().
 * Relevant soc properties configuration:
 *   CPU_local_port send ETH packet (to the out_port):
 *     tm_port_header_type_in_0=ETH
 *   Define CPU_RCPU_port as CPU RCPU port:
 *     rcpu_rx_pbmp=0x200000000000000000000000000000000000000000000000000
 *
 * RCPU packet format:
 *   -----------------------------------------------------
 *   |                    DA[47:16]                      |
 *   |       DA[15:0]       |         SA[47:32]          |
 *   |                    SA[31:0]                       |
 *   |       TPID[15:0]     |         VLAN_TAG[15:0]     |
 *   |     Ethertype[15:0]  |        SIGNATURE[15:0]     |
 *   | Operation[8:0] | Flags[8:0] | TransactionID[15:0] |
 *   |    DataLength[15:0]  |       ReplayLenagth[15:0]  |
 *   |                   Reserved[31:0]                  |
 *   -----------------------------------------------------
 *
 * Operation:
 *  0x01 - for SCHAN request
 *  0x02 - for SCHAN replay (created by cmic)
 *  0x03 - for RPIO request (cmic regs)
 *  0x04 - for RPIO replay (created by cmic)
 *  0x10 - TOCPU_PACKET (Not Supported)
 *  0x20 - FROMCPU_PACKET (Not Supported)
 *
 * SCHAN foramt:
 *   --------------------------------------
 *   | Remote Packet PIO Header (32 Bytes)|
 *   | Sbus command Opcode (4 Bytes)      |
 *   | Sbus command Address (4 Bytes)     |
 *   | Sbus command Data (0-88)           |
 *   --------------------------------------
 *
 * RPIO foramt:
 *  The first word bits are:
 *    [31]    = 0
 *    [30]    = 1 for write, 0 for read
 *    [29:26] = 0
 *    [25:24] = "11"
 *    [23:0]  = Cmic register address
 *  In case of write the next 4 bytes are the data.
 * Reserved should be set to 0x0.
 *
 * Test Scenario
 * ./bcm.user
 * cd ../../../../regress/bcm
 * bcm shell "config add tm_port_header_type_in_0.0=ETH"
 * bcm shell "config add tm_port_header_type_out_0.0=ETH"
 * bcm shell "config add tm_port_header_type_in_202.0=ETH"
 * bcm shell "config add rcpu_rx_pbmp.0=0xf00000000000000000000000000000000000000000000000001"
 * bcm shell "0: tr 141 NoInit=1"
 * bcm shell "config add custom_feature_init_verify=1"
 * bcm shell "config delete custom_feature_multithread_en*"
 * bcm shell "config delete custom_feature_kbp_multithread_en*"
 * bcm shell "0: tr 141 NoDeinit=1"
 * cint ../../src/./examples/dnx/cint_dnx_cmic_rcpu.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vlan.c
 * cint
 * cmic_rcpu_appl(0,201,203);
 * exit;
 *
 * sending packets to write to a register
 * tx 1 psrc=202 data=0x00000000000300000000000a810000018874560001010001000c00040000000000000001340002000000016e00001233
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000383800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to read from a register
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012c0002000000016e
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a0000000000000800200000000000000000000000000000000000000000000000000000000000030380200ffffedcc00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to write to a register
 * tx 1 psrc=202 data=0x00000000000300000000000a810000018874560001010001000c00040000000000000001340002000000016e0000abcd
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000383800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to read from a register
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012c0002000000016e
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a0000000000000800200000000000000000000000000000000000000000000000000000000000030380200ffff543200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to write to a register
 * tx 1 psrc=202 data=0x00000000000300000000000a810000018874560001010001000c00040000000000000001340002000000016e8700ff00
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000383800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to read from a register
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012c0002000000016e
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a000000000000080020000000000000000000000000000000000000000000000000000000000003038020078ff00ff00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to write to a register
 * tx 1 psrc=202 data=0x00000000000300000000000a810000018874560001010001000c00040000000000000001340002000000016e071acb78
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000383800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to read from a register
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012c0002000000016e
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a0000000000000800200000000000000000000000000000000000000000000000000000000000030380200f8e5348700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packets to write to a smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012460020001c3800500000012
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000283800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packet to read from smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000011c60020001c38005000000
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a0000000000000800200000000000000000000000000000000000000000000000000000000000020380200000c001200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packet to read from smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012460020001c3800500000057
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000283800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packet to read from smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000011c60020001c38005000000
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008002000000000000000000000000000000000000000000000000000000000000203802000002005700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packet to read from smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012460020001c3800500000006
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000283800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packet to read from smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000011c60020001c38005000000
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008002000000000000000000000000000000000000000000000000000000000000203802000003000600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packet to read from smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000012460020001c38005000000e3
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a00000000000008001000000000000000000000000000000000000000000000000000000000000283800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * sending packet to read from smooth devision
 * tx 1 psrc=202 data=0x00000000000300000000000a8100000188745600010100010008000800000000000000011c60020001c38005000000
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000000a00000000000381000001887456000200000100a0000000000000800200000000000000000000000000000000000000000000000000000000000020380200000b00e300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * bcm shell "config delete tm_port_header_type_in_0.0"
 * bcm shell "config delete tm_port_header_type_out_0.0"
 * bcm shell "config delete tm_port_header_type_in_202.0"
 * bcm shell "config delete rcpu_rx_pbmp.0"
 * bcm shell "0: tr 141 NoInit=1"
 * bcm shell "config add custom_feature_init_verify=1"
 * bcm shell "config delete custom_feature_multithread_en*"
 * bcm shell "config delete custom_feature_kbp_multithread_en*"
 * bcm shell "0: tr 141 NoDeinit=1"
 * 
 *   Inject packet from in port. RCPU packet format (according to cmic_rcpu_appl()):
 *     1. DA (lmac) = {0x0, 0x0, 0x0, 0x0, 0x0, 0x3}
 *     2. SA (rmac) = {0x0, 0x0, 0x0, 0x0, 0x0, 0xa}
 *     3. Signature = 0x5600
 *     4. Ethertype = 0x8874
 *     5. Vlan = 0x1
 *     6. Tpid = 0x8100
 *     7. payload (after Reserved word):
 *          a. write eci_test_register=0x01B3            with 0x0000aaaa       8874560001010001000C0004000000000000000134000200000001B30000aaaa
 *          b. read  eci_test_register=0x01B3                                  88745600010100010008000800000000000000012C000200000001B3
 *          c. SCHAN multi regs - ECI_TEST_REGISTER,
 *                                ECI_SCRATCH_PAD_0,
 *                                ECI_SCRATCH_PAD_1                            887456000101000100100004000000000000000334000200000001b35a5a5a5a34000200000001fa00005a5a34000200000001fb5a5a0000
 *          d. SCHAN multi mems - ECI_PKT_SRC_TO_CHANNEL_MAP entry 14,15       8874560001010001001000040000000000000002240002000000008e0000005a240002000000008f000000a5
 *          e. PIO multi: QSPI_MSPI_TXRAM00 + QSPI_MSPI_TXRAM01                887456000301000100200000000000004000000000000000180212405a5a5a5a400000000000000018021244a5a5a5a5
 *   Injecting Packet from CPU port:
 *     1. additional SOC properties:
 *          tm_port_header_type_out_0=ETH
 *     2. Send CPU packet to perform SCHAN operation (write smooth_devision=0x005c0000 entry 0 with=0x20):
            tx 1 Length=100 data=0x00000000000300000000000a810000018874560001010001000C00040000000027300200005C000000000020
 *
 *    Excepted  result:
 *      1. In write operation Register/Table changed according to Packet data value.
 *         In read operation packet data field is set to register/table value.
 *      2. CMIC_PKT_COUNT_SCHAN, CMIC_PKT_COUNT_SCHAN_REP increased in SCHAN operation.
 *      3. CMIC_PKT_COUNT_PIO, CMIC_PKT_COUNT_PIO_REPLY in RPIO operation
 */

/* rcpu local_port */
int cpu_rcpu_port = 201;

/* RCPU Example */
int cmic_rcpu_example(int unit, int cpu_rcpu_port, int out_port, bcm_mac_t lmac, bcm_mac_t rmac, uint32 sig, uint32 ethertype, uint32 vlan, uint32 tpid)
{
    int rv = BCM_E_NONE;
    uint32 lmac_lo, lmac_hi, rmac_lo, rmac_hi;
    bcm_l2_addr_t l2addr;

    /* RCPU request forwarding */
    bcm_l2_addr_t_init(&l2addr, lmac, vlan);
    l2addr.port = cpu_rcpu_port;
    l2addr.vid = vlan;
    l2addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }

    /* RCPU replay forwarding */
    bcm_l2_addr_t_init(&l2addr, rmac, vlan);
    l2addr.port = out_port;
    l2addr.vid = vlan;
    l2addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }

    /* Enabling CMIC. */
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuCmicEnable, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuCmicEnable, rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuMatchLocalMac, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuMatchLocalMac, rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuSchanEnable, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuSchanEnable, rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuMatchVlan, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuMatchVlan, rv=%d.\n", rv);
        return rv;
    }

    /* Configuring LMAC and RMAC */
    lmac_lo = ((lmac[5]) | (lmac[4] << 8) | (lmac[3] << 16)) & 0xffffff;
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuLocalMacNonOui, lmac_lo);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuLocalMacNonOui, rv=%d.\n", rv);
        return rv;
    }
    lmac_hi = ((lmac[2]) | (lmac[1] << 8) | (lmac[0] << 16)) & 0xffffff;
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuLocalMacOui, lmac_hi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuLocalMacOui, rv=%d.\n", rv);
        return rv;
    }

    rmac_lo = ((rmac[5]) | (rmac[4] << 8) | (rmac[3] << 16)) & 0xffffff;
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuDestMacNonOui, rmac_lo);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuDestMacNonOui, rv=%d.\n", rv);
        return rv;
    }
    rmac_hi = ((rmac[2]) | (rmac[1] << 8) | (rmac[0] << 16)) & 0xffffff;
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuDestMacOui, rmac_hi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuDestMacOui, rv=%d.\n", rv);
        return rv;
    }

    printf("cmic_rcpu_example: lmac_lo=0x%x, lmac_hi=0x%x, rmac_lo=0x%x, rmac_hi=0x%x,\n", lmac_lo, lmac_hi, rmac_lo, rmac_hi);
    printf("cmic_rcpu_example: sig=0x%x, ethertype=0x%x, vlan=0x%x, tpid=0x%x,\n", sig, ethertype, vlan, tpid);

    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuSignature, sig);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuSignature, rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuEthertype, ethertype);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuEthertype, rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuVlan, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuVlan, rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchRemoteCpuTpid, tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), bcmSwitchRemoteCpuTpid, rv=%d.\n", rv);
        return rv;
    }

    return rv;
}

/*
 * verify CMIC RCPU configuration
 */
int cmic_rcpu_config_verify(int unit)
{
    int rv = BCM_E_NONE, core, port_i, rcpu_base_q_pair_min = 0, rcpu_base_q_pair_max = 96;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_config_t port_config;
    bcm_pbmp_t cpu_pbmp, pbmp;
    uint32 port_fap_index, port_base_q_pair, port_nof_q_pairs, port_to_ps_num, rcpu_base_q_pair = 255, flags;

    printf("\n Port of NIF+CPU EGQ MAPPING:\n*********************\n");
    printf(" Port #  |  Priorities  |  Base Q-Pair  |   PS #  | Core | TM Port |\n");
    printf("---------|--------------|---------------|---------|------|---------|\n");

    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get rv=%d \n", rv);
        return rv;
    }

    BCM_PBMP_CLEAR(cpu_pbmp);
    BCM_PBMP_OR(cpu_pbmp, port_config.cpu);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_OR(pbmp, port_config.nif);
    BCM_PBMP_OR(pbmp, port_config.cpu);

    BCM_PBMP_ITER(pbmp, port_i) {
        rv = bcm_port_get(unit, port_i, &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_get rv=%d \n", rv);
            return rv;
        }

        core = mapping_info.core;
        port_fap_index = mapping_info.tm_port;
        port_nof_q_pairs = mapping_info.num_priorities;
        port_base_q_pair = mapping_info.base_q_pair;
        port_to_ps_num = (port_base_q_pair / 8);

        if (port_fap_index == cpu_rcpu_port) {
            rcpu_base_q_pair = port_base_q_pair;
        }

        printf("  %3u    |       %1u      |      %3u      |    %2u   |   %1u  |   %3u   |\n",
                port_i, port_nof_q_pairs, port_base_q_pair, port_to_ps_num, core, port_fap_index);
    }

    printf("====== cpu pbmp, rcpu port \n");
    print cpu_pbmp;
    print cpu_rcpu_port;

    if ((rcpu_base_q_pair >= rcpu_base_q_pair_min) && \
        (rcpu_base_q_pair <= rcpu_base_q_pair_max)) {
        printf("\nrcpu base_q_pair is %d in range of [0,96]\n", rcpu_base_q_pair);
        printf("cmic_rcpu_config_verify: PASS\n\n");
    } else {
        printf("\nrcpu base_q_pair is %d out of range [0,96]\n", rcpu_base_q_pair);
        printf("cmic_rcpu_config_verify: FAIL\n\n");
        return BCM_E_FAIL;
    }

    return rv;
}

/*
 * Example for CMIC RCPU application
 */
int cmic_rcpu_appl(int unit, int rcpu_port, int out_port)
{
    int rv = BCM_E_NONE;
    bcm_mac_t lmac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, rmac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    uint32 sig, ethertype, vlan, tpid;
    lmac[5] = 0x3;
    rmac[5] = 0xa;
    sig = 0x5600;
    ethertype = 0x8874;
    vlan = 0x1;
    tpid = 0x8100;

    rv = cmic_rcpu_example(unit, rcpu_port, out_port, lmac, rmac, sig, ethertype, vlan, tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, cmic_rcpu_example(), rv=%d.\n", rv);
        return rv;
    }

    return rv;
}
