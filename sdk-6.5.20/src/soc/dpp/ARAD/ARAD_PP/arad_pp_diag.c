#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG

#include <shared/bsl.h>



#include <shared/swstate/access/sw_state_access.h>
#include <bcm/policer.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_ports.h>
#if defined(INCLUDE_KBP)
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#endif 

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fcf.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_sa_auth.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv6.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>
#include <soc/dpp/PPD/ppd_api_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_test.h>

#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_port.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_diag.h>

#include <soc/dpp/TMC/tmc_api_packet.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_ac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_vlan_edit.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/sand/sand_signals.h>




#define SOC_PPC_DIAG_PKT_TRACE_MAX                               (SOC_SAND_U32_MAX)
#define SOC_PPC_DIAG_FLAVOR_MAX                                  (SOC_PPC_NOF_DIAG_FLAVORS-1)
#define ARAD_PP_DIAG_LKUP_NUM_MAX                                (SOC_SAND_U32_MAX)
#define ARAD_PP_DIAG_BANK_ID_MAX                                 (SOC_SAND_U32_MAX)

#define ARAD_PP_DIAG_HDR_QUAL_SIZE      (11)


#define ARAD_PP_DIAG_HDR_INDEX_SIZE      (7)


#define ARAD_PP_DIAG_HDR_MPLS_BYTE_SIZE      (4)

#define ARAD_PP_DIAG_FLD_LSB_MIN (0)
#define ARAD_PP_DIAG_FLD_MSB_MAX (255)
#define ARAD_PP_DIAG_FLD_LEN (256)

#define ARAD_PP_FIRST_TBL_ENTRY (0)


#define ARAD_PP_DIAG_FRWRD_DECISION_DEST_DROP_CODE  (SOC_IS_JERICHO(unit)? 0x7FFFF: 0x3FFFF)
#define ARAD_PP_DIAG_FRWRD_DECISION_TYPE_UC_FLOW    (SOC_IS_JERICHO(unit)? 3: 1)
#define ARAD_PP_DIAG_FRWRD_DECISION_TYPE_MC         (0x1) 
#define ARAD_PP_DIAG_FRWRD_DECISION_TYPE_EGRESS_MC  (0x2) 

#if defined(INCLUDE_KBP)

#define ARAD_PP_DIAG_FORWARDING_TYPE_MAX_PARAMETERS     4
#endif 





#define SOC_CHECK_DIAG_LAST_PACKET_VALIDITY(func_name, core)                           \
  res = arad_pp_diag_is_valid(unit, core, func_name, &ret_val);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 999, exit);                            \
  if (!ret_val) {                                                                \
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_DIAG_INVALID_LAST_PACKET_ERR, 999, exit);  \
  }   


#define ARAD_PP_DIAG_PERCENT(x,y) (((100 * (x)) + ((y) / 2)) / (y))




#if defined(INCLUDE_KBP)
typedef struct{

    CONST char *name;

    uint32 msb;

    uint32 lsb;

    void (*print_function)(CONST char *param_name, uint32 msb, uint32 lsb, CONST void *param);
}ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER;

STATIC void _uint8_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35d|\n\r"), param_name, msb, lsb, *((uint8*)param)));
} 

STATIC void _uint16_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35d|\n\r"), param_name, msb, lsb, *((uint16*)param)));
} 

STATIC void _uint32_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35d|\n\r"), param_name, msb, lsb, *((uint32*)param)));
}

STATIC void _ipv4_address_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    char
        decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|%-35s|\n\r"), param_name, msb, lsb, soc_sand_pp_ip_long_to_string(*((uint32*)param), 1 ,decimal_ip)));
}

STATIC void _ipv6_address_print(CONST char* param_name, uint32 msb, uint32 lsb, CONST void *param) {
    LOG_CLI((BSL_META("%-20s|%-4d|%-4d|"), param_name, msb, lsb));
    soc_sand_SAND_PP_IPV6_ADDRESS_print((SOC_SAND_PP_IPV6_ADDRESS*)param);
    LOG_CLI((BSL_META("|\n\r")));
}

typedef struct{

uint32 size_in_bytes;

uint32 nof_params;

ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER params[ARAD_PP_DIAG_FORWARDING_TYPE_MAX_PARAMETERS];
}ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETERS;

CONST static ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETERS forwarding_type_parameters[ARAD_KBP_FRWRD_IP_NOF_TABLES] = {

    {   10,
        3,
        {
            {"vrf", 75, 64, _uint16_print},
            {"SIP", 63, 32, _ipv4_address_print},
            {"DIP", 31, 0, _ipv4_address_print},
            {"", 0, 0, NULL}
        }
    },

    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },

    {   12,
        4,
        {
            {"vrf", 91, 80, _uint16_print},
            {"in_rif", 75, 64, _uint16_print},
            {"SIP", 63, 32, _ipv4_address_print},
            {"DIP", 27, 0, _ipv4_address_print}
        }
    },

    {   34,
        3,
        {
            {"vrf", 267, 256, _uint16_print},
            {"SIP", 255, 128, _ipv6_address_print},
            {"DIP", 127, 0, _ipv6_address_print},
            {"", 0, 0, NULL}
        }
    },

    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },

    {   35,
        4,
        {
            {"vrf", 275, 264, _uint16_print},
            {"in_rif", 259, 248, _uint16_print},
            {"SIP", 247, 120, _ipv6_address_print},
            {"DIP", 119, 0, _ipv6_address_print}
        }
    },

    {   6,
        4,
        {
            {"in_rif", 42, 31, _uint16_print},
            {"in_port", 30, 23, _uint8_print},
            {"exp", 22, 20, _uint8_print},
            {"mpls_label", 19, 0, _uint32_print}
        }
    },

    {   2,
        1,
        {
            {"trill", 15, 0, _uint16_print},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },

    {   4,
        3,
        {
            {"esdai", 31, 31, _uint8_print},
            {"fid_vsi", 30, 16, _uint16_print},
            {"dis_tree_nick", 15, 0, _uint16_print},
            {"", 0, 0, NULL}
        }
    },

    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },

    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },

    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    },

    {   0,
        0, 
        {
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL},
            {"", 0, 0, NULL}
        }
    }
};
#endif 





CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_diag[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_MODE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_MODE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_MODE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_MODE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_MODE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_MODE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_PKT_TRACE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_PKT_TRACE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_PKT_TRACE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_PARSING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_PARSING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_PARSING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TERMINATION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TERMINATION_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TERMINATION_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_TRAPS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_TRAPS_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_TRAPS_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_ALL_TO_CPU),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_ALL_TO_CPU_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_ALL_TO_CPU_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_STAT_RESTORE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_STAT_RESTORE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_STAT_RESTORE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LEARNING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LEARNING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LEARNING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_ENCAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_ENCAP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_DIAG_ENCAP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_EG_DROP_LOG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_EG_DROP_LOG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_EG_DROP_LOG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DB_TCAM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_SEND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_SEND_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_SEND_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_GET_ERRS_PTR),
  
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_DBG_VAL_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_LIF_DB_ID_TO_DB_TYPE_MAP_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_DIAG_PKT_ETH_HEADER_BUILD),


  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_diag[] =
{
  
  {
    SOC_PPC_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR,
    "SOC_PPC_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR",
    "The parameter 'pkt_trace' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_DIAG_PKT_TRACES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_DIAG_LEM_LKUP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VALID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VALID_OUT_OF_RANGE_ERR",
    "The parameter 'valid' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_DIAG_LEM_LKUP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BUFF_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BUFF_OUT_OF_RANGE_ERR",
    "The parameter 'buff' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'buff_len' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_DIAG_FLAVOR_OUT_OF_RANGE_ERR,
    "SOC_PPC_DIAG_FLAVOR_OUT_OF_RANGE_ERR",
    "The parameter 'flavor' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_DIAG_FLAVORS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR",
    "The parameter 'lkup_num' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BANK_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BANK_ID_OUT_OF_RANGE_ERR",
    "The parameter 'bank_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LKUP_USAGE_OUT_OF_RANGE_ERR",
    "The parameter 'lkup_usage' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_DIAG_TCAM_USAGES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_IN_TM_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'in_tm_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_PARSER_PROGRAM_POINTER_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_PARSER_PROGRAM_POINTER_OUT_OF_RANGE_ERR",
    "The parameter 'parser_program_pointer' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_PORT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_PACKET_QUAL_OUT_OF_RANGE_ERR",
    "The parameter 'packet_qual' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'code' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_TRAP_CODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_CPU_DEST_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_dest' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_IP_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_IP_OUT_OF_RANGE_ERR",
    "The parameter 'ip' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_BASE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'base_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_OPCODE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'length' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_FEC_PTR_OUT_OF_RANGE_ERR",
    "The parameter 'fec_ptr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_ENCAP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'encap_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETH_ENCAP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VLAN_TAG_FORMAT_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_tag_format' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_NEXT_PRTCL_OUT_OF_RANGE_ERR",
    "The parameter 'next_prtcl' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_L2_NEXT_PRTCL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_HDR_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'hdr_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_PKT_HDR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_HDR_OFFSET_OUT_OF_RANGE_ERR",
    "The parameter 'hdr_offset' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_RANGE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'range_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TERM_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'term_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_DIAG_MPLS_TERM_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'label' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_RANGE_BIT_OUT_OF_RANGE_ERR",
    "The parameter 'range_bit' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_FRWRD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_PKT_FRWRD_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VRF_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VRF_OUT_OF_RANGE_ERR",
    "The parameter 'vrf' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TRILL_UC_OUT_OF_RANGE_ERR",
    "The parameter 'trill_uc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_FRWRD_HDR_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'frwrd_hdr_index' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U8_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_VALID_FIELDS_OUT_OF_RANGE_ERR",
    "The parameter 'valid_fields' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_METER1_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_METER1_OUT_OF_RANGE_ERR",
    "The parameter 'meter1' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_METER2_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_METER2_OUT_OF_RANGE_ERR",
    "The parameter 'meter2' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_DP_METER_CMD_OUT_OF_RANGE_ERR",
    "The parameter 'dp_meter_cmd' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_COUNTER1_OUT_OF_RANGE_ERR",
    "The parameter 'counter1' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_COUNTER2_OUT_OF_RANGE_ERR",
    "The parameter 'counter2' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_CUD_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_CUD_OUT_OF_RANGE_ERR",
    "The parameter 'cud' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_DROP_LOG_OUT_OF_RANGE_ERR",
    "The parameter 'drop_log' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_ETHER_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'ether_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_TOTAL_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'total_size' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_NOF_PACKETS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_packets' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_INVALID_TM_PORT_ERR,
    "ARAD_PP_DIAG_INVALID_TM_PORT_ERR",
    "Egress information provided invalid tm port.\n\r ",    
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
  {
    ARAD_PP_DIAG_RESTORE_NOT_SAVED_ERR,
    "ARAD_PP_DIAG_RESTORE_NOT_SAVED_ERR",
    "try to restore traps status when not saved.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_DIAG_LIF_DB_ID_OUT_OF_RANGE_ERR",
    "LIF DB ID is not used. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_DIAG_INVALID_LAST_PACKET_ERR,
    "ARAD_PP_DIAG_INVALID_LAST_PACKET_ERR",
    "No relevant packet sent for diagnostics \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

#define ARAD_PP_DIAG_FILTER_NOF_ETPP_FILTERS (18)
#define ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_1 (32)
#define ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_2 (9)

#define ARAD_PP_DIAG_FILTER_TO_LOG(__trap_log,__trap_name)  \
    __trap_log[((__trap_name) >> 31) & 1] |= ((__trap_name) & 0x7FFFFFFF)

static uint32 arad_pp_diag_filter_bit_to_enum[] = 
{
    SOC_PPC_DIAG_EG_DROP_REASON_ILLEGAL_EEI,
    SOC_PPC_DIAG_EG_DROP_REASON_PHP_CONFLICT,
    SOC_PPC_DIAG_EG_DROP_REASON_POP_IP_VERSION_ERR,
    0,
    SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET,
    0,
    SOC_PPC_DIAG_EG_DROP_REASON_IP_TUNNEL_SNOOPED_PACKET,
    0,
    SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET,
    0,
    SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET,
    SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LINK_LAYER_ENTRY_NEEDED,
    SOC_PPC_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP,
    SOC_PPC_DIAG_EG_DROP_REASON_STP_BLOCK,
    SOC_PPC_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE,
    SOC_PPC_DIAG_EG_DROP_REASON_OUT_LIF_WITH_DROP,
    SOC_PPC_DIAG_EG_DROP_REASON_MTU_VIOLATION,
    SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LAST_ENTRY_NOT_AC,
    SOC_PPC_DIAG_EG_DROP_REASON_BOUNCE_BACK,
    SOC_PPC_DIAG_EG_DROP_REASON_OTM_INVALID,
    SOC_PPC_DIAG_EG_DROP_REASON_DSS_STACKING,
    SOC_PPC_DIAG_EG_DROP_REASON_EXCLUDE_SRC,
    SOC_PPC_DIAG_EG_DROP_REASON_LAG_MULTICAST,
    SOC_PPC_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP,
    SOC_PPC_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE,
    SOC_PPC_DIAG_EG_DROP_REASON_SRC_EQUAL_DEST,
    SOC_PPC_DIAG_EG_DROP_REASON_UNKNOWN_DA,
    SOC_PPC_DIAG_EG_DROP_REASON_SPLIT_HORIZON,
    SOC_PPC_DIAG_EG_DROP_REASON_PRIVATE_VLAN,
    SOC_PPC_DIAG_EG_DROP_REASON_TTL_SCOPE,
    SOC_PPC_DIAG_EG_DROP_REASON_MTU_VIOLATION,
    SOC_PPC_DIAG_EG_DROP_REASON_TRILL_TTL_ZERO,
    SOC_PPC_DIAG_EG_DROP_REASON_TRILL_SAME_INTERFACE,
    SOC_PPC_DIAG_EG_DROP_REASON_CNM,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_VERSION_ERR,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_VERSION_ERR,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_CHECKSUM_ERR,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_IHL_SHORT,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TOTAL_LEGNTH,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_1,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_1,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_WITH_OPTIONS,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_0,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_0,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_EQUAL_DIP,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_DIP_IS_ZERO,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_IS_MC,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_IS_MC,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DIP_UNSPECIFIED,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_UNSPECIFIED,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LOOPBACK,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_HOP_BY_HOP,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_DEST,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_DEST,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_SRC,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_SRC,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_COMPATIBLE,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_MAPPED,
    SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DEST_MC,
};


#ifdef SAND_LOW_LEVEL_SIMULATION
#define ARAD_DEBUG_INTERNAL (1)
#endif





uint32
  arad_pp_diag_init_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core_id

)
{
  return arad_pp_diag_pkt_trace_clear_unsafe(unit, core_id, SOC_PPC_DIAG_PKT_TRACE_ALL);
}


uint32
  arad_pp_diag_is_valid(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  uint32             func_name,
    SOC_SAND_OUT uint32 *           ret_val
  )
{
  uint32 reg_val;
  uint32
    res = SOC_SAND_OK;
  uint64 reg_val64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!SOC_IS_ARDON(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core_id, &reg_val64));


      if (!COMPILER_64_IS_ZERO(reg_val64)) {
          *(ret_val) = 1;
          goto exit;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_DBG_FLP_PROGRAM_SELECTION_CAM_LINEr(unit, core_id, &reg_val64));
      if (!COMPILER_64_IS_ZERO(reg_val64)) {
          *(ret_val) = 1;
          goto exit;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_DBG_FLP_SELECTED_PROGRAMr(unit, core_id, &reg_val));
      if (reg_val != 0) {
          *(ret_val) = 1;
          goto exit;
      }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHB_DBG_PMF_SELECTED_CAM_LINE_0r(unit, core_id, &reg_val));
  if (reg_val != 0) {
      *(ret_val) = 1;
      goto exit;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_IHB_DBG_PMF_SELECTED_CAM_LINE_1r(unit, core_id,&reg_val));
  if (reg_val != 0) {
      *(ret_val) = 1;
      goto exit;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_IHB_DBG_PMF_SELECTED_PROGRAMr(unit, core_id,&reg_val));
  if (reg_val != 0) {
      *(ret_val) = 1;
      goto exit;
  }

  *(ret_val) = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_is_valid()", 0, 0);
}




#if defined(ARAD_DEBUG_INTERNAL) && ARAD_DEBUG_INTERNAL
 STATIC uint32
   arad_pp_diag_dbg_sim_fil_val(
   SOC_SAND_IN  uint32               sample_id,
   SOC_SAND_IN  uint32               blk,
   SOC_SAND_IN  uint32               addr,
   SOC_SAND_OUT uint32               regs_val[ARAD_PP_DIAG_DBG_VAL_LEN]
 )
 {
   return 0;
 }
#endif

uint32
  arad_pp_diag_dbg_val_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core_id,
    SOC_SAND_IN  uint32               blk,
    SOC_SAND_IN  ARAD_PP_DIAG_REG_FIELD  *fld,
    SOC_SAND_OUT uint32               val[ARAD_PP_DIAG_DBG_VAL_LEN]
  )
{
  uint32
    reg32 = 0,
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    data_above_64,
    field_above_64;
  soc_reg_t
      reg_00e0r, reg_00e1r, 
      ihp_00e0r = SOC_IS_JERICHO_PLUS(unit) ? IHP_REG_0110r : (SOC_IS_JERICHO(unit)? IHP_REG_010Cr: IHP_REG_00E0r),
      ihp_00e1r = SOC_IS_JERICHO_PLUS(unit) ? IHP_REG_0111r : (SOC_IS_JERICHO(unit)? IHP_REG_010Dr: IHP_REG_00E1r),
      ihb_00e0r = SOC_IS_JERICHO(unit)? IHB_REG_00FEr: IHB_REG_00E0r,
      ihb_00e1r  = SOC_IS_JERICHO(unit)? IHB_REG_00FFr: IHB_REG_00E1r;
      
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_PP_CLEAR(val,uint32,ARAD_PP_DIAG_DBG_VAL_LEN); 

  if ((blk == ARAD_IHP_ID) || (blk == ARAD_IHB_ID))
  {
    reg_00e0r = (blk == ARAD_IHP_ID)? ihp_00e0r: ihb_00e0r;
    reg_00e1r = (blk == ARAD_IHP_ID)? ihp_00e1r: ihb_00e1r;
    ARAD_FLD_TO_REG(reg_00e0r, FIELD_0_30f, fld->base, reg32, 11, exit);
    ARAD_FLD_TO_REG(reg_00e0r, FIELD_31_31f, 1, reg32, 16, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_set(unit, reg_00e0r, core_id, 0, reg32));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, soc_reg_above_64_get(unit, reg_00e1r, core_id, 0, data_above_64));
    soc_reg_above_64_field_get(unit, reg_00e1r , data_above_64, FIELD_0_255f, field_above_64);    
  }
  else if (blk == ARAD_EGQ_ID)
  {
      ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_ADDRESSf, fld->base, reg32, 10, exit);
      ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, 1, reg32, 15, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_DBG_COMMANDr(unit, core_id,  reg32));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EGQ_DBG_DATAr(unit, core_id, data_above_64));
      soc_reg_above_64_field_get(unit, EGQ_DBG_DATAr, data_above_64, DBG_DATAf, field_above_64);    
  }
  else if (blk == ARAD_EPNI_ID)
  {
      ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_ADDRESSf, fld->base, reg32, 10, exit);
      ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, 1, reg32, 15, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_DBG_COMMANDr(unit, core_id,  reg32));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_DBG_DATAr(unit, core_id, data_above_64));
      soc_reg_above_64_field_get(unit, EPNI_DBG_DATAr, data_above_64, DBG_DATAf, field_above_64);    
  }

