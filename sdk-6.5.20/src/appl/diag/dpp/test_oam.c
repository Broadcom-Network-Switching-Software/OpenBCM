/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: test_oam.c
 */

/* Includes */
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <appl/diag/shell.h>
#include <soc/cmext.h>
#include <soc/drv.h>
#include <soc/portmod/portmod.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <sal/appl/sal.h>
#include <bcm/vlan.h>
#include <bcm/oam.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm/port.h>
#include <bcm/l2.h>
#include <soc/dpp/drv.h>
#include <appl/diag/dpp/test_oamp.h>

STATIC oamp_test_init_param_t oamp_test_p[OAMP_TEST_MAX_NUM_DEVICES];
static uint32 	*rmeps;
static uint32 	*lmeps;
static uint32 	*loss_entries;
static uint32 	*timeout_event_count;
static uint32 	*timein_event_count;
static uint32 	*timein_event_state;
int           	*gport_list;   /* should be numEndpoints/numLevels */
int   			number_of_gports;

/* Global Variables */
int test_oam_timeout_event_counts = 0;
int test_oam_timein_event_counts = 0;
int test_oam_rdi_event_counts = 0;
int starting_vlan = 100;
int num_levels;
/* Two copies: one per core */
int current_lm_counter_base[2]={0};
int current_counter_range_index[2]={0};
/* All ingress OAM counter engines should be configured with a
   lower counter engine ID than all egress counter engines IDs */
int next_available_ingress_counter_engine = 0; /* Assuming no other application is calling counter_config_set() */
int next_available_egress_counter_engine = 8; /* reserving first 6 engines for ingress oam counter engine*/
int current_counter_id=16;
int loss_id = 0;
int counter_engine_status[16] = {0};
int loss_index = 0;

/* Usage for tr 161/oam */
int oamp_usage_print(int unit) {
    char oamp_usage[] =
        "Usage (tr oamp):"
        "\n\tTestRun OAMP commands\n\t"
        "Usages:\n\t"
        "tr oamp <OPTION> <parameters> ..."
        "\nOPTION can be:"
        "\nOAM - \tRun OAM tests"
        "\n\tParameters:"
        "\n\t\tPort1=<port1> - port on which endpoints will be defined (default 13)."
        "\n\t\tPort2=<port2> - port on which endpoints will be defined (default 14)."
        "\n\t\tNumberEndpoints=<NumEndpoints> - Number pairs of endpoints to be created(default 1024)."
        "\n\t\tSeed=<seed number> - Seed of the test for reproduce (default 0).";
    char oamp_usage_2[] =
        "\n\tOptional parameters:"
        "\n\t\tNumberUpdate=<NumberUpdate> -The number of updates to random endpoints"
        "\n\t\ttestSLM=<0/1> - Set up SLM seassion per LIF"
        "\n\t\tuseMAID48=<0/1> - Create Group MIAD with 48B name(default 0)"
        "\nBFD - \tRun BFD tests"
        "\n\tParameters:"
        "\n\t\tPort1=<port1> - port on which endpoints will be defined (default 13)."
        "\n\t\tPort2=<port2> - port on which endpoints will be defined (default 14).";
    char oamp_usage_3[] =
        "\n\t\tNumberEndpoints=<NumEndpoints> - Number pairs of endpoints to be created(default 1024)."
        "\n\t\tSeed=<seed number> - Seed of the test for reproduce (default 0)."
        "\n\tOptional parameters:"
        "\n\t\tNumberStates=<NumberStates> - Run StateChange test with <NumberStates> changes of local_state"
        "\n\t\tTimeTest=1 - Run EP creation time test (default 0)"
        "\n\t\tTimeoutEventsTest=1 - Run EP timeout events verification test (default 0)"
        "\n";
/* ISO C89 compilers are required to support up to '509' characters length */
    cli_out("%s", oamp_usage);
    cli_out("%s\n", oamp_usage_2);
    cli_out("%s\n", oamp_usage_3);
    return CMD_OK;
}

/* The gport returns vlan_port_id_parameter. Each gport will be used for 8 Endpoints for Jericho and 2 Endpoints for Arad Plus */
int
test_oam_create_vlan_gport(int unit, int port, int vlan, int *vlan_port_id) {
    int ret;
    bcm_vlan_port_t vp;
    int gport;

    BCMDNX_INIT_FUNC_DEFS;
    bcm_vlan_port_t_init(&vp);
    BCM_GPORT_MODPORT_SET(gport, unit, port);
    vp.match_vlan = vlan;
    vp.egress_vlan = vlan;
    vp.port = gport;
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;

    ret = bcm_vlan_port_create(unit, &vp);
    BCMDNX_IF_ERR_EXIT(ret);

    *vlan_port_id = vp.vlan_port_id;

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Create Vlan:\t\t[DONE]\n")));
exit:
    BCMDNX_FUNC_RETURN;
}


