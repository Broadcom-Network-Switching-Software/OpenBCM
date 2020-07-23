/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: test_bfd.c
 */

/* Includes */
#include <appl/diag/shell.h>
#include <bcm/bfd.h>
#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/port.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <shared/bsl.h>
#include <sal/core/time.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/property.h>
#include <appl/diag/dpp/test_oamp.h>
#include <sal/core/sync.h>

/* Global Variables */
int test_bfd_local_encap;                               /* Local encap */
int test_bfd_remote_encap;                              /* Remote encap */
int test_bfd_time_out;                                  /* Variable used for check for time-out events */
int test_bfd_period[TEST_BFD_MAX_NUM_PERIODS];          /* Random predefined values for BFD transmission rate for local endpoints */
int test_bfd_def_period[TEST_BFD_NUM_DEF_PERIODS];      /* Predefined values for BFD transmission rate for local endpoints */
int test_bfd_IP_address[TEST_BFD_NUM_IP];               /* IP addresses */
int test_bfd_intervals[TEST_BFD_MAX_NUM_INTERVALS];     /* Values for Desired Min TX and Required Min RX Intervals */
int test_bfd_state_change;                              /* Number of bcmBFDEventStateChange occurred*/
int test_bfd_number_states;                             /* Number of times that state will be changed */
test_bfd_lock_t test_bfd_info_lock;                     