#if defined(ARAD_DEBUG_INTERNAL) && ARAD_DEBUG_INTERNAL
  arad_pp_diag_dbg_sim_fil_val(1,blk, fld->base,field_above_64);
#endif


  res = soc_sand_bitstream_get_any_field(field_above_64,fld->lsb,(fld->msb - fld->lsb +1),val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_dbg_val_get_unsafe()", 0, 0);
}
 

STATIC uint32
  arad_pp_diag_term_code_to_header_index(
    SOC_SAND_IN  SOC_PPC_PKT_TERM_TYPE    term_type,
    SOC_SAND_IN  uint32                   lbl_index
  )
{
  uint32
    nof_labels=0;

  switch(term_type)
  {
  case SOC_PPC_PKT_TERM_TYPE_MPLS_ETH:
  case SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH:
    nof_labels = 1;
    break;
  case SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH:
  case SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH:
    nof_labels = 2;
    break;
  case SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH:
  case SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH:
    nof_labels = 3;
    break;
  default:
    nof_labels = 0;
  break;
  }
  
  if (lbl_index >= nof_labels)
  {
    return 0;
  }

  
  return 1 + lbl_index;
}


STATIC uint32
  arad_pp_diag_is_trap_code(
    SOC_SAND_IN  uint32                                 trap_code
  )
{
    if(trap_code >= SOC_PPC_TRAP_CODE_INTERNAL_BLOCK_PREFIX && 
       trap_code <= SOC_PPC_TRAP_CODE_INTERNAL_BLOCK_PREFIX + 4
       ) {
        return 0;
    }
    return 1;
}



uint32
  arad_pp_diag_fwd_decision_in_buffer_parse(
    SOC_SAND_IN  int                                  unit,    
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE signal_type,
    SOC_SAND_IN  uint32                                  dest_buffer,
    SOC_SAND_IN  uint32                                  asd_buffer,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO             *fwd_decision
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 flags = ARAD_PP_FWD_DECISION_PARSE_DEST|ARAD_PP_FWD_DECISION_PARSE_EEI;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  if(signal_type == ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM){
      flags = flags | ARAD_PP_FWD_DECISION_APP_TYPE_IS_TM;
  }

  res = arad_pp_fwd_decision_in_buffer_parse(
        unit,
        dest_buffer,
        asd_buffer,
        flags,
        fwd_decision
      );

  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_fwd_decision_in_buffer_parse()", 0, 0);

}


STATIC SOC_PPC_DIAG_FWD_LKUP_TYPE
  arad_pp_diag_frwrd_type_to_lkup_type_map(
    SOC_SAND_IN  SOC_PPC_PKT_FRWRD_TYPE   frwrd_type
  )
{
  switch(frwrd_type)
  {
  case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC;
  case SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC;
  case SOC_PPC_PKT_FRWRD_TYPE_MPLS:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM;
  case SOC_PPC_PKT_FRWRD_TYPE_TRILL:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC;
  case SOC_PPC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT;
  case SOC_PPC_PKT_FRWRD_TYPE_CUSTOM1:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF;
  case SOC_PPC_PKT_FRWRD_TYPE_CPU_TRAP:
  default:
    return SOC_PPC_DIAG_FWD_LKUP_TYPE_NONE;
  }
}

uint32
  arad_pp_diag_traps_range_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            regs_val[2],
    SOC_SAND_IN  uint32                            rng_id,
    SOC_SAND_IN  uint32                            nof_traps,
    SOC_SAND_OUT SOC_PPC_DIAG_TRAPS_INFO           *traps_info
  )
{
  uint32
    indx;
  SOC_PPC_TRAP_CODE
    trap_code;
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TRAPS_RANGE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  for (indx = 0; indx < nof_traps; ++indx)
  {
    if (soc_sand_bitstream_test_bit(regs_val, indx))
    {
      res = arad_pp_trap_sb_to_trap_code_map_get(
              unit,
              rng_id,
              indx,
              &trap_code
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_sand_bitstream_set_bit(
              traps_info->trap_stack,
              trap_code
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      

      res = arad_pp_trap_mgmt_trap_code_to_trap_strength(unit, trap_code, 
            &traps_info->trap_stack_strength[trap_code], &traps_info->additional_trap_stack_strength[trap_code]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_traps_range_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_UNSAFE);
  
  res = arad_diag_sample_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_set_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_sample_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  enable
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_SET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_set_verify()", 0, 0);
}

uint32
  arad_pp_diag_sample_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_SAMPLE_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = arad_diag_sample_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_sample_enable_get_unsafe()", 0, 0);
}


uint32
  arad_pp_diag_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO                 *mode_info
  )
{
  soc_error_t
    rv = SOC_E_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_MODE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  rv = sw_state_access[unit].dpp.soc.arad.pp.diag.mode_info.set(
          unit,
          mode_info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_MODE_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_DIAG_MODE_INFO, mode_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_set_verify()", 0, 0);
}

uint32
  arad_pp_diag_mode_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_MODE_INFO_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_DIAG_MODE_INFO                      *mode_info
  )
{
  soc_error_t
    rv = SOC_E_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_MODE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode_info);


  rv = sw_state_access[unit].dpp.soc.arad.pp.diag.mode_info.get(
          unit,
          mode_info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_VSI_INFO_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vsi_mode_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VSI_INFO                      *vsi_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_VSI_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  SOC_PPC_DIAG_VSI_INFO_clear(vsi_info);

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "VSI", &vsi_info->vsi, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_mode_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_diag_get_raw_signal(
      int core_id,
      ARAD_MODULE_ID prm_blk,
      int prm_addr_msb,
      int prm_addr_lsb,
      int prm_fld_msb,
      int prm_fld_lsb,
      uint32 *regs_val)
{
    ARAD_PP_DIAG_REG_FIELD 
        fld;

    uint32
         unit = 0,
         res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_GET_EPNI_RAW_SIGNAL);

    if (regs_val == NULL) {
        res = SOC_SAND_ERR;
        goto exit;
    }
    
    
    ARAD_PP_DIAG_FLD_READ(&fld, core_id, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb, 20);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_epni_raw_signal()", 0, 0);

}


uint32
  arad_pp_diag_pkt_trace_clear_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                  core_id,
    SOC_SAND_IN  uint32                               pkt_trace
  )
{
  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA
    ihb_fec_entry_accessed_tbl_data;
  uint64
    clear_val_64 = COMPILER_64_INIT(0, 0);
  uint32
    clear_val = 0xFFFFFFFF;
  uint32
    res = SOC_SAND_OK;
  uint64
    field64;
  uint32 
      nof_flp_program_selection_lines;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_PKT_TRACE_CLEAR_UNSAFE);

  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_FEC)
  {
    ihb_fec_entry_accessed_tbl_data.fec_entry_accessed = 0;
    res = arad_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
            unit,
            MEM_BLOCK_ANY,
            ARAD_PP_FIRST_TBL_ENTRY,
            &ihb_fec_entry_accessed_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_LIF)
  {
      COMPILER_64_SET(clear_val_64,  0, SOC_SAND_U32_MAX );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR, WRITE_IHP_VTT_PROGRAM_ENCOUNTEREDr(unit, core_id, clear_val_64));
  }
  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_FLP)
  {
      nof_flp_program_selection_lines = SOC_DPP_DEFS_GET(unit, nof_flp_program_selection_lines);

      
      if(nof_flp_program_selection_lines > 64) {
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
      }
      if(nof_flp_program_selection_lines < 64) {
          COMPILER_64_SET(clear_val_64, 0, 1);
          COMPILER_64_SHL(clear_val_64, nof_flp_program_selection_lines);
          COMPILER_64_SUB_32(clear_val_64, 1);
      } else {
          COMPILER_64_SET(clear_val_64, SOC_SAND_U32_MAX , SOC_SAND_U32_MAX );
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_PROGRAM_SELECTION_CAM_LINEr(unit, core_id, clear_val_64));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_SELECTED_PROGRAMr(unit, core_id, clear_val));
  }

  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_PMF)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_PMF_SELECTED_CAM_LINE_0r(unit, core_id,  clear_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  47,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_PMF_SELECTED_CAM_LINE_1r(unit, core_id,  clear_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  48,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_PMF_SELECTED_PROGRAMr(unit, core_id,  clear_val));  
  }

  
  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_TRAP)
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_LLR_TRAP_0r(unit, core_id,  clear_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_LLR_TRAP_1r(unit, core_id,  clear_val));
    
    COMPILER_64_SET(field64,0,clear_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_VTT_TRAPS_ENCOUNTEREDr(unit, core_id,  &field64));
    
    COMPILER_64_SET(field64,0,clear_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_TRAP_0r(unit, core_id, clear_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_DBG_FLP_TRAP_1r(unit, core_id, clear_val));
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_DBG_FER_TRAPr(unit, core_id,  clear_val));
  }

  if (pkt_trace & SOC_PPC_DIAG_PKT_TRACE_EG_DROP_LOG)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_PP_INT_REGr(unit, core_id,  clear_val));
    if (SOC_IS_QUX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INT_REGr(unit, clear_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INT_REG_2r(unit, clear_val));
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr(unit, core_id, clear_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_ERPP_DISCARD_INTERRUPT_REGISTER_2r(unit, core_id, clear_val));
    }
  }

exit:

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_trace_clear_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_pkt_trace_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pkt_trace
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_PKT_TRACE_CLEAR_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pkt_trace, SOC_PPC_DIAG_PKT_TRACE_MAX, SOC_PPC_DIAG_PKT_TRACE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_trace_clear_verify()", 0, 0);
}


uint32
  arad_pp_diag_received_packet_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_RECEIVED_PACKET_INFO      *rcvd_pkt_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  )
{
  uint32
    dummy,
    tmp=0;
  uint8
    is_lag;
  uint32
    res = SOC_SAND_OK, valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rcvd_pkt_info);

  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(rcvd_pkt_info);

  *ret_val = SOC_PPC_DIAG_OK; 

  res = arad_pp_diag_is_valid(unit, core_id, SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  dpp_dsig_read(unit, core_id, "IRPP", "NIF", NULL, "Port_Termination_Context", &rcvd_pkt_info->in_tm_port, 1);
  dpp_dsig_read(unit, core_id, "IRPP", NULL, "Parser", "Parser_Program_Ptr", &tmp, 1);

  switch(tmp) {
  case ARAD_PARSER_PROG_RAW_ADDR_START:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_RAW;
    break;

  case ARAD_PARSER_PROG_TM_ADDR_START:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_TM;
    break;

  case ARAD_PARSER_PROG_ETH_ADDR_START:
  case ARAD_PARSER_PROG_RAW_MPLS_ADDR_START:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_ETH;
    break;

  case ARAD_PARSER_PROG_FTMH_ADDR_START:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_STACKING;
    break;

  default:
    rcvd_pkt_info->pp_context = ARAD_PORT_HEADER_TYPE_NONE;
  }

  dpp_dsig_read(unit, core_id, "IRPP", "NIF", NULL, "Packet_Header_Size", &rcvd_pkt_info->packet_header.buff_len, 1);
  
  rcvd_pkt_info->packet_header.buff_len += 1;

  dpp_dsig_read(unit, core_id, "IRPP", "NIF", NULL, "Packet_Header", rcvd_pkt_info->packet_header.buff, 32);
  dpp_dsig_read(unit, core_id, "IRPP", NULL, "Parser", "Src_System_Port", &tmp, 1);
  res = arad_ports_logical_sys_id_parse(
          unit,
          tmp, 
          &is_lag,
          &(rcvd_pkt_info->src_sys_port.sys_id),
          &dummy,
          &(rcvd_pkt_info->src_sys_port.sys_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  if (is_lag)
  {
    rcvd_pkt_info->src_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
  }
  else
  {
    rcvd_pkt_info->src_sys_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
  }

  dpp_dsig_read(unit, core_id, "IRPP", NULL, "Parser", "In_PP_Port", &rcvd_pkt_info->in_pp_port, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_received_packet_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_received_packet_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_received_packet_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_parsing_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PARSING_INFO              *pars_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  )
{
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    act_hdr_index,
    hdr_indx,
    inner_eth_indx=0,
    outer_eth_indx=0,
    qual[2],
    in_port,
    hdr_qual,
    lst_mpls_header=0;
  uint8
    inc_bos=FALSE,
    outer_eth_vld = FALSE,
    inner_eth_vld = FALSE,
    prev_mpls=FALSE,
    found,
    l3_next_prtcl_ndx;
  SOC_PPC_PORT_INFO
    port_info;
  SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO
    tpid_profile_info;
  SOC_PPC_LLP_PARSE_TPID_VALUES
    tpid_vals;
  uint32
    res = SOC_SAND_OK;
  uint32
      valid;
  SOC_PPC_L3_NEXT_PRTCL_TYPE l3_next_prtcl_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_PARSING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pars_info);

  SOC_PPC_DIAG_PARSING_INFO_clear(pars_info);

  *ret_val = SOC_PPC_DIAG_OK;

  res = arad_pp_diag_is_valid(unit, core_id, SOC_PPC_DIAG_PARSING_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Packet_Format_Code", regs_val, 1);
  pars_info->pfc_hw = regs_val[0];

  dpp_dsig_read(unit, core_id, "IRPP", "Parser", "LL", "Packet_Format_Qualifier", qual, 2);
  dpp_dsig_read(unit, core_id, "IRPP", "Parser", "LL", "Header_Offset", regs_val, 2);
  for (act_hdr_index = 0, hdr_indx = 0 ; hdr_indx < SOC_PPC_DIAG_MAX_NOF_HDRS; ++hdr_indx, ++act_hdr_index)
  {
    
    pars_info->hdrs_stack[hdr_indx].hdr_type = dpp_parser_pfc_hdr_type_at_index(pars_info->pfc_hw, hdr_indx);
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_ETH && outer_eth_vld == FALSE)
    {
      outer_eth_indx = hdr_indx;
      outer_eth_vld = TRUE;
    }
    else if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_ETH)
    {
      inner_eth_indx = hdr_indx;
      inner_eth_vld = TRUE;
    }

    
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_NONE)
    {
      break;
    }

    
    if (prev_mpls && pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_MPLS)
    {
      
      pars_info->hdrs_stack[hdr_indx].hdr_offset = pars_info->hdrs_stack[hdr_indx-1].hdr_offset + ARAD_PP_DIAG_HDR_MPLS_BYTE_SIZE;
      
      --act_hdr_index;
    }
    else
    {
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              ARAD_PP_DIAG_HDR_INDEX_SIZE* (act_hdr_index + 1),
              ARAD_PP_DIAG_HDR_INDEX_SIZE,
              &(pars_info->hdrs_stack[hdr_indx].hdr_offset)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      hdr_qual = 0;
      res = soc_sand_bitstream_get_any_field(
              qual,
              ARAD_PP_DIAG_HDR_QUAL_SIZE*act_hdr_index,
              ARAD_PP_DIAG_HDR_QUAL_SIZE,
              &hdr_qual
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (
          (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_IPV4) ||
          (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_IPV6)
         )
      {
        l3_next_prtcl_ndx = SOC_SAND_GET_BITS_RANGE(hdr_qual,10,7);
        res = arad_pp_ndx_to_l3_prtcl(l3_next_prtcl_ndx, &l3_next_prtcl_type, &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        pars_info->hdrs_stack[hdr_indx].ip.next_prtcl = l3_next_prtcl_type;
        pars_info->hdrs_stack[hdr_indx].ip.hdr_err = SOC_SAND_GET_BIT(hdr_qual,0);
        pars_info->hdrs_stack[hdr_indx].ip.is_mc = SOC_SAND_GET_BIT(hdr_qual,5);
        pars_info->hdrs_stack[hdr_indx].ip.is_fragmented = SOC_SAND_GET_BIT(hdr_qual,6);
      }
      else if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_MPLS)
      {
        inc_bos = (uint8)SOC_SAND_GET_BIT(hdr_qual,0);
      }
      else if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_ETH)
      {
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.inner_tpid = SOC_SAND_GET_BITS_RANGE(hdr_qual,1,0);
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.is_outer_prio = (uint8)SOC_SAND_GET_BITS_RANGE(hdr_qual,2,2);
        pars_info->hdrs_stack[hdr_indx].eth.tag_fromat.outer_tpid = SOC_SAND_GET_BITS_RANGE(hdr_qual,4,3);
        pars_info->hdrs_stack[hdr_indx].eth.encap_type = (SOC_SAND_PP_ETH_ENCAP_TYPE)SOC_SAND_GET_BITS_RANGE(hdr_qual,6,5);
      }
    }

    
    if (pars_info->hdrs_stack[hdr_indx].hdr_type == SOC_PPC_PKT_HDR_TYPE_MPLS)
    {
      lst_mpls_header = hdr_indx;
      prev_mpls = TRUE;
    }
  }
  
  if (lst_mpls_header != 0)
  {
    pars_info->hdrs_stack[lst_mpls_header].mpls.bos = inc_bos;
  }
  

  
  if (outer_eth_vld)
  {
    dpp_dsig_read(unit, core_id, "IRPP", "Parser", "LL", "Packet_Format_Qualifier.Header_1.next_protocol", regs_val, 1);
    res = arad_pp_l2_next_prtcl_interanl_val_to_type_map(
            regs_val[0],
            &(pars_info->hdrs_stack[outer_eth_indx].eth.next_prtcl),
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Incoming_Tag_Structure", regs_val, 1);
    pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tag_format = (SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT)regs_val[0];

    dpp_dsig_read(unit, core_id, "IRPP", "LL", "VT", "Outer_VID", &pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].vid, 1);
    dpp_dsig_read(unit, core_id, "IRPP", "LL", "VT", "Inner_VID", &pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].vid, 1);
  }

  dpp_dsig_read(unit, core_id, "IRPP", "LL", "VT", "Initial_VID", &pars_info->initial_vid, 1);
 
  dpp_dsig_read(unit, core_id, "IRPP", NULL, "Parser", "In_PP_Port", &in_port, 1);
  
  res = arad_pp_port_info_get_unsafe(
          unit,
          SOC_CORE_DEFAULT,
          in_port,
          &port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_llp_parse_tpid_profile_info_get_unsafe(
          unit,
          port_info.tpid_profile,
          &tpid_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_llp_parse_tpid_values_get_unsafe(
          unit,
          &tpid_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  
  if (outer_eth_vld && pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_NONE)
  {
    
    if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
    else if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];  
  }
  else
  {
      
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[0].vid = 0;
  }

  
  if (outer_eth_vld && pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.inner_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_NONE)
  {
    
    if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
    else if (pars_info->hdrs_stack[outer_eth_indx].eth.tag_fromat.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2)
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];  
  }
  else
  {
      
      pars_info->hdrs_stack[outer_eth_indx].eth.vlan_tags[1].vid = 0;
  }

  if (inner_eth_vld)
  {
    
    if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.outer_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_NONE)
    {
      
      if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1)
        pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
      else if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.outer_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2)
        pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];
    }
    else
    {
      
      pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[0].vid = 0;
    }  
  
    
    if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid != SOC_PPC_LLP_PARSE_TPID_INDEX_NONE)
    {
      
      if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1)
        pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid1.index];
      else if (pars_info->hdrs_stack[inner_eth_indx].eth.tag_fromat.inner_tpid == SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2)
        pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[1].tpid = tpid_vals.tpid_vals[tpid_profile_info.tpid2.index];
    }
    else
    {
      
      pars_info->hdrs_stack[inner_eth_indx].eth.vlan_tags[1].vid = 0;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_parsing_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_parsing_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_PARSING_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_parsing_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_termination_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TERM_INFO                 *term_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  )
{
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    lbl_indx,
    frwrd_offset = 0,
    qual,
    hdr_index;
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_DIAG_PARSING_INFO
    pars_info;
  uint32  valid;
  uint32 pfc_hw;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TERMINATION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_info);

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_TERMINATION_INFO_GET, &valid);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  *ret_val = SOC_PPC_DIAG_OK; 
  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  SOC_PPC_DIAG_TERM_INFO_clear(term_info);

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "TT_Code", &term_info->term_type, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "Parser", "LL", "Packet_Format_Qualifier", regs_val, 2);

  
  for (lbl_indx=0; lbl_indx <3; ++lbl_indx)
  {
    hdr_index = arad_pp_diag_term_code_to_header_index(term_info->term_type,0);
    if (hdr_index == 0)
    {
      break;
    }
    
    qual = 0;
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            hdr_index*ARAD_PP_DIAG_HDR_QUAL_SIZE, 
            ARAD_PP_DIAG_HDR_QUAL_SIZE,
            &qual
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Code", &term_info->frwrd_type, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Offset_Index", &frwrd_offset, 1);

  
  res = arad_pp_diag_parsing_info_get_unsafe(unit,core_id,&pars_info, ret_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  if(soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0) == 2)
  {
       
        res = dpp_parser_pfc_get_hw_by_sw(unit, DPP_PFC_ETH_ETH, &pfc_hw, NULL);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        term_info->is_scnd_my_mac =  ((frwrd_offset >  4) ||
                                     ((frwrd_offset == 4) &&
                                         !(((pars_info.hdrs_stack[1].hdr_type == SOC_PPC_PKT_HDR_TYPE_IPV4) ||
                                             (pars_info.hdrs_stack[1].hdr_type == SOC_PPC_PKT_HDR_TYPE_IPV6)) &&
                                             (pars_info.hdrs_stack[1].ip.next_prtcl == SOC_PPC_L3_NEXT_PRTCL_TYPE_MPLS))) ||
                                     ((frwrd_offset == 3) && (pars_info.pfc_hw == pfc_hw)));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_termination_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_termination_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_TERMINATION_INFO_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_termination_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_frwrd_lpm_lkup_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                             *fec_ptr,
    SOC_SAND_OUT uint8                            *found,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT              *ret_val

  )
{
  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY
      test_lpm_key;
   
  uint32
    res = SOC_SAND_OK;
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lpm_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_ptr);
  SOC_SAND_CHECK_NULL_INPUT(found);

  *ret_val = SOC_PPC_DIAG_OK;


  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  ARAD_PP_DIAG_IPV4_TEST_VPN_KEY_clear(&test_lpm_key);
  sal_memcpy(&test_lpm_key.key,&lpm_key->key,sizeof(SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY));
  test_lpm_key.vrf = lpm_key->vrf;

  res = arad_pp_frwrd_ipv4_test_lpm_lkup_get(unit,&test_lpm_key,fec_ptr,found);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lpm_lkup_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_frwrd_lpm_lkup_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY              *lpm_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET_VERIFY);

  res = SOC_PPC_DIAG_IPV4_VPN_KEY_verify(unit, lpm_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lpm_lkup_get_verify()", 0, 0);
}