int destroy_all_vlans(int unit) {
    int gport;
    int ret;

    BCMDNX_INIT_FUNC_DEFS;

    for (gport = 0; gport < number_of_gports; gport++) {
        ret = bcm_vlan_port_destroy(unit, gport_list[gport]);
        BCMDNX_IF_ERR_EXIT(ret);
    }

exit:
    BCMDNX_FUNC_RETURN;
}
int oam_configure_oamp_reply(int unit, int mep_id, int opcode) {

    int ret = 0;
    int trap_code;
    bcm_rx_trap_t trap_type;
    bcm_oam_action_type_t action_type;
    bcm_oam_endpoint_action_t action;

    BCMDNX_INIT_FUNC_DEFS;
    trap_type = bcmRxTrapOamEthAccelerated;
    action_type = bcmOAMActionUcFwd;

    /* Configure trap */
    ret = bcm_rx_trap_type_get(unit, 0, trap_type, &trap_code);
    BCMDNX_IF_ERR_EXIT(ret);

    bcm_oam_endpoint_action_t_init(&action);

    BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

    BCM_OAM_OPCODE_SET(action, opcode);
    BCM_OAM_ACTION_SET(action, action_type);

    BCM_OAM_ACTION_SET(action,bcmOAMActionSLMEnable);
    BCM_OAM_ACTION_SET(action,bcmOAMActionCountEnable);

    ret = bcm_oam_endpoint_action_set(unit, mep_id, &action);
    BCMDNX_IF_ERR_EXIT(ret);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Create local and remote endpoint pair */
int
test_oam_create_endpoint_with_rmep(int unit, endpoint_create_info_t *ep1, endpoint_create_info_t *ep2, int mep_index, int group,int set_slm) {
    int ret;
    bcm_oam_endpoint_info_t lmep = { 0 };
    bcm_oam_endpoint_info_t rmep = { 0 };
    int sampling_ratio_ind;
    bcm_oam_loss_t loss_obj;
    bcm_mac_t src_mac_mep_arad = { 0x00, 0xb0, 0xc7, 0x1f, 0x82, 0x00 };
    bcm_mac_t dst_mac_mep_arad = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x35 };
    bcm_mac_t mac_mep_jer = {0x00, 0x00, 0x00, 0x01, 0x02, 0xff };
    BCMDNX_INIT_FUNC_DEFS;

    sampling_ratio_ind = sal_rand() % 9;

    bcm_oam_endpoint_info_t_init(&lmep);
    bcm_oam_endpoint_info_t_init(&rmep);

    lmep.type = bcmOAMEndpointTypeEthernet;
    lmep.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
    lmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    lmep.group = group;
    lmep.name = ep1->name;
    lmep.level = ep1->level;
    lmep.vlan = ep1->vlan;
    lmep.lm_counter_base_id = ep1->counter_base_id;
    BCM_GPORT_VLAN_PORT_ID_SET(lmep.gport, ep1->vlan_port_id);
    BCM_GPORT_SYSTEM_PORT_ID_SET(lmep.tx_gport, ep1->port);
    lmep.outer_tpid = 0x8100;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        src_mac_mep_arad[5] = ep1->port & 0xFF;
        sal_memcpy(lmep.src_mac_address, src_mac_mep_arad, 6);
        sal_memcpy(lmep.dst_mac_address, dst_mac_mep_arad, 6);

    } else {
        sal_memcpy(lmep.src_mac_address, mac_mep_jer, 6);
        lmep.src_mac_address[5] = ep1->mac;
        sal_memcpy(lmep.dst_mac_address, mac_mep_jer, 6);
        lmep.dst_mac_address[5] = ep1->mac;
    }
    lmep.sampling_ratio = sampling_ratio_ind;

    /* Create MEP */
    ret = bcm_oam_endpoint_create(unit, &lmep);
    BCMDNX_IF_ERR_EXIT(ret);
    lmeps[mep_index] = lmep.id;
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Created Endpoint: LMEP1: %d\t\t[DONE]\n"), lmep.id));

    rmep.type = bcmOAMEndpointTypeEthernet;
    rmep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    if(ep2->is_rdi) {
        rmep.flags2 |= BCM_OAM_ENDPOINT2_RDI_ON_LOC | BCM_OAM_ENDPOINT2_RDI_CLEAR_ON_LOC_CLEAR;
    }
    rmep.id = lmep.id;
    rmep.local_id = lmep.id;
    rmep.ccm_period = 9; /* BCM_OAM_ENDPOINT_CCM_PERIOD_3MS < rmep.ccm_period  < BCM_OAM_ENDPOINT_CCM_PERIOD_10MS */
    rmep.name = ep2->name;
    rmep.loc_clear_threshold = 1;

    /* Create RMEP */
    ret = bcm_oam_endpoint_create(unit, &rmep);
    BCMDNX_IF_ERR_EXIT(ret);
    rmeps[mep_index] = rmep.id;
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Created Remote Endpoint: RMEP1: %d\t\t[DONE]\n"), rmep.id));

    
    /* Set up SLM entry if it's requaried*/
    if (SOC_IS_JERICHO(unit)) {
        /* set up only one pair of SLM per LIF */
        if (set_slm) {
            bcm_oam_loss_t_init(&loss_obj);
            loss_obj.id         = lmep.id;
            loss_obj.period     = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
            loss_obj.flags      = BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_STATISTICS_EXTENDED | BCM_OAM_LOSS_SLM;
            sal_memcpy(loss_obj.peer_da_mac_address,mac_mep_jer, 6);
            loss_obj.peer_da_mac_address[5] = ep2->mac;
            if (SOC_IS_QAX(unit)) {
                loss_obj.loss_id = loss_id;
                loss_obj.flags |= BCM_OAM_LOSS_WITH_ID;
                loss_id++;
            }
            /* Create SLM*/
            ret = bcm_oam_loss_add(unit, &loss_obj);
            BCMDNX_IF_ERR_EXIT(ret);
            LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Add SLM Entry: %d to Endpoint: %d\t\t[DONE]\n"), loss_obj.id,lmep.id));

            /* Config SLRs */
            ret = oam_configure_oamp_reply(unit,lmep.id,55);
            BCMDNX_IF_ERR_EXIT(ret);
            /* Store ID */
            loss_entries[loss_index] = lmep.id;
            loss_index++;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/* Calculates the index of specific MEP in the
   event array */
int get_event_index(int unit, int16 name) {
    int   vlan, level;
    int   mep_index;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        vlan    = (name & 0xfffffffc) >> 2;
        level   = ((name & 0x2) >> 1) + 1;
    } else {
        vlan    = (name & 0xfffffff0) >> 4;
        level   = (name & 0xe) >> 1;
    }
    mep_index = 2 * (((vlan - starting_vlan) * num_levels) + level) + (name & 0x1);
    return mep_index;
}

/* Validate loss statistics per pair of MEPs */
int
test_oam_loss_verify(int unit, int index) {

    bcm_error_t ret;
    bcm_oam_loss_t loss_mep_1;
    bcm_oam_loss_t loss_mep_2;
    int loss_to_verify;
    int loss_to_verify_2;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        cli_out("SLM is not supported on devices below JERICHO");
        return 0;
    }

    bcm_oam_loss_t_init(&loss_mep_1);
    bcm_oam_loss_t_init(&loss_mep_2);

    loss_mep_1.id = loss_entries[index];
    loss_mep_2.id = loss_entries[index + 1];

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Get loss with ID: [%d]\n"),loss_mep_1.id));
    cli_out("Get loss with ID: [%d]\n",loss_mep_1.id);
    ret = bcm_oam_loss_get(unit, &loss_mep_1);
    BCMDNX_IF_ERR_EXIT(ret);

    cli_out("Get loss with ID: [%d]\n",loss_mep_2.id);
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Get loss with ID: [%d]\n"),loss_mep_2.id));
    ret = bcm_oam_loss_get(unit, &loss_mep_2);
    BCMDNX_IF_ERR_EXIT(ret);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Loss: Compare MEP(%d) with MEP(%d)\n"),loss_mep_1.id,loss_mep_2.id ));

    if(!loss_mep_1.tx_nearend || !loss_mep_2.tx_nearend ||
         !loss_mep_1.rx_nearend || !loss_mep_1.rx_nearend ||
         !loss_mep_1.tx_farend || !loss_mep_1.tx_farend ||
         !loss_mep_1.rx_farend || !loss_mep_1.rx_farend){
         cli_out("Statisticts are not updated!\n");
         BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
         }
         
    /* Validate tx_nearend */
    loss_to_verify = loss_mep_1.tx_nearend - loss_mep_2.tx_nearend; /* Since we don't know which value is bigger, verify both cases*/
    loss_to_verify_2 = loss_mep_2.tx_nearend - loss_mep_1.tx_nearend;
    if (loss_to_verify > 5 || loss_to_verify_2 > 5) {
            cli_out("Mismatch in Loss statistics: tx_nearend(ID:%d):%d ,tx_nearend(ID:%d):%d\n",
                            loss_mep_1.id,loss_mep_1.tx_nearend,loss_mep_2.id,loss_mep_2.tx_nearend);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    }
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "MEP(%d)tx_nearend:%d \t MEP(%d)tx_nearend:%d\n"),
                                    loss_mep_1.id,loss_mep_1.tx_nearend,loss_mep_2.id,loss_mep_2.tx_nearend));

    /* Validate rx_nearend */
    loss_to_verify = loss_mep_2.rx_nearend - loss_mep_1.rx_nearend;/* Since we don't know which value is bigger, verify both cases*/
    loss_to_verify_2 = loss_mep_2.rx_nearend - loss_mep_1.rx_nearend;
    if (loss_to_verify > 5 || loss_to_verify_2 > 5) {
            cli_out("Mismatch in Loss statistics: rx_nearend(ID:%d):%d ,rx_nearend(ID:%d):%d\n",
                            loss_mep_1.id,loss_mep_1.rx_nearend,loss_mep_2.id,loss_mep_2.rx_nearend);
            
    }
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "MEP(%d)rx_nearend:%d \t MEP(%d)rx_nearend:%d\n"),
                                    loss_mep_1.id,loss_mep_1.rx_nearend,loss_mep_2.id,loss_mep_2.rx_nearend));

    /* Validate tx_farend */
    loss_to_verify = loss_mep_2.tx_farend - loss_mep_1.tx_farend;/* Since we don't know which value is bigger, verify both cases*/
    loss_to_verify_2 = loss_mep_2.tx_farend - loss_mep_1.tx_farend;
    if (loss_to_verify > 5 || loss_to_verify_2 > 5) {
            cli_out("Mismatch in Loss statistics: tx_farend(ID:%d):%d ,tx_farend(ID:%d):%d\n",
                            loss_mep_1.id,loss_mep_1.tx_farend,loss_mep_2.id,loss_mep_2.tx_farend);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    }
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "MEP(%d)tx_farend:%d \t MEP(%d)tx_farend:%d\n"),
                                    loss_mep_1.id,loss_mep_1.tx_farend,loss_mep_2.id,loss_mep_2.tx_farend));

    /* Validate rx_farend */
    loss_to_verify = loss_mep_1.rx_farend - loss_mep_2.rx_farend;/* Since we don't know which value is bigger, verify both cases*/
    loss_to_verify_2 = loss_mep_2.rx_farend - loss_mep_1.rx_farend;
    if (loss_to_verify > 5 || loss_to_verify_2 > 5) {
            cli_out("Mismatch in Loss statistics: MEP(%d)rx_farend:%d \t MEP(%d)rx_farend:%d\n",
                            loss_mep_1.id,loss_mep_1.rx_farend,loss_mep_2.id,loss_mep_2.rx_farend);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    }
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "MEP(%d)rx_farend:%d \t MEP(%d)rx_farend:%d\n"), loss_mep_1.id,loss_mep_1.rx_farend,loss_mep_2.id,loss_mep_2.rx_farend));

