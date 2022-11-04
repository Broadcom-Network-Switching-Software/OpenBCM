/*
 * Example of L1/L2 forwarding for FlexE 1588 client (channel ID = 81)
 * Test Scenario 
 *
 * Test Scenario - start
  cd ../../../../src/examples/dnx
  c ./port/cint_dynamic_port_add_remove.c
  c ./cint_flexe_test.c
  c ./cint_flexe_1588.c
  c
  cint_flexe_1588_main(0, 200, 1, 20);
  exit;
 *
 * Tests:
 *
 * 1. FlexE 1588 L1 forwarding
 *     SOC properties:
           config add flexe_device_mode=CENTRALIZE
           config add ucode_port_13.BCM8848X=XE18:core_0.13:cross_connect
           config add ucode_port_20.BCM8848X=LGE0:core_0:flexe
           tr 141
 *
 *         Note: port 13 must be an ethernet port with Ethernet MAC (as an example, CPU ports do not have an ethernet MAC).
 *
           cint
           cint_flexe_1588_main(0, 13, 0, 20);
           exit;
 *
           port lpb 20 mode=phy
 *
 *     On Q2A A0:
 *     Inject the following packet from the traffic generator:
 *         0010940000030010940000048100001288F70200000001160002005000000000029118035280011B1900000000A01EA0AA1688F7
 *         0002002C0000000000000000000000000000000000000000000000000000000000000000000000B01715552800FF8BFBF8B7
 *
 *     Expect to receive packet at port 13:
 *         ffffffffffff0000000000008100001288f7010000000116000200500000000004a70157ee93011b1900000000a01ea0aa1688f7
 *         0002002c0000000000215e9549b530000000000000000000000000000000000000000000000000b01715552800ff
 *
 *     On Q2A B0:
 *     Inject the following packet from the traffic generator:
 *         0010940000030010940000048100000B88F70200000001160002005000000000029118035280011B1900000000A01EA0AA1688F7
 *         0002002C0000000000000000000000000000000000000000000000000000000000000000000000B01715552800FF65C784E5
 *
 *     Expect to receive packet at port 13:
 *         ffffffffffff0000000000008100000b88f7010000000116000200500000000004a70157ee93011b1900000000a01ea0aa1688f7
 *         0002002c0000000000215e9549b530000000000000000000000000000000000000000000000000b01715552800ff
 *
 * 2. FlexE 1588 L2 forwarding
 *     SOC properties:
           config add flexe_device_mode=CENTRALIZE
           config add ucode_port_20.BCM8848X=LGE0:core_0:flexe
           tr 141
 *
           cint
           cint_flexe_1588_main(0, 200, 1, 20);
           exit;
 *
           port lpb 20 mode=phy
 *
 *     Traffic:
 *     On Q2A A0:
          tx 1 psrc=200 data=0x0010940000030010940000048100001288F70200000001160002005000000000029118035280011B1900000000A01EA0AA1688F70002002C0000000000000000000000000000000000000000000000000000000000000000000000B01715552800FF8BFBF8B7
 *     On Q2A B0:
          tx 1 psrc=200 data=0x0010940000030010940000048100000B88F70200000001160002005000000000029118035280011B1900000000A01EA0AA1688F70002002C0000000000000000000000000000000000000000000000000000000000000000000000B01715552800FF65C784E5
 *
 *     Expect to receive packet at port 200:
 *     On Q2A A0:
 *         ffffffffffff0000000000008100001288f7010000000116000200500000000004a70157ee93011b1900000000a01ea0aa1688f7
 *         0002002c0000000000215e9549b530000000000000000000000000000000000000000000000000b01715552800ff
 *     On Q2A B0:
 *         ffffffffffff0000000000008100000b88f7010000000116000200500000000004a70157ee93011b1900000000a01ea0aa1688f7
 *         0002002c0000000000215e9549b530000000000000000000000000000000000000000000000000b01715552800ff
 * Test Scenario - end
 *
 * Injected/Extracted packet format:
 *     1588oETH0oFlexE_PTPoETH1
 *     FlexE_PTP header format:
 *         Direction(1B):   02
 *         Port ID(1B):     00
 *         Mode Side(2B):   0000
 *         TS CTR(2B):      0116
 *         Msg type(1B):    00
 *         PTP ver(1B):     02
 *         Length(2B):      0050
 *         Timestamp(10B):  00000000029118035280
 *
 */