#if defined(INCLUDE_KBP)
STATIC void
    arad_pp_diag_request_to_structure(
        SOC_SAND_IN  uint32 *request_buffer,
        SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID type,
        SOC_SAND_OUT void **params)
{
    uint32
        nof_params = forwarding_type_parameters[type].nof_params;
    uint32
        param_ndx;
    uint32
        tmp;
    CONST ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER
        *cur_param;
    for(param_ndx = 0; param_ndx < nof_params; ++param_ndx) {
        cur_param = &forwarding_type_parameters[type].params[param_ndx];
        
        if(cur_param->msb - cur_param->lsb + 1 > 16) {
                SHR_BITCOPY_RANGE((uint32*) params[param_ndx],
                                  0,
                                  request_buffer,
                                  cur_param->lsb,
                                  cur_param->msb - cur_param->lsb + 1);
        } else {
            tmp = 0;
            SHR_BITCOPY_RANGE(&tmp,
                              0,
                              request_buffer,
                              cur_param->lsb,
                              cur_param->msb - cur_param->lsb + 1);
            if(cur_param->msb - cur_param->lsb + 1 <= 8) {
                *((uint8*)params[param_ndx]) = tmp & 0xff;
            } else {
                *((uint16*)params[param_ndx]) = tmp & 0xffff;
            }
        }
    }
}

STATIC SOC_PPC_DIAG_IPV4_UNICAST_RPF
  arad_pp_diag_request_to_ipv4_unicast_rpf(uint32 *request_buffer)
{
  void
    *params[3];
  SOC_PPC_DIAG_IPV4_UNICAST_RPF
    res;
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_clear(&res);
  params[0] = &res.vrf;
  params[1] = &res.sip;
  params[2] = &res.dip;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, params);
  
  return res;
}
  
STATIC SOC_PPC_DIAG_IPV4_MULTICAST
  arad_pp_diag_request_to_ipv4_multicast(uint32 *request_buffer)
{
  void
    *params[5];
  SOC_PPC_DIAG_IPV4_MULTICAST
    res;

  SOC_PPC_DIAG_IPV4_MULTICAST_clear(&res);
  params[0] = &res.vrf;
  params[1] = &res.in_rif;
  params[2] = &res.sip;
  params[3] = &res.dip;
  params[4] = &res.dip;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, params);
    
  return res;
}

STATIC SOC_PPC_DIAG_IPV6_UNICAST_RPF
  arad_pp_diag_request_to_ipv6_unicast_rpf(uint32 *request_buffer)
{
  void
    *params[3];
  SOC_PPC_DIAG_IPV6_UNICAST_RPF
    res;

  SOC_PPC_DIAG_IPV6_UNICAST_RPF_clear(&res);
  params[0] = &res.vrf;
  params[1] = res.sip.address;
  params[2] = res.dip.address;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, params);
  
  return res;
}

STATIC SOC_PPC_DIAG_IPV6_MULTICAST
  arad_pp_diag_request_to_ipv6_multicast(uint32 *request_buffer)
{
  void
    *params[5];
  SOC_PPC_DIAG_IPV6_MULTICAST
    res;

  SOC_PPC_DIAG_IPV6_MULTICAST_clear(&res);
  params[0] = &res.vrf;
  params[1] = &res.in_rif;
  params[2] = res.sip.address;
  params[3] = res.dip.address;
  params[4] = res.dip.address;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, params);

  return res;
}

STATIC SOC_PPC_DIAG_MPLS
  arad_pp_diag_request_to_mpls(uint32 *request_buffer)
{
  void
    *params[4];
  SOC_PPC_DIAG_MPLS
    res;

  SOC_PPC_DIAG_MPLS_clear(&res);
  params[0] = &res.in_rif;
  params[1] = &res.in_port;
  params[2] = &res.exp;
  params[3] = &res.mpls_label;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_LSR, params);
  
  return res;
}

STATIC SOC_PPC_DIAG_TRILL_UNICAST
  arad_pp_diag_request_to_trill_unicast(uint32 *request_buffer)
{
  void
    *params[1];
  SOC_PPC_DIAG_TRILL_UNICAST
    res;

  SOC_PPC_DIAG_TRILL_UNICAST_clear(&res);
  params[0] = &res.egress_nick;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, params);
  
  return res;
}

STATIC SOC_PPC_DIAG_TRILL_MULTICAST
  arad_pp_diag_request_to_trill_multicast(uint32 *request_buffer)
{
  void
    *params[3];
  SOC_PPC_DIAG_TRILL_MULTICAST
    res;

  SOC_PPC_DIAG_TRILL_MULTICAST_clear(&res);
  params[0] = &res.esdai;
  params[1] = &res.fid_vsi;
  params[2] = &res.dist_tree_nick;
  arad_pp_diag_request_to_structure(request_buffer, ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, params);
  
  return res;
}