int test_bfd_lock_init(int unit) {
    test_bfd_lock_t *temp_state = &test_bfd_info_lock;
    BCMDNX_INIT_FUNC_DEFS;

    if (temp_state->is_init==0) {
        sal_memset(temp_state, 0, sizeof(test_bfd_lock_t));
        temp_state->lock = sal_mutex_create("test_bfd_info_lock");
        if (!temp_state->lock) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Failed to create mutex\r\n")));
        }
        temp_state->is_init =1;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
  * Set values for:
  * TX periods
  * Desired Min TX Intervals
  * Required Min RX Intervals
  * Source and Destination IPs
*/
void test_bfd_set_global_values(void) {
    int i;

    for(i = 0; i < TEST_BFD_MAX_NUM_PERIODS ;i++) {
        test_bfd_period[i] = sal_rand() % 996 + 4;     /* random value between 4 - 1000 */
    }

    for(i=0; i < TEST_BFD_MAX_NUM_INTERVALS ;i++) {
        test_bfd_intervals[i] = sal_rand() % 1000000;    /* random value between 0 - 1,000,000 */
    }

/* Predifined periods.
   Needed when soc property custom_feature_bfd_fixed_tx_rate_mgmt is set to 1. */
    test_bfd_def_period[0] = 4;
    test_bfd_def_period[1] = 10;
    test_bfd_def_period[2] = 20;
    test_bfd_def_period[3] = 50;
    test_bfd_def_period[4] = 100;
    test_bfd_def_period[5] = 1000;

    /* IP addresses for MEPs */
    test_bfd_IP_address[0] = 0x01010164;
    test_bfd_IP_address[1] = 0x02010164;
    test_bfd_IP_address[2] = 0x03010164;
    test_bfd_IP_address[3] = 0x04010164;
    test_bfd_IP_address[4] = 0x05010164;
    test_bfd_IP_address[5] = 0x06010164;
    test_bfd_IP_address[6] = 0x07010164;
    test_bfd_IP_address[7] = 0x08010164;
}

/* Create Endpoint */
int test_bfd_createEndpoint(int unit,int local_port, int encap_id, int src_ip,
                 int remote_ip, int local_discr, int remote_discr) {
    int ret;
    int gport = local_port;
    int tx_i;
    int rx_i;
    bcm_bfd_endpoint_info_t bfd_endpoint_info = {0};
    BCMDNX_INIT_FUNC_DEFS;

    tx_i = sal_rand() % 8;
    rx_i = sal_rand() % 8;

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);
    bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_IN_HW | BCM_BFD_ENDPOINT_MULTIHOP;
    bfd_endpoint_info.src_ip_addr = src_ip;
    bfd_endpoint_info.dst_ip_addr = remote_ip;
    bfd_endpoint_info.local_discr = local_discr;
    bfd_endpoint_info.remote_discr = remote_discr;
    bfd_endpoint_info.type = bcmBFDTunnelTypeUdp;
    BCM_GPORT_MODPORT_SET(bfd_endpoint_info.tx_gport, unit, gport);

    bfd_endpoint_info.bfd_period = 40;
    bfd_endpoint_info.ip_ttl = 255;
    bfd_endpoint_info.udp_src_port = 49152;
    bfd_endpoint_info.egress_if = encap_id;
    bfd_endpoint_info.local_min_tx = test_bfd_intervals[tx_i];
    bfd_endpoint_info.local_min_rx = test_bfd_intervals[rx_i];
    bfd_endpoint_info.local_detect_mult = TEST_BFD_DETEC_MULT_3;
    bfd_endpoint_info.loc_clear_threshold = 1;
    bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_EXPLICIT_DETECTION_TIME;
    bfd_endpoint_info.bfd_detection_time = TEST_BFD_DETECTION_TIME_10MS;

    ret = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    BCMDNX_IF_ERR_EXIT(ret);
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Create Endpoint with ID[%d]:\t\t[DONE]\n"),bfd_endpoint_info.id));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Update Endpoint period and detection time
 * Test flow
  1. Change period of first endpoint to random number between 4 and 1000
  2. Change detection_time of second endpoint to random number between 1.5 * Period and 2.5 * Period
  3. Sleep 4 seconds
  4. If time-out occurred return FAIL.
  5. Destroy endpoints at random
*/
int test_bfd_updateEndpoint(int unit, int endpoint_1_id, int endpoint_2_id ) {
    bcm_bfd_endpoint_info_t bfd_endpoint_info_1 = {0};
    bcm_bfd_endpoint_info_t bfd_endpoint_info_2 = {0};
    int ret;
    int detTime;
    int period_i;
    int origin_period;
    int origin_detTime;
    int del_enable;
    BCMDNX_INIT_FUNC_DEFS;

    test_bfd_time_out = 0;
    period_i = sal_rand() % 7;
    detTime = sal_rand() % 1000 + 1500;

    ret = bcm_bfd_endpoint_get(unit,endpoint_1_id, &bfd_endpoint_info_1);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_bfd_endpoint_get(unit,endpoint_2_id, &bfd_endpoint_info_2);
    BCMDNX_IF_ERR_EXIT(ret);

    origin_period = bfd_endpoint_info_1.bfd_period;
    origin_detTime = bfd_endpoint_info_2.bfd_detection_time;
    bfd_endpoint_info_1.bfd_period = test_bfd_period[period_i];
    bfd_endpoint_info_2.bfd_detection_time = detTime * test_bfd_period[period_i];

    bfd_endpoint_info_1.flags |= BCM_BFD_ENDPOINT_UPDATE;
    bfd_endpoint_info_1.flags |= BCM_BFD_ENDPOINT_WITH_ID;

    bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_UPDATE;
    bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_WITH_ID;

    ret = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info_1);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info_2);
    BCMDNX_IF_ERR_EXIT(ret);

    /* Updating the period may have a ~4 second delay */
    sal_sleep(4);

    if (test_bfd_time_out >= 1) {
        cli_out("[TIMEOUT]Endpoint_1[%d] update Period from value=%d to value=%d\n",endpoint_1_id,origin_period,bfd_endpoint_info_1.bfd_period);
        cli_out("[TIMEOUT]Endpoint_2[%d] update DetTime from value=%d to value=%d\n",endpoint_2_id,origin_detTime,bfd_endpoint_info_2.bfd_detection_time);
        cli_out("[TIMEOUT]Endpoint_1[%d] Desire min TX  value=%d to Require min RX value=%d\n",endpoint_1_id,bfd_endpoint_info_1.local_min_tx,bfd_endpoint_info_1.local_min_rx);
        cli_out("[TIMEOUT]Endpoint_2[%d] Desire min TX  value=%d to Require min RX value=%d\n",endpoint_2_id,bfd_endpoint_info_2.local_min_tx,bfd_endpoint_info_2.local_min_rx);
        return BCM_E_FAIL;
    }

    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Remote endpoint of endpoint[%d] is endpoint[%d]\n"),bfd_endpoint_info_1.id,bfd_endpoint_info_1.remote_id));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Remote endpoint of endpoint[%d] is endpoint[%d]\n"),bfd_endpoint_info_2.id,bfd_endpoint_info_2.remote_id));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Update period of Endpoint[%d] from %d to %d\n"),endpoint_1_id,origin_period,bfd_endpoint_info_1.bfd_period));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Update detection Time of Endpoint[%d] from %d to %d\n"),endpoint_2_id,origin_detTime,bfd_endpoint_info_2.bfd_detection_time));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Update Endpoint:\t\t[DONE]\n")));

    /* Randomly choose to destroy endpoints */
    del_enable = sal_rand() % 2;
    if (del_enable) {
        ret = bcm_bfd_endpoint_destroy(unit, bfd_endpoint_info_1.id);
        BCMDNX_IF_ERR_EXIT(ret);
        LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Destroyed Endpoint[%d]:\t\t[DONE]\n"),endpoint_1_id));
        ret = bcm_bfd_endpoint_destroy(unit, bfd_endpoint_info_2.id);
        BCMDNX_IF_ERR_EXIT(ret);
        LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Destroyed Endpoint[%d]:\t\t[DONE]\n"),endpoint_2_id));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Update Endpoint period and detection time with predefined values
 * For this test the SOC property "custom_feature_bfd_fixed_tx_rate_mgmt"  must be set to 1
 * Test flow
  1. Change period of first endpoint to random number from {4,10,20,50,100,1000}
  2. Change detection_time of second endpoint to random number between 1.5 * Period and 2.5 * Period
  3. Sleep (2*Detection Time of remote Endpoint) micro seconds
  4. If time-out occurred return FAIL.
  5. Destroy endpoints at random
*/
int test_bfd_preDefinedUpdate(int unit, int endpoint_1_id, int endpoint_2_id) {
    bcm_bfd_endpoint_info_t bfd_endpoint_info_1 = {0};
    bcm_bfd_endpoint_info_t bfd_endpoint_info_2 = {0};
    int ret;
    int detTime;
    int period_i;
    int origin_period;
    int origin_detTime;
    int del_enable;
    BCMDNX_INIT_FUNC_DEFS;

    test_bfd_time_out = 0;

    period_i = sal_rand() % 6;
    detTime = sal_rand() % 1000 + 3500; 

    ret = bcm_bfd_endpoint_get(unit,endpoint_1_id, &bfd_endpoint_info_1);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_bfd_endpoint_get(unit,endpoint_2_id, &bfd_endpoint_info_2);
    BCMDNX_IF_ERR_EXIT(ret);

    origin_period = bfd_endpoint_info_1.bfd_period;
    origin_detTime = bfd_endpoint_info_2.bfd_detection_time;
    bfd_endpoint_info_1.bfd_period = test_bfd_def_period[period_i];
    bfd_endpoint_info_2.bfd_detection_time = detTime * test_bfd_def_period[period_i];

    bfd_endpoint_info_1.flags |= BCM_BFD_ENDPOINT_UPDATE;
    bfd_endpoint_info_1.flags |= BCM_BFD_ENDPOINT_WITH_ID;

    bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_UPDATE;
    bfd_endpoint_info_2.flags |= BCM_BFD_ENDPOINT_WITH_ID;

    ret = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info_1);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info_2);
    BCMDNX_IF_ERR_EXIT(ret);

    /* Need delay because packets will be sent from one endpoint to another before continuing with the next iteration */
    sal_usleep(2*bfd_endpoint_info_2.bfd_detection_time);

    if (test_bfd_time_out >= 1) {
        cli_out("\n[TIMEOUT]Wait time=%d\n",(2*bfd_endpoint_info_2.bfd_detection_time));
        cli_out("\n[TIMEOUT]Detection multiplier value=%d\n",detTime);
        cli_out("[TIMEOUT]Endpoint_1[%d] update Period from value=%d to value=%d\n",endpoint_1_id,origin_period,bfd_endpoint_info_1.bfd_period);
        cli_out("[TIMEOUT]Endpoint_2[%d] update DetTime from value=%d to value=%d\n",endpoint_2_id,origin_detTime,bfd_endpoint_info_2.bfd_detection_time);
        cli_out("[TIMEOUT]Endpoint_1[%d] Desire min TX  value=%d to Require min RX value=%d\n",endpoint_1_id,bfd_endpoint_info_1.local_min_tx,bfd_endpoint_info_1.local_min_rx);
        cli_out("[TIMEOUT]Endpoint_2[%d] Desire min TX  value=%d to Require min RX value=%d\n",endpoint_2_id,bfd_endpoint_info_2.local_min_tx,bfd_endpoint_info_2.local_min_rx);
        return BCM_E_FAIL;
    }

    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Remote endpoint of endpoint[%d] is endpoint[%d]\n"),bfd_endpoint_info_1.id,bfd_endpoint_info_1.remote_id));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Remote endpoint of endpoint[%d] is endpoint[%d]\n"),bfd_endpoint_info_2.id,bfd_endpoint_info_2.remote_id));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Update period of Endpoint[%d] from %d to %d\n"),endpoint_1_id,origin_period,bfd_endpoint_info_1.bfd_period));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Update detection Time of Endpoint[%d] from %d to %d\n"),endpoint_2_id,origin_detTime,bfd_endpoint_info_2.bfd_detection_time));
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Update Endpoint:\t\t[DONE]\n")));

    /* Randomly choose to destroy endpoints */
    del_enable = sal_rand() % 2;

    if (del_enable) {
        ret = bcm_bfd_endpoint_destroy(unit, bfd_endpoint_info_1.id);
        BCMDNX_IF_ERR_EXIT(ret);
        LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Destroyed Endpoint[%d]:\t\t[DONE]\n"),endpoint_1_id));
        ret = bcm_bfd_endpoint_destroy(unit, bfd_endpoint_info_2.id);
        BCMDNX_IF_ERR_EXIT(ret);
        LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"Destroyed Endpoint[%d]:\t\t[DONE]\n"),endpoint_2_id));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Repeated State updates
 * Test flow
  1. Change local_state in first endpoint
  2. Increment test_bfd_number_states
  2. If bcmBFDEventStateChange occurred increment test_bfd_state_change
  4. Wait test_bfd_number_states*100ms
  4. If timeout occurred or test_bfd_state_change != test_bfd_number_states return FAIL