exit:
    BCMDNX_FUNC_RETURN;

}

/* Function for period update of LMEP */
int
test_oam_updateLMEP(int unit, int index, int *event_index) {

    int ret;
    int lmep_id;
    bcm_oam_endpoint_info_t lmep;
    BCMDNX_INIT_FUNC_DEFS;
    bcm_oam_endpoint_info_t_init(&lmep);

    lmep_id = lmeps[index];

    ret = bcm_oam_endpoint_get(unit, lmep_id, &lmep);
    BCMDNX_IF_ERR_EXIT(ret);
    *event_index = get_event_index(unit, lmep.name);

    lmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    lmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    lmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    ret = bcm_oam_endpoint_create(unit, &lmep);
    BCMDNX_IF_ERR_EXIT(ret);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Update period of Endpoint [%d] to %d\n"),lmep_id,lmep.ccm_period));

    /* update mep configuration of the remote MEP */
    lmep_id = lmeps[index + 1];

    ret = bcm_oam_endpoint_get(unit, lmep_id, &lmep);
    BCMDNX_IF_ERR_EXIT(ret);

    lmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    lmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    lmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    ret = bcm_oam_endpoint_create(unit, &lmep);
    BCMDNX_IF_ERR_EXIT(ret);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Update period of Endpoint [%d] to %d\n"),lmep_id,lmep.ccm_period));

exit:
    BCMDNX_FUNC_RETURN;

}

/* Function for period update of RMEP */
int
test_oam_updateRMEP(int unit, int index) {

    int ret;
    int rmep_id;
    bcm_oam_endpoint_info_t rmep;
    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_endpoint_info_t_init(&rmep);

    rmep_id = rmeps[index];

    ret = bcm_oam_endpoint_get(unit, rmeps[index], &rmep);
    BCMDNX_IF_ERR_EXIT(ret);

    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS + 3;
    rmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    rmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    ret = bcm_oam_endpoint_create(unit, &rmep);
    BCMDNX_IF_ERR_EXIT(ret);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Update period of Endpoint [%d] to %d\n"),rmep_id ,rmep.ccm_period));

    rmep_id = rmeps[index + 1];

    ret = bcm_oam_endpoint_get(unit, rmeps[index + 1], &rmep);
    BCMDNX_IF_ERR_EXIT(ret);

    rmep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS + 3;
    rmep.flags |= BCM_OAM_ENDPOINT_REPLACE;
    rmep.flags |= BCM_OAM_ENDPOINT_WITH_ID;

    ret = bcm_oam_endpoint_create(unit, &rmep);
    BCMDNX_IF_ERR_EXIT(ret);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Update period of Endpoint [%d] to %d\n"),rmep_id ,rmep.ccm_period));

exit:
    BCMDNX_FUNC_RETURN;

}

/* Function for name update of MAIDs */
int
test_oam_updateGroup(int unit, int index, int index2) {

    int ret;
    bcm_oam_group_info_t rename_group_info;
    uint8 group_new_tx_MAID[BCM_OAM_GROUP_NAME_LENGTH] = { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
                                                       0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
                                                       0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
                                                       0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
                                                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                                                       0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF };
    uint8 group_new_rx_MAID[BCM_OAM_GROUP_NAME_LENGTH] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                                                       0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
                                                       0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
                                                       0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
                                                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                                                       0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF };
    BCMDNX_INIT_FUNC_DEFS;

    /* Update 1st group */
    bcm_oam_group_info_t_init(&rename_group_info);
    ret = bcm_oam_group_get(unit,index,&rename_group_info);
    BCMDNX_IF_ERR_EXIT(ret);

    rename_group_info.flags = BCM_OAM_GROUP_RX_NAME | BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE | BCM_OAM_GROUP_WITH_ID | BCM_OAM_GROUP_REPLACE;
    rename_group_info.id = index;
    sal_memcpy(rename_group_info.name, group_new_tx_MAID, BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(rename_group_info.rx_name, group_new_rx_MAID, BCM_OAM_GROUP_NAME_LENGTH);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Rename group with id=%d\n"),index));
    ret = bcm_oam_group_create(unit, &rename_group_info);
    BCMDNX_IF_ERR_EXIT(ret);

    /* Update 2nd group */
    bcm_oam_group_info_t_init(&rename_group_info);
    ret = bcm_oam_group_get(unit,index2,&rename_group_info);
    BCMDNX_IF_ERR_EXIT(ret);

    rename_group_info.flags = BCM_OAM_GROUP_RX_NAME | BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE | BCM_OAM_GROUP_WITH_ID | BCM_OAM_GROUP_REPLACE;
    rename_group_info.id = index2;
    sal_memcpy(rename_group_info.name, group_new_rx_MAID, BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(rename_group_info.rx_name, group_new_tx_MAID, BCM_OAM_GROUP_NAME_LENGTH);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Rename group with id=%d\n"),index));
    ret = bcm_oam_group_create(unit, &rename_group_info);
    BCMDNX_IF_ERR_EXIT(ret);
exit:
    BCMDNX_FUNC_RETURN;

}

/* Set CallBacks */
int
test_oam_cb(int unit, uint32 flags, bcm_oam_event_type_t event_type, bcm_oam_group_t group, bcm_oam_endpoint_t endpoint, void *user_data) {
    bcm_oam_endpoint_info_t lmep;
    int   ret;
    int   mep_index;

    BCMDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Flags %d, Event type %d Group %d,Endpoint 0x%x .\n"),flags, event_type, group, endpoint));

    ret = bcm_oam_endpoint_get(unit, endpoint, &lmep);
    BCMDNX_IF_ERR_EXIT(ret);

    mep_index = get_event_index(unit, lmep.name);

    if (event_type == bcmOAMEventEndpointCCMTimeout) {
        ++timeout_event_count[mep_index];
        ++test_oam_timeout_event_counts;
        if (timein_event_state[mep_index] == 1) {
            cli_out("TimeOut Error, mep_index: %d\n", mep_index);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
        }
        timein_event_state[mep_index] = 1;
    } else  if (event_type == bcmOAMEventEndpointCCMTimein) {
        test_oam_timein_event_counts++;
        timein_event_count[mep_index]++;
        if (timein_event_state[mep_index] == 0) {
            cli_out("TimeIn Error, mep_index: %d\n", mep_index);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
        }
        timein_event_state[mep_index] = 0;
    } else if ((event_type == bcmOAMEventEndpointRemote) ||
               (event_type == bcmOAMEventEndpointRemoteUp)) {
        test_oam_rdi_event_counts++;
        cli_out("RDI, mep_index: %d\n", mep_index);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* Initialize OAM and Create OAM group with 48 maid, aplicable only for QAX*/
int
test_oam_init_and_surroundings_48maid(int unit, int is_maid48, int *group_id1, int *group_id2) {
    int ret;
    bcm_oam_group_info_t group_info_1;
    bcm_oam_group_info_t group_info_2;
    uint8 group_Tx_MAID[BCM_OAM_GROUP_NAME_LENGTH] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
                                                       0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
                                                       0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
                                                       0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
                                                       0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
                                                       0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF };
    uint8 group_Rx_MAID[BCM_OAM_GROUP_NAME_LENGTH] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
                                                       0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
                                                       0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
                                                       0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
                                                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                                                       0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF };

    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_group_info_t_init(&group_info_1);
    bcm_oam_group_info_t_init(&group_info_2);

    group_info_1.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE | BCM_OAM_GROUP_RX_NAME;
    sal_memcpy(group_info_1.name, group_Tx_MAID, BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(group_info_1.rx_name, group_Rx_MAID, BCM_OAM_GROUP_NAME_LENGTH);
    group_info_1.group_name_index = SOC_IS_QUX(unit) ?  0x280 : 0x2080;
    ret = bcm_oam_group_create(unit, &group_info_1);
    BCMDNX_IF_ERR_EXIT(ret);
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Create OAM group:\t[DONE]\n")));
    *group_id1 = group_info_1.id;

    group_info_2.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE | BCM_OAM_GROUP_RX_NAME;
    sal_memcpy(group_info_2.name, group_Rx_MAID, BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(group_info_2.rx_name, group_Tx_MAID, BCM_OAM_GROUP_NAME_LENGTH);
    group_info_2.group_name_index = SOC_IS_QUX(unit) ?  0x260 : 0x2060;
    ret = bcm_oam_group_create(unit, &group_info_2);
    BCMDNX_IF_ERR_EXIT(ret);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Create OAM group:\t[DONE]\n")));
    *group_id2 = group_info_2.id;
   
exit:
    BCMDNX_FUNC_RETURN;
}

/* Initialize OAM and Create OAM group */
int
test_oam_init_and_surroundings(int unit, int *group_id) {
    int ret;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = { 1, 3, 2, 0xab, 0xcd };
    bcm_oam_group_info_t group_info;
    BCMDNX_INIT_FUNC_DEFS;

    bcm_oam_group_info_t_init(&group_info);
    sal_memcpy(group_info.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);

    ret = bcm_oam_group_create(unit, &group_info);
    BCMDNX_IF_ERR_EXIT(ret);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Create OAM group:\t[DONE]\n")));

    *group_id = group_info.id;

exit:
  BCMDNX_FUNC_RETURN;
}

/* Events registration */
int
test_oam_register_events(int unit) {
    int ret;
    bcm_oam_event_types_t timeout_event, timein_event;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timeout_event);
    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timein_event);

    BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
    ret = bcm_oam_event_register(unit, timeout_event, test_oam_cb, (void *)1);
    BCMDNX_IF_ERR_EXIT(ret);

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
    ret = bcm_oam_event_register(unit, timein_event, test_oam_cb, (void *)2);
    BCMDNX_IF_ERR_EXIT(ret);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Events registration */
int
test_oam_register_events_rdi(int unit) {
    int ret;
    bcm_oam_event_types_t timeout_event, timein_event, rdi_event;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timeout_event);
    BCM_OAM_EVENT_TYPE_CLEAR_ALL(timein_event);
    BCM_OAM_EVENT_TYPE_CLEAR_ALL(rdi_event);

    BCM_OAM_EVENT_TYPE_SET(timeout_event, bcmOAMEventEndpointCCMTimeout);
    ret = bcm_oam_event_register(unit, timeout_event, test_oam_cb, (void *)1);
    BCMDNX_IF_ERR_EXIT(ret);

    BCM_OAM_EVENT_TYPE_SET(timein_event, bcmOAMEventEndpointCCMTimein);
    ret = bcm_oam_event_register(unit, timein_event, test_oam_cb, (void *)2);
    BCMDNX_IF_ERR_EXIT(ret);
    
    BCM_OAM_EVENT_TYPE_SET(rdi_event, bcmOAMEventEndpointRemote);
    BCM_OAM_EVENT_TYPE_SET(rdi_event, bcmOAMEventEndpointRemoteUp);
    ret = bcm_oam_event_register(unit, timein_event, test_oam_cb, (void *)3);
    BCMDNX_IF_ERR_EXIT(ret);
exit:
    BCMDNX_FUNC_RETURN;
}

int _vlan_level__to_name(int unit, int vlan, int level, int parity) {
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        return (vlan << 2) + ((level - 1) << 1) + parity;
    } else {
        return (vlan << 4) + (level << 1) + parity;
    }
}


