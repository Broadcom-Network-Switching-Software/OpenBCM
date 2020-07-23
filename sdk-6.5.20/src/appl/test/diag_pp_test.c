/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <appl/diag/shell.h>
#include <appl/diag/test.h>
#include <sal/appl/io.h>
#include <soc/drv.h>


#ifdef BCM_PETRA_SUPPORT
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <appl/dpp/FecAllocation/fec_allocation.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/PPC/ppc_api_lif.h>

/* Defines for constant values. */

/* 
 * In Jericho, global outlifs take up the higher banks. In arad, they take a lower bank, depneding on the used
 *  application.
 */
#define DIAG_PP_DEFAULT_VPLS_GLOBAL_OUTLIF(_unit)               ((SOC_IS_JERICHO(_unit)) ? ((SOC_IS_JERICHO_PLUS(_unit))? 8196 : 4100) : 12288)

#define DIAG_PP_DEFAULT_ROUTER_AND_MPLS_GLOBAL_OUTLIF(_unit)    ((SOC_IS_JERICHO_PLUS(_unit)) ? 8192 : 4096)


/*
 *  In Jericho, phase bank range is 8k, and in arad it's 4k. Since rifs take the bottom 4k, all other phases
 *  take the next bank.
 */ 
#define DIAG_PP_FIRST_DEFAULT_EGRESS_LOCAL_OUTLIF(_unit)  ((SOC_IS_JERICHO_PLUS(_unit)) ? 8194 : (SOC_IS_JERICHO(unit)) ? 8192 : 4096)

#define DIAG_PP_SECOND_DEFAULT_EGRESS_LOCAL_OUTLIF(_unit) ((SOC_IS_JERICHO(_unit)) ? 16384 : 8192)

/*
 * In Jericho, rifs are a part of the EEDB, and bank 0 (0-8192) can be used for phase two applications like mpls tunnel.
 *  In QAX, the rifs are no longer part of the EEDB, and bank 0 (0-8192) is fully resereved for rifs.
 */
#define DIAG_PP_FIRST_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(_unit) ((SOC_IS_JERICHO_PLUS(unit)) ? 8194 : 4096)

#define DIAG_PP_SECOND_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(_unit) ((SOC_IS_JERICHO_PLUS(unit)) ? 16384 : 8192)

#define DIAG_PP_THIRD_VPLS_EGRESS_LOCAL_OUTLIF(_unit)  ((SOC_IS_JERICHO_PLUS(_unit)) ? 24576 : (SOC_IS_JERICHO(_unit)) ? 16384 : 12288)

/* In Arad, inlif range 0-4095 is reserved for rifs. In Jericho, it's not an issue. */
#define DIAG_PP_DEAFULT_INGRESS_ONLY_GLOBAL_INLIF(_unit)    ((SOC_IS_JERICHO(_unit)) ? 2 : 4096)

#define DIAG_PP_DEAFULT_INGRESS_ONLY_GLOBAL_INLIF_3(_unit)  ((SOC_IS_JERICHO(_unit)) ? 2 : 12288)

#define DIAG_PP_DEAFULT_INGRESS_LOCAL_LIF(_unit)            (SOC_IS_JERICHO(_unit) ? 32768 : 16384)

/* This number is affected by the ecmp reserved fecs. */
#define DIAG_PP_DEAFULT_FEC(_unit)                          (SOC_DPP_DEFS_GET(unit, nof_ecmp))

/*
 *  For Jericho, since not all ports are on the same core, it's possible that the cpu port is not 0, and
 *  therefore the TM port is also different.
 *  For Arad, this issue is irrelevant, so we always use TM port 0.
 */
#define DIAG_PP_TM_PORT(_unit, _in_local_port) (SOC_IS_JERICHO(unit) ? _in_local_port : 0)

char diag_pp_test_usage[] =
"Diag PP Test Usage:\n"
"  Bridge=<value>         1: Test bridge appl.\n"
"  Router=<value>         1: Test router appl.\n"
"  Mpls=<value>           1: Test mpls appl.\n"
"  Vpls=<value>           1: Test vpls appl., sending a packet from PWE to AC.\n"
"                         2: Test vpls appl., sending a packet from AC to PWE.\n"
"  Meter=<value>          1: Test meter appl. id1=<value> and id2=<value> match Meter1 and Meter2 ids.\n"
;

typedef struct diag_pp_received_packets_fields
{
    uint32 in_tm_port;
    uint32 packet_qual;
    soc_port_t sys_id;
    SOC_PPC_PORT in_pp_port;
    SOC_TMC_PORT_HEADER_TYPE pp_context;
    SOC_SAND_PP_SYS_PORT_TYPE sys_port_type;
}diag_pp_received_packets_fields_t;

typedef struct diag_pp_parsing_info_fields
{
    uint32 pfc_hw;
    SOC_PPC_DIAG_PARSING_HEADER_INFO hdrs_stack[SOC_PPC_DIAG_MAX_NOF_HDRS];
}diag_pp_parsing_info_fields_t;

typedef struct diag_pp_termination_info_fields
{
    SOC_PPC_PKT_TERM_TYPE term_type;
    SOC_PPC_PKT_FRWRD_TYPE frwrd_type;
}diag_pp_termination_info_fields_t;

typedef struct diag_pp_vlan_edit_info_fields
{
    uint32 cmd_id;
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO cmd_info;
    SOC_SAND_PP_VLAN_TAG ac_tag;
    SOC_SAND_PP_VLAN_TAG ac_tag2;
}diag_pp_vlan_edit_info_fields_t;

typedef struct diag_pp_pkt_tm_info_fields
{
    uint32 valid_fields;
    SOC_PPC_FRWRD_DECISION_INFO frwrd_decision;
    SOC_SAND_PP_TC tc;
    SOC_SAND_PP_DP dp;
    uint32 meter1;
    uint32 meter2;
    uint32 dp_meter_cmd;
    uint32 counter1;
    uint32 counter2;
    uint32 cud;
    uint32 eth_meter_ptr;
    uint32 ingress_shaping_da;
    uint32 ecn_capable;
    uint32 cni;
    SOC_SAND_PP_ETHERNET_DA_TYPE da_type;
    uint32 st_vsq_ptr;
    uint32 lag_lb_key;
    uint32 ignore_cp;
    uint32 snoop_id;
}diag_pp_pkt_tm_info_fields_t;

typedef struct diag_pp_lif_lkup_info_fields
{
    SOC_PPC_DIAG_LIF_LKUP_TYPE type;
    SOC_PPC_DIAG_LIF_KEY key;
    uint32 base_index;
    uint32 opcode_id;
    SOC_PPC_DIAG_LIF_VALUE value;
    uint8 found;
}diag_pp_lif_lkup_info_fields_t;

typedef struct diag_pp_frwrd_lpm_lkup_fields
{
    uint32 fec_ptr;
    uint8 found;
}diag_pp_frwrd_lpm_lkup_fields_t;

typedef struct diag_pp_lem_lkup_info_fields
{
    SOC_PPC_DIAG_LEM_LKUP_TYPE type;
    SOC_PPC_DIAG_LEM_KEY key;
    SOC_PPC_DIAG_LEM_VALUE value;
    uint8 valid;
}diag_pp_lem_lkup_info_fields_t;

typedef struct diag_pp_test_params
{
    int32 bridge;
    int32 router;
    int32 mpls;
    int32 vpls;
    int32 counter;
	int32 meter;
	int32 fec_allocation;
    int32 in_core;
    int32 out_core;
	int32 meter_id_1;
	int32 meter_id_2;
    int32 in_port;
    int32 out_port;
}diag_pp_test_params_t;

STATIC diag_pp_test_params_t diag_pp[SOC_MAX_NUM_DEVICES];

int diag_pp_vlan_edit_info_ac_tag_compare(SOC_SAND_PP_VLAN_TAG *ac_tag, SOC_SAND_PP_VLAN_TAG *expected_ac_tag);


int
diag_pp_received_packet_get(int unit, int core_id, diag_pp_received_packets_fields_t *pkt_info, SOC_PPC_DIAG_RESULT *ret_val)
{
    int rv = 0;
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO prm_rcvd_pkt_info;
    uint32 soc_sand_dev_id;


    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(&prm_rcvd_pkt_info);
    rv = soc_ppd_diag_received_packet_info_get(soc_sand_dev_id, core_id, &prm_rcvd_pkt_info, ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_received_packet_info_get() Failed:\n");
        return rv;
    } 
    if (*ret_val != SOC_PPC_DIAG_OK) {
        return rv;
    }


    pkt_info->in_pp_port    = prm_rcvd_pkt_info.in_pp_port;
    pkt_info->in_tm_port    = prm_rcvd_pkt_info.in_tm_port;
    pkt_info->packet_qual   = prm_rcvd_pkt_info.packet_qual;
    pkt_info->pp_context    = prm_rcvd_pkt_info.pp_context;
    pkt_info->sys_id        = prm_rcvd_pkt_info.src_sys_port.sys_id;
    pkt_info->sys_port_type = prm_rcvd_pkt_info.src_sys_port.sys_port_type;

    return rv;
}

int
diag_pp_parsing_info_get(int unit, int core_id, diag_pp_parsing_info_fields_t *pars_info)
{
    int rv = 0;
    SOC_PPC_DIAG_PARSING_INFO prm_pars_info;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_PARSING_INFO_clear(&prm_pars_info);
    rv = soc_ppd_diag_parsing_info_get(soc_sand_dev_id, core_id, &prm_pars_info, &ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_parsing_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_parsing_info_get() Failed:\n");
        return rv;
    } 

    pars_info->pfc_hw = prm_pars_info.pfc_hw;
    memcpy(pars_info->hdrs_stack, prm_pars_info.hdrs_stack, sizeof(SOC_PPC_DIAG_PARSING_HEADER_INFO)*SOC_PPC_DIAG_MAX_NOF_HDRS);

    return rv;
}

int
diag_pp_termination_info_get(int unit, int core_id, diag_pp_termination_info_fields_t *term_info)
{
    int rv = 0;
    SOC_PPC_DIAG_TERM_INFO prm_term_info;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_TERM_INFO_clear(&prm_term_info);
    rv = soc_ppd_diag_termination_info_get(soc_sand_dev_id, core_id, &prm_term_info, &ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_termination_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK)
    { 
        cli_out("Error: soc_ppd_diag_termination_info_get() Failed:\n");
        return rv;
    } 

    term_info->term_type = prm_term_info.term_type;
    term_info->frwrd_type = prm_term_info.frwrd_type;

    return rv;
}

int
diag_pp_vlan_edit_info_get(int unit, int core_id, diag_pp_vlan_edit_info_fields_t *vlan_edit_info)
{
    int rv = 0;
    SOC_PPC_DIAG_VLAN_EDIT_RES prm_vlan_edit_info;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_VLAN_EDIT_RES_clear(&prm_vlan_edit_info);
    rv = soc_ppd_diag_ing_vlan_edit_info_get(soc_sand_dev_id, core_id, &prm_vlan_edit_info, &ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_ing_vlan_edit_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_ing_vlan_edit_info_get() Failed:\n");
        return rv;
    } 

    vlan_edit_info->cmd_id = prm_vlan_edit_info.cmd_id;
    memcpy(&vlan_edit_info->cmd_info, &prm_vlan_edit_info.cmd_info, sizeof(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO));
    memcpy(&vlan_edit_info->ac_tag, &prm_vlan_edit_info.ac_tag, sizeof(SOC_SAND_PP_VLAN_TAG));
    memcpy(&vlan_edit_info->ac_tag2, &prm_vlan_edit_info.ac_tag2, sizeof(SOC_SAND_PP_VLAN_TAG));

    return CMD_OK;

}

int
diag_pp_pkt_tm_info_get(int unit, int core_id, diag_pp_pkt_tm_info_fields_t *pkt_tm_info)
{
    int rv = 0;
    SOC_PPC_DIAG_PKT_TM_INFO prm_pkt_tm_info;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_PKT_TM_INFO_clear(&prm_pkt_tm_info);
    rv = soc_ppd_diag_pkt_associated_tm_info_get(soc_sand_dev_id, core_id, &prm_pkt_tm_info, &ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_pkt_associated_tm_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_pkt_associated_tm_info_get() Failed:\n");
        return rv;
    } 

    memcpy(&pkt_tm_info->frwrd_decision, &prm_pkt_tm_info.frwrd_decision, sizeof(SOC_PPC_FRWRD_DECISION_INFO));
    pkt_tm_info->valid_fields = prm_pkt_tm_info.valid_fields;
    pkt_tm_info->tc = prm_pkt_tm_info.tc;
    pkt_tm_info->dp = prm_pkt_tm_info.dp;
    pkt_tm_info->meter1 = prm_pkt_tm_info.meter1;
    pkt_tm_info->meter2 = prm_pkt_tm_info.meter2;
    pkt_tm_info->dp_meter_cmd = prm_pkt_tm_info.dp_meter_cmd;
    pkt_tm_info->counter1 = prm_pkt_tm_info.counter1;
    pkt_tm_info->counter2 = prm_pkt_tm_info.counter2;
    pkt_tm_info->cud = prm_pkt_tm_info.cud;
    pkt_tm_info->eth_meter_ptr = prm_pkt_tm_info.eth_meter_ptr;
    pkt_tm_info->ingress_shaping_da = prm_pkt_tm_info.ingress_shaping_da;
    pkt_tm_info->ecn_capable = prm_pkt_tm_info.ecn_capable;
    pkt_tm_info->cni = prm_pkt_tm_info.cni;
    pkt_tm_info->da_type = prm_pkt_tm_info.da_type;
    pkt_tm_info->st_vsq_ptr = prm_pkt_tm_info.st_vsq_ptr;
    pkt_tm_info->lag_lb_key = prm_pkt_tm_info.lag_lb_key;
    pkt_tm_info->ignore_cp = prm_pkt_tm_info.ignore_cp;
    pkt_tm_info->snoop_id = prm_pkt_tm_info.snoop_id;

    return rv;
}

int
diag_pp_lif_lkup_info_get(int unit, int core_id, diag_pp_lif_lkup_info_fields_t *lif_lkup_info, SOC_PPC_DIAG_DB_USE_INFO *db_info)
{
    int rv = 0;
    SOC_PPC_DIAG_LIF_LKUP_INFO prm_lif_lkup_info;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_LIF_LKUP_INFO_clear(&prm_lif_lkup_info);
    rv = soc_ppd_diag_db_lif_lkup_info_get(soc_sand_dev_id, core_id, db_info, &prm_lif_lkup_info, &ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_db_lif_lkup_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_db_lif_lkup_info_get() Failed:\n");
        return rv; 
    } 

    lif_lkup_info->type = prm_lif_lkup_info.type;
    memcpy(&lif_lkup_info->key, &prm_lif_lkup_info.key, sizeof(SOC_PPC_DIAG_LIF_KEY));
    lif_lkup_info->found = prm_lif_lkup_info.found;
    memcpy(&lif_lkup_info->value, &prm_lif_lkup_info.value, sizeof(SOC_PPC_DIAG_LIF_VALUE));
    lif_lkup_info->base_index = prm_lif_lkup_info.base_index;
    lif_lkup_info->opcode_id = prm_lif_lkup_info.opcode_id;

    return rv;
}

int
diag_pp_frwrd_trace_info_get(int unit, int core_id, SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *frwrd_trace_info, SOC_PPC_DIAG_RESULT *ret_val)
{
    int rv = 0;
    uint32 soc_sand_dev_id;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(frwrd_trace_info);
    rv = soc_ppd_diag_frwrd_decision_trace_get(soc_sand_dev_id, core_id, frwrd_trace_info, ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_frwrd_decision_trace_get() Failed:\n");
        return rv;
    } 

    return rv;
}

int
diag_pp_traps_info_get(int unit, int core_id, SOC_PPC_DIAG_TRAPS_INFO *traps_info)
{
    int rv = 0;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_TRAPS_INFO_clear(traps_info);
    rv = soc_ppd_diag_traps_info_get(soc_sand_dev_id, core_id, traps_info, &ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_traps_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_parsing_info_get() Failed:\n");
        return rv;
    } 


    return rv;
}

cmd_result_t
diag_pp_encap_info_get(int unit, int core_id, SOC_PPC_DIAG_ENCAP_INFO *encap_info)
{
    uint32 res;
    SOC_PPC_DIAG_RESULT                     ret_val;

    SOC_PPC_DIAG_ENCAP_INFO_clear(encap_info);
    res = soc_ppd_diag_encap_info_get(unit, core_id, encap_info, &ret_val);
    if (handle_sand_result(res) != SOC_E_NONE) 
    { 
        cli_out("Error: soc_ppd_diag_encap_info_get() Failed:\n");
        return CMD_FAIL;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_encap_info_get() Failed:\n");
        return CMD_OK;
    } 

    return CMD_OK;
}

int
diag_pp_frwrd_lkup_info_get(int unit, int core_id, SOC_PPC_DIAG_FRWRD_LKUP_INFO *frwrd_lkup_info)
{
    int rv = 0;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(frwrd_lkup_info);
    rv = soc_ppd_diag_frwrd_lkup_info_get(soc_sand_dev_id, core_id, frwrd_lkup_info, &ret_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_frwrd_lkup_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) {
        cli_out("Error: soc_ppd_diag_frwrd_lkup_info_get() Failed:\n");
        return rv;
    }

    return CMD_OK;
}

int
diag_pp_frwrd_lpm_lkup_get(int unit, int core_id, diag_pp_frwrd_lpm_lkup_fields_t *lpm_lkup_info)
{
    int rv = 0;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_IPV4_VPN_KEY prm_lpm_key;
    uint32 ip_addr = 0x7fffff00;
    uint8 prefix_len = 24;
    uint32 vrf = 0;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_IPV4_VPN_KEY_clear(&prm_lpm_key);
    prm_lpm_key.key.subnet.ip_address = ip_addr;
    prm_lpm_key.key.subnet.prefix_len = prefix_len;
    prm_lpm_key.vrf = vrf;
    rv = soc_ppd_diag_frwrd_lpm_lkup_get(
          soc_sand_dev_id,
          core_id,
          &prm_lpm_key,
          &lpm_lkup_info->fec_ptr,
          &lpm_lkup_info->found,
          &ret_val
          );
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_frwrd_lpm_lkup_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_frwrd_lpm_lkup_get() Failed:\n");
        return rv;
    } 

    return rv;
}

int
diag_pp_lem_lkup_info_get(int unit, int core_id, diag_pp_lem_lkup_info_fields_t *lem_lkup_info)
{
    int rv = 0;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_DB_USE_INFO prm_db_info;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_DB_USE_INFO_clear(&prm_db_info);
    prm_db_info.lkup_num = 0;
    rv = soc_ppd_diag_db_lem_lkup_info_get(
          soc_sand_dev_id,
          core_id,
          &prm_db_info,
          &lem_lkup_info->type,
          &lem_lkup_info->key,
          &lem_lkup_info->value,
          &lem_lkup_info->valid
          );

    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: soc_ppd_diag_db_lem_lkup_info_get() Failed:\n");
        return rv;
    } 

    return rv;
}

int
diag_pp_egress_vlan_edit_info_get(int unit, int core_id, SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *egress_vlan_edit_info)
{
    int rv = 0;
    uint32 soc_sand_dev_id;
    SOC_PPC_DIAG_RESULT                     ret_val;

    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(egress_vlan_edit_info);
    rv = soc_ppd_diag_egress_vlan_edit_info_get(soc_sand_dev_id, core_id, egress_vlan_edit_info, &ret_val);

    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
    { 
        cli_out("Error: diag_pp_egress_vlan_edit_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: diag_pp_egress_vlan_edit_info_get() Failed:\n");
        return rv;
    } 

    return rv;

}