STATIC uint32
  arad_pp_diag_reply_to_ip_reply_record(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 *reply,
    SOC_SAND_OUT SOC_PPC_DIAG_IP_REPLY_RECORD* ip_reply_record_rpf
  )
{
  uint32
    tmp,
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(ip_reply_record_rpf);

  
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 55, 1);
  ip_reply_record_rpf->match_status = tmp & 0xff;

  
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 42, 1);
  ip_reply_record_rpf->is_synamic = tmp & 0xff;

  
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 41, 1);
  ip_reply_record_rpf->p2p_service = tmp & 0xff;

  
  tmp = 0;
  SHR_BITCOPY_RANGE(&tmp, 0, reply, 39, 2);
  ip_reply_record_rpf->identifier = tmp & 0xff;

  if(ip_reply_record_rpf->identifier == 0x0) {
    
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 35, 2);
    ip_reply_record_rpf->out_lif_valid = tmp & 0xff;

    
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 19, 16);
    ip_reply_record_rpf->out_lif = tmp & 0xffff;

    
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 0, 19);
    
    res = arad_pp_fwd_decision_in_buffer_parse(
            unit,
            tmp,
            0,
            ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_LEGACY,
            &ip_reply_record_rpf->destination
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  } else if(ip_reply_record_rpf->identifier == 0x1) {
    
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 16, 23);
    ip_reply_record_rpf->eei = tmp;
    
    
    tmp = 0;
    SHR_BITCOPY_RANGE(&tmp, 0, reply, 0, 15);
    ip_reply_record_rpf->fec_ptr =tmp & 0xff;
  } else {
  
    SOC_SAND_SET_ERROR_CODE(0, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_reply_to_ip_reply_record_rpf()",0,0);
}    

STATIC void
    ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(
        SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID type,
        SOC_SAND_IN void **params,
        SOC_SAND_IN uint32 offset
    )
{
    uint32
        nof_params = forwarding_type_parameters[type].nof_params;
    uint32
        param_ndx;
    CONST ARAD_PP_DIAG_FORWARDING_TYPE_PARAMETER
        *cur_param;
    uint32
        offset_lcl = offset - 8*forwarding_type_parameters[type].size_in_bytes;

    LOG_CLI((BSL_META("Master Key:\n\r")));
    LOG_CLI((BSL_META("\n\r")));
    LOG_CLI((BSL_META("-------------------------------------------------------------------\n\r")));
    LOG_CLI((BSL_META("%-20s|%-4s|%-4s|%-35s|\n\r"), "Field name", "MSB", "LSB", "Last-packet Value"));
    LOG_CLI((BSL_META("-------------------------------------------------------------------\n\r")));
    for(param_ndx = 0; param_ndx < nof_params; ++param_ndx) {
        cur_param = &forwarding_type_parameters[type].params[param_ndx];
        cur_param->print_function(cur_param->name, cur_param->msb + offset_lcl, cur_param->lsb + offset_lcl, params[param_ndx]);
    }
}
 
uint32
    arad_pp_diag_get_frwrd_type_size(
        SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID type,
        SOC_SAND_OUT uint32                   *size
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(size);

    SOC_SAND_ERR_IF_ABOVE_NOF(type, ARAD_KBP_FRWRD_IP_NOF_TABLES, 1, 10, exit);
    
    *size = forwarding_type_parameters[type].size_in_bytes;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_frwrd_type_size()",0,0);
}

ARAD_KBP_FRWRD_IP_TBL_ID
    arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(
        SOC_SAND_IN SOC_PPC_DIAG_FWD_LKUP_TYPE type
    )
{
    switch (type) {
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_NONE:
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV4_MC;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
            return ARAD_KBP_FRWRD_TBL_ID_IPV6_MC;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
            return ARAD_KBP_FRWRD_TBL_ID_LSR;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
            return ARAD_KBP_FRWRD_TBL_ID_TRILL_UC;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
            return ARAD_KBP_FRWRD_TBL_ID_TRILL_MC;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF:
        case SOC_PPC_NOF_DIAG_FWD_LKUP_TYPES:
        default:
            return ARAD_KBP_FRWRD_IP_NOF_TABLES;
    }
}
 
void
  SOC_PPC_DIAG_IPV4_UNICAST_RPF_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_UNICAST_RPF *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[3];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
  params[1] = &info->sip;
  params[2] = &info->dip;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV4_MULTICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV4_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[4];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
  params[1] = &info->in_rif;
  params[2] = &info->sip;
  params[3] = &info->dip;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_UNICAST_RPF_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_UNICAST_RPF *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[3];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
  params[1] = &info->sip;
  params[2] = &info->dip;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_IPV6_MULTICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_IPV6_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[4];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->vrf;
  params[1] = &info->in_rif;
  params[2] = &info->sip;
  params[3] = &info->dip;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_MPLS_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_MPLS *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[4];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->in_rif;
  params[1] = &info->in_port;
  params[2] = &info->exp;
  params[3] = &info->mpls_label;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_LSR, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_UNICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_UNICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void
    *params[1];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->egress_nick;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_DIAG_TRILL_MULTICAST_print_with_offsets(
    SOC_SAND_IN SOC_PPC_DIAG_TRILL_MULTICAST *info,
    SOC_SAND_IN uint32 offset
  )
{
  CONST void*
    params[3];
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  params[0] = &info->esdai;
  params[1] = &info->fid_vsi;
  params[2] = &info->dist_tree_nick;
  ARAD_PP_DIAG_FORWARDING_TYPE_print_with_offset(ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, params, offset);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  arad_pp_diag_get_request(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  int                     core_id,
    SOC_SAND_IN  uint32                     nof_bytes,
    SOC_SAND_OUT uint32                     *buffer
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(buffer);

  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "KBP", "KBP_Key", buffer, 32);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_request()",0,0);
}

STATIC uint32
  arad_pp_diag_get_replies(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_IP_REPLY_RECORD         *ip_reply_record,
    SOC_SAND_OUT SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD  *second_ip_reply_record
  )
{
  uint32
    res;
  uint32
    buf[2],
    tmp_buf[ARAD_PP_DIAG_DBG_VAL_LEN];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(ip_reply_record);
  SOC_SAND_CHECK_NULL_INPUT(second_ip_reply_record);

  dpp_dsig_read(unit, core_id, "IRPP", "KBP", NULL, "KBP_Result_LSB", tmp_buf, 4);

  SHR_BITCOPY_RANGE(buf, 24, tmp_buf + 3, 0, 32);
  SHR_BITCOPY_RANGE(buf, 0, tmp_buf + 2, 8, 24);
      
  
  res = arad_pp_diag_reply_to_ip_reply_record(
          unit,
          buf,
          ip_reply_record
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  
  second_ip_reply_record->match_status = (SHR_BITGET(tmp_buf + 3, 30) != 0);
  if(second_ip_reply_record->match_status) {
    buf[0] = 0;
    SHR_BITCOPY_RANGE(buf, 8, tmp_buf + 2, 0, 8);
    SHR_BITCOPY_RANGE(buf, 0, tmp_buf + 1, 24, 8);
    second_ip_reply_record->dest_id = buf[0] & 0xffff;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_get_replies()",0,0);
}
#endif 

void
  ARAD_PP_DIAG_MTR_INFO_print(
    SOC_SAND_IN         int unit,
    SOC_SAND_IN         bcm_policer_t policer_id, 
    SOC_SAND_IN         bcm_policer_config_t *policer_cfg, 
    SOC_SAND_IN         int cbl, 
    SOC_SAND_IN         int ebl)
{
  int
      display_cbl, display_ebl, cbl_level, ebl_level, i, j, ast_count;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(policer_cfg);


  
  display_cbl = cbl * 8 / 1000;
  display_ebl = ebl * 8 / 1000;

  cbl_level = ebl_level = 0;
  if (display_cbl > 0 && policer_cfg->ckbits_burst != 0) {
      cbl_level = ARAD_PP_DIAG_PERCENT(display_cbl, policer_cfg->ckbits_burst);
  }
  if (display_ebl > 0 && policer_cfg->pkbits_burst != 0) {
      ebl_level = ARAD_PP_DIAG_PERCENT(display_ebl, policer_cfg->pkbits_burst);
  }
  

  LOG_CLI((BSL_META("Meter id: %d\n\r"), policer_id));
  LOG_CLI((BSL_META("Mode: %s\n\r"), SOC_PPC_MTR_MODE_to_string(policer_cfg->mode)));

  LOG_CLI((BSL_META("-------------------------------------------------------------------------\n\r")));
  LOG_CLI((BSL_META("Bucket Name  |Configured Rate(kbps)|Size(kb)          |Bucket Level      \n\r")));
  LOG_CLI((BSL_META("Committed    |")));
  LOG_CLI((BSL_META("%-21d|"), policer_cfg->ckbits_sec));
  LOG_CLI((BSL_META("%-18d|"), policer_cfg->ckbits_burst)); 
  LOG_CLI((BSL_META("%d\n\r"), display_cbl));
  if (policer_cfg->mode != bcmPolicerModeCommitted) {
      LOG_CLI((BSL_META("Excess       |")));
      LOG_CLI((BSL_META("%-21d|"), policer_cfg->pkbits_sec));
      LOG_CLI((BSL_META("%-18d|"), policer_cfg->pkbits_burst)); 
      LOG_CLI((BSL_META("%d\n\r"), display_ebl));
  }

  
  LOG_CLI((BSL_META("\r\n  CBL     ")));
  if (policer_cfg->mode != bcmPolicerModeCommitted) {
      LOG_CLI((BSL_META("   EBL     \r\n")));
  }

  else{
      LOG_CLI((BSL_META("\r\n")));
  }
  for (i = 80; i >= 0; i -= 20) {
      LOG_CLI((BSL_META("|")));


      if (cbl_level == 0 || i > cbl_level) {
          LOG_CLI((BSL_META("     ")));
      } else if (i < cbl_level - (cbl_level % 20)) {
          LOG_CLI((BSL_META("*****")));
      } else {
          ast_count = (cbl_level % 20) / 5;
          for (j = 0; j < ast_count; j++) {
               LOG_CLI((BSL_META("*")));
          }
          for (j = 0; j < 5 - ast_count; j++) {
               LOG_CLI((BSL_META(" ")));
          }
      }

      LOG_CLI((BSL_META("|")));
      if (policer_cfg->mode != bcmPolicerModeCommitted) {
          LOG_CLI((BSL_META("     |")));

          if (ebl_level == 0 || i > ebl_level) {
              LOG_CLI((BSL_META("     ")));
          } else if (i < ebl_level - (ebl_level % 20)) {
              LOG_CLI((BSL_META("*****")));
          } else {
              ast_count = (ebl_level % 20) / 5;
              for (j = 0; j < ast_count; j++) {
                 LOG_CLI((BSL_META("*")));
              }
              for (j = 0; j < 5 - ast_count; j++) {
                 LOG_CLI((BSL_META(" ")));
              }
          }

          LOG_CLI((BSL_META("|\r\n")));
      } else {
          LOG_CLI((BSL_META("\r\n")));
      }
  }

  LOG_CLI((BSL_META("-------    ")));
  if (policer_cfg->mode != bcmPolicerModeCommitted) {
      LOG_CLI((BSL_META(" -------\r\n")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_DIAG_ETH_POLICER_INFO_print(
	SOC_SAND_IN		int 						unit,
	SOC_SAND_IN		bcm_port_t 					port,
	SOC_SAND_IN		SOC_PPC_MTR_BW_PROFILE_INFO *policer_cfg, 
	SOC_SAND_IN		int 						*bucket_lvl,
    SOC_SAND_IN		uint32 						agg_policer_valid,
    SOC_SAND_IN		uint32 						agg_policer_id)
{
	int i, bucket;
	char *dataType_name[5] = {" UNK_UCAST ","   UCAST   "," UNK_MCAST ","   MCAST   ","   BCAST   "};
	char *disable_cir;
	char *packet_mode;
	char *color_blind;
	char *packet_adj;

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	SOC_SAND_CHECK_NULL_INPUT(policer_cfg);
	SOC_SAND_CHECK_NULL_INPUT(bucket_lvl);

	LOG_CLI((BSL_META("|--------------------------------------------PORT %3d-------------------------------------------------|\n\r"),port));
	if (agg_policer_valid == 1) {
        LOG_CLI((BSL_META("|                                aggregate policer index = %d                                         |\n\r"),agg_policer_id));
	}
	LOG_CLI((BSL_META("|-------------------------------------------------------------------------------------------------------------------|\n\r")));
	LOG_CLI((BSL_META("| Data Type |   Rate(kbps)  |  Burst(kbps)  |  Bucket(kbps) | Cir Disable | Color Blind | Packet Mode | Packet Adj  |\n\r")));
	LOG_CLI((BSL_META("|-------------------------------------------------------------------------------------------------------------------|\n\r")));
	
	for (i=0;i<SOC_PPC_NOF_MTR_ETH_TYPES;i++) {
		
		bucket = bucket_lvl[i] < 0x400000 ? bucket_lvl[i] : bucket_lvl[i] - 0x800000;
		bucket = bucket/125;
		disable_cir = policer_cfg[i].disable_cir    == 1 ? "TRUE " : "FALSE";
		packet_mode = policer_cfg[i].is_packet_mode == 1 ? "TRUE " : "FALSE";
		color_blind = policer_cfg[i].color_mode     == 0 ? "TRUE " : "FALSE";
		packet_adj = policer_cfg[i].is_pkt_truncate == 1 ? "TRUE " : "FALSE";
		LOG_CLI((BSL_META("|%s|  %11d  |    %7d    |    %7d    |    %s    |    %s    |    %s    |    %s    |\n\r"), 
				 dataType_name[i],policer_cfg[i].cir, policer_cfg[i].cbs, bucket,
				 disable_cir,color_blind,packet_mode,packet_adj));
		LOG_CLI((BSL_META("|-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - - - - - - - - |\n\r")));
	}
	LOG_CLI((BSL_META("|-------------------------------------------------------------------------------------------------------------------|\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_DIAG_AGGREGATE_ETH_POLICER_INFO_print(
	SOC_SAND_IN		int 						unit,
	SOC_SAND_IN		bcm_policer_t 	            policer_id,
	SOC_SAND_IN		int 	                    nom_of_policers,
    SOC_SAND_IN		SOC_PPC_MTR_BW_PROFILE_INFO *policer_cfg, 
	SOC_SAND_IN		int 						*bucket_lvl)
{
	int i, j, bucket;
	char *dataType_name[4][4] = {{" ALL TRAFFIC ",      ""       ,      ""       ,      ""       },
                                 {"  UNK_UC/MC  ","  MC/UC/BC   ",       ""      ,      ""       },
                                 {"  UC/UNK_UC  ","  MC/UNK_MC  ","    BCAST    ",      ""       },
                                 {"   UNK_UC    ","      UC     ","  UNK_UC/MC  ","    BCAST    "}
                                };
	char *disable_cir;
	char *packet_mode;
	char *color_blind;
	char* packet_adj;
	SOC_SAND_INIT_ERROR_DEFINITIONS(0);
	SOC_SAND_CHECK_NULL_INPUT(policer_cfg);
	SOC_SAND_CHECK_NULL_INPUT(bucket_lvl);

    
    i = nom_of_policers - 1;

	LOG_CLI((BSL_META("----------------------------------------------POLICER ID: 0x%x---------------------------------------------------\n\r"),policer_id));
	LOG_CLI((BSL_META("|---------------------------------------------------------------------------------------------------------------------|\n\r")));
	LOG_CLI((BSL_META("|  Data Type  |   Rate(kbps)  |  Burst(kbps)  |  Bucket(kbps) | Cir Disable | Color Blind | Packet Mode | Packet Adj  |\n\r")));
	LOG_CLI((BSL_META("|---------------------------------------------------------------------------------------------------------------------|\n\r")));
	
	for (j=0;j<nom_of_policers;j++) {
		
		bucket = bucket_lvl[j] < 0x400000 ? bucket_lvl[j] : bucket_lvl[j] - 0x800000;
		bucket = bucket/125;
		disable_cir = policer_cfg[j].cir            == 0 ? "TRUE " : "FALSE";
		packet_mode = policer_cfg[j].is_packet_mode == 0 ? "FALSE" : "TRUE ";
		color_blind = policer_cfg[j].color_mode     == 0 ? "FALSE" : "TRUE ";
		packet_adj  = policer_cfg[j].is_pkt_truncate== 0 ? "FALSE" : "TRUE ";
		LOG_CLI((BSL_META("|%s|  %11d  |    %7d    |    %7d    |    %s    |    %s    |    %s    |    %s    |\n\r"), 
				 dataType_name[i][j],policer_cfg[j].cir, policer_cfg[j].cbs, bucket,
				 disable_cir,color_blind,packet_mode,packet_adj));
		LOG_CLI((BSL_META("|-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - - - - - - - -  -  -  -|\n\r")));
	}
	LOG_CLI((BSL_META("|---------------------------------------------------------------------------------------------------------------------|\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32
  arad_pp_diag_frwrd_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_LKUP_INFO           *frwrd_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    dest=0,
    ofst_index=0,
    lbl_index=0,
    frwrd_code=0,
    vrf,
    mac_long[2],
    tmp=0,
    diag_flavor;
  SOC_PPC_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_KEY_ENCODED
    lem_key;
  ARAD_PP_LEM_ACCESS_KEY
    lem_access_key;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    lem_payload_str;
  uint32
    lem_payload_buf[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S] = {0};
  uint32 fid_nof_bits;
  uint32  valid;
  int use_tcam = 0;

#if defined(INCLUDE_KBP)
  uint32
    buf[32];
  uint32
    size_in_bytes;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(frwrd_info);

  *ret_val = SOC_PPC_DIAG_OK;

  res = arad_pp_diag_is_valid(unit, core_id, SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET, &valid);                  
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(frwrd_info);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);


  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Code", &frwrd_code, 1);
   
  
  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  diag_flavor = diag_mode.flavor;

  
  if ((diag_flavor & SOC_PPC_DIAG_FLAVOR_RAW) == 0)
  {
    frwrd_info->frwrd_type = arad_pp_diag_frwrd_type_to_lkup_type_map(frwrd_code);

  }

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Offset_Index", &ofst_index, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Offset_Extension", &lbl_index, 1);

  frwrd_info->frwrd_hdr_index = (uint8)(ofst_index + lbl_index);

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "VRF", &vrf, 1);

  if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC || frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC)
  {    
#if defined(INCLUDE_KBP)
  if(frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC && ARAD_KBP_ENABLE_IPV4_UC) {
    
    res = arad_pp_diag_get_frwrd_type_size(
            arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC),
            &size_in_bytes
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    res = arad_pp_diag_get_request(
            unit,
            core_id,
            size_in_bytes,
            buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    frwrd_info->lkup_key.kbp_ipv4_unicast_rpf = arad_pp_diag_request_to_ipv4_unicast_rpf(buf);

    
    res = arad_pp_diag_get_replies(
            unit,
            core_id,
            &frwrd_info->ip_reply_record,
            &frwrd_info->second_ip_reply_result
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    frwrd_info->is_kbp = TRUE;
  } else if(frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC && ARAD_KBP_ENABLE_IPV4_MC) {
    
    res = arad_pp_diag_get_frwrd_type_size(
            arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC),
            &size_in_bytes
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

    res = arad_pp_diag_get_request(
            unit,
            core_id,
            size_in_bytes,
            buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    
    frwrd_info->lkup_key.kbp_ipv4_multicast = arad_pp_diag_request_to_ipv4_multicast(buf);

    
    res = arad_pp_diag_get_replies(
            unit,
            core_id,
            &frwrd_info->ip_reply_record,
            &frwrd_info->second_ip_reply_result
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    frwrd_info->is_kbp = TRUE;
  }
#endif 
  }

  if (vrf != 0) {
      if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC) {
          frwrd_info->frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN;
      } else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC) {
          frwrd_info->frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN;
      }
  }
  if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC) {
      dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "TRILL_MC", &tmp, 1);
      if (tmp == 1) {
          frwrd_info->frwrd_type = SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC;
      }
#if defined(INCLUDE_KBP)
      if(frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC && ARAD_KBP_ENABLE_TRILL_UC) {
        
        res = arad_pp_diag_get_frwrd_type_size(
                arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC),
                &size_in_bytes
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

        res = arad_pp_diag_get_request(
                unit,
                core_id,
                size_in_bytes,
                buf
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        
        frwrd_info->lkup_key.kbp_trill_unicast = arad_pp_diag_request_to_trill_unicast(buf);

        
        res = arad_pp_diag_get_replies(
                unit,
                core_id,
                &frwrd_info->ip_reply_record,
                &frwrd_info->second_ip_reply_result
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
        frwrd_info->is_kbp = TRUE;
      } else if(frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC && ARAD_KBP_ENABLE_TRILL_MC) {
        
        res = arad_pp_diag_get_frwrd_type_size(
                arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC),
                &size_in_bytes
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

        res = arad_pp_diag_get_request(
                unit,
                core_id,
                size_in_bytes,
                buf
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        
        frwrd_info->lkup_key.kbp_trill_multicast = arad_pp_diag_request_to_trill_multicast(buf);

        
        res = arad_pp_diag_get_replies(
                unit,
                core_id,
                &frwrd_info->ip_reply_record,
                &frwrd_info->second_ip_reply_result
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
        frwrd_info->is_kbp = TRUE;
      } 
#endif 
  }

  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "LEM", "LEM_2nd_Key", regs_val, 3);

  if (diag_flavor & SOC_PPC_DIAG_FLAVOR_RAW)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,74,frwrd_info->lkup_key.raw);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT)
  {
    frwrd_info->lkup_key.mact.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    mac_long[0] = 0;
    mac_long[1] = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,0,48,mac_long);
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
    
       
    res = soc_sand_pp_mac_address_long_to_struct(
            mac_long,
            &(frwrd_info->lkup_key.mact.key_val.mac.mac)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    
    tmp = 0;
    fid_nof_bits = SOC_DPP_DEFS_GET(unit, fid_nof_bits);
    res = soc_sand_bitstream_get_any_field(regs_val,48,fid_nof_bits,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    frwrd_info->lkup_key.mact.key_val.mac.fid = tmp;
  }
  else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC || frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,32,&frwrd_info->lkup_key.ipv4_uc.key.subnet.ip_address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
    frwrd_info->lkup_key.ipv4_uc.key.subnet.prefix_len = 32;
    res = soc_sand_bitstream_get_any_field(regs_val,32,12,&frwrd_info->lkup_key.ipv4_uc.vrf);
    SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);
  }
  else if (
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC ||
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN ||
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC ||
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC  
          )
  {
    
    if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC ||
        frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC) {
        if(SOC_IS_ARADPLUS_AND_BELOW(unit) || (SOC_DPP_CONFIG(unit)->pp.l3_mc_use_tcam != ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE)){
            use_tcam = 1;
        }
    } else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN ||
              frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC) {
        if(SOC_IS_ARADPLUS_AND_BELOW(unit)){
            use_tcam = 1;
        }
    } 
    if (use_tcam) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            dpp_dsig_read(unit, core_id, "IRPP", "FLP", "TCAM", "Key", regs_val, 5);
        } else {
            dpp_dsig_read(unit, core_id, "IRPP", "FLP", "TCAM", "TCAM_1st_Key", regs_val, 5);
        }
    } else {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            dpp_dsig_read(unit, core_id, "IRPP", "FLP", "LPM", "Key", regs_val, 5);
        } else {
            dpp_dsig_read(unit, core_id, "IRPP", "FLP", "LPM", "LPM_2nd_A_Lookup_Key", regs_val, 5);
        }
    } 

    if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                        
            res = soc_sand_bitstream_get_any_field(regs_val,0,32,&frwrd_info->lkup_key.ipv4_mc.source.ip_address);
            SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
            frwrd_info->lkup_key.ipv4_mc.source.prefix_len = 32;
            
            res = soc_sand_bitstream_get_any_field(regs_val,32,32,&frwrd_info->lkup_key.ipv4_mc.group);
            SOC_SAND_CHECK_FUNC_RESULT(res, 175, exit);
            frwrd_info->lkup_key.ipv4_mc.group |= 0xe << 28;
            
            res = soc_sand_bitstream_get_any_field(regs_val,64,12,&frwrd_info->lkup_key.ipv4_mc.inrif);
            SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
            frwrd_info->lkup_key.ipv4_mc.inrif_valid = TRUE;
        } else {
            
            res = soc_sand_bitstream_get_any_field(regs_val,68,15,&frwrd_info->lkup_key.ipv4_mc.inrif);
            SOC_SAND_CHECK_FUNC_RESULT(res, 185, exit);
            frwrd_info->lkup_key.ipv4_mc.inrif_valid = TRUE;
            
            res = soc_sand_bitstream_get_any_field(regs_val,84,32,&frwrd_info->lkup_key.ipv4_mc.source.ip_address);
            SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
            frwrd_info->lkup_key.ipv4_mc.source.prefix_len = 32;
            
            res = soc_sand_bitstream_get_any_field(regs_val,116,28,&frwrd_info->lkup_key.ipv4_mc.group);
            SOC_SAND_CHECK_FUNC_RESULT(res, 195, exit);
            frwrd_info->lkup_key.ipv4_mc.group |= 0xe << 28;
            
            res = soc_sand_bitstream_get_any_field(regs_val,144,14,&frwrd_info->lkup_key.ipv4_mc.group);
            SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);              
        }
    } else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC || frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {            
            
            res = soc_sand_bitstream_get_any_field(regs_val,0,128,frwrd_info->lkup_key.ipv6_uc.key.subnet.ipv6_address.address);
            SOC_SAND_CHECK_FUNC_RESULT(res, 205, exit);
            frwrd_info->lkup_key.ipv6_uc.key.subnet.prefix_len = 132;
            
            res = soc_sand_bitstream_get_any_field(regs_val,128,12,&frwrd_info->lkup_key.ipv6_uc.vrf);
            SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
        } else {            
            
            res = soc_sand_bitstream_get_any_field(regs_val,16,128,frwrd_info->lkup_key.ipv6_uc.key.subnet.ipv6_address.address);
            SOC_SAND_CHECK_FUNC_RESULT(res, 215, exit);
            frwrd_info->lkup_key.ipv6_uc.key.subnet.prefix_len = 132;
            
            res = soc_sand_bitstream_get_any_field(regs_val,144,14,&frwrd_info->lkup_key.ipv6_uc.vrf);
            SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
        }
#if defined(INCLUDE_KBP)
    if(frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC && ARAD_KBP_ENABLE_IPV6_UC) {
      
      res = arad_pp_diag_get_frwrd_type_size(
              arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC),
              &size_in_bytes
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);

      res = arad_pp_diag_get_request(
              unit,
              core_id,
              size_in_bytes,
              buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

      
      frwrd_info->lkup_key.kbp_ipv6_unicast_rpf = arad_pp_diag_request_to_ipv6_unicast_rpf(buf);

      
      res = arad_pp_diag_get_replies(
              unit,
              core_id,
              &frwrd_info->ip_reply_record,
              &frwrd_info->second_ip_reply_result
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
      frwrd_info->is_kbp = TRUE;
    }
#endif 
   } else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            
            res = soc_sand_bitstream_get_any_field(regs_val,0,128,frwrd_info->lkup_key.ipv6_mc.group.ipv6_address.address);
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
            
            res = soc_sand_bitstream_get_any_field(regs_val,128,12,&frwrd_info->lkup_key.ipv6_mc.inrif.val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 255, exit);
        } else {
            
            res = soc_sand_bitstream_get_any_field(regs_val,1,15,&frwrd_info->lkup_key.ipv6_mc.inrif.val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
            
            res = soc_sand_bitstream_get_any_field(regs_val,16,128,frwrd_info->lkup_key.ipv6_mc.group.ipv6_address.address);
            SOC_SAND_CHECK_FUNC_RESULT(res, 265, exit);
            
            res = soc_sand_bitstream_get_any_field(regs_val,144,14,&frwrd_info->lkup_key.ipv6_mc.vrf_ndx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
        }
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_MC) {
      
      res = arad_pp_diag_get_frwrd_type_size(
              arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC),
              &size_in_bytes
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 275, exit);

      res = arad_pp_diag_get_request(
              unit,
              core_id,
              size_in_bytes,
              buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

      
      frwrd_info->lkup_key.kbp_ipv6_multicast = arad_pp_diag_request_to_ipv6_multicast(buf);

      
      res = arad_pp_diag_get_replies(
              unit,
              core_id,
              &frwrd_info->ip_reply_record,
              &frwrd_info->second_ip_reply_result
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
      frwrd_info->is_kbp = TRUE;
    }
#endif 
    }
  }
  else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM)
  {
    uint32 value;
    char *propval;

    res = soc_sand_bitstream_get_any_field(regs_val,0,ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.in_label);
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);
    tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
    res = soc_sand_bitstream_get_any_field(regs_val,ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM,ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM,&value);
    SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);
    tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;

    frwrd_info->lkup_key.ilm.mapped_exp =(uint8)value;

    propval = soc_property_get_str(unit, spn_MPLS_CONTEXT);
    if (propval && sal_strcmp(propval, "port") == 0) {
        
        if (SOC_IS_JERICHO(unit)) {
            
            uint32 port_and_core;

            res = soc_sand_bitstream_get_any_field(regs_val,tmp,ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM_JERICHO,&port_and_core);
            SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);

            frwrd_info->lkup_key.ilm.in_local_port = port_and_core & ((1 << ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM) -1);
            frwrd_info->lkup_key.ilm.in_core = (port_and_core >> ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM);

            tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM_JERICHO;
        }
        else {
            res = soc_sand_bitstream_get_any_field(regs_val,tmp,ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.in_local_port);
            SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);

            tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
        }
    } else if(propval && sal_strcmp(propval, "interface") == 0){
        
        res = soc_sand_bitstream_get_any_field(regs_val,tmp,ARAD_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM,&frwrd_info->lkup_key.ilm.inrif);
        SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
    } else if(propval && sal_strcmp(propval, "vrf") == 0){
        
        res = soc_sand_bitstream_get_any_field(regs_val,tmp,ARAD_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_VRF_IN_ILM,&value);
        SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
        frwrd_info->lkup_key.ilm.vrf = (uint16)value;
    }



    tmp += ARAD_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_MPLS) {
      
      res = arad_pp_diag_get_frwrd_type_size(
              arad_pp_diag_fwd_lkup_type_to_frwrd_ip_tbl_id(SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM),
              &size_in_bytes
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 335, exit);

      res = arad_pp_diag_get_request(
              unit,
              core_id,
              size_in_bytes,
              buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 340, exit);

      
      frwrd_info->lkup_key.kbp_mpls = arad_pp_diag_request_to_mpls(buf);

      
      res = arad_pp_diag_get_replies(
              unit,
              core_id,
              &frwrd_info->ip_reply_record,
              &frwrd_info->second_ip_reply_result
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);
      frwrd_info->is_kbp = TRUE;
    }