/* Switches the MEP_id in the given index, in the lmeps and rmeps arrays,
   with the last MEP pair */
void switch_endpoints(int index, int length) {
    int temp;

    temp = lmeps[index];
    lmeps[index] = lmeps[length - 2];
    lmeps[length - 2] = temp;

    temp = lmeps[index + 1];
    lmeps[index + 1] = lmeps[length - 1];
    lmeps[length - 1] = temp;

    temp = rmeps[index];
    rmeps[index] = rmeps[length - 2];
    rmeps[length - 2] = temp;

    temp = rmeps[index + 1];
    rmeps[index + 1] = rmeps[length - 1];
    rmeps[length - 1] = temp;

}

/* 
 * Test driver 
 * The test flow will be as follows:
  1. Set a loopback connection to port_1 and port_2.
  2. Initialize OAM
  3. Create OAM group and VLANs
  4. Register events
  5. Create endpoints with RMEP
  6. Update LMEP and RMEP
  7. Destroy all endpoints
*/

int
test_run_oam_rdi(int unit, int port_1, int port_2, int numEndpoints, int seed) {

    int ret;
    int group_id1 = 0;
    int group_id2 = 0;
    int endpoint = 0;
    int name1;
    int name2;
    int level;
    int starting_level;
    int vlan;
    int vlan_port_id1 = 0;
    int vlan_port_id2 = 0;
    uint32   start_time = 0;
    uint32   end_time_2 = 0;
    int      MEPnum;
    uint32   time_acc, time_no;
    endpoint_create_info_t ep1={0};
    endpoint_create_info_t ep2={0};
    int   i;
    uint32     end_time_1 = 0;

    BCMDNX_INIT_FUNC_DEFS;

    sal_srand(seed);

    timein_event_count   = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */
    timeout_event_count  = sal_alloc((numEndpoints + 1) * 2 * 4, "timeout_event_count"); /* Length = 4words * 2 * numEndpoints */
    rmeps                = sal_alloc((numEndpoints + 1) * 2 * 4, "rmeps_db"); /* Length = 4words * 2 * numEndpoints */
    lmeps                = sal_alloc((numEndpoints + 1) * 2 * 4, "lmeps_db"); /* Length = 4words * 2 * numEndpoints */
    gport_list           = sal_alloc((numEndpoints + 1) * 2 * 4, "gport_list_db"); /* Length = 4words * 2 * numEndpoints */
    timein_event_state   = sal_alloc((numEndpoints + 1) * 2 * 4, "lmeps_count"); /* Length = 4words * 2 * numEndpoints */

    for (i = 0; i < numEndpoints * 2; ++i) {
        timein_event_state[i] = 0;
    }

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        cli_out("\nTest is not supported for Arad.\n");
        BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }

    ret = bcm_port_loopback_set(unit, port_1, BCM_PORT_LOOPBACK_PHY);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_port_loopback_set(unit, port_2, BCM_PORT_LOOPBACK_PHY);
    BCMDNX_IF_ERR_EXIT(ret);
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "LoopBack set:\t\t[DONE]\n")));

    ret = bcm_port_class_set(unit, port_1, bcmPortClassId, port_1);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_port_class_set(unit, port_2, bcmPortClassId, port_2);
    BCMDNX_IF_ERR_EXIT(ret);

    ret = test_oam_init_and_surroundings(unit, &group_id1);  /* returns Group ID */
    group_id2 = group_id1;

    BCMDNX_IF_ERR_EXIT(ret);

    ret = test_oam_register_events_rdi(unit);
    BCMDNX_IF_ERR_EXIT(ret);
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Register Events:\t\t[DONE]\n")));

    /* 
    For Arad Plus starting level should begin from 1 and we work with 2 levels only.
    For Jericho starting level should begin from 0 and we work with 8 levels.
    */
    MEPnum = 2 * numEndpoints;
    if (MEPnum > 2048) { /* MEP index array limit*/
        /* Number of endpoint pairs is limited by the throughput of the ports */
        cli_out("\nMax number of MEP pairs is 1024. Aborting test.\n");
        BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        num_levels                = 2;
        starting_level            = 1;
    } else {
        num_levels                = 8;
        starting_level            = 0;
    }

    number_of_gports              = 0;
    test_oam_rdi_event_counts     = 0;
    time_no                       = 0;
    time_acc                      = 0;
    /* Create endpoints */
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Creating %d LMEP pairs \n"), numEndpoints));
    for (endpoint = 0; endpoint < numEndpoints; ++endpoint) {
        if ((endpoint % 100) == 0) {
            cli_out("Created Endpoint Pairs: %d\n", endpoint);
        }

        vlan = starting_vlan + endpoint / (num_levels);

        level = endpoint % num_levels;
        if (level == 0) {
            /* For every first endpoint per LIF, create VLAN gports */
            ret = test_oam_create_vlan_gport(unit, port_1, vlan, &vlan_port_id1);
            BCMDNX_IF_ERR_EXIT(ret);
            gport_list[number_of_gports] = vlan_port_id1;
            number_of_gports++;
            ret = test_oam_create_vlan_gport(unit, port_2, vlan, &vlan_port_id2);
            BCMDNX_IF_ERR_EXIT(ret);
            gport_list[number_of_gports] = vlan_port_id2;
            number_of_gports++;
            LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Create  Vlans:\t\t[DONE]\n")));
        }

        level = starting_level + level;
        name1 = _vlan_level__to_name(unit, vlan, level, 0);
        name2 = _vlan_level__to_name(unit, vlan, level, 1);

        ep1.name = name1;
        ep1.vlan_port_id = vlan_port_id2;
        ep1.vlan = vlan;
        ep1.level = level;
        ep1.port = port_1;
        ep2.name = name2;
        ep2.vlan_port_id = vlan_port_id1;
        ep2.vlan = vlan;
        ep2.port = port_2;
        ep2.level = level;
        ep2.is_rdi = 1;

        /* Create conected endpoints on port1 and port2 */
        start_time = sal_time_usecs();
        ret = test_oam_create_endpoint_with_rmep(unit, &ep1, &ep2, (2 * endpoint)/*MEP's index*/, group_id1, 0/* don't set slm*/); /* pair mep's index is sequental to lmep */
        BCMDNX_IF_ERR_EXIT(ret);
        end_time_1 = sal_time_usecs();
        BCMDNX_IF_ERR_EXIT(ret);

        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Creating endpoint1 with Level %d , vlan port id1 0x%x.\n"), level, vlan_port_id1));

        time_acc += (end_time_1 - start_time);
        time_no  += 1;

        cli_out("No %d, Endpoint create took %d, Total %d, Average %d \n",time_no, (end_time_1 - start_time), time_acc ,(time_acc/time_no)); 

    }

    number_of_gports = 0;

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Creating %d LMEP pairs \n"), numEndpoints));
    for (endpoint = 0; endpoint < numEndpoints; ++endpoint) {
        if ((endpoint % 100) == 0) {
            cli_out("Created Endpoint Pairs: %d\n", endpoint);
        }

        vlan = starting_vlan + endpoint / (num_levels);

        level = endpoint % num_levels;
        if (level == 0) {
            /* For every first endpoint per LIF, create VLAN gports */

            vlan_port_id1 = gport_list[number_of_gports];
            number_of_gports++;
            vlan_port_id2 = gport_list[number_of_gports];
            number_of_gports++;
            LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Create  Vlans:\t\t[DONE]\n")));
        }

        level = starting_level + level;
        name1 = _vlan_level__to_name(unit, vlan, level, 0);
        name2 = _vlan_level__to_name(unit, vlan, level, 1);

        ep1.name = name1;
        ep1.vlan_port_id = vlan_port_id2;
        ep1.vlan = vlan;
        ep1.level = level;
        ep1.port = port_1;
        ep1.is_rdi = 1;
        ep2.name = name2;
        ep2.vlan_port_id = vlan_port_id1;
        ep2.vlan = vlan;
        ep2.port = port_2;
        ep2.level = level;

        /* Create conected endpoints on port1 and port2 */
        start_time = sal_time_usecs();
        
        ret = test_oam_create_endpoint_with_rmep(unit, &ep2, &ep1, (2 * endpoint + 1)/*MEP's index*/, group_id2, 0/* don't set slm*/); /* pair mep's index is sequental to lmep */
        BCMDNX_IF_ERR_EXIT(ret);
        end_time_2 = sal_time_usecs(); /* measure the time took for 2 endpoint creations */

        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Creating endpoint2 with Level %d , vlan port id2 0x%x.\n"), level, vlan_port_id2));

        time_acc += (end_time_2 - start_time);
        time_no  += 1;
        cli_out("No %d, Endpoint create took %d, Total %d, Average %d \n",time_no, (end_time_2 - start_time), time_acc ,(time_acc/time_no)); 
           
    }
    
    cli_out("\n#####################################\n\tCREATED %d ENDPOINTS\n", endpoint);
    sal_sleep(3);    /* To test there are no interrupts */

    if (test_oam_rdi_event_counts >  0) {
        cli_out("\nUnexpected RDIs occured: number = %d\n", test_oam_rdi_event_counts);
        BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    } else {
        cli_out("\nNo unexpected RDI event occured.\n");
    }

    cli_out("\n#####################################\n\tRETURNING \n");