int
diag_pp_received_packet_compare(diag_pp_received_packets_fields_t *pkt_info, diag_pp_received_packets_fields_t *expected_pkt_info)
{
    int rv = 0;

    if(pkt_info->in_pp_port != expected_pkt_info->in_pp_port)
    {
        cli_out("Unexpected in_pp_port. Received: %d Expected: %d\n", pkt_info->in_pp_port, expected_pkt_info->in_pp_port);
        rv = CMD_FAIL;
    }
    if(pkt_info->in_tm_port != expected_pkt_info->in_tm_port)
    {
        cli_out("Unexpected in_tm_port. Received: %d Expected: %d\n", pkt_info->in_tm_port, expected_pkt_info->in_tm_port);
        rv = CMD_FAIL;
    }
    if(pkt_info->packet_qual != expected_pkt_info->packet_qual)
    {
        cli_out("Unexpected packet_qual. Received: %d Expected: %d\n", pkt_info->packet_qual, expected_pkt_info->packet_qual);
        rv = CMD_FAIL;
    }
    if(pkt_info->pp_context != expected_pkt_info->pp_context)
    {
        cli_out("Unexpected pp_context. Received: %d Expected: %d\n", pkt_info->pp_context, expected_pkt_info->pp_context);
        rv = CMD_FAIL;
    }
    if(pkt_info->sys_id != expected_pkt_info->sys_id)
    {
        cli_out("Unexpected sys_id. Received: %d Expected: %d\n", pkt_info->sys_id, expected_pkt_info->sys_id);
        rv = CMD_FAIL;
    }
    if(pkt_info->sys_port_type != expected_pkt_info->sys_port_type)
    {
        cli_out("Unexpected sys_port_type. Received: %d Expected: %d\n", pkt_info->sys_port_type, expected_pkt_info->sys_port_type);
        rv = CMD_FAIL;
    }

    return rv;
}

int
diag_pp_parsing_info_compare(diag_pp_parsing_info_fields_t *pars_info, diag_pp_parsing_info_fields_t *expected_pars_info, uint8 num_of_headers)
{
    int rv = 0;
    uint32 hdr_index;

    if(pars_info->pfc_hw != expected_pars_info->pfc_hw)
    {
        cli_out("Unexpected hdr_type. Received: %d Expected: %d\n", pars_info->pfc_hw, expected_pars_info->pfc_hw);
        rv = CMD_FAIL;
    }

    for(hdr_index = 0; hdr_index < num_of_headers; hdr_index++)
    {
        if(pars_info->hdrs_stack[hdr_index].hdr_offset != expected_pars_info->hdrs_stack[hdr_index].hdr_offset)
        {
            cli_out("Unexpected hdr_offset. Received: %d Expected: %d\n", 
                    pars_info->hdrs_stack[hdr_index].hdr_offset, 
                    expected_pars_info->hdrs_stack[hdr_index].hdr_offset);
            rv = CMD_FAIL;
        }
        if(pars_info->hdrs_stack[hdr_index].hdr_type != expected_pars_info->hdrs_stack[hdr_index].hdr_type)
        {
            cli_out("Unexpected hdr_type. Received: %d Expected: %d\n", 
                    pars_info->hdrs_stack[hdr_index].hdr_type, 
                    expected_pars_info->hdrs_stack[hdr_index].hdr_type);
            rv = CMD_FAIL;
        }
        if(pars_info->hdrs_stack[hdr_index].hdr_type == SOC_PPC_PKT_HDR_TYPE_ETH)
        {
            if(memcmp(&pars_info->hdrs_stack[hdr_index].eth, &expected_pars_info->hdrs_stack[hdr_index].eth, sizeof(SOC_PPC_DIAG_PARSING_L2_INFO)) != 0)
            {
                cli_out("Unexpected differences in ETH header.\n");
                rv = CMD_FAIL;
            }
        }
        else if(pars_info->hdrs_stack[hdr_index].hdr_type == SOC_PPC_PKT_HDR_TYPE_IPV4)
        {
            if(memcmp(&pars_info->hdrs_stack[hdr_index].ip, &expected_pars_info->hdrs_stack[hdr_index].ip, sizeof(SOC_PPC_DIAG_PARSING_IP_INFO)) != 0)
            {
                cli_out("Unexpected differences in IPv4 header.\n");
                rv = CMD_FAIL;
            }
        }
        else
        {
            break;
        }
    }
    return rv;
}

int
diag_pp_termination_info_compare(diag_pp_termination_info_fields_t *term_info, diag_pp_termination_info_fields_t *expected_term_info)
{
    int rv = 0;

    if(term_info->term_type != expected_term_info->term_type)
    {
        cli_out("Unexpected term_type. Received: %d Expected: %d\n", term_info->term_type, expected_term_info->term_type);
        rv = CMD_FAIL;
    }
    if(term_info->term_type != SOC_PPC_PKT_TERM_TYPE_NONE)
    {
        /* Check forwarding type only if termination was done */
        if(term_info->frwrd_type != expected_term_info->frwrd_type)
        {
            cli_out("Unexpected frwrd_type. Received: %d Expected: %d\n", term_info->frwrd_type, expected_term_info->frwrd_type);
            rv = CMD_FAIL;
        }
    }

    return rv;
}

int
diag_pp_vlan_edit_info_compare(diag_pp_vlan_edit_info_fields_t *vlan_edit_info, diag_pp_vlan_edit_info_fields_t *expected_vlan_edit_info)
{
    int rv = 0;

    if(vlan_edit_info->cmd_id != expected_vlan_edit_info->cmd_id)
    {
        cli_out("Unexpected cmd_id. Received: %d Expected: %d\n", vlan_edit_info->cmd_id, expected_vlan_edit_info->cmd_id);
        rv = CMD_FAIL;
    }
    if(memcmp(&vlan_edit_info->cmd_info, &expected_vlan_edit_info->cmd_info, sizeof(SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO)) != 0)
    {
        cli_out("Unexpected differences in cmd_info.\n");
        rv = CMD_FAIL;
    }
    if(diag_pp_vlan_edit_info_ac_tag_compare(&(vlan_edit_info->ac_tag), &(expected_vlan_edit_info->ac_tag)) != CMD_OK)
    {
        cli_out("Unexpected differences in ac_tag.\n");
        rv = CMD_FAIL;
    }
    if(diag_pp_vlan_edit_info_ac_tag_compare(&(vlan_edit_info->ac_tag2), &(expected_vlan_edit_info->ac_tag2)) != CMD_OK)
    {
        cli_out("Unexpected differences in ac_tag2.\n");
        rv = CMD_FAIL;
    }
 
    return rv;
}

int
diag_pp_vlan_edit_info_ac_tag_compare(SOC_SAND_PP_VLAN_TAG *ac_tag, SOC_SAND_PP_VLAN_TAG *expected_ac_tag){
    int rv = 0;

    if (ac_tag->vid == BCM_VLAN_INVALID && expected_ac_tag->vid == BCM_VLAN_INVALID) 
    {
        return rv;
    }

    if (memcmp(ac_tag, expected_ac_tag, sizeof(SOC_SAND_PP_VLAN_TAG )) != 0) {
        cli_out("Unexpected differences in tpid, received: %d, expected: %d.\n", ac_tag->tpid, expected_ac_tag->tpid);
        cli_out("Unexpected differences in vid,  received: %d, expected: %d.\n", ac_tag->vid, expected_ac_tag->vid);
        cli_out("Unexpected differences in pcp,  received: %d, expected: %d.\n", ac_tag->pcp, expected_ac_tag->pcp);
        cli_out("Unexpected differences in dei,  received: %d, expected: %d.\n", ac_tag->dei, expected_ac_tag->dei);
        return CMD_FAIL;
    }

    return rv;
}