#endif 
  }
  else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,16,&frwrd_info->lkup_key.trill_uc);
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);

    dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "TRILL_MC", &tmp, 1);
  }
  else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC)
  {
    res = soc_sand_bitstream_get_any_field(regs_val,0,16,&frwrd_info->lkup_key.trill_mc.tree_nick);
    SOC_SAND_CHECK_FUNC_RESULT(res, 380, exit);

    res = soc_sand_bitstream_get_any_field(regs_val,17,15,&frwrd_info->lkup_key.trill_mc.fid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);

  }

  else if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF)
  {

    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&lem_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&lem_access_key);

    sal_memcpy(&lem_key.buffer,regs_val,sizeof(uint32)*SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit));

    res = arad_pp_lem_key_encoded_parse(unit,&lem_key,&lem_access_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);

    arad_pp_fcf_route_key_from_lem_key(&lem_access_key,&frwrd_info->lkup_key.fcf);
  }

  
  
  if (diag_flavor & SOC_PPC_DIAG_FLAVOR_RAW)
  {
      dpp_dsig_read(unit, core_id, "IRPP", "LEM", "LL", "LL_LEM_Result", frwrd_info->lkup_res.raw, 2);
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,7,0,46,0,5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,21,0,45,1,5);
      }
    res = soc_sand_bitstream_get_any_field(regs_val,0,SOC_IS_JERICHO(unit) ? 46 : 45,frwrd_info->lkup_res.raw);
    SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);
  }
  else if (
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC ||
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN ||
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC ||
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC  
          )
  {
      if (use_tcam) {
          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "FLP", "Action_Match", regs_val, 5);
          } else {
              dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "FLP", "TCAM_1st_Match", regs_val, 5);
          }
      } else {
          dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "LPM_2nd_Lookup_Found", regs_val, 5);
      }
      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 420, exit);
      if (tmp)
      {
        frwrd_info->key_found = TRUE;
      
        if (use_tcam) {
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "FLP", "Action", regs_val, 5);
            } else {
                dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "FLP", "TCAM_1st_Result", regs_val, 5);
            }
        } else {
            dpp_dsig_read(unit, core_id, "IRPP", "LPM", "FLP", "LPM_2nd_A_Lookup_Result", regs_val, 5);
        }

        res = soc_sand_bitstream_get_any_field(regs_val,0,19,&dest);
        SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);
        payload.asd = 0;
        res = arad_pp_diag_fwd_decision_in_buffer_parse(
                unit,                      
                ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                dest,
                payload.asd,
                &(frwrd_info->lkup_res.frwrd_decision)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 440, exit);      
      }
      else
      {
        frwrd_info->key_found = FALSE;
      }
  }
    
  else if (
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC ||
            frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN
          )
  {
    

    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,29,0,95,0,5);
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,95,0,5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,21,0,90,0,5);
    }
    
    tmp = 0;
    
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 47: 45, 1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);
    if (tmp)
    {
      
      frwrd_info->key_found = tmp;
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              0, 
              SOC_IS_JERICHO(unit)? 45: 43,
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 470, exit);

      res = arad_pp_lem_access_payload_parse(
              unit,              
              lem_payload_buf,
              ARAD_PP_LEM_ACCESS_NOF_KEY_TYPES,
              &lem_payload_str
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 480, exit);

      res = arad_pp_frwrd_em_dest_to_fec(
              unit,
              &lem_payload_str,
              &(frwrd_info->lkup_res.host_info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);

    }
    else
    {

      if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,33,0,89,70,5);
        
        if (regs_val[0] == 0) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,33,0,29,10,5);
        }
      } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,31,0,89,70,5);
        
        if (regs_val[0] == 0) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,31,0,29,10,5);
        }
      } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,14,0,15,1,5);
      }
      
      frwrd_info->key_found = TRUE;
      frwrd_info->lkup_res.frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
      frwrd_info->lkup_res.frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;
      if (SOC_IS_JERICHO(unit) || SOC_IS_QAX(unit)) {
        frwrd_info->lkup_res.frwrd_decision.dest_id = JER_PP_KAPS_DEFAULT_DECODE(JER_PP_KAPS_FEC_DECODE(regs_val[0]));
      } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        frwrd_info->lkup_res.frwrd_decision.dest_id = regs_val[0];
      }
    }
  }
  
  else
  {
    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,29,0,47,0, 5);
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,47,0, 5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,21,0,45,0,5);
    }
    
    tmp = 0;

    
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 47: 45,1,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);
    if (tmp)
    {
      frwrd_info->key_found = TRUE;

      
      if (SOC_IS_JERICHO(unit)) {
          res = soc_sand_bitstream_get_any_field(regs_val, 0, 45,lem_payload_buf);
      }
      else {
          res = soc_sand_bitstream_get_any_field(regs_val, 0, 43,lem_payload_buf);
      }
      SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);

      res = arad_pp_lem_access_payload_parse(
              unit,               
              lem_payload_buf, 
              ARAD_PP_LEM_ACCESS_NOF_KEY_TYPES,
              &payload
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 505, exit);

      if (frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT || frwrd_info->frwrd_type == SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT)
      {
        res = arad_pp_fwd_decision_in_buffer_parse(
                unit,                
                payload.dest,
                payload.asd,
                payload.flags,
                &(frwrd_info->lkup_res.mact.frwrd_info.forward_decision)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 530, exit);
      }
      else
      {
        res = arad_pp_diag_fwd_decision_in_buffer_parse(
                unit,                
                ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_17,
                payload.dest,
                payload.asd,
                &(frwrd_info->lkup_res.frwrd_decision)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 540, exit);
      }
    }
    else
    {
      frwrd_info->key_found = FALSE;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lkup_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_frwrd_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_FRWRD_LKUP_INFO_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_lkup_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_traps_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_TRAPS_INFO                *traps_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  )
{
  uint32
    regs_val[3],
    tmp_regs_val[3],
    reg_val,
    is_trapped,
    tmp;
  SOC_PPC_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;
  uint64
    reg64_val;
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_TRAPS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  *ret_val = SOC_PPC_DIAG_OK; 
 
  res = arad_pp_diag_is_valid(unit, core_id, SOC_PPC_DIAG_TRAPS_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                            
      SOC_SAND_EXIT_NO_ERROR;
  }   

  SOC_PPC_DIAG_TRAPS_INFO_clear(traps_info);
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_LLR_TRAP_0r(unit, core_id, &regs_val[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_LLR_TRAP_1r(unit, core_id, &regs_val[1]));

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          0,
          42,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_VTT_TRAPS_ENCOUNTEREDr(unit, core_id, &reg64_val));
  regs_val[0] = COMPILER_64_LO(reg64_val);
  regs_val[1] = COMPILER_64_HI(reg64_val);

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          1,
          55,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_FLP_TRAP_0r(unit, core_id, &tmp_regs_val[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_DBG_FLP_TRAP_1r(unit, core_id, &tmp_regs_val[1]));

  regs_val[0] = tmp_regs_val[0] | ((tmp_regs_val[1] & 0x3FFF) << 18);
  regs_val[1] = (tmp_regs_val[1] & 0xFFFFC000) >> 14;

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          2,
          44,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_DBG_FER_TRAPr(unit, core_id, &regs_val[0]));

  res = arad_pp_diag_traps_range_info_get_unsafe(
          unit,
          regs_val,
          3,
          8,
          traps_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_DBG_LAST_RESOLVED_TRAPr(unit, core_id, &reg_val));
  
  
  tmp = 0;
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_VALIDf);
  traps_info->committed_trap.is_pkt_trapped = SOC_SAND_NUM2BOOL(tmp);

  
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODEf);
  is_trapped = arad_pp_diag_is_trap_code(tmp);

  traps_info->committed_trap.is_pkt_trapped = traps_info->committed_trap.is_pkt_trapped && is_trapped;

  if(traps_info->committed_trap.is_pkt_trapped) {

      arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, tmp, &traps_info->committed_trap.code);
      res = arad_pp_trap_frwrd_profile_info_get_unsafe(
              unit,
              traps_info->committed_trap.code,
              &(traps_info->committed_trap.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }

  
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODE_STRENGTHf);
  traps_info->committed_trap.info.strength = tmp;

  
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_SNOOP_CODEf);
  arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, tmp, &traps_info->committed_snoop.code);
  res = arad_pp_trap_snoop_profile_info_get_unsafe(
          unit,
          traps_info->committed_snoop.code,
          &(traps_info->committed_snoop.info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_SNOOP_CODE_STRENGTHf);
  traps_info->committed_snoop.info.strength = tmp;

 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr(unit, core_id, &reg_val));
  
  
  tmp = 0;
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr, reg_val, DBG_LAST_TRAP_CHANGE_DESTINATION_VALIDf);
  traps_info->trap_updated_dest.is_pkt_trapped = SOC_SAND_NUM2BOOL(tmp);

  
  tmp = soc_reg_field_get(unit, IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr, reg_val, DBG_LAST_TRAP_CHANGE_DESTINATIONf);
  is_trapped = arad_pp_diag_is_trap_code(tmp);
  traps_info->trap_updated_dest.is_pkt_trapped = traps_info->trap_updated_dest.is_pkt_trapped && is_trapped;

  if(traps_info->trap_updated_dest.is_pkt_trapped) {
      arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, tmp, &traps_info->trap_updated_dest.code);
      res = arad_pp_trap_frwrd_profile_info_get_unsafe(
              unit,
              traps_info->trap_updated_dest.code,
              &(traps_info->trap_updated_dest.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
  }

  
  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  if (diag_mode.flavor & SOC_PPC_DIAG_FLAVOR_CLEAR_ON_GET)
  {
    res = arad_pp_diag_pkt_trace_clear_unsafe(
            unit,
            core_id,
            SOC_PPC_DIAG_PKT_TRACE_TRAP
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_traps_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_traps_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_TRAPS_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_traps_info_get_verify()", 0, 0);
}


STATIC void 
  arad_pp_diag_frwrd_action_trap_code_update(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                              internal_trap_code,
    SOC_SAND_IN  SOC_PPC_DIAG_FRWRD_DECISION_PHASE   frwrd_decision_phase,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info
    )
{
  SOC_PPC_TRAP_CODE trap_code;
    
  if(arad_pp_diag_is_trap_code(internal_trap_code)){
      arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit,internal_trap_code,&trap_code);
      frwrd_trace_info->trap[frwrd_decision_phase].action_profile.trap_code = trap_code;
  }
  else{
    if(frwrd_trace_info->trap[frwrd_decision_phase].action_profile.frwrd_action_strength > 0)
    {
      frwrd_trace_info->trap[frwrd_decision_phase].action_profile.trap_code = SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT;
    }
  }
}



uint32
  arad_pp_diag_frwrd_decision_trace_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                         *ret_val
  )
{
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    dest,
    trap,
    fld_val,
    reg_val,
    eei=0;
  SOC_PPC_TRAP_CODE
    trap_code;
  uint32
    res = SOC_SAND_OK;
  uint32 valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(frwrd_trace_info);

  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(frwrd_trace_info);
  
  *ret_val = SOC_PPC_DIAG_OK;

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET, &valid);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                          
  if (!valid) {        
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                       
      SOC_SAND_EXIT_NO_ERROR;
  }   

  

  dpp_dsig_read(unit, core_id, "IRPP", "LL", "VT", "Snoop_Strength", regs_val, 1);
  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.snoop_action_strength = regs_val[0];

  dpp_dsig_read(unit, core_id, "IRPP", "LL", "VT", "Fwd_Action_Strength", regs_val, 1);
  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.frwrd_action_strength = regs_val[0];

  dpp_dsig_read(unit, core_id, "IRPP", "LL", "VT", "Fwd_Action_CPU_Trap_Code", regs_val, 1);
  arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit,regs_val[0],&trap_code);
  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT].action_profile.trap_code = trap_code;

  dpp_dsig_read(unit, core_id, "IRPP", "LL", "VT", "Fwd_Action_CPU_Trap_Qualifier", regs_val, 1);
  frwrd_trace_info->trap_qual[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT] = regs_val[0];

  
  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Snoop_Strength", regs_val, 1);
  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.snoop_action_strength = regs_val[0];
  
  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_Strength", regs_val, 1);
  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].action_profile.frwrd_action_strength = regs_val[0];

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_CPU_Trap_Code", regs_val, 1);
  arad_pp_diag_frwrd_action_trap_code_update(unit, regs_val[0], SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF, frwrd_trace_info);

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_CPU_Trap_Qualifier", regs_val, 1);
  frwrd_trace_info->trap_qual[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF] = regs_val[0];

  
  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_Dst", &dest, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "EEI", &eei, 1);

   res = arad_pp_diag_fwd_decision_in_buffer_parse(
          unit,          
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
          dest,
          eei,
          &(frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].frwrd_decision)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Out_LIF", regs_val, 1);
  frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF].outlif = regs_val[0];

  
  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Snoop_Strength", regs_val, 1);
  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.snoop_action_strength = regs_val[0];

  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Fwd_Action_Strength", regs_val, 1);
  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].action_profile.frwrd_action_strength = regs_val[0];

  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Fwd_Action_CPU_Trap_Code", regs_val, 1);
  arad_pp_diag_frwrd_action_trap_code_update(unit, regs_val[0], SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND, frwrd_trace_info);

  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Fwd_Action_CPU_Trap_Qualifier", regs_val, 1);
  frwrd_trace_info->trap_qual[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND] = regs_val[0];

 
  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Fwd_Action_Dst", &dest, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "EEI", &eei, 1);

  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,        
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Out_LIF", regs_val, 1);
  frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND].outlif = regs_val[0];

  
  dpp_dsig_read(unit, core_id, "IRPP", "PMF", "FER", "Dst", &dest, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "PMF", "FER", "EEI", &eei, 1);

  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,        
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  dpp_dsig_read(unit, core_id, "IRPP", "PMF", "FER", "Out_LIF", regs_val, 1);
  frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF].outlif = regs_val[0];

  
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Fwd_Action.Dst", &dest, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "EEI", &eei, 1);

  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,        
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Out_LIF", regs_val, 1);
  frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP].outlif = regs_val[0];

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_IHB_DBG_LAST_RESOLVED_TRAPr(unit, core_id, &reg_val));

  trap = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODEf);

  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.frwrd_action_strength = 
      soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_CPU_TRAP_CODE_STRENGTHf);

  arad_pp_diag_frwrd_action_trap_code_update(unit, trap, SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC, frwrd_trace_info);

  frwrd_trace_info->trap[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC].action_profile.snoop_action_strength = 
      soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_SNOOP_CODE_STRENGTHf);

  fld_val = 
      soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, reg_val, DBG_LAST_RESOLVED_VALIDf);

  frwrd_trace_info->trap_qual[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC] = fld_val;

  
  dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "Dst", &dest, 1);
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "EEI", &eei, 1);
  res = arad_pp_diag_fwd_decision_in_buffer_parse(
        unit,
        ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
        dest,
        eei,
        &(frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].frwrd_decision)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Out_LIF", regs_val, 1);
  frwrd_trace_info->frwrd[SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED].outlif = regs_val[0];;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_decision_trace_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_frwrd_decision_trace_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_FRWRD_DECISION_TRACE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_frwrd_decision_trace_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_learning_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_LEARN_INFO              *learn_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                  *ret_val

  )
{
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    lem_payload;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    lem_key;
  ARAD_PP_LEM_ACCESS_KEY
    lem_access_key;
  uint32
    res = SOC_SAND_OK;
  uint32  valid;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_LEARNING_INFO_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(learn_info);

  *ret_val = SOC_PPC_DIAG_OK;   

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_LEARNING_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                          
      SOC_SAND_EXIT_NO_ERROR;
  }   

  SOC_PPC_DIAG_LEARN_INFO_clear(learn_info);

  
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Ingress_Learn_Enable", &tmp, 1);

  if(tmp)
      learn_info->ingress = TRUE;

  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Egress_Learn_Enable", &tmp, 1);

  
  if (tmp || learn_info->ingress) {
      learn_info->valid = TRUE;
  } else {
      goto exit;
  }

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload);
  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&lem_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&lem_access_key);

  
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Learn_or_Transplant", regs_val, 1);
  learn_info->is_new_key = !regs_val[0];

  
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Learn_Data", regs_val, 2);

  
  res = arad_pp_lem_access_payload_parse(unit,regs_val,ARAD_PP_LEM_ACCESS_NOF_KEY_TYPES,&lem_payload);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_frwrd_mact_payload_convert(unit,&lem_payload,&learn_info->value);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Learn_Key", lem_key.buffer, SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S);

  res = arad_pp_lem_key_encoded_parse(unit,&lem_key,&lem_access_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  
  res = arad_pp_frwrd_mact_key_parse(unit,&lem_access_key,&learn_info->key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_learning_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_learning_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_LEARNING_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_learning_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_ing_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_VLAN_EDIT_RES           *vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                  *ret_val

  )
{
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  uint32
    res = SOC_SAND_OK;
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vec_res);

  *ret_val = SOC_PPC_DIAG_OK;

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET, &valid);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;
      SOC_SAND_EXIT_NO_ERROR;
  }

  SOC_PPC_DIAG_VLAN_EDIT_RES_clear(vec_res);

  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "VLAN_Edit_Cmd", regs_val, 2);

  vec_res->adv_mode = SOC_DPP_CONFIG(unit)->pp.vlan_translate_mode;

  
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,0,12,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  vec_res->ac_tag.vid = (SOC_SAND_PP_VLAN_ID)tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,24,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  vec_res->ac_tag.dei = (SOC_SAND_PP_DEI_CFI)tmp;
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,25,3,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  vec_res->ac_tag.pcp = (SOC_SAND_PP_PCP_UP)tmp;

  
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,12,12,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  vec_res->ac_tag2.vid = (SOC_SAND_PP_VLAN_ID)tmp;

  
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,28,6,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  vec_res->cmd_id = tmp;
  
  res = arad_pp_lif_ing_vlan_edit_command_info_get_unsafe(
          unit,
          vec_res->cmd_id,
          &vec_res->cmd_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_ing_vlan_edit_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_ing_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_ING_VLAN_EDIT_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_ing_vlan_edit_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_pkt_associated_tm_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_PKT_TM_INFO             *pkt_tm_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                  *ret_val

  )
{
  uint32
    regs_val[DSIG_MAX_SIZE_UINT32],
    dest,
    type=0,
    dest_id=0,
    sys_logic_port_id,
    lag_id,
    lag_member_id,
    sys_phys_port_id,
    tmp=0;
  uint32
    res = SOC_SAND_OK;
  uint8 is_lag_not_phys;
  uint32  valid;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pkt_tm_info);

  *ret_val = SOC_PPC_DIAG_OK;                                                           

 
  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                           
      SOC_SAND_EXIT_NO_ERROR;
  }   

  SOC_PPC_DIAG_PKT_TM_INFO_clear(pkt_tm_info);

  dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd", regs_val, 12);

  
  
  tmp = 0;
  
  res = soc_sand_bitstream_get_any_field( regs_val, SOC_IS_QAX(unit) ? 0 : (SOC_IS_JERICHO(unit) ? 118: 116),1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (tmp)
  {
      pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_DEST;
      
      
      dest = 0;
      
      
      res = soc_sand_bitstream_get_any_field( regs_val, SOC_IS_QAX(unit) ? 1 : (SOC_IS_JERICHO(unit)? 132: 130), SOC_IS_JERICHO(unit)? 19: 18, &dest);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (dest==ARAD_PP_DIAG_FRWRD_DECISION_DEST_DROP_CODE) {
          LOG_INFO(BSL_LS_SOC_DIAG,
                   (BSL_META_U(unit,
                               "0")));
          pkt_tm_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
      } else {
          type = 0;
          res = soc_sand_bitstream_get_any_field(&dest, 17, SOC_IS_JERICHO(unit)? 2: 1, &type);
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          if (type == ARAD_PP_DIAG_FRWRD_DECISION_TYPE_UC_FLOW){
              pkt_tm_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW;
              res = soc_sand_bitstream_get_any_field(&dest, 0, 17, &dest_id);
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
          } else {;
              type=0;
              res = soc_sand_bitstream_get_any_field(&dest, SOC_IS_JERICHO(unit)? 17: 16, 2, &type);
              SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
              if (type == ARAD_PP_DIAG_FRWRD_DECISION_TYPE_MC) {
                  pkt_tm_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
                  res = soc_sand_bitstream_get_any_field(&dest,0, SOC_IS_JERICHO(unit)? 17: 16, &dest_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
              } else if (SOC_IS_JERICHO(unit) && (type == ARAD_PP_DIAG_FRWRD_DECISION_TYPE_EGRESS_MC)) {
                  pkt_tm_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
                  res = soc_sand_bitstream_get_any_field(&dest,0, 17, &dest_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
              } else {
                  
                  
                  sys_logic_port_id=0;
                  res = soc_sand_bitstream_get_any_field(&dest, 0, 16, &sys_logic_port_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
                  sys_phys_port_id=0;
                  res = arad_ports_logical_sys_id_parse(unit, sys_logic_port_id, &is_lag_not_phys, &lag_id, &lag_member_id, &sys_phys_port_id);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
                  if (is_lag_not_phys){
                      pkt_tm_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG;
                      dest_id = lag_id;
                  } else {
                      pkt_tm_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
                      dest_id = sys_phys_port_id;
                  }
              }
          }
      }
      pkt_tm_info->frwrd_decision.dest_id = dest_id;
  }

  
  
  tmp = 1;
  
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_TC;
    
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_QAX(unit) ? 46 : (SOC_IS_JERICHO(unit)? 129: 127), 3,&tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    pkt_tm_info->tc = (SOC_SAND_PP_TC)tmp;
  }

  
   
  tmp = 1;
  
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_DP;
    
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val,  SOC_IS_QAX(unit) ? 44: (SOC_IS_JERICHO(unit)? 127: 125), 2, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    pkt_tm_info->dp = (SOC_SAND_PP_DP)tmp;
  }

  
  
  
  
  tmp = 0;
  
  res = soc_sand_bitstream_get_any_field(regs_val,SOC_IS_QAX(unit) ? 96 : (SOC_IS_JERICHO(unit) ? 204 : 196), SOC_IS_JERICHO(unit) ? 1 : SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_METER1;
    
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_QAX(unit) ? 80 : (SOC_IS_JERICHO(unit)? 187: 196), SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &pkt_tm_info->meter1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  
  tmp = 0;
  
  res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_QAX(unit) ? 113 : (SOC_IS_JERICHO(unit)? 222: 211), SOC_IS_JERICHO(unit) ? 1 : SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_METER2;
    
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_QAX(unit) ? 97 : (SOC_IS_JERICHO(unit)? 205: 211), SOC_DPP_DEFS_GET(unit, meter_pointer_nof_bits), &pkt_tm_info->meter2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }
  
  tmp = 1;
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD;
    dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd.DP_Meter_Cmd", &pkt_tm_info->dp_meter_cmd, 1);
  }

  
  dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd.Counter_A_Update", &tmp, 1);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1;
    dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd.Counter_A_Ptr", &pkt_tm_info->counter1, 1);
  }
  
  dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd.Counter_B_Update", &tmp, 1);
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2;
    dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd.Counter_B_Ptr", &pkt_tm_info->counter2, 1);
  }

  
  tmp = 1;  
  if (tmp)
  {
      pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR;
      
      res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_QAX(unit) ? 69 : 0, 11,&pkt_tm_info->eth_meter_ptr);
      SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  }

  if(SOC_IS_QAX(unit))
  {
      pkt_tm_info->ingress_shaping_da = -1;
      pkt_tm_info->ecn_capable = -1;
      pkt_tm_info->cni = -1;

  } else {
      
      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 151: 148, 1, &tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
      if (tmp)
      {
          pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA;
          res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 152: 149, 17, &pkt_tm_info->ingress_shaping_da);
          SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
      }

      
      tmp = 1;  
      if (tmp)
      {
          pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE;
          res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 169: 166, 1, &pkt_tm_info->ecn_capable);
          SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
      }
      
      tmp = 1;  
      if (tmp)
      {
          pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_CNI;
          res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 170: 167, 1, &pkt_tm_info->cni);
          SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
      }
  }

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      res = soc_sand_bitstream_get_any_field(regs_val,168,2,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  } else {
      pkt_tm_info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_INVALID;
  }
  if (SOC_IS_ARADPLUS_AND_BELOW(unit) && tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE;
    switch (tmp) {
    case 1:
      pkt_tm_info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
      break;
    case 2:
      pkt_tm_info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC;
      break;
    case 3:
      pkt_tm_info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 235, exit);
      break;
    }
  }
  
  tmp = 1;  
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_QAX(unit) ? 132 : (SOC_IS_JERICHO(unit)? 179: 188), 8, &pkt_tm_info->st_vsq_ptr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  }
  
  tmp = 1;  
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY;
    dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd.LAG_LB_Key", &pkt_tm_info->lag_lb_key, 1);
  }
  
  if(SOC_IS_JERICHO_PLUS(unit))
  {
      tmp = 0;
      pkt_tm_info->ignore_cp = -1;
  }
  else
      tmp = 1;  
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP;
    dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "TM_Cmd.Ignore_CP", &pkt_tm_info->ignore_cp, 1);
  }

  
  tmp = 1;
  if (tmp)
  {
    pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID;
    
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_QAX(unit) ? 116 : (SOC_IS_JERICHO(unit)? 119: 117), 4, &pkt_tm_info->snoop_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
  }

  
  
  dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "Out_LIF", &pkt_tm_info->cud, 1);
  pkt_tm_info->valid_fields |= SOC_PPC_DIAG_PKT_TM_FIELD_CUD;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_associated_tm_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_pkt_associated_tm_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_PKT_ASSOCIATED_TM_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_pkt_associated_tm_info_get_verify()", 0, 0);
}