exit:
    ret = bcm_port_loopback_set(unit, port_1, 0);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_port_loopback_set(unit, port_2, 0);
    BCMDNX_IF_ERR_EXIT(ret);
    sal_sleep(1);  /* to avoid warnings during the MEPs' destroy */

    ret = bcm_oam_endpoint_destroy_all(unit, group_id1);
    BCMDNX_IF_ERR_EXIT(ret);

    ret = destroy_all_vlans(unit);
    BCMDNX_IF_ERR_EXIT(ret);

    sal_free(timein_event_count);
    sal_free(timeout_event_count);
    sal_free(rmeps);
    sal_free(lmeps);
    sal_free(gport_list);
    sal_free(timein_event_state);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Destroy all Endpoints:\t\t[DONE]\n")));

    BCMDNX_FUNC_RETURN;
}

int _get_core_and_tm_port_from_port(int unit, bcm_gport_t gport, int * core, int * tm_port)
{
    bcm_error_t rc=0;
    uint32 dummy_flags=0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
    BCMDNX_IF_ERR_EXIT(rc);

    *core = mapping_info.core;
    *tm_port = mapping_info.tm_port;

exit:
    BCMDNX_FUNC_RETURN;
}

int _set_counter_source_and_engines(int unit, int *counter_base, int port) {
   bcm_stat_counter_engine_t counter_engine;
   bcm_stat_counter_config_t counter_config;
   bcm_color_t colors[] = { bcmColorGreen, bcmColorYellow, bcmColorRed, bcmColorBlack };
   uint8    drop_fwd[] = { 0, 1 };
   int      index1=0, index2=0;
   int      ret=0;
   int      core, tm_port=0;
   uint32   counter_engine_flags = 0;
   int rv=0;
   BCMDNX_INIT_FUNC_DEFS;
   rv = _get_core_and_tm_port_from_port(unit, port, &core, &tm_port);
   BCMDNX_IF_ERR_EXIT(ret);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
       /* In Arad the counters have been statically allocated via soc properties. Furthurmore LM-PCP is not available.
          simply increment the counter and return.*/
       *counter_base = ++current_lm_counter_base[core];
       return 0;
    }

   if (SOC_IS_QAX(unit)) {
        /* QAX may recevive the counter from a 2nd source.
          For this two work multiple sources per counter engine should be enbaled. */
        current_counter_range_index[0] = current_counter_range_index[0] ? current_counter_range_index[0] : 1;
        current_counter_range_index[1] = current_counter_range_index[1] ? current_counter_range_index[1] : 1;
        counter_engine_flags = BCM_STAT_COUNTER_MULTIPLE_SOURCES_PER_ENGINE;
   }

   /* The counter processor can work in double mode. When working in double mode,
      each counter engine can support twice the number of counters. This is acheived by
      splitting each entry into two entries of 32b. This is the suggested mode for OAM
      as OAM messages has 32b fields for loss measaurment. Working in this mode is MANDATORY
      for OAM to prevent the accessing of the same engine from both ingress / egress
      at the same time (which can cause errors with counter stamping / increment). */

   /* Get number of counter engines and number of counter pointers per counter engine. */
   if (current_lm_counter_base[core] % 32768 == 0) {
       /* Ran out of available counters. Allocate another bunch. */
       /* Configuring Egress engine. */

       /* Setting format to double mode. */
       counter_config.format.format_type = bcmStatCounterFormatDoublePackets;
       /* Since we are indifferent to color/droped-forwarded we only need one counter set. */
       counter_config.format.counter_set_mapping.counter_set_size = 1;
       /* Using 8 entries for 8 PCP values. */
       counter_config.format.counter_set_mapping.num_of_mapping_entries = 8;
       for (index1 = 0; index1 < bcmColorPreserve; index1++) {
          for (index2 = 0; index2 < 2; index2++) {
             /* Counter configuration is independent on the color, drop precedence.*/
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].offset = 0; /* Must be zero since counter_set_size is 1 */
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.color = colors[index1];
             counter_config.format.counter_set_mapping.entry_mapping[index1 * 2 + index2].entry.is_forward_not_drop = drop_fwd[index2];
          }
       }
       counter_config.source.core_id = core;
       counter_config.source.command_id = 0;
       /* In double mode, two engines are required for the same range. One for egress and one for ingress.
          Setting the first counter base id accordingly. */
       counter_config.source.pointer_range.start = current_counter_range_index[core] * NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE;
       counter_config.source.pointer_range.end = counter_config.source.pointer_range.start + NUMBER_OF_COUNTERS_PER_COUNTER_SOURCE - 1;
       current_counter_range_index[core]++;

       /* Set lm_counter_base_id to the first one in the range. */
       current_lm_counter_base[core] = counter_config.source.pointer_range.start;
       
       /* Egress OAM counter engines should be configured with a
          higher counter engine id than all ingress counter engines IDs */
       counter_config.source.engine_source = bcmStatCounterSourceEgressOam;
       counter_engine.engine_id = next_available_egress_counter_engine++;
       counter_engine.flags = counter_engine_flags;

       rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
       BCM_IF_ERROR_RETURN(rv);
       cli_out("Engine id %d for egress , core %d configured with range 0x%05x - 0x%05x\n",counter_engine.engine_id,
              core, counter_config.source.pointer_range.start, counter_config.source.pointer_range.end);
       counter_engine_status[counter_engine.engine_id] = 1;

       /* Configuring Ingress engine (Same range). */

       counter_config.source.engine_source = bcmStatCounterSourceIngressOam;

       counter_engine.engine_id = next_available_ingress_counter_engine++;

       rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
       BCM_IF_ERROR_RETURN(rv);
       cli_out("Engine id %d for ingress, core %d configured with range 0x%05x - 0x%05x\n",counter_engine.engine_id,
              core, counter_config.source.pointer_range.start, counter_config.source.pointer_range.end);
       counter_engine_status[counter_engine.engine_id] = 1;
   }

   /* counter engines are set. Return a counter base. */
   /* Increment by 8 to allow PCP based LM if needed. */
   current_lm_counter_base[core] += 8;
   /* ID 0 cannot be used - the value 0 is reserved to signify disabling LM counters. */
   *counter_base = current_lm_counter_base[core];

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Test driver 
 * The test flow will be as follows:
  1. Set a loopback connection to port_1 and port_2.
  2. Initialize OAM
  3. Create OAM group and VLANs
  4. Register events
  5. Create endpoints with RMEP
  6. Update LMEP and RMEP
  7. Destroy all endpoints
