/*
 * Example of L1/L2 forwarding for FlexE 1588 client (channel ID = 81)
 * Test Scenario 
 *
 * cd ../../../../src/examples/dnx
 * c ./port/cint_dynamic_port_add_remove.c
 * c ./cint_flexe_test.c
 * c ./cint_flexe_1588.c
 * c
 * cint_flexe_1588_main(0, 200, 1, 20);
 * exit;
 *
 * Tests:
 *
 * 1. FlexE 1588 L1 forwarding
 *     SOC properties:
 *         flexe_device_mode=CENTRALIZE
 *         ucode_port_13.BCM8848X=XE18:core_0.13:cross_connect
 *         ucode_port_20.BCM8848X=LGE0:core_0:flexe
 *
 *         Note: port 13 must be an ethernet port with Ethernet MAC (as an example, CPU ports do not have an ethernet MAC).
 *
 *     cint
 *         cint_flexe_1588_main(0, 13, 0, 20);
 *
 *     port lpb 20 mode=phy
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
 *         flexe_device_mode=CENTRALIZE
 *         ucode_port_20.BCM8848X=LGE0:core_0:flexe
 *
 *     cint
 *         cint_flexe_1588_main(0, 200, 1, 20);
 *
 *     port lpb 20 mode=phy
 *
 *     Traffic:
 *     On Q2A A0:
 *         tx 1 psrc=200 data=0x0010940000030010940000048100001288F7020000000116000200500000000002911803528
 *                              0011B1900000000A01EA0AA1688F70002002C00000000000000000000000000000000000000
 *                              00000000000000000000000000000000B01715552800FF8BFBF8B7
 *     On Q2A B0:
 *         tx 1 psrc=200 data=0x0010940000030010940000048100000B88F7020000000116000200500000000002911803528
 *                              0011B1900000000A01EA0AA1688F70002002C00000000000000000000000000000000000000
 *                              00000000000000000000000000000000B01715552800FF65C784E5
 *
 *     Expect to receive packet at port 200:
 *     On Q2A A0:
 *         ffffffffffff0000000000008100001288f7010000000116000200500000000004a70157ee93011b1900000000a01ea0aa1688f7
 *         0002002c0000000000215e9549b530000000000000000000000000000000000000000000000000b01715552800ff
 *     On Q2A B0:
 *         ffffffffffff0000000000008100000b88f7010000000116000200500000000004a70157ee93011b1900000000a01ea0aa1688f7
 *         0002002c0000000000215e9549b530000000000000000000000000000000000000000000000000b01715552800ff
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
           40,         41,         50000,      50000,            500,                0
};

/* FlexE 1588 main function */
int cint_flexe_1588_main(
    int unit,
    bcm_port_t eth_port,
    int is_l2,
    bcm_port_t flexe_phy_port)
{
    bcm_port_phy_timesync_config_t timesync_phy_config;
    bcm_port_t client_a;
    bcm_port_t client_1588;
    bcm_gport_t flexe_grp;
    int client_speed;
    int nof_client_slots;
    int nof_grp_slots;
    int speed_1588;
    int ii, rv;
    int flexe_gen;
    int start_lane;

    flexe_gen = *(dnxc_data_get(unit, "nif", "framer", "gen", NULL));

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

    /* Create FlexE group */
    BCM_FLEXE_GROUP_GPORT_SET(&flexe_grp, 0);
    flexe_phy_ports[0] = flexe_phy_port;
    rv = dnx_flexe_util_group_create(unit, flexe_grp, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to create flexe_grp\n", rv);
        return rv;
    }

    /* Create busA client */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to create busA clients\n", rv);
        return rv;
    }

    if (is_l2)
    {
        if (flexe_gen < 2) {
            /* Create 1588 client */
            rv = dnx_flexe_util_client_create(unit, client_1588, port_type_1588_L2, speed_1588, 0);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), fail to create 1588 client\n", rv);
                return rv;
            }
        } else {
            /* FlexE_Gen2: Create PTP MGMT client. The speed is fixed with 10G */
            speed_1588 = 10000;
            rv = dnx_flexe_util_client_create(unit, client_1588, port_type_L2_mgmt_ptp, speed_1588, 0);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), fail to create PTP MGMT client\n", rv);
                return rv;
            }
        }
    }
    else
    {
        if (flexe_gen < 2) {
            /* Create 1588 client */
            rv = dnx_flexe_util_client_create(unit, client_1588, port_type_1588_L1, speed_1588, 0);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), fail to create 1588 client\n", rv);
                return rv;
            }
        } else {
            /* FlexE_Gen2: Create PTP L1 interface. The speed is fixed with 10G. And MGMT channel is 3 */
            speed_1588 = 10000;
            rv = dnx_flexe_util_L1_eth_add(unit, client_1588, start_lane, 1, speed_1588, bcmPortPhyFecNone, 3);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), fail to create PTP L1 interface\n", rv);
                return rv;
            }
        }
    }

    if (flexe_gen < 2) {
        rv |= bcm_port_enable_set(unit, client_1588, 0);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), fail to disable 1588 client\n", rv);
            return rv;
        }
    }

    if (is_l2)
    {
        /* Force forward: eth_port -> client_1588 */
        rv = bcm_port_force_forward_set(unit, eth_port, client_1588, 1);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), fail to set force forward eth_port -> client_1588\n", rv);
            return rv;
        }

        /* Force forward: client_1588 -> eth_port  */
        rv = bcm_port_force_forward_set(unit, client_1588, eth_port, 1);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), fail to set force forward client_1588 -> eth_port\n", rv);
            return rv;
        }
    }
    else
    {
        if (flexe_gen < 2) {        
            /* Disable ports */
            rv = bcm_port_enable_set(unit, eth_port, 0);
            
            /* Start to build L1 datapath: eth_port -> client_1588 */
            rv = bcm_port_flexe_flow_set(unit, 0, eth_port, 0, client_1588);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), fail in bcm_port_flexe_flow_set, eth_port -> client_1588 ****\n", rv);
                return rv;
            }
            
            /* Start to build L1 datapath: client_1588 -> eth_port  */
            rv = bcm_port_flexe_flow_set(unit, 0, client_1588, 0, eth_port);
            if (rv != BCM_E_NONE) {
                printf("Error(%d), fail in bcm_port_flexe_flow_set, client_1588 -> eth_port\n", rv);
                return rv;
            }
            
            /* Enable ports */
            rv = bcm_port_enable_set(unit, eth_port, 1);
        }
    }

    if (flexe_gen < 2) {
        rv |= bcm_port_enable_set(unit, client_1588, 1);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), fail to enable ports\n", rv);
            return rv;
        }
    }

    /* Calendar and OH setting in flexe_grp */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (ii = 0; ii < nof_client_slots; ii++) {
        Cal_Slots_A[ii] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_grp_slots);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in dnx_flexe_util_calendar_slots_set\n", rv);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to set OhGroupID\n", rv);
        return rv;
    }

    /* Enable 1588 on FlexE PHY */
    bcm_port_phy_timesync_config_t_init(&timesync_phy_config);
    timesync_phy_config.flags = (BCM_PORT_PHY_TIMESYNC_ENABLE | BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE);
    rv = bcm_port_enable_set(unit, flexe_phy_port, 0);
    rv |= bcm_port_phy_timesync_config_set(unit, flexe_phy_port, &timesync_phy_config);
    rv |= bcm_port_enable_set(unit, flexe_phy_port, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to enable 1588 on FlexE PHY\n", rv);
        return rv;
    }

    return rv;
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
    bcm_port_t client_L1_rx = 60;
    bcm_port_t client_L2_rx = 61;
    bcm_gport_t flexe_grp;
    int client_speed;
    int nof_client_slots;
    int nof_grp_slots;
    int speed_1588;
    int client_b_speed = 10000;
    int ii, rv;

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
    rv = dnx_flexe_util_group_create(unit, flexe_grp, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to create flexe_grp\n", rv);
        return rv;
    }

    /* Create busA client */
    rv = dnx_flexe_util_client_create(unit, client_a, port_type_busA, client_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to create busA clients\n", rv);
        return rv;
    }

    /* Create 1588 client */
    rv = dnx_flexe_util_client_create(unit, client_1588, port_type_1588_L1, speed_1588, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to create 1588 client\n", rv);
        return rv;
    }

    /* Create busB clients */
    rv = dnx_flexe_util_client_create(unit, client_L2_tx, port_type_busB_L2_L3, client_b_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_L1_tx, port_type_busB_L1, client_b_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_L1_rx, port_type_busB_L1, client_b_speed, 0);
    rv |= dnx_flexe_util_client_create(unit, client_L2_rx, port_type_busB_L2_L3, client_b_speed, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to create busB clients\n", rv);
        return rv;
    }

    /* Disable ports */
    rv = bcm_port_enable_set(unit, client_1588, 0);
    rv |= bcm_port_enable_set(unit, client_L2_tx, 0);
    rv |= bcm_port_enable_set(unit, client_L1_tx, 0);
    rv |= bcm_port_enable_set(unit, client_L1_rx, 0);
    rv |= bcm_port_enable_set(unit, client_L2_rx, 0);
    rv |= bcm_port_enable_set(unit, eth_port_tx, 0);
    rv |= bcm_port_enable_set(unit, eth_port_rx, 0);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to disable busB clients and ETH ports\n", rv);
        return rv;
    }

    /* Build Tx datapath:
     * cpu_port_tx --L2--> client_L2_tx --> client_L1_tx --> eth_port_tx
     */
    rv = bcm_port_force_forward_set(unit, cpu_port_tx, client_L2_tx, 1);
    rv |= bcm_port_flexe_flow_set(unit, 0, client_L2_tx, 0, client_L1_tx);
    rv |= bcm_port_flexe_flow_set(unit, 0, client_L1_tx, 0, eth_port_tx);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in building Tx datapath ****\n", rv);
        return rv;
    }

    /* Build 1588 datapath:
     * eth_port_tx --> client_1588 --> eth_port_rx
     */
    rv = bcm_port_flexe_flow_set(unit, 0, eth_port_tx, 0, client_1588);
    rv |= bcm_port_flexe_flow_set(unit, 0, client_1588, 0, eth_port_rx);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in building 1588 datapath\n", rv);
        return rv;
    }

    /* Build Rx datapath:
     * eth_port_rx --> client_L1_rx --> client_L2_rx --L2--> cpu_port_rx
     */
    rv = bcm_port_flexe_flow_set(unit, 0, eth_port_rx, 0, client_L1_rx);
    rv |= bcm_port_flexe_flow_set(unit, 0, client_L1_rx, 0, client_L2_rx);
    rv |= bcm_port_force_forward_set(unit, client_L2_rx, cpu_port_rx, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in building Rx datapath ****\n", rv);
        return rv;
    }

    /* Enable ports */
    rv = bcm_port_enable_set(unit, client_1588, 1);
    rv |= bcm_port_enable_set(unit, client_L2_tx, 1);
    rv |= bcm_port_enable_set(unit, client_L1_tx, 1);
    rv |= bcm_port_enable_set(unit, client_L1_rx, 1);
    rv |= bcm_port_enable_set(unit, client_L2_rx, 1);
    rv |= bcm_port_enable_set(unit, eth_port_tx, 1);
    rv |= bcm_port_enable_set(unit, eth_port_rx, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to enable busB clients and ETH ports\n", rv);
        return rv;
    }

    /* Calendar and OH setting in flexe_grp */
    dnx_flexe_util_cal_slots_clear(FLEXE_CAL_A);
    for (ii = 0; ii < nof_client_slots; ii++) {
        Cal_Slots_A[ii] = client_a;
    }
    rv = dnx_flexe_util_calendar_slots_set(unit, flexe_grp, 0, FLEXE_CAL_A, nof_grp_slots);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail in dnx_flexe_util_calendar_slots_set\n", rv);
        return rv;
    }
    rv = bcm_port_flexe_oh_set(unit, flexe_grp, BCM_PORT_FLEXE_TX, bcmPortFlexeOhGroupID, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to set OhGroupID\n", rv);
        return rv;
    }

    /* Enable 1588 on FlexE PHY */
    bcm_port_phy_timesync_config_t_init(&timesync_phy_config);
    timesync_phy_config.flags = (BCM_PORT_PHY_TIMESYNC_ENABLE | BCM_PORT_PHY_TIMESYNC_ONE_STEP_ENABLE);
    rv = bcm_port_enable_set(unit, flexe_phy_port, 0);
    rv |= bcm_port_phy_timesync_config_set(unit, flexe_phy_port, &timesync_phy_config);
    rv |= bcm_port_enable_set(unit, flexe_phy_port, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to enable 1588 on FlexE PHY\n", rv);
        return rv;
    }

    /* Loopback ports */
    rv = bcm_port_loopback_set(unit, eth_port_tx, BCM_PORT_LOOPBACK_PHY);
    rv |= bcm_port_loopback_set(unit, eth_port_rx, BCM_PORT_LOOPBACK_PHY);
    rv |= bcm_port_loopback_set(unit, flexe_phy_port, BCM_PORT_LOOPBACK_PHY);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), fail to loopback ports\n", rv);
        return rv;
    }

    return rv;
}