*/
int test_bfd_changeState(int unit, int endpoint_id) {
    int ret;
    BCMDNX_INIT_FUNC_DEFS;

    test_bfd_state_change = 0;

    /* At this stage,state change interrupts will occur ,each state change will trigger a state change in its partner endpoint.*/
    ret = test_bfd_update_state(unit,endpoint_id);
    BCMDNX_IF_ERR_EXIT(ret);

    /* Wait for all events to occur */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        sal_usleep(test_bfd_number_states * 50000);
    } else {
        sal_usleep(test_bfd_number_states * 100000);
    }

    if (test_bfd_time_out >= 1) {
        cli_out("[State Error]Timeout event occur !\n");
        cli_out("[State Error]Endpoints %d and %d\n",endpoint_id,(endpoint_id+1));
        cli_out("[State Error]Expected:%d \tActual:%d \tTimeouts:%d\n",
                                            test_bfd_number_states,test_bfd_state_change,test_bfd_time_out);
        return BCM_E_FAIL;
    }

    if (test_bfd_number_states != test_bfd_state_change) {
        cli_out("[State Error]State changes are different than expected !\n");
        cli_out("[State Error]Endpoints %d and %d\n",endpoint_id,(endpoint_id+1));
        cli_out("[State Error]Expected:%d \tActual:%d \tTimeouts:%d\n",
                                            test_bfd_number_states,test_bfd_state_change,test_bfd_time_out);
        return BCM_E_FAIL;
    }
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"--> |STATES|\n\t--> Expected:%d \tActual:%d \tTimeouts:%d\n\n"),
                                                test_bfd_number_states,test_bfd_state_change,test_bfd_time_out));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Multithreading
 * Test flow
  1. Change local_state in first endpoint
   1.1 Take mutex
   1.2 Update local_state
   1.3 Release mutex
  2. Change local_state in first endpoint
   2.1 Take mutex
   2.2 Update local_min_tx in first endpoint
   2.3 Release mutex
  3. If bcmBFDEventStateChange occurred increment test_bfd_state_change
  4. Wait test_bfd_number_states*100ms
  5. If timeout occurred or test_bfd_state_change != test_bfd_number_states return FAIL