*/
int
test_run_oam(int unit, int port_1, int port_2, int numEndpoints, int seed, int numUpdate, int testSLM, int is_maid48) {

    int ret = 0;
    int group_id1 = 0;
    int group_id2 = 0;
    int endpoint = 0;
    int name1;
    int name2;
    int level;
    int starting_level;
    int vlan;
    int vlan_port_id1 = 0;
    int vlan_port_id2 = 0;
    int counter_base1= 0;
    int counter_base2=0;
    int index = 0;
    int update;
    int set_slm=0;
    uint32   max_total_endpoint_create_time = 0;
    uint32   start_time = 0;
    uint32   end_time_2 = 0;
    int      MEPnum;
    uint32   time_acc, time_no;
    endpoint_create_info_t ep1={0};
    endpoint_create_info_t ep2={0};
    int   i;
    int   event_index;
    int time_slm = 0;
    int time_no_slm = 0;

    uint32     end_time_1 = 0;
    char *system_indicator_name = NULL;
    const char *system_name_64B = "64B";
    int is_system_64b = 0;

    BCMDNX_INIT_FUNC_DEFS;

    sal_srand(seed);

    system_indicator_name = soc_property_get_str(unit, "dvapi_system_indicator");
    if ((NULL != system_indicator_name)
        && (sal_strncmp(system_indicator_name, system_name_64B, sal_strlen(system_name_64B)) == 0))
    {
        is_system_64b = 1;
    }

    timein_event_count   = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */
    timeout_event_count  = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */
    rmeps                = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */
    lmeps                = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */
    loss_entries         = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */
    gport_list           = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */
    timein_event_state   = sal_alloc((numEndpoints + 1) * 2 * 4, "timein_event_count"); /* Length = 4words * 2 * numEndpoints */

    if (SOC_IS_QAX(unit)) { loss_id = 0x2000; }
    if (SOC_IS_QUX(unit)) { loss_id = 0x800; }

    for (i = 0; i < numEndpoints * 2; ++i) {
        timein_event_state[i] = 0;
    }

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        cli_out("\nTest is not supported for Arad.\n");
        BCMDNX_IF_ERR_EXIT(BCM_E_UNAVAIL);
    }
    
    if (SOC_IS_JERICHO(unit) && testSLM) {
        /* Add padd size to SLM packets*/
        ret = portmod_port_pad_size_set(unit,port_1,90);
        BCMDNX_IF_ERR_EXIT(ret);
        ret = portmod_port_pad_size_set(unit,port_2,90);
        BCMDNX_IF_ERR_EXIT(ret);
    }

    ret = bcm_port_loopback_set(unit, port_1, BCM_PORT_LOOPBACK_PHY);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_port_loopback_set(unit, port_2, BCM_PORT_LOOPBACK_PHY);
    BCMDNX_IF_ERR_EXIT(ret);
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "LoopBack set:\t\t[DONE]\n")));

    ret = bcm_port_class_set(unit, port_1, bcmPortClassId, port_1);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_port_class_set(unit, port_2, bcmPortClassId, port_2);
    BCMDNX_IF_ERR_EXIT(ret);

    if (is_maid48 && SOC_IS_QAX(unit)) {
        ret = test_oam_init_and_surroundings_48maid(unit, is_maid48, &group_id1, &group_id2);  /* returns Group ID */
    } else {
        ret = test_oam_init_and_surroundings(unit, &group_id1);  /* returns Group ID */
        group_id2 = group_id1;
    }
    BCMDNX_IF_ERR_EXIT(ret);

    ret = test_oam_register_events(unit);
    BCMDNX_IF_ERR_EXIT(ret);
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Register Events:\t\t[DONE]\n")));

    /* 
    For Arad Plus starting level should begin from 1 and we work with 2 levels only.
    For Jericho starting level should begin from 0 and we work with 8 levels.
    */
    MEPnum = 2 * numEndpoints;
    if (MEPnum > 2048) { /* MEP index array limit*/
        /* Number of endpoint pairs is limited by the throughput of the ports */
        cli_out("\nMax number of MEP pairs is 1024. Aborting test.\n");
        BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    }
    if (numUpdate > numEndpoints) {
        cli_out("\nIllegal number of Updates. Aborting test.\n");
        BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    }
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        num_levels                = 2;
        starting_level            = 1;
        /* First 2 EP create take 76000usec due to configuration of OAM1/2 entries.
           Following EP creates takes 3000usec. Every EP create after take 8usec
           longer (in average)
           Total EP Create Time = 76000 + 6500 * (n-2) + 10/2 * (n-2)^2
           Additional 10% added for up-limit */

        max_total_endpoint_create_time  = 1.1 * (76000 + 6500 * (MEPnum - 2) + 5 * (MEPnum - 2) * (MEPnum - 2));
    } else {
        num_levels                = 8;
        starting_level            = 0;
        /* First 2 EP create take 82000usec due to configuration of OAM1/2 entries.
           Following EP creates takes 3000usec. Every EP create after take 8usec
           longer (in average)
           Total EP Create Time = 82000 + 3000 * (n-2) + 8/2 * (n-2)^2
           Additional 10% added for up-limit */
        if (SOC_IS_QUX(unit)) {
            /* First 2 EP create take 125000usec due to configuration of OAM1/2 entries. Due to QUX DMA operation needs more time. */
            max_total_endpoint_create_time  = 1.1 * (125000 + 3000 * (MEPnum - 2) + 4 * (MEPnum - 2) * (MEPnum - 2)); /* Max time in usec */
        } else if (SOC_IS_JERICHO_PLUS(unit) && is_system_64b) {
            max_total_endpoint_create_time  = 1.1 * (120000 + 3000 * (MEPnum - 2) + 4 * (MEPnum - 2) * (MEPnum - 2)); /* Max time in usec */
        } else {
            max_total_endpoint_create_time  = 1.1 * (84000 + 3000 * (MEPnum - 2) + 4 * (MEPnum - 2) * (MEPnum - 2)); /* Max time in usec */
        }
    }

    number_of_gports              = 0;
    test_oam_timein_event_counts  = 0;
    test_oam_timeout_event_counts = 0;
    time_no                       = 0;
    time_acc                      = 0;
    /* Create endpoints */
    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Creating %d LMEP pairs \n"), numEndpoints));
    for (endpoint = 0; endpoint < numEndpoints; ++endpoint) {
        if ((endpoint % 100) == 0) {
            cli_out("Created Endpoint Pairs: %d\n", endpoint);
        }

        vlan = starting_vlan + endpoint / (num_levels);

        level = endpoint % num_levels;
        if (level == 0) {
            if (testSLM > 0) {
                set_slm = 1; /* Set up single SLM pair per LIF */
                ret = _set_counter_source_and_engines(unit, &counter_base1, port_1);
                BCMDNX_IF_ERR_EXIT(ret);
                ret = _set_counter_source_and_engines(unit, &counter_base2, port_2);
                BCMDNX_IF_ERR_EXIT(ret);
            }
            /* For every first endpoint per LIF, create VLAN gports */
            ret = test_oam_create_vlan_gport(unit, port_1, vlan, &vlan_port_id1);
            BCMDNX_IF_ERR_EXIT(ret);
            gport_list[number_of_gports] = vlan_port_id1;
            number_of_gports++;
            ret = test_oam_create_vlan_gport(unit, port_2, vlan, &vlan_port_id2);
            BCMDNX_IF_ERR_EXIT(ret);
            gport_list[number_of_gports] = vlan_port_id2;
            number_of_gports++;
            LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Create  Vlans:\t\t[DONE]\n")));
        }

        level = starting_level + level;
        name1 = _vlan_level__to_name(unit, vlan, level, 0);
        name2 = _vlan_level__to_name(unit, vlan, level, 1);

        ep1.name = name1;
        ep1.vlan_port_id = vlan_port_id2;
        ep1.vlan = vlan;
        ep1.level = level; /* Up to 8 different levels on JER and above, 2 on Arad+ */
        ep1.port = port_1;
        ep1.counter_base_id = counter_base1; /* Use different counter_base_id per LIF */
        ep1.mac = vlan; /* randomize the DA address per LIF */
        ep1.is_rdi = 0; /* Set a default value */
        
        ep2.name = name2;
        ep2.vlan_port_id = vlan_port_id1;
        ep2.vlan = vlan;
        ep2.port = port_2;
        ep2.level = level;
        ep2.counter_base_id = counter_base2;
        ep2.mac = vlan+1; /* randomize the DA address per LIF */
        ep2.is_rdi = 0; /* Set a default value */
        
        /* Create conected endpoints on port1 and port2 */
        start_time = sal_time_usecs();
        ret = test_oam_create_endpoint_with_rmep(unit, &ep1, &ep2, (2 * endpoint)/*MEP's index*/, group_id1, set_slm); /* pair mep's index is sequental to lmep */
        BCMDNX_IF_ERR_EXIT(ret);
        end_time_1 = sal_time_usecs();
        ret = test_oam_create_endpoint_with_rmep(unit, &ep2, &ep1, (2 * endpoint + 1)/*MEP's index*/, group_id2, set_slm); /* pair mep's index is sequental to lmep */
        BCMDNX_IF_ERR_EXIT(ret);
        end_time_2 = sal_time_usecs(); /* measure the time took for 2 endpoint creations */

        time_no_slm += (set_slm * 2);
        set_slm = 0;

        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Creating endpoint1 with Level %d , vlan port id1 0x%x.\n"), level, vlan_port_id1));
        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Creating endpoint2 with Level %d , vlan port id2 0x%x.\n"), level, vlan_port_id2));

        time_acc += (end_time_2 - start_time);
        time_no  += 2;
        time_slm = 140000;/*additional time needed for SLM in usec*/

        cli_out("No %d, Endpoint create took %d and %d, Total %d, Average %d \n",time_no, (end_time_2 - end_time_1), (end_time_1 - start_time), time_acc ,(time_acc/time_no)); 

        if (test_oam_timeout_event_counts >  0) {
            cli_out("\nUnexpected interrupts occured: %d\n", test_oam_timeout_event_counts);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
        }
    }

    cli_out("\n#####################################\n\tCREATED %d ENDPOINTS\n", endpoint);
    sal_sleep(3);    /* To test there are no interrupts */

    if (test_oam_timeout_event_counts >  0) {
        cli_out("\nUnexpected interrupts occured: %d\n", test_oam_timeout_event_counts);
        BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    } else {
        cli_out("\nNo unexpected interrupts occured.\n");
    }

    /* Endpoint create time test */
    if (time_no > 0) {
        max_total_endpoint_create_time  +=((2400 * time_no) + (testSLM * time_slm * time_no_slm)/*additional time in case of SLM entry*/);
        if (time_acc > max_total_endpoint_create_time) {
            cli_out("\nEndpoint create takes tooooooo long. Max average expected %d, average received %d\n", (max_total_endpoint_create_time / time_no), (time_acc / time_no));
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
        } else {
            cli_out("\n\nEndpoint create time test passed. Max average expected %d, average received %d\n\n", (max_total_endpoint_create_time / time_no), (time_acc / time_no));
        }
    }

    /* Check if the timeout_counts less than the number of endpoints.*/
    ret = bcm_port_loopback_set(unit, port_1, 0);
    BCMDNX_IF_ERR_EXIT(ret);
    if (!SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma) {
        sal_sleep(3);  /* delay needed to eliminate call-back function congestion */
    }
    ret = bcm_port_loopback_set(unit, port_2, 0);
    BCMDNX_IF_ERR_EXIT(ret);
    cli_out("\n#####################################\n\tTraffic stopped\n");
    sal_sleep(3); /* Let all MEPs get timeout event */

    /* Sleep to wait for the timeout*/
    if (test_oam_timeout_event_counts <  (endpoint * 2)) {
        cli_out("\nIncorrect number of timeouts. expected %d received %d\n", endpoint * 2, test_oam_timeout_event_counts);
        BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    } else {
        cli_out("\nNumber of timeouts. expected %d received %d\n", endpoint * 2, test_oam_timeout_event_counts);
    }

    /* Check if the timein_counts less than the number of endpoints. */
    test_oam_timein_event_counts = 0;
    if (is_maid48 && SOC_IS_QAX(unit)) {
        /* Update the RX and TX MAID names*/
        ret = test_oam_updateGroup(unit,group_id1,group_id2);
        BCMDNX_IF_ERR_EXIT(ret);
    }

    ret = bcm_port_loopback_set(unit, port_1, BCM_PORT_LOOPBACK_PHY);
    BCMDNX_IF_ERR_EXIT(ret);
    if (!SOC_DPP_CONFIG(unit)->pp.oam_use_event_fifo_dma) {
        sal_sleep(3);  /* delay needed to eliminate call-back function congestion */
    }
    ret = bcm_port_loopback_set(unit, port_2, BCM_PORT_LOOPBACK_PHY);
    BCMDNX_IF_ERR_EXIT(ret);
    cli_out("\n#####################################\n\tTraffic started\n");
    sal_sleep(3); /* Let all MEPs get timein events */

    /* Sleep to wait for the timein */
    if (test_oam_timein_event_counts < (endpoint * 2)) {
        cli_out("\nIncorrect number of timeins. expected %d received %d.\n", endpoint * 2, test_oam_timein_event_counts);
        BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
    } else {
        cli_out("\nNumber of timeins. expected %d received %d\n", endpoint * 2, test_oam_timein_event_counts);
    }

    /* CCM Period update test 
    Here we update the ccm period of numUpdate random endpoint pairs, leaving the rmep period the same.
    We expect timeout event on the updated MEPs since they receive CCM packets slower than the required by rmep_period.
    Once in ccm_period time they will get timein again because of the incoming CCM and then timeout again.
    We check that at least 2 timeout events have been accepted and all of the events were received from the updated MEPs.       .
    On phase 2, we update the rmep_period of the chosen MEP pair and expect no timeout events. */
    if (numUpdate > 0) {
        cli_out("\n#####################################\n\tCCM Period update test started\n");
    }

    for (update = 1; update <= numUpdate; ++update) {

        if (update % 100 == 0) {
            cli_out("\nUpdate test runnig... %d\n", update);
        }
        test_oam_timein_event_counts = 0;
        test_oam_timeout_event_counts = 0;
        for (i = 0; i < (2 * numEndpoints); i++) {
            timeout_event_count[i] = 0;
            timein_event_count[i] = 0;
        }

        index = sal_rand() % (numEndpoints - update + 1); /* Choose one MEP pair out of the MEP that were not chosen yet. */
        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "#####################################\n\tUpdate %d, LMEP %d\n"), update, index));
        ret = test_oam_updateLMEP(unit, 2 * index, &event_index);
        BCMDNX_IF_ERR_EXIT(ret);

        sal_msleep(100); /* Sleep to wait for the timeouts, and make sure all of the time outs were received from the chosen MEPs */
        if ((test_oam_timeout_event_counts < 2) ||
            (test_oam_timeout_event_counts > (timeout_event_count[event_index] + timeout_event_count[event_index + 1]))
           ) {
            cli_out("\nIncorrect number of timeouts. expected %d received %d, index: %dMEPevents: %d,%d\n", 2, test_oam_timeout_event_counts, index, timeout_event_count[2 * index], timeout_event_count[2 * index + 1]);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
        }

        LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "#####################################\n\tUpdate %d, RMEP %d\n"), update, index));
        ret = test_oam_updateRMEP(unit, 2 * index);
        BCMDNX_IF_ERR_EXIT(ret);
        sal_msleep(100); /* Sleep to let time for the timeout event */

        test_oam_timeout_event_counts = 0;

        sal_msleep(100); /* Sleep to let time for the timeout event */
        if (test_oam_timeout_event_counts > 0) {
            cli_out("\nIncorrect number of timeouts. Expected %d received %d\n", 0, test_oam_timeout_event_counts);
            BCMDNX_IF_ERR_EXIT(BCM_E_FAIL);
        }

        switch_endpoints(2 * index, (2 * (numEndpoints - update + 1)));   /* take the chosen MEPs out of the list for the next iteration by
                                                                             switching them with the last pair of the array. */
    }
    if (numUpdate > 0) {
        cli_out("\n\nEndpoint update test passed. \n\n");
    }

    if (testSLM>0) {
        /* Stop traffic for more accurate results*/

        cli_out("\n#####################################\n\tLOSS TEST STARTED\n");
        sal_msleep(3000);

        /* Perfom loss validation, we have 1 pair of loss entries per LIF */
        for (i=0; i < (loss_index / 2) ;i++) {
            ret = test_oam_loss_verify(unit,(2 * i));
            BCMDNX_IF_ERR_EXIT(ret);
        }

        cli_out("\n\nLoss test passed. \n\n");
    }