STATIC uint32 
arad_pp_diag_encap_info_decode_ees_action(SOC_SAND_IN int unit, 
                                          SOC_SAND_OUT SOC_PPC_DIAG_ENCAP_INFO *encap_info, 
                                          uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN])
{
    uint32
        tmp2=0,
        curr_eep_index = encap_info->nof_eeps;
    uint32
        res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    tmp2 = 0;
    
    res = soc_sand_bitstream_get_any_field(regs_val, 4, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits), &tmp2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
        if (curr_eep_index != 0 && encap_info->eep[curr_eep_index] != SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID) {
            SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(tmp2, encap_info->eep[curr_eep_index], SOC_E_INTERNAL, 36, exit);
        }
    }
    
    encap_info->eep[curr_eep_index] = tmp2;

    res = arad_pp_eg_encap_entry_get_unsafe(
       unit,
       SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
       encap_info->eep[curr_eep_index],
       1,
       &encap_info->encap_info[curr_eep_index],
       &(encap_info->eep[(curr_eep_index + 1)]),
       &tmp2
       );
    curr_eep_index++;
    encap_info->nof_eeps = curr_eep_index; 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_encap_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_diag_encap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_ENCAP_INFO             *encap_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                 *ret_val
  )
{
  ARAD_PP_DIAG_REG_FIELD
      fld;
  uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
      tmp=0,
      otm_port,
      tmp2=0,
      valid=0;
  uint32
      res = SOC_SAND_OK;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
      cur_eep_type = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
  uint32
      base_q_pair,
      nof_priorities,
      found,
      flags;
  soc_pbmp_t 
      pbmp;
  soc_port_t
      port_i;
  int
      core;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_ENCAP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(encap_info);
  *ret_val = SOC_PPC_DIAG_OK;                                                           


  SOC_PPC_DIAG_ENCAP_INFO_clear(encap_info);

  

  
  dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_0_Valid", &valid, 1);

  if (valid)
  {
    

    
    dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_0.EEI", regs_val, 1);
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(regs_val, 20, 4, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (tmp <= 7) {
       
       encap_info->mpls_cmd.command = SOC_PPC_MPLS_COMMAND_TYPE_PUSH;
       encap_info->mpls_cmd.push_profile = tmp;
       
       soc_sand_bitstream_get_any_field(regs_val, 0, 20, &encap_info->mpls_cmd.label); 
    } else if (tmp == 10) {
        uint32 is_multicast = 0;
       
        encap_info->eei.type = SOC_PPC_EEI_TYPE_TRILL;

        soc_sand_bitstream_get_any_field(regs_val, 17, 1, &(is_multicast));
        encap_info->eei.val.trill_dest.is_multicast = (uint8)is_multicast;
        soc_sand_bitstream_get_any_field(regs_val, 1,
            ARAD_PP_ASD_NOF_BITS_TRILL_NICK,
            &(encap_info->eei.val.trill_dest.dest_nick));
    }
  }

  
  tmp = 0;
  dpp_dsig_read(unit, core_id, "ETPP", "LL", NULL, "FHEI_Code", &tmp, 1);

  tmp2 = 0;
  dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "FHEI_Code", &tmp2, 1);

  if (tmp == 1 && tmp2 == 5) {
      
      encap_info->mpls_cmd.command = SOC_PPC_MPLS_COMMAND_TYPE_POP;

      dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "FHEI", regs_val, 1);

      encap_info->mpls_cmd.has_cw = tmp;

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,15,1,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 665, exit);

      encap_info->mpls_cmd.model = (tmp == 1) ? SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE:SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM;

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,16,4,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 670, exit);

      encap_info->mpls_cmd.pop_next_header = tmp;

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(regs_val,20,2,&tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 675, exit);

      encap_info->mpls_cmd.tpid_profile = tmp;
            
  } else if (tmp == 2 && tmp2 == 5) {
      
      encap_info->mpls_cmd.command = SOC_PPC_MPLS_COMMAND_TYPE_SWAP;

      dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "FHEI", regs_val, 2);

      encap_info->mpls_cmd.label = tmp;
  }

  dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_1_Valid", &valid, 1);
  if (valid) {
      dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_1", regs_val, 5);

      res = arad_pp_diag_encap_info_decode_ees_action(unit, encap_info, regs_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);
  }

  dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_2_Valid", &valid, 1);
  if (valid) {
      dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_2", regs_val, 5);

      res = arad_pp_diag_encap_info_decode_ees_action(unit, encap_info, regs_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);
  }

  dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_3_Valid", &valid, 1);
  if (valid) {
      dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_3", regs_val, 5);

      res = arad_pp_diag_encap_info_decode_ees_action(unit, encap_info, regs_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);
  }

  if (SOC_IS_JERICHO_PLUS(unit)) {

      dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_4_Valid", &valid, 1);
      if (valid) {
          dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_4", regs_val, 5);

          res = arad_pp_diag_encap_info_decode_ees_action(unit, encap_info, regs_val);
          SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);
      }

      dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_5_Valid", &valid, 1);
      if (valid) {
          dpp_dsig_read(unit, core_id, "ETPP", "Prp", NULL, "EES_Action_5", regs_val, 5);

          res = arad_pp_diag_encap_info_decode_ees_action(unit, encap_info, regs_val);
          SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);
      }
  }
  

  dpp_dsig_read(unit, core_id, "ETPP", "Encap", "LL", "Out_LIF", &tmp, 1);

  res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, tmp, &cur_eep_type);
  if (cur_eep_type != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {
    
    
      if (SOC_IS_QAX_A0(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,129,102,5);
      } else if (SOC_IS_JERICHO_PLUS(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,209,182,5);
      } else if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,189,174,5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,128,113,5);
      }
    
    res = soc_sand_bitstream_get_any_field(regs_val,0,16,&encap_info->ll_vsi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);

            
    tmp = 0;
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        if (core_id == 0) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 104, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_0_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_0_LAST_LOOKUP_RESULT_FOUNDf, &tmp));
        }
        else {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 104, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_1_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_1_LAST_LOOKUP_RESULT_FOUNDf, &tmp));
        }
    }
    else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 104, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_LAST_LOOKUP_RESULT_FOUNDf, &tmp));
    }

    if (tmp) {
      
          tmp = 0;
          if (SOC_IS_JERICHO_PLUS_A0(unit)) {
              if (core_id == 0) {
                  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 104, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_0_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_0_LAST_LOOKUP_RESULTf, &tmp));
              }
              else {
                  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 104, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_1_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_1_LAST_LOOKUP_RESULTf, &tmp));
              }
          }
          else {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 104, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EDB_ESEM_LAST_LOOKUPr, REG_PORT_ANY, 0, ESEM_LAST_LOOKUP_RESULTf, &tmp));
          }
          
          encap_info->out_ac = tmp;
    } else {
      
        
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,4,0,89+22,89+6,5);
        } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,4,0,63+21,63+6,5);
        }
      
      res = soc_sand_bitstream_get_any_field(regs_val, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits), &encap_info->out_ac);
      SOC_SAND_CHECK_FUNC_RESULT(res, 335, exit);
    }
  } else {
    
    
    if (SOC_IS_JERICHO_PLUS(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,8,0,17,2,5);
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,8,0,25,10,5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,8,0,15,0,5);
    }
    
    res = soc_sand_bitstream_get_any_field(regs_val, 0, 16, &encap_info->ll_vsi);
    SOC_SAND_CHECK_FUNC_RESULT(res, 337, exit);

    
    encap_info->out_ac = tmp;

      if (encap_info->nof_eeps && (encap_info->ll_vsi == 0xffff))
      {
          res = arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, encap_info->eep[0], &cur_eep_type);
          SOC_SAND_CHECK_FUNC_RESULT(res, 338, exit);
          
          if (cur_eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER)
          {
              if (SOC_IS_QAX_A0(unit)) {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,145,130,5);
              }
              else if (SOC_IS_QUX(unit) || SOC_IS_QAX_B0(unit) || SOC_IS_JERICHO_PLUS(unit)) {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,225,210,5);
              }
              else if (SOC_IS_JERICHO(unit)) {
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,217,202,5);
              } else {
                  
                  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,153,138,5);
              }
              
              res = soc_sand_bitstream_get_any_field(regs_val, 0, 16, &encap_info->ll_vsi);
              SOC_SAND_CHECK_FUNC_RESULT(res, 339, exit);
          }
      }
  }


  
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,7,0,42,35,5);
  
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(regs_val,0,8,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);

  
  res = soc_port_sw_db_valid_ports_get(unit, 0, &pbmp);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

  found = 0;
  SOC_PBMP_ITER(pbmp, port_i) {

      res = soc_port_sw_db_flags_get(unit, port_i, &flags);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 155, exit);

      if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags)) {
          continue;
      }

      res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, &base_q_pair);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);

      res = soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &nof_priorities);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 170, exit);

      if (tmp >= base_q_pair && tmp < (base_q_pair + nof_priorities)) {
          res = soc_port_sw_db_local_to_tm_port_get(unit, port_i, &otm_port, &core);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 180, exit);
          encap_info->tm_port = otm_port;
          found = 1;
          break;
      }
  }  

  if (found == 0) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_DIAG_INVALID_TM_PORT_ERR, 4001, exit);
  }

  
  ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,7,0,50,43,5);
  
  res = soc_sand_bitstream_get_any_field(regs_val,0,8,&encap_info->pp_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_encap_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_encap_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_DIAG_ENCAP_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_encap_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_out_rif_get_unsafe(
    SOC_SAND_IN     int     unit,
    SOC_SAND_IN     int     core_id,
    SOC_SAND_OUT    uint8   *rif_is_valid,
    SOC_SAND_OUT    uint32  *out_rif
  )
{
  ARAD_PP_DIAG_REG_FIELD
      fld;
  uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
  uint32
      tmp,
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(rif_is_valid);
  SOC_SAND_CHECK_NULL_INPUT(out_rif);

  *rif_is_valid = 0;
  *out_rif = 0;

  
  
  if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 2, 0, 244, 244, 5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID, 0, 1, 154, 154, 5);
  }

  res = soc_sand_bitstream_get_any_field(regs_val,0,1,&tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (tmp) {
      *rif_is_valid = tmp;
      
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID, 2, 0, 137, 122, 5);
      res = soc_sand_bitstream_get_any_field(regs_val,0,16,out_rif);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_out_rif_get_unsafe()", 0, 0);
}