*/
int test_bfd_Multithreading(int unit, int endpoint_id) {
    int ret;
    int index;
    int tx_i;
    bcm_bfd_endpoint_info_t bfd_endpoint_info = {0};
    BCMDNX_INIT_FUNC_DEFS;

    test_bfd_state_change = 0;

/* At this stage,state change interrupts will occur ,each state change will trigger a state change in its partner endpoint.*/
    ret = test_bfd_update_state(unit,endpoint_id);
    BCMDNX_IF_ERR_EXIT(ret);

/* Update local_state while updating test_bfd_intervals */
    for (index = 0; index < test_bfd_number_states ; index++) {

        tx_i = sal_rand() % 8;

        TEST_BFD_INFO_LOCK
        ret = bcm_bfd_endpoint_get(unit,endpoint_id, &bfd_endpoint_info);
        BCMDNX_IF_ERR_EXIT(ret);

        bfd_endpoint_info.local_min_tx = test_bfd_intervals[tx_i];
        bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_UPDATE;
        bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_WITH_ID;

        ret = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
        BCMDNX_IF_ERR_EXIT(ret);
        TEST_BFD_INFO_UNLOCK
    }

    /* Wait extra time for first mep to initialize in 64B mode */
    if (endpoint_id==1 || endpoint_id==2) {
        /* Wait for all events to occur */
        sal_usleep(test_bfd_number_states * 1200000);
    } else {
        /* Wait for all events to occur */
        sal_usleep(test_bfd_number_states * 700000);
    }
    if (test_bfd_time_out >= 1) {
        cli_out("[Multithreading Error]Timeout event occur !\n");
        cli_out("[Multithreading Error]Endpoints %d and %d\n",endpoint_id,(endpoint_id+1));
        cli_out("[Multithreading Error]Expected:%d \tActual:%d \tTimeouts:%d\n",
                                        test_bfd_number_states,test_bfd_state_change,test_bfd_time_out);
        return BCM_E_FAIL;
    }

    if (test_bfd_number_states != test_bfd_state_change) {
        cli_out("[Multithreading Error]State changes are different than expected !\n");
        cli_out("[Multithreading Error]Endpoints %d and %d\n",endpoint_id,(endpoint_id+1));
        cli_out("[Multithreading Error]Expected:%d \tActual:%d \tTimeouts:%d\n",
                                            test_bfd_number_states,test_bfd_state_change,test_bfd_time_out);
        return BCM_E_FAIL;
    }
    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"--> |Multithreading|\n\t--> Expected:%d \tActual:%d \tTimeouts:%d\n\n"),
                                                test_bfd_number_states,test_bfd_state_change,test_bfd_time_out));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Add host for source IP */