exit:

    ret = bcm_port_loopback_set(unit, port_1, 0);
    BCMDNX_IF_ERR_EXIT(ret);
    ret = bcm_port_loopback_set(unit, port_2, 0);
    BCMDNX_IF_ERR_EXIT(ret);
    sal_sleep(1);  /* to avoid warnings during the MEPs' destroy */

    ret = bcm_oam_endpoint_destroy_all(unit, group_id1);
    BCMDNX_IF_ERR_EXIT(ret);
    if (is_maid48 && SOC_IS_QAX(unit)) {
        ret = bcm_oam_endpoint_destroy_all(unit, group_id2);
        BCMDNX_IF_ERR_EXIT(ret);
    }

    ret = destroy_all_vlans(unit);
    BCMDNX_IF_ERR_EXIT(ret);

    sal_free(timein_event_count);
    sal_free(timeout_event_count);
    sal_free(rmeps);
    sal_free(lmeps);
    sal_free(loss_entries);
    sal_free(gport_list);
    sal_free(timein_event_state);

    LOG_VERBOSE(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Destroy all Endpoints:\t\t[DONE]\n")));

    BCMDNX_FUNC_RETURN;
}

int
oamp_test(int unit, args_t *a, void *p) {

    oamp_test_init_param_t *init_param = p;

    if (!init_param->option) {
        return oamp_usage_print(unit);
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    } else if (!sal_strcasecmp(init_param->option, "BFD")) {
        return test_run_bfd(unit, init_param->port1, init_param->port2, init_param->numEndpoints, init_param->seed, init_param->numStates, init_param->EndpointCreationTimeTest, init_param->timeoutEventsTest);
    } else if (!sal_strncasecmp(init_param->option, "OAM", strlen("OAM"))) {
        
        if(!sal_strcasecmp(init_param->option, "OAM_RDI")){
            return test_run_oam_rdi(unit, init_param->port1, init_param->port2, init_param->numEndpoints, init_param->seed);
        } else {
            return test_run_oam(unit, init_param->port1, init_param->port2, init_param->numEndpoints, init_param->seed, init_param->numUpdate, init_param->testSLM, init_param->is_Maid48);
        }
    } else {
        return oamp_usage_print(unit);
    }

}