struct flexe_1588_client_info_t
{
    bcm_port_t busa_port;
    bcm_port_t busb_1588_port;
	int group_speed;
	int client_speed;
	int flexe_oh_1588_speed;
    int l1_mgmt_port_start_lane;
};

flexe_1588_client_info_t flexe_1588_client_info = {
    /* busa_port busb_1588_port group_speed client_speed flexe_oh_1588_speed l1_mgmt_port_start_lane */
           60,         90,         50000,      50000,            500,                0
};

int dnx_ptp_port_create(
    int unit,
    bcm_port_t port_1588,
    int speed_1588,
    int is_l2,
    int start_lane)
{
    int flexe_gen;

    flexe_gen = *(dnxc_data_get(unit, "nif", "framer", "gen", NULL));

    if (is_l2)
    {
        if (flexe_gen < 2) {
            /* Create 1588 client */
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, port_1588, port_type_1588_L2, speed_1588, 0), "");
        } else {
            /* FlexE_Gen2: Create PTP MGMT client. The speed is fixed with 10G */
            speed_1588 = 10000;
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, port_1588, port_type_L2_mgmt_ptp, speed_1588, 0), "fail to create PTP MGMT client");
        }
    }
    else
    {
        if (flexe_gen < 2) {
            /* Create 1588 client */
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, port_1588, port_type_1588_L1, speed_1588, 0), "fail to create 1588 client");
        } else {
            /* FlexE_Gen2: Create PTP L1 interface. The speed is fixed with 10G. And MGMT channel is 3 */
            speed_1588 = 10000;
            BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_L1_eth_add(unit, port_1588, start_lane, 1, speed_1588, bcmPortPhyFecNone, 3), "fail to create PTP L1 interface");
        }
    }

    return BCM_E_NONE;
}

int dnx_ptp_port_data_path_create(
    int unit,
    bcm_port_t eth_port,
    bcm_port_t ptp_port,
    int is_l2)
{
    int flexe_gen;

    flexe_gen = *(dnxc_data_get(unit, "nif", "framer", "gen", NULL));

    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ptp_port, 0), "fail to disable 1588 client");

    if (is_l2) {
        /* Force forward: eth_port -> client_1588 */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, eth_port, ptp_port, 1), "fail to set force forward eth_port -> client_1588");

        /* Force forward: client_1588 -> eth_port  */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, ptp_port, eth_port, 1), "fail to set force forward client_1588 -> eth_port");
    } else {
        if (flexe_gen < 2) {
            /* Disable ports */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port, 0), "");

            /* Start to build L1 datapath: eth_port -> client_1588 */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_port, 0, ptp_port), "eth_port -> client_1588");

            /* Start to build L1 datapath: client_1588 -> eth_port  */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, ptp_port, 0, eth_port), "client_1588 -> eth_port");

            /* Enable ports */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port, 1), "");
        }
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, ptp_port, 1), "fail to enable ports");

    return BCM_E_NONE;
}