uint32
  arad_pp_diag_eg_drop_log_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  )
{
  uint32
    reg_val,
    trap_val,
    offset = 0,
    bit_indx;
  SOC_PPC_DIAG_MODE_INFO
    diag_mode;
  uint32
    res = SOC_SAND_OK;
  soc_reg_t egq_erpp_discard_int_reg  = INVALIDr,
            egq_erpp_discard_int_reg2 = INVALIDr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EG_DROP_LOG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(eg_drop_log);

  SOC_PPC_DIAG_EG_DROP_LOG_INFO_clear(eg_drop_log);



  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,
                                       READ_EPNI_PP_INT_REGr(unit, SOC_CORE_ALL, &reg_val));
  


  for(bit_indx = 0; bit_indx < ARAD_PP_DIAG_FILTER_NOF_ETPP_FILTERS; ++bit_indx) {
      if(reg_val & SOC_SAND_BIT(bit_indx)){
          trap_val = arad_pp_diag_filter_bit_to_enum[bit_indx];
          ARAD_PP_DIAG_FILTER_TO_LOG(eg_drop_log->drop_log,trap_val);
      }
  }

  if (SOC_REG_IS_VALID(unit, EGQ_ERPP_DISCARD_INT_REGr)){
      egq_erpp_discard_int_reg  = EGQ_ERPP_DISCARD_INT_REGr;
  } else {
      egq_erpp_discard_int_reg  = EGQ_ERPP_DISCARD_INTERRUPT_REGISTERr;
  }
  if (SOC_REG_IS_VALID(unit, EGQ_ERPP_DISCARD_INT_REG_2r)){
      egq_erpp_discard_int_reg2 = EGQ_ERPP_DISCARD_INT_REG_2r;
  } else {
      egq_erpp_discard_int_reg2 = EGQ_ERPP_DISCARD_INTERRUPT_REGISTER_2r;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,
      soc_reg32_get(unit, egq_erpp_discard_int_reg, REG_PORT_ANY, 0, &reg_val));
  offset = ARAD_PP_DIAG_FILTER_NOF_ETPP_FILTERS;

  for(bit_indx = 0; bit_indx < ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_1; ++bit_indx) {
      if(reg_val & SOC_SAND_BIT(bit_indx)){
          trap_val = arad_pp_diag_filter_bit_to_enum[bit_indx+offset];
          ARAD_PP_DIAG_FILTER_TO_LOG(eg_drop_log->drop_log,trap_val);
      }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,
      soc_reg32_get(unit, egq_erpp_discard_int_reg2, REG_PORT_ANY, 0, &reg_val));
  offset += ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_1;

  for(bit_indx = 0; bit_indx < ARAD_PP_DIAG_FILTER_NOF_ERPP_FILTERS_2; ++bit_indx) {
      if(reg_val & SOC_SAND_BIT(bit_indx)){
          trap_val = arad_pp_diag_filter_bit_to_enum[bit_indx+offset];
          ARAD_PP_DIAG_FILTER_TO_LOG(eg_drop_log->drop_log,trap_val);
      }
  }

  
  res = arad_pp_diag_mode_info_get_unsafe(unit, &diag_mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (diag_mode.flavor & SOC_PPC_DIAG_FLAVOR_CLEAR_ON_GET)
  {
    reg_val = 0xFFFFFFFF;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EPNI_PP_INT_REGr(unit, SOC_CORE_ALL, reg_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,
        soc_reg32_set(unit, egq_erpp_discard_int_reg, REG_PORT_ANY, 0,  reg_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,
        soc_reg32_set(unit, egq_erpp_discard_int_reg2, REG_PORT_ANY, 0,  reg_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_eg_drop_log_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_eg_drop_log_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EG_DROP_LOG_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_eg_drop_log_get_verify()", 0, 0);
}

STATIC
uint32
  arad_pp_diag_isem_access_key_to_lookup_type(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_ISEM_ACCESS_KEY_TYPE            key_type,
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_LKUP_TYPE              *lookup_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (key_type) {
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:                              
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID:                    
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_PCP_VID_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_TUNNEL_VID_VID:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PON_VD_ETHERTYPE_TUNNEL_PCP_VID_VID: 
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_DESIGNATED_VID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_AC_TST2:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TST2_TCAM:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID_VID:
    *lookup_type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;
    break; 
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L3:                             
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_FRR:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_ELI:                           
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_ELI:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L2_ELI:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF_ELI:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_PWE_L2_GAL:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VLAN_DOMAIN_MPLS:
    *lookup_type = SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB:
    *lookup_type = SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_SIP_DIP:  
    *lookup_type = SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_APPOINTED_FORWARDER:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NATIVE_INNER_TPID:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_VSI:
    *lookup_type = SOC_PPC_DIAG_LIF_LKUP_TYPE_TRILL;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VNI:   
    *lookup_type = SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_GRE:   
    *lookup_type = SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE;
    break;
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_FCOE:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV4:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SPOOF_IPV6:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_EoIP:    
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_SIP: 
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_DIP_AFTER_RECYCLE:  
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IPV6_TUNNEL_CASCADE_DIP_AFTER_RECYCLE:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_RPA:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_TT:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISA:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_TT_ISB:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_OAM_STAT_VT:
  case ARAD_PP_ISEM_ACCESS_KEY_TYPE_BFD_STAT_VT:
  default:
    break;
  } 

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_isem_access_key_to_lookup_type()", 0, 0);
}


uint32
  arad_pp_diag_db_lif_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO            *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LIF_LKUP_INFO          *lkup_info,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                 *ret_val

  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    prefix,
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    raw_key[6]={0},
    tmp=0,
    port=0;
  SOC_PPC_DIAG_MODE_INFO
    diag_mode;
  ARAD_PP_ISEM_ACCESS_ENTRY
    isem_res;
  ARAD_PP_ISEM_ACCESS_KEY
    isem_key;
  ARAD_PP_IHP_LIF_TABLE_TBL_DATA
    ihp_lif_table_data;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA vtt_port_config_tbl;
  uint8
    found;
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO    pwe_additional_info; 

  SOC_PPC_RIF_INFO dummy;
  uint32 vsi_dummy;
  SOC_PPC_MPLS_TERM_PROFILE_INFO mpls_term_profile; 
  uint32  valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_UNSAFE);

  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  *ret_val = SOC_PPC_DIAG_OK;  

  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_FRWRD_LPM_LKUP_GET, &valid);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid) {   
      *ret_val = SOC_PPC_DIAG_NOT_FOUND;                                                           
      SOC_SAND_EXIT_NO_ERROR;
  }   

  SOC_PPC_DIAG_LIF_LKUP_INFO_clear(lkup_info);
  SOC_PPC_DIAG_LIF_KEY_clear(&lkup_info->key);

  
  if (db_info->bank_id == 0) {
      if (db_info->lkup_num == 1) { 
          
          if (SOC_IS_QAX(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,25,0,17,0,5);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,23,0,18,0,5);
          } else if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,21,0,18,0,5);
          } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,14,0,17,0,5);
          }
      } else {
          
          if (SOC_IS_QAX(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,25,0,35,18,5);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,23,0,37,19,5);
          } else if (SOC_IS_JERICHO(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,21,0,37,19,5);
          } else {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,14,0,35,18,5);
          }
      }
  }
  else if (db_info->bank_id == 1) { 
      if (db_info->lkup_num == 1) { 
          
          if (SOC_IS_QAX(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,17,0,5);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,25,0,18,0,5);
          } else if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,23,0,18,0,5);
          } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,16,0,17,0,5);
          }
      } else {
          
          if (SOC_IS_QAX(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,35,18,5);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,25,0,37,19,5);
          } else if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,23,0,37,19,5);
          } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,16,0,35,18,5);
          }
      }
  }
  else if(db_info->bank_id == 2) { 
      if (db_info->lkup_num == 1) { 
          
          if (SOC_IS_QAX(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,15,0,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+1,1,5);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,13,0,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+1,1,5);
          } else if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,25,0,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+1,1,5);
          } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,12,0,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+1,1,5);
          }
      } else {
          
          if (SOC_IS_QAX(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,15,0,(4*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,5);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,13,0,(4*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,5);
          } else if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,25,0,(4*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,5);
          } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,12,0,(4*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,(2*SOC_DPP_DEFS_GET(unit,tcam_action_width))+3,5);
          }
      }
  } else { 
      if (db_info->lkup_num == 1)
          goto exit;
      lkup_info->found = TRUE;
      
      if (SOC_IS_JERICHO_PLUS(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2,5,105,98,5);
      } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2,5,35,28,5);
      } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,3,5,16,9,5);
      }

      res = soc_sand_bitstream_get_any_field(regs_val, 0, 8, &port);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
                   unit,
                   core_id,
                   port,
                   &vtt_port_config_tbl
                 );
      SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

      lkup_info->key.ac.raw_key = port;
      lkup_info->base_index = vtt_port_config_tbl.default_port_lif;
  }

  
  if(db_info->bank_id != 3) {
      if (db_info->bank_id == 2) {
          res = soc_sand_bitstream_get_any_field(regs_val, (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width)), 1, &tmp);
          SOC_SAND_CHECK_FUNC_RESULT(res, 1225, exit);
          lkup_info->found = SOC_SAND_NUM2BOOL(tmp);
      } else {
          res = soc_sand_bitstream_get_any_field(regs_val, (SOC_DPP_DEFS_GET(unit,isem_payload_nof_bits) + 1), 1, &tmp);
          SOC_SAND_CHECK_FUNC_RESULT(res, 1220, exit);
          lkup_info->found = SOC_SAND_NUM2BOOL(tmp);
      }
      if (!lkup_info->found) {
          goto exit;
      }
  }

    res = arad_pp_isem_access_entry_from_buffer( unit, regs_val, &isem_res);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  
  if (db_info->bank_id == 0 || db_info->bank_id == 1) { 
     if (db_info->bank_id == 0) {
         
         
         
         if (SOC_IS_QAX(unit)) {
           ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,24,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         } else if (SOC_IS_JERICHO_PLUS(unit)) {
             ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,22,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         } else if (SOC_IS_JERICHO(unit)) {
           ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,20,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         } else {
           ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,13,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         }
     } else { 
         
         
         
         if (SOC_IS_QAX(unit)) {
           ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,26,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         } else if (SOC_IS_JERICHO_PLUS(unit)) {
             ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,24,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         } else if (SOC_IS_JERICHO(unit)) {
           ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,22,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         } else {
           ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,15,0,2*SOC_DPP_DEFS_GET(unit,isem_width)-1,0,5);
         }
     }

     
     if (db_info->lkup_num == 1) { 
        res = soc_sand_bitstream_get_any_field(regs_val, 0, SOC_DPP_DEFS_GET(unit, isem_width), raw_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
     } else { 
        res = soc_sand_bitstream_get_any_field(regs_val, SOC_DPP_DEFS_GET(unit, isem_width), SOC_DPP_DEFS_GET(unit, isem_width), raw_key);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
     }
  } else if(db_info->bank_id == 2) { 
     if (db_info->lkup_num == 1) { 
         
         
         if (SOC_IS_QAX(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,14,0,165,0,10);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,12,0,165,0,10);
          } else if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,24,0,165,0,10);
          } else {
            ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,11,0,165,0,10);
          }
          res = soc_sand_bitstream_get_any_field(regs_val, 6, 160, raw_key);
          SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

          res = soc_sand_bitstream_set_any_field(regs_val, 161, 5, raw_key);
          SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      } else { 
          
          if (SOC_IS_QAX(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,14,0,255,166,12);
              tmp = 256-166;
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,12,0,255,166,12);
              tmp = 256-166;
          } else if (SOC_IS_JERICHO(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,24,0,255,172,12);
              tmp = 256-172;
          } else {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,11,0,255,166,12);
              tmp = 256-166;
          }
          res = soc_sand_bitstream_set_any_field(regs_val, 0, tmp, raw_key);
          SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

          if (SOC_IS_QAX(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,14,1,69,0,14);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,12,1,69,0,14);
          } else if (SOC_IS_JERICHO(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,24,1,75,0,14);
          } else {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,11,1,69,0,14);
          }
          res = soc_sand_bitstream_set_any_field(regs_val, tmp + 1, SOC_IS_JERICHO(unit)? 76: 70, raw_key);
          SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

          
          if (SOC_IS_QAX(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,14,1,75,70,13);
          } else if (SOC_IS_JERICHO_PLUS(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,12,1,75,70,13);
          } else if (SOC_IS_JERICHO(unit)) {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,24,0,171,166,13);
          } else {
              ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,11,1,75,70,13);
          }
          res = soc_sand_bitstream_set_any_field(regs_val, 161, 6, raw_key);
          SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
      }
  }

    if(db_info->bank_id != 3)
    {
        
        res = arad_pp_diag_mode_info_get_unsafe(
                unit,
                &diag_mode
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        
        res = arad_pp_isem_access_key_from_buffer(
                unit,
                raw_key,
                db_info->lkup_num,
                db_info->bank_id,
                &isem_key
        );

        res = arad_pp_isem_prefix_from_buffer(unit,raw_key,db_info->lkup_num,db_info->bank_id, &prefix);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

        res = arad_pp_isem_access_prefix_to_key_type(unit, prefix, db_info->lkup_num,db_info->bank_id, raw_key, &(isem_key.key_type));
        SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);



        if (diag_mode.flavor & SOC_PPC_DIAG_FLAVOR_RAW)
        {
            ARAD_PP_COPY(lkup_info->key.raw,raw_key,uint32,6);
        }
        else
        {
            res = arad_pp_diag_isem_access_key_to_lookup_type(
                    unit,
                    isem_key.key_type,
                    &lkup_info->type
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
        }

        
        lkup_info->base_index = isem_res.sem_result_ndx;

        res = arad_pp_ihp_lif_table_tbl_get_unsafe(unit, lkup_info->base_index, &ihp_lif_table_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        
        if (diag_mode.flavor & SOC_PPC_DIAG_FLAVOR_RAW)
        {
            ARAD_PP_COPY(lkup_info->value.raw,ihp_lif_table_data.lif_table_entry,uint32,4);
        }
        else if (ihp_lif_table_data.double_data_entry == 1) {
            ARAD_PP_COPY(lkup_info->value.raw,ihp_lif_table_data.lif_table_entry,uint32,4);
            lkup_info->type = SOC_PPC_DIAG_LIF_LKUP_TYPE_DOUBLE_DATA_ENTRY;
        }
        else
        {
            
            if ((ihp_lif_table_data.type == 0x0 || 
                    ihp_lif_table_data.type == 0x1) && 
                    (lkup_info->type == SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL) )
            {
                lkup_info->type = SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE;
            }
        }
    }
    else 
        lkup_info->type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;

    switch(lkup_info->type)
    {
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_AC:
        res = arad_pp_l2_lif_ac_get_internal_unsafe(unit, NULL, TRUE,
                                                    &lkup_info->base_index,
                                                    &(lkup_info->value.ac),
                                                    &found);

        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE:
        res = arad_pp_l2_lif_pwe_get_internal_unsafe(unit, 0, TRUE,
                                                     &lkup_info->base_index,
                                                     &pwe_additional_info,
                                                     &(lkup_info->value.pwe),
                                                     &found);

      res = arad_pp_mpls_term_profile_info_get_unsafe(unit, lkup_info->value.pwe.term_profile,&mpls_term_profile);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      lkup_info->value.pwe.has_cw = ((mpls_term_profile.flags & SOC_PPC_MPLS_TERM_HAS_CW) != 0);
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
      res = arad_pp_rif_mpls_label_map_get_internal_unsafe(unit, NULL, TRUE,
                                                           &lkup_info->base_index,
                                                           &(lkup_info->value.mpls),
                                                           &dummy,
                                                           &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
      res = arad_pp_rif_ip_tunnel_term_get_internal_unsafe(unit, NULL, TRUE,
                                                           &lkup_info->base_index,
                                                           &(lkup_info->value.ip),
                                                           &dummy,
                                                           &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_TRILL:
        res = arad_pp_l2_lif_trill_uc_get_internal_unsafe(unit, 0, TRUE,
                                                          &lkup_info->base_index,
                                                          &(lkup_info->value.trill),
                                                          &found);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID:

      res = arad_pp_l2_lif_isid_key_parse_unsafe(
              unit,
              &isem_key,
              &(lkup_info->key.mim)
            );

      res = arad_pp_l2_lif_isid_get_internal_unsafe(unit,
                                                    NULL,
                                                    TRUE, 
                                                    &lkup_info->base_index,
                                                    &vsi_dummy,
                                                    &(lkup_info->value.mim),
                                                    &found
                                                    );

      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);      
      break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE:
      res = arad_pp_l2_lif_gre_get_internal_unsafe(unit, NULL, TRUE,
                                                   &lkup_info->base_index,
                                                   &(lkup_info->value.l2gre),
                                                   &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN: 
      arad_pp_l2_lif_vxlan_get_internal_unsafe(unit, NULL, TRUE,
                                               &lkup_info->base_index,
                                               &(lkup_info->value.vxlan),
                                               &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
    default:
      break;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lif_lkup_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_db_lif_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LIF_LKUP_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_DIAG_DB_USE_INFO, db_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lif_lkup_info_get_verify()", 0, 0);
}


uint32
  arad_pp_diag_db_lem_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO              *db_info,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_LKUP_TYPE            *type,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_KEY                  *key,
    SOC_SAND_OUT SOC_PPC_DIAG_LEM_VALUE                *val,
    SOC_SAND_OUT uint8                                 *valid
  )
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    tmp=0;
  SOC_PPC_DIAG_MODE_INFO
    diag_mode;
  ARAD_PP_LEM_ACCESS_KEY
    lem_key_str;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
    lem_key_buf;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    lem_payload_str;
  ARAD_PP_ISEM_ACCESS_KEY
    isem_key;
  uint32
    lem_payload_buf[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S] = {0};
  uint32
    res = SOC_SAND_OK;
  uint32  valid_diag;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(type);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(valid);


  res = arad_pp_diag_is_valid(unit, core_id, ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET, &valid_diag);                    \
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  if (!valid_diag) {   
      *valid = 0;
      goto exit;                                                     
  }   

  SOC_PPC_DIAG_LEM_KEY_clear(key);
  SOC_PPC_DIAG_LEM_VALUE_clear(val);
  ARAD_PP_LEM_ACCESS_KEY_clear(&lem_key_str);  

  ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&lem_key_buf);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload_str);

  
  if (SOC_IS_QAX(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,28,0,205,0,5);
  } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,26,0,205,0,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,22,0,234,0,5);
  }
  
  *valid = FALSE;

  if (db_info->lkup_num == 0) {
    
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            SOC_IS_JERICHO(unit)? 0: 43,
            SOC_DPP_DEFS_GET(unit, lem_width),
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,29,0,95,0,5);
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,95,0,5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,21,0,90,0,5);
    }
    tmp = 0;
    
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_DPP_DEFS_GET(unit, lem_payload_width) + 2, 1, &tmp); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (tmp)
    {
      *valid = TRUE;
      
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              0,
              SOC_DPP_DEFS_GET(unit, lem_payload_width),
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  } else if (db_info->lkup_num == 1) {
    
    res = soc_sand_bitstream_get_any_field(
            regs_val,
            SOC_IS_JERICHO(unit)? 126: 161,
            SOC_DPP_DEFS_GET(unit, lem_width),
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    
    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,29,0,95,0,5);
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,95,0,5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,21,0,90,0,5);
    }

    
    
    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,7,127,127,51);
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,4,7,62,62,51);
    } else if (SOC_IS_ARADPLUS(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0,6,189,189,51);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,0,6,40,40,5);
    }
    tmp = regs_val[0];

    
    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,29,0,95,0,5);
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,27,0,95,0,5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHB_ID,21,0,90,0,5);
    }

    if (tmp) {
      *valid = TRUE;
      
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              SOC_DPP_DEFS_GET(unit, lem_payload_width) + 3,
              SOC_DPP_DEFS_GET(unit, lem_payload_width),
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  } else if (db_info->lkup_num == 2) {
    
      if (SOC_IS_JERICHO(unit)) {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,6,0,95,0,5);
      } else {
          ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,22,0,89,0,5);
      }
      
      res = soc_sand_bitstream_get_any_field(
            regs_val,
            SOC_IS_JERICHO(unit)? 0: 16,
            SOC_DPP_DEFS_GET(unit, lem_width),
            lem_key_buf.buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    

    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,2,0,255,0,5);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_IHP_ID,21,0,45,0,5);
    }
    tmp = 0;
    
    res = soc_sand_bitstream_get_any_field(regs_val, SOC_IS_JERICHO(unit)? 114: 45, 1, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (tmp)
    {
      *valid = TRUE;
      
      res = soc_sand_bitstream_get_any_field(
              regs_val,
              SOC_IS_JERICHO(unit)? 69: 2,
              SOC_DPP_DEFS_GET(unit, lem_payload_width),
              lem_payload_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

 
  res = arad_pp_diag_mode_info_get_unsafe(
          unit,
          &diag_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (diag_mode.flavor & SOC_PPC_DIAG_FLAVOR_RAW)
  {
    ARAD_PP_COPY(key->raw, lem_key_buf.buffer, uint32, SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit));
    ARAD_PP_COPY(val->raw, lem_payload_buf, uint32, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
    goto exit;
  }

  res = arad_pp_lem_key_encoded_parse(
          unit,
          &lem_key_buf,
          &lem_key_str
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*valid)
  {
    res = arad_pp_lem_access_payload_parse(
            unit,            
            lem_payload_buf,
            ARAD_PP_LEM_ACCESS_NOF_KEY_TYPES,
            &lem_payload_str
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC)
  {
    *type = SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT;
    res = arad_pp_frwrd_mact_key_parse(
            unit,
            &lem_key_str,
            &(key->mact)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(&(key->mact));
    if (!*valid)
    {
      goto exit;
    }
    res = arad_pp_frwrd_mact_payload_convert(
            unit,
            &lem_payload_str,
            &(val->mact)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM)
  {
    *type = SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM;
    
    arad_pp_frwrd_ilm_lem_key_parse(
            &lem_key_str,
            &(key->ilm)
          );
    
    if (!*valid)
    {
      goto exit;
    }
    
    arad_pp_frwrd_ilm_lem_payload_parse(
            unit,
            &lem_payload_str,
            &(val->ilm)
          );
    
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH)
  {
    *type  = SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH;
    res = arad_pp_sa_based_key_from_lem_key_map(
            unit,
            &lem_key_str,
            &(key->sa_auth)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!*valid)
    {
      goto exit;
    }
    res = arad_pp_sa_based_payload_from_lem_payload_map(
            unit,
            &lem_payload_str,
            &(val->sa_auth),
            &(val->sa_vid_assign)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_IP_HOST)
  {
    *type  = SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST;

    key->host.subnet.subnet.ip_address = lem_key_str.param[0].value[0];
    key->host.subnet.subnet.prefix_len = 32;
    key->host.vrf_id = lem_key_str.param[1].value[0];

    if (!*valid)
    {
      goto exit;
    }

    res = arad_pp_frwrd_em_dest_to_fec(
            unit,
            &lem_payload_str,
            &(val->host)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_EXTENDED)
  {
    *type  = SOC_PPC_DIAG_LEM_LKUP_TYPE_EXTEND_P2P;
    res = arad_pp_isem_access_key_from_buffer(unit,lem_key_str.param[0].value,db_info->lkup_num, db_info->bank_id, &isem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    switch(isem_key.key_type)
    {
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD:
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.key_type = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT;
      key->extend_p2p_key.key.ac.outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;
      key->extend_p2p_key.key.ac.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:    
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.key_type = (isem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID)?SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN:SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      key->extend_p2p_key.key.ac.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:    
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_AC;
      key->extend_p2p_key.key.ac.key_type = (isem_key.key_type == ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID)?SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN:SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN;;
      key->extend_p2p_key.key.ac.vlan_domain = isem_key.key_info.l2_eth.vlan_domain;
      key->extend_p2p_key.key.ac.outer_vid = isem_key.key_info.l2_eth.outer_vid;
      key->extend_p2p_key.key.ac.inner_vid = isem_key.key_info.l2_eth.inner_vid;
    break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PBB:
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID;
      key->extend_p2p_key.key.mim.isid_id = isem_key.key_info.pbb.isid;
      key->extend_p2p_key.key.mim.isid_domain = isem_key.key_info.pbb.isid_domain;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_L1_IN_RIF:
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      key->extend_p2p_key.key.mpls.label_id = isem_key.key_info.mpls.label;
      key->extend_p2p_key.key.mpls.vsid = isem_key.key_info.mpls.in_rif;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS:
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL;
      key->extend_p2p_key.key.mpls.label_id = isem_key.key_info.mpls.label;
      key->extend_p2p_key.key.mpls.vsid = 0;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL_DIP:
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL;
      key->extend_p2p_key.key.ip_tunnel.dip = isem_key.key_info.ip_tunnel.dip;
      key->extend_p2p_key.key.ip_tunnel.sip = isem_key.key_info.ip_tunnel.sip;
      key->extend_p2p_key.key.ip_tunnel.dip_prefix_len = isem_key.key_info.ip_tunnel.dip_prefix_len;
      key->extend_p2p_key.key.ip_tunnel.sip_prefix_len = isem_key.key_info.ip_tunnel.sip_prefix_len;
      break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK:
      key->extend_p2p_key.type = SOC_PPC_DIAG_LIF_LKUP_TYPE_TRILL;
      key->extend_p2p_key.key.nick_name = isem_key.key_info.trill.nick_name;
      break;
    default:
      break;
    }
    if (!*valid)
    {
      goto exit;
    }

  }
  else if (lem_key_str.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_ADJ)
  {
    *type  = SOC_PPC_DIAG_LEM_LKUP_TYPE_TRILL_ADJ;
    res = arad_pp_sa_based_trill_key_from_lem_key_map(
            unit,
            &lem_key_str,
            &(key->trill_adj.sys_port_id),
            &(key->trill_adj.mac_key)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!*valid)
    {
      goto exit;
    }
    res = arad_pp_sa_based_payload_from_lem_payload_map(
            unit,
            &lem_payload_str,
            &(val->sa_auth),
            &(val->sa_vid_assign)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  }
  else
  {
    ARAD_PP_COPY(key->raw, lem_key_buf.buffer, uint32, SOC_DPP_DEFS_LEM_WIDTH_IN_UINT32S(unit));
    ARAD_PP_COPY(val->raw, lem_payload_buf, uint32, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lem_lkup_info_get_unsafe()", 0, 0);
}

uint32
  arad_pp_diag_db_lem_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO                    *db_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_DB_LEM_LKUP_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_DIAG_DB_USE_INFO, db_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_db_lem_lkup_info_get_verify()", 0, 0);
}




uint32
  arad_pp_diag_egress_vlan_edit_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_egress_vlan_edit_info_get_verify()", 0, 0);
}

uint32
  arad_pp_diag_egress_vlan_edit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO      *prm_vec_res,
    SOC_SAND_OUT SOC_PPC_DIAG_RESULT                    *ret_val
  ) 
{
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
    ac_entry_reg[2];
  SOC_PPC_DIAG_ENCAP_INFO 
    encap_info;
  SOC_PPC_EG_AC_INFO 
    ac_fields;
  uint32
    res = SOC_SAND_OK,
    temp_buffer,
    tag_format_int = 0,
    edit_profile_size;
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
    cur_eep_type;
  uint64
      reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_DIAG_EGRESS_VLAN_EDIT_INFO_GET_UNSAFE);

  COMPILER_64_ZERO(reg_val);

  SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);

  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(prm_vec_res);

  SOC_PPC_DIAG_ENCAP_INFO_clear(&encap_info);
  *ret_val = SOC_PPC_DIAG_OK;
  

  arad_pp_diag_encap_info_get_unsafe(unit, core_id, &encap_info, ret_val);
  if (*ret_val != SOC_PPC_DIAG_OK) {
      goto exit;
  }

  
  
  SOC_PPC_EG_AC_INFO_clear(&ac_fields);

  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(unit, encap_info.out_ac, &cur_eep_type);

  if (cur_eep_type == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {

      arad_pp_eg_ac_info_get_unsafe(unit, encap_info.out_ac, &ac_fields);

      prm_vec_res->key.edit_profile = ac_fields.edit_info.edit_profile;
      prm_vec_res->ac_tag.vid = ac_fields.edit_info.vlan_tags[0].vid;
      prm_vec_res->ac_tag2.vid = ac_fields.edit_info.vlan_tags[1].vid;

  } else {

       
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EPNI_DEFAULT_AC_ENTRYr(unit, REG_PORT_ANY, &reg_val));

      ac_entry_reg[0] = COMPILER_64_LO(reg_val);
      ac_entry_reg[1] = COMPILER_64_HI(reg_val);

      edit_profile_size = SOC_DPP_DEFS_GET(unit, nof_eve_profile_bits);
      res = soc_sand_bitstream_get_any_field(ac_entry_reg, 28, edit_profile_size, &temp_buffer);
      SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
      prm_vec_res->key.edit_profile = temp_buffer;

      if (SOC_IS_JERICHO_PLUS(unit)) {
          res = soc_sand_bitstream_get_any_field(ac_entry_reg, 16, 12, &temp_buffer);
          SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
          prm_vec_res->ac_tag.vid = temp_buffer;

          res = soc_sand_bitstream_get_any_field(ac_entry_reg, 4, 12, &temp_buffer);
          SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
          prm_vec_res->ac_tag2.vid = temp_buffer;
      } else {
          res = soc_sand_bitstream_get_any_field(ac_entry_reg, 4, 12, &temp_buffer);
          SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
          prm_vec_res->ac_tag.vid = temp_buffer;

          res = soc_sand_bitstream_get_any_field(ac_entry_reg, 16, 12, &temp_buffer);
          SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          prm_vec_res->ac_tag2.vid = temp_buffer;
      }
  }


  
  
  if (SOC_IS_QAX_A0(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,77,69,5);
  } else if (SOC_IS_JERICHO_PLUS(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,157,149,5);
  } else if (SOC_IS_JERICHO(unit)) {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,149,141,5);
  } else {
      ARAD_PP_DIAG_FLD_READ(&fld,core_id,ARAD_EPNI_ID,11,0,88,81,5);
  }

  res = soc_sand_bitstream_get_any_field(
          regs_val,
          SOC_IS_JERICHO(unit)? 4: 3,
          4,
          &tag_format_int
        );

  (prm_vec_res->key).tag_format = tag_format_int;
  
  arad_pp_eg_vlan_edit_command_info_get_unsafe(
          unit, 
          &(prm_vec_res->key), 
          &(prm_vec_res->cmd_info)
  );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_diag_egress_vlan_edit_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_diag_cos_info_get_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 core_id 

  )
{
    uint32 res = SOC_SAND_OK;
    uint32 fld_val[4];
    uint32 eg_cos_aft_fwd_header_code; 
    uint32 eg_cos_aft_fwd_dscp_remark; 
    uint32 eg_cos_aft_fwd_exp_remark;     

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    
    
    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {  
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,102,100,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,107,106,fld_val[1]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,2,0,117,115,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,2,0,122,121,fld_val[1]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Ingress Initial COS assignment: {TC(%d), DP(%d)}\n"), 
                        fld_val[0], fld_val[1]));
                        
    
    
    
    
    
    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,3,16,11,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,58,56,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,62,61,fld_val[2]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,3,35,28,fld_val[3]); 
    } else if (SOC_IS_JERICHO_PLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,3,20,15,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,58,56,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,62,61,fld_val[2]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,3,39,32,fld_val[3]); 
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,2,185,180,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,58,56,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,0,62,61,fld_val[2]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,3,2,204,197,fld_val[3]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,2,133,128,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,0,58,56,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,0,62,61,fld_val[2]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,2,152,145,fld_val[3]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Ingress L2/L3 (LIF) COS: {COS-profile(%d), TC(%d), DP(%d), Terminated-DSCP-EXP(%d)}\n"), 
                        fld_val[0], fld_val[1],fld_val[2], fld_val[3]));                        

    
    
    
    
    if (SOC_IS_QAX(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,166,164,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,173,172,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,98,91,fld_val[2]); 
    } else if (SOC_IS_JERICHO_PLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,171,169,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,178,177,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,103,96,fld_val[2]); 
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,101,99,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,108,107,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHP_ID,core_id,4,8,33,26,fld_val[2]); 
    } else if (SOC_IS_ARADPLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,7,158,156,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,7,165,164,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,7,90,83,fld_val[2]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,7,9,7,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,7,16,15,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,6,197,190,fld_val[2]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Ingress FWD COS: {TC(%d), DP(%d), In-DSCP-EXP(%d)}\n"), 
                        fld_val[0], fld_val[1],fld_val[2])); 

    
    
    
    
    if (SOC_IS_JERICHO_PLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,13,207,205,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,13,209,208,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,13,155,148,fld_val[2]); 
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,11,9,7,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,11,11,10,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,0,10,213,206,fld_val[2]); 
    } else if (SOC_IS_ARADPLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,9,10,230,228,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,9,10,232,231,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,9,10,178,171,fld_val[2]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,9,10,197,195,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,9,10,199,198,fld_val[1]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,9,10,145,138,fld_val[2]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Ingress L2/L3 COS after FP: {TC(%d), DP(%d), In-DSCP-EXP(%d)}\n"), 
                        fld_val[0], fld_val[1],fld_val[2]));  
                        

    
    
    
    
    if (SOC_IS_JERICHO_PLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,2,3,55,53,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,2,3,57,56,fld_val[1]); 
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,2,2,182,180,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,2,2,184,183,fld_val[1]); 
    } else if (SOC_IS_ARADPLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,10,2,157,155,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,10,2,159,158,fld_val[1]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,12,0,129,127,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_IHB_ID,core_id,12,0,126,125,fld_val[1]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Ingress L2/L3 COS to Ingress TM: {TC(%d), DP(%d)}\n"), 
                        fld_val[0], fld_val[1]));                       

    
    
    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_EGQ_ID,core_id,4,1,50,48,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_EGQ_ID,core_id,4,1,23,22,fld_val[1]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_EGQ_ID,core_id,3,0,3,1,fld_val[0]); 
        ARAD_PP_DIAG_FLD_GET(ARAD_EGQ_ID,core_id,3,0,5,4,fld_val[1]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Egress COS before Egress TM: {TC(%d), DP(%d)}\n"), 
                        fld_val[0], fld_val[1]));    
                      
    
    
    
    
    
    ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,2,1,49,47,fld_val[0]); 
    ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,2,1,22,21,fld_val[1]); 
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,5,0,178,171,fld_val[2]);     
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,5,0,180,173,fld_val[2]); 
    }        
    LOG_CLI((BSL_META_U(unit,
                        "Egress COS after Egress TM: {TC(%d), DP(%d), In-DSCP-EXP(%d)}\n"), 
                        fld_val[0], fld_val[1],fld_val[2]));  

    
    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,5,0,178,171,fld_val[0]);         
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,5,0,182,179,eg_cos_aft_fwd_header_code); 
        if ((eg_cos_aft_fwd_header_code == 0) || 
            (eg_cos_aft_fwd_header_code == 1) || 
            (eg_cos_aft_fwd_header_code == 2)) { 
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,0,247,235,eg_cos_aft_fwd_dscp_remark); 
            fld_val[1] = 0;
            SHR_BITCOPY_RANGE(&fld_val[1], 0, &eg_cos_aft_fwd_dscp_remark, 6, 4);
            fld_val[2] = 0;
            SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_dscp_remark, 10, 2);
        } else if (eg_cos_aft_fwd_header_code == 5) { 
            ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,0,254,248,eg_cos_aft_fwd_exp_remark);
            fld_val[1] = 0;
            SHR_BITCOPY_RANGE(&fld_val[1] ,0, &eg_cos_aft_fwd_exp_remark, 1, 4);
            fld_val[2] = 0;
            SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_exp_remark, 5, 2);
        } else { 
            fld_val[1] = -1;
            fld_val[2] = -1;
        }           
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,7,0,74,67,fld_val[3]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,5,0,180,173,fld_val[0]);         
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,7,0,93,90,eg_cos_aft_fwd_header_code);   
        if ((eg_cos_aft_fwd_header_code == 0) || 
            (eg_cos_aft_fwd_header_code == 1) || 
            (eg_cos_aft_fwd_header_code == 2)) { 
            if (SOC_IS_QAX_B0(unit) || SOC_IS_QUX(unit)){
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,1,60,48,eg_cos_aft_fwd_dscp_remark);
            } else if (SOC_IS_QAX(unit)) {
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,0,236,224,eg_cos_aft_fwd_dscp_remark);
            } else if (SOC_IS_JERICHO_PLUS(unit)){
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,1,60,48,eg_cos_aft_fwd_dscp_remark);
            } else {
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,1,53,41,eg_cos_aft_fwd_dscp_remark);
            }
            fld_val[1] = 0;
            SHR_BITCOPY_RANGE(&fld_val[1], 0, &eg_cos_aft_fwd_dscp_remark, 6, 4);
            fld_val[2] = 0;
            SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_dscp_remark, 10, 2);
        } else if (eg_cos_aft_fwd_header_code == 5) { 
            if (SOC_IS_QAX(unit)) {
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,0,243,237,eg_cos_aft_fwd_exp_remark);
            } else if (SOC_IS_JERICHO_PLUS(unit)){
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,1,67,61,eg_cos_aft_fwd_exp_remark);
            } else {
                ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,11,1,60,54,eg_cos_aft_fwd_exp_remark);
            }
            fld_val[1] = 0;
            SHR_BITCOPY_RANGE(&fld_val[1], 0, &eg_cos_aft_fwd_exp_remark, 1, 4);
            fld_val[2] = 0;
            SHR_BITCOPY_RANGE(&fld_val[2], 0, &eg_cos_aft_fwd_exp_remark, 5, 2);
        } else { 
            fld_val[1] = -1;
            fld_val[2] = -1;
        }                    
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,7,0,76,69,fld_val[3]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Egress COS after FWD: {In-DSCP-EXP(%d), Remark-profile(%d), DP(%d), Out-DSCP-EXP(%d)}\n"), 
                        fld_val[0], fld_val[1],fld_val[2], fld_val[3]));
    
    
    
    if (SOC_IS_JERICHO_PLUS(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,8,0,77,70,fld_val[0]); 
    } else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,8,0,85,78,fld_val[0]); 
    } else {
        ARAD_PP_DIAG_FLD_GET(ARAD_EPNI_ID,core_id,8,0,73,66,fld_val[0]); 
    }
    LOG_CLI((BSL_META_U(unit,
                        "Egress COS after Encap: {Out-DSCP-EXP(%d)}\n"), 
                        fld_val[0]));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_sig_cos_print_cos_data()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_diag_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_diag;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_diag_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_diag;
}
uint32
  SOC_PPC_DIAG_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_DIAG_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->flavor, SOC_PPC_DIAG_FLAVOR_MAX, SOC_PPC_DIAG_FLAVOR_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_DIAG_MODE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_DIAG_DB_USE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_DIAG_DB_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->lkup_num, ARAD_PP_DIAG_LKUP_NUM_MAX, ARAD_PP_DIAG_LKUP_NUM_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->bank_id, ARAD_PP_DIAG_BANK_ID_MAX, ARAD_PP_DIAG_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_DIAG_DB_USE_INFO_verify()",0,0);
}
uint32
  SOC_PPC_DIAG_IPV4_VPN_KEY_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_DIAG_IPV4_VPN_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_NOF(info->vrf, SOC_DPP_DEFS_GET(unit, nof_vrfs), ARAD_PP_DIAG_VRF_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY, &(info->key), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_DIAG_IPV4_VPN_KEY_verify()",0,0);
}