int test_bfd_add_host(int unit, int addr, int vrf, int intf) {
    int ret;
    bcm_l3_host_t l3host;
    BCMDNX_INIT_FUNC_DEFS;

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = addr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = intf;
    l3host.l3a_modid = 0;
    l3host.l3a_port_tgid = 0;

    ret = bcm_l3_host_add(unit, &l3host);
    BCMDNX_IF_ERR_EXIT(ret);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Change local_state of the endpoints with random value between 0-3 */
int test_bfd_update_state(int unit, int endpoint_id) {
    bcm_bfd_endpoint_info_t bfd_endpoint_info = {0};
    int ret;
    int state;                              /* Next state*/
    int origin_state;                       /* Original state of the Endpoint*/
    BCMDNX_INIT_FUNC_DEFS;

    TEST_BFD_INFO_LOCK
    ret = bcm_bfd_endpoint_get(unit,endpoint_id, &bfd_endpoint_info);
    BCMDNX_IF_ERR_EXIT(ret);

    origin_state = bfd_endpoint_info.local_state;
    do {
        state = sal_rand() % 4;
    }
    while (state == origin_state);

    bfd_endpoint_info.local_state = state;
    bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_UPDATE;
    bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_WITH_ID;

    ret = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    BCMDNX_IF_ERR_EXIT(ret);
    TEST_BFD_INFO_UNLOCK

    LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"|STATE CHANGED|\n--> EP[%d] *** Old State[%d] *** New State[%d]\n"),
                        bfd_endpoint_info.id,origin_state,bfd_endpoint_info.local_state));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set CallBacks */