/* FlexE 1588 main function */
int cint_flexe_1588_main(
    int unit,
    bcm_port_t eth_port,
    int is_l2,
    bcm_port_t flexe_phy_port,
    int flexe_group_id)
{
    bcm_port_phy_timesync_config_t timesync_phy_config;
    bcm_port_interface_info_t intf_info;
    bcm_port_mapping_info_t map_info;
    bcm_port_t client_a;
    bcm_port_t client_1588;
    bcm_gport_t flexe_grp;
    int client_speed;
    int nof_client_slots;
    int nof_grp_slots;
    int speed_1588;
    int ii, rv;
    int start_lane;
    uint32 flags;
    int is_l1_eth_port;
    int loopback_mode;


    /***************************************************************************
     * Datapath creation:
     *                                   FlexE Core
     *                         +----------------------------+
     *                         |                            |
     *     CPU / eth_port <--> |  client_1588  |  client_a  | <--> FlexE_PHY(loopback)
     *                         |                            |
     *                         +----------------------------+
     *
     ***************************************************************************/

    client_a = flexe_1588_client_info.busa_port;
    client_1588 = flexe_1588_client_info.busb_1588_port;
    nof_grp_slots = flexe_1588_client_info.group_speed/5000;
    client_speed = flexe_1588_client_info.client_speed;
    nof_client_slots = flexe_1588_client_info.client_speed/5000;
    speed_1588 = flexe_1588_client_info.flexe_oh_1588_speed;
    start_lane = flexe_1588_client_info.l1_mgmt_port_start_lane;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, flexe_phy_port, &flags, &intf_info, &map_info), "fail to get port info");

    is_l1_eth_port = flags & BCM_PORT_ADD_CROSS_CONNECT;
    /* Create PTP port if it is not created */
    rv = bcm_port_get(unit, client_1588, &flags, &intf_info, &map_info);
    if (rv != BCM_E_NONE) {
        BCM_IF_ERROR_RETURN_MSG(dnx_ptp_port_create(unit, client_1588, speed_1588, is_l2, start_lane), "fail to create ptp port");
        BCM_IF_ERROR_RETURN_MSG(dnx_ptp_port_data_path_create(unit, eth_port, client_1588, is_l2), "fail to create data path for ptp port");
    }

    if (is_l1_eth_port) {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, flexe_phy_port, bcmPortControlL1Eth1588RxEnable, 1), "failed to enable 1588 at l1 eth port");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, flexe_phy_port, bcmPortControlL1Eth1588TxEnable, 1), "failed to enable 1588 at l1 eth port");
    } else {
        /* Create FlexE group */
        BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, flexe_group_id);
        flexe_phy_ports[0] = flexe_phy_port;
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, 1), "fail to create flexe_grp");

        /* Create busA client */
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a + flexe_group_id, port_type_busA, client_speed, 0), "fail to create busA clients");

        /* Calendar and OH setting in flexe_grp */
        dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
        for (ii = 0; ii < nof_client_slots; ii++) {
            Cal_Slots_A[ii] = client_a + flexe_group_id;
        }
        BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_grp_slots),
            "fail in dnx_flexe_util_calendar_slots_set");
        BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, flexe_group_id + 1),
            "fail to set OhGroupID");
    }

    /* Enable 1588 on FlexE PHY */
    bcm_port_phy_timesync_config_t_init(&timesync_phy_config);
    timesync_phy_config.flags = (BCM_PORT_PHY_TIMESYNC_ENABLE | BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_get(unit, flexe_phy_port, &loopback_mode), "fail in bcm_port_loopback_get()");
    if (loopback_mode != BCM_PORT_LOOPBACK_NONE) {
        /** Clear loopback before disabling port */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy_port, BCM_PORT_LOOPBACK_NONE), "fail in bcm_port_loopback_set()");
        bshell(unit,"sleep 1");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_phy_port, 0), "fail to enable 1588 on FlexE PHY");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_phy_timesync_config_set(unit, flexe_phy_port, &timesync_phy_config), "fail to enable 1588 on FlexE PHY");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_phy_port, 1), "fail to enable 1588 on FlexE PHY");
    if (loopback_mode != BCM_PORT_LOOPBACK_NONE) {
        /** Restore loopback setting */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy_port, loopback_mode), "fail in bcm_port_loopback_set()");
        bshell(unit,"sleep 1");
    }
    return BCM_E_NONE;
}