int
oamp_usage_parse(int unit, args_t *a, oamp_test_init_param_t *init_param) {

    int rv = CMD_OK;

/*
 * Setting default values
*/
    parse_table_t    pt;
    init_param->port1 = OAMP_TEST_DEF_PORT_1;
    init_param->port2 = OAMP_TEST_DEF_PORT_2;
    init_param->numEndpoints = OAMP_TEST_DEF_NUM_EP_PAIRS;
    init_param->seed = OAMP_TEST_DEF_SEED;
    init_param->numStates = OAMP_TEST_DEF_NUM_STATES;
    init_param->numUpdate = OAMP_TEST_DEF_NUM_UPDATE;
    init_param->testSLM = OAMP_TEST_DEF_NORMAL;
    init_param->EndpointCreationTimeTest = OAMP_TEST_DEF_NORMAL;
    init_param->timeoutEventsTest = OAMP_TEST_DEF_NORMAL;
    init_param->is_Maid48 = OAMP_TEST_DEF_SHORT_MAID;
/*
 * Parsing the arguments
*/
    init_param->option = ARG_GET(a);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Port1", PQ_DFL | PQ_INT, (void *)0, &(init_param->port1), NULL);
    parse_table_add(&pt, "Port2", PQ_DFL | PQ_INT, (void *)0, &(init_param->port2), NULL);
    parse_table_add(&pt, "NumberEndpoints", PQ_DFL | PQ_INT, (void *)0, &(init_param->numEndpoints), NULL);
    parse_table_add(&pt, "Seed", PQ_DFL | PQ_INT, (void *)0, &(init_param->seed), NULL);
    parse_table_add(&pt, "NumberStates", PQ_DFL | PQ_INT, (void *)0, &(init_param->numStates), NULL);
    parse_table_add(&pt, "NumberUpdate", PQ_DFL | PQ_INT, (void *)0, &(init_param->numUpdate), NULL);
    parse_table_add(&pt, "testSLM", PQ_DFL | PQ_INT, (void *)0, &(init_param->testSLM), NULL);
    parse_table_add(&pt, "TimeTest", PQ_DFL | PQ_INT, (void *)0, &(init_param->EndpointCreationTimeTest), NULL);
    parse_table_add(&pt, "TimeoutEventsTest", PQ_DFL | PQ_INT, (void *)0, &(init_param->timeoutEventsTest), NULL);
    parse_table_add(&pt, "useMAID48", PQ_DFL | PQ_INT, (void *)0, &(init_param->is_Maid48), NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        oamp_usage_print(unit);
        parse_arg_eq_done(&pt);
        rv = CMD_FAIL;
    }
    parse_arg_eq_done(&pt);
    return rv;
}

int
oamp_test_init(int unit, args_t *a, void **p) {

    oamp_test_init_param_t *init_param;
    init_param = &oamp_test_p[unit];

    if (SOC_DPP_CONFIG(unit)->pp.oam_enable == 0) {
        return CMD_FAIL;
    }
    sal_memset(init_param, 0x0, sizeof(oamp_test_init_param_t));
    *p = init_param;

    return oamp_usage_parse(unit, a, init_param);
}

int
oamp_test_done(int unit, void *p) {

    cli_out("OAM Test Done\n");
    return CMD_OK;
}