int test_bfd_cb(int unit,uint32 flags,bcm_bfd_event_types_t event_types,bcm_bfd_endpoint_t endpoint,void *user_data) {
    int event_i;
    int ret;

    BCMDNX_INIT_FUNC_DEFS;

    for (event_i = 0; event_i < bcmBFDEventCount; event_i++) {
        if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i)) {
            switch (event_i) {
            case bcmBFDEventEndpointTimeout:
                test_bfd_time_out++;
                cli_out("[TIMEOUT EVENT] Endpoint ID[%d]\n",endpoint);
                break;
            case bcmBFDEventEndpointTimein:
                break;
            case bcmBFDEventStateChange:
                if (test_bfd_state_change >= test_bfd_number_states) {
                    break;
                }else{
                    test_bfd_state_change++;
                    ret = test_bfd_update_state(unit,endpoint);
                    BCMDNX_IF_ERR_EXIT(ret);
                    break;
                }
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Events registration */
int test_bfd_register_events(int unit) {
    int ret;
    bcm_bfd_event_types_t event_type;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_BFD_EVENT_TYPE_CLEAR_ALL(event_type);
    BCM_BFD_EVENT_TYPE_SET(event_type, bcmBFDEventEndpointTimeout);
    ret = bcm_bfd_event_register(unit, event_type, test_bfd_cb, (void*) 2);
    if (ret != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(ret);
    }

    BCM_BFD_EVENT_TYPE_SET(event_type, bcmBFDEventEndpointTimein);
    ret = bcm_bfd_event_register(unit, event_type, test_bfd_cb, (void*) 1 );
    if (ret != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(ret);
    }

    BCM_BFD_EVENT_TYPE_SET(event_type, bcmBFDEventStateChange);
    ret = bcm_bfd_event_register(unit, event_type, test_bfd_cb, (void*) 1 );
    if (ret != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(ret);
    }

    ret = BCM_E_NONE;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Port initialization */
int test_bfd_init_ports(int unit,int local_port, int remote_port, int vlan) {
    int ret;
    int i_ip;
    bcm_l3_intf_t l3_intf_local;
    bcm_l3_egress_t l3eg;
    bcm_if_t local_egid;
    bcm_if_t remote_egid;

    bcm_mac_t mymac = {0x00, 0x00, 0x64, 0x01, 0x01, 0x02};
    bcm_mac_t rmac = {0x00, 0x00, 0x64, 0x01, 0x01, 0x02};
    BCMDNX_INIT_FUNC_DEFS;

    ret = bcm_bfd_init(unit);
    /* BFD might be already initialized */
    if (ret != BCM_E_INIT) {
        BCMDNX_IF_ERR_EXIT(ret);
        LOG_VERBOSE(BSL_LS_BCM_BFD,(BSL_META_U(unit,"BFD Initialization:\t\t[DONE]\n")));
    }

    bcm_l3_intf_t_init(&l3_intf_local);
    l3_intf_local.l3a_vid = vlan;
    l3_intf_local.l3a_mtu = 1500;
    sal_memcpy(l3_intf_local.l3a_mac_addr, mymac, 6);
    l3_intf_local.l3a_vrf = 0;
    ret = bcm_l3_intf_create(unit, &l3_intf_local);
    BCMDNX_IF_ERR_EXIT(ret);

    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = l3_intf_local.l3a_intf_id;
    l3eg.vlan   = l3_intf_local.l3a_vid;
    l3eg.port = local_port;
    l3eg.failover_id = 0;
    l3eg.failover_if_id = 0;
    sal_memcpy(l3eg.mac_addr, rmac, 6);
    /* configure only egress side */
    l3eg.flags = BCM_L3_VIRTUAL_OBJECT;
    ret = bcm_l3_egress_create(unit, l3eg.flags, &l3eg, &local_egid);
    BCMDNX_IF_ERR_EXIT(ret);
    test_bfd_local_encap = l3eg.encap_id;

    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = l3_intf_local.l3a_intf_id;
    l3eg.vlan   = l3_intf_local.l3a_vid;
    l3eg.port = remote_port;
    l3eg.failover_id = 0;
    l3eg.failover_if_id = 0;
    sal_memcpy(l3eg.mac_addr, rmac, 6);
    /* configure only egress side */
    l3eg.flags = BCM_L3_VIRTUAL_OBJECT;
    ret = bcm_l3_egress_create(unit, l3eg.flags, &l3eg, &remote_egid);
    BCMDNX_IF_ERR_EXIT(ret);
    test_bfd_remote_encap = l3eg.encap_id;

    /* Adding host for every source IP */
    for(i_ip = 0; i_ip < 8; i_ip++) {
        test_bfd_add_host(unit,test_bfd_IP_address[i_ip],l3_intf_local.l3a_vrf,local_egid);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Test driver 
 * The test flow will be as follows:
  1. Set a loopback connection to port_1 and port_2.
  2. Initialize BFD
  3. Set up LIFs and RIFs.
  4. Register events
  5. Create N pairs of BFD endpoints: For each pair, endpoint1.remote_discr == endpoint2.local_discr and the other way around.
  6. Perform the tests for each of the pairs
  7. Destroy all endpoints
  8. Verify BFD Timeout events
*/
int test_run_bfd(int unit, int port_1, int port_2, int numEndpoints, int seed, int numStates, int BFDTimeTest, int BFDTimeoutEventsTest) {
   int ret;
   int desc_ep_1;
   int desc_ep_2;
   int ep_i;
   int ip_i_1;
   int ip_i_2;
   int endpoint_id;        /* Endpoint discriminator*/
   int thread_test;        /* Choose when to run Multithreading test */
   uint32  max_total_endpoint_create_time = 0;
   int     MEPnum;
   uint32  start_time = 0;
   uint32  end_time_2 = 0;
   uint32  time_acc = 0;
   uint32  time_no = 0;
   uint32  end_time_1 = 0;
   int convert_endpoint_id_1, convert_endpoint_id_2;

   BCMDNX_INIT_FUNC_DEFS;
   test_bfd_lock_init(unit);
   sal_srand(seed);
   test_bfd_set_global_values();
   test_bfd_number_states = numStates;

   if (!BFDTimeTest) {
      /*     With this loopbacks EP1 will send to port_1 and receive on port_2, while EP2 will receive on port-1 and send on port_2 */
      ret = bcm_port_loopback_set(unit, port_1, BCM_PORT_LOOPBACK_PHY);
      BCMDNX_IF_ERR_EXIT(ret);
      ret = bcm_port_loopback_set(unit, port_2, BCM_PORT_LOOPBACK_PHY);
      BCMDNX_IF_ERR_EXIT(ret);
      LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "LoopBack set:\t\t[DONE]\n")));

      ret = test_bfd_init_ports(unit, port_1, port_2, TEST_BFD_VLAN);
      BCMDNX_IF_ERR_EXIT(ret);
      LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "Initialization ports:\t\t[DONE]\n")));
      ret = test_bfd_register_events(unit);
      BCMDNX_IF_ERR_EXIT(ret);
      LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "Register Events:\t\t[DONE]\n")));
   }

   endpoint_id = 1;

   if (BFDTimeTest) {
      MEPnum = 2 * numEndpoints;
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {

         /* First 2 EP create take 82000usec due to configuration of OAM1/2 entries.
            Following EP creates takes 3000usec. Every EP create after take 8usec
            longer (in average)
            Total EP Create Time = 32300 + 4500 * (n-2) + 9/2 * (n-2)^2
            Additional 10% added for up-limit */

          max_total_endpoint_create_time  = 1.1 * (32300 + 4500 * (MEPnum - 2) + (9 * (MEPnum - 2) * (MEPnum - 2)) / 2);
      } else {
         /* First 2 EP create take 82000usec due to configuration of OAM1/2 entries.
            Following EP creates takes 3000usec. Every EP create after take 8usec
            longer (in average)
            Total EP Create Time = 24000 + 1800 * (n-2) + 4.8/2 * (n-2)^2
            Additional 10% added for up-limit */
         max_total_endpoint_create_time  = 1.1 * (24000 + 1800 * (MEPnum - 2) + (24 * (MEPnum - 2) * (MEPnum - 2)) / 10); /* Max time in usec */
      }
      time_no = 0; time_acc = 0;
   }

   /* Creation of N-pairs BFD endpoints */
   for (ep_i = 0; ep_i < numEndpoints; ep_i++) {

      desc_ep_1 = endpoint_id;
      desc_ep_2 = endpoint_id + 1;

      if (SOC_IS_QUX(unit)) {
          convert_endpoint_id_1 = (((desc_ep_1 & 0x700) << 2) | (desc_ep_1 & 0xff));
          convert_endpoint_id_2 = (((desc_ep_2 & 0x700) << 2) | (desc_ep_2 & 0xff));
      } else {
          convert_endpoint_id_1 = desc_ep_1;
          convert_endpoint_id_2 = desc_ep_2;
      }

      thread_test = sal_rand() % 2;

      do {
         ip_i_1 = sal_rand() % 8;
         ip_i_2 = sal_rand() % 8;
      }
      while (ip_i_1 == ip_i_2);

      if (BFDTimeTest)    start_time = sal_time_usecs();
      ret = test_bfd_createEndpoint(unit, port_1, test_bfd_local_encap, test_bfd_IP_address[ip_i_1], test_bfd_IP_address[ip_i_2], convert_endpoint_id_1, convert_endpoint_id_2);
      BCMDNX_IF_ERR_EXIT(ret);
      if (BFDTimeTest)    end_time_1 = sal_time_usecs(); 

      ret = test_bfd_createEndpoint(unit, port_2, test_bfd_remote_encap, test_bfd_IP_address[ip_i_2], test_bfd_IP_address[ip_i_1], convert_endpoint_id_2, convert_endpoint_id_1);
      BCMDNX_IF_ERR_EXIT(ret);
      if (BFDTimeTest)    end_time_2 = sal_time_usecs(); /* measure the time took for 2 endpoint creations */

        if (endpoint_id==1) {
            sal_usleep(test_bfd_number_states * 50000); /* Wait 50mS for first EPs to initialize */
        }
      if (BFDTimeTest) {
         time_acc += (end_time_2 - start_time);
         time_no  += 2;
         cli_out("No %d, Endpoint create took %d and %d, Total %d, Average %d \n",time_no, (end_time_2 - end_time_1), (end_time_1 - start_time), time_acc ,(time_acc/time_no)); 
      } else if (!BFDTimeoutEventsTest) {
         if (numStates) {
            if (thread_test) {
               LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "\n State Change Test")));
               LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "\n**************** Pair N[%d] ****************\n"),ep_i + 1));
               ret = test_bfd_changeState(unit, desc_ep_1);
               BCMDNX_IF_ERR_EXIT(ret);
            }
            LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "\n Multi Threading Test")));
            LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "\n**************** Pair N[%d] ****************\n"),ep_i + 1));
            ret = test_bfd_Multithreading(unit, desc_ep_1);
            BCMDNX_IF_ERR_EXIT(ret);
         } else {
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_fixed_tx_rate_mgmt", 0) == 1) {
               ret = test_bfd_preDefinedUpdate(unit, desc_ep_1, desc_ep_2);
               BCMDNX_IF_ERR_EXIT(ret);
            } else {
               ret = test_bfd_updateEndpoint(unit, desc_ep_1, desc_ep_2);
               BCMDNX_IF_ERR_EXIT(ret);
            }
         }
      }

      endpoint_id += 2;
   }
   if (BFDTimeoutEventsTest) {
       uint32 reg;
       reg = 0;
       /* Wait for all endpoints to be up and running */
       sal_usleep(2*numEndpoints * 300000);

       /*Disable OAMP Transmission */
       ret = READ_OAMP_MODE_REGISTERr(unit, &reg);
       BCMDNX_IF_ERR_EXIT(ret);
       soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, TRANSMIT_ENABLEf, 0);
       ret = WRITE_OAMP_MODE_REGISTERr(unit,reg);
       BCMDNX_IF_ERR_EXIT(ret);
       /* Wait for all events to occur */
       sal_usleep(numEndpoints * 100000);

       if (test_bfd_time_out != (2*numEndpoints)) {
         cli_out("\n\nEndpoints %d timeout count %d does not match with expected count %d failed.\n\n", numEndpoints, test_bfd_time_out, 2*numEndpoints);
         ret = bcm_bfd_endpoint_destroy_all(unit);
         BCMDNX_IF_ERR_EXIT(ret);
         LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Destroy all Endpoints:\t\t[DONE]\n")));
         return CMD_FAIL;
       } else {
           cli_out("\n\nEndpoint %d timeout count %d match with expected count %d passed.\n\n", numEndpoints, test_bfd_time_out, 2*numEndpoints);
           /*Enable OAMP Transmission */
           ret = READ_OAMP_MODE_REGISTERr(unit, &reg);
           BCMDNX_IF_ERR_EXIT(ret);
           soc_reg_field_set(unit, OAMP_MODE_REGISTERr, &reg, TRANSMIT_ENABLEf, 1);
           ret = WRITE_OAMP_MODE_REGISTERr(unit,reg);
           BCMDNX_IF_ERR_EXIT(ret);
       }
   }
   if ((BFDTimeTest)&&(time_no > 0)) {
      if ((time_acc) > max_total_endpoint_create_time) {
         cli_out("\nEndpoint create takes too long. Max average expected %d, average received %d\n", max_total_endpoint_create_time / time_no, (time_acc / time_no));
         ret = bcm_bfd_endpoint_destroy_all(unit);
         BCMDNX_IF_ERR_EXIT(ret);
         LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Destroy all Endpoints:\t\t[DONE]\n")));
         return CMD_FAIL;
      }
      cli_out("\n\nEndpoint create time test passed. Max average expected %d, average received %d\n\n", (max_total_endpoint_create_time / time_no), (time_acc / time_no));
   }

   /* Destroy BFD endpoints */
   ret = bcm_bfd_endpoint_destroy_all(unit);
   BCMDNX_IF_ERR_EXIT(ret);
   LOG_VERBOSE(BSL_LS_BCM_BFD, (BSL_META_U(unit, "Destroy all Endpoints:\t\t[DONE]\n")));

   /* Destroy L3 interfaces */
   ret = bcm_l3_intf_delete_all(unit);
   BCMDNX_IF_ERR_EXIT(ret);

exit:
   BCMDNX_FUNC_RETURN;
}