/* FlexE 1588 L1 datapath. Just for test purpose */
int cint_flexe_1588_L1_test(
    int unit,
    bcm_port_t eth_port_tx,
    bcm_port_t eth_port_rx,
    bcm_port_t cpu_port_tx,
    bcm_port_t cpu_port_rx,
    bcm_port_t flexe_phy_port)
{
    bcm_port_phy_timesync_config_t timesync_phy_config;
    bcm_port_t client_a;
    bcm_port_t client_1588;
    bcm_port_t client_L2_tx = 50;
    bcm_port_t client_L1_tx = 51;
    bcm_port_t client_L1_rx = 70;
    bcm_port_t client_L2_rx = 71;
    bcm_gport_t flexe_grp;
    int client_speed;
    int nof_client_slots;
    int nof_grp_slots;
    int speed_1588;
    int client_b_speed = 10000;
    int ii;

    /***************************************************************************
     * Datapath:
     *                                   FlexE Core 
     *                         +----------------------------+
     *        (L2)             |                            |
     *     cpu_port_tx ------> |  client_L2_tx              |
     *                         |       |                    |
     *     (L1, loopback)      |       V                    |
     *     eth_port_tx <------ |  client_L1_tx              |
     *           |             |                            |
     *           +-----------> |  client_1588 <--> client_a | <--> FlexE_PHY(loopback)
     *                         |       |                    |
     *      (L1, loopback)     |       V                    |
     *     eth_port_rx <------ | <-----+                    |
     *           |
     *           +-----------> |  client_L1_rx              |
     *                         |       |                    |
     *        (L2)             |       V                    |
     *     cpu_port_rx <------ |  client_L2_rx              |
     *                         |                            |
     *                         +----------------------------+
     *
     ***************************************************************************/

    client_a = flexe_1588_client_info.busa_port;
    client_1588 = flexe_1588_client_info.busb_1588_port;
	nof_grp_slots = flexe_1588_client_info.group_speed/5000;
	client_speed = flexe_1588_client_info.client_speed;
	nof_client_slots = flexe_1588_client_info.client_speed/5000;
	speed_1588 = flexe_1588_client_info.flexe_oh_1588_speed;

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 0);
    flexe_phy_ports[0] = flexe_phy_port;
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_group_create(unit, flexe_grp, 1), "fail to create flexe_grp");

    /* Create busA client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0),
        "fail to create busA clients");

    /* Create 1588 client */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_1588, port_type_1588_L1, speed_1588, 0),
        "fail to create 1588 client");

    /* Create busB clients */
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_L2_tx, port_type_busB_L2_L3, client_b_speed, 0),
        "fail to create busB clients");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_L1_tx, port_type_busB_L1, client_b_speed, 0),
        "fail to create busB clients");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_L1_rx, port_type_busB_L1, client_b_speed, 0),
        "fail to create busB clients");
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_client_create(unit, client_L2_rx, port_type_busB_L2_L3, client_b_speed, 0),
        "fail to create busB clients");

    /* Disable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_1588, 0), "fail to disable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L2_tx, 0), "fail to disable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L1_tx, 0), "fail to disable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L1_rx, 0), "fail to disable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L2_rx, 0), "fail to disable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_tx, 0), "fail to disable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_rx, 0), "fail to disable busB clients and ETH ports");

    /* Build Tx datapath:
     * cpu_port_tx --L2--> client_L2_tx --> client_L1_tx --> eth_port_tx
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, cpu_port_tx, client_L2_tx, 1), "fail in building Tx datapath ****");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_L2_tx, 0, client_L1_tx), "fail in building Tx datapath ****");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_L1_tx, 0, eth_port_tx), "fail in building Tx datapath ****");

    /* Build 1588 datapath:
     * eth_port_tx --> client_1588 --> eth_port_rx
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_port_tx, 0, client_1588), "fail in building 1588 datapath");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_1588, 0, eth_port_rx), "fail in building 1588 datapath");

    /* Build Rx datapath:
     * eth_port_rx --> client_L1_rx --> client_L2_rx --L2--> cpu_port_rx
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, eth_port_rx, 0, client_L1_rx), "fail in building Rx datapath ****");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_flow_set(unit, 0, client_L1_rx, 0, client_L2_rx), "fail in building Rx datapath ****");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_force_forward_set(unit, client_L2_rx, cpu_port_rx, 1), "fail in building Rx datapath ****");

    /* Enable ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_1588, 1), "fail to enable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L2_tx, 1), "fail to enable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L1_tx, 1), "fail to enable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L1_rx, 1), "fail to enable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, client_L2_rx, 1), "fail to enable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_tx, 1), "fail to enable busB clients and ETH ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, eth_port_rx, 1), "fail to enable busB clients and ETH ports");

    /* Calendar and OH setting in flexe_grp */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (ii = 0; ii < nof_client_slots; ii++) {
        Cal_Slots_A[ii] = client_a;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_grp_slots),
        "fail in dnx_flexe_util_calendar_slots_set");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1),
        "fail to set OhGroupID");

    /* Enable 1588 on FlexE PHY */
    bcm_port_phy_timesync_config_t_init(&timesync_phy_config);
    timesync_phy_config.flags = (BCM_PORT_PHY_TIMESYNC_ENABLE | BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_phy_port, 0), "fail to enable 1588 on FlexE PHY");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_phy_timesync_config_set(unit, flexe_phy_port, &timesync_phy_config),
        "fail to enable 1588 on FlexE PHY");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, flexe_phy_port, 1), "fail to enable 1588 on FlexE PHY");

    /* Loopback ports */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_port_tx, BCM_PORT_LOOPBACK_PHY), "fail to loopback ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, eth_port_rx, BCM_PORT_LOOPBACK_PHY), "fail to loopback ports");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_loopback_set(unit, flexe_phy_port, BCM_PORT_LOOPBACK_PHY), "fail to loopback ports");

    return BCM_E_NONE;
}

