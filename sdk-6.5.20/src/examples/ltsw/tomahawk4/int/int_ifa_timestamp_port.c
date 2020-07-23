/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Demonstrate port tail timestamp function.
 *
 *  Usage    : BCM.0> cint int_ifa_timestamp_port.c
 *
 *  config   : int_ifa_timestamp_port_config.bcm
 *
 *  Log file : int_ifa_timestamp_port_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below APIs:
 *  ========
 *   bcm_port_control_set
 *   bcm_time_tod_set
 *
 *  Summary:
 *  ========
 *  The setup has VLAN 2 with member ports 1 and 2, and VLAN 3 with
 *  member ports 2 and CPU port. The packets ingressing on port 1 with outer VLAN
 *  id 2 will be sent to port 2 and the packets ingressing on port 2 will be sent
 *  to CPU port. Port 1,2 have egress port action to delete outer VLAN tag so
 *  single tagged packets received on port 1 should egress untagged on port 2.
 *  The default ingress action entry of port 2 will add outer TAG for
 *  incoming untagged packets so CPU should receive flooded packets with
 *  outer VLAN 3.
 *  Timestamp is insert at ingress port 1 and egress port 2.
 *
 *  Setup diagram:
 *                        ^
 *                        |
 *                        | CPU
 *                +---------------------------+
 *                |          BCM56996         |
 *                |       ^                   |
 *                |VLAN 3 |                   |
 *                |packet |                   |
 *           +--->| ------!            VLAN 2 |
 *           |  2 |                    packet |
 *           !----|<--------------------------|<------------------
 *                |                           | 1
 *                |   VLAN 2: Ports 1, 2      |
 *                |   VLAN 3: Ports CPU, 2    |
 *                |                           |
 *                +---------------------------+
 *
 *  Ingress port(s): 1.
 *
 *  Egress port(s): 2 and CPU.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Create vlan and add port to vlan.
 *
 *    2) Step2 - Configuration (Done in ts_config())
 *    ======================================================
 *      a) Configure timestamp insertion per port.
 *      b) Enable ToD auto-update.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result after send a packet.
 *      b) Expected Result:
 *         Packet raw data (106):
 *      0000: 0000 00aa aaaa 0000 00bb bbbb 8100 000c
 *      0010: 1234 1234 1234 1234 1234 1234 1234 1234
 *      0020: 1234 1234 1234 1234 1234 1234 1234 1234
 *      0030: 1234 1234 1234 1234 1234 1234 1234 1234
 *      0040: 1234 1234 1234 1234 1234 148f 4ffb 0000
 *      0050: 0000 0000 0000 2222 0000 0000 0000 0000
 *      0060: 0000 5001 ffff
 */
cint_reset();

int unit = 0;
bcm_port_t port_in = 1, port_out = 2;
bcm_vlan_t vlan_in = 11, vlan_out = 12;
bcm_pbmp_t pbmp, ubmp;
bcm_vlan_action_set_t action;
uint32 stages;
bcm_time_tod_t tod_set;

/*
 * This function is pre-test setup.
 */
void test_setup()
{
    /* Create ingress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_in);
    BCM_PBMP_PORT_ADD(pbmp, port_out);
    print bcm_vlan_create(unit, vlan_in);
    print bcm_vlan_port_add(unit, vlan_in, pbmp, ubmp);

    /* Create egress vlan and add port to vlan */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_out);
    BCM_PBMP_PORT_ADD(pbmp, 0); /* Added CPU port to same vlan. */
    print bcm_vlan_create(unit, vlan_out);
    print bcm_vlan_port_add(unit, vlan_out, pbmp, ubmp);

    /* Set default egress TAG action to remove outer and inner VLAN tags of egress packets and send untagged packets on port 1,2. */
    bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionDelete;
    action.new_outer_vlan = vlan_out;
    print bcm_vlan_port_egress_default_action_set(unit, port_in, &action);
    print bcm_vlan_port_egress_default_action_set(unit, port_out, &action);

    print bcm_port_untagged_vlan_set(unit, port_in, vlan_in);
    print bcm_port_untagged_vlan_set(unit, port_out, vlan_out);
}

void ts_config()
{
    print bcm_port_control_set(unit, port_in, bcmPortControlPacketTimeStampInsertRx, 1);
    print bcm_port_control_set(unit, port_in, bcmPortControlPacketTimeStampRxId, 0x1111);
    print bcm_port_control_set(unit, port_out, bcmPortControlPacketTimeStampInsertTx, 1);
    print bcm_port_control_set(unit, port_out, bcmPortControlPacketTimeStampTxId, 0xffff);

    /* Enable ToD auto-update. */
    stages = BCM_TIME_STAGE_ALL;
    bcm_time_tod_t_init(&tod_set);
    tod_set.auto_update = 1;
    print bcm_time_tod_set(unit, stages, &tod_set);
}

/*
 * This function does the following.
 * Transmit the below packet on port_1 and verify
 */
void verify()
{
    char str[512];
    uint64 in_pkt, out_pkt;

    /* Set ports to mac loopback */
    print bcm_port_loopback_set(unit, port_in, BCM_PORT_LOOPBACK_MAC);
    print bcm_port_loopback_set(unit, port_out, BCM_PORT_LOOPBACK_MAC);

    /* Start traffic. */
    bshell(unit, "pw start");
    bshell(unit, "pw report all");

    /* Send pacekt to port_1 */
    snprintf(str, 512, "tx 1 pbm=%d data=000000AAAAAA000000BBBBBB8100000B12341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234123412341234;sleep 5", port_in);
    printf("%s\n", str);
    bshell(unit, str);
    bshell(unit, "show c");
}

test_setup();
ts_config();
verify();