int
diag_pp_pkt_tm_info_compare(diag_pp_pkt_tm_info_fields_t *pkt_tm_info, diag_pp_pkt_tm_info_fields_t *expected_pkt_tm_info)
{
    int rv = 0;

    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DEST)
    {
        if(memcmp(&pkt_tm_info->frwrd_decision, &expected_pkt_tm_info->frwrd_decision, sizeof(SOC_PPC_FRWRD_DECISION_INFO)) != 0)
        {
            cli_out("Unexpected differences in frwrd_decision.\n");
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_TC)
    {
        if(pkt_tm_info->tc != expected_pkt_tm_info->tc)
        {
            cli_out("Unexpected tc. Received: %d Expected: %d\n", pkt_tm_info->tc, expected_pkt_tm_info->tc);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DP)
    {
        if(pkt_tm_info->dp != expected_pkt_tm_info->dp)
        {
            cli_out("Unexpected dp. Received: %d Expected: %d\n", pkt_tm_info->dp, expected_pkt_tm_info->dp);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER1)
    {
        if(pkt_tm_info->meter1 != expected_pkt_tm_info->meter1)
        {
            cli_out("Unexpected meter1. Received: %d Expected: %d\n", pkt_tm_info->meter1, expected_pkt_tm_info->meter1);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER2)
    {
        if(pkt_tm_info->meter2 != expected_pkt_tm_info->meter2)
        {
            cli_out("Unexpected meter2. Received: %d Expected: %d\n", pkt_tm_info->meter2, expected_pkt_tm_info->meter2);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD)
    {
        if(pkt_tm_info->dp_meter_cmd != expected_pkt_tm_info->dp_meter_cmd)
        {
            cli_out("Unexpected dp_meter_cmd. Received: %d Expected: %d\n", pkt_tm_info->dp_meter_cmd, expected_pkt_tm_info->dp_meter_cmd);
            rv = CMD_FAIL;
        }
    }
    if (pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1)
    {
        if(pkt_tm_info->counter1 != expected_pkt_tm_info->counter1)
        {
            cli_out("Unexpected counter1. Received: %d Expected: %d\n", pkt_tm_info->counter1, expected_pkt_tm_info->counter1);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2)
    {
        if(pkt_tm_info->counter2 != expected_pkt_tm_info->counter2)
        {
            cli_out("Unexpected counter2. Received: %d Expected: %d\n", pkt_tm_info->counter2, expected_pkt_tm_info->counter2);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CUD)
    {
        if(pkt_tm_info->cud != expected_pkt_tm_info->cud)
        {
            cli_out("Unexpected cud. Received: %d Expected: %d\n", pkt_tm_info->cud, expected_pkt_tm_info->cud);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR)
    {
        if(pkt_tm_info->eth_meter_ptr != expected_pkt_tm_info->eth_meter_ptr)
        {
            cli_out("Unexpected eth_meter_ptr. Received: %d Expected: %d\n", pkt_tm_info->eth_meter_ptr, expected_pkt_tm_info->eth_meter_ptr);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA)
    {
        if(pkt_tm_info->ingress_shaping_da != expected_pkt_tm_info->ingress_shaping_da)
        {
            cli_out("Unexpected ingress_shaping_da. Received: %d Expected: %d\n", pkt_tm_info->ingress_shaping_da, expected_pkt_tm_info->ingress_shaping_da);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE)
    {
        if(pkt_tm_info->ecn_capable != expected_pkt_tm_info->ecn_capable)
        {
            cli_out("Unexpected ecn_capable. Received: %d Expected: %d\n", pkt_tm_info->ecn_capable, expected_pkt_tm_info->ecn_capable);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CNI)
    {
        if(pkt_tm_info->cni != expected_pkt_tm_info->cni)
        {
            cli_out("Unexpected cni. Received: %d Expected: %d\n", pkt_tm_info->cni, expected_pkt_tm_info->cni);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE)
    {
        if(pkt_tm_info->da_type != expected_pkt_tm_info->da_type)
        {
            cli_out("Unexpected da_type. Received: %d Expected: %d\n", pkt_tm_info->da_type, expected_pkt_tm_info->da_type);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR)
    {
        if(pkt_tm_info->st_vsq_ptr != expected_pkt_tm_info->st_vsq_ptr)
        {
            cli_out("Unexpected st_vsq_ptr. Received: %d Expected: %d\n", pkt_tm_info->st_vsq_ptr, expected_pkt_tm_info->st_vsq_ptr);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY)
    {
        if(pkt_tm_info->lag_lb_key != expected_pkt_tm_info->lag_lb_key)
        {
            cli_out("Unexpected lag_lb_key. Received: %d Expected: %d\n", pkt_tm_info->lag_lb_key, expected_pkt_tm_info->lag_lb_key);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP)
    {
        if(pkt_tm_info->ignore_cp != expected_pkt_tm_info->ignore_cp)
        {
            cli_out("Unexpected ignore_cp. Received: %d Expected: %d\n", pkt_tm_info->ignore_cp, expected_pkt_tm_info->ignore_cp);
            rv = CMD_FAIL;
        }
    }
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID)
    {
        if(pkt_tm_info->snoop_id != expected_pkt_tm_info->snoop_id)
        {
            cli_out("Unexpected snoop_id. Received: %d Expected: %d\n", pkt_tm_info->snoop_id, expected_pkt_tm_info->snoop_id);
            rv = CMD_FAIL;
        }
    }
 
    return rv;
}

int
diag_pp_pkt_tm_info_meter_compare(diag_pp_pkt_tm_info_fields_t *pkt_tm_info, diag_pp_pkt_tm_info_fields_t *expected_pkt_tm_info)
{
    int rv = 0;

    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER1)
    {
        if(pkt_tm_info->meter1 != expected_pkt_tm_info->meter1)
        {
            cli_out("Unexpected meter1. Received: %d Expected: %d\n", pkt_tm_info->meter1, expected_pkt_tm_info->meter1);
            rv = CMD_FAIL;
        }
    } else {
		if (expected_pkt_tm_info->meter1) {
			cli_out("Unexpected meter1. Received: Invalid Expected: %d\n", expected_pkt_tm_info->meter1);
            rv = CMD_FAIL;
		}
	}
    if(pkt_tm_info->valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER2)
    {
        if(pkt_tm_info->meter2 != expected_pkt_tm_info->meter2)
        {
            cli_out("Unexpected meter2. Received: %d Expected: %d\n", pkt_tm_info->meter2, expected_pkt_tm_info->meter2);
            rv = CMD_FAIL;
        }
	} else {
		if (expected_pkt_tm_info->meter2) {
			cli_out("Unexpected meter2. Received: Invalid Expected: %d\n", expected_pkt_tm_info->meter2);
            rv = CMD_FAIL;
		}
	} 
    return rv;
}

int
diag_pp_lif_lkup_info_compare(diag_pp_lif_lkup_info_fields_t *lif_lkup_info, diag_pp_lif_lkup_info_fields_t *expected_lif_lkup_info)
{
    int rv = 0;

    if(lif_lkup_info->type != expected_lif_lkup_info->type)
    {
        cli_out("Unexpected type. Received: %d Expected: %d\n", lif_lkup_info->type, expected_lif_lkup_info->type);
        rv = CMD_FAIL;
    }
    if(lif_lkup_info->found)
    {
        if(lif_lkup_info->value.ac.service_type != expected_lif_lkup_info->value.ac.service_type)
        {
            cli_out("Unexpected service_type. Received: %d Expected: %d\n", lif_lkup_info->value.ac.service_type, expected_lif_lkup_info->value.ac.service_type);
            rv = CMD_FAIL;
        }
        if(lif_lkup_info->value.ac.vsid != expected_lif_lkup_info->value.ac.vsid)
        {
            cli_out("Unexpected vsid. Received: %d Expected: %d\n", lif_lkup_info->value.ac.vsid, expected_lif_lkup_info->value.ac.vsid);
            rv = CMD_FAIL;
        }
        if(lif_lkup_info->value.ac.learn_record.learn_type != expected_lif_lkup_info->value.ac.learn_record.learn_type)
        {
            cli_out("Unexpected learn_type. Received: %d Expected: %d\n", lif_lkup_info->value.ac.learn_record.learn_type, expected_lif_lkup_info->value.ac.learn_record.learn_type);
            rv = CMD_FAIL;
        }
        if(lif_lkup_info->value.ac.default_frwrd.default_frwd_type != expected_lif_lkup_info->value.ac.default_frwrd.default_frwd_type)
        {
            cli_out("Unexpected default_frwd_type. Received: %d Expected: %d\n", lif_lkup_info->value.ac.default_frwrd.default_frwd_type, expected_lif_lkup_info->value.ac.default_frwrd.default_frwd_type);
            rv = CMD_FAIL;
        }
        if(lif_lkup_info->value.ac.orientation != expected_lif_lkup_info->value.ac.orientation)
        {
            cli_out("Unexpected orientation. Received: %d Expected: %d\n", lif_lkup_info->value.ac.orientation, expected_lif_lkup_info->value.ac.orientation);
            rv = CMD_FAIL;
        }
    }
    /*if(lif_lkup_info->base_index != expected_lif_lkup_info->base_index)
    {
        cli_out("Unexpected base_index. Received: %d Expected: %d\n", lif_lkup_info->base_index, expected_lif_lkup_info->base_index);
        rv = CMD_FAIL;
    }*/
    if(lif_lkup_info->opcode_id != expected_lif_lkup_info->opcode_id)
    {
        cli_out("Unexpected opcode_id. Received: %d Expected: %d\n", lif_lkup_info->opcode_id, expected_lif_lkup_info->opcode_id);
        rv = CMD_FAIL;
    }
 
    return rv;
}

int
diag_pp_frwrd_trace_info_compare(SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *frwrd_trace_info, SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *expected_frwrd_trace_info)
{
    uint32 frwrd_decision_phase;
    int rv = 0;

    for(frwrd_decision_phase = 0; frwrd_decision_phase < SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES; frwrd_decision_phase++)
    {
        if(frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.type != expected_frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.type)
        {
            cli_out("Unexpected frwrd_decision type in phase %d. Received: %d Expected: %d\n", 
                    frwrd_decision_phase, 
                    frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.type, 
                    expected_frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.type);
            rv = CMD_FAIL;
        }
        if(frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.dest_id != expected_frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.dest_id)
        {
            cli_out("Unexpected frwrd_decision dest_id in phase %d. Received: %d Expected: %d\n", 
                    frwrd_decision_phase, 
                    frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.dest_id, 
                    expected_frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.dest_id);
            rv = CMD_FAIL;
        }
        if(memcmp(&frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.additional_info, 
                  &expected_frwrd_trace_info->frwrd[frwrd_decision_phase].frwrd_decision.additional_info, 
                  sizeof(SOC_PPC_FRWRD_DECISION_TYPE_INFO)) != 0)
        {
            cli_out("Unexpected differences in frwrd_decision additional info in phase %d.\n", 
                    frwrd_decision_phase);
            rv = CMD_FAIL;

        }
        if(frwrd_trace_info->trap[frwrd_decision_phase].action_profile.trap_code != expected_frwrd_trace_info->trap[frwrd_decision_phase].action_profile.trap_code)
        {
            cli_out("Unexpected trap_code in phase %d. Received: %d Expected: %d\n", 
                    frwrd_decision_phase, 
                    frwrd_trace_info->trap[frwrd_decision_phase].action_profile.trap_code, 
                    expected_frwrd_trace_info->trap[frwrd_decision_phase].action_profile.trap_code);
            rv = CMD_FAIL;
        }
        if(frwrd_trace_info->trap[frwrd_decision_phase].action_profile.frwrd_action_strength != expected_frwrd_trace_info->trap[frwrd_decision_phase].action_profile.frwrd_action_strength)
        {
            cli_out("Unexpected frwrd_action_strength in phase %d. Received: %d Expected: %d\n", 
                    frwrd_decision_phase, 
                    frwrd_trace_info->trap[frwrd_decision_phase].action_profile.frwrd_action_strength, 
                    expected_frwrd_trace_info->trap[frwrd_decision_phase].action_profile.frwrd_action_strength);
            rv = CMD_FAIL;
        }
    }

    return rv;
}

int
diag_pp_traps_info_compare(SOC_PPC_DIAG_TRAPS_INFO *traps_info, SOC_PPC_DIAG_TRAPS_INFO *expected_traps_info)
{
    int rv = 0;

    if(traps_info->committed_trap.is_pkt_trapped != expected_traps_info->committed_trap.is_pkt_trapped)
    {
        cli_out("Unexpected is_pkt_trapped Received: %d Expected: %d\n", 
                traps_info->committed_trap.is_pkt_trapped, 
                expected_traps_info->committed_trap.is_pkt_trapped);
        rv = CMD_FAIL;
    }
    if(traps_info->committed_trap.code != expected_traps_info->committed_trap.code)
    {
        cli_out("Unexpected committed trap code. Received: %d Expected: %d\n", 
                traps_info->committed_trap.code, 
                expected_traps_info->committed_trap.code);
        rv = CMD_FAIL;
    }
    if(memcmp(&traps_info->committed_trap.info, &expected_traps_info->committed_trap.info, sizeof(SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO)) != 0)
    {
        cli_out("Unexpected differences in commiteted trap info.\n");
        rv = CMD_FAIL;
    }
    if(traps_info->committed_snoop.code != expected_traps_info->committed_snoop.code)
    {
        cli_out("Unexpected commited snoop code. Received: %d Expected: %d\n", 
                traps_info->committed_snoop.code, 
                expected_traps_info->committed_snoop.code);
        rv = CMD_FAIL;
    }
    if(traps_info->committed_snoop.info.strength != expected_traps_info->committed_snoop.info.strength)
    {
        cli_out("Unexpected commited snoop strength. Received: %d Expected: %d\n", 
                traps_info->committed_snoop.info.strength, 
                expected_traps_info->committed_snoop.info.strength);
        rv = CMD_FAIL;
    }
    if(traps_info->committed_snoop.info.strength != 0)
    {
        if(traps_info->committed_snoop.info.snoop_cmnd != expected_traps_info->committed_snoop.info.snoop_cmnd)
        {
            cli_out("Unexpected commited snoop cmnd. Received: %d Expected: %d\n", 
                    traps_info->committed_snoop.info.snoop_cmnd, 
                    expected_traps_info->committed_snoop.info.snoop_cmnd);
            rv = CMD_FAIL;
        }
    }
    if(traps_info->trap_updated_dest.is_pkt_trapped != expected_traps_info->trap_updated_dest.is_pkt_trapped)
    {
        cli_out("Unexpected trap_updated_dest is_pkt_trapped. Received: %d Expected: %d\n", 
                traps_info->trap_updated_dest.is_pkt_trapped, 
                expected_traps_info->trap_updated_dest.is_pkt_trapped);
        rv = CMD_FAIL;
    }
    if(traps_info->trap_updated_dest.code != expected_traps_info->trap_updated_dest.code)
    {
        cli_out("Unexpected trap_updated_dest code. Received: %d Expected: %d\n", 
                traps_info->trap_updated_dest.code, 
                expected_traps_info->trap_updated_dest.code);
        rv = CMD_FAIL;
    }
    if(memcmp(&traps_info->trap_updated_dest.info, &expected_traps_info->trap_updated_dest.info, sizeof(SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO)) != 0)
    {
        cli_out("Unexpected differences in trap_updated_des info.\n");
        rv = CMD_FAIL;
    }
    if(memcmp(traps_info->trap_stack, expected_traps_info->trap_stack, sizeof(uint32)*(SOC_PPC_NOF_TRAP_CODES/SOC_SAND_NOF_BITS_IN_UINT32)) != 0)
    {
        cli_out("Unexpected differences in trap_stack.\n");
        rv = CMD_FAIL;
    }

    return rv;
}

int
diag_pp_encap_info_compare(SOC_PPC_DIAG_ENCAP_INFO *encap_info, SOC_PPC_DIAG_ENCAP_INFO *expected_encap_info)
{
    int eep_index;
    int rv = 0;

   if(expected_encap_info->nof_eeps != encap_info->nof_eeps)
   {
       cli_out("Unexpected differences in number of encapsulations.\n");
       rv = CMD_FAIL;
   }

    for(eep_index = 0; eep_index < expected_encap_info->nof_eeps; eep_index++)
    {
        if(memcmp(&encap_info->encap_info[eep_index], &expected_encap_info->encap_info[eep_index], sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO)) != 0)
        {
            cli_out("Unexpected differences in encap info, eep %d.\n", eep_index);
            rv = CMD_FAIL;
        }
        if(encap_info->eep[eep_index] != expected_encap_info->eep[eep_index])
        {
            cli_out("Unexpected eep pointer in eep %d. Received: %d Expected: %d\n", 
                    eep_index,
                    encap_info->eep[eep_index], 
                    expected_encap_info->eep[eep_index]);
            rv = CMD_FAIL;
        }
    }

    if(encap_info->ll_vsi != expected_encap_info->ll_vsi)
    {
        cli_out("Unexpected ll_vsi. Received: %d Expected: %d\n", 
                encap_info->ll_vsi, 
                expected_encap_info->ll_vsi);
        rv = CMD_FAIL;
    }
    if(encap_info->out_ac != expected_encap_info->out_ac)
    {
        cli_out("Unexpected out_ac. Received: %d Expected: %d\n", 
                encap_info->out_ac, 
                expected_encap_info->out_ac);
        rv = CMD_FAIL;
    }
    if(encap_info->tm_port != expected_encap_info->tm_port)
    {
        cli_out("Unexpected tm_port. Received: %d Expected: %d\n", 
                encap_info->tm_port, 
                expected_encap_info->tm_port);
        rv = CMD_FAIL;
    }
    if(encap_info->pp_port != expected_encap_info->pp_port)
    {
        cli_out("Unexpected pp_port. Received: %d Expected: %d\n", 
                encap_info->pp_port, 
                expected_encap_info->pp_port);
        rv = CMD_FAIL;
    }

    return rv;
}

int
diag_pp_frwrd_lkup_info_compare(SOC_PPC_DIAG_FRWRD_LKUP_INFO *frwrd_lkup_info, SOC_PPC_DIAG_FRWRD_LKUP_INFO *expected_frwrd_lkup_info)
{
    int rv = 0;

    if(frwrd_lkup_info->frwrd_type != expected_frwrd_lkup_info->frwrd_type)
    {
        cli_out("Unexpected frwrd_type. Received: %d Expected: %d\n", 
                frwrd_lkup_info->frwrd_type, 
                expected_frwrd_lkup_info->frwrd_type);
        rv = CMD_FAIL;
    }
    if(frwrd_lkup_info->frwrd_hdr_index != expected_frwrd_lkup_info->frwrd_hdr_index)
    {
        cli_out("Unexpected frwrd_hdr_index. Received: %d Expected: %d\n", 
                frwrd_lkup_info->frwrd_hdr_index, 
                expected_frwrd_lkup_info->frwrd_hdr_index);
        rv = CMD_FAIL;
    }
    if(memcmp(&frwrd_lkup_info->lkup_key, &expected_frwrd_lkup_info->lkup_key, sizeof(SOC_PPC_DIAG_FRWRD_LKUP_KEY)) != 0)
    {
        cli_out("Unexpected differences in lkup_key\n");
        rv = CMD_FAIL;
    }
    if(frwrd_lkup_info->key_found != expected_frwrd_lkup_info->key_found)
    {
        cli_out("Unexpected key_found. Received: %d Expected: %d\n", 
                frwrd_lkup_info->key_found, 
                expected_frwrd_lkup_info->key_found);
        rv = CMD_FAIL;
    }
    if (memcmp(&frwrd_lkup_info->lkup_res, &expected_frwrd_lkup_info->lkup_res, sizeof(SOC_PPC_DIAG_FRWRD_LKUP_VALUE)) != 0) {
        cli_out("Unexpected differences in lkup_res\n");
        rv = CMD_FAIL;
    }

    return rv;
}

int
diag_pp_frwrd_lpm_lkup_compare(diag_pp_frwrd_lpm_lkup_fields_t *lpm_lkup_info, diag_pp_frwrd_lpm_lkup_fields_t *expected_lpm_lkup_info) {
    int rv = 0;

    if (lpm_lkup_info->fec_ptr != expected_lpm_lkup_info->fec_ptr) {
        cli_out("Unexpected fec_ptr. Received: %d Expected: %d\n",
                lpm_lkup_info->fec_ptr,
                expected_lpm_lkup_info->fec_ptr);
        rv = CMD_FAIL;
    }
    if (lpm_lkup_info->found != expected_lpm_lkup_info->found) {
        cli_out("Unexpected found. Received: %d Expected: %d\n",
                lpm_lkup_info->found,
                expected_lpm_lkup_info->found);
        rv = CMD_FAIL;
    }

    return rv;
}

int
diag_pp_lem_lkup_info_compare(diag_pp_lem_lkup_info_fields_t *lem_lkup_info, diag_pp_lem_lkup_info_fields_t *expected_lem_lkup_info) {
    int rv = 0;

    if (lem_lkup_info->type != expected_lem_lkup_info->type) {
        cli_out("Unexpected type. Received: %d Expected: %d\n",
                lem_lkup_info->type,
                expected_lem_lkup_info->type);
        rv = CMD_FAIL;
    }
    if (memcmp(&lem_lkup_info->key, &expected_lem_lkup_info->key, sizeof(SOC_PPC_DIAG_LEM_KEY)) != 0) {
        cli_out("Unexpected differences in key\n");
        rv = CMD_FAIL;
    }
    if (memcmp(&lem_lkup_info->value, &expected_lem_lkup_info->value, sizeof(SOC_PPC_DIAG_LEM_VALUE)) != 0) {
        cli_out("Unexpected differences in value\n");
        rv = CMD_FAIL;
    }
    if (lem_lkup_info->valid != expected_lem_lkup_info->valid) {
        cli_out("Unexpected valid. Received: %d Expected: %d\n",
                lem_lkup_info->valid,
                expected_lem_lkup_info->valid);
        rv = CMD_FAIL;
    }

    return rv;
}

int
diag_pp_egress_vlan_edit_info_compare(SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO  *egress_vlan_edit_info, SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *expected_egress_vlan_edit_info) {
    int rv = 0;

    if (egress_vlan_edit_info->key.tag_format != expected_egress_vlan_edit_info->key.tag_format) {
        cli_out("Unexpected tag format. Received: %d Expected: %d\n",
                egress_vlan_edit_info->key.tag_format,
                expected_egress_vlan_edit_info->key.tag_format);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->key.edit_profile != expected_egress_vlan_edit_info->key.edit_profile) {
        cli_out("Unexpected edit profile. Received: %d Expected: %d\n",
                egress_vlan_edit_info->key.edit_profile,
                expected_egress_vlan_edit_info->key.edit_profile);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->cmd_info.tags_to_remove != expected_egress_vlan_edit_info->cmd_info.tags_to_remove) {
        cli_out("Unexpected number of tags to remove. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.tags_to_remove,
                expected_egress_vlan_edit_info->cmd_info.tags_to_remove);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->cmd_info.cep_editing != expected_egress_vlan_edit_info->cmd_info.cep_editing) {
        cli_out("Unexpected cep editing. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.cep_editing,
                expected_egress_vlan_edit_info->cmd_info.cep_editing);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->cmd_info.outer_tag.tpid_index != expected_egress_vlan_edit_info->cmd_info.outer_tag.tpid_index) {
        cli_out("Unexpected outer tag tpid index. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.outer_tag.tpid_index,
                expected_egress_vlan_edit_info->cmd_info.outer_tag.tpid_index);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->cmd_info.outer_tag.vid_source != expected_egress_vlan_edit_info->cmd_info.outer_tag.vid_source) {
        cli_out("Unexpected outer tag vid source. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.outer_tag.vid_source,
                expected_egress_vlan_edit_info->cmd_info.outer_tag.vid_source);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->cmd_info.outer_tag.pcp_dei_source != expected_egress_vlan_edit_info->cmd_info.outer_tag.pcp_dei_source) {
        cli_out("Unexpected outer tag pcp dei source. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.outer_tag.pcp_dei_source,
                expected_egress_vlan_edit_info->cmd_info.outer_tag.pcp_dei_source);
    }
    if (egress_vlan_edit_info->cmd_info.inner_tag.tpid_index != expected_egress_vlan_edit_info->cmd_info.inner_tag.tpid_index) {
        cli_out("Unexpected inner tag tpid index. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.inner_tag.tpid_index,
                expected_egress_vlan_edit_info->cmd_info.inner_tag.tpid_index);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->cmd_info.inner_tag.vid_source != expected_egress_vlan_edit_info->cmd_info.inner_tag.vid_source) {
        cli_out("Unexpected inner tag vid source. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.inner_tag.vid_source,
                expected_egress_vlan_edit_info->cmd_info.inner_tag.vid_source);
    }
    if (egress_vlan_edit_info->cmd_info.inner_tag.pcp_dei_source != expected_egress_vlan_edit_info->cmd_info.inner_tag.pcp_dei_source) {
        cli_out("Unexpected inner tag pcp dei source. Received: %d Expected: %d\n",
                egress_vlan_edit_info->cmd_info.inner_tag.pcp_dei_source,
                expected_egress_vlan_edit_info->cmd_info.inner_tag.pcp_dei_source);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->ac_tag.vid != expected_egress_vlan_edit_info->ac_tag.vid) {
        cli_out("Unexpected vid for outer out ac tag. Received: %d Expected: %d\n",
                egress_vlan_edit_info->ac_tag.vid,
                expected_egress_vlan_edit_info->ac_tag.vid);
        rv = CMD_FAIL;
    }
    if (egress_vlan_edit_info->ac_tag2.vid != expected_egress_vlan_edit_info->ac_tag2.vid) {
        cli_out("Unexpected vid for innter out ac tag. Received: %d Expected: %d\n",
                egress_vlan_edit_info->ac_tag2.vid,
                expected_egress_vlan_edit_info->ac_tag2.vid);
        rv = CMD_FAIL;
    }

    return rv;
}

int
diag_pp_received_packet_bridge_test(int unit, int port, int core_id) {
    int rv = 0;
    diag_pp_received_packets_fields_t received_pkt_info;
    diag_pp_received_packets_fields_t expected_pkt_info;
    int   expected_core;  
    SOC_PPC_DIAG_RESULT  ret_val;

    /* Initialize expected packet info */
    expected_pkt_info.packet_qual   = 0;
    expected_pkt_info.pp_context    = SOC_TMC_PORT_HEADER_TYPE_ETH;
    expected_pkt_info.sys_id        = port;
    expected_pkt_info.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
    rv = soc_port_sw_db_local_to_pp_port_get(unit, expected_pkt_info.sys_id, &expected_pkt_info.in_pp_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_bridge_test() local_to_pp Failed:\n");
        return rv;
    }
    rv = soc_port_sw_db_local_to_tm_port_get(unit, DIAG_PP_TM_PORT(unit, port), &expected_pkt_info.in_tm_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_bridge_test() local_to_pp Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_received_packet_get(unit, core_id, &received_pkt_info, &ret_val)) < 0) {
        cli_out("Error: diag_pp_received_packet_get() Failed:\n");
        return rv;
    }

    if (ret_val != SOC_PPC_DIAG_OK) {
        cli_out("Error: Packet not found\n");
        return rv;
    }

    if ((rv = diag_pp_received_packet_compare(&received_pkt_info, &expected_pkt_info)) < 0) {
        cli_out("Error: diag_pp_received_packet_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_parsing_info_bridge_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_parsing_info_fields_t received_pars_info;
    diag_pp_parsing_info_fields_t expected_pars_info;

    /* Initialize expected parsing info */
    SOC_PPC_DIAG_PARSING_L2_INFO_clear(&expected_pars_info.hdrs_stack[0].eth);

    if((rv = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ETH, &expected_pars_info.pfc_hw, NULL)) < 0) {
        cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
        return rv;
    }
    expected_pars_info.hdrs_stack[0].hdr_offset = 0;
    expected_pars_info.hdrs_stack[0].hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
    expected_pars_info.hdrs_stack[0].eth.encap_type = SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2;
    expected_pars_info.hdrs_stack[0].eth.next_prtcl = SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.is_outer_prio = 0;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
    expected_pars_info.hdrs_stack[0].eth.vlan_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].tpid = 0x8100;
    expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].vid = 20;

    if ((rv = diag_pp_parsing_info_get(unit, core_id, &received_pars_info)) < 0) {
        cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_parsing_info_compare(&received_pars_info, &expected_pars_info, 1)) < 0) {
        cli_out("Error: diag_pp_parsing_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_termination_info_bridge_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_termination_info_fields_t received_term_info;
    diag_pp_termination_info_fields_t expected_term_info;

    /* Initialize expected termination info */
    expected_term_info.term_type  = SOC_PPC_PKT_TERM_TYPE_NONE;

    if ((rv = diag_pp_termination_info_get(unit, core_id, &received_term_info)) < 0) {
        cli_out("Error: diag_pp_termination_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_termination_info_compare(&received_term_info, &expected_term_info)) < 0) {
        cli_out("Error: diag_pp_termination_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_vlan_edit_info_bridge_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_vlan_edit_info_fields_t received_vlan_edit_info;
    diag_pp_vlan_edit_info_fields_t expected_vlan_edit_info;

    /* Initialize expected vlan edit info */
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info.ac_tag));
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info.ac_tag2));
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&(expected_vlan_edit_info.cmd_info));
    expected_vlan_edit_info.cmd_id  = (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) ? 5 : 0;
    expected_vlan_edit_info.ac_tag.vid = 0;
    expected_vlan_edit_info.ac_tag.tpid = 0;
    expected_vlan_edit_info.ac_tag.pcp = 0;
    expected_vlan_edit_info.ac_tag.dei = 0;
    expected_vlan_edit_info.ac_tag2.vid = 0;
    expected_vlan_edit_info.ac_tag2.tpid = 0;
    expected_vlan_edit_info.ac_tag2.pcp = 0;
    expected_vlan_edit_info.ac_tag2.dei = 0;
    expected_vlan_edit_info.cmd_info.tags_to_remove = 0;
    expected_vlan_edit_info.cmd_info.tpid_profile = 0;
    expected_vlan_edit_info.cmd_info.inner_tag.tpid_index = 0;
    expected_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
    expected_vlan_edit_info.cmd_info.outer_tag.tpid_index = 0;
    expected_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info.cmd_info.outer_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;

    if ((rv = diag_pp_vlan_edit_info_get(unit, core_id, &received_vlan_edit_info)) < 0) {
        cli_out("Error: diag_pp_vlan_edit_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_vlan_edit_info_compare(&received_vlan_edit_info, &expected_vlan_edit_info)) < 0) {
        cli_out("Error: diag_pp_vlan_edit_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_pkt_tm_info_bridge_test(int unit, int src_id, int dest_id, int core_id) {
    int rv = 0;
    int tm_port;
    bcm_gport_t gport;
    diag_pp_pkt_tm_info_fields_t received_pkt_tm_info;
    diag_pp_pkt_tm_info_fields_t expected_pkt_tm_info;

    /* Initialize expected termination info */
    SOC_PPC_FRWRD_DECISION_INFO_clear(&(expected_pkt_tm_info.frwrd_decision));
    expected_pkt_tm_info.frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_pkt_tm_info.frwrd_decision.dest_id = dest_id;
    expected_pkt_tm_info.frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
    expected_pkt_tm_info.frwrd_decision.additional_info.outlif.val = 0;
    expected_pkt_tm_info.tc = 0;
    expected_pkt_tm_info.dp = 1;
    expected_pkt_tm_info.dp_meter_cmd = 0;
    expected_pkt_tm_info.cud = 0;
    if ((rv = bcm_port_gport_get(unit, src_id, &gport)) < 0) {
        cli_out("Error: bcm_port_gport_get() Failed:\n");
        return rv;
    }
    tm_port = BCM_GPORT_MODPORT_PORT_GET(gport);
    expected_pkt_tm_info.eth_meter_ptr = tm_port*5 + 1; /* 5 meters associated with each port */
    expected_pkt_tm_info.ecn_capable = 0;
    expected_pkt_tm_info.cni = 0;
    expected_pkt_tm_info.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
    expected_pkt_tm_info.st_vsq_ptr = 0;
    if (SOC_IS_ARADPLUS(unit))                 {
        expected_pkt_tm_info.lag_lb_key = 257;
    }                                          else                                       {
        expected_pkt_tm_info.lag_lb_key = 4885;
    }
    expected_pkt_tm_info.ignore_cp = 1;
    expected_pkt_tm_info.snoop_id = 0;


    if ((rv = diag_pp_pkt_tm_info_get(unit, core_id, &received_pkt_tm_info)) < 0) {
        cli_out("Error: diag_pp_pkt_tm_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_pkt_tm_info_compare(&received_pkt_tm_info, &expected_pkt_tm_info)) < 0) {
        cli_out("Error: diag_pp_pkt_tm_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_lif_lkup_info_bridge_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_lif_lkup_info_fields_t received_lif_lkup_info;
    diag_pp_lif_lkup_info_fields_t expected_lif_lkup_info;
    SOC_PPC_DIAG_DB_USE_INFO *db_info = NULL;

    /* Initialize DB info */
    db_info = sal_alloc(sizeof(SOC_PPC_DIAG_DB_USE_INFO), "diag_pp_lif_lkup_info_bridge_test.db_info");
    if (!db_info) {
        cli_out("Memory allocation failure.\n");
        return -1;
    }
    SOC_PPC_DIAG_DB_USE_INFO_clear(db_info);
    db_info->lkup_num = 1;
    db_info->bank_id = 0;

    /* Initialize expected info */
    memset(&expected_lif_lkup_info, 0, sizeof(diag_pp_lif_lkup_info_fields_t));
    SOC_PPC_DIAG_LIF_KEY_clear(&expected_lif_lkup_info.key);
    SOC_PPC_L2_LIF_AC_KEY_clear(&expected_lif_lkup_info.key.ac);
    expected_lif_lkup_info.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;
    expected_lif_lkup_info.found = 1;
    expected_lif_lkup_info.value.ac.service_type = SOC_PPC_L2_LIF_AC_SERVICE_TYPE_MP;
    expected_lif_lkup_info.value.ac.vsid = 30;
    expected_lif_lkup_info.value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;
    expected_lif_lkup_info.value.ac.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
    expected_lif_lkup_info.value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    expected_lif_lkup_info.base_index = DIAG_PP_DEAFULT_INGRESS_ONLY_GLOBAL_INLIF(unit);

    if ((rv = diag_pp_lif_lkup_info_get(unit, core_id, &received_lif_lkup_info, db_info)) < 0) {
        cli_out("Error: diag_pp_lif_lkup_info_get() Failed:\n");
        sal_free(db_info);
        return rv;
    }

    if ((rv = diag_pp_lif_lkup_info_compare(&received_lif_lkup_info, &expected_lif_lkup_info)) < 0) {
        cli_out("Error: diag_pp_lif_lkup_info_compare() Failed:\n");
        sal_free(db_info);
        return rv;
    }

    sal_free(db_info);
    return rv;
}

int
diag_pp_frwrd_trace_info_bridge_test(int unit, int dest_id, int core_id) {
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO received_frwrd_trace_info;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO expected_frwrd_trace_info;
    SOC_PPC_DIAG_RESULT  ret_val;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(&expected_frwrd_trace_info);
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.dest_id = dest_id;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = 3;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.dest_id = dest_id;

    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.frwrd_action_strength = 3;
    expected_frwrd_trace_info.trap_qual[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC] = 1;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.dest_id = dest_id;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.dest_id = dest_id;


    if ((rv = diag_pp_frwrd_trace_info_get(unit, core_id, &received_frwrd_trace_info, &ret_val)) < 0) {
        cli_out("Error: diag_pp_frwrd_trace_info_get() Failed:\n");
        return rv;
    }
    if (ret_val != SOC_PPC_DIAG_OK) {
        return rv;
    }

    if ((rv = diag_pp_frwrd_trace_info_compare(&received_frwrd_trace_info, &expected_frwrd_trace_info)) < 0) {
        cli_out("Error: diag_pp_frwrd_trace_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_traps_info_bridge_test(int unit, int core_id) {
    int rv = 0;
    SOC_PPC_DIAG_TRAPS_INFO received_traps_info;
    SOC_PPC_DIAG_TRAPS_INFO expected_traps_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_TRAPS_INFO_clear(&expected_traps_info);
    expected_traps_info.committed_trap.info.strength = 3;
    expected_traps_info.committed_snoop.code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;
    expected_traps_info.trap_stack[0] = 0x8000;
    expected_traps_info.trap_stack[1] = 0x33480000;

    if ((rv = diag_pp_traps_info_get(unit, core_id, &received_traps_info)) < 0) {
        cli_out("Error: diag_pp_traps_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_traps_info_compare(&received_traps_info, &expected_traps_info)) < 0) {
        cli_out("Error: diag_pp_traps_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

cmd_result_t
diag_pp_encap_info_bridge_test(int unit, int port, int core_id) {
    cmd_result_t result = CMD_OK;
    SOC_PPC_DIAG_ENCAP_INFO *received_encap_info = NULL;
    SOC_PPC_DIAG_ENCAP_INFO *expected_encap_info = NULL;
    int expected_core;
    int rv = 0;

    /* Initialize expected info */
    expected_encap_info = sal_alloc(sizeof(*expected_encap_info), "diag_pp_encap_info_bridge_test.expected_encap_info");
    if(expected_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    SOC_PPC_DIAG_ENCAP_INFO_clear(expected_encap_info);
    expected_encap_info->encap_info[0].entry_type = SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_ARAD;
    expected_encap_info->nof_eeps = 0;
    expected_encap_info->ll_vsi = 30;
    if (SOC_IS_JERICHO_PLUS(unit)) {
        /* jericho+ / Qax, 1st entry is null entry. Therefore, offset of 2 compared to jericho */
        expected_encap_info->out_ac = 8195; 
    } else if (SOC_IS_JERICHO(unit)) {
        expected_encap_info->out_ac = 8193; 
    } else {
        expected_encap_info->out_ac = 4097;
    }
    
    rv = soc_port_sw_db_local_to_pp_port_get(unit, port, &(expected_encap_info->pp_port), &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_encap_info_bridge_test() local_to_pp Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }
    rv = soc_port_sw_db_local_to_tm_port_get(unit, port, &(expected_encap_info->tm_port), &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_encap_info_bridge_test() local_to_tm Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }


    received_encap_info = sal_alloc(sizeof(*received_encap_info), "diag_pp_encap_info_bridge_test.received_encap_info");
    if(received_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    if (diag_pp_encap_info_get(unit, core_id, received_encap_info) != CMD_OK) {
        cli_out("Error: diag_pp_encap_info_get() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }

    if (diag_pp_encap_info_compare(received_encap_info, expected_encap_info)) {
        cli_out("Error: diag_pp_encap_info_compare() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }

exit:
    if(received_encap_info) {
        sal_free(received_encap_info);
    }
    if(expected_encap_info) {
        sal_free(expected_encap_info);
    }
    return result;
}

int
diag_pp_frwrd_lkup_info_bridge_test(int unit, int port, int core_id) {
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO received_frwrd_lkup_info;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO expected_frwrd_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(&expected_frwrd_lkup_info);
    expected_frwrd_lkup_info.frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT;
    expected_frwrd_lkup_info.frwrd_hdr_index = 1;
    expected_frwrd_lkup_info.lkup_key.mact.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[0] = 0x2;
    expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.fid = 30;
    expected_frwrd_lkup_info.key_found = 1;
    expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.dest_id = port;
    expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
    expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.outlif.val = 0;
    expected_frwrd_lkup_info.lkup_res.mact.aging_info.age_status = 6;



    if ((rv = diag_pp_frwrd_lkup_info_get(unit, core_id, &received_frwrd_lkup_info)) < 0) {
        cli_out("Error: diag_pp_frwrd_lkup_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_frwrd_lkup_info_compare(&received_frwrd_lkup_info, &expected_frwrd_lkup_info)) < 0) {
        cli_out("Error: diag_pp_frwrd_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_frwrd_lpm_lkup_bridge_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_frwrd_lpm_lkup_fields_t received_lpm_lkup_info;
    diag_pp_frwrd_lpm_lkup_fields_t expected_lpm_lkup_info;

    /* Initialize expected info */
    expected_lpm_lkup_info.fec_ptr = 0;
    expected_lpm_lkup_info.found = 1;

    if ((rv = diag_pp_frwrd_lpm_lkup_get(unit, core_id, &received_lpm_lkup_info)) < 0) {
        cli_out("Error: diag_pp_frwrd_lpm_lkup_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_frwrd_lpm_lkup_compare(&received_lpm_lkup_info, &expected_lpm_lkup_info)) < 0) {
        cli_out("Error: diag_pp_frwrd_lpm_lkup_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_lem_lkup_info_bridge_test(int unit, int port, int core_id) {
    int rv = 0;
    diag_pp_lem_lkup_info_fields_t received_lem_lkup_info;
    diag_pp_lem_lkup_info_fields_t expected_lem_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_LEM_KEY_clear(&expected_lem_lkup_info.key);
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&(expected_lem_lkup_info.key.mact));
    SOC_PPC_DIAG_LEM_VALUE_clear(&expected_lem_lkup_info.value);
    expected_lem_lkup_info.type = SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT;
    expected_lem_lkup_info.valid = 1;
    expected_lem_lkup_info.key.mact.key_val.mac.mac.address[0] = 0x2;
    expected_lem_lkup_info.key.mact.key_val.mac.fid = 30;
    expected_lem_lkup_info.key.mact.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.dest_id = port;
    expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
    expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.outlif.val = 0;
    expected_lem_lkup_info.value.mact.aging_info.age_status = 0;

    if ((rv = diag_pp_lem_lkup_info_get(unit, core_id, &received_lem_lkup_info)) < 0) {
        cli_out("Error: diag_pp_lem_lkup_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_lem_lkup_info_compare(&received_lem_lkup_info, &expected_lem_lkup_info)) < 0) {
        cli_out("Error: diag_pp_lem_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_egress_vlan_edit_info_bridge_test(int unit, int core_id) {
    int rv = 0;
    SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO received_egress_vlan_edit_info;
    SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO expected_egress_vlan_edit_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(&expected_egress_vlan_edit_info);
    /* command key */
    expected_egress_vlan_edit_info.key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    expected_egress_vlan_edit_info.key.edit_profile = 2;
    /* command information */
    expected_egress_vlan_edit_info.cmd_info.tags_to_remove = 1;
    expected_egress_vlan_edit_info.cmd_info.cep_editing = 0;
    expected_egress_vlan_edit_info.cmd_info.outer_tag.tpid_index = 0;
    expected_egress_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0;
    expected_egress_vlan_edit_info.cmd_info.outer_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
    expected_egress_vlan_edit_info.cmd_info.inner_tag.tpid_index = 0;
    expected_egress_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_egress_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
    /* outer out ac tag */
    expected_egress_vlan_edit_info.ac_tag.vid = 20;
    /* inner out ac tag */
    expected_egress_vlan_edit_info.ac_tag2.vid = 0;

    if ((rv = diag_pp_egress_vlan_edit_info_get(unit, core_id, &received_egress_vlan_edit_info)) < 0) {
        cli_out("Error: diag_pp_egress_vlan_edit_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_egress_vlan_edit_info_compare(&received_egress_vlan_edit_info, &expected_egress_vlan_edit_info)) < 0) {
        cli_out("Error: diag_pp_egress_vlan_edit_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_received_packet_router_test(int unit, int in_port, int core_id) {
    int rv = 0;
    diag_pp_received_packets_fields_t received_pkt_info;
    diag_pp_received_packets_fields_t expected_pkt_info;
    int        expected_core;
    SOC_PPC_DIAG_RESULT     ret_val;


    /* Initialize expected packet info */
    expected_pkt_info.packet_qual   = 0;
    expected_pkt_info.pp_context    = SOC_TMC_PORT_HEADER_TYPE_ETH;
    expected_pkt_info.sys_id        = in_port;
    expected_pkt_info.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;

    rv = soc_port_sw_db_local_to_pp_port_get(unit, expected_pkt_info.sys_id, &expected_pkt_info.in_pp_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_router_test() local_to_pp Failed:\n");
        return rv;
    }
    rv = soc_port_sw_db_local_to_pp_port_get(unit, DIAG_PP_TM_PORT(unit, in_port), &expected_pkt_info.in_tm_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_router_test() local_to_pp Failed:\n");
        return rv;
    }


    if ((rv = diag_pp_received_packet_get(unit, core_id, &received_pkt_info, &ret_val)) < 0) {
        cli_out("Error: diag_pp_received_packet_get() Failed:\n");
        return rv;
    }
    if (ret_val != SOC_PPC_DIAG_OK) {
        return rv;
    }


    if ((rv = diag_pp_received_packet_compare(&received_pkt_info, &expected_pkt_info)) < 0) {
        cli_out("Error: diag_pp_received_packet_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_parsing_info_router_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_parsing_info_fields_t received_pars_info;
    diag_pp_parsing_info_fields_t expected_pars_info;

    /* Initialize expected packet info */
    /* ETH header */
    SOC_PPC_DIAG_PARSING_L2_INFO_clear(&expected_pars_info.hdrs_stack[0].eth);

    if((rv = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_IPV4_ETH, &expected_pars_info.pfc_hw, NULL)) < 0) {
        cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
        return rv;
    }
    expected_pars_info.hdrs_stack[0].hdr_offset = 0;
    expected_pars_info.hdrs_stack[0].hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
    expected_pars_info.hdrs_stack[0].eth.encap_type = SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2;
    expected_pars_info.hdrs_stack[0].eth.next_prtcl = SOC_PPC_L2_NEXT_PRTCL_TYPE_IPV4;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.is_outer_prio = 0;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
    expected_pars_info.hdrs_stack[0].eth.vlan_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].tpid = 0x8100;
    expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].vid = 15;
    /* IPv4 header */
    SOC_PPC_DIAG_PARSING_IP_INFO_clear(&expected_pars_info.hdrs_stack[1].ip);
    expected_pars_info.hdrs_stack[1].hdr_offset = 18;
    expected_pars_info.hdrs_stack[1].hdr_type = SOC_PPC_PKT_HDR_TYPE_IPV4;
    expected_pars_info.hdrs_stack[1].ip.next_prtcl = SOC_PPC_L3_NEXT_PRTCL_TYPE_TCP;
    expected_pars_info.hdrs_stack[1].ip.is_mc = 0;
    expected_pars_info.hdrs_stack[1].ip.is_fragmented = 0;
    expected_pars_info.hdrs_stack[1].ip.hdr_err = 0;

    if ((rv = diag_pp_parsing_info_get(unit, core_id, &received_pars_info)) < 0) {
        cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_parsing_info_compare(&received_pars_info, &expected_pars_info, 2)) < 0) {
        cli_out("Error: diag_pp_parsing_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_termination_info_router_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_termination_info_fields_t received_term_info;
    diag_pp_termination_info_fields_t expected_term_info;

    /* Initialize expected termination info */
    expected_term_info.term_type  = SOC_PPC_PKT_TERM_TYPE_ETH;
    expected_term_info.frwrd_type = SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC;

    if ((rv = diag_pp_termination_info_get(unit, core_id, &received_term_info)) < 0) {
        cli_out("Error: diag_pp_termination_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_termination_info_compare(&received_term_info, &expected_term_info)) < 0) {
        cli_out("Error: diag_pp_termination_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_vlan_edit_info_router_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_vlan_edit_info_fields_t received_vlan_edit_info;
    diag_pp_vlan_edit_info_fields_t expected_vlan_edit_info;
    int adv_mode;

    /* Initialize expected vlan edit info */
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info.ac_tag));
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info.ac_tag2));
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&(expected_vlan_edit_info.cmd_info));
    expected_vlan_edit_info.cmd_id  = 56;
    expected_vlan_edit_info.ac_tag.vid = 15;
    expected_vlan_edit_info.ac_tag.tpid = 0;
    expected_vlan_edit_info.ac_tag.pcp = 0;
    expected_vlan_edit_info.ac_tag.dei = 0;
    expected_vlan_edit_info.ac_tag2.vid = 0;
    expected_vlan_edit_info.ac_tag2.tpid = 0;
    expected_vlan_edit_info.ac_tag2.pcp = 0;
    expected_vlan_edit_info.ac_tag2.dei = 0;
    expected_vlan_edit_info.cmd_info.tags_to_remove = 0;
    expected_vlan_edit_info.cmd_info.tpid_profile = 0;
    expected_vlan_edit_info.cmd_info.inner_tag.tpid_index = 0;
    expected_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
    expected_vlan_edit_info.cmd_info.outer_tag.tpid_index = 0;
    expected_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info.cmd_info.outer_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;

    if (SOC_IS_JERICHO(unit)) {
        rv = bcm_switch_control_get(0, bcmSwitchPortVlanTranslationAdvanced, &adv_mode);
        if (rv) {
            cli_out("Error: bcm_switch_control_get() Failed:\n"); 
            return rv;
        }

        if (adv_mode) {
            expected_vlan_edit_info.cmd_id  = 0;
            expected_vlan_edit_info.ac_tag.vid = 0;
        }
    }

    if ((rv = diag_pp_vlan_edit_info_get(unit, core_id, &received_vlan_edit_info)) < 0) {
        cli_out("Error: diag_pp_vlan_edit_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_vlan_edit_info_compare(&received_vlan_edit_info, &expected_vlan_edit_info)) < 0) {
        cli_out("Error: diag_pp_vlan_edit_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_pkt_tm_info_router_test(int unit, int port, int core_id) {
    int rv = 0;
    diag_pp_pkt_tm_info_fields_t received_pkt_tm_info;
    diag_pp_pkt_tm_info_fields_t expected_pkt_tm_info;

    /* Initialize expected termination info */
    SOC_PPC_FRWRD_DECISION_INFO_clear(&(expected_pkt_tm_info.frwrd_decision));
    expected_pkt_tm_info.frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_pkt_tm_info.frwrd_decision.dest_id = port;
    expected_pkt_tm_info.frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
    expected_pkt_tm_info.frwrd_decision.additional_info.outlif.val = 0;
    expected_pkt_tm_info.tc = 0;
    expected_pkt_tm_info.dp = 1;
    expected_pkt_tm_info.dp_meter_cmd = 0;
    expected_pkt_tm_info.cud = 100;
    expected_pkt_tm_info.eth_meter_ptr = 320;
    /* ECN introduced in ARAD_B0*/
    if (SOC_IS_ARAD_A0(unit)) {
        expected_pkt_tm_info.ecn_capable = 1;
        expected_pkt_tm_info.cni = 1;
    } else {
        expected_pkt_tm_info.ecn_capable = 0;
        expected_pkt_tm_info.cni = 0;
    }
    expected_pkt_tm_info.st_vsq_ptr = 0;
    if (SOC_IS_ARADPLUS(unit))                 {
        expected_pkt_tm_info.lag_lb_key = 257;
    }                                          else                                       {
        expected_pkt_tm_info.lag_lb_key = 4885;
    }
    expected_pkt_tm_info.ignore_cp = 0;
    expected_pkt_tm_info.snoop_id = 0;


    if ((rv = diag_pp_pkt_tm_info_get(unit, core_id, &received_pkt_tm_info)) < 0) {
        cli_out("Error: diag_pp_pkt_tm_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_pkt_tm_info_compare(&received_pkt_tm_info, &expected_pkt_tm_info)) < 0) {
        cli_out("Error: diag_pp_pkt_tm_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_pkt_tm_info_meter_test(int unit, int core_id, int meter_id_1, int meter_id_2) {
    int rv = 0;
    diag_pp_pkt_tm_info_fields_t received_pkt_tm_info;
    diag_pp_pkt_tm_info_fields_t expected_pkt_tm_info;

    /* Initialize expected termination info */
    SOC_PPC_FRWRD_DECISION_INFO_clear(&(expected_pkt_tm_info.frwrd_decision));
    expected_pkt_tm_info.meter1 = meter_id_1;
	expected_pkt_tm_info.meter2 = meter_id_2;

    if ((rv = diag_pp_pkt_tm_info_get(unit, core_id, &received_pkt_tm_info)) < 0) {
        cli_out("Error: diag_pp_pkt_tm_info_get() Failed:\n");
        return rv;
    }

    if ((rv = diag_pp_pkt_tm_info_meter_compare(&received_pkt_tm_info, &expected_pkt_tm_info)) < 0) {
        cli_out("Error: diag_pp_pkt_tm_info_meter_compare() Failed:\n");
        return rv;
    }

    return rv;
}


int
diag_pp_lif_lkup_info_router_test(int unit, int core_id) {
    int rv = 0;
    diag_pp_lif_lkup_info_fields_t received_lif_lkup_info;
    diag_pp_lif_lkup_info_fields_t expected_lif_lkup_info;
    SOC_PPC_DIAG_DB_USE_INFO *db_info = NULL;

    /* Initialize DB info */
    db_info = sal_alloc(sizeof(SOC_PPC_DIAG_DB_USE_INFO), "diag_pp_lif_lkup_info_router_test.db_info");
    if(!db_info) {
        cli_out("Memory allocation failure.\n");
        return -1;
    }

    SOC_PPC_DIAG_DB_USE_INFO_clear(db_info);
    db_info->lkup_num = 0;
    db_info->bank_id = 3;

    /* Initialize expected termination info */
    memset(&expected_lif_lkup_info, 0, sizeof(diag_pp_lif_lkup_info_fields_t));
    SOC_PPC_DIAG_LIF_KEY_clear(&expected_lif_lkup_info.key);
    SOC_PPC_RIF_IP_TERM_KEY_clear(&expected_lif_lkup_info.key.ip_tunnel);
    expected_lif_lkup_info.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;
    expected_lif_lkup_info.key.ac.raw_key=200;
    expected_lif_lkup_info.found = 1;
    expected_lif_lkup_info.base_index = 0;
    expected_lif_lkup_info.opcode_id = 0;
    expected_lif_lkup_info.value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    expected_lif_lkup_info.value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;

    if((rv = diag_pp_lif_lkup_info_get(unit, core_id, &received_lif_lkup_info, db_info )) < 0)
    { 
        cli_out("Error: diag_pp_lif_lkup_info_get() Failed:\n");
        sal_free(db_info);
        return rv;
    } 

    if((rv = diag_pp_lif_lkup_info_compare(&received_lif_lkup_info, &expected_lif_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_lif_lkup_info_compare() Failed:\n");
        sal_free(db_info);
        return rv;
    }

    sal_free(db_info);
    return rv;
}

int
diag_pp_frwrd_trace_info_router_test(int unit, int port, int core_id)
{
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO received_frwrd_trace_info;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO expected_frwrd_trace_info;
    SOC_PPC_DIAG_RESULT  ret_val;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(&expected_frwrd_trace_info);
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = 3;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);

    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.frwrd_action_strength = 3;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.dest_id = port;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.type = SOC_PPC_NOF_EEI_TYPES_PB;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.val.outlif = DIAG_PP_DEFAULT_ROUTER_AND_MPLS_GLOBAL_OUTLIF(unit);
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].outlif = 100;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.dest_id = port;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.type = SOC_PPC_NOF_EEI_TYPES_PB;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.val.outlif = DIAG_PP_DEFAULT_ROUTER_AND_MPLS_GLOBAL_OUTLIF(unit);
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].outlif = 100;

    if((rv = diag_pp_frwrd_trace_info_get(unit, core_id, &received_frwrd_trace_info, &ret_val)) < 0)
    { 
        cli_out("Error: diag_pp_frwrd_trace_info_get() Failed:\n");
        return rv;
    } 

    if (ret_val != SOC_PPC_DIAG_OK) {
       cli_out("Error: Packet not found.\n");
       return rv;
    }
    if ((rv = diag_pp_frwrd_trace_info_compare(&received_frwrd_trace_info, &expected_frwrd_trace_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_trace_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

cmd_result_t
diag_pp_traps_info_router_test(int unit, int core_id)
{
    cmd_result_t result = CMD_OK;
    SOC_PPC_DIAG_TRAPS_INFO *received_traps_info = NULL;
    SOC_PPC_DIAG_TRAPS_INFO *expected_traps_info = NULL;

    expected_traps_info = sal_alloc(sizeof(*expected_traps_info), "diag_pp_traps_info_router_test.expected_traps_info");
    if(expected_traps_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    /* Initialize expected info */
    SOC_PPC_DIAG_TRAPS_INFO_clear(expected_traps_info);
    expected_traps_info->committed_trap.info.strength = 3;
    expected_traps_info->committed_snoop.code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;
    expected_traps_info->trap_stack[0] = 0x8000;
    expected_traps_info->trap_stack[1] = 0x12400000;
    expected_traps_info->trap_stack[2] = 0x2000;
    expected_traps_info->trap_stack[5] = 0x20000;

    received_traps_info = sal_alloc(sizeof(*received_traps_info), "diag_pp_traps_info_router_test.received_traps_info");
    if(received_traps_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    result = diag_pp_traps_info_get(unit, core_id, received_traps_info);
    if(result != CMD_OK)
    { 
        cli_out("Error: diag_pp_traps_info_get() Failed:\n");
        goto exit;
    } 

    result = diag_pp_traps_info_compare(received_traps_info, expected_traps_info);
    if(result != CMD_OK)
    {
        cli_out("Error: diag_pp_traps_info_compare() Failed:\n");
        goto exit;
    }

exit:
    if(received_traps_info) {
        sal_free(received_traps_info);
    }
    if(expected_traps_info) {
        sal_free(expected_traps_info);
    }
    return result;
}

int
diag_pp_encap_info_router_test(int unit, int port, int core_id)
{
    int rv = 0;
    int enacp_info_port = port;
    int expected_core;
    cmd_result_t result = CMD_OK;

    SOC_PPC_DIAG_ENCAP_INFO *received_encap_info = NULL;
    SOC_PPC_DIAG_ENCAP_INFO *expected_encap_info = NULL;

    /* Initialize expected info */
    expected_encap_info = sal_alloc(sizeof(*expected_encap_info), "diag_pp_encap_info_router_test.expected_encap_info");
    if(expected_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }

    SOC_PPC_DIAG_ENCAP_INFO_clear(expected_encap_info);

    expected_encap_info->nof_eeps = 2;
    expected_encap_info->eep[0] = 100;
    expected_encap_info->encap_info[0].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI;
    expected_encap_info->encap_info[0].entry_val.vsi_info.out_vsi = 100;
    if (SOC_IS_JERICHO_PLUS(unit)) {
        expected_encap_info->encap_info[0].entry_val.vsi_info.outrif_profile_index = 1;
    }
    expected_encap_info->eep[1] = DIAG_PP_FIRST_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
    expected_encap_info->encap_info[1].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP;
    expected_encap_info->encap_info[1].entry_val.ll_info.dest_mac.address[0] = 0x1d;
    expected_encap_info->encap_info[1].entry_val.ll_info.dest_mac.address[1] = 0xcd;
    expected_encap_info->encap_info[1].entry_val.ll_info.out_vid_valid = 1;
    expected_encap_info->encap_info[1].entry_val.ll_info.out_vid = 100;
    expected_encap_info->ll_vsi = 100;
    expected_encap_info->out_ac = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID);
    expected_encap_info->tm_port = port;
    expected_encap_info->pp_port = port;
  
    rv = soc_port_sw_db_local_to_pp_port_get(unit, enacp_info_port, &expected_encap_info->pp_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }
    rv = soc_port_sw_db_local_to_tm_port_get(unit, enacp_info_port, &expected_encap_info->tm_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }

    received_encap_info = sal_alloc(sizeof(*received_encap_info), "diag_pp_encap_info_router_test.received_encap_info");
    if(received_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }

    if((rv = diag_pp_encap_info_get(unit, core_id, received_encap_info)) != CMD_OK)
    { 
        cli_out("Error: diag_pp_encap_info_get() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    } 

    if((rv = diag_pp_encap_info_compare(received_encap_info, expected_encap_info)) < 0)
    {
        cli_out("Error: diag_pp_encap_info_compare() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }

exit:
    if(received_encap_info) {
        sal_free(received_encap_info);
    }
    if(expected_encap_info) {
        sal_free(expected_encap_info);
    }
    return result;

}

int
diag_pp_frwrd_lkup_info_router_test(int unit, int core_id)
{
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO received_frwrd_lkup_info;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO expected_frwrd_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(&expected_frwrd_lkup_info);
    expected_frwrd_lkup_info.frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC;
    expected_frwrd_lkup_info.frwrd_hdr_index = 2;
    expected_frwrd_lkup_info.lkup_key.ipv4_uc.key.subnet.ip_address = 0x7FFFFF03;
    expected_frwrd_lkup_info.lkup_key.ipv4_uc.key.subnet.prefix_len = 32;
    expected_frwrd_lkup_info.key_found = 1;
    expected_frwrd_lkup_info.lkup_res.host_info.frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);

    if((rv = diag_pp_frwrd_lkup_info_get(unit, core_id, &received_frwrd_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_lkup_info_get() Failed:\n");
        return rv;
    }

    if((rv = diag_pp_frwrd_lkup_info_compare(&received_frwrd_lkup_info, &expected_frwrd_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_frwrd_lpm_lkup_router_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_frwrd_lpm_lkup_fields_t received_lpm_lkup_info;
    diag_pp_frwrd_lpm_lkup_fields_t expected_lpm_lkup_info;

    /* Initialize expected info */
    expected_lpm_lkup_info.fec_ptr = 1025;
    expected_lpm_lkup_info.found = 1;

    if((rv = diag_pp_frwrd_lpm_lkup_get(unit, core_id, &received_lpm_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_lpm_lkup_get() Failed:\n");
        return rv;
    }

    if((rv = diag_pp_frwrd_lpm_lkup_compare(&received_lpm_lkup_info, &expected_lpm_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_lpm_lkup_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_lem_lkup_info_router_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_lem_lkup_info_fields_t received_lem_lkup_info;
    diag_pp_lem_lkup_info_fields_t expected_lem_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_LEM_KEY_clear(&expected_lem_lkup_info.key);
    SOC_PPC_DIAG_LEM_VALUE_clear(&expected_lem_lkup_info.value);
    expected_lem_lkup_info.type = SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST;
    expected_lem_lkup_info.valid = 1;
    expected_lem_lkup_info.key.host.subnet.subnet.ip_address = 0x7FFFFF03;
    expected_lem_lkup_info.key.host.subnet.subnet.prefix_len = 32;
    expected_lem_lkup_info.value.host.frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);

    if((rv = diag_pp_lem_lkup_info_get(unit, core_id, &received_lem_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_lem_lkup_info_get() Failed:\n");
        return rv;
    }

    if((rv = diag_pp_lem_lkup_info_compare(&received_lem_lkup_info, &expected_lem_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_lem_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int 
diag_pp_egress_vlan_edit_info_router_test(int unit, int core_id)
{
   int rv = 0;
   SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO received_egress_vlan_edit_info;
   SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO expected_egress_vlan_edit_info;

   /* Initialize expected info */
   SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(&expected_egress_vlan_edit_info);
   /* command key */
   expected_egress_vlan_edit_info.key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
   expected_egress_vlan_edit_info.key.edit_profile = 0;
   /* command information */
   expected_egress_vlan_edit_info.cmd_info.tags_to_remove = 0;
   expected_egress_vlan_edit_info.cmd_info.cep_editing = 0;
   expected_egress_vlan_edit_info.cmd_info.outer_tag.tpid_index = 0;
   expected_egress_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_VSI;
   expected_egress_vlan_edit_info.cmd_info.outer_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
   expected_egress_vlan_edit_info.cmd_info.inner_tag.tpid_index = 0;
   expected_egress_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY;
   expected_egress_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
   /* outer out ac tag */
   expected_egress_vlan_edit_info.ac_tag.vid = 0;
   /* inner out ac tag */
   expected_egress_vlan_edit_info.ac_tag2.vid = 0;

   if ((rv = diag_pp_egress_vlan_edit_info_get(unit, core_id, &received_egress_vlan_edit_info)) < 0)
   {
        cli_out("Error: diag_pp_egress_vlan_edit_info_get() Failed:\n");
        return rv;
   }

   if((rv = diag_pp_egress_vlan_edit_info_compare(&received_egress_vlan_edit_info, &expected_egress_vlan_edit_info)) < 0)
   {
       cli_out("Error: diag_pp_egress_vlan_edit_info_compare() Failed:\n");
       return rv;
   }

   return rv;
}

int
diag_pp_received_packet_mpls_test(int unit, int in_port, int core_id)
{
    int rv = 0;
    diag_pp_received_packets_fields_t received_pkt_info;
    diag_pp_received_packets_fields_t expected_pkt_info;
    int expected_core;
    SOC_PPC_DIAG_RESULT      ret_val;

    /* Initialize expected packet info */
    expected_pkt_info.in_tm_port    = 0;
    expected_pkt_info.packet_qual   = 0;
    expected_pkt_info.pp_context    = SOC_TMC_PORT_HEADER_TYPE_ETH;
    expected_pkt_info.sys_id        = in_port;
    expected_pkt_info.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
    
    rv = soc_port_sw_db_local_to_pp_port_get(unit, expected_pkt_info.sys_id, &expected_pkt_info.in_pp_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        return rv;
    }
    rv = soc_port_sw_db_local_to_tm_port_get(unit, DIAG_PP_TM_PORT(unit, in_port), &expected_pkt_info.in_tm_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        return rv;
    }

    if((rv = diag_pp_received_packet_get(unit, core_id, &received_pkt_info, &ret_val )) < 0)
    {
        cli_out("Error: diag_pp_received_packet_get() Failed:\n");
        return rv;
    }

    if (ret_val != SOC_PPC_DIAG_OK) {
        return rv;
    }

    if((rv = diag_pp_received_packet_compare(&received_pkt_info, &expected_pkt_info)) < 0)
    {
        cli_out("Error: diag_pp_received_packet_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_parsing_info_mpls_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_parsing_info_fields_t received_pars_info;
    diag_pp_parsing_info_fields_t expected_pars_info;

    /* Initialize expected packet info */
    /* ETH header */
    SOC_PPC_DIAG_PARSING_L2_INFO_clear(&expected_pars_info.hdrs_stack[0].eth);

    if((rv = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_MPLS2_ETH, &expected_pars_info.pfc_hw, NULL)) < 0) {
        cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
        return rv;
    }
    expected_pars_info.hdrs_stack[0].hdr_offset = 0;
    expected_pars_info.hdrs_stack[0].hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
    expected_pars_info.hdrs_stack[0].eth.encap_type = SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2;
    expected_pars_info.hdrs_stack[0].eth.next_prtcl = SOC_PPC_L2_NEXT_PRTCL_TYPE_MPLS;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.is_outer_prio = 0;
    expected_pars_info.hdrs_stack[0].eth.tag_fromat.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
    expected_pars_info.hdrs_stack[0].eth.vlan_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].tpid = 0x8100;
    expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].vid = 10;

    /* MPLS header */
    SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(&expected_pars_info.hdrs_stack[1].mpls);
    expected_pars_info.hdrs_stack[1].hdr_offset = 18;
    expected_pars_info.hdrs_stack[1].hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;
    expected_pars_info.hdrs_stack[1].mpls.bos = 0;

    /* MPLS header */
    SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(&expected_pars_info.hdrs_stack[2].mpls);
    expected_pars_info.hdrs_stack[2].hdr_offset = 22;
    expected_pars_info.hdrs_stack[2].hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;
    expected_pars_info.hdrs_stack[2].mpls.bos = 1;

    if((rv = diag_pp_parsing_info_get(unit, core_id, &received_pars_info )) < 0)
    {
        cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
        return rv;
    }

    if((rv = diag_pp_parsing_info_compare(&received_pars_info, &expected_pars_info, 2)) < 0)
    {
        cli_out("Error: diag_pp_parsing_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_termination_info_mpls_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_termination_info_fields_t received_term_info;
    diag_pp_termination_info_fields_t expected_term_info;

    /* Initialize expected termination info */
    expected_term_info.term_type  = SOC_PPC_PKT_TERM_TYPE_MPLS_ETH;
    expected_term_info.frwrd_type = SOC_PPC_PKT_FRWRD_TYPE_MPLS;

    if((rv = diag_pp_termination_info_get(unit, core_id, &received_term_info )) < 0)
    {
        cli_out("Error: diag_pp_termination_info_get() Failed:\n");
        return rv;
    }

    if((rv = diag_pp_termination_info_compare(&received_term_info, &expected_term_info)) < 0)
    {
        cli_out("Error: diag_pp_termination_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_vlan_edit_info_mpls_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_vlan_edit_info_fields_t received_vlan_edit_info;
    diag_pp_vlan_edit_info_fields_t expected_vlan_edit_info;
    int adv_mode;

    /* Initialize expected vlan edit info */
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info.ac_tag));
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info.ac_tag2));
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&(expected_vlan_edit_info.cmd_info));
    expected_vlan_edit_info.cmd_id  = 56;
    expected_vlan_edit_info.ac_tag.vid = 10;
    expected_vlan_edit_info.ac_tag.tpid = 0;
    expected_vlan_edit_info.ac_tag.pcp = 0;
    expected_vlan_edit_info.ac_tag.dei = 0;
    expected_vlan_edit_info.ac_tag2.vid = 0;
    expected_vlan_edit_info.ac_tag2.tpid = 0;
    expected_vlan_edit_info.ac_tag2.pcp = 0;
    expected_vlan_edit_info.ac_tag2.dei = 0;
    expected_vlan_edit_info.cmd_info.tags_to_remove = 0;
    expected_vlan_edit_info.cmd_info.tpid_profile = 0;
    expected_vlan_edit_info.cmd_info.inner_tag.tpid_index = 0;
    expected_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
    expected_vlan_edit_info.cmd_info.outer_tag.tpid_index = 0;
    expected_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info.cmd_info.outer_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;

    if (SOC_IS_JERICHO(unit)) {
        rv = bcm_switch_control_get(0, bcmSwitchPortVlanTranslationAdvanced, &adv_mode);
        if (rv) {
            cli_out("Error: bcm_switch_control_get() Failed:\n"); 
            return rv;
        }

        if (adv_mode) {
            expected_vlan_edit_info.cmd_id  = 0;
            expected_vlan_edit_info.ac_tag.vid = 0;
        }
    }

    if((rv = diag_pp_vlan_edit_info_get(unit, core_id, &received_vlan_edit_info )) < 0)
    {
        cli_out("Error: diag_pp_vlan_edit_info_get() Failed:\n");
        return rv;
    }

    if((rv = diag_pp_vlan_edit_info_compare(&received_vlan_edit_info, &expected_vlan_edit_info)) < 0)
    {
        cli_out("Error: diag_pp_vlan_edit_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_pkt_tm_info_mpls_test(int unit, int port, int core_id)
{
    int rv = 0;
    diag_pp_pkt_tm_info_fields_t received_pkt_tm_info;
    diag_pp_pkt_tm_info_fields_t expected_pkt_tm_info;

    /* Initialize expected termination info */
    SOC_PPC_FRWRD_DECISION_INFO_clear(&(expected_pkt_tm_info.frwrd_decision));
    expected_pkt_tm_info.frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_pkt_tm_info.frwrd_decision.dest_id = port;
    expected_pkt_tm_info.frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_NONE;
    expected_pkt_tm_info.frwrd_decision.additional_info.outlif.val = 0;
    expected_pkt_tm_info.tc = 0;
    expected_pkt_tm_info.dp = 1;
    expected_pkt_tm_info.dp_meter_cmd = 0;
    expected_pkt_tm_info.cud = DIAG_PP_DEFAULT_ROUTER_AND_MPLS_GLOBAL_OUTLIF(unit);
    expected_pkt_tm_info.eth_meter_ptr = 320;
    /* ECN introduced in ARAD_B0*/
    if(SOC_IS_ARAD_A0(unit))
    {
        expected_pkt_tm_info.ecn_capable = 1;
        expected_pkt_tm_info.cni = 1;
    }
    else
    {
        expected_pkt_tm_info.ecn_capable = 0;
        expected_pkt_tm_info.cni = 0;
    }
    expected_pkt_tm_info.st_vsq_ptr = 0;
    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) 
    {
      expected_pkt_tm_info.lag_lb_key = 4885;
    }
    else
    {
      expected_pkt_tm_info.lag_lb_key = 257;
    }
    expected_pkt_tm_info.ignore_cp = 0;
    expected_pkt_tm_info.snoop_id = 0;


    if((rv = diag_pp_pkt_tm_info_get(unit, core_id, &received_pkt_tm_info )) < 0)
    {
        cli_out("Error: diag_pp_pkt_tm_info_get() Failed:\n");
        return rv;
    }

    if((rv = diag_pp_pkt_tm_info_compare(&received_pkt_tm_info, &expected_pkt_tm_info)) < 0)
    {
        cli_out("Error: diag_pp_pkt_tm_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_lif_lkup_info_mpls_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_lif_lkup_info_fields_t received_lif_lkup_info;
    diag_pp_lif_lkup_info_fields_t expected_lif_lkup_info;
    SOC_PPC_DIAG_DB_USE_INFO *db_info = NULL;

    /* Initialize DB info */
    db_info = sal_alloc(sizeof(SOC_PPC_DIAG_DB_USE_INFO), "diag_pp_lif_lkup_info_mpls_test.db_info");
    if(!db_info) {
        cli_out("Memory allocation failure.\n");
        return -1;
    }

    SOC_PPC_DIAG_DB_USE_INFO_clear(db_info);
    db_info->lkup_num = 0;
    db_info->bank_id = 1;

    /* Initialize expected db lif lkup info */
    memset(&expected_lif_lkup_info, 0, sizeof(diag_pp_lif_lkup_info_fields_t));
    SOC_PPC_DIAG_LIF_KEY_clear(&expected_lif_lkup_info.key);
    SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&expected_lif_lkup_info.key.mpls);
    expected_lif_lkup_info.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
    expected_lif_lkup_info.value.mpls.cos_profile = 0;
    expected_lif_lkup_info.found = 1; 
    expected_lif_lkup_info.base_index = DIAG_PP_DEAFULT_INGRESS_LOCAL_LIF(unit);
    expected_lif_lkup_info.opcode_id = 0;

    if((rv = diag_pp_lif_lkup_info_get(unit, core_id, &received_lif_lkup_info, db_info )) < 0)
    {
        cli_out("Error: diag_pp_lif_lkup_info_get() Failed:\n");
        sal_free(db_info);
        return rv;
    }

    if((rv = diag_pp_lif_lkup_info_compare(&received_lif_lkup_info, &expected_lif_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_lif_lkup_info_compare() Failed:\n");
        sal_free(db_info);
        return rv;
    }

    sal_free(db_info);
    return rv;
}

int
diag_pp_frwrd_trace_info_mpls_test(int unit, int port, int core_id)
{
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO received_frwrd_trace_info;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO expected_frwrd_trace_info;
    SOC_PPC_DIAG_RESULT  ret_val;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(&expected_frwrd_trace_info);
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_SWAP;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.additional_info.eei.val.mpls_command.label = 2000;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = 3;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_SWAP;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.additional_info.eei.val.mpls_command.label = 2000;

    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.frwrd_action_strength = 3;

    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.dest_id = port;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].outlif = DIAG_PP_FIRST_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_SWAP;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.val.mpls_command.label = 2000;


    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.dest_id = port;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].outlif = DIAG_PP_FIRST_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_SWAP;
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.val.mpls_command.label = 2000;

    if((rv = diag_pp_frwrd_trace_info_get(unit, core_id, &received_frwrd_trace_info, &ret_val)) < 0)
    { 
        cli_out("Error: diag_pp_frwrd_trace_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) {
       cli_out("Error: Packet not found.\n");
       return rv;
    }

    if((rv = diag_pp_frwrd_trace_info_compare(&received_frwrd_trace_info, &expected_frwrd_trace_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_trace_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_traps_info_mpls_test(int unit, int core_id)
{
    int rv = 0;
    SOC_PPC_DIAG_TRAPS_INFO received_traps_info;
    SOC_PPC_DIAG_TRAPS_INFO expected_traps_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_TRAPS_INFO_clear(&expected_traps_info);
    expected_traps_info.committed_trap.info.strength = 3;
    expected_traps_info.committed_snoop.code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;
    expected_traps_info.trap_stack[0] = 0x8000;
    expected_traps_info.trap_stack[1] = 0x10000000;
    expected_traps_info.trap_stack[2] = 0x2000;
    expected_traps_info.trap_stack[4] = 0x40000000;
    expected_traps_info.trap_stack[5] = 0x1000;

    if((rv = diag_pp_traps_info_get(unit, core_id, &received_traps_info)) < 0)
    { 
        cli_out("Error: diag_pp_traps_info_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_traps_info_compare(&received_traps_info, &expected_traps_info)) < 0)
    {
        cli_out("Error: diag_pp_traps_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

cmd_result_t
diag_pp_encap_info_mpls_test(int unit,int port, int core_id)
{
    cmd_result_t result = CMD_OK;
    soc_error_t rv = SOC_E_NONE;
    SOC_PPC_DIAG_ENCAP_INFO *received_encap_info = NULL;
    SOC_PPC_DIAG_ENCAP_INFO *expected_encap_info = NULL;
    int   expected_core;
    
    expected_encap_info = sal_alloc(sizeof(*expected_encap_info), "diag_pp_encap_info_mpls_test.expected_encap_info");
    if(expected_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    /* Initialize expected info */
    SOC_PPC_DIAG_ENCAP_INFO_clear(expected_encap_info);

    expected_encap_info->mpls_cmd.command = SOC_TMC_MPLS_COMMAND_TYPE_SWAP;
    expected_encap_info->mpls_cmd.label = 2000;
    expected_encap_info->mpls_cmd.push_profile = 0;
    expected_encap_info->nof_eeps = 2;
    expected_encap_info->eep[0] = DIAG_PP_FIRST_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
    expected_encap_info->encap_info[0].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP;
    expected_encap_info->encap_info[0].entry_val.mpls_encap_info.tunnels[0].tunnel_label = 200;
    expected_encap_info->encap_info[0].entry_val.mpls_encap_info.tunnels[0].push_profile = 1;
    expected_encap_info->encap_info[0].entry_val.mpls_encap_info.nof_tunnels = 1;
    expected_encap_info->encap_info[0].entry_val.mpls_encap_info.orientation = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
    expected_encap_info->encap_info[0].entry_val.mpls_encap_info.out_vsi = 10;
    expected_encap_info->encap_info[0].entry_val.mpls_encap_info.oam_lif_set = 0;
    if (SOC_IS_JERICHO(unit)) {
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.outlif_profile = 1;
    }
    expected_encap_info->eep[1] = DIAG_PP_SECOND_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
    expected_encap_info->encap_info[1].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP;
    expected_encap_info->encap_info[1].entry_val.ll_info.dest_mac.address[0] = 0x22;
    expected_encap_info->encap_info[1].entry_val.ll_info.out_vid_valid = 1;
    expected_encap_info->encap_info[1].entry_val.ll_info.out_vid = 10;
    expected_encap_info->ll_vsi = 10;
    expected_encap_info->out_ac = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID);
    rv = soc_port_sw_db_local_to_pp_port_get(unit, port, &expected_encap_info->pp_port, &expected_core);
    if (SOC_FAILURE(rv)) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }
    rv = soc_port_sw_db_local_to_tm_port_get(unit, port, &expected_encap_info->tm_port, &expected_core);
    if (SOC_FAILURE(rv)) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }
  

    received_encap_info = sal_alloc(sizeof(*received_encap_info), "diag_pp_encap_info_mpls_test.received_encap_info");
    if(received_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    result = diag_pp_encap_info_get(unit, core_id, received_encap_info);
    if(result != CMD_OK)
    { 
        cli_out("Error: diag_pp_encap_info_get() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    } 

    result = diag_pp_encap_info_compare(received_encap_info, expected_encap_info);
    if(result != CMD_OK)
    {
        cli_out("Error: diag_pp_encap_info_compare() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }

exit:
    if(received_encap_info) {
        sal_free(received_encap_info);
    }
    if(expected_encap_info) {
        sal_free(expected_encap_info);
    }
    return result;
}

int
diag_pp_frwrd_lkup_info_mpls_test(int unit, int core_id)
{
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO received_frwrd_lkup_info;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO expected_frwrd_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(&expected_frwrd_lkup_info);
    expected_frwrd_lkup_info.frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM;
    expected_frwrd_lkup_info.frwrd_hdr_index = 3;
    expected_frwrd_lkup_info.lkup_key.ilm.in_label = 1000;
    expected_frwrd_lkup_info.lkup_key.ilm.mapped_exp = 0;
    expected_frwrd_lkup_info.lkup_key.ilm.in_local_port = 0;
    expected_frwrd_lkup_info.lkup_key.ilm.inrif = 0;
    expected_frwrd_lkup_info.key_found = 1;
    expected_frwrd_lkup_info.lkup_res.frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    expected_frwrd_lkup_info.lkup_res.frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);
    expected_frwrd_lkup_info.lkup_res.frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
    expected_frwrd_lkup_info.lkup_res.frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_SWAP;
    expected_frwrd_lkup_info.lkup_res.frwrd_decision.additional_info.eei.val.mpls_command.label = 2000;
    expected_frwrd_lkup_info.lkup_res.frwrd_decision.additional_info.eei.val.mpls_command.push_profile = 0;
    if((rv = diag_pp_frwrd_lkup_info_get(unit, core_id, &received_frwrd_lkup_info)) < 0)
    { 
        cli_out("Error: diag_pp_frwrd_lkup_info_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_frwrd_lkup_info_compare(&received_frwrd_lkup_info, &expected_frwrd_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_frwrd_lpm_lkup_mpls_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_frwrd_lpm_lkup_fields_t received_lpm_lkup_info;
    diag_pp_frwrd_lpm_lkup_fields_t expected_lpm_lkup_info;

    /* Initialize expected info */
    expected_lpm_lkup_info.fec_ptr = 1025;
    expected_lpm_lkup_info.found = 1;

    if((rv = diag_pp_frwrd_lpm_lkup_get(unit, core_id, &received_lpm_lkup_info)) < 0)
    { 
        cli_out("Error: diag_pp_frwrd_lpm_lkup_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_frwrd_lpm_lkup_compare(&received_lpm_lkup_info, &expected_lpm_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_lpm_lkup_compare() Failed:\n");
        return rv;
    }


    return rv;
}

int
diag_pp_lem_lkup_info_mpls_test(int unit, int core_id)
{
    int rv = 0;
    diag_pp_lem_lkup_info_fields_t received_lem_lkup_info;
    diag_pp_lem_lkup_info_fields_t expected_lem_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_LEM_KEY_clear(&expected_lem_lkup_info.key);
    SOC_PPC_DIAG_LEM_VALUE_clear(&expected_lem_lkup_info.value);
    expected_lem_lkup_info.type = SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM;
    expected_lem_lkup_info.valid = 1;
    expected_lem_lkup_info.key.ilm.in_label = 1000;
    expected_lem_lkup_info.key.ilm.mapped_exp = 0;
    expected_lem_lkup_info.key.ilm.in_local_port = 0;
    expected_lem_lkup_info.key.ilm.inrif = 0;
    expected_lem_lkup_info.value.ilm.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
    expected_lem_lkup_info.value.ilm.dest_id = DIAG_PP_DEAFULT_FEC(unit);
    expected_lem_lkup_info.value.ilm.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
    expected_lem_lkup_info.value.ilm.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_SWAP;
    expected_lem_lkup_info.value.ilm.additional_info.eei.val.mpls_command.label = 2000;
    expected_lem_lkup_info.value.ilm.additional_info.eei.val.mpls_command.push_profile = 0;

    if((rv = diag_pp_lem_lkup_info_get(unit, core_id, &received_lem_lkup_info)) < 0)
    { 
        cli_out("Error: diag_pp_lem_lkup_info_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_lem_lkup_info_compare(&received_lem_lkup_info, &expected_lem_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_lem_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int 
diag_pp_egress_vlan_edit_info_mpls_test(int unit, int core_id)
{
   int rv = 0;
   SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO received_egress_vlan_edit_info;
   SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO expected_egress_vlan_edit_info;

   /* Initialize expected info */
   SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(&expected_egress_vlan_edit_info);
   /* command key */
   expected_egress_vlan_edit_info.key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
   expected_egress_vlan_edit_info.key.edit_profile = 0;
   /* command information */
   expected_egress_vlan_edit_info.cmd_info.tags_to_remove = 0;
   expected_egress_vlan_edit_info.cmd_info.cep_editing = 0;
   expected_egress_vlan_edit_info.cmd_info.outer_tag.tpid_index = 0;
   expected_egress_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_VSI;
   expected_egress_vlan_edit_info.cmd_info.outer_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
   expected_egress_vlan_edit_info.cmd_info.inner_tag.tpid_index = 0;
   expected_egress_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY;
   expected_egress_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
   /* outer out ac tag */
   expected_egress_vlan_edit_info.ac_tag.vid = 0;
   /* inner out ac tag */
   expected_egress_vlan_edit_info.ac_tag2.vid = 0;

   if ((rv = diag_pp_egress_vlan_edit_info_get(unit, core_id, &received_egress_vlan_edit_info)) < 0)
   {
        cli_out("Error: diag_pp_egress_vlan_edit_info_get() Failed:\n");
        return rv;
   }

   if((rv = diag_pp_egress_vlan_edit_info_compare(&received_egress_vlan_edit_info, &expected_egress_vlan_edit_info)) < 0)
   {
       cli_out("Error: diag_pp_egress_vlan_edit_info_compare() Failed:\n");
       return rv;
   }

   return rv;
   
}

int
diag_pp_received_packet_vpls_test(int unit, int core_id, uint32 ac_to_pwe, int in_local_port)
{
    int rv = 0;
    diag_pp_received_packets_fields_t received_pkt_info;
    diag_pp_received_packets_fields_t expected_pkt_info;
    int expected_core;
    SOC_PPC_DIAG_RESULT  ret_val;


    rv = soc_port_sw_db_local_to_pp_port_get(unit, in_local_port, &expected_pkt_info.in_pp_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_vpls_test() local_to_pp Failed:\n");
        return rv;
    }
    rv = soc_port_sw_db_local_to_tm_port_get(unit,  DIAG_PP_TM_PORT(unit, in_local_port), &expected_pkt_info.in_tm_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_vpls_test() local_to_pp Failed:\n");
        return rv;
    }


    expected_pkt_info.packet_qual   = 0;
    expected_pkt_info.pp_context    = SOC_TMC_PORT_HEADER_TYPE_ETH;
    expected_pkt_info.sys_id        = in_local_port;
    expected_pkt_info.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;


    if((rv = diag_pp_received_packet_get(unit, core_id, &received_pkt_info, &ret_val )) < 0)
    { 
        cli_out("Error: diag_pp_received_packet_get() Failed:\n");
        return rv;
    } 

    if (ret_val != SOC_PPC_DIAG_OK) {
        return rv;
    }
    if ((rv = diag_pp_received_packet_compare(&received_pkt_info, &expected_pkt_info)) < 0)
    {
        cli_out("Error: diag_pp_received_packet_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_parsing_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe)
{
    int 
        rv = 0,
        number_of_headers;
    diag_pp_parsing_info_fields_t received_pars_info;
    diag_pp_parsing_info_fields_t expected_pars_info;

    /* Initialize expected packet info */
    
    if (ac_to_pwe) {
        number_of_headers = 1;

        if((rv = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ETH, &expected_pars_info.pfc_hw, NULL)) < 0) {
            cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
            return rv;
        }

        /* ETH header */
        SOC_PPC_DIAG_PARSING_L2_INFO_clear(&expected_pars_info.hdrs_stack[0].eth);
        expected_pars_info.hdrs_stack[0].hdr_offset = 0;
        expected_pars_info.hdrs_stack[0].hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
        expected_pars_info.hdrs_stack[0].eth.encap_type = SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2;
        expected_pars_info.hdrs_stack[0].eth.next_prtcl = SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER;
        expected_pars_info.hdrs_stack[0].eth.tag_fromat.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
        expected_pars_info.hdrs_stack[0].eth.tag_fromat.is_outer_prio = 0;
        expected_pars_info.hdrs_stack[0].eth.tag_fromat.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2;
        expected_pars_info.hdrs_stack[0].eth.vlan_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
        expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].tpid = 0x8100;
        expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].vid = 10;
        expected_pars_info.hdrs_stack[0].eth.vlan_tags[1].tpid = 0x9100;
        expected_pars_info.hdrs_stack[0].eth.vlan_tags[1].vid = 20;
        
    } else {
        DPP_PFC_E pfc_sw;
        number_of_headers = 3;
        if (SOC_IS_JERICHO(unit)) { /* In jericho, due to second stage parsing, MPLS2_ETH is parsed as MPLS1_ETH*/
            pfc_sw = DPP_PFC_ETH_MPLS1_ETH;
        } else{
            pfc_sw = DPP_PFC_ETH_MPLS2_ETH;
        }
        if((rv = dpp_parser_pfc_get_hw_by_sw(unit, pfc_sw, &expected_pars_info.pfc_hw, NULL)) < 0) {
            cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
            return rv;
        }
        
        /* ETH header */
        SOC_PPC_DIAG_PARSING_L2_INFO_clear(&expected_pars_info.hdrs_stack[0].eth);
        expected_pars_info.hdrs_stack[0].hdr_offset = 0;
        expected_pars_info.hdrs_stack[0].hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
        expected_pars_info.hdrs_stack[0].eth.encap_type = SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2;
        expected_pars_info.hdrs_stack[0].eth.next_prtcl = SOC_PPC_L2_NEXT_PRTCL_TYPE_MPLS;
        expected_pars_info.hdrs_stack[0].eth.tag_fromat.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
        expected_pars_info.hdrs_stack[0].eth.tag_fromat.is_outer_prio = 0;
        expected_pars_info.hdrs_stack[0].eth.tag_fromat.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
        expected_pars_info.hdrs_stack[0].eth.vlan_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
        expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].tpid = 0x8100;
        expected_pars_info.hdrs_stack[0].eth.vlan_tags[0].vid = 100;
        
        /* MPLS header */
        SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(&expected_pars_info.hdrs_stack[1].mpls);
        expected_pars_info.hdrs_stack[1].hdr_offset = 18;
        expected_pars_info.hdrs_stack[1].hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;
        expected_pars_info.hdrs_stack[1].mpls.bos = 0;
    
        /* MPLS header */
        SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(&expected_pars_info.hdrs_stack[2].mpls);
        expected_pars_info.hdrs_stack[2].hdr_offset = 22;
        expected_pars_info.hdrs_stack[2].hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;
        expected_pars_info.hdrs_stack[2].mpls.bos = 1;
        
        /* ETH header */
        SOC_PPC_DIAG_PARSING_L2_INFO_clear(&expected_pars_info.hdrs_stack[3].eth);
        expected_pars_info.hdrs_stack[3].hdr_offset = 26;
        expected_pars_info.hdrs_stack[3].hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
        expected_pars_info.hdrs_stack[3].eth.encap_type = SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2;
        expected_pars_info.hdrs_stack[3].eth.next_prtcl = SOC_PPC_L2_NEXT_PRTCL_TYPE_OTHER;
        expected_pars_info.hdrs_stack[3].eth.tag_fromat.outer_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
        expected_pars_info.hdrs_stack[3].eth.tag_fromat.is_outer_prio = 0;
        expected_pars_info.hdrs_stack[3].eth.tag_fromat.inner_tpid = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
        expected_pars_info.hdrs_stack[3].eth.vlan_tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        
    }

    if((rv = diag_pp_parsing_info_get(unit, core_id, &received_pars_info )) < 0)
    { 
        cli_out("Error: diag_pp_parsing_info_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_parsing_info_compare(&received_pars_info, &expected_pars_info, number_of_headers)) < 0)
    {
        cli_out("Error: diag_pp_parsing_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_termination_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe)
{
    int rv = 0;
    diag_pp_termination_info_fields_t received_term_info;
    diag_pp_termination_info_fields_t expected_term_info;

    /* Initialize expected termination info */
    if (ac_to_pwe){
        expected_term_info.term_type  = SOC_PPC_PKT_TERM_TYPE_NONE;
    } else {
        expected_term_info.term_type  = SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH;
    }
    
    expected_term_info.frwrd_type = SOC_PPC_PKT_FRWRD_TYPE_BRIDGE;

    if((rv = diag_pp_termination_info_get(unit, core_id, &received_term_info )) < 0)
    { 
        cli_out("Error: diag_pp_termination_info_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_termination_info_compare(&received_term_info, &expected_term_info)) < 0)
    {
        cli_out("Error: diag_pp_termination_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

cmd_result_t
diag_pp_vlan_edit_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe)
{
    cmd_result_t result = CMD_OK;
    diag_pp_vlan_edit_info_fields_t *received_vlan_edit_info = NULL;
    diag_pp_vlan_edit_info_fields_t *expected_vlan_edit_info = NULL;

    expected_vlan_edit_info = sal_alloc(sizeof(*expected_vlan_edit_info), "diag_pp_vlan_edit_info_vpls_test.expected_vlan_edit_info");
    if(expected_vlan_edit_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    /* Initialize expected vlan edit info */
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info->ac_tag));
    SOC_SAND_PP_VLAN_TAG_clear(&(expected_vlan_edit_info->ac_tag2));
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&(expected_vlan_edit_info->cmd_info));
    
    if (ac_to_pwe) {
        expected_vlan_edit_info->cmd_id  = 11;
        expected_vlan_edit_info->cmd_info.tags_to_remove = 2; 
    } else {
        expected_vlan_edit_info->cmd_id  = 0;
        expected_vlan_edit_info->cmd_info.tags_to_remove = 0;
    }
    
    expected_vlan_edit_info->ac_tag.vid = 0;
    expected_vlan_edit_info->ac_tag.tpid = 0;
    expected_vlan_edit_info->ac_tag.pcp = 0;
    expected_vlan_edit_info->ac_tag.dei = 0;
    expected_vlan_edit_info->ac_tag2.vid = 0;
    expected_vlan_edit_info->ac_tag2.tpid = 0;
    expected_vlan_edit_info->ac_tag2.pcp = 0;
    expected_vlan_edit_info->ac_tag2.dei = 0;
    expected_vlan_edit_info->cmd_info.tpid_profile = 0;
    expected_vlan_edit_info->cmd_info.inner_tag.tpid_index = 0;
    expected_vlan_edit_info->cmd_info.inner_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info->cmd_info.inner_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
    expected_vlan_edit_info->cmd_info.outer_tag.tpid_index = 0;
    expected_vlan_edit_info->cmd_info.outer_tag.vid_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_EMPTY;
    expected_vlan_edit_info->cmd_info.outer_tag.pcp_dei_source = SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_NONE;
    
    received_vlan_edit_info = sal_alloc(sizeof(*received_vlan_edit_info), "diag_pp_vlan_edit_info_vpls_test.received_vlan_edit_info");
    if(received_vlan_edit_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }
    result = diag_pp_vlan_edit_info_get(unit, core_id, received_vlan_edit_info);
    if(result != CMD_OK)
    { 
        cli_out("Error: diag_pp_vlan_edit_info_get() Failed:\n");
        goto exit;
    } 

    result = diag_pp_vlan_edit_info_compare(received_vlan_edit_info, expected_vlan_edit_info);
    if(result != CMD_OK)
    {
        cli_out("Error: diag_pp_vlan_edit_info_compare() Failed:\n");
        goto exit;
    }

exit:
    if (received_vlan_edit_info) {
        sal_free(received_vlan_edit_info);
    }
    if(expected_vlan_edit_info) {
        sal_free(expected_vlan_edit_info);
    }
    return result;
}

int
diag_pp_encap_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe, int out_local_port)
{
    int rv = 0;
    cmd_result_t result = CMD_OK;
    SOC_PPC_DIAG_ENCAP_INFO *received_encap_info = NULL;
    SOC_PPC_DIAG_ENCAP_INFO *expected_encap_info = NULL;
    uint32   tm_port, pp_port;
    int      expected_core;

    /* Initialize expected info */
    expected_encap_info = sal_alloc(sizeof(*expected_encap_info), "diag_pp_encap_info_vpls_test.expected_encap_info");
    if(expected_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }

    SOC_PPC_DIAG_ENCAP_INFO_clear(expected_encap_info);

    if (ac_to_pwe) {
        
        expected_encap_info->mpls_cmd.command = SOC_TMC_MPLS_COMMAND_TYPE_PUSH;
        expected_encap_info->mpls_cmd.label = 1982;
        expected_encap_info->mpls_cmd.push_profile = 3;
        expected_encap_info->nof_eeps = 2;
        expected_encap_info->eep[0] = DIAG_PP_FIRST_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
        expected_encap_info->encap_info[0].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP;
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.tunnels[0].tunnel_label = 1000;
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.tunnels[0].push_profile = 1;
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.tunnels[1].tunnel_label = 1002;
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.tunnels[1].push_profile = 2;
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.nof_tunnels = 2;
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.out_vsi = 200;
        expected_encap_info->encap_info[0].entry_val.mpls_encap_info.oam_lif_set = 0;
        if (SOC_IS_JERICHO(unit)) {
            expected_encap_info->encap_info[0].entry_val.mpls_encap_info.outlif_profile = 1;
        }
    
        expected_encap_info->eep[1] = DIAG_PP_SECOND_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
        expected_encap_info->encap_info[1].entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP;
        expected_encap_info->encap_info[1].entry_val.ll_info.dest_mac.address[0] = 0x22;
        expected_encap_info->encap_info[1].entry_val.ll_info.out_vid_valid = 1;
        expected_encap_info->encap_info[1].entry_val.ll_info.out_vid = 200;
    
        expected_encap_info->ll_vsi = 200;
        expected_encap_info->out_ac = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID);
        tm_port = out_local_port;
        pp_port = out_local_port;
        
    } else {
        
        expected_encap_info->ll_vsi = 6202;
        expected_encap_info->out_ac = DIAG_PP_THIRD_VPLS_EGRESS_LOCAL_OUTLIF(unit);
        tm_port = out_local_port;
        pp_port = out_local_port;
        
    }
    rv = soc_port_sw_db_local_to_pp_port_get(unit, pp_port, &expected_encap_info->pp_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }
    rv = soc_port_sw_db_local_to_tm_port_get(unit, tm_port, &expected_encap_info->tm_port, &expected_core);
    if ( rv < 0) {
        cli_out("Error: diag_pp_received_packet_mpls_test() local_to_pp Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }

    received_encap_info = sal_alloc(sizeof(*received_encap_info), "diag_pp_encap_info_vpls_test.received_encap_info");
    if(received_encap_info == NULL) {
        cli_out("Memory allocation failure!\n");
        result = CMD_FAIL;
        goto exit;
    }

    if((rv = diag_pp_encap_info_get(unit, core_id, received_encap_info)) != CMD_OK)
    { 
        cli_out("Error: diag_pp_encap_info_get() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    } 

    if((rv = diag_pp_encap_info_compare(received_encap_info, expected_encap_info)) < 0)
    {
        cli_out("Error: diag_pp_encap_info_compare() Failed:\n");
        result = CMD_FAIL;
        goto exit;
    }

exit:
    if(received_encap_info) {
        sal_free(received_encap_info);
    }
    if(expected_encap_info) {
        sal_free(expected_encap_info);
    }
    return result;

}

int
diag_pp_frwrd_lkup_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe, int out_local_port)
{
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO received_frwrd_lkup_info;
    SOC_PPC_DIAG_FRWRD_LKUP_INFO expected_frwrd_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(&expected_frwrd_lkup_info);
    expected_frwrd_lkup_info.frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT;
    expected_frwrd_lkup_info.key_found = 1;
    expected_frwrd_lkup_info.lkup_key.mact.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.fid = 6202;
    
    if (ac_to_pwe){   
        expected_frwrd_lkup_info.frwrd_hdr_index = 1;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[0] = 0x6;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[1] = 0x5;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[2] = 0x4;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[3] = 0x3;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[4] = 0x2;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[5] = 0x1;
        
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_PUSH;
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.label = 1982;
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.push_profile = 3;
    } else {
        expected_frwrd_lkup_info.frwrd_hdr_index = 3;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[0] = 0xc;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[1] = 0xb;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[2] = 0xa;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[3] = 0x9;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[4] = 0x8;
        expected_frwrd_lkup_info.lkup_key.mact.key_val.mac.mac.address[5] = 0x7;
        
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.dest_id = out_local_port;
        expected_frwrd_lkup_info.lkup_res.mact.aging_info.age_status = 6;

        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
        expected_frwrd_lkup_info.lkup_res.mact.frwrd_info.forward_decision.additional_info.outlif.val = DIAG_PP_DEFAULT_VPLS_GLOBAL_OUTLIF(unit);
    }
    
    if((rv = diag_pp_frwrd_lkup_info_get(unit, core_id, &received_frwrd_lkup_info)) < 0)
    { 
        cli_out("Error: diag_pp_frwrd_lkup_info_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_frwrd_lkup_info_compare(&received_frwrd_lkup_info, &expected_frwrd_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int 
diag_pp_egress_vlan_edit_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe)
{
    int rv = 0;
    SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO received_egress_vlan_edit_info;
    SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO expected_egress_vlan_edit_info;
    
    /* Initialize expected info */
    SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(&expected_egress_vlan_edit_info);
    expected_egress_vlan_edit_info.key.tag_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;

    /* command key */
    if(ac_to_pwe) {
        expected_egress_vlan_edit_info.key.edit_profile = 0;
    } else { 
        expected_egress_vlan_edit_info.key.edit_profile = 3;
    }
    
    /* command information */
    expected_egress_vlan_edit_info.cmd_info.tags_to_remove = 0;
    expected_egress_vlan_edit_info.cmd_info.cep_editing = 0;
    expected_egress_vlan_edit_info.cmd_info.outer_tag.tpid_index = 0;
    expected_egress_vlan_edit_info.cmd_info.outer_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
    if(ac_to_pwe) {
        expected_egress_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_VSI;
        expected_egress_vlan_edit_info.cmd_info.inner_tag.tpid_index = 0;
        expected_egress_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY;
        expected_egress_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE;
    } else {
        expected_egress_vlan_edit_info.cmd_info.outer_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0;
        expected_egress_vlan_edit_info.cmd_info.inner_tag.tpid_index = 1;
        expected_egress_vlan_edit_info.cmd_info.inner_tag.vid_source = SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1;
        expected_egress_vlan_edit_info.cmd_info.inner_tag.pcp_dei_source = SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP;
        
        /* outer out ac tag */
        expected_egress_vlan_edit_info.ac_tag.vid = 10;
        /* inner out ac tag */
        expected_egress_vlan_edit_info.ac_tag2.vid = 20; 
    }
    
    if ((rv = diag_pp_egress_vlan_edit_info_get(unit, core_id, &received_egress_vlan_edit_info)) < 0)
    {
         cli_out("Error: diag_pp_egress_vlan_edit_info_get() Failed:\n");
         return rv;
    }
    
    if((rv = diag_pp_egress_vlan_edit_info_compare(&received_egress_vlan_edit_info, &expected_egress_vlan_edit_info)) < 0)
    {
        cli_out("Error: diag_pp_egress_vlan_edit_info_compare() Failed:\n");
        return rv;
    }
    
    return rv;
}

int
diag_pp_lem_lkup_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe, int out_local_port)
{
    int rv = 0;
    diag_pp_lem_lkup_info_fields_t received_lem_lkup_info;
    diag_pp_lem_lkup_info_fields_t expected_lem_lkup_info;

    /* Initialize expected info */
    SOC_PPC_DIAG_LEM_KEY_clear(&expected_lem_lkup_info.key);
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&expected_lem_lkup_info.key.mact);
    SOC_PPC_DIAG_LEM_VALUE_clear(&expected_lem_lkup_info.value);
    expected_lem_lkup_info.type = SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT;
    expected_lem_lkup_info.valid = 1;
    expected_lem_lkup_info.key.mact.key_val.mac.fid = 6202;
    expected_lem_lkup_info.key.mact.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    expected_lem_lkup_info.value.mact.aging_info.age_status = 0;
    if (ac_to_pwe){
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[0] = 0x6;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[1] = 0x5;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[2] = 0x4;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[3] = 0x3;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[4] = 0x2;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[5] = 0x1;
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_PUSH;
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.label = 1982;
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.eei.val.mpls_command.push_profile = 3;
    } else {
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[0] = 0xc;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[1] = 0xb;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[2] = 0xa;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[3] = 0x9;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[4] = 0x8;
        expected_lem_lkup_info.key.mact.key_val.mac.mac.address[5] = 0x7;

        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.dest_id = out_local_port;

        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
        expected_lem_lkup_info.value.mact.frwrd_info.forward_decision.additional_info.outlif.val  = DIAG_PP_DEFAULT_VPLS_GLOBAL_OUTLIF(unit);
    }

    if((rv = diag_pp_lem_lkup_info_get(unit, core_id, &received_lem_lkup_info)) < 0)
    { 
        cli_out("Error: diag_pp_lem_lkup_info_get() Failed:\n");
        return rv;
    } 

    if((rv = diag_pp_lem_lkup_info_compare(&received_lem_lkup_info, &expected_lem_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_lem_lkup_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_frwrd_trace_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe, int out_local_port)
{
    int rv = 0;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO received_frwrd_trace_info;
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO expected_frwrd_trace_info;
    SOC_PPC_DIAG_RESULT   ret_val;

    /* Initialize expected info */
    SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(&expected_frwrd_trace_info);
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    /* PHASE LIF (VTT) */
    expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.trap_code = SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP;

    /* PHASE LKUP FOUND (FLP) */
    if (ac_to_pwe) {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_PUSH;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.additional_info.eei.val.mpls_command.label = 1982;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.additional_info.eei.val.mpls_command.push_profile = 3;
        expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
        expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = 3;
    } else {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision.dest_id = out_local_port;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].outlif = 12288;
        expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
        expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = 3;
    }
    
    /* PHASE PMF */
    if (ac_to_pwe) {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.dest_id = DIAG_PP_DEAFULT_FEC(unit);
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_PUSH;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.additional_info.eei.val.mpls_command.label = 1982;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.additional_info.eei.val.mpls_command.push_profile = 3;
    } else {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision.dest_id = out_local_port;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].outlif = 12288;
    }

    /* PHASE FEC */
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    expected_frwrd_trace_info.trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.frwrd_action_strength = 3;
    
    /* PHASE TRAP */
    if (ac_to_pwe) {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.dest_id = out_local_port;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].outlif = DIAG_PP_FIRST_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_PUSH;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.val.mpls_command.label = 1982;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.additional_info.eei.val.mpls_command.push_profile = 3;
    } else {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision.dest_id = out_local_port;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].outlif = 12288;
    }

    /* PHASE ING RESOLVED (INGRESS TM COMMAND) */
    if (ac_to_pwe) {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.dest_id = out_local_port;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].outlif = DIAG_PP_FIRST_MPLS_DEFAULT_EGRESS_LOCAL_OUTLIF(unit);
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_TMC_MPLS_COMMAND_TYPE_PUSH;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.val.mpls_command.label = 1982;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.additional_info.eei.val.mpls_command.push_profile = 3;
    } else {
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision.dest_id = out_local_port;
        expected_frwrd_trace_info.frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].outlif = 12288;
    }

    if((rv = diag_pp_frwrd_trace_info_get(unit, core_id, &received_frwrd_trace_info, &ret_val)) < 0)
    { 
        cli_out("Error: diag_pp_frwrd_trace_info_get() Failed:\n");
        return rv;
    } 
    if (ret_val != SOC_PPC_DIAG_OK) {
       cli_out("Error: Packet not found.\n");
       return rv;
    }

    if((rv = diag_pp_frwrd_trace_info_compare(&received_frwrd_trace_info, &expected_frwrd_trace_info)) < 0)
    {
        cli_out("Error: diag_pp_frwrd_trace_info_compare() Failed:\n");
        return rv;
    }

    return rv;
}

int
diag_pp_lif_lkup_info_vpls_test(int unit, int core_id, uint32 ac_to_pwe)
{
    int rv = 0;
    diag_pp_lif_lkup_info_fields_t received_lif_lkup_info;
    diag_pp_lif_lkup_info_fields_t expected_lif_lkup_info;
    SOC_PPC_DIAG_DB_USE_INFO *db_info = NULL;
    int old_learn_mode; /* Remember the learn mode to be restored in the end of the test */

    /* Initialize DB info */
    db_info = sal_alloc(sizeof(SOC_PPC_DIAG_DB_USE_INFO), "diag_pp_lif_lkup_info_vpls_test.db_info");
    if(!db_info) {
        cli_out("Memory allocation failure.\n");
        return -1;
    }

    rv = bcm_switch_control_get(0, bcmSwitchL2LearnMode, &old_learn_mode);
    if (rv) {
        cli_out("Error: bcm_switch_control_get() Failed:\n"); 
        sal_free(db_info);
        return rv;
    }

    rv = bcm_switch_control_set(0, bcmSwitchL2LearnMode, old_learn_mode | BCM_L2_LEARN_DISABLE);
    if (rv) {
        cli_out("Error: bcm_switch_control_set() Failed:\n"); 
        sal_free(db_info);
        return rv;
    }

    /* Lookup results in VT + SEM-B */
    if (!ac_to_pwe) {
        /* This lookup is only valid for PWE->AC */
        SOC_PPC_DIAG_DB_USE_INFO_clear(db_info);
        db_info->lkup_num = 0;
        db_info->bank_id = 1;

        /* Initialize expected db lif lkup info */
        memset(&expected_lif_lkup_info, 0, sizeof(diag_pp_lif_lkup_info_fields_t));
        SOC_PPC_DIAG_LIF_KEY_clear(&expected_lif_lkup_info.key);
        SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&expected_lif_lkup_info.key.mpls);
        
        expected_lif_lkup_info.key.mpls.vsid = 0;
        expected_lif_lkup_info.key.mpls.label_index = SOC_PPC_MPLS_LABEL_INDEX_ALL; 
        expected_lif_lkup_info.key.mpls.label_id_second = 0; 
        expected_lif_lkup_info.value.mpls.cos_profile = 0;
        expected_lif_lkup_info.found = 1; 
        expected_lif_lkup_info.opcode_id = 0;
        expected_lif_lkup_info.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
        expected_lif_lkup_info.base_index = DIAG_PP_DEAFULT_INGRESS_LOCAL_LIF(unit);
        expected_lif_lkup_info.key.mpls.label_id = 1002;
        expected_lif_lkup_info.key.mpls.flags = 0;

        if((rv = diag_pp_lif_lkup_info_get(unit, core_id, &received_lif_lkup_info, db_info )) < 0)
        { 
            cli_out("Error: diag_pp_lif_lkup_info_get() Failed:\n");
            sal_free(db_info);
            return rv;
        } 

        if((rv = diag_pp_lif_lkup_info_compare(&received_lif_lkup_info, &expected_lif_lkup_info)) < 0)
        {
            cli_out("Error: diag_pp_lif_lkup_info_compare() Failed:\n");
            sal_free(db_info);
            return rv;
        }
    }

    /* Lookup results in TT + SEM-B */
    SOC_PPC_DIAG_DB_USE_INFO_clear(db_info);
    db_info->lkup_num = 1;
    db_info->bank_id = 1;

    /* Initialize expected db lif lkup info */
    memset(&expected_lif_lkup_info, 0, sizeof(diag_pp_lif_lkup_info_fields_t));
    SOC_PPC_DIAG_LIF_KEY_clear(&expected_lif_lkup_info.key);

    expected_lif_lkup_info.found = 1; 
    expected_lif_lkup_info.opcode_id = 0;

    if (ac_to_pwe) {
        SOC_PPC_L2_LIF_AC_KEY_clear(&expected_lif_lkup_info.key.ac);
        expected_lif_lkup_info.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;
        expected_lif_lkup_info.base_index = DIAG_PP_DEAFULT_INGRESS_ONLY_GLOBAL_INLIF_3(unit);
        expected_lif_lkup_info.key.ac.key_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN;
        expected_lif_lkup_info.key.ac.outer_vid = 10;
        expected_lif_lkup_info.key.ac.inner_vid = 20;
        expected_lif_lkup_info.value.ac.vsid = 6202;
        expected_lif_lkup_info.value.ac.learn_record.learn_type = SOC_PPC_L2_LIF_AC_LEARN_SYS_PORT;
        expected_lif_lkup_info.value.ac.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    } else {
        SOC_PPC_MPLS_LABEL_RIF_KEY_clear(&expected_lif_lkup_info.key.mpls);
        expected_lif_lkup_info.key.mpls.vsid = 0;
        expected_lif_lkup_info.key.mpls.label_index = SOC_PPC_MPLS_LABEL_INDEX_ALL; 
        expected_lif_lkup_info.key.mpls.label_id_second = 0; 
        expected_lif_lkup_info.value.mpls.cos_profile = 0;
        expected_lif_lkup_info.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE;
        if (SOC_IS_JERICHO(unit)) {
            expected_lif_lkup_info.base_index = 32772;
        } else {
            expected_lif_lkup_info.base_index = 20480;
        }
        expected_lif_lkup_info.key.mpls.label_id = 2010;
        expected_lif_lkup_info.key.mpls.flags = SOC_IS_ARADPLUS(unit) ? 0 : SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS;
        expected_lif_lkup_info.key.pwe = 2010;
    }

    if((rv = diag_pp_lif_lkup_info_get(unit, core_id, &received_lif_lkup_info, db_info )) < 0)
    { 
        cli_out("Error: diag_pp_lif_lkup_info_get() Failed:\n");
        sal_free(db_info);
        return rv;
    } 

    if((rv = diag_pp_lif_lkup_info_compare(&received_lif_lkup_info, &expected_lif_lkup_info)) < 0)
    {
        cli_out("Error: diag_pp_lif_lkup_info_compare() Failed:\n");
        sal_free(db_info);
        return rv;
    }

    if((rv = bcm_switch_control_set(0, bcmSwitchL2LearnMode, old_learn_mode)) < 0)
    {
        cli_out("Error: bcm_switch_control_set() Failed:\n");
        sal_free(db_info);
        return rv;
    }

    sal_free(db_info);
    return rv;
}



int
per_vsi_get_count(int unit, bcm_vlan_t vsi, uint32 *ret_count);

int
get_mact_num_entries(int unit, uint32 *ret_count);

int
l2_count_bcm_command_test(int unit)
{

    int rv = 0;
    uint32 counter= 0;
    uint32 expected_counter = 0;

    rv = per_vsi_get_count(unit,0,&counter);
    if (rv < 0) {
        cli_out("Error: per_vsi_get_count() Failed:\n");
        return rv;
    }else if (counter != 777) {
        cli_out("Error: expected value 777 from per_vsi_get_count(), got %d:\n",counter);
        return -1;
    }


    rv = per_vsi_get_count(unit,1,&counter);
    if (rv < 0) {
        cli_out("Error: per_vsi_get_count() Failed:\n");
        return rv;
    }else if (counter != 65535) {
        cli_out("Error: expected value 65535 from per_vsi_get_count(), got %d:\n",counter);
        return -1;
    }


    rv = per_vsi_get_count(unit,4095,&counter);
    if (rv < 0) {
        cli_out("Error: per_vsi_get_count() Failed:\n");
        return rv;
    }else if (counter != 876) {
        cli_out("Error: expected value 876 from per_vsi_get_count(), got %d:\n",counter);
        return -1;
    }

    rv = per_vsi_get_count(unit,8000,&counter);
    if (rv < 0) {
        cli_out("Error: per_vsi_get_count() Failed:\n");
        return rv;
    }else if (counter != 862) {
        cli_out("Error: expected value 862 from per_vsi_get_count(), got %d:\n",counter);
        return -1;
    }

    rv = per_vsi_get_count(unit,32767,&counter);
    if (rv < 0) {
        cli_out("Error: per_vsi_get_count() Failed:\n");
        return rv;
    }else if (counter != 862) {
        cli_out("Error: expected value 862 from per_vsi_get_count(), got %d:\n",counter);
        return -1;
    }

    if (SOC_IS_QUX(unit)) {
        expected_counter = 65535;
    } else {
        expected_counter = 262143;
    }

    rv = get_mact_num_entries(unit,&counter);
    if (rv < 0) {
        cli_out("Error: per_vsi_get_count() Failed:\n");
        return rv;
    } else if (counter != expected_counter) {
        cli_out("Error: expected value %d from get_mact_num_entries(), got %d:\n",expected_counter, counter);
        return -1;
    }

    return rv; 
}

int
diag_pp_test_init(int unit, args_t *a, void **p)
{
    int rv = 0;
    parse_table_t pt;
    diag_pp_test_params_t *dpt;
    dpt = &diag_pp[unit];
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Bridge", PQ_INT, (void *) 0,
      &(dpt->bridge), NULL);
    parse_table_add(&pt, "Router", PQ_INT, (void *) 0,
      &(dpt->router), NULL);
    parse_table_add(&pt, "Mpls", PQ_INT, (void *) 0, 
      &(dpt->mpls), NULL);
    parse_table_add(&pt, "Vpls", PQ_INT, (void *) 0, 
      &(dpt->vpls), NULL);
    parse_table_add(&pt, "Counter", PQ_INT, (void *) 0, 
      &(dpt->counter), NULL);
	parse_table_add(&pt, "Meter", PQ_INT, (void *) 0, 
      &(dpt->meter), NULL);
	parse_table_add(&pt, "FECALLOC", PQ_INT, (void *) 0,
	  &(dpt->fec_allocation), NULL);
    parse_table_add(&pt, "core", PQ_INT, (void *) 0, 
      &(dpt->in_core), NULL);
    parse_table_add(&pt, "out_core", PQ_INT, (void *) 0,
      &(dpt->out_core), NULL);
    parse_table_add(&pt, "in_port", PQ_INT, (void *) -1, 
      &(dpt->in_port), NULL);
    parse_table_add(&pt, "out_port", PQ_INT, (void *) -1,
      &(dpt->out_port), NULL);
	
	/*meter ids*/
	parse_table_add(&pt, "id1", PQ_INT, (void *) 0, 
					&(dpt->meter_id_1), NULL);
	parse_table_add(&pt, "id2", PQ_INT, (void *) 0, 
					&(dpt->meter_id_2), NULL);

    *p = dpt;
    
    if (parse_arg_eq(a, &pt) < 0) 
    {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",diag_pp_test_usage);
        rv = -1;
    } 
    else if (ARG_CNT(a) != 0) 
    {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",diag_pp_test_usage);
        rv = -1;
    }

    parse_arg_eq_done(&pt);

    if (dpt->in_port != -1)
    {
        rv = soc_port_sw_db_core_get(unit, dpt->in_port, &(dpt->in_core));
        if ( rv < 0) {
            cli_out("Error: soc_port_sw_db_core_get() Failed on in-port:\n");
        }
    }

    if (dpt->out_port != -1)
    {
        rv = soc_port_sw_db_core_get(unit, dpt->out_port, &(dpt->out_core));
        if ( rv < 0) {
            cli_out("Error: soc_port_sw_db_core_get() Failed on out-port:\n");
        }
    }

    return rv;
}

int
diag_pp_test_done(int unit, void *p)
{
    cli_out("Diag PP Tests Done\n");
    return 0;
}

int
diag_pp_test(int unit, args_t *a, void *p)
{
    int rv = 0;
    diag_pp_test_params_t *dpt = p;
 
    if(dpt->bridge)
    {
        if((rv = diag_pp_received_packet_bridge_test(unit, dpt->in_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_received_packet_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_parsing_info_bridge_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_parsing_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_termination_info_bridge_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_termination_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_vlan_edit_info_bridge_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_vlan_edit_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_pkt_tm_info_bridge_test(unit, dpt->in_port, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_pkt_tm_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_lif_lkup_info_bridge_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_lif_lkup_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_trace_info_bridge_test(unit, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_trace_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_traps_info_bridge_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_traps_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_encap_info_bridge_test(unit, dpt->out_port, dpt->out_core)) < 0)
        {
            cli_out("Error: diag_pp_encap_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_lkup_info_bridge_test(unit, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_lkup_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_lpm_lkup_bridge_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_lpm_lkup_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_lem_lkup_info_bridge_test(unit, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_lem_lkup_info_bridge_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_egress_vlan_edit_info_bridge_test(unit, dpt->out_core )) < 0)
        {
            cli_out("Error: diag_pp_egress_vlan_edit_info_bridge_test () Failed:\n");
            goto exit;
        }
    }
    else if(dpt->router)
    {
        if((rv = diag_pp_received_packet_router_test(unit, dpt->in_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_received_packet_router_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_parsing_info_router_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_parsing_info_router_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_termination_info_router_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_termination_info_router_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_vlan_edit_info_router_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_vlan_edit_info_router_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_pkt_tm_info_router_test(unit, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_pkt_tm_info_router_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_lif_lkup_info_router_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_lif_lkup_info_router_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_trace_info_router_test(unit, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_trace_info_router_test () Failed:\n");
            goto exit;
        }
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if((rv = diag_pp_traps_info_router_test(unit, dpt->in_core)) < 0)
            {
                cli_out("Error: diag_pp_traps_info_router_test () Failed:\n");
                goto exit;
            }
        }
        if((rv = diag_pp_encap_info_router_test(unit, dpt->out_port, dpt->out_core)) < 0)
        {
            cli_out("Error: diag_pp_encap_info_router_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_lkup_info_router_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_lkup_info_router_test () Failed:\n");
            goto exit;
        }
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
            if((rv = diag_pp_frwrd_lpm_lkup_router_test(unit, dpt->in_core)) < 0)
            {
                cli_out("Error: diag_pp_frwrd_lpm_lkup_router_test () Failed:\n");
                goto exit;
            }
        }
        if((rv = diag_pp_lem_lkup_info_router_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_lem_lkup_info_router_test () Failed:\n");
            goto exit;      
        }
        if((rv = diag_pp_egress_vlan_edit_info_router_test(unit, dpt->out_core)) < 0)
        {
            cli_out("Error: diag_pp_egress_vlan_edit_info_router_test () Failed:\n");
            goto exit;
        }
    }
    else if(dpt->mpls)
    {
        if((rv = diag_pp_received_packet_mpls_test(unit, dpt->in_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_received_packet_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_parsing_info_mpls_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_parsing_info_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_termination_info_mpls_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_termination_info_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_vlan_edit_info_mpls_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_vlan_edit_info_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_pkt_tm_info_mpls_test(unit, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_pkt_tm_info_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_lif_lkup_info_mpls_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_lif_lkup_info_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_trace_info_mpls_test(unit, dpt->out_port, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_trace_info_mpls_test () Failed:\n");
            goto exit;
        } 
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if((rv = diag_pp_traps_info_mpls_test(unit, dpt->in_core)) < 0)
            {
                cli_out("Error: diag_pp_traps_info_mpls_test () Failed:\n");
                goto exit;
            }
        }
        if((rv = diag_pp_encap_info_mpls_test(unit, dpt->out_port, dpt->out_core)) < 0)
        {
            cli_out("Error: diag_pp_encap_info_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_lkup_info_mpls_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_lkup_info_mpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_lem_lkup_info_mpls_test(unit, dpt->in_core)) < 0)
        {
            cli_out("Error: diag_pp_lem_lkup_info_mpls_test () Failed:\n");
            goto exit;      
        }
        if ((rv = diag_pp_egress_vlan_edit_info_mpls_test(unit, dpt->out_core)) < 0)
        {
            cli_out("Error: diag_pp_egress_vlan_edit_info_mpls_test () Failed:\n");
        }
    }
    else if(dpt->vpls)
    {
        uint32 ac_to_pwe = (dpt->vpls == 2);
         if((rv = diag_pp_received_packet_vpls_test(unit, dpt->in_core, ac_to_pwe, dpt->in_port)) < 0)
        {
            cli_out("Error: diag_pp_received_packet_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_parsing_info_vpls_test(unit, dpt->in_core, ac_to_pwe)) < 0)
        {
            cli_out("Error: diag_pp_parsing_info_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_termination_info_vpls_test(unit, dpt->in_core, ac_to_pwe)) < 0)
        {
            cli_out("Error: diag_pp_termination_info_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_vlan_edit_info_vpls_test(unit, dpt->in_core, ac_to_pwe)) < 0)
        {
            cli_out("Error: diag_pp_vlan_edit_info_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_lif_lkup_info_vpls_test(unit, dpt->in_core, ac_to_pwe)) < 0)
        {
            cli_out("Error: diag_pp_lif_lkup_info_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_trace_info_vpls_test(unit, dpt->in_core, ac_to_pwe, dpt->out_port)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_trace_info_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_encap_info_vpls_test(unit, dpt->out_core, ac_to_pwe, dpt->out_port)) < 0)
        {
            cli_out("Error: diag_pp_encap_info_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_frwrd_lkup_info_vpls_test(unit, dpt->in_core, ac_to_pwe, dpt->out_port)) < 0)
        {
            cli_out("Error: diag_pp_frwrd_lkup_info_vpls_test () Failed:\n");
            goto exit;
        }
        if((rv = diag_pp_lem_lkup_info_vpls_test(unit, dpt->in_core, ac_to_pwe,dpt->out_port)) < 0)
        {
            cli_out("Error: diag_pp_lem_lkup_info_vpls_test () Failed:\n");
            goto exit;      
        }
        if ((rv = diag_pp_egress_vlan_edit_info_vpls_test(unit, dpt->out_core, ac_to_pwe)) < 0)
        {
            cli_out("Error: diag_pp_egress_vlan_edit_info_vpls_test () Failed:\n");
        }
    }
    else if(dpt->counter)
    {
        cli_out(" running bcm shell command test , for command l2 count :\n");
        if ((rv = l2_count_bcm_command_test(unit)) < 0) 
        {
            cli_out("Error: l2_count_bcm_command_test () Failed:\n");
        }
    }
	else if(dpt->meter) 
	{
		if((rv = diag_pp_pkt_tm_info_meter_test(unit, dpt->in_core, dpt->meter_id_1, dpt->meter_id_2)) < 0)
        {
     
	        cli_out("Error: diag_pp_pkt_tm_info_meter_test () Failed:\n");
            goto exit;
        }
	}
	else if(dpt->fec_allocation) {
		if ((rv = fec_allocation_main(unit, 0)) < 0)
		{
           cli_out("Error: diag_pp_fec_allocation() Failed:\n");
           goto exit;
        }
	}
exit:
    if (rv < 0) 
    {
        test_error(unit, "Diag PP test Failed!!!\n");
        return rv;
    }

    return rv;
}

#endif