uint32
  arad_pp_diag_prge_first_instr_get(int unit, int core_id, uint32 *first_instruction) {

    return dpp_dsig_read(unit, core_id, "ETPP", "PRGE", NULL, "PRGE_Instruction_1_Index", first_instruction, 1);

}






int
arad_pp_diag_ftmh_cfg_get(int unit, int * p_cfg_ftmh_lb_key_ext_en, int * p_cfg_ftmh_stacking_ext_enable)
{

        int
            rv = SOC_E_NONE;

        rv = soc_ftmh_cfg_get(unit,p_cfg_ftmh_lb_key_ext_en, p_cfg_ftmh_stacking_ext_enable);
        return rv;
}




uint32
arad_pp_diag_epni_prge_program_tbl_get(int unit, uint32 offset, ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA *tbl_data)
{

        uint32
            rv = SOC_E_NONE;

        rv = arad_pp_epni_prge_program_tbl_get_unsafe( unit, offset, tbl_data);

        return rv;
}




int
arad_pp_diag_mem_read(int unit,
                        soc_mem_t mem,
                        int copyno,
                        int in_line,
                        void* val){

        int
            rv = SOC_E_NONE;

        if (copyno == -1) {
            rv = soc_mem_read(unit, mem, EPNI_BLOCK(unit, SOC_CORE_ALL), in_line, val); 
        }

        else{
            rv = soc_mem_read(unit, mem, copyno, in_line, val); 
        }

        return rv;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
