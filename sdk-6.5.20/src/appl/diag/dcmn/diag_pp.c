/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag.c
 * Purpose:     Device diagnostics commands.
 */

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>

#include <soc/hwstate/hw_log.h>

#include <appl/diag/diag.h>
#include <appl/diag/diag_pp.h>
#include <appl/diag/diag_alloc.h>

#include <soc/drv.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <bcm/vswitch.h>
#include <bcm/policer.h>
#include <bcm/l3.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/policer.h>

#include <sal/appl/sal.h>
#include <appl/diag/dcmn/diag.h>
#include <appl/diag/sand/diag_sand_dsig.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/system.h>
#include <appl/dpp/FecAllocation/fec_allocation.h>
#include <appl/dpp/FecPerformance/fec_performance.h>

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_metering.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPC/ppc_api_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/JER/JER_PP/jer_pp_metering.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/sand/sand_signals.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/QAX/qax_multicast_imp.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#if defined(BCM_JERICHO_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#endif

/* Use at the beginning of every procedure using Signal DB to avoid multiple checks inside */
#define VERIFY_SIGNAL_DB(unit)                                          \
  if(sand_signal_device_get(unit) == NULL) {                            \
      LOG_CLI((BSL_META_U(unit,                                         \
        "Signal DB for %s was not found\n\r"), SOC_CHIP_STRING(unit))); \
      return CMD_FAIL;                                                  \
  }

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>

#ifndef BSL_LOG_MODULE
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX
#endif

/*************
 * DEFINES   *
 */
#define DIAG_PP_GET_BLOCK_NOF_ENTRIES 100

#define DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, attribute, size) \
    PRT_ROW_ADD(PRT_ROW_SEP_NONE); \
    PRT_CELL_SET("%s",attribute); \
    PRT_CELL_SET("%d",size); \
    if (size > 32) { \
        PRT_CELL_SET("%02x%08x%08x",ftmh_header_old[2], ftmh_header_old[1], ftmh_header_old[0]); \
        PRT_CELL_SET("%02x%08x%08x",ftmh_header_new[2], ftmh_header_new[1], ftmh_header_new[0]); \
    } else { \
        soc_sand_bitstream_get_field(ftmh_header_old, offset, size, &val_old); \
        soc_sand_bitstream_get_field(ftmh_header_new, offset, size, &val_new); \
        PRT_CELL_SET("%x",val_old); \
        PRT_CELL_SET("%x",val_new); \
    } \
    offset += size;

cmd_result_t cmd_ppd_api_diag_frwrd_decision_trace_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_outlif_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_outlif_info_get_ll(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_outlif_info_get_vsi(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_received_packet_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_parsing_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_db_lif_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_termination_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_traps_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_encap_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_frwrd_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_frwrd_lpm_lkup_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_ing_vlan_edit_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_learning_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_mode_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_mode_info_set(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_pkt_associated_tm_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_pkt_trace_clear(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_db_lem_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_vsi_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_rif_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_vtt_dump_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_flp_dump_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_egress_vlan_edit_info_get(int unit, args_t* a); 
cmd_result_t cmd_ppd_api_diag_mtr_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_eth_policer_info_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_aggregate_eth_policer_info_get(int unit, args_t* a);
cmd_result_t cmd_diag_gport_info_get(int unit, args_t* a);
cmd_result_t cmd_bcm_cross_connect_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_ipv4_mc_route_get(int unit, args_t* a);
cmd_result_t cmd_diag_occupation_profile_info_get(int unit, args_t* a);
cmd_result_t cmd_diag_egress_drop(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_lif_show(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_pp_last(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_tpid_globals_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_tpid_profiles_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_fec_get(int unit, args_t* a);
cmd_result_t cmd_diag_fec_last(int unit, int core);
cmd_result_t cmd_diag_fec_print_all(int unit);
cmd_result_t cmd_diag_fec_print(int unit, int fec_ndx, bcm_l3_egress_t* fec_obj);
cmd_result_t cmd_ppd_api_diag_tpid_port_profile_get(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_db_glem_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_diag_fec_allocation(int unit);
cmd_result_t cmd_diag_occ_mgmt(int unit, args_t* a);
cmd_result_t cmd_ppd_api_diag_cos_get(int unit, args_t* a);
cmd_result_t cmd_diag_fec_performance(int unit, args_t* a);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
cmd_result_t cmd_ppd_api_diag_kbp_info_and_flags(int unit, args_t* a);
#endif
#ifdef BCM_JERICHO_SUPPORT
cmd_result_t cmd_diag_kaps_lkup_info_get(int unit, args_t* a);
cmd_result_t cmd_diag_load_balancing(int unit, args_t* a);
#endif /* BCM_JERICHO_SUPPORT */
cmd_result_t cmd_diag_ftmh_header_change_show(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_full_test(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_mem_test(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_mem_test1(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_reg_test(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_show(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_commit(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_purge(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_diag(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_set_immediate_hw_access(int unit, args_t* a);
cmd_result_t cmd_diag_hw_log_restore_hw_access(int unit, args_t* a);
/*extern int soc_crash_rec_test(int unit, soc_mem_t mem, void* data);
extern void soc_crash_rec_print_list(int unit);
extern void soc_hw_log_commit(int unit);*/
extern const char *arad_pp_lb_vector_names[];
extern const char *arad_pp_lb_polynomial_names[];
const DIAG_PP_COMMAND_DEF diag_pp_command_def[] =
{
/*     --------------------------------------------------------------------------------
      |        Long          |   Short    |             Print                         |
      |      Command Name    |Command Name|            Function                       |
       -------------------------------------------------------------------------------- */
    { "Frwrd_Decision_Trace"  , "FDT"    , cmd_ppd_api_diag_frwrd_decision_trace_get  },
    { "Outlif_Info"           , "OL"     , cmd_ppd_api_diag_outlif_info_get           },
    { "Outlif_Info_vsi"       , "OL_VSI" , cmd_ppd_api_diag_outlif_info_get_vsi       },
    { "Outlif_Info_ll"        , "OL_LL"  , cmd_ppd_api_diag_outlif_info_get_ll        },
    { "Received_Packet_Info"  , "RPI"    , cmd_ppd_api_diag_received_packet_info_get  },
    { "Parsing_Info"          , "PI"     , cmd_ppd_api_diag_parsing_info_get          },
    { "DB_LIF_lkup_info"      , "DBLIF"  , cmd_ppd_api_diag_db_lif_lkup_info_get      },
    { "TERMination_Info"      , "TERMI"  , cmd_ppd_api_diag_termination_info_get      },
    { "TRAPS_Info"            , "TRAPSI" , cmd_ppd_api_diag_traps_info_get            },
    { "ENCAP_info"            , "ENCAP"  , cmd_ppd_api_diag_encap_info_get            },
    { "Frwrd_Lkup_Info"       , "FLI"    , cmd_ppd_api_diag_frwrd_lkup_info_get       },
    { "Frwrd_LPM_lkup"        , "FLPM"   , cmd_ppd_api_diag_frwrd_lpm_lkup_get        },
    { "Ing_Vlan_Edit_info"    , "IVE"    , cmd_ppd_api_diag_ing_vlan_edit_info_get    },
    { "LeaRNing_info"         , "LRN"    , cmd_ppd_api_diag_learning_info_get         },
    { "MODE_info_Get"         , "MODEG"  , cmd_ppd_api_diag_mode_info_get             },
    { "MODE_info_Set"         , "MODES"  , cmd_ppd_api_diag_mode_info_set             },
    { "PKT_associated_TM_info", "PKTTM"  , cmd_ppd_api_diag_pkt_associated_tm_info_get},
    { "PKT_TRace_Clear"       , "PKTTRC" , cmd_ppd_api_diag_pkt_trace_clear           },
    { "MeTeR_info_get"        , "MTR"    , cmd_ppd_api_diag_mtr_info_get              },
    { "EthPoLiCeR_info_get"   , "EPLCR"   , cmd_ppd_api_diag_eth_policer_info_get      },
    { "AGgregateEthPoLiCeR_info_get", "AGEPLCR" , cmd_ppd_api_diag_aggregate_eth_policer_info_get },
	{ "DB_LEM_lkup_info"      , "DBLEM"  , cmd_ppd_api_diag_db_lem_lkup_info_get      },
    { "Egress_DROP"           , "EDROP"  , cmd_diag_egress_drop                       },
    { "LIF_show"              , "LIF"    , cmd_ppd_api_diag_lif_show                  },
    { "LAST"                  , "LAST"   , cmd_ppd_api_diag_pp_last                   },
    { "TPID_Globals_show"     , "TPIDG"  , cmd_ppd_api_diag_tpid_globals_get          },
    { "TPID_PRofiles_show"    , "TPIDPR" , cmd_ppd_api_diag_tpid_profiles_get         },    
    { "TPID_POrt_profile_show", "TPIDPO" , cmd_ppd_api_diag_tpid_port_profile_get     },
    { "FEC"                   , "FEC"    , cmd_ppd_api_diag_fec_get                   },
    { "VTT_DUMP"              , "VTT"    , cmd_ppd_api_diag_vtt_dump_get              },
    { "FLP_DUMP"              , "FLP"    , cmd_ppd_api_diag_flp_dump_get              },
#if !defined(__KERNEL__)
    { "Signals"               , "SIG"    , cmd_sand_dsig_show                         },
#endif
    { "Egr_Vlan_Edit_Info"    , "EVE"    , cmd_ppd_api_diag_egress_vlan_edit_info_get },
    { "GPort"                 , "GP"     , cmd_diag_gport_info_get                    },
    { "Cross-Connect-traverse", "CC"     , cmd_bcm_cross_connect_get                  },
    { "IPv4 MC Route Table"   , "IPv4_MC", cmd_ppd_api_diag_ipv4_mc_route_get         },
    { "VSI_decision"          , "VSI"    , cmd_ppd_api_diag_vsi_info_get              },
    { "RIF_profile_info"      , "RIF"    , cmd_ppd_api_diag_rif_info_get              },
    { "If_PROFILE_Status"     , "IPROFILE" , cmd_diag_occupation_profile_info_get     },
    { "FTMH_Header_Change_Show", "FTMHHC" , cmd_diag_ftmh_header_change_show          },
#ifdef CRASH_RECOVERY_SUPPORT
    { "HW_Log_Full_Test"      , "HWLOGFULLTEST",    cmd_diag_hw_log_full_test         },
    { "HW_Log_Single_Mem_Test", "HWLOGTESTMEM",     cmd_diag_hw_log_mem_test          },
    { "HW_Log_Single1_Mem_Test","HWLOGTESTMEM1",    cmd_diag_hw_log_mem_test1         },
    { "HW_Log_Single_Test_Reg", "HWLOGTESTREG",     cmd_diag_hw_log_reg_test          },
    { "HW_Log_Show"           , "HWLOGSHOW",        cmd_diag_hw_log_show              },
    { "HW_Log_Commit"         , "HWLOGCOMMIT",      cmd_diag_hw_log_commit            },
    { "HW_Log_Purge"          , "HWLOGPURGE",       cmd_diag_hw_log_purge             },
    { "HW_Log_Diagnostics"    , "HWLOGDIAG",        cmd_diag_hw_log_diag              },
    { "HW_Log_Set_Immediate_Hw_Access", "HWLOGIM",       cmd_diag_hw_log_set_immediate_hw_access},
    { "HW_Log_Restore_Hw_Access"      , "HWLOGRESTORE",  cmd_diag_hw_log_restore_hw_access},
#endif /* CRASH_RECOVERY_SUPPORT */
    { "OCCupation_mgmt_show"  , "OCC",              cmd_diag_occ_mgmt                 },
    { "Class_Of_Service"      , "COS" ,   cmd_ppd_api_diag_cos_get                    },
    { "DB_GLEM_lkup_info"     , "DBGLEM", cmd_ppd_api_diag_db_glem_lkup_info_get      },
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
	{ "KBP_diagnostics"  	  , "KBP"  ,  cmd_ppd_api_diag_kbp_info_and_flags         },
#endif /* defined(INCLUDE_KBP) */
#ifdef  BCM_JERICHO_SUPPORT
    { "Kaps_LKuP_info_get"    , "KLKP",    cmd_diag_kaps_lkup_info_get                },
    { "Load_balancing"        , "LB",       cmd_diag_load_balancing                   },
#endif /* BCM_JERICHO_SUPPORT */
};

#ifdef CRASH_RECOVERY_SUPPORT
cmd_result_t cmd_diag_hw_log_full_test(int unit, args_t* a)
{
    bcm_error_t rv;
    rv = soc_mem_iterate(unit, soc_hw_log_mem_test, NULL);
    if( rv != BCM_E_NONE) {

        return CMD_FAIL;
    }
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_mem_test(int unit, args_t* a)
{
    soc_hw_log_mem_test(unit, 188, NULL);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_mem_test1(int unit, args_t* a)
{
    soc_hw_log_mem_test(unit, 689, NULL);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_reg_test(int unit, args_t* a)
{
    soc_hw_log_reg_test(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_show(int unit, args_t* a)
{
    soc_hw_log_print_list(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_commit(int unit, args_t* a)
{
    soc_hw_log_commit(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_purge(int unit, args_t* a)
{
    soc_hw_log_purge(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_diag(int unit, args_t* a)
{
    cli_out("this diag is deprecated\n");
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_set_immediate_hw_access(int unit, args_t* a)
{
    soc_hw_set_immediate_hw_access(unit);
    return CMD_OK;
}

cmd_result_t cmd_diag_hw_log_restore_hw_access(int unit, args_t* a)
{
    soc_hw_restore_immediate_hw_access(unit);
    return CMD_OK;
}

#endif /* CRASH_RECOVERY_SUPPORT */

/******************************************************************** 
 *  Function handler:  frwrd_decision_trace_get (section diag)
 */
int dnx_diag_frwrd_decision_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *frwrd_trace_info_per_core);
cmd_result_t
  cmd_ppd_api_diag_frwrd_decision_trace_get(int unit, args_t* a) 
{   
  uint32 
    ret; 
  uint32
    soc_sand_dev_id; 
  parse_table_t    pt;
  int              prm_core=-1;
  int              core, first_core, last_core;
  SOC_PPC_DIAG_RESULT           ret_val;

  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO   
    prm_frwrd_trace_info;

  /*
   * For printing to xml file
   */
  char                              *prm_file = NULL;
#if !defined(NO_FILEIO)
  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO      frwrd_trace_info_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)];
#endif
  uint8                             b_print_to_xml = 0;

  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }

  VERIFY_SIGNAL_DB(unit);

  if ((prm_core < -1 ) 
      || 
    (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {

       cli_out("error ilegal core ID for device\n");
       parse_arg_eq_done(&pt);
       return CMD_FAIL;
  } 
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }
  if (prm_core == -1 || b_print_to_xml) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {
  /* Call function */

      ret = soc_ppd_diag_frwrd_decision_trace_get(
              soc_sand_dev_id,
              core,
              &prm_frwrd_trace_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      { 
          if (!b_print_to_xml) {
              LOG_CLI((BSL_META_U(unit, "%s Packet not found\n\r"),get_core_str(unit, core)));
          }

      } else {
          if (ret_val != SOC_PPC_DIAG_OK) {
              if (!b_print_to_xml) {
                  LOG_CLI((BSL_META_U(unit, "%s Packet not found\n\r"),get_core_str(unit, core)));
              }
          }
          else {
              if (!b_print_to_xml) {
                  cli_out("%s\r\n", get_core_str(unit, core));
                  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_print(&prm_frwrd_trace_info);
              } else {
#if !defined(NO_FILEIO)
                  frwrd_trace_info_per_core[core] = prm_frwrd_trace_info;
#endif
              }

          }
      }
  }
  if (b_print_to_xml) {
          /*
          * Print to XML file
          */
#if !defined(NO_FILEIO)
          dnx_diag_frwrd_decision_info_info_xml_print(unit, prm_file, frwrd_trace_info_per_core);
#endif
      }

  parse_arg_eq_done(&pt);
  return CMD_OK;
} 

#if !defined(NO_FILEIO)
int dnx_diag_frwrd_decision_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO *frwrd_trace_info_per_core){

    int core = 0;
    uint32 ind = 0;
    xml_node curTop, curDiags, curDiag, curCores, curCore, decision_phase ;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
         * Add xml structure:
         * <diag cat="pp" name="Frwrd_Decision_Trace">
                <cores list="Yes">
                    
                </cores>
         * </diag>
         */

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "Frwrd_Decision_Trace"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                {
                    for (ind = 0; ind < SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES; ++ind) { 
                        SHR_NULL_CHECK((decision_phase = dbx_xml_child_add(curCore, "phase", 5)), _SHR_E_FAIL, "decision_phase");
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "decision_phase", (char *)SOC_PPC_DIAG_FRWRD_DECISION_PHASE_to_string(ind)));
                        if (frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.type != SOC_PPC_NOF_FRWRD_DECISION_TYPES) {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "frwrd_decision_type", (char *)SOC_PPC_FRWRD_DECISION_TYPE_to_string(frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.type)));
                                  if (frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_DROP)
                                  {
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "frwrd_decision_id", "drop"));
                                  }
                                  else
                                  {
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "frwrd_decision_id", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.dest_id));
                                  }

                                  switch(frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.type)
                                  {  
                                    case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
                                    case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
                                    case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
                                        if(frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_EMPTY)
                                        {
                                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "uc_outlif_type", (char *)SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.outlif.type)));
                                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "uc_outlif_val", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.outlif.val));                                  
                                            break;
                                        }

                                        
                                    case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
                                    case SOC_PPC_FRWRD_DECISION_TYPE_MC:
                                       
                                        switch(frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.type)
                                        {
                                          case SOC_PPC_EEI_TYPE_EMPTY:
                                              SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "mc_eei_val", "empty"));                                  
                                            break;
                                        case SOC_PPC_EEI_TYPE_OUTLIF:
                                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "mc_eei_outlif_val", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.outlif));                                  
                                        break;
                                          case SOC_PPC_EEI_TYPE_MIM:
                                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "mc_eei_mim_isid", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.isid));
                                            break;
                                          case SOC_PPC_EEI_TYPE_MPLS:
                                            if (frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_PUSH)
                                            {
                                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "eei_type_mpls_push_label", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.mpls_command.label));
                                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "eei_type_mpls_push_profile", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.mpls_command.push_profile));
                                            }
                                            else if (frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.mpls_command.command == SOC_PPC_MPLS_COMMAND_TYPE_SWAP)
                                            {
                                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "eei_type_mpls_swap_label", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.mpls_command.label));
                                            }
                                            else /*POP*/
                                            {
                                                SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "eei_type_mpls_pop", (char *)SOC_PPC_MPLS_COMMAND_TYPE_to_string(frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.mpls_command.command)));
                                            }
                                            break;
                                          case SOC_PPC_EEI_TYPE_TRILL:
                                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "eei_type_trill_nick", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.trill_dest.dest_nick));
                                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "eei_type_trill_type", (frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.eei.val.trill_dest.is_multicast ? "MC" : "UC")));
                                            break;
                                          default:
                                            break;
                                        }

                                        break;

                                  case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "trap_code", (char *)SOC_PPC_TRAP_CODE_to_string(frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.trap_info.action_profile.trap_code)));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "trap_strength", frwrd_trace_info_per_core->frwrd[ind].frwrd_decision.additional_info.trap_info.action_profile.frwrd_action_strength));
                                    break;
                                  default:
                                    break;
                                  }
                        } else {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "frwrd_decision_type", "Not Valid"));
                        }

                        if (frwrd_trace_info_per_core->frwrd[ind].outlif != 0)
                        {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "outlif_val", frwrd_trace_info_per_core->frwrd[ind].outlif));
                        }

                        if (ind != SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED && frwrd_trace_info_per_core->trap[ind].action_profile.frwrd_action_strength > 0) {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(decision_phase, "trap_code",(char *)SOC_PPC_TRAP_CODE_to_string(frwrd_trace_info_per_core->trap[ind].action_profile.trap_code)));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "trap_action_strength",frwrd_trace_info_per_core->trap[ind].action_profile.frwrd_action_strength));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(decision_phase, "trap_qual",frwrd_trace_info_per_core->trap_qual[ind]));
                        }
                    } 
                }
                dbx_xml_node_end(curCore, 4);

            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);
    }
    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

 
 exit:
    SHR_FUNC_EXIT;
}
#endif


/******************************************************************** 
 *  Function handler: received_packet_info_get (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_received_packet_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO   
    prm_rcvd_pkt_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=-1;
  SOC_PPC_DIAG_RESULT  ret_val;
  int              core, first_core, last_core;

  soc_sand_dev_id = (unit);
  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(&prm_rcvd_pkt_info);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  VERIFY_SIGNAL_DB(unit);

  if ((prm_core < -1 ) || (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores)))
  {
       cli_out("error illegal core ID for device\n");
       return CMD_FAIL;
  } 

  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_received_packet_info_get(
              soc_sand_dev_id,
              core,
              &prm_rcvd_pkt_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
          return CMD_FAIL; 
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Packet not found\n\r"),get_core_str(unit, core)));
          continue; 
      } 
      cli_out("%s\r\n", get_core_str(unit, core));

      SOC_PPC_DIAG_RECEIVED_PACKET_INFO_print(&prm_rcvd_pkt_info);
  }
  return CMD_OK;
} 
  

/******************************************************************** 
 *  Function handler:  cmd_ppd_api_diag_outlif_info_get(section diag)
 */
cmd_result_t 
   cmd_ppd_api_diag_outlif_info_get (int unit, args_t* a)
{
    /*defines*/
    uint32 counter=0, eedb_index=0, DB_size=0, nof_entries=0,error_count=0;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];/*where we put the lif info before proccessing*/ 
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];
    int rv = BCM_E_NONE;
    /*SOC_PPC_EG_ENCAP_ENTRY_TYPE entry_type;*/
    SOC_PPC_EG_ENCAP_EEP_TYPE eedb_entry_type;
    _bcm_lif_type_e out_lif_usage;
    int rsvs_offset = 0;
    int rsvs_mask = 0;

    /*function calls*/
    DB_size=SOC_DPP_DEFS_GET(unit,nof_out_lifs);
    for (rsvs_offset = 0; rsvs_offset < SOC_DPP_DEFS_GET(unit, nof_eg_encap_rsvs); rsvs_offset++)
    {
        rsvs_mask += (1 << rsvs_offset);
    }
    /*
    *  In Jericho+ and above device, the rifs are not in the EEDB, but they still share the same index
    *  space as the other outlifs.
    */
    eedb_index = SOC_IS_JERICHO_PLUS(unit) ? SOC_DPP_CONFIG(unit)->l3.nof_rifs : 0;
    for (; eedb_index < DB_size; eedb_index++) 
    {
        if (SOC_IS_QUX_A0(unit))
        {
            /* OUTLIF format: bank ID + nof_eg_encap_rsvs + nof_eg_encap_lsbs 1LSB
            * The outlifs which reserve bits != 0 isn't valid, skip them  
            */
            rsvs_offset = (SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs) + 1);
            if (((eedb_index - SOC_DPP_CONFIG(unit)->l3.nof_rifs) >> rsvs_offset) & rsvs_mask)
            {
                continue;
            }
        }

        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get ( unit, 0, eedb_index, NULL, &out_lif_usage); /*get eedb entry type*/
        if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
        { 
            error_count++;
            continue ;     
        }

        eedb_entry_type = SOC_PPC_EG_ENCAP_EEP_TYPE_DATA;            /*default value (this is used only to tell*/
                                                               /* PWE and MPLS, unless the type is PWE according to the entry type, we'll assume mpls*/
        if (out_lif_usage == _bcmDppLifTypeMplsPort ) 
        {
            eedb_entry_type = SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP;
        }

        if (out_lif_usage != _bcmDppLifTypeVlan ) 
        {         /* && out_lif_usage != _bcmDppLifTypeAny      this data encapsulation is not supported*/
            rv = soc_ppd_eg_encap_entry_get(unit,           /*and cannot be displayed, hence, to avoid repetetive error printing on such entries, we skip them*/
                       eedb_entry_type, eedb_index, 0,
                       encap_entry_info, next_eep, &nof_entries);
            if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
            { 
               error_count++;
               continue ;     
            }                                  
            if (encap_entry_info[0].entry_type != SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_ARAD )
               /*the type used for undefined entries (empty)*/
            {
                counter+=1;  
                cli_out("_________________________________________________\r\n");
                cli_out("entry ID %d contains the LIF:\n\r",eedb_index);                  
                SOC_PPC_EG_ENCAP_ENTRY_INFO_print(unit, &(encap_entry_info[0])) ; 
                cli_out("_________________________________________________\r\n \r\n");      

                /*For NON Half-Entry type, eedb index should increment by 2*/
                if ((encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI) &&
                  (encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC)) 
                {
                   eedb_index++;
                }
            }
        }
    } 

    cli_out("The number of valid outlifs retrieved from EEDB is : %d\r\n",counter);
    cli_out("The number of failed attempts, due to errors is : %d\r\n",error_count);
    return CMD_OK;
}
/******************************************************************** 
 *  Function handler:  cmd_ppd_api_diag_outlif_info_get_VSI(section diag)
 */
cmd_result_t 
    cmd_ppd_api_diag_outlif_info_get_vsi (int unit, args_t* a)
{
 
    /*defines*/
   uint32 counter=0, eedb_index=0, DB_size=0, nof_entries=0,error_count=0;
   SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];/*where we put the lif info before proccessing */
   uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];
   int rv = BCM_E_NONE;
   /*SOC_PPC_EG_ENCAP_ENTRY_TYPE entry_type;*/
   SOC_PPC_EG_ENCAP_EEP_TYPE eedb_entry_type;
   _bcm_lif_type_e out_lif_usage;

   /*function calls*/
   DB_size=SOC_DPP_DEFS_GET(unit,nof_out_lifs);
   for (eedb_index=0;eedb_index<DB_size;eedb_index++) {
      rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get ( unit, 0, eedb_index, NULL, &out_lif_usage); /*get SW eedb entry type*/
      if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
      { 
          error_count++;
          continue ;     
      }
       eedb_entry_type = SOC_PPC_EG_ENCAP_EEP_TYPE_DATA;            /*default value (this is used only to tell*/
                                                                    /*PWE and MPLS, unless the type is PWE according to the entry type, we'll assume mpls*/
       if (out_lif_usage == _bcmDppLifTypeMplsPort ) {
           eedb_entry_type = SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP;
       }
       if (out_lif_usage != _bcmDppLifTypeVlan && out_lif_usage != _bcmDppLifTypeAny) {  /*this data encapsulation is not supported */                                                            
           rv =soc_ppd_eg_encap_entry_get(unit,                                          /*and cannot be displayed, hence, to avoid repetetive error printing on such entries, we skip them*/ 
                  eedb_entry_type, eedb_index, 0,                                        /*this function retrieves the entry*/
                  encap_entry_info, next_eep, &nof_entries);
                    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
                    { 
                        error_count++;
                        continue ;     
                    }
            if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI)
                {
                if (counter==0) {
                    cli_out("___________________________________________________________________\n\r");
                    cli_out("EEDB index |Remark Profile|Out VSI    |OAM LIF Set|Outlif Profile |\n\r");
                }
                    cli_out("  %-9d|", eedb_index);
                    cli_out("  %-12u|", encap_entry_info[0].entry_val.vsi_info.remark_profile);/*print aligned swap info- the rest are the same*/
                    cli_out("  %-9u|", encap_entry_info[0].entry_val.vsi_info.out_vsi);
                    cli_out("  %-9u|", encap_entry_info[0].entry_val.vsi_info.oam_lif_set);
                    cli_out("     0x%02x      |\n\r", encap_entry_info[0].entry_val.vsi_info.outlif_profile);
                    counter += 1;/*keep track of the number of lifs of a certain kind*/
 
                }
        }
    }
    if (counter!=0) { /*give output as to how many lifs are defined*/
        cli_out("There are %d VSI entries in the EEDB\n\r",counter);
    }
    else{cli_out("There are no VSI entries in the EEDB\r\n");}
    cli_out("There have been %d entries that were not obtained due to error\n\r",error_count);
    return CMD_OK;
}



/******************************************************************** 
 *  Function handler:  cmd_ppd_api_diag_outlif_info_get_ll(section diag)
 */
cmd_result_t 
    cmd_ppd_api_diag_outlif_info_get_ll (int unit, args_t* a)
{
 
    /*defines*/
   uint32 counter=0, eedb_index=0, DB_size=0, nof_entries=0,error_count=0;
   SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];/*where we put the lif info before proccessing*/ 
   uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_ARAD];
   int rv = BCM_E_NONE;
  /*SOC_PPC_EG_ENCAP_ENTRY_TYPE entry_type;*/
   SOC_PPC_EG_ENCAP_EEP_TYPE eedb_entry_type;
   _bcm_lif_type_e out_lif_usage;
   /*function calls*/
   DB_size=SOC_DPP_DEFS_GET(unit,nof_out_lifs);
   for (eedb_index=0;eedb_index<DB_size;eedb_index++) {
   
        rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get ( unit, 0, eedb_index, NULL, &out_lif_usage); /*get SW eedb entry type*/
      if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
      { 
          error_count++;
          continue ;     
      }
      eedb_entry_type = SOC_PPC_EG_ENCAP_EEP_TYPE_DATA;            /*default value (this is used only to tell*/
                                                                   /*PWE and MPLS, unless the type is PWE according to the entry type, we'll assume mpls*/
      if (out_lif_usage == _bcmDppLifTypeMplsPort ) 
      {
          eedb_entry_type = SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP;
      }
      if (out_lif_usage != _bcmDppLifTypeVlan && out_lif_usage != _bcmDppLifTypeAny) {                    /*this data encapsulation is not supported and will incite errors*/                                                                    /*and cannot be displayed, hence, to avoid repetetive error printing on such entries, we skip them*/
          rv =soc_ppd_eg_encap_entry_get(unit,                      /*actually retrieve the value*/
                          eedb_entry_type, eedb_index, 0,
                          encap_entry_info, next_eep, &nof_entries);
      
           if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) 
           {
                   error_count++;
                   continue ;  
           }
               
           if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP)/*we only want to add entries of this type to the table*/
           {
                if (counter==0) 
                {

                    cli_out("_________________________________________________________________________________________________________________________________________\n\r");
                    cli_out("EEDB index   |Out Vid      |PCP Dei      |tpid index   |LL Remark Profile  |Out AC LSB  |OAM LIF Set  |Dest MAC Address |Outlif Profile  |\n\r");
                }
           
                cli_out("  %-11d|", eedb_index);
                if(encap_entry_info[0].entry_val.ll_info.out_vid_valid !=  0) /*to avoid printing the valid bits seprately*/
                    cli_out("  %-11u|", encap_entry_info[0].entry_val.ll_info.out_vid); 
                else 
                    cli_out("    Invalid  |");
                if (encap_entry_info[0].entry_val.ll_info.pcp_dei_valid != 0)
                    cli_out("  %-11u|", encap_entry_info[0].entry_val.ll_info.pcp_dei);
                else 
                    cli_out("   Invalid   |");
                cli_out("  %-11u|", encap_entry_info[0].entry_val.ll_info.tpid_index);
                cli_out("  %-17u|", encap_entry_info[0].entry_val.ll_info.ll_remark_profile);
                if(encap_entry_info[0].entry_val.ll_info.out_ac_valid != 0)
                    cli_out("  %-10u|", encap_entry_info[0].entry_val.ll_info.out_ac_lsb);
                else 
                    cli_out("  Invalid   |");   
                cli_out("  %-11u|", encap_entry_info[0].entry_val.ll_info.oam_lif_set);
                soc_sand_SAND_PP_MAC_ADDRESS_print(&(encap_entry_info[0].entry_val.ll_info.dest_mac)); /*try this*/
                cli_out("|    0x%02x        |\n\r", encap_entry_info[0].entry_val.ll_info.outlif_profile);
                counter += 1;/*keep track of number of active LL lifs*/ 
           }
      }
   }
    if (counter != 0) { /*give output as to how many lifs are defined*/
        cli_out("the number of Link Layer entries in the EEDB is %d \n\r" , counter);
    }
    else{ cli_out( "there are no Link Layer entries in the EEDB\r\n" ) ; }
    cli_out("there have been %d entries that could not be obtained due to errors\r\n" ,error_count);
    return CMD_OK;
}


/******************************************************************** 
 *  Function handler: parsing_info_get (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_parsing_info_get(int unit, args_t* a)  
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_PARSING_INFO   
    prm_pars_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core = -1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;
  
  soc_sand_dev_id = (unit); 
  SOC_PPC_DIAG_PARSING_INFO_clear(&prm_pars_info);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  VERIFY_SIGNAL_DB(unit);

  if ((prm_core < -1 ) 
      || 
    (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 

  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_parsing_info_get(
              soc_sand_dev_id,
              core,
              &prm_pars_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
          return CMD_FAIL;     
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 
      } 

      cli_out("%s\r\n", get_core_str(unit, core));
      SOC_PPC_DIAG_PARSING_INFO_print(unit, &prm_pars_info);
  }
  return CMD_OK; 
} 

cmd_result_t 
  cmd_ppd_api_diag_db_lif_lkup_info_specific_get(int unit, int core_id, SOC_PPC_DIAG_DB_USE_INFO* prm_db_info)  
{      
    SOC_PPC_DIAG_LIF_LKUP_INFO
      prm_lkup_info;
    uint32
      ret;
    uint32
      soc_sand_dev_id;
    int
      lif_profile;
    int 
        global_lif;
    bcm_gport_t 
        gport;
    SOC_PPC_DIAG_RESULT                     ret_val;
    bcm_error_t rv;


    SOC_PPC_DIAG_LIF_LKUP_INFO_clear(&prm_lkup_info);
    soc_sand_dev_id = (unit);
    

    /* Call function */
    ret = soc_ppd_diag_db_lif_lkup_info_get(
            soc_sand_dev_id,
            core_id,
            prm_db_info,
            &prm_lkup_info,
            &ret_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        cli_out("Error: soc_ppd_diag_db_lif_lkup_info_get() Failed:\n");
        return CMD_FAIL; 
    } 

    if(prm_lkup_info.found)
    {
        SOC_PPC_DIAG_DB_USE_INFO_print(prm_db_info);
        if(prm_db_info->bank_id == 3)
            cli_out("Port %u\r\n", prm_lkup_info.key.ac.raw_key);
        SOC_PPC_DIAG_LIF_LKUP_INFO_print(&prm_lkup_info);

        rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, prm_lkup_info.base_index, &global_lif);
        if(rv != BCM_E_NONE) {
            cli_out("Failed in the function _bcm_dpp_global_lif_mapping_local_to_global_get\n");
            return CMD_FAIL;
        }

        rv = _bcm_dpp_lif_to_gport(unit, global_lif, _BCM_DPP_LIF_TO_GPORT_INGRESS | _BCM_DPP_LIF_TO_GPORT_GLOBAL_LIF, &gport);
        if(rv != BCM_E_NONE) {
            cli_out("Failed in the function _bcm_dpp_lif_to_gport\n");
            return CMD_FAIL;
        }


        cli_out("global_in_lif_id: %i", global_lif);
        cli_out("\n\rgport_id: %u\n\r", gport);
    }

    if (prm_lkup_info.found) {
        /* Check if profile assignment printing is necessary */
        switch (prm_lkup_info.type) {
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_AC:
            lif_profile = prm_lkup_info.value.ac.lif_profile;
            break;
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE:
            lif_profile = prm_lkup_info.value.pwe.lif_profile;
            break;
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
            lif_profile = prm_lkup_info.value.mpls.lif_profile;
            break;
        case SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
            lif_profile = prm_lkup_info.value.ip.lif_profile;
            break;
        default: /* No profile usage */
            lif_profile = -1;
            break;
        }
        if (lif_profile > -1) {
            ret = SOC_PPC_OCC_PROFILE_USAGE_print(unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_SIZE);
            if (soc_sand_get_error_code_from_error_word(ret) != CMD_OK) 
            { 
                return CMD_FAIL; 
            } 
        }
    }

    cli_out("\n");

    return CMD_OK;
}
/******************************************************************** 
 *  Function handler: db_lif_lkup_info_get (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_db_lif_lkup_info_get(int unit, args_t* a)  
{   
  SOC_PPC_DIAG_DB_USE_INFO
    prm_db_info;
  
  parse_table_t    pt;
  uint32              default_val = 4;
  uint32              bank_id=4, bank_id_tmp;
  uint32              lkup_num=4, lkup_num_tmp;
  int                 prm_core=0; 

  SOC_PPC_DIAG_DB_USE_INFO_clear(&prm_db_info);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "bank_id", PQ_DFL|PQ_INT, &default_val,  &bank_id, NULL);
  parse_table_add(&pt, "lkup_num", PQ_DFL|PQ_INT, &default_val, &lkup_num, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);
  
  if ((prm_core < 0 ) 
      || 
    (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 
  
  if (lkup_num == default_val) {
      for (lkup_num_tmp = 0; lkup_num_tmp < 2; lkup_num_tmp++) {
          prm_db_info.lkup_num = lkup_num_tmp;           
          if (bank_id == default_val) {
              for (bank_id_tmp = 0; bank_id_tmp < ((lkup_num_tmp == 0) ? 4 : 3); bank_id_tmp++) {
                  prm_db_info.bank_id = bank_id_tmp;
                  if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
                  {
                      return CMD_FAIL;
                  }
              }
          } else {
              prm_db_info.bank_id = bank_id;
              if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
              {
                  return CMD_FAIL;
              }
          }
      }

      
  } else {
      prm_db_info.lkup_num = lkup_num;      
      if (bank_id == default_val) {
          for (bank_id_tmp = 0; bank_id_tmp < ((lkup_num == 0 ) ? 4 : 3); bank_id_tmp++) {
              prm_db_info.bank_id = bank_id_tmp;
              if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
              {
                  return CMD_FAIL;
              }
          }
      } else {
          prm_db_info.bank_id = bank_id;
          if (cmd_ppd_api_diag_db_lif_lkup_info_specific_get(unit, prm_core, &prm_db_info) != CMD_OK)
          {
              return CMD_FAIL;
          }
      }
  }

  
 
  return CMD_OK;
} 

/******************************************************************** 
 *  Function handler: termination_info_get (section diag)
 */
int dnx_diag_termi_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_TERM_INFO *termi_info_per_core);
cmd_result_t 
  cmd_ppd_api_diag_termination_info_get(int unit, args_t* a)   
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_TERM_INFO   
    prm_term_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=-1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;

  /*
   * For printing to xml file
   */
  char                          *prm_file = NULL;
#if !defined(NO_FILEIO)
  SOC_PPC_DIAG_TERM_INFO         termi_info_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)];
  int                            termi_info_per_core_len = 0;
#endif
  uint8                          b_print_to_xml = 0;

  SOC_PPC_DIAG_TERM_INFO_clear(&prm_term_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  if ((prm_core < -1 ) 
      || 
    (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
       cli_out("error ilegal core ID for device\n");
       parse_arg_eq_done(&pt);
       return CMD_FAIL;
  } 
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }
  if (prm_core == -1 || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }


  for (core=first_core; core < last_core; core++) {

        /* Call function */
        ret = soc_ppd_diag_termination_info_get(
            soc_sand_dev_id,
            core,
            &prm_term_info,
            &ret_val
        );
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
            parse_arg_eq_done(&pt);
            return CMD_FAIL;  
        } 
        if (ret_val != SOC_PPC_DIAG_OK) { 
            LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),core));
            continue; 
        }
        if (!b_print_to_xml) {
            cli_out("%s\r\n", get_core_str(unit, core));

            SOC_PPC_DIAG_TERM_INFO_print(&prm_term_info);
            cli_out("\n");
        }
        else {
#if !defined(NO_FILEIO)
            termi_info_per_core[termi_info_per_core_len++] = prm_term_info;
#endif
        }
    }
        if (b_print_to_xml) {
          /*
           * Print to XML file
           */
#if !defined(NO_FILEIO)
            dnx_diag_termi_info_xml_print(unit, prm_file, termi_info_per_core);
#endif
        }
  parse_arg_eq_done(&pt);
  return CMD_OK; 
} 



/*
 * Print last packet(s) termination information (per core) to an xml file
 */
#if !defined(NO_FILEIO)
int dnx_diag_termi_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_TERM_INFO *termi_info_per_core){

    int core = 0;
    char *term_type_s;
    char term_type_ss[24] = "";
    char *frwrd_type_s;
    xml_node curTop, curDiags, curDiag, curCores, curCore;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
         * Add xml structure:
         * <diag cat="pp" name="termi_decision">
                <cores list="Yes">
                    <core id="0" term_type="eth_none" frwrd_type="bridge"/>
                    <core id="1" term_type="eth_none" frwrd_type=" Unknown"/>
                </cores>
         * </diag>
         */

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "TERMination_Info"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                if (termi_info_per_core[core].is_scnd_my_mac) {
                    sal_sprintf(term_type_ss,"eth_%s",SOC_PPC_PKT_TERM_TYPE_to_string(termi_info_per_core[core].term_type)); 
                    SHR_IF_ERR_EXIT(dbx_xml_property_mod_str(curCore, "term_type", term_type_ss));
                } else { 
                    term_type_s = (char *)SOC_PPC_PKT_TERM_TYPE_to_string(termi_info_per_core[core].term_type);  
                    SHR_IF_ERR_EXIT(dbx_xml_property_mod_str(curCore, "term_type", term_type_s));
                }
                

                frwrd_type_s = (char *)SOC_PPC_PKT_FRWRD_TYPE_to_string(termi_info_per_core[core].frwrd_type);
                SHR_IF_ERR_EXIT(dbx_xml_property_mod_str(curCore, "frwrd_type", frwrd_type_s));     
            }
            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);

    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

exit:
    SHR_FUNC_EXIT;
}
#endif

/******************************************************************** 
 *  Function handler: traps_info_get (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_traps_info_get(int unit, args_t* a)    
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_TRAPS_INFO   
    prm_traps_info;
  uint32
    soc_sand_dev_id;
  int prm_core =  -1;
  parse_table_t    pt;
  int              core, first_core, last_core;
  SOC_PPC_DIAG_RESULT                     ret_val;
 
  SOC_PPC_DIAG_TRAPS_INFO_clear(&prm_traps_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  } 
  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_traps_info_get(
              soc_sand_dev_id,
              core,
              &prm_traps_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
          return CMD_FAIL; 
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 
      } 

      cli_out("%s\n", get_core_str(unit, core));
      SOC_PPC_DIAG_TRAPS_INFO_print(&prm_traps_info);
  }
  return CMD_OK; 
} 
  

/******************************************************************** 
 *  Function handler: encap_info_get (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_encap_info_get(int unit, args_t* a)  
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_ENCAP_INFO   
    prm_encap_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=-1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;

  SOC_PPC_DIAG_ENCAP_INFO_clear(&prm_encap_info);
  soc_sand_dev_id = (unit);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      return CMD_FAIL;
  }


  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

  /* Call function */
      ret = soc_ppd_diag_encap_info_get(
              soc_sand_dev_id,
              core,
              &prm_encap_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
            return CMD_FAIL; 
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 
      } 
      cli_out("%s\n", get_core_str(unit, core));
      SOC_PPC_DIAG_ENCAP_INFO_print(unit, &prm_encap_info);

      if (SOC_IS_JERICHO(unit)) {
          ret = SOC_PPC_OCC_PROFILE_USAGE_print(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_SIZE(unit));
          if (soc_sand_get_error_code_from_error_word(ret) != CMD_OK) 
          { 
              return CMD_FAIL; 
          }
      }
  }
  return CMD_OK; 
} 



/******************************************************************** 
 *  Function handler: frwrd_lkup_info_get (section diag)
 */
int dnx_diag_frwrd_lkup_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_FRWRD_LKUP_INFO *frwrd_info_per_core);
cmd_result_t 
  cmd_ppd_api_diag_frwrd_lkup_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_FRWRD_LKUP_INFO   
    prm_frwrd_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=-1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;
   /*
   * For printing to xml file
   */
  char                              *prm_file = NULL;
#if !defined(NO_FILEIO)
  SOC_PPC_DIAG_FRWRD_LKUP_INFO      frwrd_info_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)];
  int                               frwrd_info_per_core_len = 0;
#endif
  uint8                             b_print_to_xml = 0;

  soc_sand_dev_id = (unit); 

  SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(&prm_frwrd_info);
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
       cli_out("error ilegal core ID for device\n");
       parse_arg_eq_done(&pt);
       return CMD_FAIL;
  } 
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }
  if (prm_core == -1 || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_frwrd_lkup_info_get(
              soc_sand_dev_id,
              core,
              &prm_frwrd_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
            parse_arg_eq_done(&pt);
            return CMD_FAIL; 
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 
      } 
      if (!b_print_to_xml) {
          cli_out("%s\n", get_core_str(unit, core));
          SOC_PPC_DIAG_FRWRD_LKUP_INFO_print(&prm_frwrd_info);
      } else {
#if !defined(NO_FILEIO)
          frwrd_info_per_core[frwrd_info_per_core_len++] = prm_frwrd_info;
#endif
      }

      if (b_print_to_xml) {
          /*
          * Print to XML file
          */
#if !defined(NO_FILEIO)
          dnx_diag_frwrd_lkup_info_info_xml_print(unit, prm_file, frwrd_info_per_core);
#endif
      }
  }

  parse_arg_eq_done(&pt);
  return CMD_OK;  
} 

#if !defined(NO_FILEIO)
/*
 * {
 */
/*
 * Print last packet(s) termination information (per core) to an xml file
 */
int frwrding_decision_info_to_xml(int unit, SOC_PPC_FRWRD_DECISION_INFO *decision_info, xml_node cur_node);
int frwrd_lkup_info_dump(int unit, SOC_PPC_DIAG_FRWRD_LKUP_KEY *lkup_key_info, xml_node lkup_key, SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type, uint8 is_kbp);
int dnx_diag_frwrd_lkup_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_FRWRD_LKUP_INFO *frwrd_info_per_core){

    int core = 0;
    xml_node curTop, curDiags, curDiag, curCores, curCore, lkup_key, lkup_res, slb_info, frwrd_decision, aging_info, frwrding_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
        * Add xml structure:
        * <diag cat="pp" name="Frwrd_Lkup_Info">
        *         <cores list="Yes">
        *                 <core id="0" frwrd_type="ilm" frwrd_hdr_index="3" key_found="1">
        *                         <lkup_key name="ilm" in_label="5000" in_local_port="0" mapped_exp="0" inrif="0" vrf="0"/>
        *                         <lkup_res name="frwrd_decision">
        *                                 <frwrd_decision type="fec" dest_id="4096">
        *         <eei type="mpls" mpls_command="swap" mpls_label="8000" mpls_push_profile="0"/>
        *         `</frwrd_decision>
        *     </lkup_res>
        *                 </core>
        *                 <core id="1" frwrd_type="none" frwrd_hdr_index="0" key_found="0">
        *                         <lkup_key raw_0="0x0" raw_1="0x0"/>
        *                 </core>
        *         </cores>
        * </diag> 
        */ 

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "Frwrd_Lkup_Info"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "frwrd_type", (char *)SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(frwrd_info_per_core[core].frwrd_type)));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "frwrd_hdr_index", frwrd_info_per_core[core].frwrd_hdr_index));
                {
                    SHR_NULL_CHECK((lkup_key = dbx_xml_child_add(curCore, "lkup_key", 5)), _SHR_E_FAIL, "lkup_key");
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
                      if(frwrd_info_per_core[core].is_kbp) {
                          SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "look_up_device", "ELK"));
                      }
                      SHR_IF_ERR_EXIT(frwrd_lkup_info_dump(unit, &frwrd_info_per_core[core].lkup_key, lkup_key, frwrd_info_per_core[core].frwrd_type, frwrd_info_per_core[core].is_kbp));
#else
                      SHR_IF_ERR_EXIT(frwrd_lkup_info_dump(unit, &frwrd_info_per_core[core].lkup_key, lkup_key, frwrd_info_per_core[core].frwrd_type, FALSE));
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
                      if(frwrd_info_per_core[core].is_kbp) {
                          /* Corresponds to "Result 1" from the diag output*/
                        xml_node  ip_reply_record, second_ip_reply_record;
                        SHR_NULL_CHECK((ip_reply_record = dbx_xml_child_add(curCore, "ip_reply_record", 5)), _SHR_E_FAIL, "ip_reply_record");
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(ip_reply_record, "key_found", (frwrd_info_per_core[core].ip_reply_record.match_status ? "Yes" : "No")));
                        if (frwrd_info_per_core[core].ip_reply_record.match_status) {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(ip_reply_record, "dynamic", (frwrd_info_per_core[core].ip_reply_record.is_synamic ? "Yes" : "No")));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(ip_reply_record, "p2p_service", (frwrd_info_per_core[core].ip_reply_record.p2p_service ? "Yes" : "No")));
                            if (frwrd_info_per_core[core].ip_reply_record.identifier == 0x0) {
                                SHR_IF_ERR_EXIT(dbx_xml_property_set_str(ip_reply_record, "outlif_valid", (frwrd_info_per_core[core].ip_reply_record.out_lif_valid ? "Yes" : "No")));
                                if (frwrd_info_per_core[core].ip_reply_record.out_lif_valid) {
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ip_reply_record, "outlif", (frwrd_info_per_core[core].ip_reply_record.out_lif)));    
                                }
                                SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &frwrd_info_per_core[core].ip_reply_record.destination, ip_reply_record));
                            }
                            if (frwrd_info_per_core[core].ip_reply_record.identifier == 0x1) {
                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ip_reply_record, "eei", (frwrd_info_per_core[core].ip_reply_record.eei)));
                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ip_reply_record, "fec_ptr", (frwrd_info_per_core[core].ip_reply_record.fec_ptr)));
                            }
                        }
                        /* Corresponds to "Result 2" from the diag output*/
                        SHR_NULL_CHECK((second_ip_reply_record = dbx_xml_child_add(curCore, "second_ip_reply_record", 5)), _SHR_E_FAIL, "second_ip_reply_record");
                        if (frwrd_info_per_core[core].second_ip_reply_result.match_status) {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(second_ip_reply_record, "key_found",  "Yes"));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(second_ip_reply_record, "type",  "fec"));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(second_ip_reply_record, "dest_id", frwrd_info_per_core[core].second_ip_reply_result.dest_id ));
                        } else {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(second_ip_reply_record, "key_found",  "No"));
                        }
                      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
                      {

                        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "key_found", frwrd_info_per_core[core].key_found));
                        if (frwrd_info_per_core[core].key_found)
                        {
                            SHR_NULL_CHECK((lkup_res = dbx_xml_child_add(curCore, "lkup_res", 5)), _SHR_E_FAIL, "lkup_res");
                            /* This switch case correlates to SOC_PPC_DIAG_FRWRD_LKUP_VALUE_print*/
                            switch(frwrd_info_per_core[core].frwrd_type)
                            {
                            case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_res, "name", "mact"));
                                    SHR_NULL_CHECK((frwrding_info = dbx_xml_child_add(lkup_res, "frwrding_info", 6)), _SHR_E_FAIL, "frwrding_info");
                                    SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &frwrd_info_per_core[core].lkup_res.mact.frwrd_info.forward_decision, frwrding_info));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "drop_when_sa_is_known", frwrd_info_per_core[core].lkup_res.mact.frwrd_info.drop_when_sa_is_known));
                                    SHR_NULL_CHECK((aging_info = dbx_xml_child_add(lkup_res, "aging_info", 6)), _SHR_E_FAIL, "aging_info");
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(aging_info, "aging_info_is_dynamic", frwrd_info_per_core[core].lkup_res.mact.aging_info.is_dynamic));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(aging_info, "age_status", frwrd_info_per_core[core].lkup_res.mact.aging_info.age_status));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "accessed", frwrd_info_per_core[core].lkup_res.mact.accessed));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "group", frwrd_info_per_core[core].lkup_res.mact.group));
#ifdef BCM_88660_A0
                                    SHR_NULL_CHECK((slb_info = dbx_xml_child_add(lkup_res, "slb_info", 6)), _SHR_E_FAIL, "slb_info");
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_fec", frwrd_info_per_core[core].lkup_res.mact.slb_info.fec));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_lag_group_and_member", frwrd_info_per_core[core].lkup_res.mact.slb_info.lag_group_and_member));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_match_fec_entries", frwrd_info_per_core[core].lkup_res.mact.slb_info.match_fec_entries));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_match_lag_entries", frwrd_info_per_core[core].lkup_res.mact.slb_info.match_lag_entries));
#endif /* BCM_88660_A0 */
                                break;
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_res, "name", "bmact"));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "drop_sa", frwrd_info_per_core[core].lkup_res.bmact.drop_sa));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "i_sid_domain", frwrd_info_per_core[core].lkup_res.bmact.i_sid_domain));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "mc_id", frwrd_info_per_core[core].lkup_res.bmact.mc_id));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "sys_port_id", frwrd_info_per_core[core].lkup_res.bmact.sys_port_id));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "sa_learn_fec_id", frwrd_info_per_core[core].lkup_res.bmact.sa_learn_fec_id));
                                break;
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_res, "name", "frwrd_decision"));
                                    SHR_NULL_CHECK((frwrd_decision = dbx_xml_child_add(lkup_res, "frwrd_decision", 6)), _SHR_E_FAIL, "frwrd_decision");
                                    SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &frwrd_info_per_core[core].lkup_res.frwrd_decision, frwrd_decision));
                                break;
                                case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_res, "name", "host_lookup"));
                                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "fec_id", frwrd_info_per_core[core].lkup_res.host_info.fec_id));
                                    SHR_NULL_CHECK((frwrd_decision = dbx_xml_child_add(lkup_res, "frwrd_decision", 6)), _SHR_E_FAIL, "frwrd_decision");
                                    SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &frwrd_info_per_core[core].lkup_res.host_info.frwrd_decision, frwrd_decision));
                                    if (frwrd_info_per_core[core].lkup_res.host_info.eep != SOC_PPC_EEP_NULL) {
                                        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_res, "eep", frwrd_info_per_core[core].lkup_res.host_info.eep));
                                    } else {
                                        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_res, "eep", "NULL"));
                                    }
                                break;
                                default:
                                  SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(lkup_res, "raw_0", frwrd_info_per_core[core].lkup_res.raw[0]));
                                  SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(lkup_res, "raw_1", frwrd_info_per_core[core].lkup_res.raw[1]));
                                break;
                            }
                        }
                      }
                      dbx_xml_node_end(curCore, 4);
                }

            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);
    }
    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

 
 exit:
    SHR_FUNC_EXIT;
}
/* Function corresponds to SOC_PPC_FRWRD_DECISION_TYPE_INFO_print, but instead prints to XML file, more accurately under XML node passed as param */
int frwrding_decision_info_to_xml(int unit, SOC_PPC_FRWRD_DECISION_INFO *info, xml_node cur_node) {

    xml_node eei, outlif, trap;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "type", (char *)SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type)));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "dest_id", info->dest_id));

    switch (info->type) {
    case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
    case SOC_PPC_FRWRD_DECISION_TYPE_MC: 
        SHR_NULL_CHECK((eei = dbx_xml_child_add(cur_node, "eei", 5)), _SHR_E_FAIL, "eei");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(eei, "type", (char *)SOC_PPC_EEI_TYPE_to_string(info->additional_info.eei.type)));
        switch(info->additional_info.eei.type)
        {
        case SOC_PPC_EEI_TYPE_MPLS:
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(eei, "mpls_command", (char *)SOC_PPC_MPLS_COMMAND_TYPE_to_string(info->additional_info.eei.val.mpls_command.command)));
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(eei, "mpls_label", info->additional_info.eei.val.mpls_command.label));
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(eei, "mpls_push_profile", info->additional_info.eei.val.mpls_command.push_profile));
        break;
        case SOC_PPC_EEI_TYPE_TRILL:
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(eei, "trill_nick", info->additional_info.eei.val.trill_dest.dest_nick));
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(eei, "trill_is_mc", info->additional_info.eei.val.trill_dest.is_multicast));
        break;
        case SOC_PPC_EEI_TYPE_MIM:
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(eei, "mim_isid", info->additional_info.eei.val.isid));
        break;
        default:
        break;
        }
    break;
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
        SHR_NULL_CHECK((outlif = dbx_xml_child_add(cur_node, "outlif", 5)), _SHR_E_FAIL, "outlif");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(outlif, "type", (char *)SOC_PPC_OUTLIF_ENCODE_TYPE_to_string(info->additional_info.outlif.type)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(outlif, "val", (info->additional_info.outlif.val)));      
    break;
    case SOC_PPC_FRWRD_DECISION_TYPE_TRAP: 
        SHR_NULL_CHECK((trap = dbx_xml_child_add(cur_node, "trap", 5)), _SHR_E_FAIL, "trap");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(trap, "frwrd_action_strength", info->additional_info.trap_info.action_profile.frwrd_action_strength));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(trap, "snoop_action_strength", info->additional_info.trap_info.action_profile.snoop_action_strength));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(trap, "trap_code", (char *)SOC_PPC_TRAP_CODE_to_string(info->additional_info.trap_info.action_profile.trap_code)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(trap, "bcm_api", (char *)SOC_PPC_TRAP_CODE_to_api_string(info->additional_info.trap_info.action_profile.trap_code)));
    break;
    default:
        break;
    }
exit:
    SHR_FUNC_EXIT;
}
/* Function prints SOC_SAND_PP_IPV6_ADDRESS to XML node given as param , with attribute name as passed as param aswell*/
int ipv6_print_to_xml(int unit, xml_node cur_node, char *attribute_name, SOC_SAND_PP_IPV6_ADDRESS *ipv6_addr){

    uint32 indx;
    char ipv6_addr_string[128] = "" ;
    char ipv6_addr_string_nibble[8] = "";
    SHR_FUNC_INIT_VARS(unit);

    for (indx = SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S; indx > 0; --indx)
    {
        sal_sprintf(ipv6_addr_string_nibble,"%08x", (uint32)(ipv6_addr->address[indx-1]));
        sal_strcat(ipv6_addr_string, ipv6_addr_string_nibble);           

        if (indx > 1)
        {
          sal_strcat(ipv6_addr_string, ":");
        }
    }
    dbx_xml_property_mod_str(cur_node, attribute_name, ipv6_addr_string);
     
    SHR_FUNC_EXIT;

}
/* Function prints SOC_SAND_PP_MAC_ADDRESS to XML node given as param , with attribute name as passed as param aswell*/
int mac_print_to_xml(int unit, xml_node cur_node, char *attribute_name, SOC_SAND_PP_MAC_ADDRESS *mac_addr){

    uint32 indx;
    char mac_addr_string[24] = "";
    char mac_addr_string_nibble[2] = "";
    SHR_FUNC_INIT_VARS(unit);

    for (indx = SOC_SAND_PP_MAC_ADDRESS_NOF_U8; indx > 0; --indx)
    {
        
        sal_sprintf(mac_addr_string_nibble,"%02x", (mac_addr->address[indx-1]));
        sal_strcat(mac_addr_string, mac_addr_string_nibble);

        if (indx > 1)
        {
            sal_strcat(mac_addr_string, ":");
        }
    }

    dbx_xml_property_mod_str(cur_node, attribute_name, mac_addr_string);
     
    SHR_FUNC_EXIT;


}

/* Function corresponds to SOC_PPC_DIAG_FRWRD_LKUP_KEY_print , which prints to the "lkup_key" node */
int frwrd_lkup_info_dump(int unit, SOC_PPC_DIAG_FRWRD_LKUP_KEY *lkup_key_info, xml_node lkup_key, SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type, uint8 is_kbp) {

    char decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
    xml_node subnet, group, source, key;
    SHR_FUNC_INIT_VARS(unit);


  switch(frwrd_type)
  {
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "mact"));
      switch(lkup_key_info->mact.key_type) {
      case  SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
         SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "ipv4_mc_dip", lkup_key_info->mact.key_val.ipv4_mc.dip));
         SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "ipv4_mc_fid", lkup_key_info->mact.key_val.ipv4_mc.fid));
         break;
      case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
      default:
          mac_print_to_xml(unit, lkup_key, "mac" ,&lkup_key_info->mact.key_val.mac.mac);
          SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "mac_fid" ,lkup_key_info->mact.key_val.mac.fid));
      }
     break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "bmact"));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "b_vid", lkup_key_info->bmact.b_vid));
      mac_print_to_xml(unit, lkup_key, "b_mac_addr", &lkup_key_info->bmact.b_mac_addr);
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(lkup_key, "flags", lkup_key_info->bmact.flags));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "local_port_ndx", lkup_key_info->bmact.local_port_ndx));
      break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "ipv4_uc"));
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        xml_node masterKey, search1, search2;
        SHR_NULL_CHECK((masterKey = dbx_xml_child_add(lkup_key, "masterKey", 6)), _SHR_E_FAIL, "masterKey");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "vrf", lkup_key_info->kbp_ipv4_unicast_rpf.vrf));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(masterKey, "sip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_unicast_rpf.sip,1,decimal_ip)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(masterKey, "dip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_unicast_rpf.dip,1,decimal_ip)));
        SHR_NULL_CHECK((search1 = dbx_xml_child_add(lkup_key, "search1", 6)), _SHR_E_FAIL, "search1");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search1, "vrf", lkup_key_info->kbp_ipv4_unicast_rpf.vrf));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(search1, "dip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_unicast_rpf.dip,1,decimal_ip)));
        SHR_NULL_CHECK((search2 = dbx_xml_child_add(lkup_key, "search2", 6)), _SHR_E_FAIL, "search2");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search2, "vrf", lkup_key_info->kbp_ipv4_unicast_rpf.vrf));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(search2, "sip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_unicast_rpf.sip,1,decimal_ip)));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      { 
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "vrf", lkup_key_info->ipv4_uc.vrf));
        SHR_NULL_CHECK((key = dbx_xml_child_add(lkup_key, "key", 6)), _SHR_E_FAIL, "key");
        SHR_NULL_CHECK((subnet = dbx_xml_child_add(key, "subnet", 7)), _SHR_E_FAIL, "subnet");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(subnet, "subnet_sip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->ipv4_uc.key.subnet.ip_address,1,decimal_ip)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(subnet, "prefix_len", lkup_key_info->ipv4_uc.key.subnet.prefix_len));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "ipv4_vpn"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "vrf", lkup_key_info->ipv4_uc.vrf));
        SHR_NULL_CHECK((key = dbx_xml_child_add(lkup_key, "key", 6)), _SHR_E_FAIL, "key");
        SHR_NULL_CHECK((subnet = dbx_xml_child_add(key, "subnet", 7)), _SHR_E_FAIL, "subnet");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(subnet, "sip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->ipv4_uc.key.subnet.ip_address,1,decimal_ip)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(subnet, "prefix_len", lkup_key_info->ipv4_uc.key.subnet.prefix_len));

    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "ipv4_mc"));
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        xml_node masterKey, search1, search2;
        SHR_NULL_CHECK((masterKey = dbx_xml_child_add(lkup_key, "masterKey", 6)), _SHR_E_FAIL, "masterKey");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "vrf", lkup_key_info->kbp_ipv4_multicast.vrf));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "in_rif", lkup_key_info->kbp_ipv4_multicast.in_rif));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(masterKey, "sip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_multicast.sip,1,decimal_ip)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(masterKey, "dip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_multicast.dip,1,decimal_ip)));
        SHR_NULL_CHECK((search1 = dbx_xml_child_add(lkup_key, "search1", 6)), _SHR_E_FAIL, "search1");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search1, "in_rif", lkup_key_info->kbp_ipv4_multicast.in_rif));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(search1, "dip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_multicast.dip,1,decimal_ip)));
        SHR_NULL_CHECK((search2 = dbx_xml_child_add(lkup_key, "search2", 6)), _SHR_E_FAIL, "search2");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search2, "vrf", lkup_key_info->kbp_ipv4_multicast.vrf));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(search2, "sip", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->kbp_ipv4_multicast.sip,1,decimal_ip)));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "group", lkup_key_info->ipv4_mc.group));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "inrif", lkup_key_info->ipv4_mc.inrif));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "inrif_valid", lkup_key_info->ipv4_mc.inrif_valid));
        SHR_NULL_CHECK((source = dbx_xml_child_add(lkup_key, "source", 6)), _SHR_E_FAIL, "source");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(source, "source_ip_address", (char *)soc_sand_pp_ip_long_to_string(lkup_key_info->ipv4_mc.source.ip_address,1,decimal_ip)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(source, "prefix_len", lkup_key_info->ipv4_mc.source.prefix_len));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "ipv6_uc"));
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        xml_node masterKey, search1, search2;
        SHR_NULL_CHECK((masterKey = dbx_xml_child_add(lkup_key, "masterKey", 6)), _SHR_E_FAIL, "masterKey");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "vrf", lkup_key_info->kbp_ipv6_unicast_rpf.vrf));
        ipv6_print_to_xml(unit, masterKey, "sip", &lkup_key_info->kbp_ipv6_unicast_rpf.sip);
        ipv6_print_to_xml(unit, masterKey, "dip", &lkup_key_info->kbp_ipv6_unicast_rpf.dip);
        SHR_NULL_CHECK((search1 = dbx_xml_child_add(lkup_key, "search1", 6)), _SHR_E_FAIL, "search1");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search1, "vrf", lkup_key_info->kbp_ipv6_unicast_rpf.vrf));
        ipv6_print_to_xml(unit, search1, "dip", &lkup_key_info->kbp_ipv6_unicast_rpf.dip);
        SHR_NULL_CHECK((search2 = dbx_xml_child_add(lkup_key, "search2", 6)), _SHR_E_FAIL, "search2");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search2, "vrf", lkup_key_info->kbp_ipv6_unicast_rpf.vrf));
        ipv6_print_to_xml(unit, search2, "sip", &lkup_key_info->kbp_ipv6_unicast_rpf.sip);

      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "vrf", lkup_key_info->ipv6_uc.vrf));
        SHR_NULL_CHECK((key = dbx_xml_child_add(lkup_key, "key", 6)), _SHR_E_FAIL, "key");
        SHR_NULL_CHECK((subnet = dbx_xml_child_add(key, "subnet", 7)), _SHR_E_FAIL, "subnet");
        ipv6_print_to_xml(unit, subnet, "ipv6_address", &lkup_key_info->ipv6_uc.key.subnet.ipv6_address);
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(subnet, "prefix_len", lkup_key_info->ipv6_uc.key.subnet.prefix_len));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "ipv6_mc"));

#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        xml_node masterKey, search1, search2;
        SHR_NULL_CHECK((masterKey = dbx_xml_child_add(lkup_key, "masterKey", 6)), _SHR_E_FAIL, "masterKey");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "vrf", lkup_key_info->kbp_ipv6_multicast.vrf));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "in_rif", lkup_key_info->kbp_ipv6_multicast.in_rif));
        ipv6_print_to_xml(unit, masterKey, "sip", &lkup_key_info->kbp_ipv6_multicast.sip);
        ipv6_print_to_xml(unit, masterKey, "dip", &lkup_key_info->kbp_ipv6_multicast.dip);
        SHR_NULL_CHECK((search1 = dbx_xml_child_add(lkup_key, "search1", 6)), _SHR_E_FAIL, "search1");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search1, "in_rif", lkup_key_info->kbp_ipv6_multicast.in_rif));
        ipv6_print_to_xml(unit, search1, "sip", &lkup_key_info->kbp_ipv6_multicast.sip);
        ipv6_print_to_xml(unit, search1, "dip", &lkup_key_info->kbp_ipv6_multicast.dip);
        SHR_NULL_CHECK((search2 = dbx_xml_child_add(lkup_key, "search2", 6)), _SHR_E_FAIL, "search2");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(search2, "vrf", lkup_key_info->kbp_ipv6_multicast.vrf));
        ipv6_print_to_xml(unit, search2, "sip", &lkup_key_info->kbp_ipv6_multicast.sip);

      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "vrf", lkup_key_info->ipv6_mc.vrf_ndx));
        SHR_NULL_CHECK((group = dbx_xml_child_add(lkup_key, "group", 6)), _SHR_E_FAIL, "group");
        ipv6_print_to_xml(unit, group, "ipv6_address", &lkup_key_info->ipv6_mc.group.ipv6_address);
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(group, "prefix_len", lkup_key_info->ipv6_mc.group.prefix_len));
        if (lkup_key_info->ipv6_mc.inrif.mask) {
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "inrif_val", lkup_key_info->ipv6_mc.inrif.val));
            SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(lkup_key, "inrif_mask", lkup_key_info->ipv6_mc.inrif.mask));
        }
        if (lkup_key_info->ipv6_mc.source.prefix_len) {
            SHR_NULL_CHECK((source = dbx_xml_child_add(lkup_key, "source", 6)), _SHR_E_FAIL, "source");
            ipv6_print_to_xml(unit, source, "source_ipv6_address", &lkup_key_info->ipv6_mc.source.ipv6_address);
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(source, "source_prefix_len", lkup_key_info->ipv6_mc.source.prefix_len));
        }
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "ilm"));
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        xml_node masterKey;
        SHR_NULL_CHECK((masterKey = dbx_xml_child_add(lkup_key, "masterKey", 6)), _SHR_E_FAIL, "masterKey");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "in_rif", lkup_key_info->kbp_mpls.in_rif));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "in_port", lkup_key_info->kbp_mpls.in_port));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "exp", lkup_key_info->kbp_mpls.exp));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "mpls_label", lkup_key_info->kbp_mpls.mpls_label));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "in_label", lkup_key_info->ilm.in_label));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "in_local_port", lkup_key_info->ilm.in_local_port));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "mapped_exp", lkup_key_info->ilm.mapped_exp));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "inrif", lkup_key_info->ilm.inrif));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "vrf", lkup_key_info->ilm.vrf));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "trill_uc"));
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
          xml_node masterKey;
        SHR_NULL_CHECK((masterKey = dbx_xml_child_add(lkup_key, "masterKey", 6)), _SHR_E_FAIL, "masterKey");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "egress_nick", lkup_key_info->kbp_trill_unicast.egress_nick));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "trill_uc", lkup_key_info->trill_uc));
      }
    break;
    case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "trill_mc"));
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
      if(is_kbp) {
        xml_node masterKey;
        SHR_NULL_CHECK((masterKey = dbx_xml_child_add(lkup_key, "masterKey", 6)), _SHR_E_FAIL, "masterKey");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "fid_vsi", lkup_key_info->kbp_trill_multicast.fid_vsi));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "dist_tree_nick", lkup_key_info->kbp_trill_multicast.dist_tree_nick));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(masterKey, "esdai", lkup_key_info->kbp_trill_multicast.esdai));
      } else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "adjacent_eep", lkup_key_info->trill_mc.adjacent_eep));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "esadi", lkup_key_info->trill_mc.esadi));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "fid", lkup_key_info->trill_mc.fid));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "ing_nick", lkup_key_info->trill_mc.ing_nick));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "tree_nick", lkup_key_info->trill_mc.tree_nick));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "tts", lkup_key_info->trill_mc.tts));
      }
    break;
  case SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF:
    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(lkup_key, "name", "fcf"));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "d_id", lkup_key_info->fcf.d_id));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "vfi", lkup_key_info->fcf.vfi));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(lkup_key, "prefix_len", lkup_key_info->fcf.prefix_len));
  break;
    default:
    SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(lkup_key, "raw_0", lkup_key_info->raw[0]));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(lkup_key, "raw_1", lkup_key_info->raw[1]));
    break;
  }
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
#endif /* NO_FILEIO */

/******************************************************************** 
 *  Function handler: frwrd_lpm_lkup_get (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_frwrd_lpm_lkup_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_IPV4_VPN_KEY   
    prm_lpm_key;
  uint32   
    prm_fec_ptr;
  uint8   
    prm_found;
  uint32
    soc_sand_dev_id;
  ip_addr_t   ip_addr;
  parse_table_t    pt;
  int              prm_core=-1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;
   
  soc_sand_dev_id = (unit); 
  SOC_PPC_DIAG_IPV4_VPN_KEY_clear(&prm_lpm_key);
 
  /* Get parameters */ 

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
  parse_table_add(&pt, "prefix_len", PQ_DFL|PQ_INT, 0, &prm_lpm_key.key.subnet.prefix_len, NULL);
  parse_table_add(&pt, "vrf", PQ_DFL|PQ_INT, 0, &prm_lpm_key.vrf, NULL);
  parse_table_add(&pt, "ip_address", PQ_DFL|PQ_IP, 0, &ip_addr, NULL);

  prm_lpm_key.vrf = (uint32)-1;

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

   if ((prm_core < -1 ) ||
       (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      return CMD_FAIL;
  }

  if (prm_lpm_key.vrf == (uint32)-1) {
      cli_out(" *** SW error - expecting parameter lpm_key after frwrd_lpm_lkup_get***\n\r");
      return CMD_FAIL;
  }
  prm_lpm_key.key.subnet.ip_address = ip_addr;
  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_frwrd_lpm_lkup_get(
              soc_sand_dev_id,
              core,
              &prm_lpm_key,
              &prm_fec_ptr,
              &prm_found,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
            return CMD_FAIL;
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 
      } 

      cli_out("fec_ptr: %u\n\r",prm_fec_ptr);

      cli_out("found: %u\n\r",prm_found);
  }
  return CMD_OK;
}



/******************************************************************** 
 *  Function handler: ing_vlan_edit_info_get (section diag)
 */
cmd_result_t
  cmd_ppd_api_diag_ing_vlan_edit_info_get(int unit, args_t* a)
{
  uint32
    ret;
  SOC_PPC_DIAG_VLAN_EDIT_RES
    prm_vec_res;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=-1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;

  soc_sand_dev_id = (unit);
  SOC_PPC_DIAG_VLAN_EDIT_RES_clear(&prm_vec_res);

  /* Get parameters */
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  if ((prm_core < -1 ) ||
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
       cli_out("error ilegal core ID for device\n");
       return CMD_FAIL;
  }

  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_ing_vlan_edit_info_get(
              soc_sand_dev_id,
              core,
              &prm_vec_res,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
            return CMD_FAIL;
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 
      }

      cli_out("%s\n", get_core_str(unit, core));
      SOC_PPC_DIAG_VLAN_EDIT_RES_print(&prm_vec_res);
  }
  return CMD_OK;
}



/******************************************************************** 
 *  Function handler: learning_info_get (section diag)
 */
int dnx_diag_lrn_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_LEARN_INFO *lrn_info_per_core);
cmd_result_t
  cmd_ppd_api_diag_learning_info_get(int unit, args_t* a)
{   
  uint32 
    ret;
  SOC_PPC_DIAG_LEARN_INFO
    prm_learn_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;

  int prm_core=-1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;
  /*
   * For printing to xml file
   */
  char                              *prm_file = NULL;
#if !defined(NO_FILEIO)
  SOC_PPC_DIAG_LEARN_INFO           lrn_info_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)];
#endif
  uint8                             b_print_to_xml = 0;

  soc_sand_dev_id = (unit);
  SOC_PPC_DIAG_LEARN_INFO_clear(&prm_learn_info);
 
 /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
  }
  
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }
  if (prm_core == -1 || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

          /* Call function */
          ret = soc_ppd_diag_learning_info_get(
                  soc_sand_dev_id,
                  core,
                  &prm_learn_info,
                  &ret_val
                );
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
          { 
                parse_arg_eq_done(&pt);
                return CMD_FAIL; 
          } 
          if (ret_val != SOC_PPC_DIAG_OK) 
          { 
              LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
              continue; 
          } 

          if (!b_print_to_xml) {
              cli_out("%s\n", get_core_str(unit, core));
              SOC_PPC_DIAG_LEARN_INFO_print(&prm_learn_info);
          } else {
#if !defined(NO_FILEIO)
              lrn_info_per_core[core] = prm_learn_info; 
#endif
          }
  }

  if (b_print_to_xml) {
#if !defined(NO_FILEIO)
       dnx_diag_lrn_info_info_xml_print(unit, prm_file, lrn_info_per_core);
#endif
  }

  parse_arg_eq_done(&pt);
  return CMD_OK;  
} 

#if !defined(NO_FILEIO)
int dnx_diag_lrn_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_LEARN_INFO *lrn_info_per_core){

    int core = 0;
    xml_node curTop, curDiags, curDiag, curCores, curCore, key, value, aging_info, frwrd_info, frwr_decision, slb_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
        * Add xml structure:         
        * <diag cat="pp" name="LeaRNing_Info">
	*		<cores list="Yes">
	*			<core id="0" valid="No Learning info for packet"/>
	*			<core id="1" ingress="1" is_new_key="1">
	*				<key mac="07:08:09:0a:0b:0c" fid="6202"/>
	*				<value accessed="0" group="0">
	*					<aging_info is_dynamic="0" age_status="0"/>
	*					<frwrd_info drop_when_sa_is_known="0">
	*						<frwr_decision type="trap" dest_id="0">
	*				                    <trap frwrd_action_strength="0" snoop_action_strength="0" trap_code="pbp_sa_drop_0" bcm_api="bcmRxTrapMimDiscardMacsaFwd"/>
	*						</frwr_decision>
	*					</frwrd_info>
	*					<slb_info slb_info_fec="0" slb_info_lag_group_and_member="0" slb_info_match_fec_entries="0" slb_info_match_lag_entries="0"/>
	*				</value>
	*			</core>
	*		</cores>
	*	</diag> 
        */ 

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "LeaRNing_Info"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                if (lrn_info_per_core[core].valid) {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "ingress", lrn_info_per_core[core].ingress));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "is_new_key", lrn_info_per_core[core].is_new_key));
                    SHR_NULL_CHECK((key = dbx_xml_child_add(curCore, "key", 5)), _SHR_E_FAIL, "key");
                    {
                        switch(lrn_info_per_core[core].key.key_type)
                        {
                        case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(key, "dip", lrn_info_per_core[core].key.key_val.ipv4_mc.dip));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(key, "fid", lrn_info_per_core[core].key.key_val.ipv4_mc.fid));
                        break;
                        case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
                        default:
                            SHR_IF_ERR_EXIT(mac_print_to_xml(unit, key, "mac", &lrn_info_per_core[core].key.key_val.mac.mac));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(key, "fid", lrn_info_per_core[core].key.key_val.mac.fid));
                        }
                    }
                    dbx_xml_node_end(key, 5);
                    SHR_NULL_CHECK((value = dbx_xml_child_add(curCore, "value", 5)), _SHR_E_FAIL, "value");
                    {
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(value, "accessed", lrn_info_per_core[core].value.accessed));
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(value, "group", lrn_info_per_core[core].value.group));
                        SHR_NULL_CHECK((aging_info = dbx_xml_child_add(value, "aging_info", 6)), _SHR_E_FAIL, "aging_info");
                        {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(aging_info, "is_dynamic", lrn_info_per_core[core].value.aging_info.is_dynamic));
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(aging_info, "age_status", lrn_info_per_core[core].value.aging_info.age_status));
                        }
                        dbx_xml_node_end(aging_info, 6);
                        SHR_NULL_CHECK((frwrd_info = dbx_xml_child_add(value, "frwrd_info", 6)), _SHR_E_FAIL, "frwrd_info");
                        {
                            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(frwrd_info, "drop_when_sa_is_known", lrn_info_per_core[core].value.frwrd_info.drop_when_sa_is_known));
                            SHR_NULL_CHECK((frwr_decision = dbx_xml_child_add(frwrd_info, "frwr_decision", 7)), _SHR_E_FAIL, "frwr_decision");
                            {
                                SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &lrn_info_per_core[core].value.frwrd_info.forward_decision, frwr_decision));
                            }
                            dbx_xml_node_end(frwr_decision, 7);
                        }
                        dbx_xml_node_end(frwrd_info, 6);
#ifdef BCM_88660_A0
                                SHR_NULL_CHECK((slb_info = dbx_xml_child_add(value, "slb_info", 6)), _SHR_E_FAIL, "slb_info");
                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_fec", lrn_info_per_core[core].value.slb_info.fec));
                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_lag_group_and_member", lrn_info_per_core[core].value.slb_info.lag_group_and_member));
                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_match_fec_entries", lrn_info_per_core[core].value.slb_info.match_fec_entries));
                                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "slb_info_match_lag_entries", lrn_info_per_core[core].value.slb_info.match_lag_entries));
#endif /* BCM_88660_A0 */
                        
                    }
                    dbx_xml_node_end(value, 5);

                } else {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "valid", "No Learning info for packet"));
                }

                dbx_xml_node_end(curCore, 4);
                }

            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);
  
    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);
 
 exit:
    SHR_FUNC_EXIT;
}
#endif

/******************************************************************** 
 *  Function handler: mode_info_get (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_mode_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_MODE_INFO   
    prm_mode_info;
  uint32
    soc_sand_dev_id;

  soc_sand_dev_id = (unit); 
  SOC_PPC_DIAG_MODE_INFO_clear(&prm_mode_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_diag_mode_info_get(
          soc_sand_dev_id,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 

  SOC_PPC_DIAG_MODE_INFO_print(&prm_mode_info);
  return CMD_OK;  
} 
  
/******************************************************************** 
 *  Function handler: mode_info_set (section diag)
 */
cmd_result_t 
  cmd_ppd_api_diag_mode_info_set(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_MODE_INFO   
    prm_mode_info_get;
  SOC_PPC_DIAG_MODE_INFO   
    prm_mode_info;
  uint32
    soc_sand_dev_id;
  char* flavor;
  int strnlen_flavor;
  parse_table_t    pt;
  int32 params_num;

  soc_sand_dev_id = (unit); 
  SOC_PPC_DIAG_MODE_INFO_clear(&prm_mode_info);
  SOC_PPC_DIAG_MODE_INFO_clear(&prm_mode_info_get);

  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_diag_mode_info_get(
          soc_sand_dev_id,
          &prm_mode_info_get
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL; 
  } 
  /* magic number is updated as 0 in sw_state_xxx function for prm_mode_info_get, only copy useful variable(flavor) */
  prm_mode_info.flavor = prm_mode_info_get.flavor;
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "flavor", PQ_DFL|PQ_STRING, 0, (void *)&flavor, NULL);
  prm_mode_info.flavor = SOC_PPC_DIAG_FLAVOR_NONE;

  params_num = parse_arg_eq(a, &pt);
  if (0 > params_num) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  /*
   * Make sure at least one of the strings is NULL terminated.
   */
  strnlen_flavor = sal_strnlen(flavor,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
  if ((params_num > 0) && (! sal_strncasecmp(flavor, "RAW", strnlen_flavor))) {
      prm_mode_info.flavor |= SOC_PPC_DIAG_FLAVOR_RAW;
  } 
  else if ((params_num > 0) && (! sal_strncasecmp(flavor, "clear_on_get", strnlen_flavor))) {
      prm_mode_info.flavor |= SOC_PPC_DIAG_FLAVOR_CLEAR_ON_GET;
  }
  else {
      prm_mode_info.flavor = 0;
  }
  parse_arg_eq_done(&pt);

  /* Call function */
  ret = soc_ppd_diag_mode_info_set(
          soc_sand_dev_id,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL;
  } 

  return CMD_OK; 
} 

/******************************************************************** 
 *  Function handler: pkt_associated_tm_info_get (section diag)
 */
int dnx_diag_pkt_tm_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_PKT_TM_INFO *pkt_tm_info_to_xml);
int 
  cmd_ppd_api_diag_pkt_associated_tm_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_PKT_TM_INFO   
    prm_pkt_tm_info;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core=-1;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;

 /*
  *    Variables for XML printing
  */
#if !defined(NO_FILEIO)
  SOC_PPC_DIAG_PKT_TM_INFO          pkt_tm_info_to_xml[SOC_DPP_DEFS_MAX(NOF_CORES)];
#endif
  char                              *prm_file = NULL;
  uint8                             b_print_to_xml = 0;


  soc_sand_dev_id = (unit); 
  SOC_PPC_DIAG_PKT_TM_INFO_clear(&prm_pkt_tm_info);
 
  /* Get parameters */ 

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);
   
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }


  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
  }

  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }
  if (prm_core == -1 || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_pkt_associated_tm_info_get(
              soc_sand_dev_id,
              core,
              &prm_pkt_tm_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
            parse_arg_eq_done(&pt);
            return CMD_FAIL; 
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          if (!b_print_to_xml) {
              LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
              continue; 
          }
      } 
      if (!b_print_to_xml) {
          SOC_PPC_DIAG_PKT_TM_INFO_print(&prm_pkt_tm_info);
      } else {
#if !defined(NO_FILEIO)
          pkt_tm_info_to_xml[core] = prm_pkt_tm_info;
#endif
      }
  }

  if (b_print_to_xml) {
#if !defined(NO_FILEIO)
      dnx_diag_pkt_tm_info_info_xml_print(unit, prm_file, pkt_tm_info_to_xml);
#endif
  }

  parse_arg_eq_done(&pt);
  return CMD_OK;  
} 

#if !defined(NO_FILEIO)
int dnx_diag_pkt_tm_info_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_PKT_TM_INFO *pkt_tm_info_to_xml){

    int core = 0;
    xml_node curTop, curDiags, curDiag, curCores, curCore;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
         * Add xml structure:
         * <diag cat="pp" name="PKT_associated_TM_info">
		 *    <cores list="Yes">
		 *    	<core id="0" frwrd_decision="uc_port" dest_id="202" forward_decision_outlif="0" tc="0" dp="1" meter1="Not Valid" meter2="Not Valid" dp_meter_cmd="0" counter1="Not Valid" counter2="Not Valid" cud="0" eth_meter_ptr="1006" ingress_shaping_da="Not Valid" ecn_capable="0" cni="0" st_vsq_ptr="0" lag_lb_key="257" ignore_cp="1" snoop_id="0"/>
		 *      <core id="1" frwrd_decision="uc_port" dest_id="202" forward_decision_outlif="0" tc="0" dp="1" meter1="Not Valid" meter2="Not Valid" dp_meter_cmd="0" counter1="Not Valid" counter2="Not Valid" cud="0" eth_meter_ptr="1006" ingress_shaping_da="Not Valid" ecn_capable="0" cni="0" st_vsq_ptr="0" lag_lb_key="257" ignore_cp="1" snoop_id="0"/>
		 *    </cores>
		 * </diag>
         */

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "PKT_associated_TM_info"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));

                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DEST)
                  {
                    SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit,&pkt_tm_info_to_xml[core].frwrd_decision, curCore));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "frwrd_decision", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_TC)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "tc", pkt_tm_info_to_xml[core].tc));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "tc", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DP)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "dp", pkt_tm_info_to_xml[core].dp));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "dp", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER1)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "meter0", pkt_tm_info_to_xml[core].meter1));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "meter0", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER2)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "meter1", pkt_tm_info_to_xml[core].meter2));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "meter1", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "dp_meter_cmd", pkt_tm_info_to_xml[core].dp_meter_cmd));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "dp_meter_cmd", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "counter1", pkt_tm_info_to_xml[core].counter1));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "counter1", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "counter2", pkt_tm_info_to_xml[core].counter2));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "counter2", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CUD)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "cud", pkt_tm_info_to_xml[core].cud));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "cud", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "eth_meter_ptr", pkt_tm_info_to_xml[core].eth_meter_ptr));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "eth_meter_ptr", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "ingress_shaping_da", pkt_tm_info_to_xml[core].ingress_shaping_da));
                  }
                  else
                  {
                      if(pkt_tm_info_to_xml[core].ingress_shaping_da != -1) {
                          SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "ingress_shaping_da", "Not Valid"));
                      }
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "ecn_capable", pkt_tm_info_to_xml[core].ecn_capable));
                  }
                  else
                  {
                      if(pkt_tm_info_to_xml[core].ecn_capable != -1) {
                          SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "ecn_capable", "Not Valid"));
                      }
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_CNI)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "cni", pkt_tm_info_to_xml[core].cni));
                  }
                  else
                  {
                      if(pkt_tm_info_to_xml[core].cni != -1) {
                          SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "cni", "Not Valid"));
                      }
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "da_type", (char *)soc_sand_SAND_PP_ETHERNET_DA_TYPE_to_string(pkt_tm_info_to_xml[core].da_type)));
                  }
                  else
                  {
                      if(pkt_tm_info_to_xml[core].da_type != -1) {
                          SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "da_type", "Not Valid"));
                      }
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "st_vsq_ptr", pkt_tm_info_to_xml[core].st_vsq_ptr));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "st_vsq_ptr", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "lag_lb_key", pkt_tm_info_to_xml[core].lag_lb_key));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "lag_lb_key", "Not Valid"));
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "ignore_cp", pkt_tm_info_to_xml[core].ignore_cp));
                  }
                  else
                  {
                      if(pkt_tm_info_to_xml[core].ignore_cp != -1) {
                          SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "ignore_cp", "Not Valid"));
                      }
                  }
                  if (pkt_tm_info_to_xml[core].valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID)
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "snoop_id", pkt_tm_info_to_xml[core].snoop_id));
                  }
                  else
                  {
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "snoop_id", "Not Valid"));
                  }

                
            }   
            dbx_xml_node_end(curCores, 3);
         }   
        dbx_xml_node_end(curDiag, 2);
     }   
    dbx_xml_node_end(curDiags, 1);
    
    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

 
 exit:
    SHR_FUNC_EXIT;
}
#endif

/******************************************************************** 
 *  Function handler: pkt_associated_tm_info_get (section diag)
 */
int 
  cmd_ppd_api_diag_mtr_info_get(int unit, args_t* a) 
{   
  uint32 
    ret, meter_core, meter_group, core_id;   
  bcm_policer_config_t
    policer_cfg;
  bcm_policer_t
    policer_id;
  parse_table_t    
	pt;
  int 
	cbl, ebl;
  SOC_PPC_MTR_METER_ID
	meter_id;

  bcm_policer_config_t_init(&policer_cfg);
  meter_core = meter_group = 0;
  cbl = ebl = 0;
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "id", PQ_INT, 0, &policer_id, NULL);
  parse_table_add(&pt, "group", PQ_DFL|PQ_INT, 0, &meter_group, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &meter_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  /* if core/group explicity requested, update policer_id*/
  if (meter_group != 0 || meter_core != 0) {
	  if (meter_core > _DPP_POLICER_METER_MAX_CORE_ID(unit) || meter_group > 1) {
		  return CMD_FAIL; 
	  }

	  policer_id = _DPP_POLICER_ID_FROM_METER_GROUP_CORE(unit, policer_id, meter_group, meter_core); 
  }

  /* Get meter config */
  ret = bcm_petra_policer_get(unit, policer_id, &policer_cfg); 
  if (ret != BCM_E_NONE) { 
        return CMD_FAIL; 
  } 

  /* Get bucket levels*/
  SOC_PPC_MTR_METER_ID_clear(&meter_id);
  core_id = _DPP_POLICER_ID_TO_CORE(unit, policer_id);
  meter_id.group = _DPP_POLICER_ID_TO_GROUP(unit, policer_id);
  meter_id.id = _DPP_POLICER_ID_TO_METER(unit, policer_id);

  ret = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_meter_ins_bucket_get,(unit, core_id, &meter_id, &cbl, &ebl));
  if(SOC_SAND_FAILURE(ret)) {
        return CMD_FAIL;
  }

  /* Print*/
  ARAD_PP_DIAG_MTR_INFO_print(unit, policer_id, &policer_cfg, cbl, ebl);

  return CMD_OK;  
} 

/******************************************************************** 
 *  Function handler: cmd_ppd_api_diag_eth_policer_info_get (section diag)
 */
extern SOC_PPC_MTR_BW_PROFILE_INFO diag_pp_eth_policer_config;
int 
  cmd_ppd_api_diag_eth_policer_info_get(int unit, args_t* a) 
{   
  uint32 
    ret,kbits_sec,kbits_burst,port_index,
    aggregate_policer_valid,aggregate_policer_id;
  int
	core_id,i;
  bcm_port_t
	port;   
  SOC_PPC_MTR_BW_PROFILE_INFO 
    policers_arr[SOC_PPC_NOF_MTR_ETH_TYPES];
  int
    bucket_lvl_arr[SOC_PPC_NOF_MTR_ETH_TYPES] = {-1},
	data_type_arr[SOC_PPC_NOF_MTR_ETH_TYPES]  = {BCM_RATE_DLF,BCM_RATE_UCAST,BCM_RATE_UNKNOWN_MCAST,BCM_RATE_MCAST,BCM_RATE_BCAST};
  int
	eth_mtr_bkt_lvl_mem,
    eth_mtr_config_mem;
  uint32
    glbl_prf_ptr_field,
	glbl_prf_valid_field;
  uint32
    eth_mtr_config_tbl_entry,
	bckLvl_tbl_offset,
	bckLvl_tbl_entry,
	cbl_bucket_fld[4]= {CBL_0f, CBL_1f, CBL_2f, CBL_3f};
  soc_reg_above_64_val_t
	eth_mtr_bck_lvl_above_64_val;
  uint32
    eth_mtr_config_val;

  uint32 dummy_flags;
  bcm_port_interface_info_t interface_info;
  bcm_port_mapping_info_t mapping_info ;
  bcm_gport_t gport;

  parse_table_t    
	pt;

  eth_mtr_bkt_lvl_mem  = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit, MCDA_DYNAMIC);
  glbl_prf_ptr_field   = SOC_IS_QAX(unit) ? GLBL_ETH_MTR_PTRf : GLOBAL_METER_PROFILEf ;
  glbl_prf_valid_field = SOC_IS_QAX(unit) ? GLBL_ETH_MTR_PTR_VALIDf : GLOBAL_METER_PROFILE_VALIDf ;
  eth_mtr_config_mem   = SOC_IS_QAX(unit) ? CGM_ETH_MTR_PTR_MAPm : IDR_ETHERNET_METER_CONFIGm;

	/* Get parameters */ 
	parse_table_init(unit, &pt);
	parse_table_add(&pt, "port", PQ_INT, 0, &port, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

	/* init the local variables*/
	SOC_REG_ABOVE_64_CLEAR(eth_mtr_bck_lvl_above_64_val);
    gport = port;
	ret = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
	if (ret != BCM_E_NONE) {
		return CMD_FAIL;
	}
	core_id = mapping_info.core;

    port_index = (port&0xFF)*SOC_PPC_NOF_MTR_ETH_TYPES; /* port_index is the port ondex in the IDR_ETHERNET_METER_CONFIG table*/ 
    eth_mtr_config_tbl_entry = port_index;

	/* Get Eth Policer config and bucket levels*/
	for (i=0;i<SOC_PPC_NOF_MTR_ETH_TYPES;i++) {
		ret = bcm_rate_bandwidth_get(unit, port, data_type_arr[i], &kbits_sec, &kbits_burst);
		if (ret != BCM_E_NONE) {
			return CMD_FAIL;
		}
		policers_arr[i].cbs = diag_pp_eth_policer_config.cbs;
		policers_arr[i].cir = diag_pp_eth_policer_config.cir;
		policers_arr[i].disable_cir = diag_pp_eth_policer_config.disable_cir;
		policers_arr[i].color_mode = diag_pp_eth_policer_config.color_mode;
		policers_arr[i].is_packet_mode = diag_pp_eth_policer_config.is_packet_mode;
		policers_arr[i].is_pkt_truncate = diag_pp_eth_policer_config.is_pkt_truncate;

		bckLvl_tbl_entry  = (port_index + i)>>2; /* /4 */
		bckLvl_tbl_offset = (port_index + i)&0x3;/* %4 */
		ret = soc_mem_read(unit, eth_mtr_bkt_lvl_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), bckLvl_tbl_entry, (void*)eth_mtr_bck_lvl_above_64_val); 
		if (ret != BCM_E_NONE) {
			return CMD_FAIL;
		}
		bucket_lvl_arr[i] = soc_mem_field32_get(unit, eth_mtr_bkt_lvl_mem, (void*)eth_mtr_bck_lvl_above_64_val, cbl_bucket_fld[bckLvl_tbl_offset]);
	}

	/* check if that port is also attribute with an aggregate policer*/
	if (SOC_IS_QAX(unit)) {
		ret = soc_mem_array_read(unit, eth_mtr_config_mem, core_id, MEM_BLOCK_ANY, eth_mtr_config_tbl_entry, (void *)&eth_mtr_config_val);
		if (ret != BCM_E_NONE) {
			return CMD_FAIL;
		}
	}else{
		ret = soc_mem_read(unit, eth_mtr_config_mem, MEM_BLOCK_ANY, eth_mtr_config_tbl_entry, (void *)&eth_mtr_config_val);
		if (ret != BCM_E_NONE) {
			return CMD_FAIL;
		}
	}
	aggregate_policer_valid = soc_mem_field32_get(unit, eth_mtr_config_mem, &eth_mtr_config_val, glbl_prf_valid_field);
	aggregate_policer_id    = soc_mem_field32_get(unit, eth_mtr_config_mem, &eth_mtr_config_val, glbl_prf_ptr_field);

	/* Print */
	ARAD_PP_DIAG_ETH_POLICER_INFO_print(unit, port, policers_arr, bucket_lvl_arr, aggregate_policer_valid, aggregate_policer_id);

  return CMD_OK;  
} 

/******************************************************************** 
 *  Function handler: cmd_ppd_api_diag_aggregate_eth_policer_info_get (section diag)
 */
int 
  cmd_ppd_api_diag_aggregate_eth_policer_info_get(int unit, args_t* a) 
{   

  uint32 
    ret;
  int
	core_id,i,num_to_alloc=0,policer_index;
  int mode_to_nopolicers[5] = {1,3,2,-1,4}; /* according to _dpp_policer_group_num_policers under policer.h
                                               only five relevant fields*/
  bcm_policer_t policer_id;
  bcm_policer_group_mode_t mode;
  bcm_policer_config_t pol_cfg;

  SOC_PPC_MTR_BW_PROFILE_INFO 
    policers_arr[SOC_PPC_NOF_MTR_ETH_TYPES];

  int
    bucket_lvl_arr[SOC_PPC_NOF_MTR_ETH_TYPES] = {-1};
  int
	eth_mtr_bkt_lvl_mem;
  uint32
	bckLvl_tbl_offset,
	bckLvl_tbl_entry,
	cbl_bucket_fld[4]= {CBL_0f, CBL_1f, CBL_2f, CBL_3f};
  soc_reg_above_64_val_t
	  eth_mtr_bck_lvl_above_64_val;

  parse_table_t    
	pt;

	SOCDNX_INIT_FUNC_DEFS;

	eth_mtr_bkt_lvl_mem  = JER_MRPS_EM_MEM_FORMAT_BY_CHIP(unit, MCDB_DYNAMIC);

	/* Get parameters */ 
	parse_table_init(unit, &pt);
    parse_table_add(&pt, "policer_id", PQ_INT, 0, &policer_id, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

	/* init the local variables*/
	SOC_REG_ABOVE_64_CLEAR(eth_mtr_bck_lvl_above_64_val);

	core_id = _DPP_ETH_POLICER_CORE_GET(unit, policer_id);
    policer_index = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    /* policer to mode */
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.policer.policer_group.mode.get(unit, core_id, policer_index, &mode));
    num_to_alloc = mode_to_nopolicers[mode];            

	/* Get Eth Policer config and bucket levels*/
	for (i=0;i<num_to_alloc;i++) {
        ret = bcm_petra_policer_get(unit, policer_id+i,&pol_cfg);
  		if (ret != BCM_E_NONE) {
			return CMD_FAIL;
		}
		policers_arr[i].cbs = pol_cfg.ckbits_burst;
		policers_arr[i].cir = pol_cfg.ckbits_sec;
		policers_arr[i].color_mode = (pol_cfg.flags & BCM_POLICER_COLOR_BLIND) == 0 ? 0 : 1;
		policers_arr[i].is_packet_mode = (pol_cfg.flags & BCM_POLICER_MODE_PACKETS) == 0 ? 0 : 1;
		policers_arr[i].is_pkt_truncate = (pol_cfg.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE) == 0 ? 0 : 1;

		bckLvl_tbl_entry  = (policer_index + i)>>2; /* /4 */
		bckLvl_tbl_offset = (policer_index + i)&0x3;/* %4 */
		ret = soc_mem_read(unit, eth_mtr_bkt_lvl_mem, ARAD_PP_ETH_MTR_MEM_BLOCK(unit, core_id), bckLvl_tbl_entry, (void*)eth_mtr_bck_lvl_above_64_val); 
		if (ret != BCM_E_NONE) {
			return CMD_FAIL;
		}
		bucket_lvl_arr[i] = soc_mem_field32_get(unit, eth_mtr_bkt_lvl_mem, (void*)eth_mtr_bck_lvl_above_64_val, cbl_bucket_fld[bckLvl_tbl_offset]);
	}

	/* Print */
	ARAD_PP_DIAG_AGGREGATE_ETH_POLICER_INFO_print(unit, policer_id, num_to_alloc, policers_arr, bucket_lvl_arr);
	
  return CMD_OK;  
  exit:
      SOCDNX_FUNC_RETURN;

} 

/******************************************************************** 
 *  Function handler: pkt_trace_clear (section diag)
 */
int 
  cmd_ppd_api_diag_pkt_trace_clear(int unit, args_t* a) 
{   
  uint32 
    ret;   
  uint32   
    prm_pkt_trace = SOC_PPC_DIAG_PKT_TRACE_ALL;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core = 0;
   
  soc_sand_dev_id = (unit); 
 
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  if ((prm_core < 0 ) 
	  || 
	(prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
	  cli_out("error ilegal core ID for device\n");
	  return CMD_FAIL;
  }

  /* Call function */
  ret = soc_ppd_diag_pkt_trace_clear(
          soc_sand_dev_id,
          prm_core,
          prm_pkt_trace
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        return CMD_FAIL;
  } 

  return CMD_OK;  
} 


/******************************************************************** 
 *  Function handler: db_lem_lkup_info_get (section diag)
 */
int dnx_diag_db_lem_lkup_info_xml_print(int unit, char *xml_file, uint8 *prm_valid_to_xml, 
                                                SOC_PPC_DIAG_LEM_LKUP_TYPE *prm_type_to_xml, 
                                                SOC_PPC_DIAG_LEM_KEY *prm_lem_key_to_xml, 
                                                SOC_PPC_DIAG_LEM_VALUE *prm_lem_val_to_xml);
int 
                    cmd_ppd_api_diag_db_lem_lkup_info_get(int unit, args_t* a) 
{   
  uint32 
    ret;   
  SOC_PPC_DIAG_DB_USE_INFO   
    prm_db_info;
  SOC_PPC_DIAG_LEM_LKUP_TYPE   
    prm_type;
  SOC_PPC_DIAG_LEM_KEY   
    prm_key;
  SOC_PPC_DIAG_LEM_VALUE   
    prm_val;
  uint8   
    prm_valid;
  uint32
    soc_sand_dev_id;
  parse_table_t    pt;
  int              prm_core = -1;
  int              core, first_core, last_core;
  /*
  *     Parameters for XML printing
  */
#if !defined(NO_FILEIO)
  SOC_PPC_DIAG_LEM_KEY          prm_lem_key_to_xml[SOC_DPP_DEFS_MAX(NOF_CORES)];
  SOC_PPC_DIAG_LEM_LKUP_TYPE    prm_type_to_xml[SOC_DPP_DEFS_MAX(NOF_CORES)];
  SOC_PPC_DIAG_LEM_VALUE        prm_lem_val_to_xml[SOC_DPP_DEFS_MAX(NOF_CORES)];
  uint8                         prm_valid_to_xml[SOC_DPP_DEFS_MAX(NOF_CORES)];
#endif
  char*                         prm_file = NULL;
  uint8                         b_print_to_xml = 0;

  soc_sand_dev_id = (unit); 
  SOC_PPC_DIAG_DB_USE_INFO_clear(&prm_db_info);
  SOC_PPC_DIAG_LEM_KEY_clear(&prm_key);
  SOC_PPC_DIAG_LEM_VALUE_clear(&prm_val);
 
  /* Get parameters */
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "lkup_num", PQ_DFL|PQ_INT, 0, &prm_db_info.lkup_num, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
  }
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }
  if (prm_core == -1 || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

  /* Call function */
      ret = soc_ppd_diag_db_lem_lkup_info_get(
              soc_sand_dev_id,
              core,
              &prm_db_info,
              &prm_type,
              &prm_key,
              &prm_val,
              &prm_valid
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
      } 
      if (!b_print_to_xml) {
          cli_out("%s\r",get_core_str(unit, core));

          cli_out("type: %s\n\r",SOC_PPC_DIAG_LEM_LKUP_TYPE_to_string(prm_type));

          cli_out("key:\n\r");
          SOC_PPC_DIAG_LEM_KEY_print(&prm_key,prm_type);
          if (prm_valid)
          {
            cli_out("value:\n\r");
            SOC_PPC_DIAG_LEM_VALUE_print(&prm_val,prm_type);
          }
          cli_out("valid: %u\n\r",prm_valid);
      } else {
#if !defined(NO_FILEIO)
          prm_lem_key_to_xml[core] = prm_key;
          prm_type_to_xml[core] = prm_type;
          prm_lem_val_to_xml[core] = prm_val;
          prm_valid_to_xml[core] = prm_valid;
#endif

      }
      
  }
  if (b_print_to_xml) {
#if !defined(NO_FILEIO)
      dnx_diag_db_lem_lkup_info_xml_print(unit, prm_file, prm_valid_to_xml, prm_type_to_xml, prm_lem_key_to_xml, prm_lem_val_to_xml);
#endif /* NO_FILEIO*/
  }

  parse_arg_eq_done(&pt);
  return CMD_OK; 
} 
int lem_value_print_to_xml(int unit, xml_node cur_node, SOC_PPC_DIAG_LEM_LKUP_TYPE *type,  SOC_PPC_DIAG_LEM_VALUE *value);
int l2_lif_ac_key_to_xml_print(int unit, xml_node cur_node, SOC_PPC_L2_LIF_AC_KEY *lif_ac_key);
int lem_lif_key_to_xml_print(int unit, xml_node cur_node, SOC_PPC_DIAG_LIF_KEY *lif_key, SOC_PPC_DIAG_LIF_LKUP_TYPE *lif_lkup_type);
int lem_key_to_xml_print(int unit, xml_node key, SOC_PPC_DIAG_LEM_LKUP_TYPE *prm_type_to_xml, SOC_PPC_DIAG_LEM_KEY *prm_lem_key_to_xml);
#if !defined(NO_FILEIO)
int dnx_diag_db_lem_lkup_info_xml_print(int unit, char *xml_file, uint8 *prm_valid_to_xml, 
                                                SOC_PPC_DIAG_LEM_LKUP_TYPE *prm_type_to_xml, 
                                                SOC_PPC_DIAG_LEM_KEY *prm_lem_key_to_xml, 
                                                SOC_PPC_DIAG_LEM_VALUE *prm_lem_val_to_xml){

    int core = 0;
    xml_node curTop, curDiags, curDiag, curCores, curCore, key, value;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
        * Add xml structure:
        * <diag cat="pp" name="DB_LEM_lkup_info">
        *   <cores list="Yes">
        *     <core id="0" type="host" valid="0">
        *        <key>
        *            <host vrf_id="0">
        *                <subnet ip_address="0.0.0.0" prefix_len="0"/>
        *            </host>
        *        </key>
        *     </core>
        *     <core id="1" type="host" valid="0">
        *        <key>
        *            <host vrf_id="0">
        *               <subnet ip_address="224.224.224.1" prefix_len="32"/>
        *            </host>
        *        </key>
        *     </core>
        *   </cores>
        * </diag>  
        */ 

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "DB_LEM_lkup_info"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCore, "type", (char *)SOC_PPC_DIAG_LEM_LKUP_TYPE_to_string(prm_type_to_xml[core])));
                SHR_NULL_CHECK((key = dbx_xml_child_add(curCore, "key", 5)), _SHR_E_FAIL, "key");
                SHR_IF_ERR_EXIT(lem_key_to_xml_print(unit, key, &prm_type_to_xml[core], &prm_lem_key_to_xml[core]));
                if (prm_valid_to_xml[core]) {
                    SHR_NULL_CHECK((value = dbx_xml_child_add(curCore, "value", 5)), _SHR_E_FAIL, "value");
                    /* VALUE PRINT HERE -----------------------*/
                    SHR_IF_ERR_EXIT(lem_value_print_to_xml(unit, value, &prm_type_to_xml[core], &prm_lem_val_to_xml[core]));
                }
                dbx_xml_node_end(key, 5);
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "valid", prm_valid_to_xml[core]));


                dbx_xml_node_end(curCore, 4);
            }

            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);
    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

 
 exit:
    SHR_FUNC_EXIT;
}

/* This function mirrors the output/print of SOC_PPC_DIAG_LEM_VALUE_print */
int lem_value_print_to_xml(int unit, xml_node cur_node, SOC_PPC_DIAG_LEM_LKUP_TYPE *type,  SOC_PPC_DIAG_LEM_VALUE *value) {

    xml_node mact, bmact, frwrding_info, aging_info, slb_info, raw, vid_assign, host, frwrd_decision, ilm, sa_auth, expect_system_port;
    char attr_name[48];
    uint8 ind;
    SHR_FUNC_INIT_VARS(unit);

    switch(*type)
    {
    case SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT:
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "name", "mact"));
        SHR_NULL_CHECK((mact = dbx_xml_child_add(cur_node, "mact", 6)), _SHR_E_FAIL, "mact");
        SHR_NULL_CHECK((frwrding_info = dbx_xml_child_add(mact, "frwrding_info", 7)), _SHR_E_FAIL, "frwrding_info");
        SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &value->mact.frwrd_info.forward_decision, frwrding_info));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(frwrding_info, "drop_when_sa_is_known", value->mact.frwrd_info.drop_when_sa_is_known));
        dbx_xml_node_end(frwrding_info, 7);
        SHR_NULL_CHECK((aging_info = dbx_xml_child_add(mact, "aging_info", 7)), _SHR_E_FAIL, "aging_info");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(aging_info, "aging_info_is_dynamic", value->mact.aging_info.is_dynamic));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(aging_info, "age_status", value->mact.aging_info.age_status));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mact, "accessed", value->mact.accessed));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mact, "group", value->mact.group));
        
#ifdef BCM_88660_A0
        SHR_NULL_CHECK((slb_info = dbx_xml_child_add(mact, "slb_info", 7)), _SHR_E_FAIL, "slb_info");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "fec", value->mact.slb_info.fec));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "lag_group_and_member", value->mact.slb_info.lag_group_and_member));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "match_fec_entries", value->mact.slb_info.match_fec_entries));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(slb_info, "match_lag_entries", value->mact.slb_info.match_lag_entries));
#endif /* BCM_88660_A0 */
        dbx_xml_node_end(mact, 6);
    break;
    case SOC_PPC_DIAG_LEM_LKUP_TYPE_BMACT:
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "name", "bmact"));
        SHR_NULL_CHECK((bmact = dbx_xml_child_add(cur_node, "bmact", 6)), _SHR_E_FAIL, "bmact");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(bmact, "sys_port_id", value->bmact.sys_port_id));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(bmact, "i_sid_domain", value->bmact.i_sid_domain));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(bmact, "sa_learn_fec_id", value->bmact.sa_learn_fec_id));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(bmact, "drop_sa", value->bmact.drop_sa));
    break;
    case SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST:
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "name", "host_lookup"));
        SHR_NULL_CHECK((host = dbx_xml_child_add(cur_node, "host", 6)), _SHR_E_FAIL, "host");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(host, "fec_id", value->host.fec_id));
        SHR_NULL_CHECK((frwrd_decision = dbx_xml_child_add(host, "frwrd_decision", 7)), _SHR_E_FAIL, "frwrd_decision");
        SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &value->host.frwrd_decision, frwrd_decision));
        dbx_xml_node_end(frwrd_decision, 7);
        if (value->host.eep != SOC_PPC_EEP_NULL) {
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(host, "eep", value->host.eep));
        } else {
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(host, "eep", "NULL"));
        }
        dbx_xml_node_end(host, 6);
    break;
    case SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM:
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "name", "ilm"));
        SHR_NULL_CHECK((ilm = dbx_xml_child_add(cur_node, "ilm", 6)), _SHR_E_FAIL, "ilm");
        SHR_IF_ERR_EXIT(frwrding_decision_info_to_xml(unit, &value->ilm, ilm));
        dbx_xml_node_end(ilm, 6);
    break;
    case SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH:

        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "name", "sa_auth"));
        SHR_NULL_CHECK((sa_auth = dbx_xml_child_add(cur_node, "sa_auth", 6)), _SHR_E_FAIL, "sa_auth");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(sa_auth, "tagged_only", value->sa_auth.tagged_only));

        if (value->sa_auth.expect_tag_vid == SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS)
        {
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(sa_auth, "expect_tag_vid", "ANY"));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(sa_auth, "expect_tag_vid", value->sa_auth.expect_tag_vid));
        }

        if (value->sa_auth.expect_system_port.sys_id == SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
        {
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(sa_auth, "expect_system_port", "ANY"));
        }
        else
        {
            SHR_NULL_CHECK((expect_system_port = dbx_xml_child_add(cur_node, "expect_system_port", 7)), _SHR_E_FAIL, "expect_system_port");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(expect_system_port, "sys_port_type", (char *)soc_sand_SAND_PP_SYS_PORT_TYPE_to_string(value->sa_auth.expect_system_port.sys_port_type)));
            SHR_IF_ERR_EXIT(dbx_xml_property_set_int(expect_system_port, "sys_id", value->sa_auth.expect_system_port.sys_id));    
        }

        dbx_xml_node_end(sa_auth, 6);
        SHR_NULL_CHECK((vid_assign = dbx_xml_child_add(cur_node, "vid_assign", 6)), _SHR_E_FAIL, "vid_assign");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(vid_assign, "vid", value->sa_vid_assign.vid));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(vid_assign, "use_for_untagged", value->sa_vid_assign.use_for_untagged));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(vid_assign, "override_tagged", value->sa_vid_assign.override_tagged));

    break;
    default:
        SHR_NULL_CHECK((raw = dbx_xml_child_add(cur_node, "raw", 5)), _SHR_E_FAIL, "raw");
        for (ind = 0; ind < 2; ++ind)
        {  
          sal_sprintf(attr_name, "raw_%u", ind);
          SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(raw, attr_name, value->raw[ind]));
        }
    break;
    }

 exit:
    SHR_FUNC_EXIT;

}

/* This is mirroring the printing behaviour of SOC_PPC_DIAG_LEM_KEY_print */
int lem_key_to_xml_print(int unit, xml_node key, SOC_PPC_DIAG_LEM_LKUP_TYPE *prm_type_to_xml, SOC_PPC_DIAG_LEM_KEY *prm_lem_key_to_xml) {

    xml_node mact, bmact, host, subnet, ilm, sa_auth, extended_p2p, raw;
    uint8 ind = 0;
    char decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
    char attr_name[48];
    SHR_FUNC_INIT_VARS(unit);

  switch(*prm_type_to_xml)
  {
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT:
    SHR_NULL_CHECK((mact = dbx_xml_child_add(key, "mact", 6)), _SHR_E_FAIL, "mact");
    switch(prm_lem_key_to_xml->mact.key_type)
    {
    case SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC:
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mact, "dip", prm_lem_key_to_xml->mact.key_val.ipv4_mc.dip));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mact, "fid", prm_lem_key_to_xml->mact.key_val.ipv4_mc.fid));
    break;
    case SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR:
    default:
        SHR_IF_ERR_EXIT(mac_print_to_xml(unit, mact, "mac", &prm_lem_key_to_xml->mact.key_val.mac.mac));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mact, "fid", prm_lem_key_to_xml->mact.key_val.mac.fid));
    }
    dbx_xml_node_end(mact, 6);
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_BMACT:
    SHR_NULL_CHECK((bmact = dbx_xml_child_add(key, "bmact", 6)), _SHR_E_FAIL, "bmact");
    SHR_IF_ERR_EXIT(mac_print_to_xml(unit, bmact, "mac", &prm_lem_key_to_xml->bmact.b_mac_addr));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(bmact, "flags", prm_lem_key_to_xml->bmact.flags));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(bmact, "vid", prm_lem_key_to_xml->bmact.b_vid));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(bmact, "local_port", prm_lem_key_to_xml->bmact.local_port_ndx));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST:
    SHR_NULL_CHECK((host = dbx_xml_child_add(key, "host", 6)), _SHR_E_FAIL, "host");
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(host, "vrf_id", prm_lem_key_to_xml->host.vrf_id));
    SHR_NULL_CHECK((subnet = dbx_xml_child_add(host, "subnet", 7)), _SHR_E_FAIL, "subnet");
    {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(subnet, "ip_address", (char *)soc_sand_pp_ip_long_to_string(prm_lem_key_to_xml->host.subnet.subnet.ip_address,1,decimal_ip)));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(subnet, "prefix_len", prm_lem_key_to_xml->host.subnet.subnet.prefix_len));
    }
    dbx_xml_node_end(host, 6);
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM:
    SHR_NULL_CHECK((ilm = dbx_xml_child_add(key, "ilm", 6)), _SHR_E_FAIL, "ilm");
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ilm, "in_label", prm_lem_key_to_xml->ilm.in_label));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ilm, "mapped_exp", prm_lem_key_to_xml->ilm.mapped_exp));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ilm, "in_local_port", prm_lem_key_to_xml->ilm.in_local_port));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ilm, "inrif", prm_lem_key_to_xml->ilm.inrif));
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ilm, "vrf", prm_lem_key_to_xml->ilm.vrf));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH:
    SHR_NULL_CHECK((sa_auth = dbx_xml_child_add(key, "sa_auth", 6)), _SHR_E_FAIL, "sa_auth");
    SHR_IF_ERR_EXIT(mac_print_to_xml(unit, sa_auth, "sa_auth", &prm_lem_key_to_xml->sa_auth));
  break;
  case SOC_PPC_DIAG_LEM_LKUP_TYPE_EXTEND_P2P:
    SHR_NULL_CHECK((extended_p2p = dbx_xml_child_add(key, "extended_p2p", 6)), _SHR_E_FAIL, "extended_p2p");
    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(extended_p2p, "type", (char *)SOC_PPC_DIAG_LIF_LKUP_TYPE_to_string(prm_lem_key_to_xml->extend_p2p_key.type)));
    SHR_IF_ERR_EXIT(lem_lif_key_to_xml_print(unit, extended_p2p,  &prm_lem_key_to_xml->extend_p2p_key.key, &prm_lem_key_to_xml->extend_p2p_key.type));
    dbx_xml_node_end(extended_p2p, 6);
    break;
  default:
    SHR_NULL_CHECK((raw = dbx_xml_child_add(key, "raw", 6)), _SHR_E_FAIL, "raw");
    for (ind = 0; ind < 3; ++ind)
    {
      sal_sprintf(attr_name, "raw_%u", ind);
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(raw, attr_name, prm_lem_key_to_xml->raw[ind]));
    }
    break;
  }

exit:
    SHR_FUNC_EXIT;

}

/* This funtions is mirroring the output/printing of SOC_PPC_DIAG_LIF_KEY_print*/
int lem_lif_key_to_xml_print(int unit, xml_node cur_node, SOC_PPC_DIAG_LIF_KEY *lif_key, SOC_PPC_DIAG_LIF_LKUP_TYPE *lif_lkup_type) {

    char attr_name[48];
    uint8 ind;
    xml_node ac_lif, pwe_lif, mpls_rif, ip_rif, mim_lif, gre_lif, vxlan_lif, extender_lif, raw;
    SHR_FUNC_INIT_VARS(unit);

    switch(*lif_lkup_type)
    {
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_AC:
      SHR_NULL_CHECK((ac_lif = dbx_xml_child_add(cur_node, "ac_lif", 7)), _SHR_E_FAIL, "ac_lif");
      SHR_IF_ERR_EXIT(l2_lif_ac_key_to_xml_print(unit, ac_lif,  &lif_key->ac));
      dbx_xml_node_end(ac_lif, 7);
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE:
      SHR_NULL_CHECK((pwe_lif = dbx_xml_child_add(cur_node, "pwe_lif", 7)), _SHR_E_FAIL, "pwe_lif");
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(pwe_lif, "pwe", lif_key->pwe));
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL:
      SHR_NULL_CHECK((mpls_rif = dbx_xml_child_add(cur_node, "mpls_rif", 7)), _SHR_E_FAIL, "mpls_rif");
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mpls_rif, "label_id", lif_key->mpls.label_id));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mpls_rif, "vsid", lif_key->mpls.vsid));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mpls_rif, "label_id_second", lif_key->mpls.label_id_second));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mpls_rif, "flags", lif_key->mpls.flags));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(mpls_rif, "label_index", (char *)SOC_PPC_MPLS_LABEL_INDEX_to_string(lif_key->mpls.label_index)));
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL:
      SHR_NULL_CHECK((ip_rif = dbx_xml_child_add(cur_node, "ip_rif", 7)), _SHR_E_FAIL, "ip_rif");
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "dip", lif_key->ip_tunnel.dip));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "dip_prefix_len", lif_key->ip_tunnel.dip_prefix_len));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "sip", lif_key->ip_tunnel.sip));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "sip_prefix_len", lif_key->ip_tunnel.sip_prefix_len));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "flags", lif_key->ip_tunnel.flags));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "vrf", lif_key->ip_tunnel.vrf));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "ipv4_next_protocol", lif_key->ip_tunnel.ipv4_next_protocol));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(ip_rif, "ipv4_next_protocol_prefix_len", lif_key->ip_tunnel.ipv4_next_protocol_prefix_len));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ip_rif, "port_property_en", lif_key->ip_tunnel.port_property_en));
      if (lif_key->ip_tunnel.port_property_en) {
          SHR_IF_ERR_EXIT(dbx_xml_property_set_int(ip_rif, "port_property", lif_key->ip_tunnel.port_property));
      }
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID:
      SHR_NULL_CHECK((mim_lif = dbx_xml_child_add(cur_node, "mim_lif", 7)), _SHR_E_FAIL, "mim_lif");
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mim_lif, "isid_id", lif_key->mim.isid_id));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(mim_lif, "isid_domain", lif_key->mim.isid_domain));
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE:
      SHR_NULL_CHECK((gre_lif = dbx_xml_child_add(cur_node, "gre_lif", 7)), _SHR_E_FAIL, "gre_lif");
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(gre_lif, "vpn_key", lif_key->l2gre.vpn_key));
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN:
      SHR_NULL_CHECK((vxlan_lif = dbx_xml_child_add(cur_node, "vxlan_lif", 7)), _SHR_E_FAIL, "vxlan_lif");
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(vxlan_lif, "vpn_key", lif_key->vxlan.vpn_key));
    break;
    case SOC_PPC_DIAG_LIF_LKUP_TYPE_EXTENDER:
      SHR_NULL_CHECK((extender_lif = dbx_xml_child_add(cur_node, "extender_lif", 7)), _SHR_E_FAIL, "extender_lif");
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(extender_lif, "extender_port_vid", lif_key->extender.extender_port_vid));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(extender_lif, "vid", lif_key->extender.vid));
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(extender_lif, "name_space", lif_key->extender.name_space));
    break;
    default:
      SHR_NULL_CHECK((raw = dbx_xml_child_add(cur_node, "raw", 7)), _SHR_E_FAIL, "raw");   
      for (ind = 0; ind < 6; ++ind)
      {
        sal_sprintf(attr_name, "raw_%u", ind);
        SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(raw, attr_name, lif_key->raw[ind])); 
      }
    break;
    }


exit:
    SHR_FUNC_EXIT;

}


/* This function mirrors the output/printing of SOC_PPC_L2_LIF_AC_KEY_print */
int l2_lif_ac_key_to_xml_print(int unit, xml_node cur_node, SOC_PPC_L2_LIF_AC_KEY *lif_ac_key) {

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "key_type", (char *)SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_to_string(lif_ac_key->key_type)));

  if (lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_RAW)
  {
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "vlan_domain", lif_ac_key->vlan_domain));
    if (lif_ac_key->outer_vid == SOC_PPC_LIF_IGNORE_OUTER_VID)
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "outer_vid", "ignored"));
    }
    else
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "outer_vid", lif_ac_key->outer_vid));
    }
    if (lif_ac_key->inner_vid == SOC_PPC_LIF_IGNORE_INNER_VID)
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "inner_vid", "ignored"));
    }
    else
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "inner_vid", lif_ac_key->inner_vid));
    }
    if (lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN
        && lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_PCP_VLAN_VLAN
        && lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN)
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "outer_pcp", "ignored"));
    }
    else
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "outer_pcp", lif_ac_key->outer_pcp));
    }

    if (lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN)
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "ethertype", "ignored"));
    }
    else
    {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "ethertype", lif_ac_key->ethertype));
    }

    if (lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL 
        && lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN
        && lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_COMP_VLAN_COMP_VLAN
        && lif_ac_key->key_type != SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_TUNNEL_PCP_ETHER_TYPE_COMP_VLAN_COMP_VLAN) {
      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "tunnel", "ignored"));
    }
    else
    {
      if (lif_ac_key->tunnel == SOC_PPC_LIF_IGNORE_TUNNEL_VID)
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(cur_node, "tunnel", "ignored"));
      }
      else
      {
        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "tunnel", lif_ac_key->tunnel));
      }
    }
  }
  else
  {
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(cur_node, "raw_key", lif_ac_key->raw_key));
  }

exit:
    SHR_FUNC_EXIT;


}
#endif /* NO_FILEIO*/


/******************************************************************** 
 *  Function handler: vsi_info_get (section diag)
 */
int dnx_diag_vsi_info_xml_print(int unit, char *prm_file, uint32 *vsi_info_per_core);

cmd_result_t
  cmd_ppd_api_diag_vsi_info_get(int unit, args_t* a)
{
  uint32
    ret;
  SOC_PPC_DIAG_VSI_INFO
    prm_vsi_info;
  uint32
    soc_sand_dev_id;
  int              prm_core = -1;
  int              core, first_core, last_core;
  parse_table_t    pt;

  /*
   * For printing to xml file
   */
  char             *prm_file = NULL;
#if !defined(NO_FILEIO)
  uint32            vsi_info_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)] = {0};
  int               vsi_info_per_core_len = 0;
#endif
  uint8             b_print_to_xml = 0;

  soc_sand_dev_id = (unit);
  SOC_PPC_DIAG_VSI_INFO_clear(&prm_vsi_info);

  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);

  /* Get parameters */
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
  }
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }


  if ((prm_core == -1) || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_vsi_info_get(
              soc_sand_dev_id,
              core,
              &prm_vsi_info
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
        continue;
      }
      if (!b_print_to_xml) {
          cli_out("%s\n", get_core_str(unit, core));
          SOC_PPC_DIAG_VSI_INFO_print(&prm_vsi_info);
      }
      else {
#if !defined(NO_FILEIO)
          vsi_info_per_core[vsi_info_per_core_len++] = prm_vsi_info.vsi;
#endif
      }
  }

  if (b_print_to_xml) {
      /*
       * Print to XML file
       */
#if !defined(NO_FILEIO)
      dnx_diag_vsi_info_xml_print(unit, prm_file, vsi_info_per_core);
#endif
  }

  parse_arg_eq_done(&pt);
  return CMD_OK;
}


/*
 * Print last packet(s) vsi information (per core) to an xml file
 */
#if !defined(NO_FILEIO)
int dnx_diag_vsi_info_xml_print(int unit, char *xml_file, uint32 *vsi_info_per_core){

    int core = 0;
    xml_node curTop, curDiags, curDiag, curCores, curCore;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
         * Add xml structure:
         * <diag cat="pp" name="vsi_decision">
         *     <cores list="1">
         *         <core id="0" vsi="12">
         *         </core>
         *         <core id="1" vsi="0">
         *         </core>
         *     </cores>
         * </diag>
         */

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "vsi_decision"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(curCore, "vsi", vsi_info_per_core[core]));
            }
            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);

    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

exit:
    SHR_FUNC_EXIT;
}
#endif

/******************************************************************** 
 *  Function handler: rif_info_get (section diag)
 */
int dnx_diag_rif_profile_info_info_xml_print(int unit, char *xml_file, SOC_PPC_RIF_INFO *rif_info_per_core);
cmd_result_t
    cmd_ppd_api_diag_rif_info_get(int unit, args_t* a)
{
  uint32 ret;
  SOC_PPC_RIF_INFO rif_info;
  uint32 soc_sand_dev_id, rif_index, rif_profile_index;
  int size_of_signal_in_bits;
  uint32 val[24];
  int              prm_core = -1;
  parse_table_t    pt;
  int              core, first_core, last_core;
  /*
   * For printing to xml file
   */
  char                          *prm_file = NULL;
 #if !defined(NO_FILEIO)
  SOC_PPC_RIF_INFO              rif_info_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)];
  int                           rif_info_per_core_len = 0;
#endif
  uint8                         b_print_to_xml = 0;

  SOCDNX_INIT_FUNC_DEFS;

  soc_sand_dev_id = (unit);
  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);
  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
  }
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }
  if (prm_core == -1 || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      SOC_PPC_RIF_INFO_clear(&rif_info);

      /* get last rif */
      SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(soc_sand_dev_id, core, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF, val, &size_of_signal_in_bits));

      rif_index = val[0];

      /* Get info */
      ret = soc_ppd_rif_info_get(soc_sand_dev_id, rif_index, &rif_info);
      
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK){
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 

      }
      if (!b_print_to_xml) {
          LOG_CLI((BSL_META_U(unit, "\n%s\n"),get_core_str(unit, core)));
          LOG_CLI((BSL_META_U(unit, "\nLast packet RIF used: %u\n\r"),rif_index));
          LOG_CLI((BSL_META_U(unit, "RIF Information: \n\r")));
          LOG_CLI((BSL_META_U(unit, "---------------: \n\r")));
          SOC_PPC_RIF_INFO_print(&rif_info);

          if (SOC_IS_JERICHO(unit)) {      
              SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(soc_sand_dev_id, core, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF_PROFILE, val, &size_of_signal_in_bits));
              rif_profile_index = val[0];
              if(rif_info.routing_enablers_bm_id != rif_profile_index) {
                  LOG_CLI((BSL_META_U(unit,"\t WARNING!!! rif profile signal(%d) != rif profile used(%d)!!!! \n"), rif_profile_index, rif_info.routing_enablers_bm_id));
              }
          }
      } else {
#if !defined(NO_FILEIO)
          rif_info_per_core[rif_info_per_core_len++] = rif_info;
#endif
      }

      
  }

  if (b_print_to_xml) {
      /*
      * Print to XML file
      */
#if !defined(NO_FILEIO)
      dnx_diag_rif_profile_info_info_xml_print(unit, prm_file, rif_info_per_core);          
#endif
  }

  parse_arg_eq_done(&pt);
  return CMD_OK;

exit:
  SOCDNX_FUNC_RETURN;
}

/*
 * Print last packet(s) termination information (per core) to an xml file
 */
#if !defined(NO_FILEIO)
int dnx_diag_rif_profile_info_info_xml_print(int unit, char *xml_file, SOC_PPC_RIF_INFO *rif_info_per_core){

    int core = 0;
    uint32 val[24], rif_profile_index, soc_sand_dev_id;
    xml_node curTop, curDiags, curDiag, curCores, curCore, routingEnablers, warning;
    int size_of_signal_in_bits;
    SHR_FUNC_INIT_VARS(unit);


    soc_sand_dev_id = (unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
        /*
         * Add xml structure:
         * <diag cat="pp" name="RIF_profile_info">
                <cores list="Yes">
                    <core id="0" uc_rpf_enable="1" vrf_id="0" cos_profile="0" ttl_scope_index="0" routing_enablers_bm="0" enable_default_routing="0" uc_rpf_mode="1" routing_enablers_bm_id="0" intf_class="0"/>
				    <core id="1" uc_rpf_enable="0" vrf_id="0" cos_profile="0" ttl_scope_index="1" routing_enablers_bm="31" enable_default_routing="1" uc_rpf_mode="0" routing_enablers_bm_id="1" intf_class="0">
				    <enablers>IPV4UC, IPV4MC, IPV6UC, IPV6MC, MPLS </enablers>
                </cores>
         * </diag>
         */

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "RIF_profile_info"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "uc_rpf_enable", rif_info_per_core[core].uc_rpf_enable));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "vrf_id", rif_info_per_core[core].vrf_id));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "cos_profile", rif_info_per_core[core].cos_profile));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "ttl_scope_index", rif_info_per_core[core].ttl_scope_index));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "routing_enablers_bm", rif_info_per_core[core].routing_enablers_bm));

                if (rif_info_per_core[core].routing_enablers_bm > 0) {
                    SHR_NULL_CHECK((routingEnablers = dbx_xml_child_add(curCore, "enablers", 5)), _SHR_E_FAIL, "routingEnablers");
                    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4UC_ENABLED(rif_info_per_core[core].routing_enablers_bm)){
                        dbx_xml_node_add_str(routingEnablers,"IPV4UC, ");
                    }
                    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4MC_ENABLED(rif_info_per_core[core].routing_enablers_bm)){
                        dbx_xml_node_add_str(routingEnablers,"IPV4MC, ");
                    }
                    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6UC_ENABLED(rif_info_per_core[core].routing_enablers_bm)){
                        dbx_xml_node_add_str(routingEnablers,"IPV6UC, ");      
                    }
                    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6MC_ENABLED(rif_info_per_core[core].routing_enablers_bm)){
                        dbx_xml_node_add_str(routingEnablers,"IPV6MC, ");
                    }
                    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_MPLS_ENABLED(rif_info_per_core[core].routing_enablers_bm)){
                        dbx_xml_node_add_str(routingEnablers,"MPLS ");
                    } 
                }

                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "enable_default_routing", rif_info_per_core[core].enable_default_routing));
#ifdef BCM_88660_A0
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "uc_rpf_mode", rif_info_per_core[core].uc_rpf_mode));
#endif
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "routing_enablers_bm_id", rif_info_per_core[core].routing_enablers_bm_id));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "intf_class", rif_info_per_core[core].intf_class));

                if (SOC_IS_JERICHO(unit)) {      
                  SHR_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(soc_sand_dev_id, core, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_RIF_PROFILE, val, &size_of_signal_in_bits));
                  rif_profile_index = val[0];
                  if(rif_info_per_core[core].routing_enablers_bm_id != rif_profile_index) {
                      SHR_NULL_CHECK((warning = dbx_xml_child_add(curCore, "warning", 5)), _SHR_E_FAIL, "warning");
                      SHR_IF_ERR_EXIT(dbx_xml_property_set_str(warning, "val", "rif profiles not matching"));
                      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(warning, "signal", rif_profile_index));
                      SHR_IF_ERR_EXIT(dbx_xml_property_set_int(warning, "used",  rif_info_per_core[core].routing_enablers_bm_id));
                  }
                }
                
            }
            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);

    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

exit:
    SHR_FUNC_EXIT;

}
#endif

/********************************************************************
 *  Function handler: ipv4_mc_route_get (section diag)
 */
int 
cmd_ppd_api_diag_ipv4_mc_route_get(int unit, args_t* a) 
{
    int i;
    uint32   ret;
    SOC_PPC_IP_ROUTING_TABLE_RANGE      prm_block_range_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY     prm_keys[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO    prm_routes_info[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_STATUS       prm_routes_status[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION     prm_routes_location[DIAG_PP_GET_BLOCK_NOF_ENTRIES];
    uint32                              prm_nof_entries=0, index, start_index;
    char decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
    char decimal_src_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
#if defined(BCM_JERICHO_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    uint32 is_l3_mc_in_tcam = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mc_use_tcam", 0) > 0);
#endif

    cli_out(" ----------------------------------------------------------------------------------\n\r"
            "|                                    Routing Table                                 |\n\r"
            "|----------------------------------------------------------------------------------|\n\r"
            "|     Group       | Inrif |      Source        |  Fec   |   Status    |  Location  |\n\r"
            "|                 |       |                    |        |             |            |\n\r");
    cli_out(" ---------------------------------------------------------------------------------- \n\r");

    /* 
     * Need to traverse for public and private DBs separately.
     * In case of ARAD/+, iterate only once (no KAPS) 
     */
#if defined(BCM_JERICHO_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (JER_KAPS_ENABLE(unit) && (is_l3_mc_in_tcam == 0)) {
        start_index = SOC_IS_ARADPLUS_AND_BELOW(unit)?1:JER_KAPS_ENABLE_PUBLIC_DB(unit)?0:1;
    }
    else 
#endif
    {
        start_index = 1;
    }
    for (index = start_index; index < 2 ; index++) {
        SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&prm_block_range_key);
        prm_block_range_key.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED;

        prm_block_range_key.entries_to_act =  DIAG_PP_GET_BLOCK_NOF_ENTRIES;
        prm_block_range_key.entries_to_scan = 64*1024;
         
        while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&prm_block_range_key.start.payload))
        {
            prm_nof_entries = 0;
            /* set VRF indication according to public/private DB */
            /* coverity explanation: coverity has found that index can't be equal to zero.
               However, start_index may be set to 1 under certain circumstances (compilation falgs etc.)
               and then set index value accordingly. */
            /* coverity[dead_error_line:FALSE] */
            prm_block_range_key.start.payload.arr[1] = index==0?0:-1;
            /* Call function */
            ret = soc_ppd_frwrd_ipv4_mc_route_get_block(unit,
                                                       &prm_block_range_key,
                                                        prm_keys,
                                                        prm_routes_info,
                                                        prm_routes_status,
                                                        prm_routes_location,
                                                       &prm_nof_entries);

            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
            { 
                return CMD_FAIL;
            } 

            for(i = 0; i < prm_nof_entries; i++)
            {
                soc_sand_pp_ip_long_to_string(prm_keys[i].group, 1,decimal_ip);
                cli_out("| %-16s", decimal_ip);

                if(prm_keys[i].inrif_valid)
                    cli_out("| %-6d", prm_keys[i].inrif);
                else
                    cli_out("| %-6c", '-');

                soc_sand_pp_ip_long_to_string(prm_keys[i].source.ip_address, 1,decimal_src_ip);
                sal_sprintf(decimal_src_ip,"%1s/%-1u",decimal_src_ip,prm_keys[i].source.prefix_len);

                cli_out("| %-18s | %-6d | %-11s | %7s    |\n\r",
                        decimal_src_ip, prm_routes_info[i].dest_id.dest_val,
                        SOC_PPC_FRWRD_IP_ROUTE_STATUS_to_string(prm_routes_status[i]),
                        SOC_PPC_FRWRD_IP_ROUTE_LOCATION_to_string(prm_routes_location[i]));
            }
        }
    }
    cli_out(" ---------------------------------------------------------------------------------- \n\r");
    return CMD_OK;
} 

cmd_result_t
    cmd_ppd_api_diag_lif_show(int unit, args_t* a)
{
#ifndef __KERNEL__
    char *lif_type;
    int strnlen_lif_type;
    parse_table_t    pt;
    int lif_id = -1;
    int global_flag = 0;
    int internal_offset, bank_id;
    int32 params_num;
    int llif_id, glif_id = -1;
    uint32 ret;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    char *table_type = NULL;
    uint8 is_ingress;

    parse_table_init(unit, &pt);
    /* User may omit type - "in" is default */
    parse_table_add(&pt, "type", PQ_STRING, (void *) "in", (void *)&lif_type, NULL);
    parse_table_add(&pt, "id",   PQ_DFL | PQ_INT, &lif_id,      &lif_id,      NULL);
    parse_table_add(&pt, "gl",   PQ_DFL | PQ_INT, &global_flag, &global_flag, NULL);

    params_num = parse_arg_eq(a, &pt);
    if (0 > params_num) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    /* No default for lif id */
    if(lif_id == -1) {
        cli_out("lif id was not specified\n");
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    strnlen_lif_type = sal_strnlen(lif_type,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
    if(!sal_strncasecmp(lif_type, "in", strnlen_lif_type)) {
        /* Normalize string for below print */
        sal_strcpy(lif_type, "In");
        is_ingress = TRUE;
        if(global_flag == 1) {
            glif_id = lif_id;
            if((ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                          _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, lif_id, &llif_id)) != BCM_E_NONE) {
                cli_out("Global %s_LIF:0x%x does not exist\n", lif_type, lif_id);
                parse_arg_eq_done(&pt);
                return ret;
            }
        }
        else
            llif_id = lif_id;

        if((llif_id < 0) || (llif_id >= SOC_DPP_DEFS_GET(unit, nof_local_lifs))) {
            cli_out("Local %s_LIF:%d is out of range\n", lif_type, llif_id);
            parse_arg_eq_done(&pt);
            return CMD_OK;
        }

        internal_offset = ARAD_PP_IN_LIF_TO_OFFSET_IN_BANK(unit, llif_id);
        bank_id         = ARAD_PP_IN_LIF_TO_BANK_ID(unit, llif_id);
    }
    else if(!sal_strncasecmp(lif_type, "out", strnlen_lif_type)) {
        int entry;

        /* Normalize string for below print */
        sal_strcpy(lif_type, "Out");
        is_ingress = FALSE;
        if(global_flag == 1) {
            glif_id = lif_id;
            if((ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                          _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, lif_id, &llif_id)) != BCM_E_NONE) {
                cli_out("Global %s_LIF:0x%x does not exist\n", lif_type, lif_id);
                parse_arg_eq_done(&pt);
                return ret;
            }
        }
        else
            llif_id = lif_id;

        if((llif_id < 0) || (llif_id >= SOC_DPP_DEFS_GET(unit, nof_out_lifs))) {
            cli_out("Local %s_LIF:%d is out of range(%d)\n", lif_type, llif_id, SOC_DPP_DEFS_GET(unit, nof_out_lifs));
            parse_arg_eq_done(&pt);
            return CMD_OK;
        }

        entry           = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(unit, llif_id);
        internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry);
        bank_id         = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry);
    }
    else {
        cli_out("No LIF type:\"%s\"\n", lif_type);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if(glif_id != -1)
        cli_out("Global %s_LIF:0x%08x -> ", lif_type, glif_id);
    cli_out("Local %s_LIF:0x%08x -> Type:%s Bank:%d Offset:%d\n\n", lif_type, llif_id, table_type, bank_id, internal_offset);

    if((bank_id < 0) || (internal_offset < 0)) {
        cli_out("Bad entry parameters - bank_id:%d internal_offset:%d\n", bank_id, internal_offset);
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    /* Get and print the entry. */
    if (is_ingress) {
        /* Straightforward, just use the dedicated functions. */
        ret = soc_ppd_lif_table_entry_get(unit, llif_id, &lif_entry_info);
        if(SOC_SAND_FAILURE(ret)) {
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        SOC_PPC_LIF_ENTRY_INFO_print(&lif_entry_info);
    } else {
        _bcm_lif_type_e bcm_outlif_type;
        uint32 next_eep[1], nof_entries;
        SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[1];
        SOC_PPC_EG_ENCAP_EEP_TYPE soc_ppc_outlif_type;

        /* We need some metadata to get lif information. Get bcm lif type, and decide soc ppc lif type accordingly. */
        ret = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, _BCM_GPORT_ENCAP_ID_LIF_INVALID, llif_id, NULL, &bcm_outlif_type);
        if (ret != BCM_E_NONE) {
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        soc_ppc_outlif_type = (bcm_outlif_type == _bcmDppLifTypeMplsPort) ? SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP : SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP;
        ret = soc_ppd_eg_encap_entry_get(unit, soc_ppc_outlif_type, llif_id, 0/*depth*/, encap_entry_info, next_eep, &nof_entries);
        if(SOC_SAND_FAILURE(ret)) {
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        SOC_PPC_EG_ENCAP_ENTRY_INFO_print(unit, encap_entry_info);
        cli_out("Next_eep: 0x%x\n", next_eep[0]);
    }

    parse_arg_eq_done(&pt);
#else
    cli_out("This function is unavailable in Kernel mode\n");
#endif /* __KERNEL__ */
    return CMD_OK;
}

#define CMD_DIAG_PP_DEST_VALID 0x1L
#define CMD_DIAG_PP_TC_VALID 0x2L
#define CMD_DIAG_PP_DP_VALID 0x4L
#define CMD_DIAG_PP_METER_0_VALID 0x8L
#define CMD_DIAG_PP_METER_1_VALID 0x10L
#define CMD_DIAG_PP_DP_METER_CMD_VALID 0x20L
#define CMD_DIAG_PP_COUNTER_0_VALID 0x40L
#define CMD_DIAG_PP_COUNTER_1_VALID 0x80L
#define CMD_DIAG_PP_SNOOP_VALID 0x100L
#define CMD_DIAG_PP_MIRROR_VALID 0x200L
#define CMD_DIAG_PP_TRAP_VALID 0x400L

#define CMD_DIAG_PP_PACKET_ACTION_NUM_FIELDS 11

#define DECLARE_PACKET_ACTION_DESC() \
    char *packet_action_desc[] = { \
        "destination", \
        "TC", \
        "DP", \
        "Meter 0", \
        "Meter 1", \
        "DP meter cmd", \
        "Counter 0", \
        "Counter 1", \
        "Snoop action", \
        "Mirror profile", \
        "Trap code" }

#define PACKET_ACTION_DESC(i) packet_action_desc[i]

typedef struct cmd_diag_pp_packet_action_e {
    /* do not change the following section */
    uint32 dest;
    uint32 tc;
    uint32 dp;
    uint32 meter0;
    uint32 meter1;
    uint32 dp_meter_cmd;
    uint32 counter0;
    uint32 counter1;
    uint32 snoop_code;
    uint32 mirror;
    uint32 trap;
    /* do not change the section above */
    SOC_PPC_FRWRD_DECISION_TYPE dest_type;
    uint32 flags;
    uint32 lag_lb_key;
    uint32 snoop_cmd;
} cmd_diag_pp_packet_action_t;

#define DECLARE_IRPP_STAGE_DESC() \
    char *irpp_stage_desc[] = { \
        "port termination", \
        "parser", \
        "vtt", \
        "vtt", \
        "flp", \
        "pmf", \
        "fec resolution", \
        "tm" }

#define IRPP_STAGE_DESC(i) irpp_stage_desc[i]

typedef enum {
    diag_irpp_stage_port_termination,  
    diag_irpp_stage_parser,  
    diag_irpp_stage_vt,  
    diag_irpp_stage_tt,  
    diag_irpp_stage_flp,  
    diag_irpp_stage_pmf,  
    diag_irpp_stage_fec,  
    diag_irpp_stage_tm,  
    diag_irpp_stages  
} cmd_diag_pp_irpp_stage_e;

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_port_termination(int unit, int core)
{
    uint32 ret;
    
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO    
      prm_rcvd_pkt_info;
    SOC_PPC_DIAG_RESULT  ret_val;


    ret = soc_ppd_diag_received_packet_info_get(
            unit,
            core,
            &prm_rcvd_pkt_info,
            &ret_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    }
    if (ret_val != SOC_PPC_DIAG_OK) 
    {
         return CMD_FAIL; 
    }  

    /* port termination */
    LOG_CLI((BSL_META_U(unit, "    |                            \033[1mPort Termination\033[0m                             |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |       TM Port       |         PP Port         |       System Port       |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    if (prm_rcvd_pkt_info.src_sys_port.sys_port_type != SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT) {
        LOG_CLI((BSL_META_U(unit, "    |         %3d         |           %3d           |         LAG %4d        |\n\r"), 
                            prm_rcvd_pkt_info.in_tm_port, 
                            prm_rcvd_pkt_info.in_pp_port, 
                            prm_rcvd_pkt_info.src_sys_port.sys_id));
    }
    else {
        LOG_CLI((BSL_META_U(unit, "    |         %3d         |           %3d           |       %4d (0x%03X)      |\n\r"), 
                            prm_rcvd_pkt_info.in_tm_port, 
                            prm_rcvd_pkt_info.in_pp_port, 
                            prm_rcvd_pkt_info.src_sys_port.sys_id,
                            prm_rcvd_pkt_info.src_sys_port.sys_id));
    }

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    return CMD_OK;
}

STATIC const char *
    cmd_ppd_api_diag_pp_last_trap_code_desc(uint32 trap_code)
{
    static const char *trap_desc[] = {
		/* 0 */
		"PBP_SA_DROP_0",
		"PBP_SA_DROP1",
		"PBP_SA_DROP2",
		"PBP_SA_DROP3",
		"PBP_TE_TRANSPLANT",
		"PBP_TE_UNKNOWN_TUNNEL",
		"PBP_TRANSPLANT",
		"PBP_LEARN_SNOOP",
		"SA_AUTHENTICATION_FAILED",
		"PORT_NOT_PERMITTED",
		"UNEXPECTED_VID",
		"SA_MULTICAST",
		"SA_EQUALS_DA",
		"8021x",
		"ACCEPTABLE_FRAME_TYPE0",
		"ACCEPTABLE_FRAME_TYPE1",

		/* 0x10 */
		"ACCEPTABLE_FRAME_TYPE2",
		"ACCEPTABLE_FRAME_TYPE3",
		"MY_ARP",
		"ARP",
		"IGMP_MEMBERSHIP_QUERY",
		"IGMP_REPORT_LEAVE_MSG",
		"IGMP_UNDEFINED",
		"ICMP_V6_MLD_MC_LISTENER_QUERY",
		"RESERVED_MC_PREFIX_0",
		"RESERVED_MC_PREFIX_1",
		"RESERVED_MC_PREFIX_2",
		"RESERVED_MC_PREFIX_3",
		"RESERVED_MC_PREFIX_4",
		"RESERVED_MC_PREFIX_5",
		"RESERVED_MC_PREFIX_6",
		"RESERVED_MC_PREFIX_7",
		
		/* 0x20 */
		"ICMPV6_MLD_REPORT_DONE_MSG",
		"ICMPV6_MLD_UNDEFINED",
		"DHCP_SERVER",
		"DHCP_CLIENT",
		"DHCPV6_SERVER",
		"DHCPV6_CLIENT",
		"GENERAL_TRAP0",
		"GENERAL_TRAP1",
		"GENERAL_TRAP2",
		"GENERAL_TRAP3",
		"(No Desc)", /* 42 */
		"(No Desc)", /* 43 */
		"(No Desc)", /* 44 */
		"(No Desc)", /* 45 */
		"(No Desc)", /* 46 */
		"(No Desc)", /* 47 */
		
		/* 0x30 */
		"PORT_NOT_VLAN_MEMBER",
		"HEADER_SIZE_ERR",
		"MY_B_MAC_AND_LEARN_NULL",
		"MY_B_DA_UNKNOWN_I_SID",
		"MY_MAC_AND_IP_DISABLE",
		"MY_MAC_AND_MPLS_DISABLE",
		"MY_MAC_AND_ARP",
		"MY_MAC_AND_UNKNOWN_L3",
		"IP_COMP_MC_INVALID_IP",
		"TRILL_VERSION",
		"TRILL_INVALID_TTL",
		"TRILL_CHBH",
		"TRILL_NO_REVERSE_FEC",
		"TRILL_CITE",
		"TRILL_ILLEGAL_INNER_MC",
		"TRILL_DISABLE_BRIDGE_IF_DESIGNATED",
		
		/* 0x40 */
		"MPLS_INVALID_LIF_ENTRY",
		"MPLS_UNEXPECTED_BOS",
		"MPLS_UNEXPECTED_NO_BOS",
		"VTT_MPLS_TTL_0",
		"VTT_MPLS_TTL_1",
		"MPLS_CONTROL_WORD_TRAP",
		"MPLS_CONTROL_WORD_DROP",
		"STP_STATE_BLOCK",
		"STP_STATE_LEARN",
		"ILLEGAL_PFC",
		"IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED",
		"ETHER_IP_VERSION_ERROR",
		"PROTECTION_PATH_INVALID",
		"TRILL_NO_ADJACENT",
		"MPLS_TERMINATION_ERROR",
		"MY_B_DA_MC_CONTINUE",
		
		/* 0x50 */
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		"(No Desc)",
		
		/* 0x60 */
		"ETH_L2CP_PEER",
		"ETH_L2CP_DROP",
		"ETH_FL_IGMP_MEMBERSHIP_QUERY",
		"ETH_FL_IGMP_REPORT_LEAVE_MSG",
		"ETH_FL_IGMP_UNDEFINED",
		"ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY",
		"ETH_FL_ICMPV6_MLD_REPORT_MODE",
		"ETH_FL_ICMPV6_MLD_UNDEFINED",
		"IPV4_VERSION_ERROR",
		"IPV4_CHECKSUM_ERROR",
		"_IPV4_HEADER_LENGTH_ERROR",
		"IPV4_TOTAL_LENGTH_ERROR",
		"IPV4_TTL0",
		"IPV4_HAS_OPTIONS",
		"IPV4_TTL",
		"IPV4_SIP_EQUAL_DIP",

		/* 0x70 */
		"IPV4_DIP_ZERO",
		"IPV4_SIP_IS_MC",
		"IPV6_VERSION_ERROR",
		"IPV6_HOP_COUNT0",
		"IPV6_HOP_COUNT1",
		"IPV6_UNSPECIFIED_DESTINATION",
		"IPV6_LOOPBACK_ADDRESS",
		"IPV6_MULTICAST_SOURCE",
		"IPV6_NEXT_HEADER_NULL",
		"IPV6_UNSPECIFIED_SOURCE",
		"IPV6_LOCAL_LINK_DESTINATION",
		"IPV6_LOCAL_SITE_DESTINATION",
		"IPV6_LOCAL_LINK_SOURCE",
		"IPV6_LOCAL_SITE_SOURCE",
		"IPV6_IPV4_COMPATIBLE_DESTINATION",
		"IPV6_IPV4_MAPPED_DESTINATION",

		/* 0x80 */
		"IPV6_MULTICAST_DESTINATION",
		"MPLS_TTL0",
		"MPLS_TTL1",
		"TCP_SN_FLAGS_ZERO",
		"TCP_SN_ZERO_FLAGS_SET",
		"TCP_SYN_FIN",
		"TCP_EQUAL_PORTS",
		"TCP_FRAGMENT_INCOMPLETE_HEADER",
		"TCP_FRAGMENT_OFFSET_LT8",
		"UDP_EQUAL_PORTS",
		"ICMP_DATA_GT_576",
		"ICMP_FRAGMENTED",
		"SA_DROP0",
		"SA_DROP1",
		"SA_DROP2",
		"SA_DROP3",

		/* 0x90 */
		"SA_NOT_FOUND0",
		"SA_NOT_FOUND1",
		"SA_NOT_FOUND2",
		"SA_NOT_FOUND3",
		"DA_NOT_FOUND0",
		"DA_NOT_FOUND1",
		"DA_NOT_FOUND2",
		"DA_NOT_FOUND3",
		"IEEE_1588_PREFIX",
		"1588_DEFAULT_DROP", /* 153 */
		"1588_DEFAULT_FORWARD", /* 154 */
		"(No Desc)", /* 155 */
		"(No Desc)", /* 156 */
		"(No Desc)", /* 157 */
		"(No Desc)", /* 158 */
		"(No Desc)", /* 159 */

		/* 0xA0 */
		"ELK_ERROR",
		"ELK_REJECTED",
		"(No Desc)", /* 162 */
		"SAME_INTERFACE",
		"(No Desc)", /* 164 */
		"(No Desc)", /* 165 */
		"UC_LOOSE_RPF_FAIL",
		"(No Desc)", /* 167 */
		"(No Desc)", /* 168 */
		"MPLS_P2P_NO_BOS",
		"MPLS_CONTROL_WORD_TRAP",
		"MPLS_CONTROL_WORD_DROP",
		"MPLS_UNKNOWN_LABEL",
		"MPLS_P2P_MPLSX4",
		"FABRIC_PROVIDED_SECURITY",
		"(No Desc)", /* 175 */

		/* 0xB0 */
		"(No Desc)", /* 176 */
		"(No Desc)", /* 177 */
		"(No Desc)", /* 178 */
		"(No Desc)", /* 179 */
		"(No Desc)", /* 180 */
		"(No Desc)", /* 181 */
		"(No Desc)", /* 182 */
		"(No Desc)", /* 183 */
		"(No Desc)", /* 184 */
		"(No Desc)", /* 185 */
		"(No Desc)", /* 186 */
		"(No Desc)", /* 187 */
		"(No Desc)", /* 188 */
		"(No Desc)", /* 189 */
		"(No Desc)", /* 190 */
		"(No Desc)", /* 191 */

		/*0xC0 */
		"FACILITY_INVALID",
		"FEC_ENTRY_ACCESSED",
		"UC_STRICT_RPF_FAIL",
		"MC_EXPLICIT_RPF_FAIL",
		"MC_USE_SIP_RPF_FAIL",
		"SIP_TRANSPLANT_DETECTION",
		"ICMP_REDIRECT",
		"(No Desc)", /* 199 */
		"(No Desc)", /* 200 */
		"(No Desc)", /* 201 */
		"(No Desc)", /* 202 */
		"(No Desc)", /* 203 */
		"(No Desc)", /* 204 */
		"EGRESS_MIRRORING_DROP", /* 205 */
		"(No Desc)", /* 206 */
		"IPV6_DISCARD", /* 207 */

		/* 0xD0 */
		"(No Desc)", /* 208 */
		"(No Desc)", /* 209 */
		"(No Desc)", /* 210 */
		"(No Desc)", /* 211 */
		"(No Desc)", /* 212 */
		"(No Desc)", /* 213 */
		"(No Desc)", /* 214 */
		"(No Desc)", /* 215 */
		"(No Desc)", /* 216 */
		"(No Desc)", /* 217 */
		"(No Desc)", /* 218 */
		"(No Desc)", /* 219 */
		"(No Desc)", /* 220 */
		"(No Desc)", /* 221 */
		"(No Desc)", /* 222 */
		"(No Desc)", /* 223 */

        /* 0xE0 */
		"ETH_OAM", /* 224 */
		"Y1731_O_MPLSTP", /* 225 */
		"Y1731_O_PWE", /* 226 */
		"BFD_O_IPV4", /* 227 */
		"BFD_O_MPLS", /* 228 */
		"BFD_O_PWE", /* 229 */
		"BFDCC_O_MPLSTP",
		"BFDCV_O_MPLSTP",
		"IPV4_DISCARD", /* 232 */
		"BFD_ECHO", /* 233 */
		"IPV6_UNICAST_RPF_TWO_PASS", /* 234 */
		"(No Desc)", /* 235 */
		"OAM_CPU_MIRROR", /* 236 */
		"OAMP_MIRROR_0", /* 237 */
		"OAMP_MIRROR_1", /* 238 */
		"OAMP_MIRROR_2", /* 239 */

        /* 0xF0 */
		"OAMP",
		"ETHERNET_OAM_ACCELERATED",
		"MPLS_OAM_ACCELERATED",
		"BFD_IP_OAM_TUNNEL_ACCELERATED",
		"BFD_PWE_OAM_ACCELERATED",
		"ETHERNET_OAM_UP_ACCELERATED",
		"OAM_CPU_UP", /* 246 */
		"PORT_DEFAULT_TRAP", /* 247 */
		"BLOCK_TRAP_VTT_VT", 
		"BLOCK_TRAP_VTT_TT",
		"BLOCK_TRAP_FLP",
		"BLOCK_TRAP_PMF_1ST_PASS",
		"BLOCK_TRAP_PMF_2ND_PASS",
		"(No Desc)", /* 253 */
		"(No Desc)", /* 254 */
		"(No Desc)" /* 255 */
    	};
	static const char *invalid_trap_desc = "Invalid Trap Code";

	if (trap_code >= 256) {
		return invalid_trap_desc;
	}

	assert(sizeof(trap_desc)/sizeof(trap_desc[0]) == 256);

	return trap_desc[trap_code % 256];
}

STATIC const char *
    cmd_ppd_api_diag_pp_last_trap_code_to_bcm_trap(uint32 trap_code)
{
    static const char *trap_desc[] = {
		/* 0 */
		"bcmRxTrapMimDiscardMacsaFwd",
		"bcmRxTrapMimDiscardMacsaDrop",
		"bcmRxTrapMimDiscardMacsaTrap",
		"bcmRxTrapMimDiscardMacsaSnoop",
		"bcmRxTrapMimTeSaMove",
		"bcmRxTrapMimTeSaUnknown",
		"bcmRxTrapMimSaMove",
		"bcmRxTrapMimSaUnknown",
		"bcmRxTrapAuthSaLookupFail",
		"bcmRxTrapAuthSaPortFail",
		"bcmRxTrapAuthSaVlanFail",
		"bcmRxTrapSaMulticast",
		"bcmRxTrapSaEqualsDa",
		"bcmRxTrap8021xFail",
		"(No Bcm Trap)",
		"(No Bcm Trap)",

		/* 0x10 */
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"bcmRxTrapArpMyIp",
		"bcmRxTrapArp",
		"bcmRxTrapIgmpMembershipQuery",
		"bcmRxTrapIgmpReportLeaveMsg",
		"bcmRxTrapIgmpUndefined",
		"bcmRxTrapIcmpv6MldMcListenerQuery",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		
		/* 0x20 */
		"bcmRxTrapIcmpv6MldReportDone",
		"bcmRxTrapIcmpv6MldUndefined",
		"bcmRxTrapDhcpv4Server",
		"bcmRxTrapDhcpv4Client",
		"bcmRxTrapDhcpv6Server",
		"bcmRxTrapDhcpv6Client",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"bcmRxTrapL2Cache",
		"(No Bcm Trap)", /* 42 */
		"(No Bcm Trap)", /* 43 */
		"(No Bcm Trap)", /* 44 */
		"(No Bcm Trap)", /* 45 */
		"(No Bcm Trap)", /* 46 */
		"(No Bcm Trap)", /* 47 */
		
		/* 0x30 */
		"bcmRxTrapPortNotVlanMember",
		"bcmRxTrapHeaderSizeErr",
		"bcmRxTrapMyBmacUknownTunnel",
		"bcmRxTrapMyBmacUnknownISid",
		"bcmRxTrapMyMacAndIpDisabled",
		"bcmRxTrapMyMacAndMplsDisable",
		"bcmRxTrapArpReply",
		"bcmRxTrapMyMacAndUnknownL3",
		"bcmRxTrapIpCompMcInvalidIp",
		"bcmRxTrapTrillVersion",
		"bcmRxTrapTrillTtl0",
		"bcmRxTrapTrillChbh",
		"bcmRxTrapTrillUnknonwnIngressNickName",
		"bcmRxTrapTrillCite",
		"bcmRxTrapTrillIllegalInnerMc",
		"bcmRxTrapTrillDesignatedVlanNoMymac",
		
		/* 0x40 */
		"(No Bcm Trap)",
		"bcmRxTrapMplsUnexpectedBos",
		"bcmRxTrapMplsUnexpectedNoBos",
		"bcmRxTrapMplsTtl0",
		"bcmRxTrapMplsTtl1",
		"bcmRxTrapMplsControlWordTrap",
		"bcmRxTrapMplsControlWordDrop",
		"bcmRxTrapStpStateBlock",
		"bcmRxTrapStpStateLearn",
		"bcmRxTrapIllegelPacketFormat",
		"bcmRxTrapIpv4TunnelTerminationAndFragment",
		"bcmRxTrapEtherIpVersion",
		"bcmRxTrapFailover1Plus1Fail",
		"bcmRxTrapAdjacentCheckFail",
		"(No Bcm Trap)",
		"bcmRxTrapMimMyBmacMulticastContinue",
		
		/* 0x50 */
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		
		/* 0x60 */
		"bcmRxTrapL2cpPeer",
		"bcmRxTrapL2cpDrop",
		"bcmRxTrapFrwrdIgmpMembershipQuery",
		"bcmRxTrapFrwrdIgmpReportLeaveMsg",
		"bcmRxTrapFrwrdIgmpUndefined",
		"bcmRxTrapFrwrdIcmpv6MldMcListenerQuery",
		"bcmRxTrapFrwrdIcmpv6MldReportDone",
		"bcmRxTrapFrwrdIcmpv6MldUndefined",
		"bcmRxTrapIpv4VersionError",
		"bcmRxTrapIpv4ChecksumError",
		"bcmRxTrapIpv4HeaderLengthError",
		"bcmRxTrapIpv4TotalLengthError",
		"bcmRxTrapIpv4Ttl0",
		"bcmRxTrapIpv4HasOptions",
		"bcmRxTrapIpv4Ttl1",
		"bcmRxTrapIpv4SipEqualDip",

		/* 0x70 */
		"bcmRxTrapIpv4DipZero",
		"bcmRxTrapIpv4SipIsMc",
		"bcmRxTrapIpv6VersionError",
		"bcmRxTrapIpv6HopCount0",
		"bcmRxTrapIpv6HopCount1",
		"bcmRxTrapIpv6UnspecifiedDestination",
		"bcmRxTrapIpv6LoopbackAddress",
		"bcmRxTrapIpv6MulticastSource",
		"bcmRxTrapIpv6NextHeaderNull",
		"bcmRxTrapIpv6UnspecifiedSource",
		"bcmRxTrapIpv6LocalLinkDestination",
		"bcmRxTrapIpv6LocalSiteDestination",
		"bcmRxTrapIpv6LocalLinkSource",
		"bcmRxTrapIpv6LocalSiteSource",
		"bcmRxTrapIpv6Ipv4CompatibleDestination",
		"bcmRxTrapIpv6Ipv4MappedDestination",

		/* 0x80 */
		"bcmRxTrapIpv6MulticastDestination",
		"bcmRxTrapMplsTtl0",
		"bcmRxTrapMplsTtl1",
		"bcmRxTrapTcpSnFlagsZero",
		"bcmRxTrapTcpSnZeroFlagsSet",
		"bcmRxTrapTcpSynFin",
		"bcmRxTrapTcpEqualPorts",
		"bcmRxTrapTcpFragmentIncompleteHeader",
		"bcmRxTrapTcpFragmentOffsetLt8",
		"bcmRxTrapUdpEqualPorts",
		"bcmRxTrapIcmpDataGt576",
		"bcmRxTrapIcmpFragmented",
		"bcmRxTrapL2DiscardMacsaFwd",
		"bcmRxTrapL2DiscardMacsaDrop",
		"bcmRxTrapL2DiscardMacsaTrap",
		"bcmRxTrapL2DiscardMacsaSnoop",

		/* 0x90 */
		"bcmRxTrapL2Learn0",
		"bcmRxTrapL2Learn1",
		"bcmRxTrapL2Learn2",
		"bcmRxTrapL2Learn3",
		"bcmRxTrapL2DlfFwd",
		"bcmRxTrapL2DlfDrop",
		"bcmRxTrapL2DlfTrp",
		"bcmRxTrapL2DlfSnoop",
		"bcmRxTrap1588",
		"(No Bcm Trap)", /* 153 */
		"(No Bcm Trap)", /* 154 */
		"(No Bcm Trap)", /* 155 */
		"(No Bcm Trap)", /* 156 */
		"(No Bcm Trap)", /* 157 */
		"(No Bcm Trap)", /* 158 */
		"(No Bcm Trap)", /* 159 */

		/* 0xA0 */
		"bcmRxTrapExternalLookupError",
		"(No Bcm Trap)",
		"bcmRxTrapOamLevel", /* 162 */
		"bcmRxTrapSameInterface",
		"(No Bcm Trap)", /* 164 */
		"(No Bcm Trap)", /* 165 */
		"bcmRxTrapUcLooseRpfFail",
		"(No Bcm Trap)", /* 167 */
		"(No Bcm Trap)", /* 168 */
		"bcmRxTrapMplsP2pNoBos",
		"bcmRxTrapMplsControlWordTrap",
		"bcmRxTrapMplsControlWordDrop",
		"bcmRxTrapOamPassive/MplsUnknownLabel",
		"bcmRxTrapMplsExtendP2pMplsx4",
		"bcmRxTrapFcoeSrcIdMismatchSa",
		"bcmRxTrapDroppedPacket", /* 175 */

		/* 0xB0 */
		"(No Bcm Trap)", /* 176 */
		"(No Bcm Trap)", /* 177 */
		"(No Bcm Trap)", /* 178 */
		"(No Bcm Trap)", /* 179 */
		"(No Bcm Trap)", /* 180 */
		"(No Bcm Trap)", /* 181 */
		"(No Bcm Trap)", /* 182 */
		"(No Bcm Trap)", /* 183 */
		"(No Bcm Trap)", /* 184 */
		"(No Bcm Trap)", /* 185 */
		"(No Bcm Trap)", /* 186 */
		"(No Bcm Trap)", /* 187 */
		"(No Bcm Trap)", /* 188 */
		"(No Bcm Trap)", /* 189 */
		"bcmRxTrapFcoeFcfPacket", /* 190 */
		"(No Bcm Trap)", /* 191 */

		/*0xC0 */
		"bcmRxTrapFailoverFacilityInvalid",
		"bcmRxTrapEgressObjectAccessed",
		"bcmRxTrapUcStrictRpfFail",
		"bcmRxTrapMcExplicitRpfFail",
		"bcmRxTrapMcUseSipRpfFail",
		"(No Bcm Trap)",
		"bcmRxTrapIcmpRedirect",
		"(No Bcm Trap)", /* 199 */
		"(No Bcm Trap)", /* 200 */
		"(No Bcm Trap)", /* 201 */
		"bcmRxTrapSat0", /* 202 */
		"bcmRxTrapSat1", /* 203 */
		"bcmRxTrapSat2", /* 204 */
		"(No Bcm Trap)", /* 205 */
		"bcmRxTrapOamBfdIpv6", /* 206 */
		"(No Bcm Trap)", /* 207 */

		/* 0xD0 */
		"(No Bcm Trap)", /* 208 */
		"(No Bcm Trap)", /* 209 */
		"(No Bcm Trap)", /* 210 */
		"(No Bcm Trap)", /* 211 */
		"(No Bcm Trap)", /* 212 */
		"(No Bcm Trap)", /* 213 */
		"(No Bcm Trap)", /* 214 */
		"(No Bcm Trap)", /* 215 */
		"(No Bcm Trap)", /* 216 */
		"(No Bcm Trap)", /* 217 */
		"(No Bcm Trap)", /* 218 */
		"(No Bcm Trap)", /* 219 */
		"(No Bcm Trap)", /* 220 */
		"(No Bcm Trap)", /* 221 */
		"(No Bcm Trap)", /* 222 */
		"(No Bcm Trap)", /* 223 */

        /* 0xE0 */
		"bcmRxTrapOamEthAccelerated", /* 224 */
		"bcmRxTrapOamY1731MplsTp", /* 225 */
		"bcmRxTrapOamY1731Pwe", /* 226 */
		"bcmRxTrapOamBfdIpv4", /* 227 */
		"bcmRxTrapOamBfdMpls", /* 228 */
		"bcmRxTrapOamBfdPwe", /* 229 */
		"bcmRxTrapOamBfdCcMplsTp",
		"bcmRxTrapOamBfdCvMplsTp",
		"(No Bcm Trap)", /* 232 */
		"(No Bcm Trap)", /* 233 */
		"(No Bcm Trap)", /* 234 */
		"bcmRxTrapOamMipSnoop2ndPass", /* 235 */
		"(No Bcm Trap)", /* 236 */
		"(No Bcm Trap)", /* 237 */
		"(No Bcm Trap)", /* 238 */
		"(No Bcm Trap)", /* 239 */

        /* 0xF0 */
		"bcmRxTrapOamp",
		"(No Bcm Trap)",
		"bcmRxTrapOamMplsAccelerated",
		"bcmRxTrapOamBfdIpTunnelAccelerated",
		"bcmRxTrapOamBfdPweAccelerated",
		"bcmRxTrapOamEthUpAccelerated",
		"(No Bcm Trap)", /* 246 */
		"bcmRxTrapRedirectToCpuPacket", /* 247 */
		"(No Bcm Trap)", 
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"(No Bcm Trap)",
		"bcmRxTrapRedirectToCpuOamPacket", /* 253 */
		"bcmRxTrapSnoopOamPacket", /* 254 */
		"bcmRxTrapRecycleOamPacke" /* 255 */
    	};
	static const char *invalid_trap_desc = "Invalid Trap Code";

	if (trap_code >= 256) {
		return invalid_trap_desc;
	}

	assert(sizeof(trap_desc)/sizeof(trap_desc[0]) == 256);

	return trap_desc[trap_code % 256];
}

/* To string??? Print a 19bits encoded destination, the width is 18 characters */
STATIC cmd_result_t
    cmd_ppd_api_diag_pp_destination_19bits_print(int unit, uint32 destination)
{
    uint8 prefix;

    
    if (destination == (SOC_IS_JERICHO(unit)? 0x7FFFF: 0x3FFFF) /* ARAD_PP_DIAG_FRWRD_DECISION_DEST_DROP_CODE */ ) {
        LOG_CLI((BSL_META_U(unit, "             DROP ")));
        return CMD_OK;
    }

    prefix = SOC_SAND_GET_BITS_RANGE(destination, 18, 17);

    switch (prefix) {
        case 3: /* 2'b11, flow ID */           
            LOG_CLI((BSL_META_U(unit, "Flow ID     %6d"), SOC_SAND_GET_BITS_RANGE(destination, 16, 0)));
            break;
        case 2: /* 2'b10, MC ID */           
            LOG_CLI((BSL_META_U(unit, "MC group    %6d"), SOC_SAND_GET_BITS_RANGE(destination, 16, 0)));
            break;
        case 1: /* 2'b01, FEC pointer */           
            LOG_CLI((BSL_META_U(unit, "FEC pointer %6d"), SOC_SAND_GET_BITS_RANGE(destination, 16, 0)));
            break;
        case 0: /* 2'b00, system port */
            {
                uint8 system_port_prefix;

                system_port_prefix = SOC_SAND_GET_BITS_RANGE(destination, 16, 15);

                switch (system_port_prefix) {
                    case 3: /* 2'b11, LAG */
                        LOG_CLI((BSL_META_U(unit, "LAG     0x%08x"), SOC_SAND_GET_BITS_RANGE(destination, 14, 0)));
                        break;
                    case 2: /* 2'b10, physical port */
                        LOG_CLI((BSL_META_U(unit, "Port        %6d"), SOC_SAND_GET_BITS_RANGE(destination, 14, 0)));
                        break;
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }

    return CMD_OK;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_print_trap_and_snoop(int unit, uint32 trap_code, uint32 snoop_code)
{
    LOG_CLI((BSL_META_U(unit, "    |                          Trap Code |                         Snoop Code |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |                                %3d |                                %3d |\n\r"),
                        trap_code, snoop_code));
    LOG_CLI((BSL_META_U(unit, "    |%35s |%35s |\n\r"), cmd_ppd_api_diag_pp_last_trap_code_desc(trap_code),
                        cmd_ppd_api_diag_pp_last_trap_code_desc(snoop_code)));
    LOG_CLI((BSL_META_U(unit, "    |%35s |%35s |\n\r"), cmd_ppd_api_diag_pp_last_trap_code_to_bcm_trap(trap_code),
                        cmd_ppd_api_diag_pp_last_trap_code_to_bcm_trap(snoop_code)));

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    return CMD_OK;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_parser(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution, int print_details)
{
    uint32 ret;

    SOC_PPC_DIAG_PARSING_INFO    
      prm_pars_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    const char* pkt_format = NULL;
    uint32 tc;
    uint32 dp;
    uint32 mirror_profile;
    uint32 trap_code;
    uint32 snoop_code;
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_DIAG_RESULT                     ret_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_parsing_info_get(
            unit,
            core,
            &prm_pars_info,
            &ret_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL;     
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        return CMD_FAIL;     
    } 

    pkt_format = dpp_parser_pfc_string_by_hw(unit, prm_pars_info.pfc_hw);
  
    /* Initial TC */
    /* dpp_dsig_read(unit, core_id, "IRPP", NULL, "VT", "Fwd_Action_TC", &tc, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,117,115,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,102,100,10);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = tc = regs_val[0];
    
    /* Initial DP */
    /* dpp_dsig_read(unit, core_id, "IRPP", NULL, "VT", "Fwd_Action_DP", &dp, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,122,121,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,107,106,20);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = dp = regs_val[0];

    /* trap */
    /* dpp_dsig_read(unit, core_id, "IRPP", NULL, "VT", "Fwd_Action_CPU_Trap_Code", &trap_code, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,146,139,30);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,131,124,30);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_TRAP_VALID;
    action_resolution->trap = trap_code = regs_val[0];

    /* mirror */
    /* dpp_dsig_read(unit, core_id, "IRPP", NULL, "VT", "LL_Mirror_Profile", &mirror_profile, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,68,65,40);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,55,52,40);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = mirror_profile = regs_val[0];

    /* snoop */
    /* dpp_dsig_read(unit, core_id, "IRPP", NULL, "VT", "Snoop_Code", &snoop_code, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,2,0,63,56,50);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,50,43,50);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop_code = snoop_code = regs_val[0];

    LOG_CLI((BSL_META_U(unit, "    |                                 \033[1mParser\033[0m                                  |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |    Packet Format    |   Init VID   |    TC    |  DP  |  Mirror Profile  |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |  %15s    |     %4d     |     %d    |   %d  |        %3d       |\n\r"),
                        pkt_format, prm_pars_info.initial_vid, tc, dp, mirror_profile));

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    if (print_details) {
        cmd_ppd_api_diag_pp_print_trap_and_snoop(unit, trap_code, snoop_code);
    }

    return CMD_OK;
exit:
  return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_vtt_lookups(int unit, int core)
{
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
	uint32 key[ARAD_PP_DIAG_DBG_VAL_LEN] = { 0 };
	uint32 payload[2] = { 0 };
	uint32 found;
	uint32 error;
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

	/*
	-------------------------------------------------------------------------
	|                                VTT lookups                            |
	-------------------------------------------------------------------------
	|                |                    key                  |   payload  |
	|------------------------------------------------------------------------
	|   SEM A 0 |                                              |            |
	|------------------------------------------------------------------------
	|   SEM B 0 |                                              |            |
	|------------------------------------------------------------------------
	|   TCAM 0  |                                              |            |
       |------------------------------------------------------------------------
       |   SEM A 1 |                                              |            |
       |------------------------------------------------------------------------
	|   SEM B 1 |                                              |            |
	|------------------------------------------------------------------------
	|   TCAM 1  |                                              |            |
	-------------------------------------------------------------------------
	*/

	LOG_CLI((BSL_META_U(unit, "    |                              \033[1mVTT lookups\033[0m                                |\n\r")));
	LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
	LOG_CLI((BSL_META_U(unit, "    |           |                       key                    |    payload   |\n\r")));
	LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    /* SEM A 1st lookup */
    /* dpp_dsig_read(unit, core_id, "IRPP", "VT", "ISEM_A", "VT_ISEM_A_Key", key, 2); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_A", "VT", "VT_ISEM_A_Result", payload, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_A", "VT", "VT_ISEM_A_Found", &found, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_A", "VT", "VT_ISEM_A_Error", &error, 1); */
    if (SOC_IS_JERICHO(unit)) {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,20,0,99,50,10);
	    key[1] = regs_val[1];
	    key[0] = regs_val[0];

        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,21,0,35,19,20);
	    payload[0] = regs_val[0];
	  
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,21,0,37,37,20);
        found = regs_val[0];

        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,21,0,36,36,20);
	    error = regs_val[0];

        if ( found && !error )
      	{
            LOG_CLI((BSL_META_U(unit, "    |   SEMA 0  |               0x%05x %08x               |     %5d    |\n\r"), key[1], key[0], payload[0]));
      	}
        else
      	{
	        LOG_CLI((BSL_META_U(unit, "    |   SEMA 0  |               0x%05x %08x               |       -      |\n\r"), key[1], key[0]));
        }	  
    }
    else {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,13,0,81,41,30);
        key[1] = regs_val[1];
        key[0] = regs_val[0];
        
        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,14,0,33,18,40);
        payload[0] = regs_val[0];
        
        LOG_CLI((BSL_META_U(unit, "    |   SEMA 0  |         	  0x%05x %08x                |     %5d    |\n\r"), key[1], key[0], payload[0]));
    }     

    /* SEM B 1st lookup */
    /* dpp_dsig_read(unit, core_id, "IRPP", "VT", "ISEM_B", "VT_ISEM_B_Key", key, 2); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_B", "VT", "VT_ISEM_B_Result", payload, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_B", "VT", "VT_ISEM_B_Found", &found, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_B", "VT", "VT_ISEM_B_Error", &error, 1); */
    if (SOC_IS_JERICHO(unit)) {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,22,0,99,50,50);
	    key[1] = regs_val[1];
	    key[0] = regs_val[0];

        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,23,0,35,19,60);
	    payload[0] = regs_val[0];
	  
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,23,0,37,37,20);
        found = regs_val[0];

        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,23,0,36,36,20);
	    error = regs_val[0];

        if ( found && !error )
      	{
            LOG_CLI((BSL_META_U(unit, "    |   SEMB 0  |               0x%05x %08x               |     %5d    |\n\r"), key[1], key[0], payload[0]));
      	}
        else
      	{
	        LOG_CLI((BSL_META_U(unit, "    |   SEMB 0  |               0x%05x %08x               |       -      |\n\r"), key[1], key[0]));
      	}	  
    }
    else {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,15,0,81,41,70);
        key[1] = regs_val[1];
        key[0] = regs_val[0];
        
        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,16,0,33,18,80);
        payload[0] = regs_val[0];
        
        LOG_CLI((BSL_META_U(unit, "    |   SEMB 0  |         	  0x%05x %08x                |     %5d    |\n\r"), key[1], key[0], payload[0]));
    }     

    /* TCAM 1st lookup */
    /* dpp_dsig_read(unit, core_id, "IRPP", "VT", "TCAM", "VT_TCAM_Key", key, 5); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "VT", "VT_TCAM_Action", payload, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "VT", "VT_TCAM_Match", &found, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "VT", "VT_TCAM_Mem_Soft_Error", &error, 1); */
    if (SOC_IS_JERICHO(unit)) {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,24,0,255,172,50);
        SHR_BITCOPY_RANGE(key, 0, regs_val, 0, (256-172));
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,24,1,75,0,50);
        SHR_BITCOPY_RANGE(key, (256-172), regs_val, 0, 76);

        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,25,0,98, 51,60);
        payload[1] = regs_val[1];
        payload[0] = regs_val[0];
	  
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,25,0,99,99,20);
        found = regs_val[0];

        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,25,0,50,50,20);
        error = regs_val[0];

        if ( found && !error ) {
            LOG_CLI((BSL_META_U(unit, "    |   TCAM 0  |0x%08x %08x %08x %08x %08x|0x%04x%08x|\n\r"), 
	            key[4], key[3], key[2], key[1], key[0], payload[1], payload[0]));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    |   TCAM 0  |0x%08x %08x %08x %08x %08x|       -      |\n\r"), 
	            key[4], key[3], key[2], key[1], key[0]));
        }	  
    }
    else {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,11,0,255,166,70);
        SHR_BITCOPY_RANGE(key, 0, regs_val, 0, (256-166));
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,11,1,69,0,70);
        SHR_BITCOPY_RANGE(key, (256-166), regs_val, 0, 70);
        
        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,12,0,82,43,80);
        payload[0] = regs_val[0];
        
        LOG_CLI((BSL_META_U(unit, "    |   TCAM 0  |0x%08x %08x %08x %08x %08x|     %5d    |\n\r"),
        	key[4], key[3], key[2], key[1], key[0], payload[0]));
    }     

    /* SEM A 2nd lookup */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "ISEM_A", "TT_ISEM_A_Key", key, 2); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_A", "TT", "TT_ISEM_A_Result", payload, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_A", "TT", "TT_ISEM_A_Found", &found, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_A", "TT", "TT_ISEM_A_Error", &error, 1); */
    if (SOC_IS_JERICHO(unit)) {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,20,0,49,0,10);
        key[1] = regs_val[1];
        key[0] = regs_val[0];

        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,21,0,16,0,20);
        payload[0] = regs_val[0];
      
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,21,0,18,18,20);
        found = regs_val[0];
        
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,21,0,17,17,20);
        error = regs_val[0];
        
        if ( found && !error ) {
            LOG_CLI((BSL_META_U(unit, "    |   SEMA 1  |               0x%05x %08x               |     %5d    |\n\r"), key[1], key[0], payload[0]));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    |   SEMA 1  |               0x%05x %08x               |       -      |\n\r"), key[1], key[0]));
        }
    }
    else {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,13,0,40,0,30);
        key[1] = regs_val[1];
        key[0] = regs_val[0];
        
        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,14,0,15,0,30);
        payload[0] = regs_val[0];
        
        LOG_CLI((BSL_META_U(unit, "    |   SEMA 1  |         	  0x%05x %08x                |     %5d    |\n\r"), key[1], key[0], payload[0]));
    }     

    /* SEM B 2nd lookup */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "ISEM_B", "TT_ISEM_B_Key", key, 2); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_B", "TT", "TT_ISEM_B_Result", payload, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_B", "TT", "TT_ISEM_B_Found", &found, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "ISEM_B", "TT", "TT_ISEM_B_Error", &error, 1); */
    if (SOC_IS_JERICHO(unit)) {
      /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,22,0,49,0,10);
        key[1] = regs_val[1];
        key[0] = regs_val[0];

        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,23,0,16,0,20);
        payload[0] = regs_val[0];
        
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,23,0,18,18,20);
        found = regs_val[0];
        
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,23,0,17,17,20);
        error = regs_val[0];
        
        if ( found && !error ) {
            LOG_CLI((BSL_META_U(unit, "    |   SEMB 1  |               0x%05x %08x               |     %5d    |\n\r"), key[1], key[0], payload[0]));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    |   SEMB 1  |               0x%05x %08x               |       -      |\n\r"), key[1], key[0]));
        }
    }
    else {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,15,0,40,0,30);
        key[1] = regs_val[1];
        key[0] = regs_val[0];
        
        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,16,0,15,0,30);
        payload[0] = regs_val[0];
        
        LOG_CLI((BSL_META_U(unit, "    |   SEMB 1  |            0x%05x %08x                |     %5d    |\n\r"), key[1], key[0], payload[0]));
    }     

    /* TCAM 2nd lookup */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "TCAM", "TT_TCAM_Key", key, 5); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "TT", "TT_TCAM_Action", payload, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "TT", "TT_TCAM_Match", &found, 1); */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TCAM", "TT", "TT_TCAM_Mem_Soft_Error", &error, 1); */
    if (SOC_IS_JERICHO(unit)) {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,24,0,165,6,50);
	    SHR_BITCOPY_RANGE(key, 0, regs_val, 0, (166-6));

        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,25,0,48,1,60);
        payload[1] = regs_val[1];
        payload[0] = regs_val[0];
	  
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,25,0,99,99,20);
        found = regs_val[0];
    
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,25,0,50,50,20);
        error = regs_val[0];
    
        if ( found && !error ) {
            LOG_CLI((BSL_META_U(unit, "    |   TCAM 1  |0x%08x %08x %08x %08x %08x|0x%04x%08x|\n\r"),
                key[4], key[3], key[2], key[1], key[0], payload[1], payload[0]));	  	
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    |   TCAM 1  |0x%08x %08x %08x %08x %08x|       -      |\n\r"),
                key[4], key[3], key[2], key[1], key[0]));	  	
        }
    }
    else {
        /* key */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,11,0,165,6,70);
	    SHR_BITCOPY_RANGE(key, 0, regs_val, 0, (166-6));
        
        /* payload */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,12,0,40,1,80);
        payload[0] = regs_val[0];
        
        LOG_CLI((BSL_META_U(unit, "    |   TCAM 1  |0x%08x %08x %08x %08x %08x|     %5d    |\n\r"),
	  	    key[4], key[3], key[2], key[1], key[0], payload[0]));
    }     

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    return CMD_OK;
    
exit:
      return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_vtt(int unit, int core, int print_details)
{
    uint32 ret = 0;

    SOC_PPC_DIAG_VSI_INFO
        prm_vsi_info;
    
    SOC_PPC_DIAG_VLAN_EDIT_RES     
        prm_vec_res; 
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    int local_lif = 0;
    uint32 system_lif = 0;
    uint32 res = SOC_SAND_OK;
    SOC_PPC_DIAG_RESULT                     ret_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    /* local LIF ID */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "In_LIF", &system_lif, 1); */
    if (SOC_IS_JERICHO(unit)) {
        if (SOC_IS_QAX(unit)) {
            if (SOC_IS_QUX(unit)) {
                ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,126,109,40);
            } else {
                ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,127,110,40);
            }
        } else if (SOC_IS_JERICHO_PLUS(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,131,114,40);
        } else {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,40,23,40);
        }

        system_lif = regs_val[0];

        /* ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,1,140,124,30);      */
        ret = _bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                    _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, system_lif, &local_lif);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
            return CMD_FAIL;
        }
    }
    else {
        /* ARAD */
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,1,245,230,30);
        local_lif = regs_val[0];
        system_lif = local_lif;
    }
      
    /* VSI */
    ret = soc_ppd_diag_vsi_info_get(
            unit,
            core,
            &prm_vsi_info
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      return CMD_FAIL;
    }

    LOG_CLI((BSL_META_U(unit, "    |                            \033[1mVLAN Translation\033[0m                             |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |          Local LIF  |            System LIF   |             VSI         |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |   %6d (0x%05X)  |      %6d (0x%05X)   |           %5d         |\n\r"),
                        local_lif, local_lif, system_lif, system_lif, prm_vsi_info.vsi));

    if (print_details) {
        ret = soc_ppd_diag_ing_vlan_edit_info_get(unit, core, &prm_vec_res, &ret_val);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
        { 
            return CMD_FAIL; 
        } 
        if (ret_val != SOC_PPC_DIAG_OK) 
        { 
            return CMD_FAIL; 
        } 

        
        if ( prm_vec_res.cmd_info.tags_to_remove || prm_vec_res.cmd_info.outer_tag.vid_source || prm_vec_res.cmd_info.inner_tag.vid_source )
        {
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                                   IVE                                   |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |       CMD ID        |   VID 0   |   VID 1   |TAGs to Remove|TPID Profile|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |         %2d          |   %4d    |   %4d    |       %d      |     %2d     |\n\r"),
                                prm_vec_res.cmd_id, prm_vec_res.ac_tag.vid, prm_vec_res.ac_tag2.vid, 
                                prm_vec_res.cmd_info.tags_to_remove, prm_vec_res.cmd_info.tpid_profile));

            if (prm_vec_res.cmd_info.outer_tag.vid_source || prm_vec_res.cmd_info.inner_tag.vid_source) {
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |  TPID index  |           VID src |  PCP-DEI src   |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            }
            
            if (prm_vec_res.cmd_info.outer_tag.vid_source)
            {        
                LOG_CLI((BSL_META_U(unit, "    |        Outer        |      %2d      |   %14s  |   %9s    |\n\r"),
                                prm_vec_res.cmd_info.inner_tag.tpid_index,
                                SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(prm_vec_res.cmd_info.outer_tag.vid_source),
                                SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(prm_vec_res.cmd_info.outer_tag.pcp_dei_source)));
            }
          
            if (prm_vec_res.cmd_info.inner_tag.vid_source)
            {
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |        Inner        |      %2d      |   %14s  |   %9s    |\n\r"),
                                prm_vec_res.cmd_info.inner_tag.tpid_index,
                                SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(prm_vec_res.cmd_info.inner_tag.vid_source),
                                SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(prm_vec_res.cmd_info.inner_tag.pcp_dei_source)));
            }
        }
        else
        {
            if (prm_vec_res.cmd_info.tpid_profile)
            {
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                                   IVE                                   |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |       CMD ID        |                   TPID Profile                    |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         %2d          |                        %2d                         |\n\r"),
                                    prm_vec_res.cmd_id, prm_vec_res.cmd_info.tpid_profile));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    return CMD_OK;
    
exit:
      return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_tt(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution, int print_details)
{
    uint32 ret;

    SOC_PPC_DIAG_TERM_INFO     
      prm_term_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32 destination;
    uint32 tc;
    uint32 dp;
    uint32 mirror_profile;
    uint32 trap_code;
    uint32 snoop_code;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_DIAG_RESULT                     ret_val;


    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_termination_info_get(
            unit,
            core,
            &prm_term_info,
            &ret_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL;  
    }
    
    /* destination */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_Dst", &destination, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,86,68,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,2,35,17,10);
    }

    action_resolution->flags |= CMD_DIAG_PP_DEST_VALID;
    action_resolution->dest = destination = regs_val[0];

    /* TC */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_TC", &tc, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,58,56,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,0,58,56,10);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = tc = regs_val[0];

    /* DP */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_DP", &dp, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,62,61,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,0,62,61,20);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = dp = regs_val[0];

    /* TRAP */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Fwd_Action_CPU_Trap_Code", &trap_code, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,2,110,103,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,2,59,52,10);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_TRAP_VALID;
    action_resolution->trap = trap_code = regs_val[0];

    /* mirror */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "LL_Mirror_Profile", &mirror_profile, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,1,144,141,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,0,55,52,10);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = mirror_profile = regs_val[0];

    /* snoop */
    /* dpp_dsig_read(unit, core_id, "IRPP", "TT", "FLP", "Snoop_Code", &snoop_code, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,3,1,82,75,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,1,163,160,20);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop_code = snoop_code = regs_val[0];

    if ((prm_term_info.term_type != SOC_PPC_PKT_TERM_TYPE_NONE) || (destination != (SOC_IS_JERICHO(unit)? 0x7FFFF: 0x3FFFF))) {
        LOG_CLI((BSL_META_U(unit, "    |                         \033[1mTunnel Termination\033[0m                              |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |  Terminated Header  |           destination   | TC | DP | Mirror Profile|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |    %12s     |    "), SOC_PPC_PKT_TERM_TYPE_to_string(prm_term_info.term_type)));
        
        cmd_ppd_api_diag_pp_destination_19bits_print(unit, destination);
        
        LOG_CLI((BSL_META_U(unit, "   |  %d |  %d |      %2d       |\n\r"), tc, dp, mirror_profile));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    }

    if (print_details) {
        cmd_ppd_api_diag_pp_print_trap_and_snoop(unit, trap_code, snoop_code);
    }

    return CMD_OK;
        
exit:
    return CMD_FAIL;
}


STATIC void
  cmd_ppd_api_FRWRD_LKUP_KEY_print(
    int unit,
    SOC_PPC_DIAG_FRWRD_LKUP_KEY *info,
    SOC_PPC_DIAG_FWD_LKUP_TYPE frwrd_type,
    uint8                      is_kbp
  )
{
    char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];

    if (info == NULL) {
        return;
    }

    switch(frwrd_type)
    {
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
            {
                SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *key;

                key = &(info->mact.key_val.mac);
                LOG_CLI((BSL_META_U(unit, "    |         key         |                 { DA, FID }                       |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |          { ")));
                soc_sand_SAND_PP_MAC_ADDRESS_print(&(key->mac));
                LOG_CLI((BSL_META_U(unit, ", %5d }             |\n\r"), key->fid));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
            {
                SOC_PPC_BMACT_ENTRY_KEY *key;

                key = &(info->bmact);

                LOG_CLI((BSL_META_U(unit, "    |         key         |                 { BMAC, BVID }                    |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |          { ")));
                soc_sand_SAND_PP_MAC_ADDRESS_print(&(key->b_mac_addr));
                LOG_CLI((BSL_META_U(unit, ", %5d }             |\n\r"), key->b_vid));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
            if(is_kbp) {
            SOC_PPC_DIAG_IPV4_UNICAST_RPF *key;

            key = &(info->kbp_ipv4_unicast_rpf);

            LOG_CLI((BSL_META_U(unit, "    |                     |   { VRF, SIP, DIP } [{ VRF, SIP }, { VRF, DIP }]  |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     |     { %4d, %15s, %15s }    |\n\r"), 
                                key->vrf, 
                                soc_sand_pp_ip_long_to_string(key->sip,1,decimal_ip),
                                soc_sand_pp_ip_long_to_string(key->dip,1,decimal_ip)));
            } else
#endif /* defined(INCLUDE_KBP) */
            { 
                SOC_PPC_DIAG_IPV4_VPN_KEY *key;

                key = &(info->ipv4_uc);

                LOG_CLI((BSL_META_U(unit, "    |                     |                   { VRF, DIP }                    |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |             { %4d, %15s }             |\n\r"),
                                    key->vrf, soc_sand_pp_ip_long_to_string(key->key.subnet.ip_address,1,decimal_ip)));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
            { 
                SOC_PPC_DIAG_IPV4_VPN_KEY *key;

                key = &(info->ipv4_uc);

                LOG_CLI((BSL_META_U(unit, "    |                     |                   { VRF, DIP }                    |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |             { %4d, %15s }             |\n\r"),
                                key->vrf, soc_sand_pp_ip_long_to_string(key->key.subnet.ip_address,1,decimal_ip)));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
#if defined(INCLUDE_KBP)  && !defined(BCM_88030)
            if(is_kbp) {
                SOC_PPC_DIAG_IPV4_MULTICAST *key;

                key = &(info->kbp_ipv4_multicast);

                LOG_CLI((BSL_META_U(unit, "    |                     | { VRF,InRIF,SIP,DIP }  [ {InRif,DIP}, {VRF,SIP} ] |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |  { %4d, %4d, %15s, %15s } |\n\r"),
                                key->vrf, key->in_rif,
                                soc_sand_pp_ip_long_to_string(key->sip,1,decimal_ip),
                                soc_sand_pp_ip_long_to_string(key->dip,1,decimal_ip)));
            } else
#endif /* defined(INCLUDE_KBP) */
            {
                SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY *key;

                key = &(info->ipv4_mc);

                LOG_CLI((BSL_META_U(unit, "    |                     |           { Group, InRIF(Valid?), SIP }           |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |   { %15s, %4d(%s), %15s }   |\n\r"),
                                  soc_sand_pp_ip_long_to_string(key->group,1,decimal_ip), key->inrif,
                                  key->inrif_valid?"V":"X",
                                  soc_sand_pp_ip_long_to_string(key->source.ip_address,1,decimal_ip)));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
            if(is_kbp) {
                SOC_PPC_DIAG_IPV6_UNICAST_RPF *key;

                key = &(info->kbp_ipv6_unicast_rpf);

                LOG_CLI((BSL_META_U(unit, "    |                     | { VRF, SIP, DIP }  [ { VRF, DIP }, { VRF, SIP } ] |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     | { %4d, "), key->vrf));
                soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->sip);
                soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->dip);
                LOG_CLI((BSL_META_U(unit, " } |\n\r")));
            } else
#endif /* defined(INCLUDE_KBP) */
            {
                SOC_PPC_DIAG_IPV6_VPN_KEY *key;

                key = &(info->ipv6_uc);

                LOG_CLI((BSL_META_U(unit, "    |                     |               { VRF, DIP(prefix) }                |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     | { %4d, "), key->vrf));

                soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->key.subnet.ipv6_address);
                LOG_CLI((BSL_META_U(unit, "(%2u) } |\n\r"), key->key.subnet.prefix_len));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
            if(is_kbp) {
                SOC_PPC_DIAG_IPV6_MULTICAST *key;

                key = &(info->kbp_ipv6_multicast);

                LOG_CLI((BSL_META_U(unit, "    |                     |{VRF, InRIF, SIP, DIP} [{InRIF,SIP,DIP}, {VRF,SIP}]|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     | { %4d, %4d, "), key->vrf, key->in_rif));

                soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->sip);
                soc_sand_SAND_PP_IPV6_ADDRESS_print(&key->dip);
                LOG_CLI((BSL_META_U(unit, " } |\n\r")));
            } else
#endif /* defined(INCLUDE_KBP) */
            {
                SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY *key;

                key = &(info->ipv6_mc);

                LOG_CLI((BSL_META_U(unit, "    |                     |                      { DIP }                      |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     | { ")));

                soc_sand_SAND_PP_IPV6_SUBNET_print(&(key->group));
                LOG_CLI((BSL_META_U(unit, " } |\n\r")));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
#if defined(INCLUDE_KBP)  && !defined(BCM_88030)
            if(is_kbp) {
                SOC_PPC_DIAG_MPLS *key;

                key = &(info->kbp_mpls);

                LOG_CLI((BSL_META_U(unit, "    |                     |           { InRIF, InPort, exp, label }           |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |        { %4d, %2d, %d, %7d (0x%6x) }        |\n\r"),
                                key->in_rif, key->in_port, key->exp, key->mpls_label, key->mpls_label));
            } else
#endif /* defined(INCLUDE_KBP) */
            {
                SOC_PPC_FRWRD_ILM_KEY *key;

                key = &(info->ilm);

                LOG_CLI((BSL_META_U(unit, "    |                     |           {VRF InRIF, InPort, exp, label }           |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |        { %4d,%4d, %2d, %d, %7d (0x%6x) }    |\n\r"),
                              key->vrf,key->inrif, key->in_local_port, key->mapped_exp, key->in_label, key->in_label));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
#if defined(INCLUDE_KBP)  && !defined(BCM_88030)
            if(is_kbp) {
                SOC_PPC_DIAG_TRILL_UNICAST *key;

                key = &(info->kbp_trill_unicast);

                LOG_CLI((BSL_META_U(unit, "    |                     |                  { egress nick }                  |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |                 { %5u (0x%4x) }                |\n\r"),
                                key->egress_nick, key->egress_nick));
            } else
#endif /* defined(INCLUDE_KBP) */
            {
                LOG_CLI((BSL_META_U(unit, "    |                     |                   { Trill UC }                    |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |                 { %5u (0x%4x) }                |\n\r"),
                                info->trill_uc, info->trill_uc));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
            if(is_kbp) {
                SOC_PPC_DIAG_TRILL_MULTICAST *key;

                key = &(info->kbp_trill_multicast);

                LOG_CLI((BSL_META_U(unit, "    |                     |           { DSDAI, FID, Dest Tree Nick }          |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |           { %3u, %5u, %5u (0x%4x) }          |\n\r"),
                                key->esdai, key->fid_vsi, key->dist_tree_nick, key->dist_tree_nick));
            } else
#endif /* defined(INCLUDE_KBP) */
            {
                SOC_PPC_TRILL_MC_ROUTE_KEY *key;

                key = &(info->trill_mc);

                LOG_CLI((BSL_META_U(unit, "    |                     | { DSDAI, FID, Tree Nick, Ing Nick, Adj EEP, TTS } |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |      { %3u, %5u, %5u, %5u, %6u, %2u }     |\n\r"),
                              key->esadi, key->fid, key->tree_nick, key->ing_nick, key->adjacent_eep, key->tts));
            }
            break;
        case SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF:
            {
                SOC_PPC_FRWRD_FCF_ROUTE_KEY *key;

                key = &(info->fcf);

                LOG_CLI((BSL_META_U(unit, "    |                     |        { Flags, VFI, DID (prefix length) }        |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |        { 0x%08x, 0x%04x, 0x%06x(%2d) }       |\n\r"),
                                key->flags, key->vfi, key->d_id, key->prefix_len));
            }
            break;
        default:
            LOG_CLI((BSL_META_U(unit, "    |                     |                        RAW                        |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |         key         |---------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     |             { 0x%08x, 0x%08x }            |\n\r"),
                          info->raw[0], info->raw[1]));
            break;
    }
}

STATIC void
cmd_ppd_api_FRWRD_DECISION_INFO_print(int unit, SOC_PPC_FRWRD_DECISION_INFO *info)
{
    uint32 outlif;

    outlif = info->additional_info.outlif.val;

    if (info->type == SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
        LOG_CLI((BSL_META_U(unit, "    |        result       |                      \033[31mDrop\033[0m                         |\n\r")));

        return;
    }

    switch(info->type)
    {
    case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
    case SOC_PPC_FRWRD_DECISION_TYPE_MC:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
    case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
        {
            SOC_PPC_EEI *eei;
            
            eei = &(info->additional_info.eei);
            switch(eei->type)
            {
            case SOC_PPC_EEI_TYPE_MPLS:
                {
                    SOC_PPC_MPLS_COMMAND *mpls_cmd;
    
                    mpls_cmd = &(eei->val.mpls_command);

                    if (outlif != 0) {
                        LOG_CLI((BSL_META_U(unit, "    |                     |           dest              |       OutLif        |\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |                     | %7s | %6d (0x%06x) |  %6d (0x%06x)  |\n\r"),
                                            SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                            info->dest_id,
                                            info->dest_id,
                                            outlif,
                                            outlif));
                    }
                    else {
                        LOG_CLI((BSL_META_U(unit, "    |                     |                      dest                         |\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |                     |       %7s         |    %6d (0x%06x)      |\n\r"),
                                            SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                            info->dest_id,
                                            info->dest_id));
                    }

                    LOG_CLI((BSL_META_U(unit, "    |        result       |---------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     |              MPLS cmd |   Label    | Push Profile |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     | %21s |  %7d   |     %2d       |\n\r"),
                                        SOC_PPC_MPLS_COMMAND_TYPE_to_string(mpls_cmd->command),
                                        mpls_cmd->label,
                                        mpls_cmd->push_profile));
                }
                break;
            case SOC_PPC_EEI_TYPE_TRILL:
                {
                    SOC_SAND_PP_TRILL_DEST *trill_dest;
    
                    trill_dest = &(eei->val.trill_dest);

                    if (outlif != 0) {
                        LOG_CLI((BSL_META_U(unit, "    |                     |           dest              |       OutLif        |\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |        result       | %7s | %6d (0x%06x) |  %6d (0x%06x)  |\n\r"),
                                            SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                            info->dest_id,
                                            info->dest_id,
                                            outlif,
                                            outlif));
                    }
                    else {
                        LOG_CLI((BSL_META_U(unit, "    |                     |                      dest                         |\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                        LOG_CLI((BSL_META_U(unit, "    |        result       |       %7s         |    %6d (0x%06x)      |\n\r"),
                                            SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                            info->dest_id,
                                            info->dest_id));
                    }
                    LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));

                    if (trill_dest->is_multicast) {
                        LOG_CLI((BSL_META_U(unit, "    |                     | Trill | Multicast |     Dest Nick = %5u         |\n\r"),
                                            trill_dest->dest_nick));
                    }
                    else {
                        LOG_CLI((BSL_META_U(unit, "    |                     | Trill | Unicast |     Dist-Tree-Nick = %5u      |\n\r"),
                                            trill_dest->dest_nick));
                    }

                }
                break;
            case SOC_PPC_EEI_TYPE_MIM:
                if (outlif != 0) {
                    LOG_CLI((BSL_META_U(unit, "    |                     |           dest              |       OutLif        |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |        result       | %7s | %6d (0x%06x) |  %6d (0x%06x)  |\n\r"),
                                        SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                        info->dest_id,
                                        info->dest_id,
                                        outlif,
                                        outlif));
                }
                else {
                    LOG_CLI((BSL_META_U(unit, "    |                     |                      dest                         |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |        result       |       %7s         |    %6d (0x%06x)      |\n\r"),
                                        SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                        info->dest_id,
                                        info->dest_id));
                }
                LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |        MiM        |        ISID = %7u         |\n\r"),
                                    eei->val.isid));
                break;
            default:
                if (outlif != 0) {
                    LOG_CLI((BSL_META_U(unit, "    |                     |           dest              |       OutLif        |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     |---------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |        result       | %7s | %6d (0x%06x) |  %6d (0x%06x)  |\n\r"),
                                        SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                        info->dest_id,
                                        info->dest_id,
                                        outlif,
                                        outlif));
                    }
                else {
                    LOG_CLI((BSL_META_U(unit, "    |                     |                      dest                         |\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |        result       |---------------------------------------------------|\n\r")));
                    LOG_CLI((BSL_META_U(unit, "    |                     |       %7s         |    %6d (0x%06x)      |\n\r"),
                                        SOC_PPC_FRWRD_DECISION_TYPE_to_string(info->type),
                                        info->dest_id,
                                        info->dest_id));
                }
                break;
            }
        }
        break;
    case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
        {
            SOC_PPC_TRAP_INFO *trap_info;
    
            trap_info = &(info->additional_info.trap_info);
                
            LOG_CLI((BSL_META_U(unit, "    |                     |                              TRAP code = %3d      |\n\r"),
                trap_info->action_profile.trap_code));
            LOG_CLI((BSL_META_U(unit, "    |        result       |       %38s      |\n\r"),
                cmd_ppd_api_diag_pp_last_trap_code_desc(trap_info->action_profile.trap_code)));
            LOG_CLI((BSL_META_U(unit, "    |                     |       %38s      |\n\r"),
                cmd_ppd_api_diag_pp_last_trap_code_to_bcm_trap(trap_info->action_profile.trap_code)));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit, "    |        result       |                  Type Unknown (%1d)                 |\n\r"),
                            info->type));
        break;
    }

}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_flp(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution, int print_details)
{
    uint32 ret;

    SOC_PPC_DIAG_FRWRD_LKUP_INFO   
      prm_frwrd_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32 tc;
    uint32 dp;
    uint32 mirror_profile;
    uint32 trap_code;
    uint32 snoop_code;
    uint32 counter0 = 0, counter0_valid = 0;
    uint32 counter1 = 0, counter1_valid = 0;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_DIAG_RESULT                     ret_val;


    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_frwrd_lkup_info_get(
              unit,
              core,
              &prm_frwrd_info,
              &ret_val
            );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        return CMD_FAIL; 
    } 

    /* TC */
    /* dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Fwd_Action_TC", &tc, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,8,101,99,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,7,158,156,10);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = tc = regs_val[0];

    /* DP */
    /* dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Fwd_Action_DP", &dp, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,8,108,107,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,7,165, 164,20);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = dp = regs_val[0];

    /* counter 0 */
    /* dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Counter_A_Update", &counter0_valid, 1);
     * if (counter0_valid != 0) {
     *      dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Counter_A_Ptr", &counter0, 1);
     * }
     */

    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,194,194,20);

        if ( (counter0_valid = regs_val[0]) != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,236,216,20);

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_0_VALID;
            action_resolution->counter0 = counter0 = regs_val[0];            
        }
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,8,123,123,20);

        if ((counter0_valid = regs_val[0]) != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,0,8,165,145,20);

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_0_VALID;
            action_resolution->counter0 = counter0 = regs_val[0];            
        }
    }   

    /* counter 1 */
    /* dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Counter_B_Update", &counter0_valid, 1);
     * if (counter0_valid != 0) {
     *      dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Counter_B_Ptr", &counter0, 1);
     * }
     */

    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,193,193,20);

        if ((counter1_valid = regs_val[0]) != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,9,215,195,20);

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_1_VALID;
            action_resolution->counter1 = counter1 = regs_val[0];            
        }
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,8,122,122,20);

        if ((counter1_valid = regs_val[0]) != 0) { /* valid */
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,0,8,144,124,20);

            action_resolution->flags |= CMD_DIAG_PP_COUNTER_1_VALID;
            action_resolution->counter1 = counter1 = regs_val[0];            
        }
    }

    /* mirror */
    /* dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "LL_Mirror_Profile", &mirror_profile, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,6,226,223,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,6,101,98,10);
    }

    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = mirror_profile = regs_val[0];
        
    LOG_CLI((BSL_META_U(unit, "    |                             \033[1mForward Lookup\033[0m                              |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |    Header Offset    |        Type  |  DB | TC | DP |   Mirror Profile   |\n\r")));

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |          %d          |   %9s  | %3s |  %d |  %d |         %2d         |\n\r"),
                        prm_frwrd_info.frwrd_hdr_index,
                        SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(prm_frwrd_info.frwrd_type),
                        prm_frwrd_info.is_kbp?"KBP":"LEM",
                        tc, dp, mirror_profile));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    cmd_ppd_api_FRWRD_LKUP_KEY_print(unit, &(prm_frwrd_info.lkup_key),prm_frwrd_info.frwrd_type, prm_frwrd_info.is_kbp);
#else
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |          %d          |   %9s  | %3s |  %d |  %d |         %2d         |\n\r"),
                        prm_frwrd_info.frwrd_hdr_index,
                        SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(prm_frwrd_info.frwrd_type),
                        "LEM",
                        tc, dp, mirror_profile));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    cmd_ppd_api_FRWRD_LKUP_KEY_print(unit, &(prm_frwrd_info.lkup_key),prm_frwrd_info.frwrd_type, FALSE);
#endif /* defined(INCLUDE_KBP) */

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    if(prm_frwrd_info.is_kbp) {
        SOC_PPC_DIAG_IP_REPLY_RECORD *result;

        result = &(prm_frwrd_info.ip_reply_record);

        if (result->match_status)
        {
            switch (result->identifier)
            {
            case 0:
                LOG_CLI((BSL_META_U(unit, "    |                     |           dest              |       OutLif        |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |       result 0      |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     | %7s | %6d (0x%06x) |  %6d (0x%06x)  |\n\r"),
                                    SOC_PPC_FRWRD_DECISION_TYPE_to_string(result->destination.type),
                                    result->destination.dest_id,
                                    result->destination.dest_id,
                                    result->out_lif,
                                    result->out_lif));
                break;
            case 1:
                LOG_CLI((BSL_META_U(unit, "    |                     |            FEC            |          EEI          |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |       result 0      |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |           %6d          |   %6d (0x%06x)   |\n\r"),
                                    result->fec_ptr, result->eei, result->eei));
                break;
            default:
                break;
            }
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    |        result 0     |                    Not Match                      |\n\r")));
        }

        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

        {
            SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD *info;

            info = &(prm_frwrd_info.second_ip_reply_result);


            if(info->match_status) {
                LOG_CLI((BSL_META_U(unit, "    |                     |                          FEC                      |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |       result 1      |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |                         %6d                    |\n\r"),
                    info->dest_id));
            } else {
                LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |        result 1     |                    Not Match                      |\n\r")));
            }
        }

        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    } else
#endif /* defined(INCLUDE_KBP) */
    {
        if (prm_frwrd_info.key_found)
        {
            switch(prm_frwrd_info.frwrd_type)
            {
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT:
              {
                SOC_PPC_FRWRD_MACT_ENTRY_VALUE *info;

                info = &(prm_frwrd_info.lkup_res.mact);
                
                cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, &(info->frwrd_info.forward_decision));            
              }
            break;
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT:
              {
                SOC_PPC_BMACT_ENTRY_INFO *bmact;

                bmact = &(prm_frwrd_info.lkup_res.bmact);

                LOG_CLI((BSL_META_U(unit, "    |                     |      Port    |      ISID      |    Learned FEC    |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |        result       |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |      %4d    |     %7d    |       %5d       |\n\r"),
                                    bmact->sys_port_id, bmact->i_sid_domain, bmact->sa_learn_fec_id));
              }
            break;
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC:
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC:
                cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, &(prm_frwrd_info.lkup_res.frwrd_decision));
            break;
            case SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_HOST:
                {
                    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO *info;

                    info = &(prm_frwrd_info.lkup_res.host_info);

                    cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, &(info->frwrd_decision));
                }
            break;
            default:
                LOG_CLI((BSL_META_U(unit, "    |                     |                         raw                       |\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |        result       |---------------------------------------------------|\n\r")));
                LOG_CLI((BSL_META_U(unit, "    |                     |                  0x%08x %08x              |\n\r"),
                                    prm_frwrd_info.lkup_res.raw[0],
                                    prm_frwrd_info.lkup_res.raw[1]));
              break;
            }
        }
        else
        {
            LOG_CLI((BSL_META_U(unit, "    |        result 0     |                    Not Match                      |\n\r")));
        }

        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    }

    if (print_details) {
        if (counter0_valid || counter1_valid) {
            LOG_CLI((BSL_META_U(unit, "    |  Valid  |        Counter 0         |  Valid  |        Counter 1         |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |    %s    |         %5d            |    %s    |          %5d           |\n\r"),
                                counter0_valid?"V":"-", counter0,
                                counter1_valid?"V":"-", counter1));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        }

        /* TRAP */
        /* dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Fwd_Action_CPU_Trap_Code", &trap_code, 1); */
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,8,151,144,10);
        }
        else {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,7,208,201,10);
        }
        
        action_resolution->flags |= CMD_DIAG_PP_TRAP_VALID;
        action_resolution->trap = trap_code = regs_val[0];

        /* snoop */
        /* dpp_dsig_read(unit, core_id, "IRPP", "FLP", "PMF", "Snoop_Code", &snoop_code, 1); */
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,1,255,252,20);
            snoop_code = regs_val[0];        
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,4,2,3,0,20);
            snoop_code |= (regs_val[0]) << 4;        
        }
        else {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,1,180,173,20);
            snoop_code = regs_val[0];        
        }
        
        action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
        action_resolution->snoop_code = snoop_code;

        cmd_ppd_api_diag_pp_print_trap_and_snoop(unit, trap_code, snoop_code);
    }

    return CMD_OK;

exit:
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_pmf(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution, int print_details)
{
    uint32 ret;
    uint32
      ind2,
      ind;

    SOC_PPC_FP_PACKET_DIAG_INFO   
      *prm_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32 mirror_profile;
    uint32 snoop_code;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    prm_info = sal_alloc(sizeof(SOC_PPC_FP_PACKET_DIAG_INFO), "cmd_ppd_api_fp_packet_diag_get.prm_info");
    
    if(!prm_info) {
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }
    
    SOC_PPC_FP_PACKET_DIAG_INFO_clear(prm_info);
    
    ret = soc_ppd_fp_packet_diag_get(
            unit,
            core,
            prm_info
          );
    
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        sal_free(prm_info);
        return CMD_FAIL; 
    } 

    LOG_CLI((BSL_META_U(unit, "    |                              \033[1mPMF actions\033[0m                                |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    for (ind = 0; ind < SOC_PPC_FP_NOF_CYCLES; ++ind)
    {
        for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACRO_SIMPLES; ++ind2)
        {
            SOC_PPC_FP_PACKET_DIAG_MACRO_SIMPLE *macro;

            macro = &(prm_info->macro_simple[ind][ind2]);
            if ((macro->action.type == SOC_PPC_FP_ACTION_TYPE_INVALID) ||
                (macro->action.type == SOC_PPC_FP_ACTION_TYPE_NOP))
            {
                continue;
            }

            LOG_CLI((BSL_META_U(unit, "    |        DB %3d       |   %24s   | %7d (0x%5X)  |\n\r"),
                                macro->db_id,
                                SOC_PPC_FP_ACTION_TYPE_to_string(macro->action.type),
                                macro->action.val,
                                macro->action.val));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        }
    
        for (ind2 = 0; ind2 < SOC_PPC_FP_NOF_MACROS; ++ind2)
        {
            SOC_PPC_FP_PACKET_DIAG_MACRO *macro;

            macro = &(prm_info->macro[ind][ind2]);
            if ((macro->action.type == SOC_PPC_FP_ACTION_TYPE_INVALID) ||
                (macro->action.type == SOC_PPC_FP_ACTION_TYPE_NOP))
            {
                continue;
            }

            LOG_CLI((BSL_META_U(unit, "    |        DB %3d       |   %24s   | %7d (0x%5X)  |\n\r"),
                                  macro->db_id,
                                  SOC_PPC_FP_ACTION_TYPE_to_string(macro->action.type),
                                  macro->action.val,
                                  macro->action.val));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        }
    }
    
    sal_free(prm_info);

    if (print_details) {
        /* mirror */
        /* dpp_dsig_read(unit, core_id, "IRPP", "PMF", NULL, "Mirror_Action", &mirror_profile, 1); */
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,9,255,254,10);
            mirror_profile = regs_val[0];
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,0,10,1,0,10);
            mirror_profile |= regs_val[0] << 2;
        }
        else {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,9,9,233,230,10);
            mirror_profile = regs_val[0];
        }
        
        action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
        action_resolution->mirror = mirror_profile;

        /* snoop */
        /* dpp_dsig_read(unit, core_id, "IRPP", "PMF", NULL, "Snoop_Code", &snoop_code, 1); */
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHP_ID,0,9,144,137,20);
        }
        else {
            ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,9,9,141,134,20);
        }
        
        action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
        action_resolution->snoop_code = snoop_code = regs_val[0];

        LOG_CLI((BSL_META_U(unit, "    |   Mirror Profile    |                                  Snoop code       |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     |                                         %3d       |\n\r"),
                            snoop_code));
        LOG_CLI((BSL_META_U(unit, "    |         %2d          |         %35s       |\n\r"),
                            mirror_profile,
                            cmd_ppd_api_diag_pp_last_trap_code_desc(snoop_code)));
        LOG_CLI((BSL_META_U(unit, "    |                     |         %35s       |\n\r"),
                            cmd_ppd_api_diag_pp_last_trap_code_to_bcm_trap(snoop_code)));    
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
    }
    
    return CMD_OK;

exit:
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_fec(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution, int print_details)
{
    SOC_PPC_FRWRD_DECISION_INFO frwrd_decision;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
      dest,
      eei=0;
    uint32 trap_code;
    uint32 mirror_profile;
    uint32 snoop_code;
    uint32 snoop_cmd;
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));
    sal_memset(&frwrd_decision, 0, sizeof(frwrd_decision));

    LOG_CLI((BSL_META_U(unit, "    |                             \033[1mFEC Resolution\033[0m                              |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    res = dpp_dsig_read(unit, core, "IRPP", "LBP", NULL, "Dst", &dest, 1);
    if (res != _SHR_E_NONE)
    {
        SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    }

    /* dpp_dsig_read(unit, core_id, "FER", "LBP", "EEI", &eei, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core,ARAD_IHB_ID, 2, 3, 86, 63, 125);
    } else  if (SOC_IS_ARADPLUS(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core,ARAD_IHB_ID, 10, 3, 48, 25, 125);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core,ARAD_IHB_ID,10, 2 , 253, 230,125);
    }
    
    eei = regs_val[0];
    res = arad_pp_diag_fwd_decision_in_buffer_parse(
          unit,          
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TM,
          dest,
          eei,
          &frwrd_decision);
    SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
    
    /*dbg: fer2lbp_eq2_out_lif [15 : 0] */
    /* dpp_dsig_read(unit, core_id, "FER", "LBP", "Out_LIF", &frwrd_decision.additional_info.outlif.val, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core,ARAD_IHB_ID, 2, 1, 62, 45, 124);
    } else  if (SOC_IS_ARADPLUS(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld,core,ARAD_IHB_ID,10, 1 , 48, 33, 1241);
    } else {
        ARAD_PP_DIAG_FLD_READ(&fld,core,ARAD_IHB_ID,10, 1 , 30, 15,124);
    }

    frwrd_decision.additional_info.outlif.val = regs_val[0];

    cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, &frwrd_decision);

    /* trap code */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, READ_IHB_DBG_LAST_RESOLVED_TRAPr(unit, core, &regs_val[0]));
    
    trap_code = soc_reg_field_get(unit, IHB_DBG_LAST_RESOLVED_TRAPr, regs_val[0], DBG_LAST_RESOLVED_CPU_TRAP_CODEf);
    
    /* mirror, not interesting, not printed */
    /* dpp_dsig_read(unit, core_id, "FER", "LBP", "Mirror_Action_Profile_Index", &mirror_profile, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,2,1,93,90,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,10,1,76,73,10);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = mirror_profile = regs_val[0];

    /* snoop */
    /* dpp_dsig_read(unit, core_id, "FER", "LBP", "Snoop_Code", &snoop_code, 1); */
    /* dpp_dsig_read(unit, core_id, "FER", "LBP", "Snoop_Action_Profile_Index", &snoop_cmd, 1); */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,2,1,20,13,10);
        action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
        action_resolution->snoop_code = snoop_code = regs_val[0];

        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,2,1,24,21,10);
        action_resolution->snoop_cmd = snoop_cmd = regs_val[0];
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,9,9,141,134,10);
        action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
        action_resolution->snoop_code = snoop_code = regs_val[0];

        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,10,1,12,9,20);
        action_resolution->snoop_cmd = snoop_cmd = regs_val[0];
    }  

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    if (print_details) {
        cmd_ppd_api_diag_pp_print_trap_and_snoop(unit, trap_code, snoop_code);
    }

    return CMD_OK;

exit:
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_tm(int unit, int core, cmd_diag_pp_packet_action_t * action_resolution, int print_details)
{
    uint32 ret;

    SOC_PPC_DIAG_PKT_TM_INFO   
      prm_pkt_tm_info;
    ARAD_PP_DIAG_REG_FIELD
      fld;
    uint32
      regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32 snoop_code;
    uint32 mirror_profile;
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_DIAG_RESULT                     ret_val;


    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_PROC_DESC_BASE_DIAG_FIRST);

    LOG_CLI((BSL_META_U(unit, "    |                             \033[1mTM Resolution\033[0m                               |\n\r")));
    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    SOC_SAND_CHECK_NULL_INPUT(action_resolution);
    sal_memset(action_resolution, 0, sizeof(*action_resolution));

    ret = soc_ppd_diag_pkt_associated_tm_info_get(
            unit,
            core,
            &prm_pkt_tm_info,
            &ret_val
          );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
          return CMD_FAIL; 
    } 
    if (ret_val != SOC_PPC_DIAG_OK) 
    { 
        return CMD_FAIL; 
    } 

    
    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_DEST)
    {
      cmd_ppd_api_FRWRD_DECISION_INFO_print(unit, &(prm_pkt_tm_info.frwrd_decision));
      action_resolution->dest_type = prm_pkt_tm_info.frwrd_decision.type;
      action_resolution->dest = prm_pkt_tm_info.frwrd_decision.dest_id;
    }
    else {
        LOG_CLI((BSL_META_U(unit, "    |        result       |                      \033[31mDrop\033[0m                         |\n\r")));
    }

    LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

    action_resolution->flags |= CMD_DIAG_PP_TC_VALID;
    action_resolution->tc = prm_pkt_tm_info.tc;
    
    action_resolution->flags |= CMD_DIAG_PP_DP_VALID;
    action_resolution->dp = prm_pkt_tm_info.dp;

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY)
    {
        action_resolution->lag_lb_key = prm_pkt_tm_info.lag_lb_key;
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1) {
        action_resolution->flags |= CMD_DIAG_PP_COUNTER_0_VALID;
        action_resolution->counter0 = prm_pkt_tm_info.counter1;
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2) {
        action_resolution->flags |= CMD_DIAG_PP_COUNTER_1_VALID;
        action_resolution->counter1 = prm_pkt_tm_info.counter2;
    }

    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER1) {
        action_resolution->flags |= CMD_DIAG_PP_METER_0_VALID;
        action_resolution->meter0 = prm_pkt_tm_info.meter1;
    }
    
    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER2) {
        action_resolution->flags |= CMD_DIAG_PP_METER_1_VALID;
        action_resolution->meter1 = prm_pkt_tm_info.meter2;
    }
    
    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD)
    {
        action_resolution->flags |= CMD_DIAG_PP_DP_METER_CMD_VALID;
        action_resolution->dp_meter_cmd = prm_pkt_tm_info.dp_meter_cmd;
    }
    
    /* mirror */
    /* dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "Mirror_Cmd", &mirror_profile, 1); From TM_Cmd */
    if (SOC_IS_QAX(unit)){
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,4,0,131,128,10);
    }
    else if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,4,0,126,123,10);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,12,0,124,121,10);
    }
    
    action_resolution->flags |= CMD_DIAG_PP_MIRROR_VALID;
    action_resolution->mirror = mirror_profile = regs_val[0];
    
    /* dpp_dsig_read(unit, core_id, "IRPP", "LBP", NULL, "Snoop_Cmd", &action_resolution->snoop_cmd, 1); From TM_Cmd */
    if (SOC_IS_JERICHO(unit)) {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,4,1,62,55,20);
    }
    else {
        ARAD_PP_DIAG_FLD_READ(&fld, core, ARAD_IHB_ID,12,0,120,117,10);            
    }
    
    action_resolution->flags |= CMD_DIAG_PP_SNOOP_VALID;
    action_resolution->snoop_code = snoop_code = regs_val[0];
    
    if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID)
    {
        action_resolution->snoop_cmd = prm_pkt_tm_info.snoop_id;
    }

    if (print_details) {
        LOG_CLI((BSL_META_U(unit, "    | TC | DP |   LB key  | counter 0 | counter 1 | meter 0| meter 1|Meter Cmd|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));


        LOG_CLI((BSL_META_U(unit, "    |  %d |  %d |"), 
                            action_resolution->tc, 
                            action_resolution->dp));

        if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY)
        {
            LOG_CLI((BSL_META_U(unit, "   %6d  |"), action_resolution->lag_lb_key));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    -    |")));
        }

        if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1) {
            LOG_CLI((BSL_META_U(unit, "   %5d   |"), action_resolution->counter0));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "     -     |")));
        }

        if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2) {
            LOG_CLI((BSL_META_U(unit, "   %5d   |"), action_resolution->counter1));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "     -     |")));
        }

        if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER1) {
            LOG_CLI((BSL_META_U(unit, "  %5d |"), action_resolution->meter0));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    -   |")));
        }

        if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_METER2) {
            LOG_CLI((BSL_META_U(unit, "  %5d |"), action_resolution->meter1));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "    -   |")));
        }

        if (prm_pkt_tm_info.valid_fields & SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD)
        {
            LOG_CLI((BSL_META_U(unit, "     %d   |\n\r"), action_resolution->dp_meter_cmd));
        }
        else {
            LOG_CLI((BSL_META_U(unit, "     -   |\n\r")));
        }

        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |   Mirror Profile    |                                  Snoop code       |\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        LOG_CLI((BSL_META_U(unit, "    |                     |                                         %3d       |\n\r"),
                            snoop_code));
        LOG_CLI((BSL_META_U(unit, "    |         %2d          |         %35s       |\n\r"),
                            action_resolution->mirror,
                            cmd_ppd_api_diag_pp_last_trap_code_desc(snoop_code)));
        LOG_CLI((BSL_META_U(unit, "    |                     |         %35s       |\n\r"),
                            cmd_ppd_api_diag_pp_last_trap_code_to_bcm_trap(snoop_code)));

        LOG_CLI((BSL_META_U(unit, "     -------------------------------------------------------------------------\n\r")));
    }

    return CMD_OK;

exit:
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_flow_to_q(int unit, int core, uint32 flow_id, uint32 tc, uint32 *queue)
{
    uint32 soc_sand_rv = 0;
    uint32 tc_profile = 0;
    ARAD_IPQ_TR_CLS new_class = 0;
    ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO info;
    
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&info, 0x0, sizeof(info));

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipq_tc_profile_get, (unit, core, 1, flow_id, &tc_profile)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipq_tc_profile_map_get,(unit, core, tc_profile, 1, tc, &new_class)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipq_explicit_mapping_mode_info_get,(unit, &info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (info.queue_id_add_not_decrement) {
        *queue = info.base_queue_id + flow_id + new_class;
    }
    else {
        *queue = info.base_queue_id - flow_id + new_class;
    }

    return CMD_OK;
    
exit:
    (void)_rv;
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_uc_port_to_q(int unit, int core, uint32 port, uint32 tc, uint32 *queue)
{
    uint8 valid = 0;
    uint8 sw_only = 0;
    uint32 base_queue = 0;
    uint32 tc_profile = 0;
    ARAD_IPQ_TR_CLS new_class = 0;
    uint32 rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    
    rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_destination_id_packets_base_queue_id_get, (unit, core, port, &valid, &sw_only, &base_queue)));
    BCM_SAND_IF_ERR_EXIT(rv);

    if (!valid) {
        return CMD_FAIL;
    }
    else {
        rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_tc_profile_get, (unit, core, 0, port, &tc_profile)));
        BCM_SAND_IF_ERR_EXIT(rv);

        rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_tc_profile_map_get,(unit, core, tc_profile, 0, tc, &new_class)));
        BCM_SAND_IF_ERR_EXIT(rv);

        *queue = base_queue + new_class;
    }

    return CMD_OK;
    
exit:
    (void)_rv;
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_lag_get_member_and_q(int unit, int core, uint32 lag_id, uint32 lb_key, uint32 tc, uint32 *lag_member, uint32 *queue)
{
    SOC_PPC_LAG_INFO    lag_info;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = soc_ppd_lag_get(unit, lag_id, &lag_info);
    soc_sand_rv = handle_sand_result(soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *lag_member = lag_info.members[(lb_key * lag_info.nof_entries) >> 16].sys_port;

    return cmd_ppd_api_diag_pp_last_uc_port_to_q(unit, core, *lag_member, tc, queue);
    
exit:
    (void)_rv;
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_mc_info_print(int unit, int core, uint32 mc_id)
{
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QAX(unit)){
        uint32 entry_index = mc_id;
        qax_mcds_t *mcds = dpp_get_mcds(unit);
        qax_mcdb_entry_t *mcdb_entry;

        mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, entry_index);
        if (DPP_MCDS_TYPE_IS_INGRESS(QAX_MCDS_ENTRY_GET_TYPE(mcdb_entry))) { /* ingress multicast */

            uint32 format;
            uint32 destination;
            uint32 cud;
            uint32 additional_cud=0;
            uint32 bitmap;

            LOG_CLI((BSL_META_U(unit, "Replication information: \n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     port                |               CUD             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

            while ((entry_index > 0) && (entry_index < QAX_MC_INGRESS_LINK_PTR_END)) {
                mcdb_entry = QAX_GET_MCDB_ENTRY(mcds, entry_index);
                format = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, FORMATf);
                switch (format) {
                    case 0: /* TAR_MCDB_BITMAP_REPLICATION_ENTRYm */
                        entry_index = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, LINK_PTRf);
                        cud = soc_mem_field32_get(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, CUD_0f);
                        additional_cud = soc_mem_field32_get(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, CUD_1f);
                        bitmap = soc_mem_field32_get(unit, TAR_MCDB_BITMAP_REPLICATION_ENTRYm, mcdb_entry, BMP_PTRf);
                        destination = (bitmap - mcds->ingress_bitmap_start)/(SOC_DPP_CONFIG(unit)->qax->nof_ingress_bitmap_bytes);
                        break;
                    case 1: /* TAR_MCDB_SINGLE_REPLICATIONm */
                       entry_index = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, LINK_PTRf);
                        destination = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, DESTINATIONf);
                        cud = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_0f);
                        additional_cud = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_entry, CUD_1f);
                        break;
                    case 2: /* TAR_MCDB_DOUBLE_REPLICATIONm */
                        entry_index++;
                        destination = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATIONf);
                        cud = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_0f);
                        break;
                    case 3: /* TAR_MCDB_DOUBLE_REPLICATIONm  (last) */
                        entry_index = QAX_MC_INGRESS_LINK_PTR_END;
                        destination = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, DESTINATION_1f);
                        cud = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_entry, CUD_1f); 
                        break;
                    default:
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unsupported ingress format found %u"), format));
                }
                LOG_CLI((BSL_META_U(unit, "    |            ")));
                cmd_ppd_api_diag_pp_destination_19bits_print(unit, destination);
                LOG_CLI((BSL_META_U(unit, "           |            0x%05X            |\n\r"), cud));
                if(additional_cud)
                {
                    LOG_CLI((BSL_META_U(unit, "    |            ")));
                    cmd_ppd_api_diag_pp_destination_19bits_print(unit, destination);
                    LOG_CLI((BSL_META_U(unit, "           |            0x%05X            |\n\r"), additional_cud));
                    additional_cud = 0;
                }
            }
			LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        }
    } else {
        uint32 entry[2];
        const unsigned bit_offset = 2 * (mc_id % 16);
        const int table_index = mc_id / 16;
        SOCDNX_IF_ERR_EXIT(READ_IDR_IRDBm(unit, MEM_BLOCK_ANY, table_index, entry));

        if ( ((entry[0]) >> bit_offset) & 0x1 ) { /* ingress multicast */
            dpp_mcds_t *mcds = dpp_get_mcds(unit);
            arad_mcdb_entry_t *mcdb_entry;
            uint32 destination;
            uint32 cud;
            uint32 next_entry = mc_id;

            LOG_CLI((BSL_META_U(unit, "Replication information: \n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |                     port                |               CUD             |\n\r")));
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));

            while ((next_entry > 0) && (next_entry != 0x3FFFF) ) {
                mcdb_entry = MCDS_GET_MCDB_ENTRY(mcds, next_entry);
                destination = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, DESTINATIONf);
                cud = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, OUT_LIFf);
                next_entry = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_entry, LINK_PTRf);

                LOG_CLI((BSL_META_U(unit, "    |            ")));
                cmd_ppd_api_diag_pp_destination_19bits_print(unit, destination);
                LOG_CLI((BSL_META_U(unit, "           |            0x%05X            |\n\r"), cud));
            }
            LOG_CLI((BSL_META_U(unit, "    |-------------------------------------------------------------------------|\n\r")));
        }
    }

    return CMD_OK;

exit:
    (void)_rv;
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_fec_info_print(int unit, int core, uint32 fec_offset, uint32 lb_key, uint32 tc);

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_last_trap_additional_info_print(int unit, SOC_PPC_FRWRD_DECISION_TYPE_INFO *info)
{
    switch (info->outlif.type) {
        case SOC_PPC_OUTLIF_ENCODE_TYPE_RAW:
            LOG_CLI((BSL_META_U(unit, "    OutLIF raw val %d\n\r"), info->outlif.val));
            break;
        case SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID:
            LOG_CLI((BSL_META_U(unit, "    OutLIF raw invalid, val %d\n\r"), info->outlif.val));
            break;
        default:
            break;
    }

    switch (info->eei.type) {
        case SOC_PPC_EEI_TYPE_MIM:
            LOG_CLI((BSL_META_U(unit, "    EEI MiM, ISID = %d\n\r"), info->eei.val.isid));
            break;
        case SOC_PPC_EEI_TYPE_TRILL:
            LOG_CLI((BSL_META_U(unit, "    EEI TRILL dest nick, val %d\n\r"), info->eei.val.trill_dest.dest_nick));
            break;
        case SOC_PPC_EEI_TYPE_MPLS:
            {
                switch (info->eei.val.mpls_command.command) {
                    case SOC_PPC_MPLS_COMMAND_TYPE_PUSH:
                        LOG_CLI((BSL_META_U(unit, "    EEI MPLS push, push profile %d, label %d\n\r"), 
                                            info->eei.val.mpls_command.push_profile, 
                                            info->eei.val.mpls_command.label));
                        break;
                    case SOC_PPC_EEI_IDENTIFIER_SWAP_VAL:
                        LOG_CLI((BSL_META_U(unit, "    EEI MPLS swap, label %d\n\r"), 
                                            info->eei.val.mpls_command.label));
                        break;
                    case SOC_PPC_EEI_IDENTIFIER_POP_VAL:
                        LOG_CLI((BSL_META_U(unit, "    EEI MPLS pop, pop next header = %d, has cw = %d, model = %s tpid profile = %d\n\r"), 
                                            info->eei.val.mpls_command.pop_next_header,
                                            info->eei.val.mpls_command.has_cw,
                                            (info->eei.val.mpls_command.model == SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE)?"pipe":"uniform",
                                            info->eei.val.mpls_command.tpid_profile));
                        break;
                    default:
                        break;
                }
            }
            break;
        case SOC_PPC_EEI_TYPE_OUTLIF:
            LOG_CLI((BSL_META_U(unit, "    EEI OutLIF, val %d\n\r"), info->eei.val.outlif));
            break;
        default:
            break;
    }

    return CMD_OK;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_fwd_info_print(int unit, int core, uint32 fwd_type, uint32 fwd_dest, uint32 lb_key, uint32 tc)
{
    uint32 rv = CMD_OK;
    uint32 soc_sand_rv;
  
    switch (fwd_type) {
        case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
            LOG_CLI((BSL_META_U(unit, "The packet is \033[31mdropped\033[0m.\n\r")));
        break;
        case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
            {
                uint32 base_queue;
            
                rv = cmd_ppd_api_diag_pp_last_flow_to_q(unit, core, fwd_dest, tc, &base_queue);

                if (rv == CMD_OK) {
                    LOG_CLI((BSL_META_U(unit, "The packet is forwarded to flow %d, queue %d.\n\r"), fwd_dest, base_queue));
                }
                else {
                    LOG_CLI((BSL_META_U(unit, "The packet is forwarded to flow %d.\n\r"), fwd_dest));
                }
            }
        break;
        case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
            {
                uint32 lag_member = 0;
                uint32 base_queue = 0;
            
                rv = cmd_ppd_api_diag_pp_last_lag_get_member_and_q(unit, core, fwd_dest, lb_key, tc, &lag_member, &base_queue);

                if (rv == CMD_OK) {
                    LOG_CLI((BSL_META_U(unit, "The packet is forwarded to LAG %d port %d, queue %d.\n\r"), fwd_dest, lag_member, base_queue));
                }
                else {
                    LOG_CLI((BSL_META_U(unit, "The packet is forwarded to LAG %d port %d.\033[31Q not valid\033[0m.\n\r"), fwd_dest, lag_member));
                }
            }
        break;
        case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
        {
            uint32 base_queue;

            rv = cmd_ppd_api_diag_pp_last_uc_port_to_q(unit, core, fwd_dest, tc, &base_queue);

            if (rv == CMD_OK) {
                LOG_CLI((BSL_META_U(unit, "The packet is forwarded to port %d, queue %d.\n\r"), fwd_dest, base_queue));
            }
            else {
                LOG_CLI((BSL_META_U(unit, "The packet is forwarded to port %d.\033[31Q not valid\033[0m.\n\r"), fwd_dest));
            }
        }
        break;
        case SOC_PPC_FRWRD_DECISION_TYPE_MC:
            {                    
                LOG_CLI((BSL_META_U(unit, "The packet is forwarded to MC group %d.\n\r"), fwd_dest));
                cmd_ppd_api_diag_pp_mc_info_print(unit, core, fwd_dest);
            }
        break;
        case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
            {
                cmd_ppd_api_diag_pp_fec_info_print(unit, core, fwd_dest, lb_key, tc);
            }
        break;
        case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
            {
                SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
                int trap_id_sw;

                if (_bcm_dpp_rx_trap_hw_id_to_sw_id(unit, fwd_dest, &trap_id_sw) == BCM_E_NONE) {
                    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit, trap_id_sw, &profile_info);
                    rv = handle_sand_result(soc_sand_rv);

                    if (rv == BCM_E_NONE) {
                        LOG_CLI((BSL_META_U(unit, "Trap forward information:\n\r")));
                        LOG_CLI((BSL_META_U(unit, "-------------------------> \n\r")));

                        if (profile_info.dest_info.add_vsi) {
                            LOG_CLI((BSL_META_U(unit, "Add VSI, VSI shift = %d \n\r"), profile_info.dest_info.vsi_shift));
                        }

                        cmd_ppd_api_diag_pp_fwd_info_print(unit, core, 
                                                            profile_info.dest_info.frwrd_dest.type, 
                                                            profile_info.dest_info.frwrd_dest.dest_id, lb_key, tc);
                        cmd_ppd_api_diag_pp_last_trap_additional_info_print(unit, &profile_info.dest_info.frwrd_dest.additional_info);
                        LOG_CLI((BSL_META_U(unit, "<------------------------- \n\r")));
                    }
                    
                }
            }
        break;
        default:
        break;
    }
    
    return rv;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_fec_info_print(int unit, int core, uint32 fec_offset, uint32 lb_key, uint32 tc)
{
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE protect_type;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO working_fec;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO protect_info;
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = soc_ppd_frwrd_fec_entry_get(unit, fec_offset, &protect_type, &working_fec, &protect_fec, &protect_info);
    BCM_SAND_IF_ERR_EXIT(rv);

    LOG_CLI((BSL_META_U(unit, "The packet is forwarded to FEC %d: \n\r"), fec_offset));
    LOG_CLI((BSL_META_U(unit, "---------------------------------> \n\r")));

    /* always working fec */
    cmd_ppd_api_diag_pp_fwd_info_print(unit, core, working_fec.dest.dest_type, working_fec.dest.dest_val, lb_key, tc);
  
    switch (working_fec.type) {
        case SOC_PPC_FEC_TYPE_DROP:
        case SOC_PPC_FEC_TYPE_SIMPLE_DEST:
        case SOC_PPC_NOF_FEC_TYPES_ARAD:/* SOC_PPC_NOF_FEC_TYPES_ARAD for remove */
            break;
        case SOC_PPC_FEC_TYPE_IP_MC: 
            LOG_CLI((BSL_META_U(unit, "    IP MC, expected InRIF = %d \n\r"), working_fec.rpf_info.expected_in_rif));
            break;
        case SOC_PPC_FEC_TYPE_IP_UC:  
        case SOC_PPC_FEC_TYPE_ROUTING:
            LOG_CLI((BSL_META_U(unit, "    IP UC, ARP ptr = %d, OutRIF = %d \n\r"), working_fec.eep, working_fec.app_info.out_rif));
            break;
        case SOC_PPC_FEC_TYPE_TRILL_MC:  
            /* nothing to do, handled in destination get */
        break;
        case SOC_PPC_FEC_TYPE_BRIDGING_INTO_TUNNEL:
        case SOC_PPC_FEC_TYPE_TUNNELING:
        case SOC_PPC_FEC_TYPE_TRILL_UC:
        case SOC_PPC_FEC_TYPE_MPLS_LSR: 
        case SOC_PPC_FEC_TYPE_FORWARD:
            LOG_CLI((BSL_META_U(unit, "    EEP = %d \n\r"), working_fec.eep));
        break;
        case SOC_PPC_FEC_TYPE_BRIDGING_WITH_AC:
            LOG_CLI((BSL_META_U(unit, "    OutAC = %d \n\r"), working_fec.app_info.out_ac_id));
        break;
        default:
        break;
    }

    LOG_CLI((BSL_META_U(unit, "<---------------------------------\n\r")));
    
exit:
    (void)_rv;
    return CMD_FAIL;
}

STATIC cmd_result_t
    cmd_ppd_api_diag_pp_snp_mrr_info_print(int unit, int core, uint32 cmd, uint32 is_snoop)
{
    uint32 data[5] = {0};
    uint32 destination;
    uint32
      res = SOC_SAND_OK;
    soc_mem_t 
        mirror_mem = (SOC_IS_QAX(unit) ? TAR_SNOOP_MIRROR_DEST_TABLEm : IRR_SNOOP_MIRROR_DEST_TABLEm);

    res = soc_mem_read(unit, mirror_mem, MEM_BLOCK_ANY, cmd + is_snoop * 16, data);

    if (res != SOC_E_NONE) {
        return CMD_FAIL;
    }

    destination = soc_mem_field32_get(unit, mirror_mem, data, DESTINATIONf);

    LOG_CLI((BSL_META_U(unit, "The packet has a %s copy. The packet is %sed to "), is_snoop?"snoop":"mirror", is_snoop?"snoop":"mirror"));
    cmd_ppd_api_diag_pp_destination_19bits_print(unit, destination);
    LOG_CLI((BSL_META_U(unit, "\n\r")));

    return CMD_OK;
}

cmd_result_t
    cmd_ppd_api_diag_pp_last(int unit, args_t* a)
{
    parse_table_t    pt;
    int prm_core = 0;
    int print_details = 0;
    int32 params_num;
    cmd_result_t cmd_result;
    uint32 reg_val;
    uint32 valid;
    uint32 trap_code;
    uint32
      res = SOC_SAND_OK;
    cmd_diag_pp_packet_action_t packet_action[diag_irpp_stages];

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL | PQ_INT, 0,    &prm_core,    NULL);
    parse_table_add(&pt, "Detailed", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, 0,    &print_details,    NULL);

    params_num = parse_arg_eq(a, &pt);

    if (0 > params_num) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if ((prm_core < 0 ) || (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
    }

    sal_memset(packet_action, 0, sizeof(packet_action));

    LOG_CLI((BSL_META_U(unit, "                          \033[1m\033[4mLast packet IRPP information:\033[0m \n\r")));

    LOG_CLI((BSL_META_U(unit, "     -------------------------------------------------------------------------\n\r")));

    if ((cmd_result = cmd_ppd_api_diag_pp_last_port_termination(unit, prm_core)) != CMD_OK) {
        return cmd_result;
    }    


    if ((cmd_result = cmd_ppd_api_diag_pp_last_parser(unit, prm_core, &packet_action[diag_irpp_stage_parser], print_details)) != CMD_OK) {
        return cmd_result;
    }    

    /* VTT lookups */
    if (print_details) {
        if ((cmd_result = cmd_ppd_api_diag_pp_last_vtt_lookups(unit, prm_core)) != CMD_OK) {
            return cmd_result;
        }     
    }

    /* VTT */
    if ((cmd_result = cmd_ppd_api_diag_pp_last_vtt(unit, prm_core, print_details)) != CMD_OK) {
        return cmd_result;
    }     

    /* Tunnel Termination information */
    if ((cmd_result = cmd_ppd_api_diag_pp_last_tt(unit, prm_core, &packet_action[diag_irpp_stage_tt], print_details)) != CMD_OK) {
        return cmd_result;
    }     

    /* FLP */
    if ((cmd_result = cmd_ppd_api_diag_pp_last_flp(unit, prm_core, &packet_action[diag_irpp_stage_flp], print_details)) != CMD_OK) {
        return cmd_result;
    }     

    /* PMF */
    if ((cmd_result = cmd_ppd_api_diag_pp_last_pmf(unit, prm_core, &packet_action[diag_irpp_stage_pmf], print_details)) != CMD_OK) {
        return cmd_result;
    }     

    /* FEC */ 
    if ((cmd_result = cmd_ppd_api_diag_pp_last_fec(unit, prm_core, &packet_action[diag_irpp_stage_fec], print_details)) != CMD_OK) {
        return cmd_result;
    }     

    /* TM resolution */  
    if ((cmd_result = cmd_ppd_api_diag_pp_last_tm(unit, prm_core, &packet_action[diag_irpp_stage_tm], print_details)) != CMD_OK) {
        return cmd_result;
    }     

    /* Summary */
    {   
        DECLARE_PACKET_ACTION_DESC();
        DECLARE_IRPP_STAGE_DESC();

        cmd_diag_pp_irpp_stage_e stage;

        LOG_CLI((BSL_META_U(unit, "\n\r")));

        cmd_ppd_api_diag_pp_fwd_info_print(unit, prm_core, 
                                            packet_action[diag_irpp_stage_tm].dest_type, 
                                            packet_action[diag_irpp_stage_tm].dest, 
                                            packet_action[diag_irpp_stage_tm].lag_lb_key, 
                                            packet_action[diag_irpp_stage_tm].tc);

        if (packet_action[diag_irpp_stage_tm].mirror > 0) {
            cmd_ppd_api_diag_pp_snp_mrr_info_print(unit, prm_core, packet_action[diag_irpp_stage_tm].mirror, 0);
        }

        if (packet_action[diag_irpp_stage_tm].snoop_cmd > 0) {
            cmd_ppd_api_diag_pp_snp_mrr_info_print(unit, prm_core, packet_action[diag_irpp_stage_tm].snoop_cmd , 1);
        }
        
        res = READ_IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr(unit, prm_core, &reg_val);

        if (res == SOC_E_NONE) {
            /* is valid */
            valid = 0;
            valid = soc_reg_field_get(unit, IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr, reg_val, DBG_LAST_TRAP_CHANGE_DESTINATION_VALIDf);

            if (valid) {
                /* get trap Code */
                trap_code = soc_reg_field_get(unit, IHB_DBG_LAST_TRAP_CHANGE_DESTINATION_REGr, reg_val, DBG_LAST_TRAP_CHANGE_DESTINATIONf);
                LOG_CLI((BSL_META_U(unit, "Trap %d (%s/%s) has changed the destination.\n\r"), trap_code, 
                    cmd_ppd_api_diag_pp_last_trap_code_desc(trap_code),
                    cmd_ppd_api_diag_pp_last_trap_code_to_bcm_trap(trap_code)));       
                cmd_ppd_api_diag_pp_fwd_info_print(unit, prm_core, 
                    SOC_PPC_FRWRD_DECISION_TYPE_TRAP, 
                    trap_code, 
                    packet_action[diag_irpp_stage_tm].lag_lb_key, 
                    packet_action[diag_irpp_stage_tm].tc);
            }
        }

        if (print_details) {
            for (stage=diag_irpp_stage_port_termination; stage<diag_irpp_stage_tm; ++stage) {
                int field;
            
                for (field=0; field<CMD_DIAG_PP_PACKET_ACTION_NUM_FIELDS; ++field) {
                    if ( (((0x1L << field) == CMD_DIAG_PP_SNOOP_VALID) && (packet_action[diag_irpp_stage_tm].snoop_cmd == 0)) ||
                        (((0x1L << field) == CMD_DIAG_PP_MIRROR_VALID) && (packet_action[diag_irpp_stage_tm].mirror == 0))) {
                        break;
                    }
                    
                    if ((packet_action[stage].flags & (0x1L << field)) && (packet_action[diag_irpp_stage_tm].flags & (0x1L << field))) {
                        if (((uint32 *)&packet_action[stage])[field] == ((uint32 *)&packet_action[diag_irpp_stage_tm])[field]) {
                            LOG_CLI((BSL_META_U(unit, "%-16s  (1st) assigned by %s\033[0m\n\r"), PACKET_ACTION_DESC(field), IRPP_STAGE_DESC(stage)));
                            packet_action[diag_irpp_stage_tm].flags &= ~((0x1L) << field);
                        }
                    }
                }
            }
        }
    }

    return CMD_OK;
}

/********************************************************************* 
 *  Function handler:  tpid_globals_get (section diag)
 */

cmd_result_t
    cmd_ppd_api_diag_tpid_globals_get(int unit, args_t* a) 
{   
  uint32
      ret; 

  SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
  ret = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    return CMD_FAIL;
  } 

  SOC_PPC_LLP_PARSE_TPID_VALUES_print(&tpid_vals);
 
  return CMD_OK;

} 

/********************************************************************* 
 *  Function handler:  tpid_profiles_get (section diag)
 */
cmd_result_t
    cmd_ppd_api_diag_tpid_profiles_get(int unit, args_t* a)
{   
  uint32 
      ret; 
  uint32
      profile;

  for (profile=0; profile<_BCM_PORT_NOF_TPID_PROFILES; profile++) {

      SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO tpid_profile_info;

      ret = soc_ppd_llp_parse_tpid_profile_info_get(unit, profile, &tpid_profile_info);

      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
        return CMD_FAIL;
      } 

      LOG_CLI((BSL_META_U(unit, "\nprofile %u:\n"), profile));
      SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_print(&tpid_profile_info);
      LOG_CLI((BSL_META_U(unit, "\n")));
  }
 
  return CMD_OK;
}


void print_fec_usage(int unit)
{
    char cmd_dpp_diag_pp_fec_usage[] =
    "Usage (DIAG pp FEC):"
    "\n\tDIAGnotsics FEC commands\n\t"
    "Usages:\n\t"
    "DIAG pp FEC [OPTION] <parameters> ..."
    "OPTION can be:"
    "\nALL                                   - Returns all allocated FEC."
    "\nLaste_Fec                             - Returns Last FEC ID in the system."
    "\nALLOCation                            - fec allocation application example."
    "\nPERformance <ecmp_count>              - allocating 256 Fecs per ecmp and printing time.";

    LOG_CLI((BSL_META_U(unit, "%s\n\n"),cmd_dpp_diag_pp_fec_usage));
}

cmd_result_t
    cmd_ppd_api_diag_fec_get(int unit, args_t* a)
{   
    char    *function;

    function = ARG_GET(a);

    if (! function ) {
        print_fec_usage(unit);
        return CMD_OK;
    } else if (DIAG_FUNC_STR_MATCH(function, "Last_Fec", "lf")) {            
        cmd_diag_fec_last(unit, 0);
        cmd_diag_fec_last(unit, 1);
    } else if (DIAG_FUNC_STR_MATCH(function, "all", "all")) {
        cmd_diag_fec_print_all(unit);
    } else if (DIAG_FUNC_STR_MATCH(function, "ALLOCation", "alloc")) {
        cmd_diag_fec_allocation(unit);
    } else if (DIAG_FUNC_STR_MATCH(function, "PERformance", "per")) {
        cmd_diag_fec_performance(unit,a);
    }else{
        print_fec_usage(unit);
        return CMD_OK;
    }

    return CMD_OK;
}     
  
/********************************************************************* 
 *  Function handler:  tpid_port_profile_get (section diag)
 */
  
cmd_result_t 
    cmd_ppd_api_diag_tpid_port_profile_get(int unit, args_t* a)
{
    SOC_PPC_PORT soc_ppd_port_i;
    SOC_PPC_PORT_INFO port_info;

    int              port_id = -1;
    int              prm_core = 0;
    parse_table_t    pt; 

    uint32 
      ret;

    SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO tpid_profile_info;
    SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;

    /* Get parameters */ 

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "port_id", PQ_DFL|PQ_IP, &port_id, &port_id, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);


    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
         return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

	if ((prm_core < 0 ) 
		|| 
	  (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
		cli_out("error ilegal core ID for device\n");
		return CMD_FAIL;
	}

    if (port_id <= -1) 
    {
        /* If port not specified, return profiles of all ports*/
        int port;
        for (port = 0; port< SOC_DPP_DEFS_GET(unit,nof_local_ports); port++) {
            soc_ppd_port_i = port;
            soc_ppd_port_info_get(unit,prm_core,soc_ppd_port_i, &port_info);
            LOG_CLI((BSL_META_U(unit, "port: %u : "), port));
            LOG_CLI((BSL_META_U(unit, "tpid_profile: %u\n"), port_info.tpid_profile));
        }
    }
    else
    {
        LOG_CLI((BSL_META_U(unit, "port: %u\n"), port_id));

        /* Profile index */
        soc_ppd_port_i = port_id;
        soc_ppd_port_info_get(unit,prm_core,soc_ppd_port_i, &port_info);
        LOG_CLI((BSL_META_U(unit, "tpid_profile: %u\n\n"), port_info.tpid_profile));

        /* Profile */
        ret = soc_ppd_llp_parse_tpid_profile_info_get(unit, port_info.tpid_profile, &tpid_profile_info);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
        { 
            return CMD_FAIL;
        }
        SOC_PPC_LLP_PARSE_TPID_PROFILE_INFO_print(&tpid_profile_info);

        /* Tpid vals of profile */
        ret = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
        { 
            return CMD_FAIL;
        }
        LOG_CLI((BSL_META_U(unit, "\n\ntpid index %u: 0x%04x\n"), tpid_profile_info.tpid1.index, tpid_vals.tpid_vals[tpid_profile_info.tpid1.index]));
        LOG_CLI((BSL_META_U(unit, "tpid index %u: 0x%04x\n"), tpid_profile_info.tpid2.index, tpid_vals.tpid_vals[tpid_profile_info.tpid2.index]));
    }
    return CMD_OK;
}

int
cmd_ppd_api_diag_vtt_dump_get(int unit, args_t* a) 
{ 
    uint32         soc_sand_dev_id; 
    parse_table_t  pt;
    uint32         default_val = 0;
    uint32         last_program = 0;
    uint32         res = SOC_SAND_OK;
    int prog_id_vt, prog_id_tt, num_of_progs_vt, num_of_progs_tt;
    int             prm_core=0;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

	if ((prm_core < 0 ) 
        || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
    }

    soc_sand_dev_id = (unit); 

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_isem_access_print_last_vtt_program_data(soc_sand_dev_id, prm_core, 1, &prog_id_vt, &prog_id_tt, &num_of_progs_vt, &num_of_progs_tt);
        /* check if previous function return an error value */
        if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) { 
            return CMD_FAIL; 
        }
    } else {
        arad_pp_isem_access_print_all_programs_data(soc_sand_dev_id);
    }

    return CMD_OK;
}

int
cmd_ppd_api_diag_flp_dump_get(int unit, args_t* a) 
{ 
    uint32
        soc_sand_dev_id; 
    parse_table_t  pt;
    uint32         default_val = 0;
    uint32         last_program = 0;
    uint32         res = SOC_SAND_OK;
    int prog_id, num_of_progs;
    int            prm_core=0;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

	if ((prm_core < 0 )|| (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
    }

    soc_sand_dev_id = (unit); 

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_flp_access_print_last_programs_data(soc_sand_dev_id, prm_core, 1, &prog_id, &num_of_progs);
        /* check if previous function return an error value */
        if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) { 
            return CMD_FAIL; 
        }
    } else {
        arad_pp_flp_access_print_all_programs_data(soc_sand_dev_id);
    }
  
  return CMD_OK;
}

/******************************************************************** 
 *  Function handler: egress_vlan_edit_info_get (section diag)
 */
int dnx_diag_egress_vlan_edit_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *egress_vlan_edit_info_per_core);
int 
cmd_ppd_api_diag_egress_vlan_edit_info_get(int unit, args_t* a) 
{
  uint32
    soc_sand_dev_id; 
  uint32 
    ret; 
  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO
    prm_vec_res;
  int              prm_core = -1;
  parse_table_t    pt;
  SOC_PPC_DIAG_RESULT                     ret_val;
  int                                     core, first_core, last_core;
  /*
   * For printing to xml file
   */
  char             *prm_file = NULL;
#if !defined(NO_FILEIO)
  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO            egress_vlan_edit_info_per_core[SOC_DPP_DEFS_MAX(NOF_CORES)];
  int               egress_vlan_edit_info_per_core_len = 0;
#endif
  uint8             b_print_to_xml = 0;


  soc_sand_dev_id = (unit); 
  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(&prm_vec_res);

  /* Get parameters */ 
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
  parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0,  &prm_file, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  if ((prm_core < -1 ) || 
      (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
      cli_out("error ilegal core ID for device\n");
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
  }
  if ((prm_file != NULL) && (dbx_file_get_type(prm_file) == DBX_FILE_XML)) {
      b_print_to_xml = 1;
  }

  if ((prm_core == -1) || b_print_to_xml) {
      /*
       * If printing to xml or core param not used, read from all cores.
       */
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core+1;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_egress_vlan_edit_info_get(soc_sand_dev_id, core, &prm_vec_res, &ret_val);

      /* check if previous function return an error value */
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
            parse_arg_eq_done(&pt);
            return CMD_FAIL; 
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
          LOG_CLI((BSL_META_U(unit, "%s Not found\n\r"),get_core_str(unit, core)));
          continue; 
      } 

      /* retrieve edit profile format (is ETAG?) */
      if (SOC_IS_ARADPLUS(unit)) {
          ret = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_eve_etag_format_get,(unit, prm_vec_res.key.edit_profile , &prm_vec_res.cmd_info.is_extender));
          if(SOC_SAND_FAILURE(ret)) {
              parse_arg_eq_done(&pt);
              return CMD_FAIL;
          }
      }

      if (unit >= SOC_MAX_NUM_DEVICES) {
          parse_arg_eq_done(&pt);
          return CMD_FAIL; 
      }
      if (!b_print_to_xml) {
          cli_out("%s\n", get_core_str(unit, core));
          SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_print(&prm_vec_res);
      } else {
#if !defined(NO_FILEIO)
          egress_vlan_edit_info_per_core[egress_vlan_edit_info_per_core_len++] = prm_vec_res;
#endif
      }

      if (!b_print_to_xml) {
          /* retrieve packet_is_tagged_after_eve indication */
          if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
              if (!SOC_IS_JERICHO_PLUS(unit)) {
                  ret = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_vlan_edit_packet_is_tagged_get, (unit,&prm_vec_res.key,&prm_vec_res.cmd_info.packet_is_tagged_after_eve));
                  if(SOC_SAND_FAILURE(ret)) {
                      parse_arg_eq_done(&pt);
                      return CMD_FAIL;
                  }
              }
              LOG_CLI((BSL_META_U(unit, "\npacket_is_tagged_after_eve: %u\n\r"), prm_vec_res.cmd_info.packet_is_tagged_after_eve));
          }
      }
  }
  if (b_print_to_xml) {
      /*
       * Print to XML file
       */
#if !defined(NO_FILEIO)
      dnx_diag_egress_vlan_edit_info_xml_print(unit, prm_file, egress_vlan_edit_info_per_core);
#endif
  }

  parse_arg_eq_done(&pt);
  return CMD_OK;

}

#if !defined(NO_FILEIO)
int dnx_diag_egress_vlan_edit_info_xml_print(int unit, char *xml_file, SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO *egress_vlan_edit_info_per_core) {

    int core = 0;
    uint32 ret;
    xml_node curTop, curDiags, curDiag, curCores, curCore, commandKey, curCommand, innerTag, outerTag, acTag, acTag2 ;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get diags root
     */
    SHR_IF_ERR_EXIT(dnx_diag_xml_root_get(unit, xml_file, &curTop, &curDiags));
    {
       /*
        * Add xml structure:
        * <diag cat="pp" name="egress_vlan_edit">
        *     <cores list="Yes">
        *         <core id="0" adv_mode="0">
        *             <command_key edit_profile="2" tag_format="s_tag"/>
        *             <command is_extender="0" tags_to_remove="1" cep_editing="0">
        *                 <inner_tag tpid_index="0" vid_source="empty" pcp_dei_source="none"/>
        *                 <outer_tag tpid_index="0" vid_source="vid_edit_outer" pcp_dei_source="outer_tag"/>
        *             </command>
        *             <ac_tag tpid="0x0" vid="20" pcp="0" dei="0"/>
        *             <ac_tag2 tpid="0x0" vid="20" pcp="0" dei="0"/>
        *         </core>
        *         <core id="1" adv_mode="0">
        *             <command_key edit_profile="2" tag_format="none"/>
        *             <command is_extender="0" tags_to_remove="0" cep_editing="0">
        *                 <inner_tag tpid_index="0" vid_source="empty" pcp_dei_source="none"/>
        *                 <outer_tag tpid_index="0" vid_source="vsi" pcp_dei_source="map"/>
        *             </command>
        *             <ac_tag tpid="0x0" vid="20" pcp="0" dei="0"/>
        *             <ac_tag2 tpid="0x0" vid="20" pcp="0" dei="0"/>
        *         </core>
        *     </cores>
        * </diag>
 */

        /*
         * diag node
         */
        SHR_NULL_CHECK((curDiag = dbx_xml_child_add(curDiags, "diag", 2)), _SHR_E_FAIL, "curDiag");
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "cat", "pp"));
        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curDiag, "name", "egress_vlan_edit"));
        {
            /*
             * cores node
             */
            SHR_NULL_CHECK((curCores = dbx_xml_child_add(curDiag, "cores", 3)), _SHR_E_FAIL, "curCores");
            SHR_IF_ERR_EXIT(dbx_xml_property_set_str(curCores, "list", "Yes"));
            /*
             * core list
             */
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                /*
                 * core node
                 */
                SHR_NULL_CHECK((curCore = dbx_xml_child_add(curCores, "core", 4)), _SHR_E_FAIL, "curCore");
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "id", core));
                SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "adv_mode", egress_vlan_edit_info_per_core[core].cmd_info.packet_is_tagged_after_eve));
                {
                    SHR_NULL_CHECK((commandKey = dbx_xml_child_add(curCore, "command_key", 5)), _SHR_E_FAIL, "commandKey");
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(commandKey, "edit_profile", egress_vlan_edit_info_per_core[core].key.edit_profile));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(commandKey, "tag_format", (char *)soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(egress_vlan_edit_info_per_core[core].key.tag_format)));

                    SHR_NULL_CHECK((curCommand = dbx_xml_child_add(curCore, "command", 5)), _SHR_E_FAIL, "curCommand");
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCommand, "is_extender", egress_vlan_edit_info_per_core[core].cmd_info.is_extender));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCommand, "tags_to_remove", egress_vlan_edit_info_per_core[core].cmd_info.tags_to_remove));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCommand, "cep_editing", egress_vlan_edit_info_per_core[core].cmd_info.cep_editing));
                    {
                        SHR_NULL_CHECK((innerTag = dbx_xml_child_add(curCommand, "inner_tag", 6)), _SHR_E_FAIL, "innerTag");
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(innerTag, "tpid_index", egress_vlan_edit_info_per_core[core].cmd_info.inner_tag.tpid_index));
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(innerTag, "vid_source", (char *) SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(egress_vlan_edit_info_per_core[core].cmd_info.inner_tag.vid_source)));
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(innerTag, "pcp_dei_source",(char *) SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(egress_vlan_edit_info_per_core[core].cmd_info.inner_tag.pcp_dei_source)));

                        SHR_NULL_CHECK((outerTag = dbx_xml_child_add(curCommand, "outer_tag", 6)), _SHR_E_FAIL, "outerTag");
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_int(outerTag, "tpid_index", egress_vlan_edit_info_per_core[core].cmd_info.outer_tag.tpid_index));
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(outerTag, "vid_source", (char *) SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_to_string(egress_vlan_edit_info_per_core[core].cmd_info.outer_tag.vid_source)));
                        SHR_IF_ERR_EXIT(dbx_xml_property_set_str(outerTag, "pcp_dei_source", (char *) SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(egress_vlan_edit_info_per_core[core].cmd_info.outer_tag.pcp_dei_source)));
                    }
                    dbx_xml_node_end(curCommand, 5);

                    SHR_NULL_CHECK((acTag = dbx_xml_child_add(curCore, "ac_tag", 5)), _SHR_E_FAIL, "acTag");
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(acTag, "tpid", egress_vlan_edit_info_per_core[core].ac_tag.tpid));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(acTag, "vid", egress_vlan_edit_info_per_core[core].ac_tag.vid));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(acTag, "pcp", egress_vlan_edit_info_per_core[core].ac_tag.pcp));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(acTag, "dei", egress_vlan_edit_info_per_core[core].ac_tag.dei));

                    SHR_NULL_CHECK((acTag2 = dbx_xml_child_add(curCore, "ac_tag2", 5)), _SHR_E_FAIL, "acTag2");
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(acTag2, "tpid", egress_vlan_edit_info_per_core[core].ac_tag2.tpid));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(acTag2, "vid", egress_vlan_edit_info_per_core[core].ac_tag2.vid));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(acTag2, "pcp", egress_vlan_edit_info_per_core[core].ac_tag2.pcp));
                    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(acTag2, "dei", egress_vlan_edit_info_per_core[core].ac_tag2.dei));
                }

                /* retrieve packet_is_tagged_after_eve indication */
              if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
                  if (!SOC_IS_JERICHO_PLUS(unit)) {
                      ret = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_vlan_edit_packet_is_tagged_get, (unit,&egress_vlan_edit_info_per_core[core].key, &egress_vlan_edit_info_per_core[core].cmd_info.packet_is_tagged_after_eve));
                      if(SOC_SAND_FAILURE(ret)) {
                          return CMD_FAIL;
                      }
                  }
                  SHR_IF_ERR_EXIT(dbx_xml_property_set_int(curCore, "packet_is_tagged_after_eve", egress_vlan_edit_info_per_core[core].cmd_info.packet_is_tagged_after_eve));
              }
                dbx_xml_node_end(curCore, 4);
            }
            dbx_xml_node_end(curCores, 3);
        }
        dbx_xml_node_end(curDiag, 2);
    }
    dbx_xml_node_end(curDiags, 1);

    dbx_xml_top_save(curTop, xml_file);
    dbx_xml_top_close(curTop);

exit:
    SHR_FUNC_EXIT;
}
#endif

/******************************************************************** 
 *  Function handler: cmd_diag_gport_info_get (section diag)
 */
int 
cmd_diag_gport_info_get(int unit, args_t* a) 
{
  uint32 
    ret; 
  parse_table_t    
    pt;
  DIAG_ALLOC_GPORT_PARAMS 
    gport_params;
  DIAG_ALLOC_GPORT_INFO
    gport_info;


  /* init gport params and infos */
  sal_memset(&gport_params, 0x0, sizeof(DIAG_ALLOC_GPORT_PARAMS));
  sal_memset(&gport_info, 0x0, sizeof(DIAG_ALLOC_GPORT_INFO));

  /* by default options for display information are enabled */
  gport_params.forwarding_database = 1;
  gport_params.lif_database = 1;
  gport_params.resource = 1;


  parse_table_init(unit, &pt);
  parse_table_add(&pt, "id", PQ_DFL|PQ_INT, 0, &gport_params.gport_id, NULL);
  parse_table_add(&pt, "RES", PQ_DFL|PQ_INT, 0, &gport_params.resource, NULL);
  parse_table_add(&pt, "FW_DB", PQ_DFL|PQ_INT, 0, &gport_params.forwarding_database, NULL);
  parse_table_add(&pt, "LIF_DB", PQ_DFL|PQ_INT, 0, &gport_params.lif_database, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }

  if (gport_params.gport_id <=0) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }

  /* Call function */
  gport_info.forwarding_info_data = sal_alloc(sizeof(_BCM_GPORT_PHY_PORT_INFO), "tmp gport");
  ret = diag_alloc_gport_info_get(unit, &gport_params, &gport_info);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
        parse_arg_eq_done(&pt);
        return CMD_FAIL; 
  } 

  diag_alloc_gport_info_print(unit, &gport_params, &gport_info);

  sal_free(gport_info.forwarding_info_data);

  parse_arg_eq_done(&pt);
  return CMD_OK;
}




/******************************************************************** 
 *  Function handler: cmd_bcm_cross_connect_get (section diag)
 */

/* helper function for cmd_bcm_cross_connect_print_cb, print gport and encap value */
void cmd_bcm_cross_connect_gport_encap_print_cb(int unit, bcm_gport_t gport,  int encap) {
    cmd_result_t result;
    bcm_error_t rv;
    SOC_PPC_FRWRD_DECISION_INFO fwd_decision;
    DIAG_ALLOC_GPORT_PARAMS     gport_params;
    DIAG_ALLOC_GPORT_INFO       gport_info;

    if (diag_alloc_gport_type_is_supported(gport)) {
        /* print encoding and id like in diag pp GP */
        /* init gport params and infos */
        diag_alloc_gport_param_init(&gport_params);
        diag_alloc_gport_info_init(&gport_info);

        gport_params.gport_id = gport;

        /* Call function */
        result = diag_alloc_gport_info_get(unit, &gport_params, &gport_info);

        if (soc_sand_get_error_code_from_error_word(result) != SOC_SAND_OK) { 
            return; 
        } 
         cli_out("   ");
        diag_alloc_gport_info_print(unit, &gport_params, &gport_info);
    } else {
        if (encap != 0 || BCM_GPORT_IS_FORWARD_PORT(gport)) {
            cli_out("   phy gport: 0x%-10x\n", gport);
            cli_out("   encap:     0x%-10x\n", encap);
        } else {
            SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);
            /* get frwrd decision */
            rv = _bcm_dpp_port_encap_to_fwd_decision(unit, gport, encap, &fwd_decision);
            if ( rv != BCM_E_NONE) {
                cli_out("error in _bcm_dpp_port_encap_to_fwd_decision\n");
                return;
            }
            cli_out("   phy gport: <%s,%d>\n", SOC_PPC_FRWRD_DECISION_TYPE_to_string(fwd_decision.type), fwd_decision.dest_id);
        }
    }

}

/* print gport and encap for both gports in cross connect */
int cmd_bcm_cross_connect_print_cb(int unit, 
    bcm_vswitch_cross_connect_t *gports, 
    void *user_data) {
    
    cli_out("gport1\n");
    cmd_bcm_cross_connect_gport_encap_print_cb(unit, gports->port1, gports->encap1);
    cli_out("gport2\n");
    cmd_bcm_cross_connect_gport_encap_print_cb(unit, gports->port2, gports->encap2);
    cli_out("\n");

    return BCM_E_NONE;
}


int 
cmd_bcm_cross_connect_get(int unit, args_t* a) 
{
  uint32 
    ret;
  int
    user_data; 

  /* call function */
  ret = bcm_vswitch_cross_connect_traverse(unit, cmd_bcm_cross_connect_print_cb, &user_data);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  {
       return CMD_FAIL;
  }
  return CMD_OK;
}


/******************************************************************** 
 *  Function handler: cmd_diag_occupation_profile_info_get (section diag)
 */

cmd_result_t
cmd_diag_egress_drop(int unit, args_t* a){
    uint32 
        ret;   
    SOC_PPC_DIAG_EG_DROP_LOG_INFO   
        prm_eg_drop_log;
   
    SOC_PPC_DIAG_EG_DROP_LOG_INFO_clear(&prm_eg_drop_log);

    /* Call function */
    ret = soc_ppd_diag_eg_drop_log_get(unit, &prm_eg_drop_log);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) { 
        cli_out(" *** soc_ppd_diag_eg_drop_log_get - FAIL"); 
        return CMD_FAIL;
    } 

    SOC_PPC_DIAG_EG_DROP_LOG_INFO_print(&prm_eg_drop_log);

    return CMD_OK;
}

cmd_result_t 
    cmd_diag_fec_allocation(int unit) {

    if (fec_allocation_main(unit, 1) != BCM_E_NONE)
    {
           return CMD_FAIL;
    }

    return CMD_OK;
}

void print_gport_part(int _gport){
    int a = -1, b = 0;
    char* type = "";
    if (a==-1){ 
        a=BCM_GPORT_LOCAL_GET(_gport);
        type ="local    ";
    }
    if (a==-1){ 
        a=BCM_GPORT_MODPORT_MODID_GET(_gport);
        b=BCM_GPORT_MODPORT_PORT_GET(_gport);
        type ="modport  ";
    }
    if (a==-1){ 
        a=BCM_GPORT_TRUNK_GET(_gport);
        type ="trunk    ";
    }
    if (a==-1){ 
        a=BCM_GPORT_MCAST_GET(_gport);
        type ="mact     ";
    }
    if (a==-1){ 
        a=BCM_GPORT_MPLS_PORT_ID_GET(_gport);
        type ="mpls_port";
    }
    if (a==-1){ 
        a=BCM_GPORT_VLAN_PORT_ID_GET(_gport);
        type ="vlan_port";
    }
    if (a==-1){ 
        a=BCM_GPORT_SYSTEM_PORT_ID_GET(_gport);
        type ="sys_port ";
    }
    if (a==-1){ 
        a=BCM_GPORT_MIRROR_GET(_gport);
    }
    cli_out("%s <%d,%d>", type, a, b);
}


cmd_result_t
    cmd_diag_fec_print(int unit, int fec_ndx, bcm_l3_egress_t* fec_obj)
{    
    /* TBD add info for :: BCM_L3_IPMC BCM_L3_RPF BCM_L3_CASCADED incapId*/
    
    cli_out("%6d |", fec_ndx);
    print_gport_part(fec_obj->port);
    cli_out(" |");

    if (fec_obj->failover_id != 0) {
        if (fec_obj->failover_if_id != 0) {
            cli_out("Primary   |");
        }else{
            cli_out("Secondary |");
        }
    }else{
        cli_out("Regular   |");
    }
        
    cli_out("%#9x |", fec_obj->intf);

    if(fec_obj->flags & BCM_L3_HIT){
        cli_out(" -YES-  |");
    }else{
        cli_out(" -NO-   |");
    }    
    return CMD_OK;
}

cmd_result_t 
    cmd_diag_fec_last(int unit, int core_id){

    int fec_ndx = 0, last_fec, first_fec;
    bcm_error_t status = BCM_E_NONE;
    cmd_result_t result = CMD_OK;
    bcm_l3_egress_t fec_obj;
    _bcm_dpp_am_fec_alloc_usage usage;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO routing_info;
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&routing_info);

    first_fec = SOC_DPP_DEFS_GET(unit, nof_ecmp);
    last_fec = SOC_DPP_DEFS_GET(unit, nof_fecs);

    cli_out("\nprinting Last FEC for core %d: \n-------------------------\n\n", core_id);

    soc_ppd_diag_frwrd_decision_get(unit, core_id, &payload.dest);    

    if (payload.dest == 0x7ffff) {
        cli_out("\tNO FEC found\n\n");
        return CMD_OK;
    }

    cli_out("\tForwarding action received: %x \n\n", payload.dest);

    arad_pp_frwrd_em_dest_to_fec(unit, &payload, &routing_info);
    if (routing_info.frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
        fec_ndx = routing_info.frwrd_decision.dest_id;        
    }else{
        cli_out("\tforwarding action is not FEC \n\n");
        return CMD_OK;
    }

    if ((fec_ndx > last_fec) || (fec_ndx < first_fec)) {

        if (fec_ndx < first_fec) {
            cli_out("\t FEC is ECMP VALUE = %d, run diag pp elb for additional info\n\n", fec_ndx);

        }else{
            cli_out("\tError FEC out of range: %d, range %d - %d\n\n", fec_ndx, first_fec, last_fec);
        }
        return CMD_OK;
    }

    cli_out("\tprinting FEC info: \n\n");

    status = bcm_dpp_am_fec_is_alloced(unit, 0 /*flag*/, 1 /*size*/, fec_ndx);
    if (status == BCM_E_EXISTS) {
    cli_out("FEC    |Destination       |Type      |intf      |accessed|\n");
    cli_out("-------+------------------+----------+----------+--------|\n");
      bcm_l3_egress_t_init(&fec_obj);
      status = _bcm_ppd_frwrd_fec_entry_get(unit, fec_ndx, &fec_obj);
      /* In JERICHO possibly set the CASCADED flag. */
      if (SOC_IS_JERICHO(unit)) {              
          status = bcm_dpp_am_fec_get_usage(unit, fec_ndx, &usage);
          if(status != BCM_E_NONE){
              return CMD_FAIL;
          }
          if (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) {
              fec_obj.flags |= BCM_L3_CASCADED;
          }              
      }
      result = cmd_diag_fec_print(unit, (int)fec_ndx, &fec_obj);
      if(result != CMD_OK){
          return result;
      }   
    }else{
      cli_out("\nfec hit but not allocated \n\n");/*TBD - fec hit but not allocated */
    }

    cli_out("\n\n");
      
    return CMD_OK;
}

cmd_result_t 
    cmd_diag_fec_print_all(int unit){

    int fec_ndx, last_fec, first_fec;
    int status = BCM_E_NONE;
    bcm_l3_egress_t fec_obj;
    _bcm_dpp_am_fec_alloc_usage usage;
    int count = 0;    

    first_fec = SOC_DPP_DEFS_GET(unit, nof_ecmp);
    last_fec = SOC_DPP_DEFS_GET(unit, nof_fecs);

    cli_out("\nFEC table dump: first valid fec = %d last valid fec = %d \n--------------------------------------------------------------\n\n\n", first_fec, last_fec);
               
    cli_out("===================Regular or Primary FECs===============||==================Protection/secondary FECs==============|\n\n");               
    cli_out("FEC    |Destination       |Type      |intf      |accessed||Protec'|Destination       |Type      |intf      |accessed|\n");
    cli_out("-------+------------------+----------+----------+--------++-------+------------------+----------+----------+--------+\n");

    for (fec_ndx = first_fec; fec_ndx < last_fec; fec_ndx++) {        

      status = bcm_dpp_am_fec_is_alloced(unit, 0 /*flags*/, 1 /*size*/, fec_ndx);
      if (status == BCM_E_EXISTS) {          
          count++;
          bcm_l3_egress_t_init(&fec_obj);
          status = _bcm_ppd_frwrd_fec_entry_get(unit, fec_ndx, &fec_obj);
          /* In JERICHO possibly set the CASCADED flag. */
          if (SOC_IS_JERICHO(unit)) {              
              status = bcm_dpp_am_fec_get_usage(unit, fec_ndx, &usage);
              if(status != BCM_E_NONE){
                  cli_out("\n Error status %d FEC %d\n", status, fec_ndx);
                  return CMD_FAIL;
              }
              if (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) {
                  fec_obj.flags |= BCM_L3_CASCADED;
              }
          }

          cmd_diag_fec_print(unit, fec_ndx, &fec_obj);
          if (fec_obj.failover_id != 0) {
              if (fec_obj.failover_if_id != 0) {
                  cli_out("|");
              }else{
                  cli_out("\n");
              }
          }else{
              cli_out("\n");
          }
      }
    }

    cli_out("\n Total FECs found: %d\n-----------------\n",count);
    
    return CMD_OK;
}

cmd_result_t 
    cmd_diag_fec_performance(int unit, args_t* a) {

    int ecmp_count, cleanup, print_dbg;
    parse_table_t    pt;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "ecmp_count", PQ_INT, 0, &ecmp_count, NULL);
    parse_table_add(&pt, "cleaup", PQ_DFL|PQ_INT, 0, &cleanup, NULL);
    parse_table_add(&pt, "print", PQ_DFL|PQ_INT, 0, &print_dbg, NULL);
    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

	if (ecmp_count < 1) {
		cli_out("Error: missing argument ecmp_count\n");
		return CMD_FAIL;
	}

	if (ecmp_count > 480) {
		cli_out("Error: maximum count is 480\n");
		return CMD_FAIL;
	}

    if (fec_performance_main(unit, print_dbg, ecmp_count, cleanup) != BCM_E_NONE)
    {
        return CMD_FAIL;
    }

    return CMD_OK;
}

cmd_result_t 
cmd_diag_occ_mgmt(int unit, args_t* a) {

    if (SOC_PPC_OCC_ALL_PROFILES_print(unit) != SOC_SAND_OK)
    {
           return CMD_FAIL;
    }

    return CMD_OK;
}

cmd_result_t 
cmd_diag_occupation_profile_info_get(int unit, args_t* a) {
    uint32 
        ret;

    ret = SOC_PPC_OCC_ALL_PROFILES_print(unit);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
        return CMD_FAIL; 
    }

    return CMD_OK;
}

cmd_result_t 
cmd_ppd_api_diag_cos_get(int unit, args_t* a) {
    uint32 
        ret;
    int              prm_core = -1;
    parse_table_t    pt;
    int              core, first_core, last_core;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0,  &prm_core, NULL);
    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

	if ((prm_core < -1 ) 
		|| 
	  (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
		cli_out("error ilegal core ID for device\n");
		return CMD_FAIL;
	}

    if (prm_core == -1) {
        first_core = 0;
        last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
    } else {
        first_core = prm_core;
        last_core = prm_core+1;
    }

    for (core=first_core; core < last_core; core++) {

        cli_out("%s\r\n", get_core_str(unit, core));

        ret = arad_pp_diag_cos_info_get_unsafe(unit, core);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
        { 
            return CMD_FAIL; 
        } 
    }
    return CMD_OK;
}

int 
  cmd_ppd_api_diag_db_glem_lkup_info_get(int unit, args_t* a) 
{   
  uint32            rv;   
  int               global_lif = 0;
  int               gport = 0;
  int               prm_core = 0;
  int               i;
  parse_table_t     pt;
  uint8             nof_printed_lifs;
  _bcm_dpp_gport_hw_resources hw_resources;
  soc_ppc_diag_glem_signals_t signals_info;
  soc_ppc_diag_glem_outlif_t *outlif_info;

  /* Get parameters */
  parse_table_init(unit, &pt);
  parse_table_add(&pt, "gport", PQ_DFL|PQ_INT, 0, &gport, NULL);
  parse_table_add(&pt, "gl", PQ_DFL|PQ_INT, 0, &global_lif, NULL);
  parse_table_add(&pt, "core", PQ_DFL|PQ_INT, 0, &prm_core, NULL);

  if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
  }
  parse_arg_eq_done(&pt);

  /* Input validation */
  cli_out("\n\n");
  if (gport != 0 && global_lif != 0) {
      cli_out("Error: Can't set both gl and gport\n");
      return CMD_FAIL;
  }

  if ((prm_core < 0 ) 
      || 
    (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
       cli_out("Error: Illegal core ID for device: %d\n", prm_core);
       return CMD_FAIL;
  }

  sal_memset(&signals_info, 0, sizeof(signals_info));

  /* Call functions */

  if (gport) {
      /* Get the global lif from the gport, and then perform the regular global lif lookup. */
      rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, &hw_resources);
      if (rv == BCM_E_NOT_FOUND || rv == BCM_E_PORT) {
          cli_out("Error: Gport 0x%x doesn't exist.\n", gport);
          return CMD_FAIL;
      } else if (rv) {
          cli_out("Error in sw state access\n");
          return CMD_FAIL;
      }

      /* Make sure we got a legal global outlif. */
      if (hw_resources.global_out_lif == _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
          cli_out("Error: No egress global lif found for gport 0x%x.\n", gport);
          return CMD_FAIL;
      }
      
      /* Global outlif found. */
      global_lif = hw_resources.global_out_lif;
      cli_out("For gport 0x%x:\n", global_lif);
  }

  if (global_lif) {
      outlif_info = &signals_info.outlifs[0];
      /* First, check if the lif is mapped or unmapped*/
      /* Perform a GLEM lookup. */
      rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_glem_access_entry_by_key_get, (unit, global_lif, &outlif_info->local_outlif, &outlif_info->accessed, &outlif_info->found));
      if (rv != BCM_E_NONE) {
          return CMD_FAIL;
      }
      outlif_info->global_outlif = global_lif;
      outlif_info->source = soc_ppc_diag_glem_outlif_source_user;

  } else {
      /* Read signals. */
      rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_diag_glem_signals_get, (unit, prm_core, &signals_info));
      if (rv != BCM_E_NONE) {
          return CMD_FAIL;
      }

  }

  nof_printed_lifs = 0;
  for (i = 0 ; i < 2 ; i++) {
      outlif_info = &signals_info.outlifs[i];
      if (outlif_info->global_outlif) {
          /* Only print No point in displaying empty searches. */
          nof_printed_lifs++;
          
          if (!outlif_info->found) {
              /* 
               *  Even if the GLEM lookup failed, it's possible that the lif is still valid because it's a direct lif.
               *  Check if this is the case, and update accordingly.
               */
              rv = _bcm_dpp_lif_id_is_mapped(unit, outlif_info->global_outlif, &outlif_info->mapped);
              if (rv != BCM_E_NONE) {
                  return CMD_FAIL;
              }

              if (!outlif_info->mapped) {
                  /* Direct lif. */
                  outlif_info->found = TRUE;
                  outlif_info->local_outlif = outlif_info->global_outlif;
                  outlif_info->mapped = FALSE;
              }
          } else {
              /* If lif was found in the GLEM, it means it's a mapped lif. */
              outlif_info->mapped = TRUE;
          }
          soc_ppc_diag_glem_outlif_print(outlif_info);
      }
  }

  if (!nof_printed_lifs) {
      cli_out("No GLEM lookups were performed.\n\n\n");
  }

  return CMD_OK; 
} 


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
int 
  cmd_ppd_api_diag_kbp_info_and_flags(int unit, args_t* a) 
{   
    uint32  default_value       = 0xFFFFFFFF;
    uint32  all_entries         = 0;
    uint32  packet_last         = 0;
    uint32  print_db            = 0;
    int32   print_mode          = -1;
    int32   print_ltr           = -1;
    uint32  print_device        = 0;
    uint32  dump_device         = 0;
    uint8   flp_prog_id         = 0xFF;
    uint8   entry_set_flag      = 0xFF;
    uint8   entry_get_flag;
    int32   iterate_table      = -1;

    int num_of_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    int nof_arg;
    int i;

    parse_table_t pt;

    SOCDNX_INIT_FUNC_DEFS;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "entry_flag"       , PQ_DFL|PQ_INT8, (void*) INT_TO_PTR(default_value), &entry_set_flag, NULL);
    parse_table_add(&pt, "iterator"         , PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &all_entries, NULL);
    parse_table_add(&pt, "table"            , PQ_DFL|PQ_NO_EQ_OPT|PQ_INT, (void*)-1, &iterate_table, NULL);
    parse_table_add(&pt, "last"             , PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &packet_last, NULL);
    parse_table_add(&pt, "print"            , PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &print_db, NULL);
    parse_table_add(&pt, "device"           , PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &print_device, NULL);
    parse_table_add(&pt, "dump"             , PQ_DFL|PQ_NO_EQ_OPT|PQ_BOOL, 0, &dump_device, NULL);
    parse_table_add(&pt, "prgm"             , PQ_DFL|PQ_INT8, (void*) INT_TO_PTR(default_value), &flp_prog_id, NULL);
    parse_table_add(&pt, "mode"             , PQ_DFL|PQ_NO_EQ_OPT|PQ_INT, (void*)-1, &print_mode, NULL);
    parse_table_add(&pt, "ltr"              , PQ_DFL|PQ_NO_EQ_OPT|PQ_INT, (void*)-1, &print_ltr, NULL);

    nof_arg = parse_arg_eq(a, &pt);
    if (0 >= nof_arg) {
        cli_out(
                "\nKBP - flags and information of KBP status and diagnostics"
                "\nUsage: diag pp kbp [option], when option is one of:"
                "\n\t\t[entry_flag=0/1]            - Disable/Enable the dump of each new entry added to KBP"
                "\n\t\t[iterator][table=id]        - Dumps all current KBP entries"
                "\n\t\t                              [table=id] optional : Iterates on specific table."
                "\n\t\t[last][prgm=x]              - Dumps the last packet that was sent to KBP information"
                "\n\t\t                              [prgm=x] optional : The last FLP program. If not assigned, last FLP  program will be read from FLP DB."
                "\n\t\t[print][mode=x][ltr=id]     - Prints out the current configuration of tables and DB."
                "\n\t\t                              [mode=x] optional : Prints different level of information. 0: Basic | 1: Advanced | 2: Debug."
                "\n\t\t                              [ltr=id] optional : Prints information about a single LTR. Can be combined with [mode]."
                "\n\t\t[device]                    - Print out to KBP_device_print.txt the device configuration, using KBP SDK API."
                "\n\t\t[dump]                      - Dump to KBP_device_dump.txt the DB and LTR configurations, keys and entries, using KBP SDK API."
                "\n\n"
                );
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (entry_set_flag != 0xFF) {
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.kbp_diag_flag.entry_flag.set(unit, entry_set_flag ));
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.frwrd_ip.kbp_diag_flag.entry_flag.get(unit, &entry_get_flag));

        if (entry_set_flag != entry_get_flag) {
            cli_out("Error In sw state setting value - diag_kbp_entry_added_flag_set\n");
        }
        if (entry_get_flag == TRUE) {
            cli_out("KBP Entry Added Diagnostic is ON\n");
        }else{
            cli_out("KBP Entry Added Diagnostic is OFF\n");
        }
    }

    if (all_entries == TRUE ) {
        BCMDNX_IF_ERR_EXIT(arad_kbp_print_diag_all_entries(unit, iterate_table));
        }

    if (packet_last == TRUE ) {
        for (i=0;i<num_of_cores;i++) {
            if (flp_prog_id != 0xFF) {
                BCMDNX_IF_ERR_EXIT(arad_kbp_print_diag_last_packet(unit, i, flp_prog_id));
            }else{
                BCMDNX_IF_ERR_EXIT(arad_kbp_print_diag_last_packet(unit, i, -1));
            }
        }
    }

    if (print_db == TRUE ) {
        arad_kbp_sw_config_print(unit, print_mode, print_ltr);
    }

    if (print_device == TRUE ) {
        arad_kbp_device_print(unit,"KBP_device_print.txt");
    }

    if (dump_device == TRUE ) {
        arad_kbp_device_dump(unit,"KBP_device_dump.txt");
    }

    parse_arg_eq_done(&pt);
    return CMD_OK;

exit:
	SOCDNX_FUNC_RETURN;
} 
#endif /* defined(INCLUDE_KBP) */

#ifdef BCM_JERICHO_SUPPORT

cmd_result_t
cmd_diag_kaps_lkup_info_get(int unit, args_t* a) {
    uint32 soc_rc;
    int              prm_core=0;
    parse_table_t    pt;

 
    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if ((prm_core < 0 ) || 
        (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
        cli_out("error ilegal core ID for device\n");
        return CMD_FAIL;
    }

    soc_rc = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_diag_kaps_lkup_info_get, (unit, prm_core)));
    if(SOC_SAND_FAILURE(soc_rc)) {
        return CMD_FAIL;
    }
    return CMD_OK;
}


cmd_result_t
cmd_diag_load_balancing(int unit, args_t* a) {
    uint32 soc_rc, print_clb = FALSE;
    int core_id, i, LB_vec_index, prm_core = -1, ecmp_flag = 0, lag_flag = 0;
    ARAD_PP_LB_PRINT_SIGNALS lb_signals;
    parse_table_t pt;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /* Get parameters */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
    /* These line are to prevent warning massage of the diag */
    parse_table_add(&pt, "ECMP", PQ_DFL|PQ_NO_EQ_OPT, 0, &ecmp_flag, NULL);
    parse_table_add(&pt, "LAG" , PQ_DFL|PQ_NO_EQ_OPT, 0, &lag_flag, NULL);

    /*Search for the LAG/ECMP flags*/
    for (i = 0; i < a->a_argc; i++) {
        if((strcmp(a->a_argv[i],"ECMP") == 0) || (strcmp(a->a_argv[i],"ecmp") == 0)) {
            ecmp_flag = 1;
        }
        if((strcmp(a->a_argv[i],"LAG") == 0) || (strcmp(a->a_argv[i],"lag") == 0)) {
            lag_flag = 1;
        }
    }


    if (0 > parse_arg_eq(a, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    VERIFY_SIGNAL_DB(unit);

    /* Make sure that one of the ECMP/LAG flags are on */
    if ((lag_flag == 0 && ecmp_flag == 0) || (lag_flag == 1 && ecmp_flag == 1)) {
        cli_out("Please add the ECMP flag or the LAG flag (not both).\n");
        return CMD_FAIL;
    }

    if ((prm_core < -1 ) || (prm_core >= (SOC_DPP_DEFS_GET(unit, nof_cores) )))
    {
         cli_out("error illegal core ID for device\n");
         return CMD_FAIL;
    } else if (prm_core != -1) {
        lb_signals.first_core = prm_core;
        lb_signals.last_core  = prm_core;
    } else {
        lb_signals.first_core = 0;
        lb_signals.last_core  = (SOC_DPP_DEFS_GET(unit, nof_cores) -1);
    }

    lb_signals.diag_type = (ecmp_flag == 1) ? ARAD_PP_DIAG_ECMP : ARAD_PP_DIAG_LAG;

    soc_rc = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_get_load_balancing_diagnostic_signal,(unit,&lb_signals)));

    if(SOC_SAND_FAILURE(soc_rc)) {
        return CMD_FAIL;
    }

    PRT_TITLE_SET("Load Balancing general Information");

    PRT_COLUMN_ADD("Field");
    PRT_COLUMN_ADD("Value");


    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("Parser mode");
    PRT_CELL_SET("%s", SOC_DPP_CONFIG(unit)->pp.parser_mode == 0 ? "Basic" : "Advanced");

    PRT_COMMIT;

    for(core_id = lb_signals.first_core; core_id <= lb_signals.last_core ; core_id++)
    {
        PRT_TITLE_SET("Load Balancing %s Information - core: %d", lb_signals.diag_type == ARAD_PP_DIAG_ECMP ? "ECMP" : "LAG" , core_id);

        PRT_COLUMN_ADD("Field");
        PRT_COLUMN_ADD("Value");

        if(lb_signals.diag_type == ARAD_PP_DIAG_ECMP) {

            if (lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_1].ecmp_is_valid) {

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("In port");
                PRT_CELL_SET("%d", lb_signals.core[core_id].in_port);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("Speculative parsing state");
                PRT_CELL_SET("%s",lb_signals.core[core_id].is_specualte == TRUE ? "Yes" : "No");

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("ECMP Group number");
                PRT_CELL_SET("0x%03x", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_1].ecmp_fec);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("ECMP group Start Pointer");
                PRT_CELL_SET("0x%04x", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_1].start_pointer);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("ECMP Group Size");
                PRT_CELL_SET("%d", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_1].group_size);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("ECMP is Protected");
                PRT_CELL_SET("%s",lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_1].is_protected == TRUE ? "Yes" : "No");

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("ECMP is Stateful");
                PRT_CELL_SET("%s",lb_signals.core[core_id].is_stateful == TRUE ? "Yes" : "No");

                if(SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit) || SOC_IS_QAX_B0(unit)) {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("ECMP Is FLB CLB combine");
                    PRT_CELL_SET("%s",lb_signals.is_combine_flb_clb == TRUE ? "Yes" : "No");
                }


                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("FEC (Received)");
                PRT_CELL_SET("0x%04x", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_1].ecmp_fec_pointer);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("FEC Offset");
                PRT_CELL_SET("%d", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_1].offset);


                if (lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].ecmp_is_valid) {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("2nd hierarchy ECMP Group");
                    PRT_CELL_SET("0x%03x", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].ecmp_fec);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("2nd hierarchy Start Pointer");
                    PRT_CELL_SET("0x%04x", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].start_pointer);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("2nd hierarchy LB Key");
                    PRT_CELL_SET("0x%04x", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].ecmp_lb_key);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("2nd hierarchy FEC (Calculated)");
                    PRT_CELL_SET("0x%04x", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].ecmp_fec_pointer);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("2nd hierarchy Group Size");
                    PRT_CELL_SET("%d", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].group_size);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("2nd hierarchy FEC Offset");
                    PRT_CELL_SET("%d", lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].offset);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("2nd hierarchy Protected");
                    PRT_CELL_SET("%s",lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].is_protected == TRUE ? "Yes" : "No");
                }

                if(lb_signals.core[core_id].is_stateful) {

                    /*
                     * Print this polynomial information in here unless the configured LB will display it later.
                     */
                    if(lb_signals.is_combine_flb_clb == FALSE) {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("ECMP polynomial");
                        PRT_CELL_SET("%s", arad_pp_lb_polynomial_names[0xF & lb_signals.core[core_id].configured.ecmp.polynomial]);

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("ECMP barrel shift offset");
                        PRT_CELL_SET("%d", lb_signals.core[core_id].configured.ecmp.key_shift);

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("ECMP seed");
                        PRT_CELL_SET("%d", lb_signals.core[core_id].configured.ecmp.seed);
                    }


                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Flow Label Destination");
                    PRT_CELL_SET("0x%08x%08x%08x", lb_signals.core[core_id].stateful.flow_label_dst[2], lb_signals.core[core_id].stateful.flow_label_dst[1], lb_signals.core[core_id].stateful.flow_label_dst[0]);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Flow Label Destination (last 47)");
                    PRT_CELL_SET("0x%04x%08x",0x7FFF &  lb_signals.core[core_id].stateful.flow_label_dst[1], lb_signals.core[core_id].stateful.flow_label_dst[0]);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Key 0 shift left offset");
                    PRT_CELL_SET("%d", lb_signals.core[core_id].stateful.key_0_shift);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Key 0 input size");
                    PRT_CELL_SET("%d", ARAD_PP_FLEX_POST_HASH_KEYS_INPUT_SIZE - lb_signals.core[core_id].stateful.key_0_shift);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Key 0 input");
                    PRT_CELL_SET("0x%04x%08x",0x7FFF &  lb_signals.core[core_id].stateful.key_0_input[1], lb_signals.core[core_id].stateful.key_0_input[0]);

                    if (lb_signals.core[core_id].hier_sig[ARAD_PP_HIERARCHY_2].ecmp_is_valid) {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("Key 1 shift right offset");
                        PRT_CELL_SET("%d", lb_signals.core[core_id].stateful.key_1_shift);

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("Key 1 input size");
                        PRT_CELL_SET("%d", ARAD_PP_FLEX_POST_HASH_KEYS_INPUT_SIZE - lb_signals.core[core_id].stateful.key_1_shift);

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("Key 1 input");
                        PRT_CELL_SET("0x%04x%08x",0x7FFF &  lb_signals.core[core_id].stateful.key_1_input[1], lb_signals.core[core_id].stateful.key_1_input[0]);

                    }

                }

                if (lb_signals.core[core_id].is_stateful == FALSE || (lb_signals.core[core_id].is_stateful == TRUE && lb_signals.is_combine_flb_clb == TRUE)) {

                    print_clb = TRUE;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("ECMP headers count");
                    PRT_CELL_SET("%d", (lb_signals.core[core_id].configured.ecmp.header_count == 0) ? 1 : 2 );

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("ECMP polynomial");
                    PRT_CELL_SET("%s", arad_pp_lb_polynomial_names[0xF & lb_signals.core[core_id].configured.ecmp.polynomial]);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("ECMP barrel shift offset");
                    PRT_CELL_SET("%d", lb_signals.core[core_id].configured.ecmp.key_shift);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("ECMP seed");
                    PRT_CELL_SET("%d", lb_signals.core[core_id].configured.ecmp.seed);

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("ECMP is port id used");
                    PRT_CELL_SET("%s",lb_signals.core[core_id].configured.ecmp.use_port_id == TRUE ? "Yes" : "No");
                    if (SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit)) {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("ECMP 2nd hierarchy polynomial");
                        PRT_CELL_SET("%s", arad_pp_lb_polynomial_names[ 0xF & lb_signals.core[core_id].configured.ecmp_2nd.polynomial]);

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("ECMP 2nd hierarchy barrel shift offset");
                        PRT_CELL_SET("%d", lb_signals.core[core_id].configured.ecmp_2nd.key_shift);
                    }
                }
            } else {
                cli_out("No ECMP information was found for core %d\n",core_id);
            }

        } else { /* ARAD_PP_DIAG_LAG */

            if(lb_signals.core[core_id].lag_valid == TRUE) {
                print_clb = TRUE;

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("In port");
                PRT_CELL_SET("%d", lb_signals.core[core_id].in_port);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("Speculative parsing state");
                PRT_CELL_SET("%s",lb_signals.core[core_id].is_specualte == TRUE ? "Yes" : "No");

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("LAG headers count");
                PRT_CELL_SET("%d",lb_signals.core[core_id].configured.lag.header_count);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("LAG polynomial");
                PRT_CELL_SET("%s", arad_pp_lb_polynomial_names[0xF & lb_signals.core[core_id].configured.lag.polynomial]);


                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("LAG barrel shift offset");
                PRT_CELL_SET("%d", lb_signals.core[core_id].configured.lag.key_shift);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("LAG seed");
                PRT_CELL_SET("%d", lb_signals.core[core_id].configured.lag.seed);

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("LAG is port id used");
                PRT_CELL_SET("%s",lb_signals.core[core_id].configured.lag.use_port_id == TRUE ? "Yes" : "No");

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("LAG starting header");
                PRT_CELL_SET("%s",lb_signals.core[core_id].configured.lag.starting_header == 1 ? "Forwarding Header" : "Forwarding Header -1");
            } else {
                cli_out("No LAG information was found for core %d\n",core_id);
            }

        }


        if (print_clb == TRUE) {

            if (lb_signals.core[core_id].configured.pfc_stage1 == lb_signals.core[core_id].configured.pfc_stage2) {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("Packet Format Code");
                PRT_CELL_SET("(%d)%s", lb_signals.core[core_id].configured.pfc_stage1, dpp_parser_pfc_string_by_hw(unit, lb_signals.core[core_id].configured.pfc_stage1));
            } else {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("Packet Format Code 1st stage parsing");
                PRT_CELL_SET("(%d)%s", lb_signals.core[core_id].configured.pfc_stage1, dpp_parser_pfc_string_by_hw(unit, lb_signals.core[core_id].configured.pfc_stage1));

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("Packet Format Code 2nd stage parsing");
                PRT_CELL_SET("(%d)%s", lb_signals.core[core_id].configured.pfc_stage2, dpp_parser_pfc_string_by_hw(unit, lb_signals.core[core_id].configured.pfc_stage2));
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("Parser Leaf Context");
            PRT_CELL_SET("(%d)%s", lb_signals.core[core_id].configured.plc, dpp_parser_plc_string_by_hw(unit, lb_signals.core[core_id].configured.pfc_stage1, lb_signals.core[core_id].configured.plc));

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("Forward index");
            PRT_CELL_SET("%d", lb_signals.core[core_id].configured.fwrd_index);

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("PLC Profile (selected bit)");
            PRT_CELL_SET("0x%04x (%d)", lb_signals.core[core_id].configured.plc_profile,lb_signals.core[core_id].configured.plc_bit);

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("Port Profile");
            PRT_CELL_SET("%d", lb_signals.core[core_id].configured.port_profile);

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("PFC profile entry index");
            PRT_CELL_SET("%d", lb_signals.core[core_id].configured.pfc_profile_index);

            for (LB_vec_index = 0;LB_vec_index < ARAD_PP_LB_VECTOR_NOF; LB_vec_index++) {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("LB vector index %d",LB_vec_index + 1);
                PRT_CELL_SET("%s", arad_pp_lb_vector_names[0x1F & lb_signals.core[core_id].configured.LB_vectors[LB_vec_index]] );
            }


            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("BOS search");
            PRT_CELL_SET("%s",lb_signals.core[core_id].configured.bos_search == TRUE ? "On" : "Off");

            if (lb_signals.core[core_id].configured.bos_search == TRUE) {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("BOS included");
                PRT_CELL_SET("%s",lb_signals.core[core_id].configured.bos_include == TRUE ? "Include" : "Skipped");

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("ELI search");
                PRT_CELL_SET("%s",lb_signals.core[core_id].configured.eli_search == TRUE ? "On" : "Off");
            }
        }
        /*The stuck shouldn't overflow in any case*/
        /* coverity[stack_use_overflow : FALSE] */
        PRT_COMMIT;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#endif /* BCM_JERICHO_SUPPORT */

cmd_result_t
cmd_diag_print_ftmh_header(uint32 unit, uint32 *ftmh_header_old, uint32 *ftmh_header_new) {
    uint32 val_old, val_new, offset = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FTMH Header Information");

    PRT_COLUMN_ADD("Attribute");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("Before-Value");
    PRT_COLUMN_ADD("After-Value");

    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "FTMH Base", 72);

    offset = 0;
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   Exclude_Src", 1);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   ECN_Enable", 1);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   CNI", 1);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   DSP_Extension_Valid", 1);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   MC_ID_or_Out_LIF", 19);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   TM Action_is_MC", 1);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   Out_Mirror_Disable", 1);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   PPH_Type", 2);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   TM Action_Type", 2);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   PP", 2);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   PP_DSP", 8);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   Src_System_Port_Aggr", 16);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   TC", 3);
    DIAG_FTMH_CONTEXT_SHOW(ftmh_header_old, ftmh_header_new, "   Packet_Size", 14);

    /*The stuck shouldn't overflow in any case*/
    /* coverity[stack_use_overflow : FALSE] */
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

cmd_result_t
cmd_diag_ftmh_header_change_show(int unit, args_t* a){
    uint32 rv;
    soc_reg_above_64_val_t reg_val, packet_cotext;
    uint32 ftmh_header_old[3] = {0}, ftmh_header_new[3] = {0};

    if (!SOC_IS_QAX(unit)) {
        cli_out("This device does not support this command.");
        return CMD_FAIL;
    }

    rv = READ_ITE_LAST_PKT_HDRr(unit, reg_val);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) {
        cli_out("Read the register ITE_LAST_PKT_HDR error: %d", rv);
        return CMD_FAIL;
    }

    soc_reg_above_64_field_get(unit, ITE_LAST_PKT_HDRr, reg_val, LAST_PKT_HEADERf, packet_cotext);
    SHR_BITCOPY_RANGE(ftmh_header_new, 0, packet_cotext, 184, 72);

    rv = READ_ITE_LAST_RECEIVED_HEADER_0r(unit, packet_cotext);
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) {
        cli_out("Read the register ITE_LAST_RECEIVED_HEADER_0 error: %d", rv);
        return CMD_FAIL;
    }

    SHR_BITCOPY_RANGE(ftmh_header_old, 0, packet_cotext, 440, 72);

    rv = cmd_diag_print_ftmh_header(unit, ftmh_header_old, ftmh_header_new);
    if (rv != CMD_OK) {
        return rv;
    }

    return CMD_OK;
}

/* Checks whether current packet is OLP */
/* If so, prints warning */
cmd_result_t
print_olp_warning(int unit, args_t* a){

    uint32                                  ret, soc_sand_dev_id;
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO       prm_rcvd_pkt_info;
    bcm_gport_t                             olp[BCM_PIPES_MAX];
    int                                     nof_olp_ports, rv, i, prm_core;
    int                                     skip_print, skip_validation;
    parse_table_t                           pt;
    SOC_PPC_DIAG_RESULT                     ret_val;
    int                                     core, first_core, last_core;
    char*                                   prm_file = NULL;


    SOCDNX_INIT_FUNC_DEFS;

    skip_print = 0;
    skip_validation = 0;
    rv = BCM_E_NONE;
    ret = 0;
    nof_olp_ports = 0;
    prm_core = -1;
    soc_sand_dev_id = (unit);
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(&prm_rcvd_pkt_info);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "core", PQ_DFL|PQ_IP, 0, &prm_core, NULL);
    parse_table_add(&pt, "file", PQ_DFL|PQ_STRING, 0, &prm_file, NULL);

    /* Get relevant core for diagnostics */
    if (0 > parse_arg_eq_keep_index(a, &pt)) {
        skip_validation = 1;
        skip_print = 1;
    }

    if (prm_file != NULL) {
        skip_validation = 1;
        skip_print = 1;
    }

    /* Get relevant core for inspection */
    if (skip_validation == 0) {
        if ((prm_core < -1 ) 
            || 
          (prm_core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
            skip_validation = 1;
            skip_print = 1;
        }
    }
    if (prm_core == -1) {
        first_core = 0;
        last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
    } else {
        first_core = prm_core;
        last_core = prm_core+1;
    }

    for (core=first_core; core < last_core; core++) {

    /* Verify that a packet has already been intercepted */
    /* Otherwise, skip OLP Warning print */
        if (skip_validation == 0) {    
            rv = arad_pp_diag_is_valid(unit, core, SOC_PPC_DIAG_RECEIVED_PACKET_INFO_GET, &ret);
            BCMDNX_IF_ERR_EXIT(rv);

            if (ret == 0) {
                skip_print = 1;
            }
        }
 
        /* In case packet has already been intercepted */
        if (skip_print == 0) {
            /* Get packet info */
            ret = soc_ppd_diag_received_packet_info_get(
                  soc_sand_dev_id,
                  core,
                  &prm_rcvd_pkt_info,
                  &ret_val
                );

            rv = soc_sand_get_error_code_from_error_word(ret);
            BCMDNX_IF_ERR_EXIT(rv); 

            if (ret_val != SOC_PPC_DIAG_OK) 
            { 
                LOG_CLI((BSL_META_U(unit, "%s Packet not found\n\r"),get_core_str(unit, core)));
                continue; 
            } 


            /* Get OLP ports array */
            rv = bcm_port_internal_get(unit,BCM_PORT_INTERNAL_OLP,BCM_PIPES_MAX,olp,&nof_olp_ports);
            if (rv != BCM_E_NONE) {
                cli_out("Error in print_olp_warning \n");
            }

            BCMDNX_IF_ERR_EXIT(rv);
            
            /* In case OLP Ports were defined, check if packet originates from OLP */
            if (nof_olp_ports > 0) {
                for (i=0 ; i < nof_olp_ports ; i++) {
                    if (prm_rcvd_pkt_info.in_pp_port == BCM_GPORT_LOCAL_GET(olp[i])) {
                        cli_out("\n************************************************ \n");
                        cli_out("Warning: %s Packet diagnostics refers to OLP packet \n", get_core_str(unit, core));
                        cli_out("************************************************ \n\n");
                    }
                }    
            }
        }
    }

exit:
    parse_arg_eq_done(&pt);
    SOCDNX_FUNC_RETURN;
}

cmd_result_t
cmd_dpp_diag_pp(int unit, args_t* a)
{
    int i;
    char      *function;
    cmd_result_t rv;

    function = ARG_GET(a);
    if (! function ) {
        return CMD_USAGE;
    } 

    if (!SOC_IS_DPP(unit)) {
        cli_out("Unit:%d is not a dpp device\n", unit);
        return CMD_NOTIMPL;
    }

    rv = print_olp_warning(unit, a);
    if (rv != CMD_OK) {
        cli_out("Error in print_olp_warning, could not determine whether packet is OLP \n");
    }

    for (i=0; i<sizeof(diag_pp_command_def)/sizeof(DIAG_PP_COMMAND_DEF); i++ ) {
        if (DIAG_FUNC_STR_MATCH(function, diag_pp_command_def[i].short_name, diag_pp_command_def[i].long_name)) {
            return (diag_pp_command_def[i].cmd_diag_pp_function(unit,a));
        }
    } 

    return CMD_USAGE;
}

void
print_pp_usage(int unit)
{
    char cmd_dpp_diag_pp_usage_1[] =
    "Usage (DIAG pp):"
    "\n\tDIAGnotsics pp commands\n\t"
    "Usages:\n\t"
    "DIAG pp [OPTION] <parameters> ..."
    "OPTION can be:"
    "\nFrwrd_Decision_Trace - returns the forwarding decision (destination + trap) for each phase in processing port,LIF, forwarding lookup, FEC resultion, TRAP"
    "\n\t\tcore - Core ID (0/1)"
    "\nReceived_Packet_Info - dump first 128 bytes from last received packet and return its association port information"
    "\n\t\tcore - Core ID (0/1)."
    "\nParsing_Info - returns packet format e.g. ipv4oEth"
    "\n\t\tcore - Core ID (0/1)"
    "\nDB_LIF_lkup_info - return the lookup keys and result from SEM DB"
    "\n\t\tbank_id - Bank number. For SEM: 0 (SEM-A), 1 (SEM-B), 2 (TCAM)."
    "\n\t\tlkup_num - Lookup number, some DB has more than one lookup"
    "\n\t\tcore - Core ID (0/1)"
    "\nTERMination_Info - retuns terminated headers and the forwarding header types" 
    "\n\t\tcore - Core ID (0/1)"
    "\nTRAPS_Info - returns wether packet was trapped, and trap-code" 
    "\n\t\tcore - Core ID (0/1)"
    "\nTrapped_Packet_Info - parse packet recived by CPU return packet size, start of network header and other proccessing attributes"  
    "\n\t\tmax_size - Maximum number of bytes to read"
    "\nENCAP_info - returns the encapsulation info in egress DB"
    "\n\t\tcore - Core ID (0/1)"
    "\nFrwrd_Lkup_Info - returns the forwarding enginetype  (IP routing, bridging, TRILL,..)"       
    "\n\t\tcore - Core ID (0/1)"
    "\nFrwrd_LPM_lkup - diag query into LPM DB. Given VRF,DIP return FECs"
    "\n\t\tip_address - Ipv4 address"
    "\n\t\tprefix_len - Number of bits to consider in the IP address starting from the msb. Range: 0-32."
    "\n\t\t\tExample for key ip_address 192.168.1.0 and prefix_len 24 would match any IP Address of the form 192.168.1.x"
    "\n\t\tvrf - VRF"
    "\n\t\tcore - Core ID (0/1)"
    "\nIng_Vlan_Edit_info - return ingress/egress vlan editing information"
    "\n\t\tcore - Core ID (0/1)"
    "\nLeaRNing_info - returns learning info of last packet"
    "\nMODE_info_Get - Gets the configuration set by the mode_info_set API."
    "\nMODE_info_Set - when setting mode to raw: rest of diag CLIs will return information (keys and payload) in hex without parsing"
    "\n\t\tflavor - Flavor indicates/affects the way the diagnostic APIs work:"
    "\n\t\t\tRAW to return a raw bit-stream i.e. without parsing into structure"
    "\n\t\t\tNONE for no flavor"
    "\nPKT_associated_TM_info - returns  the resolved destination, meter-id, and other info to be processed/used by TM blocks"
    "\n\t\tcore - Core ID (0/1)"
	"\nMeTeRing- returns the current bucket status for a given meter"
	"\n\t\tid - Meter ID"
    "\n\t\tcore - Meter core id (0/1), optional"
	"\n\t\tgroup - Meter group id (0/1), optional"
	"\nEth_PoLiCeR - returns the port-policers mapping with the relevant policers status. show aggregate policer ID if port was set to."
	"\n\t\tport - Port"
	"\nAGgregateEth_PoLiCeR - return the status of the aggregate group of policers, recognized by policer_id"
	"\n\t\tpolicer_id - Policer ID"
    "\nPKT_TRace_Clear - clear sticky status in HW"
    "\n\t\tcore - Core ID (0/1)"
    "\nDB_LEM_lkup_info - return the lookup keys and result from LEM DB. Used for forwarding lookup. E.g. MACT, ILM, host lookup"
    "\n\t\tlkup_num - Lookup number, some DB has more than one lookup";
    char cmd_dpp_diag_pp_usage_2[] =
    "\nEgress_DROP - display filters at egress"
    "\nLIF_show - display bank ID and offset inside bank"
    "\n\ttype - lif type:in/out"
    "\n\tid   - lif id (interpreted as local id by default or as global when gl=1"
    "\n\tgl   - set to 1 in order to have lif id interpreted as global one"
	"\nTPID_Globals_show - display global TPIDs"
    "\nTPID_PRofiles_show - display TPIDs profiles" 
    "\nTPID_POrt_profile_show port - display profile associated with port"
    "\n\tport - port number, optional"
    "\n\tcore - core id (0/1), optional"
    "\nKBP - flags and information of KBP status and diagnostics"
    "\nUsage: diag pp kbp [option], when option is one of:"
    "\n\t\t[entry_flag=0/1]         - Disable/Enable the dump of each new entry added to KBP"
    "\n\t\t[print_entries]          - Dumps all KBP current entries in all tables"
    "\n\t\t[last_packet][prgm=x]    - Dumps the last packet that was sent to KBP information"
    "\n\t\t                           [prgm=x] optional : The last FLP program. If not assign, last FLP  program will be read from FLP DB."
    "\nOUTLIF_Info - Prints the content of the EEDB - all OUTLIFs are displayed"
    "\nOUTLIF_Info_VSI- prints the VSI entries in particular, parsed in a chart"
    "\nOUTLIF_Info_LL- prints the Link Layer entries in particular, parsed in a chart"
    "\nVTT_Dump [last=0]  - return the valid VTT programs"
    "\nVTT_Dump [last=1]  - return last invoked VTT program"
    "\nFLP_Dump [last=0]  - return the valid FLP programs"
    "\nFLP_Dump [last=1]  - return last invoked FLP program"
    "\nEgr_Vlan_Edit_info - return egress vlan editing information"
    "\nGPort              - return information for the gport"
    "\n\tid     - gport id"
    "\n\tRES    - Display resource"
    "\n\tFW_DB  - Display forward database info"
    "\n\tLIF_DB - Display lif database info"
    "\nCross-Connect-traverse - return the cross connection traverse information"
    "\nIPv4_MC            - displays mc routing table"
    "\nVSI_decision       - VSI information"
    "\nIf_PROFILE_Status  - Interface profile allocation"
    "\nFTMH_Header_Change_Show      - display FTMH header before and after ITPP"
    "\nRIF_profile_info   - return information about last packet profile"
    "\nOCCupation_mgmt_show - display occupation maps status"
    "\nClass_Of_Service   - display cos mapping"
    "\nDB_GLEM_lkup_info  - returns the last lookup key(s) and result(s) in the Global Lif Exact Match"
    "\n\t\tgport=[value]  -    perform a GLEM lookup for gport [value]"
    "\n\t\tgl=[value]     -    perform a GLEM lookup for [value]"
    "\n\t\tcore           -    core id (optional, default 0)"
#ifdef BCM_JERICHO_SUPPORT
    "\nKaps_LKuP_info_get - return the lookup keys and results from kaps"
	"\nFEC_PReFormance        - runs FEC creation performance tests"
    "\n\t\tecmp_count=[value]  - number of ECMP groups to create"
    "\n\t\tprint=[value]       - print debugging messages"
    "\nload_balancing - Displays the ECMP and LAG load balancing information"
#endif /* BCM_JERICHO_SUPPORT */
    "\ndiag pp sig        - presents list of signals according to filter and show options"
    "\n\t\t[name=Signal] [from=Stage] [to=Stage] [stage=Stage] [order=little/big] [core=0/1]"
    "\n\t\t[show=all/detail/full/exact/stage/struct/param/internal/noexpand/nocond/double]"
    "\n";

    cli_out(cmd_dpp_diag_pp_usage_1);
    cli_out(cmd_dpp_diag_pp_usage_2);
}

