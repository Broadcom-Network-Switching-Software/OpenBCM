/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * General include file for reference design.
 */
/* { */
  #include <appl/diag/dpp/ref_sys.h>

  #if !(defined(LINUX) || defined(UNIX))
  /* { */
    #include <bcm_app/dpp/../H/drv/mem/eeprom.h>
  /* } */
  #endif

/* } */


#include <appl/diag/dpp/utils_generic_test.h>

#include <soc/dpp/SAND/Management/sand_chip_consts.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#if LINK_PSS_LIBRARIES
  #include <appl/dpp/UserInterface/ui_rom_defi_fap10m.h>
  #include <soc/dpp/SOC_SAND_FAP10M/fap10m_chip_defines.h>
#endif

#ifdef __DUNE_HRP__
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_dhrp.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_dhrp_api.h>
/* } */
#elif defined(__DUNE_SSF__)
/* { */
/* } */
#else
/* { */
  #include <CSystem/csystem_consts.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_fe200.h>
  #include <FMC/fmc_common_agent.h>
  #include <appl/diag/dpp/dune_rpc.h>
  #include <appl/diag/dpp/dune_rpc_auto_network_learning.h>

  #include <appl/diag/dpp/utils_host_board.h>
/* } */
#endif

/* { */
  #include <appl/dpp/UserInterface/ui_cli_files.h>
/* } */

#include <Bsp_drv/dffs_descriptor.h>
#include <appl/dpp/UserInterface/ui_rom_defi_diag.h>

#if LINK_TIMNA_LIBRARIES
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_access_timna.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_timna_api.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_sweept20_api.h>
/* } */
#endif
#if LINK_FAP21V_LIBRARIES
/* { */
#include <appl/dpp/UserInterface/ui_rom_defi_fap21v_api.h>
#include <appl/dpp/UserInterface/ui_rom_defi_fap21v_gfa.h>
#include <appl/dpp/UserInterface/ui_rom_defi_fap21v_app.h>
/* } */
#endif
#if LINK_PETRA_LIBRARIES
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_petra_api.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_ptg.h>
/* } */
#if PETRA_PP
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_petra_pp_api.h>
  #ifndef __DUNE_SSF__
    #include <sweep/Petra/PP/swp_p_pp_system_conf.h>
    #include <appl/dpp/UserInterface/ui_rom_defi_swp_p_pp_api.h>
    #include <appl/dpp/UserInterface/ui_rom_defi_gsa_api.h>
  #endif
/* } */
#endif
#endif
#if LINK_FE600_LIBRARIES
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_fe600_api.h>
/* } */
#endif

#if LINK_T20E_LIBRARIES
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_t20e_acc.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_ppd_api.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_ppa_api.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_oam_acc.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_oam_api.h>
/* } */
#endif

#if LINK_PPD_LIBRARIES
/* { */
#include <appl/dpp/UserInterface/ui_rom_defi_ppd_api.h>
#include <appl/dpp/UserInterface/ui_rom_defi_ppa_api.h>
/* } */
#endif

#if LINK_TMD_LIBRARIES
  /* { */
    #include <appl/dpp/UserInterface/ui_rom_defi_tmd_api.h>
  /* } */
#endif  

/* { */
  #define UI_DEMY_VAL 0
  #define DUNE_MASK                         0x0FFF
/* } */

#define INIT


/*
 * Arrays and variables related to DUNE chip set.
 * {
 */
/********************************************************
*NAME
*  Fe_rmac_crc_d_field_vals
*TYPE: BUFFER
*DATE: 12/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'RMAC_CRC_D' WITHIN THE FE CHIP (MAC ENABLERS).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_rmac_crc_d_field_vals[]
#ifdef INIT
   =
{
  {0,"NO delete on CRC err"},
  {1,"Delete on CRC err"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_tx_cdc_field_vals
*TYPE: BUFFER
*DATE: 12/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'TX_CDC' WITHIN THE FE CHIP (MAC ENABLERS).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_tx_cdc_field_vals[]
#ifdef INIT
   =
{
  {0,"Count data and ctrl cells"},
  {1,"Count ctrl cells only"},
  {2,"Count data cells only"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_pd_tbg_field_vals
*TYPE: BUFFER
*DATE: 14/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'PD_TBG' WITHIN THE FE CHIP (SERDES ENABLERS).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_pd_tbg_field_vals[]
#ifdef INIT
   =
{
  {0,"DO NOT power down TBG"},
  {1,"power down TBG"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_pd_fct_field_vals
*TYPE: BUFFER
*DATE: 14/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'PD_FCT' WITHIN THE FE CHIP (SERDES ENABLERS).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_pd_fct_field_vals[]
#ifdef INIT
   =
{
  {0,"DO NOT power down RX/TX blocks"},
  {1,"power down RX/TX blocks"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_vcms_field_vals
*TYPE: BUFFER
*DATE: 14/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'VCMS' WITHIN THE FE CHIP (SERDES ENABLERS).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_vcms_field_vals[]
#ifdef INIT
   =
{
  {0,"1.7 Volts"},
  {1,"1.6 Volts"},
  {2,"1.5 Volts"},
  {3,"1.4 Volts"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_rx_cdc_field_vals
*TYPE: BUFFER
*DATE: 12/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'RX_CDC' WITHIN THE FE CHIP (MAC ENABLERS).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_rx_cdc_field_vals[]
#ifdef INIT
   =
{
  {0,"Count data and ctrl cells"},
  {1,"Count ctrl cells only"},
  {2,"Count data cells only"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_prbs_1_geno_field_vals
*TYPE: BUFFER
*DATE: 13/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'PRBS_1_GENO' WITHIN THE FE CHIP (PRBS ENABLERS).
*  (ALSO VALID FOR PRBS_2_GENO,PRBS_3_GENO,PRBS_4_GENO)
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_prbs_1_geno_field_vals[]
#ifdef INIT
   =
{
  {0,"NO PRBS generation"},
  {1,"Generate PRBS"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_prbs_1_cko_field_vals
*TYPE: BUFFER
*DATE: 13/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'PRBS_1_CKO' WITHIN THE FE CHIP (PRBS ENABLERS).
*  (ALSO VALID FOR PRBS_2_CKO,PRBS_3_CKO,PRBS_4_CKO)
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_prbs_1_cko_field_vals[]
#ifdef INIT
   =
{
  {0,"NO PRBS checking"},
  {1,"Activate PRBS checker"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_rm_did_or_field_vals
*TYPE: BUFFER
*DATE: 12/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'RM_DID_OR' WITHIN THE FE CHIP (REACHABILITY MESSAGE
*  FAULT INDICATION).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_rm_did_or_field_vals[]
#ifdef INIT
   =
{
  {0,"NO out-of-range indication"},
  {1,"Positive out-of-range indication"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_fe_23_dup_id_field_vals
*TYPE: BUFFER
*DATE: 12/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'FE_23_DUP_ID' WITHIN THE FE CHIP (REACHABILITY MESSAGE
*  FAULT INDICATION).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_fe_23_dup_id_field_vals[]
#ifdef INIT
   =
{
  {0,"NO duplicate indication"},
  {1,"Positive duplicate indication"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_ctr_cell_cnt_o_1_field_vals
*TYPE: BUFFER
*DATE: 05/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'CTR_CELL_CNT_O_1' WITHIN THE FE CHIP (CONTROL CELLS
*  COUNTER 1).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_ctr_cell_cnt_o_1_field_vals[]
#ifdef INIT
   =
{
  {0,"NO counter overflow"},
  {1,"Counter overflow"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_ctr_cell_cnt_o_2_field_vals
*TYPE: BUFFER
*DATE: 05/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'CTR_CELL_CNT_O_2' WITHIN THE FE CHIP (CONTROL CELLS
*  COUNTER 2).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_ctr_cell_cnt_o_2_field_vals[]
#ifdef INIT
   =
{
  {0,"NO counter overflow"},
  {1,"Counter overflow"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_peco_2_field_vals
*TYPE: BUFFER
*DATE: 05/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'PECO_2' WITHIN THE FE CHIP (PACKET DATA CELLS
*  COUNTER 2).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_peco_2_field_vals[]
#ifdef INIT
   =
{
  {0,"NO counter overflow"},
  {1,"Counter overflow"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_peco_1_field_vals
*TYPE: BUFFER
*DATE: 05/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'PECO_1' WITHIN THE FE CHIP (PACKET DATA CELLS
*  COUNTER 1).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_peco_1_field_vals[]
#ifdef INIT
   =
{
  {0,"NO counter overflow"},
  {1,"Counter overflow"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_csrfo_2_field_vals
*TYPE: BUFFER
*DATE: 25/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'CSRFO_2' WITHIN THE FE CHIP (SR DATA CELLS FIFO
*  INDICATIONS 2).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_csrfo_2_field_vals[]
#ifdef INIT
   =
{
  {0,"No fifo overflow"},
  {1,"Fifo overflow"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_srfne_2_field_vals
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'SRFNE_2' WITHIN THE FE CHIP (SR DATA CELLS FIFO
*  INDICATIONS 2).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_srfne_2_field_vals[]
#ifdef INIT
   =
{
  {0,"Fifo empty"},
  {1,"Fifo not empty"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_csrfo_1_field_vals
*TYPE: BUFFER
*DATE: 25/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'CSRFO_1' WITHIN THE FE CHIP (SR DATA CELLS FIFO
*  INDICATIONS 1).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_csrfo_1_field_vals[]
#ifdef INIT
   =
{
  {0,"No fifo overflow"},
  {1,"Fifo overflow"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_srfne_1_field_vals
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'SRFNE_1' WITHIN THE FE CHIP (SR DATA CELLS FIFO
*  INDICATIONS 1).
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_srfne_1_field_vals[]
#ifdef INIT
   =
{
  {0,"Fifo empty"},
  {1,"Fifo not empty"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_planes_field_vals
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'FE_PLANES_FIELD' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_planes_field_vals[]
#ifdef INIT
   =
{
  {0,"One plane"},
  {1,"Two planes"},
  {2,"Four plane"},
  {3,"Eight planes"},
  {4,"Sixteen plane"},
  {5,"32 planes"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_repeater_field_vals
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'FE_REPEATER_FIELD' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_repeater_field_vals[]
#ifdef INIT
   =
{
  {0,"Act as FE"},
  {1,"Act as repeater"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_multistage_field_vals
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'FE_MULTISTAGE_FIELD' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_multistage_field_vals[]
#ifdef INIT
   =
{
  {0,"Act as single stage"},
  {1,"Act as multi stage"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_mode_fe2_field_vals
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'FE_MODE_FE2_FIELD' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_mode_fe2_field_vals[]
#ifdef INIT
   =
{
  {0,"Mode FE13"},
  {1,"Mode FE2"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_src_d_lvl_n_1_vals
*TYPE: BUFFER
*DATE: 09/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'SRC_D_LVL_N_1_FIELD' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_src_d_lvl_n_1_vals[]
#ifdef INIT
   =
{
  {0,"FOP"},
  {1,"FOP"},
  {2,"FE1 or FE3"},
  {3,"FE2"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_src_d_lvl_n_2_vals
*TYPE: BUFFER
*DATE: 09/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'SRC_D_LVL_N_2_FIELD' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_src_d_lvl_n_2_vals[]
#ifdef INIT
   =
{
  {0,"FOP"},
  {1,"FOP"},
  {2,"FE1 or FE3"},
  {3,"FE2"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_src_lvl_vals
*TYPE: BUFFER
*DATE: 16/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'LEVEL' (DEVICE TYPE) WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_src_lvl_vals[]
#ifdef INIT
   =
{
  {0,"FAP"},
  {1,"FAP"},
  {2,"FE3"},
  {3,"FE2"},
  {4,"FAP"},
  {5,"FAP"},
  {6,"FE1"},
  {7,"FA2"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_module_vals
*TYPE: BUFFER
*DATE: 19/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'MODULE' IN THE 'SOC_SAND_INDIRECT ACCESS' CONTEXT WITHIN
*  THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_module_vals[]
#ifdef INIT
   =
{
  {0,"DCHa"},
  {1,"DCHb"},
  {2,"DCLa"},
  {3,"DCLb"},
  {4,"CCSa"},
  {5,"CCSb"},
  {6,"RTP"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_rd_not_wr_vals
*TYPE: BUFFER
*DATE: 19/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'READ-NOT-WRITE' IN THE 'SOC_SAND_INDIRECT ACCESS' CONTEXT WITHIN
*  THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_rd_not_wr_vals[]
#ifdef INIT
   =
{
  {0,"Write operation"},
  {1,"Read operation"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;
/********************************************************
*NAME
*  Fe_ind_trigger_vals
*TYPE: BUFFER
*DATE: 19/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING VALUE OF
*  'SOC_SAND_INDIRECT ACCESS TRIGGER' IN THE 'SOC_SAND_INDIRECT ACCESS'
*  CONTEXT WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'VAL_TO_TEXT':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  VAL_TO_TEXT
    Fe_ind_trigger_vals[]
#ifdef INIT
   =
{
  {0,"Indirect access available"},
  {1,"Indirect access busy"},
  /*
   * End of array indicator. Do not remove.
   */
  {0,""}
}
#endif
;

#if !(defined(LINUX) || defined(UNIX))
/* { */

/********************************************************
*NAME
*  Fe_connect_map_group_str
*TYPE: BUFFER
*DATE: 07/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_CONNECT_MAP_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_connect_map_group_str[]
#ifdef INIT
   =
{
  {
    32,FE_CONNECTIVITY_1_REG,
    OFFSETOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.connectivity_reg_1[0])

  },
  {
    32,FE_CONNECTIVITY_2_REG,
    OFFSETOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.connectivity_reg_2[0])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_mac_rx_cntrs_group_str
*TYPE: BUFFER
*DATE: 13/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MAC_RX_CNTRS_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_mac_rx_cntrs_group_str[]
#ifdef INIT
   =
{
  {
    16,FE_MAC_RX_CNTRS_1_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_1[0])
  },
  {
    16,FE_MAC_RX_CNTRS_2_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_2[0])
  },
  {
    16,FE_MAC_RX_CNTRS_3_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_3[0])

  },
  {
    16,FE_MAC_RX_CNTRS_4_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_4[0])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_mac_tx_cntrs_group_str
*TYPE: BUFFER
*DATE: 13/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MAC_TX_CNTRS_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_mac_tx_cntrs_group_str[]
#ifdef INIT
   =
{
  {
    16,FE_MAC_TX_CNTRS_1_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_tx_cntrs_1[0])
  },
  {
    16,FE_MAC_TX_CNTRS_2_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_tx_cntrs_2[0])
  },
  {
    16,FE_MAC_TX_CNTRS_3_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_tx_cntrs_3[0])

  },
  {
    16,FE_MAC_TX_CNTRS_4_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_tx_cntrs_4[0])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_tx_sr_data_cell_group_str
*TYPE: BUFFER
*DATE: 16/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_TX_SR_DATA_CELL_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_tx_sr_data_cell_group_str[]
#ifdef INIT
   =
{
  {
    1,FE_LAST_TX_REG,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.tx_sr_data_cell.last_tx)
  },
  {
    7,FE_BODY_TX_REG,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.tx_sr_data_cell.body_tx)
  },
  {
    1,FE_SWITCHES_TX_REG,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.tx_sr_data_cell.switches_tx)

  },
  {
    1,FE_PATHS_TX_REG,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.tx_sr_data_cell.paths_tx)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_rx_sr_data_cell_group_str
*TYPE: BUFFER
*DATE: 19/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_RX_SR_DATA_CELL_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_rx_sr_data_cell_group_str[]
#ifdef INIT
   =
{
  {
    1,FE_LAST_RX_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.rx_sr_data_cell.last_rx)
  },
  {
    7,FE_BODY_RX_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.rx_sr_data_cell.body_rx)
  },
  {
    1,FE_SWITCHES_RX_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.rx_sr_data_cell.switches_rx)

  },
  {
    1,FE_PATHS_RX_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.rx_sr_data_cell.paths_rx)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_dcha_rr_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_DCHA_RR_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_dcha_rr_group_str[]
#ifdef INIT
   =
{
  {
    LAST_DCH_RR_INDEX,FE_DCHA_RR_ANY_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.dch_rr[0])
  },
  {
    1,FE_DCHA_RR_LAST_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.dch_rr[LAST_DCH_RR_INDEX])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_dchb_rr_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_DCHB_RR_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_dchb_rr_group_str[]
#ifdef INIT
   =
{
  {
    LAST_DCH_RR_INDEX,FE_DCHB_RR_ANY_REG,
    OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.dch_rr[0])
  },
  {
    1,FE_DCHB_RR_LAST_REG,
    OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.dch_rr[LAST_DCH_RR_INDEX])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_dcla_rr_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_DCLA_RR_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_dcla_rr_group_str[]
#ifdef INIT
   =
{
  {
    LAST_DCL_RR_INDEX,FE_DCLA_RR_ANY_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcla_regs.fe_dcs_dcla_detail.dcl_rr[0])
  },
  {
    1,FE_DCLA_RR_LAST_REG,
    OFFSETOF(FE_REGS,fe_dcs_dcla_regs.fe_dcs_dcla_detail.dcl_rr[LAST_DCL_RR_INDEX])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_dclb_rr_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_DCLB_RR_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_dclb_rr_group_str[]
#ifdef INIT
   =
{
  {
    LAST_DCL_RR_INDEX,FE_DCLB_RR_ANY_REG,
    OFFSETOF(FE_REGS,fe_dcs_dclb_regs.fe_dcs_dclb_detail.dcl_rr[0])
  },
  {
    1,FE_DCLB_RR_LAST_REG,
    OFFSETOF(FE_REGS,fe_dcs_dclb_regs.fe_dcs_dclb_detail.dcl_rr[LAST_DCL_RR_INDEX])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_ccsa_rr_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_CCSA_RR_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_ccsa_rr_group_str[]
#ifdef INIT
   =
{
  {
    LAST_CCS_RR_INDEX,FE_CCSA_RR_ANY_REG,
    OFFSETOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.ccs_rr[0])
  },
  {
    1,FE_CCSA_RR_LAST_REG,
    OFFSETOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.ccs_rr[LAST_CCS_RR_INDEX])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_ccsb_rr_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_CCSB_RR_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_ccsb_rr_group_str[]
#ifdef INIT
   =
{
  {
    LAST_CCS_RR_INDEX,FE_CCSB_RR_ANY_REG,
    OFFSETOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.ccs_rr[0])
  },
  {
    1,FE_CCSB_RR_LAST_REG,
    OFFSETOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.ccs_rr[LAST_CCS_RR_INDEX])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_ind_write_buff_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_IND_WRITE_BUFF_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_ind_write_buff_group_str[]
#ifdef INIT
   =
{
  {
    9,FE_WRITE_BUFF_ANY_REG,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.write_buffer.write_val[0])
  },
  {
    1,FE_WRITE_BUFF_LAST_REG,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.write_buffer.write_val[LAST_WB_INDEX])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_serdes_enablers_group_str
*TYPE: BUFFER
*DATE: 13/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_SERDES_ENABLERS_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_serdes_enablers_group_str[]
#ifdef INIT
   =
{
  {
    1,FE_SERDES_FRST_REG,
    OFFSETOF(FE_REGS,fe_serdes_eci_1_regs.fe_serdes_eci_1_detail.srds_frst_reg[0])
  },
  {
    63,FE_SERDES_REST_REG,
    OFFSETOF(FE_REGS,fe_serdes_eci_1_regs.fe_serdes_eci_1_detail.srds_rest_reg[0])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;
/********************************************************
*NAME
*  Fe_mac_ber_cntrs_group_str
*TYPE: BUFFER
*DATE: 13/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MAC_BER_CNTRS_GROUP' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe_mac_ber_cntrs_group_str[]
#ifdef INIT
   =
{
  {
    1,FE_MAC_BER_CNTRS_1_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_ber_cntrs_1[0])
  },
  {
    1,FE_MAC_BER_CNTRS_2_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_ber_cntrs_2[0])
  },
  {
    1,FE_MAC_BER_CNTRS_3_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_ber_cntrs_3[0])

  },
  {
    1,FE_MAC_BER_CNTRS_4_REG,
    OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_ber_cntrs_4[0])
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;

/********************************************************
*NAME
*  Fe1_Multi_dist_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MULTICAST_TABLE_A_REGS' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe1_Multi_dist_group_str[]
#ifdef INIT
   =
{
  {
    SOC_SAND_NOF_SPATIAL_MULTI_GROUPS,FE1_MULTICAST_DISTRIBUTION_REG,
    FE_INDIRECT_BASE+OFFSETOF(FE_INDIRECT_REGS, fe_multicast_table_a_regs)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;

/********************************************************
*NAME
*  Fe2_Multi_rep_low_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MULTICAST_TABLE_A_REGS' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe2_Multi_rep_low_group_str[]
#ifdef INIT
   =
{
  {
    SOC_SAND_NOF_SPATIAL_MULTI_GROUPS,FE2_MULTICAST_REPLICATION_L_REG,
    FE_INDIRECT_BASE+OFFSETOF(FE_INDIRECT_REGS, fe_multicast_table_a_regs)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;

/********************************************************
*NAME
*  Fe2_Multi_rep_high_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MULTICAST_TABLE_B_REGS' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe2_Multi_rep_high_group_str[]
#ifdef INIT
   =
{
  {
    SOC_SAND_NOF_SPATIAL_MULTI_GROUPS,FE2_MULTICAST_REPLICATION_L_REG,
    FE_INDIRECT_BASE+OFFSETOF(FE_INDIRECT_REGS,fe_multicast_table_b_regs)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;


/********************************************************
*NAME
*  Fe2_Multi_rep_high_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MULTICAST_TABLE_B_REGS' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe3_Multi_rep_group_str[]
#ifdef INIT
   =
{
  {
    SOC_SAND_NOF_SPATIAL_MULTI_GROUPS,FE3_MULTICAST_REPLICATION_REG,
    FE_INDIRECT_BASE+OFFSETOF(FE_INDIRECT_REGS,fe_multicast_table_b_regs)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;

/********************************************************
*NAME
*  Fe2_Multi_rep_high_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MULTICAST_TABLE_B_REGS' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe1_uni_dist_group_str[]
#ifdef INIT
   =
{
  {
    SOC_SAND_NOF_SPATIAL_MULTI_GROUPS,FE1_UNICAST_DISTRIBUTION_REG,
    FE_INDIRECT_BASE+OFFSETOF(FE_INDIRECT_REGS,fe_routing_table_regs)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;

/********************************************************
*NAME
*  Fe2_uni_routing_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MULTICAST_TABLE_B_REGS' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe2_uni_routing_group_str[]
#ifdef INIT
   =
{
  {
    SOC_SAND_NOF_SPATIAL_MULTI_GROUPS,FE2_UNICAST_ROUTING_REG,
    FE_INDIRECT_BASE+OFFSETOF(FE_INDIRECT_REGS,fe_routing_table_regs)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;

/********************************************************
*NAME
*  Fe2_uni_routing_group_str
*TYPE: BUFFER
*DATE: 20/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING STRUCTURE OF
*  'FE_MULTICAST_TABLE_B_REGS' WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'ELEMENT_OF_GROUP':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  ELEMENT_OF_GROUP
    Fe3_uni_routing_group_str[]
#ifdef INIT
   =
{
  {
    SOC_SAND_NOF_SPATIAL_MULTI_GROUPS,FE3_UNICAST_ROUTING_REG,
    FE_INDIRECT_BASE+OFFSETOF(FE_INDIRECT_REGS,fe3_uni_routing_table_regs)
  },
  /*
   * End of array indicator. Do not remove.
   */
  {0}
}
#endif
;


/********************************************************
*NAME
*  Desc_fe_reg
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING INFORMATION REGARDING MEMORY MAP OF
*  REGISTERS WITHIN THE FE CHIP.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'DESC_FE_REG':
*  See definition in dune_chips.h
*USAGE:
*  SEE dune_chips.h.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
  DESC_FE_REG
    Desc_fe_reg[]
#ifdef INIT
   =
{
  /*
   * SOC_SAND_INDIRECT REGISTERS
   * {
   */
  {
    FE_PURE_IND_ADDR_FIELD,
    FE_ACCESS_ADDR_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.indirect_access_addresss),
    PURE_ADDRESS_MASK,
    PURE_ADDRESS_SHIFT,
    "FE_PURE_IND_ADDR_FIELD",
    "\'pure address\' part of the \'access address\' register",
    "FE_ACCESS_ADDR_REG",
    "\'Access address\' register related to \'indirect access\'",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PURE_ADDRESS_MASK >> PURE_ADDRESS_SHIFT,0
  },
  {
    FE_MODULE_FIELD,
    FE_ACCESS_ADDR_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.indirect_access_addresss),
    MODULE_MASK,
    MODULE_SHIFT,
    "FE_MODULE_FIELD",
    "\'Module\' part of the \'access address\' register",
    "FE_ACCESS_ADDR_REG",
    "\'Access address\' register related to \'indirect access\'",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_module_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    MAX_MODULE,0
  },
  {
    FE_RD_NOT_WR_FIELD,
    FE_ACCESS_ADDR_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.indirect_access_addresss),
    RD_NOT_WR_MASK,
    RD_NOT_WR_SHIFT,
    "FE_RD_NOT_WR_FIELD",
    "\'Read-not-write\' bit of the \'access address\' register",
    "FE_ACCESS_ADDR_REG",
    "\'Access address\' register related to \'indirect access\'",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_rd_not_wr_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    RD_NOT_WR_MASK >> RD_NOT_WR_SHIFT,0
  },
  {
    FE_IND_TRIGGER_FIELD,
    FE_IND_TRIGGER_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.indirect_access_trigger),
    IND_TRIGGER_MASK,
    IND_TRIGGER_SHIFT,
    "FE_IND_TRIGGER_FIELD",
    "\'Trigger\' bit of the \'indirect access trigger\' register",
    "FE_IND_TRIGGER_REG",
    "\'indirect access trigger\' register related to \'indirect access\'",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_ind_trigger_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    IND_TRIGGER_MASK >> IND_TRIGGER_SHIFT,0
  },
  {
    FE_WRITE_BUFF_ANY_FIELD,
    FE_WRITE_BUFF_ANY_REG,
    FE_GEN_REGS_BLOCK,
    FE_IND_WRITE_BUFF_GROUP,
    0,
    ANY_WB_MASK,
    ANY_WB_SHIFT,
    "FE_WRITE_BUFF_ANY_FIELD",
    "Field of available bits of any element of the write buffer",
    "FE_WRITE_BUFF_ANY_REG",
    "Standard element in write buffer of \'indirect access\'",
    "FE_IND_WRITE_BUFF_GROUP",
    "Write buffer assigned to indirect access",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_ind_write_buff_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_WRITE_BUFF_LAST_FIELD,
    FE_WRITE_BUFF_LAST_REG,
    FE_GEN_REGS_BLOCK,
    FE_IND_WRITE_BUFF_GROUP,
    0,
    LAST_WB_MASK,
    LAST_WB_SHIFT,
    "FE_WRITE_BUFF_LAST_FIELD",
    "Field of available bits of any element of the write buffer",
    "FE_WRITE_BUFF_LAST_REG",
    "Last element in write buffer of \'indirect access\'",
    "FE_IND_WRITE_BUFF_GROUP",
    "Write buffer assigned to indirect access",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_ind_write_buff_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    LAST_WB_MASK >> LAST_WB_SHIFT,0
  },
  {
    FE_RTP_RR_FIELD,
    FE_RTP_RR_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.rtp_read_result),
    RTP_RR_MASK,
    RTP_RR_SHIFT,
    "FE_RTP_RR_FIELD",
    "\'Read result\' register of RTP (\'indirect access\')",
    "FE_RTP_RR_REG",
    "\'Read result\' register of RTP (\'indirect access\')",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE1_MULTICAST_DISTRIBUTION_FIELD,
    FE1_MULTICAST_DISTRIBUTION_REG,
    FE_INDIRECT_BLOCK,
    FE1_MULTICAST_DISTRIBUTION_GROUP,
    0,
    FE_ROUTING_MASK,
    FE_ROUTING_SHIFT,
    "FE1_MULTICAST_DISTRIBUTION_FIELD",
    "FE1 multicast distribution table",
    "FE1_MULTICAST_DISTRIBUTION_REG",
    "FE1 multicast distribution table",
    "FE1_MULTICAST_DISTRIBUTION_GROUP",
    "FE1 multicast distribution table",
    "FE_INDIRECT_BLOCK",
    "indirect block",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe1_Multi_dist_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    FE_ROUTING_MASK >> FE_ROUTING_SHIFT,0
  },
  {
    FE2_MULTICAST_REPLICATION_L_FIELD,
    FE2_MULTICAST_REPLICATION_L_REG,
    FE_INDIRECT_BLOCK,
    FE2_MULTICAST_REPLICATION_L_GROUP,
    0,
    FE_ROUTING_MASK,
    FE_ROUTING_SHIFT,
    "FE2_MULTICAST_REPLICATION_L_FIELD",
    "FE2 multicast replication table lower 32 bits",
    "FE2_MULTICAST_REPLICATION_L_REG",
    "FE2 multicast replication table lower 32 bits",
    "FE2_MULTICAST_REPLICATION_L_GROUP",
    "FE2 multicast replication table lower 32 bits",
    "FE_INDIRECT_BLOCK",
    "indirect block",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe2_Multi_rep_low_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    FE_ROUTING_MASK >> FE_ROUTING_SHIFT,0
  },
  {
    FE2_MULTICAST_REPLICATION_H_FIELD,
    FE2_MULTICAST_REPLICATION_H_REG,
    FE_INDIRECT_BLOCK,
    FE2_MULTICAST_REPLICATION_H_GROUP,
    0,
    FE_ROUTING_MASK,
    FE_ROUTING_SHIFT,
    "FE2_MULTICAST_REPLICATION_H_FIELD",
    "FE2 multicast replication table higher 32 bits",
    "FE2_MULTICAST_REPLICATION_H_REG",
    "FE2 multicast replication table higher 32 bit",
    "FE2_MULTICAST_REPLICATION_H_GROUP",
    "FE2 multicast replication table higher 32 bit",
    "FE_INDIRECT_BLOCK",
    "indirect block",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe2_Multi_rep_high_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    FE_ROUTING_MASK >> FE_ROUTING_SHIFT,0
  },
  {
    FE3_MULTICAST_REPLICATION_FIELD,
    FE3_MULTICAST_REPLICATION_REG,
    FE_INDIRECT_BLOCK,
    FE3_MULTICAST_REPLICATION_GROUP,
    0,
    FE_ROUTING_MASK,
    FE_ROUTING_SHIFT,
    "FE3_MULTICAST_REPLICATION_FIELD",
    "FE3 multicast replication table",
    "FE3_MULTICAST_REPLICATION_REG",
    "FE3 multicast replication table",
    "FE3_MULTICAST_REPLICATION_GROUP",
    "FE3 multicast replication table",
    "FE_INDIRECT_BLOCK",
    "indirect block",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe3_Multi_rep_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    FE_ROUTING_MASK >> FE_ROUTING_SHIFT,0
  },
  {
    FE1_UNICAST_DISTRIBUTION_FIELD,
    FE1_UNICAST_DISTRIBUTION_REG,
    FE_INDIRECT_BLOCK,
    FE1_UNICAST_DISTRIBUTION_GROUP,
    0,
    FE_ROUTING_MASK,
    FE_ROUTING_SHIFT,
    "FE1_UNICAST_DISTRIBUTION_FIELD",
    "FE1 unicast distribution table",
    "FE1_UNICAST_DISTRIBUTION_REG",
    "FE1 unicast distribution table",
    "FE1_UNICAST_DISTRIBUTION_GROUP",
    "FE1 unicast distribution table",
    "FE_INDIRECT_BLOCK",
    "indirect block",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe1_uni_dist_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    FE_ROUTING_MASK >> FE_ROUTING_SHIFT,0
  },
  {
    FE2_UNICAST_ROUTING_FIELD,
    FE2_UNICAST_ROUTING_REG,
    FE_INDIRECT_BLOCK,
    FE2_UNICAST_ROUTING_GROUP,
    0,
    FE_ROUTING_MASK,
    FE_ROUTING_SHIFT,
    "FE2_UNICAST_ROUTING_FIELD",
    "FE2 unicast routing table",
    "FE2_UNICAST_ROUTING_REG",
    "FE2 unicast routing table",
    "FE2_UNICAST_ROUTING_GROUP",
    "FE2 unicast routing table",
    "FE_INDIRECT_BLOCK",
    "indirect block",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe2_uni_routing_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    FE_ROUTING_MASK >> FE_ROUTING_SHIFT,0
  },
  {
    FE3_UNICAST_ROUTING_FIELD,
    FE3_UNICAST_ROUTING_REG,
    FE_INDIRECT_BLOCK,
    FE3_UNICAST_ROUTING_GROUP,
    0,
    FE_ROUTING_MASK,
    FE_ROUTING_SHIFT,
    "FE3_UNICAST_ROUTING_FIELD",
    "FE3 unicast routing table",
    "FE3_UNICAST_ROUTING_REG",
    "FE3 unicast routing table",
    "FE3_UNICAST_ROUTING_GROUP",
    "FE3 unicast routing table",
    "FE_INDIRECT_BLOCK",
    "indirect block",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_INDIRECT,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe3_uni_routing_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    FE_ROUTING_MASK >> FE_ROUTING_SHIFT,0
  },
  {
    FE_DCHA_RR_ANY_FIELD,
    FE_DCHA_RR_ANY_REG,
    FE_DCS_DCHA_BLOCK,
    FE_DCHA_RR_GROUP,
    0,
    DCH_RR_ANY_MASK,
    DCH_RR_ANY_SHIFT,
    "FE_DCHA_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of DCHa",
    "FE_DCHA_RR_ANY_REG",
    "Standard element in read-result buffer of DCHa (\'indirect access\')",
    "FE_DCHA_RR_GROUP",
    "Buffer assigned to read result of DCHa (indirect access)",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dcha_rr_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_DCHA_RR_LAST_FIELD,
    FE_DCHA_RR_LAST_REG,
    FE_DCS_DCHA_BLOCK,
    FE_DCHA_RR_GROUP,
    0,
    DCH_RR_LAST_MASK,
    DCH_RR_LAST_SHIFT,
    "FE_DCHA_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of DCHa",
    "FE_DCHA_RR_ANY_REG",
    "Standard element in read-result buffer of DCHa (\'indirect access\')",
    "FE_DCHA_RR_GROUP",
    "Buffer assigned to read result of DCHa (indirect access)",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dcha_rr_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    DCH_RR_LAST_MASK >> DCH_RR_LAST_SHIFT,0
  },
  {
    FE_DCHB_RR_ANY_FIELD,
    FE_DCHB_RR_ANY_REG,
    FE_DCS_DCHB_BLOCK,
    FE_DCHB_RR_GROUP,
    0,
    DCH_RR_ANY_MASK,
    DCH_RR_ANY_SHIFT,
    "FE_DCHB_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of dchb",
    "FE_DCHB_RR_ANY_REG",
    "Standard element in read-result buffer of dchb (\'indirect access\')",
    "FE_DCHB_RR_GROUP",
    "Buffer assigned to read result of dchb (indirect access)",
    "FE_DCS_DCHB_BLOCK",
    "dchb related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dchb_rr_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_DCHB_RR_LAST_FIELD,
    FE_DCHB_RR_LAST_REG,
    FE_DCS_DCHB_BLOCK,
    FE_DCHB_RR_GROUP,
    0,
    DCH_RR_LAST_MASK,
    DCH_RR_LAST_SHIFT,
    "FE_DCHB_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of dchb",
    "FE_DCHB_RR_ANY_REG",
    "Standard element in read-result buffer of dchb (\'indirect access\')",
    "FE_DCHB_RR_GROUP",
    "Buffer assigned to read result of dchb (indirect access)",
    "FE_DCS_DCHB_BLOCK",
    "dchb related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dchb_rr_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    DCH_RR_LAST_MASK >> DCH_RR_LAST_SHIFT,0
  },
  {
    FE_DCLA_RR_ANY_FIELD,
    FE_DCLA_RR_ANY_REG,
    FE_DCS_DCLA_BLOCK,
    FE_DCLA_RR_GROUP,
    0,
    DCL_RR_ANY_MASK,
    DCL_RR_ANY_SHIFT,
    "FE_DCLA_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of DCLa",
    "FE_DCLA_RR_ANY_REG",
    "Standard element in read-result buffer of DCLa (\'indirect access\')",
    "FE_DCLA_RR_GROUP",
    "Buffer assigned to read result of DCLa (indirect access)",
    "FE_DCS_DCLA_BLOCK",
    "DCLa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dcla_rr_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_DCLA_RR_LAST_FIELD,
    FE_DCLA_RR_LAST_REG,
    FE_DCS_DCLA_BLOCK,
    FE_DCLA_RR_GROUP,
    0,
    DCL_RR_LAST_MASK,
    DCL_RR_LAST_SHIFT,
    "FE_DCLA_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of DCLa",
    "FE_DCLA_RR_ANY_REG",
    "Standard element in read-result buffer of DCLa (\'indirect access\')",
    "FE_DCLA_RR_GROUP",
    "Buffer assigned to read result of DCLa (indirect access)",
    "FE_DCS_DCLA_BLOCK",
    "DCLa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dcla_rr_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    DCL_RR_LAST_MASK >> DCL_RR_LAST_SHIFT,0
  },
  {
    FE_DCLB_RR_ANY_FIELD,
    FE_DCLB_RR_ANY_REG,
    FE_DCS_DCLB_BLOCK,
    FE_DCLB_RR_GROUP,
    0,
    DCL_RR_ANY_MASK,
    DCL_RR_ANY_SHIFT,
    "FE_DCLB_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of dclb",
    "FE_DCLB_RR_ANY_REG",
    "Standard element in read-result buffer of dclb (\'indirect access\')",
    "FE_DCLB_RR_GROUP",
    "Buffer assigned to read result of dclb (indirect access)",
    "FE_DCS_DCLB_BLOCK",
    "dclb related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dclb_rr_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_DCLB_RR_LAST_FIELD,
    FE_DCLB_RR_LAST_REG,
    FE_DCS_DCLB_BLOCK,
    FE_DCLB_RR_GROUP,
    0,
    DCL_RR_LAST_MASK,
    DCL_RR_LAST_SHIFT,
    "FE_DCLB_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of dclb",
    "FE_DCLB_RR_ANY_REG",
    "Standard element in read-result buffer of dclb (\'indirect access\')",
    "FE_DCLB_RR_GROUP",
    "Buffer assigned to read result of dclb (indirect access)",
    "FE_DCS_DCLB_BLOCK",
    "dclb related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_dclb_rr_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    DCL_RR_LAST_MASK >> DCL_RR_LAST_SHIFT,0
  },
  {
    FE_CCSA_RR_ANY_FIELD,
    FE_CCSA_RR_ANY_REG,
    FE_CCSA_CCS_BLOCK,
    FE_CCSA_RR_GROUP,
    0,
    CCS_RR_ANY_MASK,
    CCS_RR_ANY_SHIFT,
    "FE_CCSA_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of CCSa",
    "FE_CCSA_RR_ANY_REG",
    "Standard element in read-result buffer of CCSa (\'indirect access\')",
    "FE_CCSA_RR_GROUP",
    "Buffer assigned to read result of CCSa (indirect access)",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_ccsa_rr_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CCSA_RR_LAST_FIELD,
    FE_CCSA_RR_LAST_REG,
    FE_CCSA_CCS_BLOCK,
    FE_CCSA_RR_GROUP,
    0,
    CCS_RR_LAST_MASK,
    CCS_RR_LAST_SHIFT,
    "FE_CCSA_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of CCSa",
    "FE_CCSA_RR_ANY_REG",
    "Standard element in read-result buffer of CCSa (\'indirect access\')",
    "FE_CCSA_RR_GROUP",
    "Buffer assigned to read result of CCSa (indirect access)",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_ccsa_rr_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    CCS_RR_LAST_MASK >> CCS_RR_LAST_SHIFT,0
  },
  {
    FE_CCSB_RR_ANY_FIELD,
    FE_CCSB_RR_ANY_REG,
    FE_CCSB_CCS_BLOCK,
    FE_CCSB_RR_GROUP,
    0,
    CCS_RR_ANY_MASK,
    CCS_RR_ANY_SHIFT,
    "FE_CCSB_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of ccsb",
    "FE_CCSB_RR_ANY_REG",
    "Standard element in read-result buffer of ccsb (\'indirect access\')",
    "FE_CCSB_RR_GROUP",
    "Buffer assigned to read result of ccsb (indirect access)",
    "FE_CCSB_CCS_BLOCK",
    "ccsb related registers in CCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_ccsb_rr_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CCSB_RR_LAST_FIELD,
    FE_CCSB_RR_LAST_REG,
    FE_CCSB_CCS_BLOCK,
    FE_CCSB_RR_GROUP,
    0,
    CCS_RR_LAST_MASK,
    CCS_RR_LAST_SHIFT,
    "FE_CCSB_RR_ANY_FIELD",
    "Field of available bits on any element of the read-results buffer of ccsb",
    "FE_CCSB_RR_ANY_REG",
    "Standard element in read-result buffer of ccsb (\'indirect access\')",
    "FE_CCSB_RR_GROUP",
    "Buffer assigned to read result of ccsb (indirect access)",
    "FE_CCSB_CCS_BLOCK",
    "ccsb related registers in CCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_ccsb_rr_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    CCS_RR_LAST_MASK >> CCS_RR_LAST_SHIFT,0
  },
  /*
   * End SOC_SAND_INDIRECT REGISTERS
   * }
   */
  {
    FE_CHIP_TYPE_FIELD,
    FE_VERSION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.version),
    FE200_CHIP_TYPE_MASK,
    FE200_CHIP_TYPE_SHIFT,
    "FE_CHIP_TYPE_FIELD",
    "Type of chip (FE)",
    "FE_VERSION_REG",
    "Version related information for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_DBG_VER_FIELD,
    FE_VERSION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.version),
    FE200_DBG_VER_MASK,
    FE200_DBG_VER_SHIFT,
    "FE_DBG_VER_FIELD",
    "Debug version of chip (FE)",
    "FE_VERSION_REG",
    "Version related information for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CHIP_VER_FIELD,
    FE_VERSION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.version),
    FE200_CHIP_VER_MASK,
    FE200_CHIP_VER_SHIFT,
    "FE_CHIP_VER_FIELD",
    "Chip version of FE",
    "FE_VERSION_REG",
    "Version related information for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CHIP_ID_FIELD,
    FE_CONFIGURATION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.configuration),
    CHIP_ID_MASK,
    CHIP_ID_SHIFT,
    "FE_CHIP_ID_FIELD",
    "Identifier of chip (FE) within fabric",
    "FE_CONFIGURATION_REG",
    "Configuration related fields for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_MODE_FE2_FIELD,
    FE_CONFIGURATION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.configuration),
    MOD_FE2_MASK,
    MOD_FE2_SHIFT,
    "FE_MODE_FE2_FIELD",
    "Operation mode of chip (FE): 0=FE13, 1=FE2",
    "FE_CONFIGURATION_REG",
    "Configuration related fields for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_mode_fe2_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_REPEATER_FIELD,
    FE_CONFIGURATION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.configuration),
    REPEATER_MASK,
    REPEATER_SHIFT,
    "FE_REPEATER_FIELD",
    "Operation mode of chip (FE): 0=FE, 1=Repeater",
    "FE_CONFIGURATION_REG",
    "Configuration related fields for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_repeater_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_PLANES_FIELD,
    FE_CONFIGURATION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.configuration),
    PLANES_MASK,
    PLANES_SHIFT,
    "FE_PLANES_FIELD",
    "Number of fabric planes in chip (FE) is 2 to the power of this value",
    "FE_CONFIGURATION_REG",
    "Configuration related fields for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_BINARY | DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_planes_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_MULTISTAGE_FIELD,
    FE_CONFIGURATION_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.configuration),
    MULTISTAGE_MASK,
    MULTISTAGE_SHIFT,
    "FE_MULTISTAGE_FIELD",
    "Indication on whether FE chip is in multistage configuration",
    "FE_CONFIGURATION_REG",
    "Configuration related fields for FE chip",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_BINARY | DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_multistage_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FECINT_FIELD,
    FE_INT_MODULE_SRC_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.interrupt_module_source),
    FECINT_MASK,
    FECINT_SHIFT,
    "FE_FECINT_FIELD",
    "The general interrupt bit",
    "FE_INT_MODULE_SRC_REG",
    "Register holds the inerrupt modules sources",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FECINT_MASK_FIELD,
    FE_INT_MODULE_SRC_MASK_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    MASK_OFFSET+OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.interrupt_module_source),
    FECINT_MASK,
    FECINT_SHIFT,
    "FE_FECINT_MASK_FIELD",
    "The general interrupt mask bit",
    "FE_INT_MODULE_SRC_MASK_REG",
    "Register holds the inerrupt mask for modules sources",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_TX_DATA_CELL_TRIGGER_FIELD,
    FE_TX_DATA_CELL_TRIGGER_REG,
    FE_GEN_REGS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.tx_data_cell_trigger),
    CELL_TRG_MASK,
    CELL_TRG_SHIFT,
    "FE_TX_DATA_CELL_TRIGGER_FIELD",
    "Trigger source routed data cell transmission by asserting this bit field",
    "FE_TX_DATA_CELL_TRIGGER_REG",
    "Register dedicated to trigger bit field",
    "",
    "",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CSRFO_1_FIELD,
    FE_SR_DATA_CELL_FIFO_IND_1_REG,
    FE_DCS_DCHA_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.sr_data_cell_fifo_ind_1),
    CSRFO_1_MASK,
    CSRFO_1_SHIFT,
    "FE_CSRFO_1_FIELD",
    "Indication on CPU source routed receive fifo overflow (links 0-31)",
    "FE_SR_DATA_CELL_FIFO_IND_1_REG",
    "Indication on CPU source routed receive fifo",
    "",
    "",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_csrfo_1_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRFNE_1_FIELD,
    FE_SR_DATA_CELL_FIFO_IND_1_REG,
    FE_DCS_DCHA_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.sr_data_cell_fifo_ind_1),
    SRFNE_1_MASK,
    SRFNE_1_SHIFT,
    "FE_SRFNE_1_FIELD",
    "Indication on CPU source routed receive fifo \'not empty\' state (links 0-31)",
    "FE_SR_DATA_CELL_FIFO_IND_1_REG",
    "Indication on CPU source routed receive fifo",
    "",
    "",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_srfne_1_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CSRFO_2_FIELD,
    FE_SR_DATA_CELL_FIFO_IND_2_REG,
    FE_DCS_DCHB_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.sr_data_cell_fifo_ind_2),
    CSRFO_2_MASK,
    CSRFO_2_SHIFT,
    "FE_CSRFO_2_FIELD",
    "Indication on CPU source routed receive fifo overflow (links 32-63)",
    "FE_SR_DATA_CELL_FIFO_IND_2_REG",
    "Indication on CPU source routed receive fifo",
    "",
    "",
    "FE_DCS_DCHB_BLOCK",
    "DCHb related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_csrfo_2_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRFNE_2_FIELD,
    FE_SR_DATA_CELL_FIFO_IND_2_REG,
    FE_DCS_DCHB_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.sr_data_cell_fifo_ind_2),
    SRFNE_2_MASK,
    SRFNE_2_SHIFT,
    "FE_SRFNE_2_FIELD",
    "Indication on CPU source routed receive fifo \'not empty\' state (links 32-63)",
    "FE_SR_DATA_CELL_FIFO_IND_2_REG",
    "Indication on CPU source routed receive fifo",
    "",
    "",
    "FE_DCS_DCHB_BLOCK",
    "DCHb related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_srfne_2_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_PEC_1_FIELD,
    FE_PKT_DATA_CELL_COUNTER_1_REG,
    FE_DCS_DCHA_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.pkt_data_cell_cntr_1),
    PEC_1_MASK,
    PEC_1_SHIFT,
    "FE_PEC_1_FIELD",
    "Counter of all data cells entering DCS (links 0-31)",
    "FE_PKT_DATA_CELL_COUNTER_1_REG",
    "Counter and overflow flag for all data cells entering DCS (links 0-31)",
    "",
    "",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT | DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_PECO_1_FIELD,
    FE_PKT_DATA_CELL_COUNTER_1_REG,
    FE_DCS_DCHA_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.pkt_data_cell_cntr_1),
    PECO_1_MASK,
    PECO_1_SHIFT,
    "FE_PECO_1_FIELD",
    "Overflow indication for counter of all data cells entering DCS (links 0-31)",
    "FE_PKT_DATA_CELL_COUNTER_1_REG",
    "Counter and overflow flag for all data cells entering DCS (links 0-31)",
    "",
    "",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_peco_1_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_PEC_2_FIELD,
    FE_PKT_DATA_CELL_COUNTER_2_REG,
    FE_DCS_DCHB_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.pkt_data_cell_cntr_2),
    PEC_2_MASK,
    PEC_2_SHIFT,
    "FE_PEC_2_FIELD",
    "Counter of all data cells entering DCS (links 0-31)",
    "FE_PKT_DATA_CELL_COUNTER_2_REG",
    "Counter and overflow flag for all data cells entering DCS (links 0-31)",
    "",
    "",
    "FE_DCS_DCHB_BLOCK",
    "DCHb related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT | DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_PECO_2_FIELD,
    FE_PKT_DATA_CELL_COUNTER_2_REG,
    FE_DCS_DCHB_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.pkt_data_cell_cntr_2),
    PECO_2_MASK,
    PECO_2_SHIFT,
    "FE_PECO_2_FIELD",
    "Overflow indication for counter of all data cells entering DCS (links 0-31)",
    "FE_PKT_DATA_CELL_COUNTER_2_REG",
    "Counter and overflow flag for all data cells entering DCS (links 0-31)",
    "",
    "",
    "FE_DCS_DCHB_BLOCK",
    "DCHb related registers in DCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_peco_2_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RCI_P_TRH_FIELD,
    FE_RCI_N_CGI_THRESHOLDS_REG,
    FE_DCS_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_eci_regs.fe_dcs_eci_detail.rci_n_cgi_thresholds),
    RCI_P_TRH_MASK,
    RCI_P_TRH_SHIFT,
    "FE_RCI_P_TRH_FIELD",
    "Congestion indication. Set RCI bit if sending fifo has more than indicated level",
    "FE_RCI_N_CGI_THRESHOLDS_REG",
    "Container of congestion thresholds (RCI and GCI)",
    "",
    "",
    "FE_DCS_ECI_BLOCK",
    "DCS related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_GCI_P_TRH_1_FIELD,
    FE_RCI_N_CGI_THRESHOLDS_REG,
    FE_DCS_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_eci_regs.fe_dcs_eci_detail.rci_n_cgi_thresholds),
    GCI_P_TRH_1_MASK,
    GCI_P_TRH_1_SHIFT,
    "FE_GCI_P_TRH_1_FIELD",
    "Congestion indication. Set GCI bits to 0x11 if output is over indicated level",
    "FE_RCI_N_CGI_THRESHOLDS_REG",
    "Container of congestion thresholds (RCI and GCI)",
    "",
    "",
    "FE_DCS_ECI_BLOCK",
    "DCS related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_GCI_P_TRH_2_FIELD,
    FE_RCI_N_CGI_THRESHOLDS_REG,
    FE_DCS_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_eci_regs.fe_dcs_eci_detail.rci_n_cgi_thresholds),
    GCI_P_TRH_2_MASK,
    GCI_P_TRH_2_SHIFT,
    "FE_GCI_P_TRH_2_FIELD",
    "Congestion indication. Set GCI bits to 0x10 if output is over indicated level",
    "FE_RCI_N_CGI_THRESHOLDS_REG",
    "Container of congestion thresholds (RCI and GCI)",
    "",
    "",
    "FE_DCS_ECI_BLOCK",
    "DCS related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_GCI_P_TRH_3_FIELD,
    FE_RCI_N_CGI_THRESHOLDS_REG,
    FE_DCS_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_dcs_eci_regs.fe_dcs_eci_detail.rci_n_cgi_thresholds),
    GCI_P_TRH_3_MASK,
    GCI_P_TRH_3_SHIFT,
    "FE_GCI_P_TRH_3_FIELD",
    "Congestion indication. Set GCI bits to 0x01 if output is over indicated level",
    "FE_RCI_N_CGI_THRESHOLDS_REG",
    "Container of congestion thresholds (RCI and GCI)",
    "",
    "",
    "FE_DCS_ECI_BLOCK",
    "DCS related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_NO_FIELD,
    FE_CCS_OVERFLOW_REG,
    FE_CCSA_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.ccs_overflows),
    CCS_OVERFLOWS_MASK,
    CCS_OVERFLOWS_SHIFT,
    "",
    "interrupt register for several CCS FIFO Overflows",
    "FE_CCS_OVERFLOW_REG",
    "",
    "",
    "",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_UNSIGNED_INT | DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CTR_CELL_CNT_1_FIELD,
    FE_CONTROL_CELLS_CNTR_1_REG,
    FE_CCSA_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.control_cells_cntr_1),
    CTR_CELL_CNT_1_MASK,
    CTR_CELL_CNT_1_SHIFT,
    "FE_CTR_CELL_CNT_1_FIELD",
    "Counter of all control cells entering CCS (links 0-31)",
    "FE_CONTROL_CELLS_CNTR_1_REG",
    "Counter and overflow flag for all data cells entering CCS (links 0-31)",
    "",
    "",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_UNSIGNED_INT | DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CTR_CELL_CNT_O_1_FIELD,
    FE_CONTROL_CELLS_CNTR_1_REG,
    FE_CCSA_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.control_cells_cntr_1),
    CTR_CELL_CNT_O_1_MASK,
    CTR_CELL_CNT_O_1_SHIFT,
    "FE_CTR_CELL_CNT_O_1_FIELD",
    "Overflow indication for counter of all control cells entering CCS (links 0-31)",
    "FE_CONTROL_CELLS_CNTR_1_REG",
    "Counter and overflow flag for all data cells entering DCS (links 0-31)",
    "",
    "",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_ctr_cell_cnt_o_1_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_NO_FIELD,
    FE_CCS_OVERFLOW_REG,
    FE_CCSB_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.ccs_overflows),
    CCS_OVERFLOWS_MASK,
    CCS_OVERFLOWS_SHIFT,
    "",
    "interrupt register for several CCS FIFO Overflows",
    "FE_CCS_OVERFLOW_REG",
    "",
    "",
    "",
    "FE_CCSB_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_UNSIGNED_INT | DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CTR_CELL_CNT_2_FIELD,
    FE_CONTROL_CELLS_CNTR_2_REG,
    FE_CCSB_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.control_cells_cntr_2),
    CTR_CELL_CNT_2_MASK,
    CTR_CELL_CNT_2_SHIFT,
    "FE_CTR_CELL_CNT_2_FIELD",
    "Counter of all control cells entering CCS (links 32-63)",
    "FE_CONTROL_CELLS_CNTR_2_REG",
    "Counter and overflow flag for all data cells entering CCS (links 32-63)",
    "",
    "",
    "FE_CCSB_CCS_BLOCK",
    "ccsb related registers in CCS of FE chip",
    DISPLAY_UNSIGNED_INT | DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_CTR_CELL_CNT_O_2_FIELD,
    FE_CONTROL_CELLS_CNTR_2_REG,
    FE_CCSB_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.control_cells_cntr_2),
    CTR_CELL_CNT_O_2_MASK,
    CTR_CELL_CNT_O_2_SHIFT,
    "FE_CTR_CELL_CNT_O_2_FIELD",
    "Overflow indication for counter of all control cells entering CCS (links 32-63)",
    "FE_CONTROL_CELLS_CNTR_2_REG",
    "Counter and overflow flag for all data cells entering DCS (links 32-63)",
    "",
    "",
    "FE_CCSB_CCS_BLOCK",
    "ccsb related registers in CCS of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_ctr_cell_cnt_o_2_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRC_D_ID_N_1_FIELD,
    FE_CONNECTIVITY_1_REG,
    FE_CCSA_CCS_BLOCK,
    FE_CONNECT_MAP_GROUP,
    0,
    SRC_D_ID_N_1_MASK,
    SRC_D_ID_N_1_SHIFT,
    "FE_SRC_D_ID_N_1_FIELD",
    "Source device ID of link N (0-31)",
    "FE_CONNECTIVITY_1_REG",
    "Connectivity register of link N (0-31)",
    "FE_CONNECT_MAP_GROUP",
    "Connectivity map on all links (0-31 and 32-63)",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_connect_map_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRC_D_LVL_N_1_FIELD,
    FE_CONNECTIVITY_1_REG,
    FE_CCSA_CCS_BLOCK,
    FE_CONNECT_MAP_GROUP,
    0,
    SRC_D_LVL_N_1_MASK,
    SRC_D_LVL_N_1_SHIFT,
    "FE_SRC_D_LVL_N_1_FIELD",
    "Source device level of link N (0-31)",
    "FE_CONNECTIVITY_1_REG",
    "Connectivity register of link N (0-31)",
    "FE_CONNECT_MAP_GROUP",
    "Connectivity map on all links (0-31 and 32-63)",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)Fe_src_d_lvl_n_1_vals,
    (ELEMENT_OF_GROUP *)Fe_connect_map_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRC_D_LN_N_N_1_FIELD,
    FE_CONNECTIVITY_1_REG,
    FE_CCSA_CCS_BLOCK,
    FE_CONNECT_MAP_GROUP,
    0,
    SRC_D_LN_N_N_1_MASK,
    SRC_D_LN_N_N_1_SHIFT,
    "FE_SRC_D_LVL_N_1_FIELD",
    "Source device link number of link N (0-31)",
    "FE_CONNECTIVITY_1_REG",
    "Connectivity register of link N (0-31)",
    "FE_CONNECT_MAP_GROUP",
    "Connectivity map on all links (0-31 and 32-63)",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_connect_map_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRC_D_ID_N_2_FIELD,
    FE_CONNECTIVITY_2_REG,
    FE_CCSB_CCS_BLOCK,
    FE_CONNECT_MAP_GROUP,
    0,
    SRC_D_ID_N_2_MASK,
    SRC_D_ID_N_2_SHIFT,
    "FE_SRC_D_ID_N_2_FIELD",
    "Source device ID of link N (32-63)",
    "FE_CONNECTIVITY_2_REG",
    "Connectivity register of link N (32-63)",
    "FE_CONNECT_MAP_GROUP",
    "Connectivity map on all links (0-31 and 32-63)",
    "FE_CCSB_CCS_BLOCK",
    "ccsb related registers in CCS of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_connect_map_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRC_D_LVL_N_2_FIELD,
    FE_CONNECTIVITY_2_REG,
    FE_CCSB_CCS_BLOCK,
    FE_CONNECT_MAP_GROUP,
    0,
    SRC_D_LVL_N_2_MASK,
    SRC_D_LVL_N_2_SHIFT,
    "FE_SRC_D_LVL_N_2_FIELD",
    "Source device level of link N (32-63)",
    "FE_CONNECTIVITY_2_REG",
    "Connectivity register of link N (32-63)",
    "FE_CONNECT_MAP_GROUP",
    "Connectivity map on all links (0-31 and 32-63)",
    "FE_CCSB_CCS_BLOCK",
    "ccsb related registers in CCS of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)Fe_src_d_lvl_n_2_vals,
    (ELEMENT_OF_GROUP *)Fe_connect_map_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRC_D_LN_N_N_2_FIELD,
    FE_CONNECTIVITY_2_REG,
    FE_CCSB_CCS_BLOCK,
    FE_CONNECT_MAP_GROUP,
    0,
    SRC_D_LN_N_N_2_MASK,
    SRC_D_LN_N_N_2_SHIFT,
    "FE_SRC_D_LVL_N_2_FIELD",
    "Source device link number of link N (32-63)",
    "FE_CONNECTIVITY_2_REG",
    "Connectivity register of link N (32-63)",
    "FE_CONNECT_MAP_GROUP",
    "Connectivity map on all links (0-31 and 32-63)",
    "FE_CCSB_CCS_BLOCK",
    "ccsb related registers in CCS of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_connect_map_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FEL_N_00_CHANGED_FIELD,
    FE_CONNECTIVITY_MAP_CHANGE_1_REG,
    FE_CCSA_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.connectivity_map_change_1),
    FEL_N_00_CHANGED_MASK,
    FEL_N_00_CHANGED_SHIFT,
    "FE_FEL_N_00_CHANGED_FIELD",
    "Indication on change of connectivity map at link 0",
    "FE_CONNECTIVITY_MAP_CHANGE_1_REG",
    "Bit map of change in connectivity map (links 0-31)",
    "",
    "",
    "FE_CCSA_CCS_BLOCK",
    "CCSa related registers in CCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FEL_N_32_CHANGED_FIELD,
    FE_CONNECTIVITY_MAP_CHANGE_2_REG,
    FE_CCSB_CCS_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.connectivity_map_change_2),
    FEL_N_32_CHANGED_MASK,
    FEL_N_32_CHANGED_SHIFT,
    "FE_FEL_N_32_CHANGED_FIELD",
    "Indication on change of connectivity map at link 32",
    "FE_CONNECTIVITY_MAP_CHANGE_2_REG",
    "Bit map of change in connectivity map (links 32-63)",
    "",
    "",
    "FE_CCSB_CCS_BLOCK",
    "CCSb related registers in CCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_SRC_MUL_FIELD,
    FE_ROUTING_PROC_ENABLES_REG,
    FE_RTP_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_eci_regs.fe_rtp_eci_detail.routing_processor_enablers),
    SRC_MUL_MASK,
    SRC_MUL_SHIFT,
    "FE_SRC_MUL_FIELD",
    "Splitting of lookup key to FE3 multicast routing table",
    "FE_ROUTING_PROC_ENABLES_REG",
    "Routing processor controls",
    "",
    "",
    "FE_RTP_ECI_BLOCK",
    "RTP related registers in ECI of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SRC_MUL_MAX,SRC_MUL_MIN
  },
  {
    FE_RM_DID_OR_FIELD,
    FE_REACH_FAULT_IND_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.reach_fault_ind),
    RM_DID_OR_MASK,
    RM_DID_OR_SHIFT,
    "FE_RM_DID_OR_FIELD",
    "Indication on reachability message with destination ID that is out-of-range",
    "FE_REACH_FAULT_IND_REG",
    "Bit map of error in received reachability messages",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_rm_did_or_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FE_23_DUP_ID_FIELD,
    FE_REACH_FAULT_IND_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.reach_fault_ind),
    FE_23_DUP_ID_MASK,
    FE_23_DUP_ID_SHIFT,
    "FE_FE_23_DUP_ID_FIELD",
    "Indication on reachability message with duplicate link to destination",
    "FE_REACH_FAULT_IND_REG",
    "Bit map of error in received reachability messages",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)Fe_fe_23_dup_id_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FE_23_DDEST1_FIELD,
    FE_REACH_DUP_DEST_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.reach_dup_link),
    FE_23_DDEST1_MASK,
    FE_23_DDEST1_SHIFT,
    "FE_FE_23_DDEST1_FIELD",
    "First (of many) link thru which destination was reported reachable",
    "FE_REACH_DUP_DEST_REG",
    "Event qualifier for the \'FE23 duplicate id\' indication flag",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FE_23_DDEST2_FIELD,
    FE_REACH_DUP_DEST_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.reach_dup_link),
    FE_23_DDEST2_MASK,
    FE_23_DDEST2_SHIFT,
    "FE_FE_23_DDEST2_FIELD",
    "Second link thru which destination was reported reachable",
    "FE_REACH_DUP_DEST_REG",
    "Event qualifier for the \'FE23 duplicate id\' indication flag",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_FE_23_DEST_ID_FIELD,
    FE_REACH_DUP_DEST_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.reach_dup_link),
    FE_23_DEST_ID_MASK,
    FE_23_DEST_ID_SHIFT,
    "FE_FE_23_DEST_ID_FIELD",
    "Destination ID that had duplicate links",
    "FE_REACH_DUP_DEST_REG",
    "Event qualifier for the \'FE23 duplicate id\' indication flag",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_OR_DEST_ID_FIELD,
    FE_REACH_DEST_OR_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.reach_dest_outofrange),
    OR_DEST_ID_MASK,
    OR_DEST_ID_SHIFT,
    "FE_OR_DEST_ID_FIELD",
    "Value of out of range destination ID received in reachability map",
    "FE_REACH_DEST_OR_REG",
    "Event qualifier for the \'RM destination id out-of-range\' indication flag",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_OR_INPUT_LN_FIELD,
    FE_REACH_DEST_OR_REG,
    FE_RTP_RTP_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_rtp_rtp_regs.fe_rtp_rtp_detail.reach_dest_outofrange),
    OR_INPUT_LN_MASK,
    OR_INPUT_LN_SHIFT,
    "FE_OR_INPUT_LN_FIELD",
    "Link thru which the offending value was receeived",
    "FE_REACH_DEST_OR_REG",
    "Event qualifier for the \'RM destination id out-of-range\' indication flag",
    "",
    "",
    "FE_RTP_RTP_BLOCK",
    "RTP related registers in RTP of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RMAC_CRC_D_FIELD,
    FE_MAC_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.mac_enablers),
    RMAC_CRC_D_MASK,
    RMAC_CRC_D_SHIFT,
    "FE_RMAC_CRC_D_FIELD",
    "If bit is asserted, MAC deletes a received cell with CRC error",
    "FE_MAC_ENABLERS_REG",
    "MAC configuration parameters: CRC and counters",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_rmac_crc_d_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_TX_CDC_FIELD,
    FE_MAC_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.mac_enablers),
    TX_CDC_MASK,
    TX_CDC_SHIFT,
    "FE_TX_CDC_FIELD",
    "Control functionality of MAC transmit counters",
    "FE_MAC_ENABLERS_REG",
    "MAC configuration parameters: CRC and counters",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_tx_cdc_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    2,0
  },
  {
    FE_RX_CDC_FIELD,
    FE_MAC_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.mac_enablers),
    RX_CDC_MASK,
    RX_CDC_SHIFT,
    "FE_RX_CDC_FIELD",
    "Control functionality of MAC receive counters",
    "FE_MAC_ENABLERS_REG",
    "MAC configuration parameters: CRC and counters",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_rx_cdc_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    2,0
  },
  {
    FE_RX_GC_CNT_N_1_FIELD,
    FE_MAC_RX_CNTRS_1_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_GC_CNT_N_MASK,
    RX_GC_CNT_N_SHIFT,
    "FE_RX_GC_CNT_N_1_FIELD",
    "Counter of good cells received at link N",
    "FE_MAC_RX_CNTRS_1_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CRC_ERR_N_1_FIELD,
    FE_MAC_RX_CNTRS_1_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CRC_ERR_N_MASK,
    RX_CRC_ERR_N_SHIFT,
    "FE_RX_CRC_ERR_N_1_FIELD",
    "Flag. Asserted when cell with CRC error is received at link N",
    "FE_MAC_RX_CNTRS_1_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_MA_ERR_N_1_FIELD,
    FE_MAC_RX_CNTRS_1_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_MA_ERR_N_MASK,
    RX_MA_ERR_N_SHIFT,
    "FE_RX_MA_ERR_N_1_FIELD",
    "Flag. Asserted when cell with misalignment error is received at link N",
    "FE_MAC_RX_CNTRS_1_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CODEG_ERR_N_1_FIELD,
    FE_MAC_RX_CNTRS_1_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CODEG_ERR_N_MASK,
    RX_CODEG_ERR_N_SHIFT,
    "FE_RX_CODEG_ERR_N_1_FIELD",
    "Flag. Asserted when cell with code-group error is received at link N",
    "FE_MAC_RX_CNTRS_1_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_GC_CNT_N_2_FIELD,
    FE_MAC_RX_CNTRS_2_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_GC_CNT_N_MASK,
    RX_GC_CNT_N_SHIFT,
    "FE_RX_GC_CNT_N_2_FIELD",
    "Counter of good cells received at link N",
    "FE_MAC_RX_CNTRS_2_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CRC_ERR_N_2_FIELD,
    FE_MAC_RX_CNTRS_2_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CRC_ERR_N_MASK,
    RX_CRC_ERR_N_SHIFT,
    "FE_RX_CRC_ERR_N_2_FIELD",
    "Flag. Asserted when cell with CRC error is received at link N",
    "FE_MAC_RX_CNTRS_2_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_MA_ERR_N_2_FIELD,
    FE_MAC_RX_CNTRS_2_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_MA_ERR_N_MASK,
    RX_MA_ERR_N_SHIFT,
    "FE_RX_MA_ERR_N_2_FIELD",
    "Flag. Asserted when cell with misalignment error is received at link N",
    "FE_MAC_RX_CNTRS_2_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CODEG_ERR_N_2_FIELD,
    FE_MAC_RX_CNTRS_2_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CODEG_ERR_N_MASK,
    RX_CODEG_ERR_N_SHIFT,
    "FE_RX_CODEG_ERR_N_2_FIELD",
    "Flag. Asserted when cell with code-group error is received at link N",
    "FE_MAC_RX_CNTRS_2_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_GC_CNT_N_3_FIELD,
    FE_MAC_RX_CNTRS_3_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_GC_CNT_N_MASK,
    RX_GC_CNT_N_SHIFT,
    "FE_RX_GC_CNT_N_3_FIELD",
    "Counter of good cells received at link N",
    "FE_MAC_RX_CNTRS_3_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CRC_ERR_N_3_FIELD,
    FE_MAC_RX_CNTRS_3_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CRC_ERR_N_MASK,
    RX_CRC_ERR_N_SHIFT,
    "FE_RX_CRC_ERR_N_3_FIELD",
    "Flag. Asserted when cell with CRC error is received at link N",
    "FE_MAC_RX_CNTRS_3_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_MA_ERR_N_3_FIELD,
    FE_MAC_RX_CNTRS_3_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_MA_ERR_N_MASK,
    RX_MA_ERR_N_SHIFT,
    "FE_RX_MA_ERR_N_3_FIELD",
    "Flag. Asserted when cell with misalignment error is received at link N",
    "FE_MAC_RX_CNTRS_3_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CODEG_ERR_N_3_FIELD,
    FE_MAC_RX_CNTRS_3_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CODEG_ERR_N_MASK,
    RX_CODEG_ERR_N_SHIFT,
    "FE_RX_CODEG_ERR_N_3_FIELD",
    "Flag. Asserted when cell with code-group error is received at link N",
    "FE_MAC_RX_CNTRS_3_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_GC_CNT_N_4_FIELD,
    FE_MAC_RX_CNTRS_4_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_GC_CNT_N_MASK,
    RX_GC_CNT_N_SHIFT,
    "FE_RX_GC_CNT_N_4_FIELD",
    "Counter of good cells received at link N",
    "FE_MAC_RX_CNTRS_4_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CRC_ERR_N_4_FIELD,
    FE_MAC_RX_CNTRS_4_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CRC_ERR_N_MASK,
    RX_CRC_ERR_N_SHIFT,
    "FE_RX_CRC_ERR_N_4_FIELD",
    "Flag. Asserted when cell with CRC error is received at link N",
    "FE_MAC_RX_CNTRS_4_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_MA_ERR_N_4_FIELD,
    FE_MAC_RX_CNTRS_4_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_MA_ERR_N_MASK,
    RX_MA_ERR_N_SHIFT,
    "FE_RX_MA_ERR_N_4_FIELD",
    "Flag. Asserted when cell with misalignment error is received at link N",
    "FE_MAC_RX_CNTRS_4_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_RX_CODEG_ERR_N_4_FIELD,
    FE_MAC_RX_CNTRS_4_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_RX_CNTRS_GROUP,
    0,
    RX_CODEG_ERR_N_MASK,
    RX_CODEG_ERR_N_SHIFT,
    "FE_RX_CODEG_ERR_N_4_FIELD",
    "Flag. Asserted when cell with code-group error is received at link N",
    "FE_MAC_RX_CNTRS_4_REG",
    "Receive cells counter and indications for link N",
    "FE_MAC_RX_CNTRS_GROUP",
    "RX counters and indications on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_rx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_TX_CNT_N_1_FIELD,
    FE_MAC_TX_CNTRS_1_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_TX_CNTRS_GROUP,
    0,
    TX_CNT_N_MASK,
    TX_CNT_N_SHIFT,
    "FE_TX_CNT_N_1_FIELD",
    "Counter of cells transmitted from link N (0-15)",
    "FE_MAC_TX_CNTRS_1_REG",
    "Transmit cells counter for link N (0-15)",
    "FE_MAC_TX_CNTRS_GROUP",
    "TX counters on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_tx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_TX_CNT_N_2_FIELD,
    FE_MAC_TX_CNTRS_2_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_TX_CNTRS_GROUP,
    0,
    TX_CNT_N_MASK,
    TX_CNT_N_SHIFT,
    "FE_TX_CNT_N_2_FIELD",
    "Counter of cells transmitted from link N (16-31)",
    "FE_MAC_TX_CNTRS_2_REG",
    "Transmit cells counter for link N (16-31)",
    "FE_MAC_TX_CNTRS_GROUP",
    "TX counters on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_tx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_TX_CNT_N_3_FIELD,
    FE_MAC_TX_CNTRS_3_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_TX_CNTRS_GROUP,
    0,
    TX_CNT_N_MASK,
    TX_CNT_N_SHIFT,
    "FE_TX_CNT_N_3_FIELD",
    "Counter of cells transmitted from link N (32-47)",
    "FE_MAC_TX_CNTRS_3_REG",
    "Transmit cells counter for link N (32-47)",
    "FE_MAC_TX_CNTRS_GROUP",
    "TX counters on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_tx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_TX_CNT_N_4_FIELD,
    FE_MAC_TX_CNTRS_4_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_TX_CNTRS_GROUP,
    0,
    TX_CNT_N_MASK,
    TX_CNT_N_SHIFT,
    "FE_TX_CNT_N_4_FIELD",
    "Counter of cells transmitted from link N (48-63)",
    "FE_MAC_TX_CNTRS_4_REG",
    "Transmit cells counter for link N (48-63)",
    "FE_MAC_TX_CNTRS_GROUP",
    "TX counters on all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_tx_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_PRBS_1_GENO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_1_GENO_MASK,
    PRBS_1_GENO_SHIFT,
    "FE_PRBS_1_GENO_FIELD",
    "Flag. If set then pseudo random sequence is sent through link \'prbs_1_genL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_geno_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_1_CKO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_1_CKO_MASK,
    PRBS_1_CKO_SHIFT,
    "FE_PRBS_1_CKO_FIELD",
    "Flag. If set then PRBS checker is activated on link \'prbs_1_ckL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_cko_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_2_GENO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_2_GENO_MASK,
    PRBS_2_GENO_SHIFT,
    "FE_PRBS_2_GENO_FIELD",
    "Flag. If set then pseudo random sequence is sent through link \'prbs_2_genL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_geno_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_2_CKO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_2_CKO_MASK,
    PRBS_2_CKO_SHIFT,
    "FE_PRBS_2_CKO_FIELD",
    "Flag. If set then PRBS checker is activated on link \'prbs_2_ckL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_cko_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_3_GENO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_3_GENO_MASK,
    PRBS_3_GENO_SHIFT,
    "FE_PRBS_3_GENO_FIELD",
    "Flag. If set then pseudo random sequence is sent through link \'prbs_3_genL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_geno_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_3_CKO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_3_CKO_MASK,
    PRBS_3_CKO_SHIFT,
    "FE_PRBS_3_CKO_FIELD",
    "Flag. If set then PRBS checker is activated on link \'prbs_3_ckL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_cko_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_4_GENO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_4_GENO_MASK,
    PRBS_4_GENO_SHIFT,
    "FE_PRBS_4_GENO_FIELD",
    "Flag. If set then pseudo random sequence is sent through link \'prbs_4_genL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_geno_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_4_CKO_FIELD,
    FE_PRBS_ENABLERS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_enablers),
    PRBS_4_CKO_MASK,
    PRBS_4_CKO_SHIFT,
    "FE_PRBS_4_CKO_FIELD",
    "Flag. If set then PRBS checker is activated on link \'prbs_4_ckL\'",
    "FE_PRBS_ENABLERS_REG",
    "PRBS configuration parameters: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_prbs_1_cko_field_vals,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PRBS_1_GENL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_1_GENL_MASK,
    PRBS_1_GENL_SHIFT,
    "FE_PRBS_1_GENL_FIELD",
    "Flag. Send PRBS thru this link (0-15) if \'prbs_1_genO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_1_GENL_MASK >> PRBS_1_GENL_SHIFT,0
  },
  {
    FE_PRBS_1_CKL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_1_CKL_MASK,
    PRBS_1_CKL_SHIFT,
    "FE_PRBS_1_CKL_FIELD",
    "Flag. Activate PRBS checker on this link (0-15) if \'prbs_1_ckO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_1_CKL_MASK >> PRBS_1_CKL_SHIFT,0
  },
  {
    FE_PRBS_2_GENL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_2_GENL_MASK,
    PRBS_2_GENL_SHIFT,
    "FE_PRBS_2_GENL_FIELD",
    "Flag. Send PRBS thru this link (16-31) if \'prbs_2_genO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_2_GENL_MASK >> PRBS_2_GENL_SHIFT,0
  },
  {
    FE_PRBS_2_CKL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_2_CKL_MASK,
    PRBS_2_CKL_SHIFT,
    "FE_PRBS_2_CKL_FIELD",
    "Flag. Activate PRBS checker on this link (16-31) if \'prbs_2_ckO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_2_CKL_MASK >> PRBS_2_CKL_SHIFT,0
  },
  {
    FE_PRBS_3_GENL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_3_GENL_MASK,
    PRBS_3_GENL_SHIFT,
    "FE_PRBS_3_GENL_FIELD",
    "Flag. Send PRBS thru this link (32-47) if \'prbs_3_genO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_3_GENL_MASK >> PRBS_3_GENL_SHIFT,0
  },
  {
    FE_PRBS_3_CKL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_3_CKL_MASK,
    PRBS_3_CKL_SHIFT,
    "FE_PRBS_3_CKL_FIELD",
    "Flag. Activate PRBS checker on this link (32-47) if \'prbs_3_ckO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_3_CKL_MASK >> PRBS_3_CKL_SHIFT,0
  },
  {
    FE_PRBS_4_GENL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_4_GENL_MASK,
    PRBS_4_GENL_SHIFT,
    "FE_PRBS_4_GENL_FIELD",
    "Flag. Send PRBS thru this link (48-63) if \'prbs_4_genO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_4_GENL_MASK >> PRBS_4_GENL_SHIFT,0
  },
  {
    FE_PRBS_4_CKL_FIELD,
    FE_PRBS_LINK_NUMS_REG,
    FE_MAC_ECI_BLOCK,
    FE_NO_GROUP,
    OFFSETOF(FE_REGS,fe_mac_eci_regs.fe_mac_eci_detail.prbs_link_nums),
    PRBS_4_CKL_MASK,
    PRBS_4_CKL_SHIFT,
    "FE_PRBS_4_CKL_FIELD",
    "Flag. Activate PRBS checker on this link (48-63) if \'prbs_1_ckO\' is set",
    "FE_PRBS_LINK_NUMS_REG",
    "PRBS link numbers: Generators and checkers",
    "",
    "",
    "FE_MAC_ECI_BLOCK",
    "MAC related registers in ECI of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)0,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PRBS_4_CKL_MASK >> PRBS_4_CKL_SHIFT,0
  },
  {
    FE_BER_CNT_1_FIELD,
    FE_MAC_BER_CNTRS_1_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_CNT_MASK,
    BER_CNT_SHIFT,
    "FE_BER_CNT_1_FIELD",
    "Counter. Counts PRBS errors on checker specified on \'prbs_1_genL\' (0-15)",
    "FE_MAC_BER_CNTRS_1_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_BER_VALID_1_FIELD,
    FE_MAC_BER_CNTRS_1_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_VALID_MASK,
    BER_VALID_SHIFT,
    "FE_BER_VALID_1_FIELD",
    "Flag. Bit corresponding to link N is set to non-zero value when PRBS\r\n"
    "  errors cross a threshold",
    "FE_MAC_BER_CNTRS_1_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_BER_CNT_2_FIELD,
    FE_MAC_BER_CNTRS_2_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_CNT_MASK,
    BER_CNT_SHIFT,
    "FE_BER_CNT_2_FIELD",
    "Counter. Counts PRBS errors on checker specified on \'prbs_2_genL\' (16-31)",
    "FE_MAC_BER_CNTRS_2_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_BER_VALID_2_FIELD,
    FE_MAC_BER_CNTRS_2_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_VALID_MASK,
    BER_VALID_SHIFT,
    "FE_BER_VALID_2_FIELD",
    "Flag. Bit corresponding to link N is set to non-zero value when PRBS\r\n"
    "  errors cross a threshold",
    "FE_MAC_BER_CNTRS_2_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_BER_CNT_3_FIELD,
    FE_MAC_BER_CNTRS_3_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_CNT_MASK,
    BER_CNT_SHIFT,
    "FE_BER_CNT_3_FIELD",
    "Counter. Counts PRBS errors on checker specified on \'prbs_3_genL\' (32-47)",
    "FE_MAC_BER_CNTRS_3_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_BER_VALID_3_FIELD,
    FE_MAC_BER_CNTRS_3_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_VALID_MASK,
    BER_VALID_SHIFT,
    "FE_BER_VALID_3_FIELD",
    "Flag. Bit corresponding to link N is set to non-zero value when PRBS\r\n"
    "  errors cross a threshold",
    "FE_MAC_BER_CNTRS_3_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_BER_CNT_4_FIELD,
    FE_MAC_BER_CNTRS_4_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_CNT_MASK,
    BER_CNT_SHIFT,
    "FE_BER_CNT_4_FIELD",
    "Counter. Counts PRBS errors on checker specified on \'prbs_4_genL\' (48-63)",
    "FE_MAC_BER_CNTRS_4_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_BER_VALID_4_FIELD,
    FE_MAC_BER_CNTRS_4_REG,
    FE_MAC_MAC_BLOCK,
    FE_MAC_BER_CNTRS_GROUP,
    0,
    BER_VALID_MASK,
    BER_VALID_SHIFT,
    "FE_BER_VALID_4_FIELD",
    "Flag. Bit corresponding to link N is set to non-zero value when PRBS\r\n"
    "  errors cross a threshold",
    "FE_MAC_BER_CNTRS_4_REG",
    "PRBS counter: Value and valid indicator",
    "FE_MAC_BER_CNTRS_GROUP",
    "BER counters on all specified links (four out of 0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_CLEAR,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_mac_ber_cntrs_group_str,
    HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
    0,0
  },
  {
    FE_PD_TBG_FIELD,
    FE_SERDES_FRST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    FE200_PD_TBG_MASK,
    FE200_PD_TBG_SHIFT,
    "FE_PD_TBG_FIELD",
    "Control. If set rhen TBG and all biasing circuits are powered down",
    "FE_SERDES_FRST_REG",
    "Control serdes on link 0 and also control the full serdes group",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_pd_tbg_field_vals,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_PD_FCT_FIELD,
    FE_SERDES_FRST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    PD_FCT_MASK,
    PD_FCT_SHIFT,
    "FE_PD_FCT_FIELD",
    "Control. If set rhen TX and RX blocks power down leaving TBG (faster recovery)",
    "FE_SERDES_FRST_REG",
    "Control serdes on link 0 and also control the full serdes group",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_pd_fct_field_vals,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    1,0
  },
  {
    FE_AMP_FIELD,
    FE_SERDES_FRST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    AMP_MASK,
    AMP_SHIFT,
    "FE_AMP_FIELD",
    "Control level of pre emphasis (with MEN, PEN). See documentation for equation.",
    "FE_SERDES_FRST_REG",
    "Control serdes on link 0 and also control the full serdes group",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    AMP_MASK >> AMP_SHIFT,0
  },
  {
    FE_MEN_FIELD,
    FE_SERDES_FRST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    MEN_MASK,
    MEN_SHIFT,
    "FE_MEN_FIELD",
    "Control level of pre emphasis (with AMP, PEN). See documentation for equation.",
    "FE_SERDES_FRST_REG",
    "Control serdes on link 0 and also control the full serdes group",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    MEN_MASK >> MEN_SHIFT,0
  },
  {
    FE_PEN_FIELD,
    FE_SERDES_FRST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    PEN_MASK,
    PEN_SHIFT,
    "FE_PEN_FIELD",
    "Control level of pre emphasis (with AMP, MEN). See documentation for equation.",
    "FE_SERDES_FRST_REG",
    "Control serdes on link 0 and also control the full serdes group",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PEN_MASK >> PEN_SHIFT,0
  },
  {
    FE_VCMS_FIELD,
    FE_SERDES_FRST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    VCMS_MASK,
    VCMS_SHIFT,
    "FE_VCMS_FIELD",
    "Select voltage common mode",
    "FE_SERDES_FRST_REG",
    "Control serdes on link 0 and also control the full serdes group",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_vcms_field_vals,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    VCMS_MASK >> VCMS_SHIFT,0
  },
  {
    FE_AMP_FIELD,
    FE_SERDES_REST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    AMP_MASK,
    AMP_SHIFT,
    "FE_AMP_FIELD",
    "Control level of pre emphasis (with MEN, PEN). See documentation for equation.",
    "FE_SERDES_REST_REG",
    "Control serdes on selected link (1-63)",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    AMP_MASK >> AMP_SHIFT,0
  },
  {
    FE_MEN_FIELD,
    FE_SERDES_REST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    MEN_MASK,
    MEN_SHIFT,
    "FE_MEN_FIELD",
    "Control level of pre emphasis (with AMP, PEN). See documentation for equation.",
    "FE_SERDES_REST_REG",
    "Control serdes on selected link (1-63)",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    MEN_MASK >> MEN_SHIFT,0
  },
  {
    FE_PEN_FIELD,
    FE_SERDES_REST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    PEN_MASK,
    PEN_SHIFT,
    "FE_PEN_FIELD",
    "Control level of pre emphasis (with AMP, MEN). See documentation for equation.",
    "FE_SERDES_REST_REG",
    "Control serdes on selected link (1-63)",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    PEN_MASK >> PEN_SHIFT,0
  },
  {
    FE_VCMS_FIELD,
    FE_SERDES_REST_REG,
    FE_SERDES_ECI_1_BLOCK,
    FE_SERDES_ENABLERS_GROUP,
    0,
    VCMS_MASK,
    VCMS_SHIFT,
    "FE_VCMS_FIELD",
    "Select voltage common mode",
    "FE_SERDES_REST_REG",
    "Control serdes on selected link (1-63)",
    "FE_SERDES_ENABLERS_GROUP",
    "Serdes control all links (0-63)",
    "FE_MAC_MAC_BLOCK",
    "MAC related registers in MAC of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_vcms_field_vals,
    (ELEMENT_OF_GROUP *)Fe_serdes_enablers_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    VCMS_MASK >> VCMS_SHIFT,0
  },
  {
    FE_LAST_TX_FIELD,
    FE_LAST_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_LAST_TX_MASK,
    SOC_SAND_LAST_TX_SHIFT,
    "FE_LAST_TX_FIELD",
    "Last three bytes on a \'tx source routed data cell\' buffer",
    "FE_LAST_TX_REG",
    "Last long on a \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_LAST_TX_MASK >> SOC_SAND_LAST_TX_SHIFT,0
  },
  {
    FE_PAYLOAD_TX_FIELD,
    FE_BODY_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    BODY_TX_MASK,
    BODY_TX_SHIFT,
    "FE_PAYLOAD_TX_FIELD",
    "Long element in body of \'tx source routed data cell\' buffer",
    "FE_BODY_TX_REG",
    "Long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    BODY_TX_MASK >> BODY_TX_SHIFT,0
  },
  {
    FE_FIRST_BYTE_TX_FIELD,
    FE_SWITCHES_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_SWITCHES_TX_FIRST_BYTE_MASK,
    SOC_SAND_SWITCHES_TX_FIRST_BYTE_SHIFT,
    "FE_FIRST_BYTE_TX_FIELD",
    "First byte in body of \'tx source routed data cell\' buffer",
    "FE_SWITCHES_TX_REG",
    "Second long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_SWITCHES_TX_FIRST_BYTE_MASK >> SOC_SAND_SWITCHES_TX_FIRST_BYTE_SHIFT,0
  },
  {
    FE_FE3_SW_FIELD,
    FE_SWITCHES_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_SWITCHES_TX_FE3SW_MASK,
    SOC_SAND_SWITCHES_TX_FE3SW_SHIFT,
    "FE_FE3_SW_FIELD",
    "Indication of which of the 32 FE3 switch will be used for TX path",
    "FE_SWITCHES_TX_REG",
    "Second long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_SWITCHES_TX_FE3SW_MASK >> SOC_SAND_SWITCHES_TX_FE3SW_SHIFT,0
  },
  {
    FE_FE2_SW_FIELD,
    FE_SWITCHES_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_SWITCHES_TX_FE2SW_MASK,
    SOC_SAND_SWITCHES_TX_FE2SW_SHIFT,
    "FE_FE2_SW_FIELD",
    "Indication of which of the 64 FE2 switch will be used for TX path",
    "FE_SWITCHES_TX_REG",
    "Second long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_SWITCHES_TX_FE2SW_MASK >> SOC_SAND_SWITCHES_TX_FE2SW_SHIFT,0
  },
  {
    FE_DEST_LVL_TX_FIELD,
    FE_PATHS_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_PATHS_TX_DEST_LVL_MASK,
    SOC_SAND_PATHS_TX_DEST_LVL_SHIFT,
    "FE_DEST_LVL_TX_FIELD",
    "Identifies the type of device the cell is transmitted to",
    "FE_PATHS_TX_REG",
    "First long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_src_lvl_vals,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_PATHS_TX_DEST_LVL_MASK >> SOC_SAND_PATHS_TX_DEST_LVL_SHIFT,0
  },
  {
    FE_SRC_LVL_TX_FIELD,
    FE_PATHS_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_PATHS_TX_SRC_LVL_MASK,
    SOC_SAND_PATHS_TX_SRC_LVL_SHIFT,
    "FE_SRC_LVL_TX_FIELD",
    "Identifies the type of device where the cell is generated",
    "FE_PATHS_TX_REG",
    "First long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)Fe_src_lvl_vals,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_PATHS_TX_SRC_LVL_MASK >> SOC_SAND_PATHS_TX_SRC_LVL_SHIFT,0
  },
  {
    FE_SRC_ID_TX_FIELD,
    FE_PATHS_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_PATHS_TX_SRC_ID_MASK,
    SOC_SAND_PATHS_TX_SRC_ID_SHIFT,
    "FE_SRC_ID_TX_FIELD",
    "Identifies (chip ID) the sender of the cell to transmit",
    "FE_PATHS_TX_REG",
    "First long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_PATHS_TX_SRC_ID_MASK >> SOC_SAND_PATHS_TX_SRC_ID_SHIFT,0
  },
  {
    FE_COLN_FIELD,
    FE_PATHS_TX_REG,
    FE_GEN_REGS_BLOCK,
    FE_TX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_PATHS_TX_COLN_MASK,
    SOC_SAND_PATHS_TX_COLN_SHIFT,
    "FE_COLN_FIELD",
    "Indicates which link number via which the cell is to be transmitted",
    "FE_PATHS_TX_REG",
    "First long data register on \'tx source routed data cell\' buffer",
    "FE_TX_SR_DATA_CELL_GROUP",
    "TX source routed data cell buffer",
    "FE_GEN_REGS_BLOCK",
    "General registers block of FE chip",
    DISPLAY_UNSIGNED_INT,
    ACCESS_READ | ACCESS_WRITE,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_tx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_PATHS_TX_COLN_MASK >> SOC_SAND_PATHS_TX_COLN_SHIFT,0
  },
  {
    FE_LAST_RX_FIELD,
    FE_LAST_RX_REG,
    FE_DCS_DCHA_BLOCK,
    FE_RX_SR_DATA_CELL_GROUP,
    0,
    LAST_RX_MASK,
    LAST_RX_SHIFT,
    "FE_LAST_RX_FIELD",
    "Last three bytes on a \'rx source routed data cell\' buffer",
    "FE_LAST_RX_REG",
    "Last long on a \'rx source routed data cell\' buffer",
    "FE_RX_SR_DATA_CELL_GROUP",
    "RX source routed data cell buffer",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_rx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    LAST_RX_MASK >> LAST_RX_SHIFT,0
  },
  {
    FE_PAYLOAD_RX_FIELD,
    FE_BODY_RX_REG,
    FE_DCS_DCHA_BLOCK,
    FE_RX_SR_DATA_CELL_GROUP,
    0,
    BODY_RX_MASK,
    BODY_RX_SHIFT,
    "FE_PAYLOAD_RX_FIELD",
    "Long element in body of \'rx source routed data cell\' buffer",
    "FE_BODY_RX_REG",
    "Long data register on \'rx source routed data cell\' buffer",
    "FE_RX_SR_DATA_CELL_GROUP",
    "RX source routed data cell buffer",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_rx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    BODY_RX_MASK >> BODY_RX_SHIFT,0
  },
  {
    FE_FIRST_BYTE_RX_FIELD,
    FE_SWITCHES_RX_REG,
    FE_DCS_DCHA_BLOCK,
    FE_RX_SR_DATA_CELL_GROUP,
    0,
    SWITCHES_RX_FIRST_BYTE_MASK,
    SWITCHES_RX_FIRST_BYTE_SHIFT,
    "FE_FIRST_BYTE_RX_FIELD",
    "First byte in body of \'rx source routed data cell\' buffer",
    "FE_SWITCHES_RX_REG",
    "Second long data register on \'rx source routed data cell\' buffer",
    "FE_RX_SR_DATA_CELL_GROUP",
    "RX source routed data cell buffer",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_rx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SWITCHES_RX_FIRST_BYTE_MASK >> SWITCHES_RX_FIRST_BYTE_SHIFT,0
  },
  {
    FE_SRC_LVL_RX_FIELD,
    FE_PATHS_RX_REG,
    FE_DCS_DCHA_BLOCK,
    FE_RX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_PATHS_RX_SRC_LVL_MASK,
    SOC_SAND_PATHS_RX_SRC_LVL_SHIFT,
    "FE_SRC_LVL_RX_FIELD",
    "Identifies the type of device where the cell is generated",
    "FE_PATHS_RX_REG",
    "First long data register on \'rx source routed data cell\' buffer",
    "FE_RX_SR_DATA_CELL_GROUP",
    "RX source routed data cell buffer",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_HEX,
    ACCESS_READ,
    (VAL_TO_TEXT *)Fe_src_lvl_vals,
    (ELEMENT_OF_GROUP *)Fe_rx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_PATHS_RX_SRC_LVL_MASK >> SOC_SAND_PATHS_RX_SRC_LVL_SHIFT,0
  },
  {
    FE_SRC_ID_RX_FIELD,
    FE_PATHS_RX_REG,
    FE_DCS_DCHA_BLOCK,
    FE_RX_SR_DATA_CELL_GROUP,
    0,
    SOC_SAND_PATHS_RX_SRC_ID_MASK,
    SOC_SAND_PATHS_RX_SRC_ID_SHIFT,
    "FE_SRC_ID_RX_FIELD",
    "Identifies (chip ID) the sender of the cell to transmit",
    "FE_PATHS_RX_REG",
    "First long data register on \'rx source routed data cell\' buffer",
    "FE_RX_SR_DATA_CELL_GROUP",
    "RX source routed data cell buffer",
    "FE_DCS_DCHA_BLOCK",
    "DCHa related registers in DCS of FE chip",
    DISPLAY_HEX | DISPLAY_UNSIGNED_INT,
    ACCESS_READ,
    (VAL_TO_TEXT *)0,
    (ELEMENT_OF_GROUP *)Fe_rx_sr_data_cell_group_str,
    HAS_MIN_VALUE | HAS_MAX_VALUE,
    SOC_SAND_PATHS_RX_SRC_ID_MASK >> SOC_SAND_PATHS_RX_SRC_ID_SHIFT,0
  },
/*
 * Last element. Do not remove.
 */
  {
    FE_LAST_FIELD
  }
}
#endif
;
/*
 * }
 */

#endif /*} LINUX */

/********************************************************
*NAME
*  Help_params
*TYPE: BUFFER
*DATE: 26/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "HELP".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      This field is automatically filled in by the system
*      and does not need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
   Help_params[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  History_len_vals
*TYPE: BUFFER
*DATE: 26/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LEN" ON A LINE
*  WITH SUBJECT 'HISTORY'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     History_len_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE,
        0,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "all",
    {
      {
        ALL_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Display the whole \'history\' buffer.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  History_len_default
*TYPE: BUFFER
*DATE: 18/FEB/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "LEN" ON A
*  LINE WITH SUBJECT 'HISTORY'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     History_len_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  ALL_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"all"
  }
}
#endif
;
/********************************************************
*NAME
*  History_get_vals
*TYPE: BUFFER
*DATE: 19/FEB/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "GET" ON A LINE
*  WITH SUBJECT 'HISTORY'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     History_get_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        NUM_HISTORY_ELEMENTS,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  History_get_default
*TYPE: BUFFER
*DATE: 19/FEB/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "GET" ON A
*  LINE WITH SUBJECT 'HISTORY'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*   int numeric_equivalent ;
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     History_get_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
*NAME
*  History_params
*TYPE: BUFFER
*DATE: 24/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "HISTORY".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      This field is automatically filled in by the system
*      and does not need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     History_params[]
#ifdef INIT
   =
{
  {
    PARAM_HIST_LEN_ID,
    "len",
    (PARAM_VAL_RULES *)&History_len_vals[0],
    (sizeof(History_len_vals) / sizeof(History_len_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&History_len_default,
    0,0,0,0,0,0,0,
    "Display indicated number of previously entered command lines.\r\n",
    "Examples:\r\n"
    "  History len 10\r\n"
    "Results in displaying the last 10 command lines.\r\n"
    "  History len all\r\n"
    "Results in displaying all command lines in history buffer.\r\n",
    "",
    {BIT(0)},
    0
  },
  {
    PARAM_HIST_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&History_get_vals[0],
    (sizeof(History_get_vals) / sizeof(History_get_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&History_get_default,
    0,0,0,0,0,0,0,
    "Get (on current command line) indicated, previously entered, command line.\r\n",
    "Examples:\r\n"
    "  History get 1\r\n"
    "Results in getting, ready for execution or editting, the last command lines.\r\n"
    "  History get 2\r\n"
    "Results in getting the command line before the last.\r\n",
    "",
    {BIT(1)},
    0
  },
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Indefinite_num_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Indefinite_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_rate_collection_rate_vals
*TYPE: BUFFER
*DATE: 27/AUG/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO 'COLLECTION_RATE' PARAMETER
*  WITHIN 'GENERAL FPGA_RATE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_rate_collection_rate_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        20000,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

#if LINK_PSS_LIBRARIES
/* { */

/********************************************************
*NAME
*  Fpga_rate_report_rate_vals
*TYPE: BUFFER
*DATE: 27/AUG/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO 'REPORT_RATE' PARAMETER
*  WITHIN 'GENERAL FPGA_RATE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_rate_report_rate_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        MAX_REPORT_RATE,MIN_REPORT_RATE,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif

#endif /* } LINK_PSS_LIBRARIES */
;
/********************************************************
*NAME
*  Dcl_file_name_vals
*TYPE: BUFFER
*DATE: 10/SEP/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO 'FILE NAME' PARAMETER (FIRST_FILE_NAME,
*  LAST_FILE_NAME) WITHIN 'GENERAL DCL HOST_FILE SET'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dcl_file_name_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        (unsigned long)((unsigned short)(-1)),0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_rate_host_ip_vals
*TYPE: BUFFER
*DATE: 27/AUG/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "HOST_IP" IN THE CONTEXT OF
*  SUBJECT 'GENERAL FPGA_RATE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_rate_host_ip_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Dcl_host_ip_vals
*TYPE: BUFFER
*DATE: 27/AUG/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "HOST_IP" and "dest_host" IN THE
*  CONTEXT OF SUBJECT 'GENERAL DCL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dcl_host_ip_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Dpss_unit_num_vals
*TYPE: BUFFER
*DATE: 10/JUL/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO AN INDEFINITE SINGLE NUMBER. FOR EXAMPLE,
*  THE PARAMETER "PRINT_ERRNO" WITHIN 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dpss_unit_num_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        3,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Mul_write_vals
*TYPE: BUFFER
*DATE: 05/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO A FEW NUMERALS OF UNLIMITED RANGE.
*  THIS IS USED FOR THE LIST OF ADDRESSES IN 'MUL_WRITE'
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Mul_write_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        MAX_REPEATED_PARAM_VAL,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Operation_mem_vals
*TYPE: BUFFER
*DATE: 06/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "OPERATION" IN THE
*  CONTEXT OF SUBJECT 'MEM' (AND 'VAL_READ').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Operation_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "equal",
    {
      {
        EQUAL_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Mask read memory contents and compare until specified value\r\n"
              "    is EQUAL to memory contents (or timeout has expired).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "not_equal",
    {
      {
        NOT_EQUAL_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Mask read memory contents and compare until specified value\r\n"
              "    is NOT EQUAL to memory contents (or timeout has expired).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Operation_mem_default
*TYPE: BUFFER
*DATE: 06/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "operation" IN THE CONTEXT OF SUBJECT 'MEM'
*  (AND 'VAL_READ').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Operation_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  EQUAL_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"equal"
  }
}
#endif
;
/********************************************************
*NAME
*  Timeout_mem_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TIMEOUT" WITHIN 'VAL_READ'
*  IN SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Timeout_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Format_mem_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FORMAT" IN THE CONTEXT
*  OF 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Format_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "long",
    {
      {
        LONG_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Four-bytes (32 bits) variable access.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "short",
    {
      {
        SHORT_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Two-bytes (16 bits) variable access.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "char",
    {
      {
        CHAR_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    One byte (8 bits) variable access.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Counters_vals
*TYPE: BUFFER
*DATE: 09/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "GET" OR "CLEAR" RELATED TO
*  "COUNTERS" IN THE CONTEXT OF 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Counters_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "val_read_failures",
    {
      {
        VAL_READ_FAILURES_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Counter of the number of \'read-compare\' failures (via \'val_read\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Format_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "FORMAT" IN THE CONTEXT
*  OF 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Format_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  LONG_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"long"
  }
}
#endif
;
/********************************************************
*NAME
*  Disp_mem_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DISP" IN THE CONTEXT
*  OF 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Disp_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "hex",
    {
      {
        HEX_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Display using hexadecimal digits",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "unsigned_integer",
    {
      {
        UNSIGNED_INTEGER_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Display unsigned numbers using decimal digits",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "signed_integer",
    {
      {
        SIGNED_INTEGER_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Display signed numbers using sign and decimal digits",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Exercises_vals
*TYPE: BUFFER
*DATE: 19/JUNE/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "Exercises" IN THE CONTEXT
*  OF 'GENERAL' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Exercises_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL_LAST,
    "event_aging",
    {
      {
        EVENT_AGING_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Test: Create two consecutive events. (will result in event aging)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "event_double",
    {
      {
        EVENT_DOUBLE_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"    Test utility: Inflict two events with same low 11 bits. This will result\r\n"
                "    in three events printed and stored NVRAM log",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "event_storm",
    {
      {
        EVENT_STORM_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"    Test utility: Inflict event storm. (will result in event counting)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "fatal_error",
    {
      {
        FATAL_ERROR_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"    Test utility: Inflict fatal-error event. (will result in reset)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "err_within_err",
    {
      {
        ERR_WITHIN_ERR_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"    Test utility: Inflict error within error. (will result in reboot)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "alarm_on",
    {
      {
        EVENT_ALARM_ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"    Test utility: Inflict alarm event. (will result in turning\r\n"
                "    red alarm led on)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "alarm_off",
    {
      {
        EVENT_ALARM_OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"    Test utility: Inflict alarm event. (eventually result in\r\n"
                "    turning red led off)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "print_task_on",
    {
      {
        EVENT_PRINT_TASK_ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Start continuous print task. (will result in long\r\n"
                "   printing every 10 seconds)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "print_task_off",
    {
      {
        EVENT_PRINT_TASK_OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Stop continuous print task.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "load_real_time_trace",
    {
      {
        LOAD_REAL_TIME_TRACE_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Load real-time-trace fifo with synthetic trace data.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "call_main_dpss_interrupt",
    {
      {
        CALL_MAIN_DPSS_INTERRUPT_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Call interruptMainSR() as if main interrupt were"
                "   injected.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "cause_software_emulation_exception",
    {
      {
        CAUSE_SOFTWARE_EMULATION_EXCEPTION_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Cause the system to create software emulation"
                "   exception.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "call_software_emulation_exception_handler",
    {
      {
        CALL_SOFTWARE_EMULATION_EXCEPTION_HANDLER_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Call soft_emu_exp_handler(), \n\r"
                "   as software emulation interrupt occurred.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "call_alignment_exception_handler",
    {
      {
        CALL_ALIGNMENT_EXCEPTION_HANDLER_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Call alignment_exception_handler(), \n\r"
                "   as alignment interrupt occurred.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "call_char_buffer_module_test",
    {
      {
        CALL_CHAR_BUFF_MODULE_TEST,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Call char_buff_test(), \n\r"
                "   To test the char buffer module.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "initialize_fpga_collection",
    {
      {
        INIT_FPGA_REAL_TIME_COLLETION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Initialize FPGA real time data collection package",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "call_fpga_collection_isr",
    {
      {
        CALL_FPGA_COLLETION_ISR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Call ISR related to data collection of all FPGAs",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "start_fpga_a_collection_process",
    {
      {
        START_FPGA_A_COLLETION_PROCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Start collection process for FPGA A (spawn task,\n\r"
                "   enable IRQ, ....",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "start_fpga_b_collection_process",
    {
      {
        START_FPGA_B_COLLETION_PROCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Start collection process for FPGA B (spawn task,\n\r"
                "   enable IRQ, ....",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "stop_fpga_a_collection_process",
    {
      {
        STOP_FPGA_A_COLLETION_PROCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Stop collection process for FPGA A (delete task,\n\r"
                "   disable IRQ, ....",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "stop_fpga_b_collection_process",
    {
      {
        STOP_FPGA_B_COLLETION_PROCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Stop collection process for FPGA B (delete task,\n\r"
                "   disable IRQ, ....",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "start_fpga_full_col_scenario",
    {
      {
        STOP_FPGA_FULL_COL_SCENARIO,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"   Test utility: Start, wait, stop collection process. This is a full\n\r"
                "   scenario running for 1 second with collection rate of 200 ms",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Temperature_vals
*TYPE: BUFFER
*DATE: 02/JUNE/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "WATCHDOG" IN THE CONTEXT
*  OF 'GENERAL' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Temperature_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "mezzanine_board_sensor",
    {
      {
        0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Show temperature on mezzanine board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_board_sensor_01",
    {
      {
        1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Show temperature on the first sensor of the host board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_board_sensor_02",
    {
      {
        2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Show temperature on the second sensor of the host board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "all_sensors",
    {
      {
        3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Show temperature on all available sensors (can't do periodic display)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  watchdog_vals
*TYPE: BUFFER
*DATE: 02/JUNE/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "WATCHDOG" IN THE CONTEXT
*  OF 'GENERAL' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Watchdog_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "stop_reset",
    {
      {
        STOP_RESET_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Test utility: Stop resetting watchdog. (will result in NMI)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "invoke_handler",
    {
      {
        INVOKE_HANDLER_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Test utility: Invoke NMI handler (watchdog expiry ISR)",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Disp_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DISP" IN THE CONTEXT
*  OF 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Disp_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  HEX_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"hex"
  }
}
#endif
;
/********************************************************
*NAME
*  App_source_file_nvram_vals
*TYPE: BUFFER
*DATE: 29/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "APP_SOURCE_FILE" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     App_source_file_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "host",
    {
      {
        HOST_SOURCE_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The version of the file on the downloading host is checked.\r\n"
    "    The value of \'app. load method\'(application_load_method) and the\r\n"
    "    comparison between the versions at the flash and at the host    \r\n"
        "    will determine the application file to be loaded.",

          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE,
        0,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)app_source_checker
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Front_board_serial_num_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FRONT_BOARD_SERIAL_NUM" IN THE
*  CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Front_board_serial_num_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        9999,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Host_db_board_serial_num_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "HOST_DB_BOARD_SERIAL_NUM" IN THE
*  CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_db_board_serial_num_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        9999,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Board_serial_num_nvram_vals
*TYPE: BUFFER
*DATE: 18/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BOARD_SERIAL_NUMBER" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Board_serial_num_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        9999,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Board_fap_pp_if_clk_nvram_vals
*TYPE: BUFFER
*DATE: 18/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP-PP INTERFACE CLOCK RATE" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Board_fap_pp_if_clk_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

#if !(defined(LINUX) || defined(UNIX))
/* { */


/********************************************************
*NAME
*  Board_host_front_line_card_nvram_vals
*TYPE: BUFFER
*DATE: 18/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP-PP INTERFACE CLOCK RATE" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Front_end_host_board_type_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "none",
    {
      {
        HOST_EEPROM_FRONT_CARD_TYPE_NONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The GFA is connected to no Front card, or to Dune's front card.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "x10",
    {
      {
        HOST_EEPROM_FRONT_CARD_TYPE_X10,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The GFA is connected to front line card: X10.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ixp2800",
    {
      {
        HOST_EEPROM_FRONT_CARD_TYPE_IXP2800,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The GFA is connected to front line card: IXP2800.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Board_fap_use_eeprom_vals_nvram_vals
*TYPE: BUFFER
*DATE: 18/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP USE EEPROM" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Board_fap_use_eeprom_vals_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Board_version_nvram_vals
*TYPE: BUFFER
*DATE: 18/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BOARD_HW_VERSION" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Board_version_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        999,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_board_version_nvram_vals
*TYPE: BUFFER
*DATE: 21/JAN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BOARD_HW_VERSION" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_board_version_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Description of hardware of board. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Front_board_version_nvram_vals
*TYPE: BUFFER
*DATE: 21/JAN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FRONT_BOARD_VERSION"
*  IN THE CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Front_board_version_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_FRONT_MAX_DEFAULT_BOARD_VERSION_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Description of hardware of front board. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_db_board_version_nvram_vals
*TYPE: BUFFER
*DATE: 21/JAN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "HOST_DB_BOARD_VERSION"
*  IN THE CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_db_board_version_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_HOST_DB_MAX_DEFAULT_BOARD_VERSION_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Description of hardware of host_db board. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Fe_configuration_nvram_vals
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "CONFIGURATION_REGISTER" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fe_configuration_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        (unsigned long)0xFFFF,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Page_lines_nvram_vals
*TYPE: BUFFER
*DATE: 23/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PAGE_LINES" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Page_lines_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        50,10,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Watchdog_period_nvram_vals
*TYPE: BUFFER
*DATE: 29/NAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "WATCHDOG_PERIOD" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Watchdog_period_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        INTERNAL_WATCHDOG_MASK,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Tlnt_timeout_std_nvram_vals
*TYPE: BUFFER
*DATE: 08/JULY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "TELNET_TIMEOUT_STANDARD" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tlnt_timeout_std_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "none",
    {
      {
        TELNET_INACTIVITY_NONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No inactivity timeout at all (infinite).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        240,4,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Tlnt_timeout_cont_nvram_vals
*TYPE: BUFFER
*DATE: 08/JULY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "TELNET_TIMEOUT_PERIODIC" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Pll_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        0xFFFFF,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Tlnt_timeout_cont_nvram_vals
*TYPE: BUFFER
*DATE: 08/JULY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "TELNET_TIMEOUT_PERIODIC" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tlnt_timeout_cont_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "none",
    {
      {
        TELNET_INACTIVITY_NONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No inactivity timeout at all (infinite).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        240,4,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Event_disp_level_nvram_vals
*TYPE: BUFFER
*DATE: 08/JULY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "EVENT_DISPLAY_LEVEL" IN
*  THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Event_disp_level_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "message",
    {
      {
        SVR_MSG,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Message\' severity level (lowest).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "warning",
    {
      {
        SVR_WRN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Warning\' severity level.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "error",
    {
      {
        SVR_ERR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Error\' severity level.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fatal",
    {
      {
        SVR_FTL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Fatal\' severity level (highest).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        SVR_FTL,SVR_MSG,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Event_nvram_level_nvram_vals
*TYPE: BUFFER
*DATE: 08/JULY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "EVENT_NVRAM_LEVEL" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Event_nvram_level_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "message",
    {
      {
        SVR_MSG,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Message\' severity level (lowest).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "warning",
    {
      {
        SVR_WRN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Warning\' severity level.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "error",
    {
      {
        SVR_ERR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Error\' severity level.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fatal",
    {
      {
        SVR_FTL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Fatal\' severity level (highest).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        SVR_FTL,SVR_MSG,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Event_syslog_level_nvram_vals
*TYPE: BUFFER
*DATE: 08/JULY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES RELATED
*  TO THE PARAMETER "EVENT_SYSLOG_LEVEL" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Event_syslog_level_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "message",
    {
      {
        SVR_MSG,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Message\' severity level (lowest).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "warning",
    {
      {
        SVR_WRN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Warning\' severity level.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "error",
    {
      {
        SVR_ERR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Error\' severity level.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fatal",
    {
      {
        SVR_FTL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    \'Fatal\' severity level (highest).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "none",
    {
      {
        SVR_LAST,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No events will be sent to a syslog server.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        SVR_FTL,SVR_MSG,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Snmp_cmd_line_nvram_vals
*TYPE: BUFFER
*DATE: 31/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_FILE_NAME" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Snmp_cmd_line_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        SNMP_CMD_LINE_SIZE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Command line arguments to pass to the SNMP agent.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  General_generic_any_number
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "param1" IN THE
*  CONTEXT OF SUBJECT 'general' (AND 'USER_TX_TABLE').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     General_generic_any_number[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
EXTERN CONST
   PARAM_VAL_RULES
     General_generic_chip_id[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        2,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  General_generic_params_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "generic_test"  IN THE CONTEXT
*  OF SUBJECT 'general'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     General_generic_params_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;

;




/********************************************************
*NAME
*  Fpga_rate_continuous_mode_vals
*TYPE: BUFFER
*DATE: 27/AUGUST/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "CONTINUOUS_MODE" IN THE
*  CONTEXT OF SUBJECT 'GENERAL FPGA_RATE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_rate_continuous_mode_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate continuous reporting (to controlling host) of FPGA rate\r\n"
              "    collection results.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    NO continuous reporting of FPGA rate collection results. Host is expected\r\n"
              "    to continuously poll.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_rate_continuous_mode_default
*TYPE: BUFFER
*DATE: 27/AUG/2004
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "CONTINUOUS_MODE" IN THE CONTEXT
*  OF 'GENERAL FPGA_RATE' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to (identifier of)
*      'string_value' in 'value' below.. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fpga_rate_continuous_mode_default
#ifdef INIT
   =
{
  VAL_SYMBOL_LAST,
  OFF_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"off"
  }
}
#endif
;

















/********************************************************
*NAME
*  Snmp_load_netsnmp_nvram_vals
*TYPE: BUFFER
*DATE: 31/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_MIBS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Snmp_load_netsnmp_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load the Net-SNMP package at startup.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Do NOT load the Net-SNMP package at startup.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Snmp_Download_mibs_nvram_vals
*TYPE: BUFFER
*DATE: 31/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_MIBS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Snmp_Download_mibs_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Download MIBs at startup from downloading host.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Do NOT download MIBs at startup from downloading host.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  base_and_option_register_nvram_vals
*TYPE: BUFFER
*DATE: 22/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETERS "BASE_REGISTER_04",
*  "BASE_REGISTER_05", "OPTION_REGISTER_04" and 05
*  IN THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     base_and_option_register_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        0,0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Eth_addr_nvram_vals
*TYPE: BUFFER
*DATE: 29/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "ETHERNET_ADDRESS" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Eth_addr_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        DUNE_MASK,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Tcp_timeout_nvram_vals
*TYPE: BUFFER
*DATE: 29/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TCP_TIMEOUT" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tcp_timeout_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        50,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Tcp_retries_nvram_vals
*TYPE: BUFFER
*DATE: 29/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TCP_RETRIES" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tcp_retries_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        20,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Board_description_nvram_vals
*TYPE: BUFFER
*DATE: 21/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BOARD_DESCRIPTION" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Board_description_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_MAX_BOARD_DESCRIPTION_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Description of hardware of board. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_board_description_nvram_vals
*TYPE: BUFFER
*DATE: 21/JAN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BOARD_DESCRIPTION" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_board_description_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_HOST_MAX_BOARD_DESCRIPTION_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Description of hardware of board. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Front_board_description_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FRONT_BOARD_DESCRIPTION"
*  IN THE CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Front_board_description_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_FRONT_MAX_BOARD_DESCRIPTION_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Description of hardware of front board. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_db_board_description_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "HOST_DB_BOARD_DESCRIPTION"
*  IN THE CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_db_board_description_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Description of hardware of host_db board. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Board_type_nvram_vals
*TYPE: BUFFER
*DATE: 19/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BOARD_TYPE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Board_type_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_MAX_BOARD_NAME_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Hardware board type. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_board_cat_num_nvram_vals
*TYPE: BUFFER
*DATE: 21/JAN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BOARD_TYPE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_board_cat_num_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_HOST_MAX_CATALOG_NUMBER_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Hardware board type. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Front_board_cat_num_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FRONT_BOARD_CATALOG_NUMBER"
*  IN THE CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Front_board_cat_num_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_FRONT_MAX_CATALOG_NUMBER_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Hardware front board type. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_db_board_cat_num_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "HOST_DB_BOARD_CATALOG_NUMBER"
*  IN THE CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_db_board_cat_num_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        B_HOST_DB_MAX_CATALOG_NUMBER_LEN - 1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Hardware host_db board type. Text display.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Dld_file_name_nvram_vals
*TYPE: BUFFER
*DATE: 31/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOAD_FILE_NAME" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dld_file_name_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DLD_FILE_NAME_LEN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Name of application file on downloading host.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Cli_file_name_vals
*TYPE: BUFFER
*DATE: 04/MAR/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FILE_NAME" IN THE
*  CONTEXT OF SUBJECT 'GENERAL CLI'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Cli_file_name_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        CLI_FILE_NAME_LEN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Name of cli file on downloading host AND on this system.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Cli_download_host_ip
*TYPE: BUFFER
*DATE: 1/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PARAM_GENERAL_CLI_DOWNLOAD_HOST_IP_ID" IN THE
*  CONTEXT OF SUBJECT 'GENERAL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Cli_download_host_ip[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Dld_host_address_nvram_vals
*TYPE: BUFFER
*DATE: 1/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DOWNLOADING_HOST_ADDRESS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dld_host_address_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Gw_address_nvram_vals
*TYPE: BUFFER
*DATE: 1/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DEFAULT_GATEWAY_ADDRESS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Gw_address_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Ip_mask_nvram_vals
*TYPE: BUFFER
*DATE: 1/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "IP_MASK" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Ip_mask_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        1,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Ip_address_nvram_vals
*TYPE: BUFFER
*DATE: 1/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "IP_ADDRESS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Ip_address_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Console_baud_nvram_vals
*TYPE: BUFFER
*DATE: 31/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "CONSOLE_BAUD_RATE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Console_baud_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "b_4800",
    {
      {
        B_4800_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Communication speed of 4800 bits per second with local trerminal.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "b_9600",
    {
      {
        B_9600_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Communication speed of 9600 bits per second with local trerminal.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "b_38400",
    {
      {
        B_38400_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Communication speed of 38400 bits per second with local trerminal.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "b_19200",
    {
      {
        B_19200_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Communication speed of 19200 bits per second with local trerminal.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Block_nvram_vals
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BLOCK" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Block_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "1",
    {
      {
        BLOCK_1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 1. Resides on parallel eeprom. Used in boot process. Variables\r\n"
        "    which are set once, at initialization and setup of the board, and not\r\n"
        "    touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "2",
    {
      {
        BLOCK_2_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 2. Resides on parallel eeprom. Used in boot process. Variables\r\n"
        "    which are occasionally updated to change the various parameters set at boot\r\n"
        "    time (e.g. IP addr.) and boot options (e.g name of appl. file to download).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "boot_parameters",
    {
      {
        BLOCK_2_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 2. Resides on parallel eeprom. Used in boot process. Variables\r\n"
        "    which are occasionally updated to change the various parameters set at boot\r\n"
        "    time (e.g. IP addr) and boot options (e.g. name of appl. file to download).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "3",
    {
      {
        BLOCK_100_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 3. Resides on serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "cpu_board_hw_parameters",
    {
      {
        BLOCK_100_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 3. Resides on serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "4",
    {
      {
        BLOCK_101_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 4. Resides on serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "cpu_board_hw_description",
    {
      {
        BLOCK_101_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 4. Resides on serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "5",
    {
      {
        BLOCK_3_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 5. Resides on parallel eeprom. Used for variables\r\n"
        "    related to the application (including DUNE chip set). Variables\r\n"
        "    are assumed to be changed occasionally.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "application_parameters",
    {
      {
        BLOCK_3_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 5. Resides on parallel eeprom. Used for variables\r\n"
        "    related to the application (including DUNE chip set). Variables\r\n"
        "    are assumed to be changed occasionally.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "6",
    {
      {
        BLOCK_4_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 6. Resides on parallel eeprom. Used for information\r\n"
        "    collected at reset/watchdog exception.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "watchdog_info",
    {
      {
        BLOCK_4_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 6. Resides on parallel eeprom. Used for information\r\n"
        "    collected at reset/watchdog exception.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "11",
    {
      {
        BLOCK_7_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 11. Resides on parallel eeprom. Used for information\r\n"
        "    collected at software exception.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "software_exception_info",
    {
      {
        BLOCK_7_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 11. Resides on parallel eeprom. Used for information\r\n"
        "    collected at software exception.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "7",
    {
      {
        BLOCK_5_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 7. Resides on parallel eeprom. Used to contain\r\n"
              "    event log \'file\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "event_log",
    {
      {
        BLOCK_5_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 7. Resides on parallel eeprom. Used to contain\r\n"
              "    event log \'file\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "8",
    {
      {
        BLOCK_6_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 8. Resides on parallel eeprom. Used for variables\r\n"
        "    related to the Net-SNMP package (SNMP agnet). Variables\r\n"
        "    are assumed to be changed occasionally.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "snmp_parameters",
    {
      {
        BLOCK_6_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 8. Resides on parallel eeprom. Used for variables\r\n"
        "    related to the Net-SNMP package (SNMP agnet). Variables\r\n"
        "    are assumed to be changed occasionally.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "9",
    {
      {
        BLOCK_200_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 9. Resides on host serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "10",
    {
      {
        BLOCK_201_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 10. Resides on host serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_sw_boot_params",
    {
      {
        BLOCK_200_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 9. Resides on host serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_hw_boot_params",
    {
      {
        BLOCK_201_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 10. Resides on host serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "12",
    {
      {
        BLOCK_300_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 13. Resides on front serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    front board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "13",
    {
      {
        BLOCK_301_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 13. Resides on front serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    front board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "front_sw_boot_params",
    {
      {
        BLOCK_300_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 12. Resides on front serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    front board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "front_hw_boot_params",
    {
      {
        BLOCK_301_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 13. Resides on front serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    front board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  /* HOST_DB { */
  {
    VAL_SYMBOL,
    "14",
    {
      {
        BLOCK_400_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 14. Resides on GFA DB serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    host_db board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "15",
    {
      {
        BLOCK_401_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 15. Resides on host_db serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    host_db board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_db_sw_boot_params",
    {
      {
        BLOCK_400_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 14. Resides on host_db serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    host_db board, and not touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_db_hw_boot_params",
    {
      {
        BLOCK_401_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Block 15. Resides on host_db serial eeprom. Used in boot process.\r\n"
        "    Variables which are set once, at initialization and setup of the\r\n"
        "    host_db board, and occasionally (but rarely) touched again.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  /* Host_db }*/

  /*
   * The follwing section allows entring block identifiers
   * as numeric valuse
   */
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        BLOCK_401_EQUIVALENT,BLOCK_1_EQUIVALENT,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_board_type_nvram_vals
*TYPE: BUFFER
*DATE: 21/JAN/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "BLOCK" IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_board_type_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "unknown_card",
    {
      {
        SOC_UNKNOWN_CARD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Default value\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "standalone_mezzanine",
    {
      {
        STANDALONE_MEZZANINE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Standalone card, that has no host\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "fe600_card_01",
    {
      {
        FABRIC_CARD_FE600_01,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)"    Host is a SOC_SAND_FE600 card \r\n",
          /*
          * Pointer to a function to call after symbolic value
          * has been accepted. Casting should be 'VAL_PROC_PTR'.
          * Casting to 'long' added here just to keep the stupid
          * compiler silent.
          */
          (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fabric_card_01",
    {
      {
        FABRIC_CARD_01,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a fabric card \r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_01",
    {
      {
        LINE_CARD_01,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a SOC_SAND_FAP10M line card \r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_fap10m_b",
    {
      {
        LINE_CARD_FAP10M_B,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a SOC_SAND_FAP10M B line card \r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa",
    {
      {
        LINE_CARD_GFA_FAP20V,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a SOC_SAND_FAP20V line card.\n\r"
              "    GFA Stands for Generic-Fabric-Adapter, and pronounced Gifa.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa_mb",
    {
      {
        LINE_CARD_GFA_MB_FAP20V,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a SOC_SAND_FAP20V line card.\n\r"
              "    GFA-MB Stands for Generic-Fabric-Adapter Mother Board.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa_mb_fap21v",
    {
      {
        LINE_CARD_GFA_MB_FAP21V,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a SOC_SAND_FAP21V line card.\n\r"
              "    GFA-MB Stands for Generic-Fabric-Adapter Mother Board.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa_FAP21V",
    {
      {
        LINE_CARD_GFA_FAP21V,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a SOC_SAND_FAP21V line card.\n\r"
              "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa_FAP80V_DDR3",
    {
      {
        LINE_CARD_GFA_PETRA_DDR3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a Soc_petra line card.\n\r"
              "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa_FAP80V_DDR2",
    {
      {
        LINE_CARD_GFA_PETRA_DDR2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a Soc_petra line card.\n\r"
              "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "line_card_gfa_FAP80V_DDR3_STR",
    {
      {
        LINE_CARD_GFA_PETRA_DDR3_STREAMING,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)"    Host is a Soc_petra line card.\n\r"
          "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
          "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
          * Pointer to a function to call after symbolic value
          * has been accepted. Casting should be 'VAL_PROC_PTR'.
          * Casting to 'long' added here just to keep the stupid
          * compiler silent.
          */
          (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa_FAP100B_DDR3",
    {
      {
        LINE_CARD_GFA_PETRA_B_DDR3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a Soc_petra B line card.\n\r"
              "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "line_card_gfa_FAP100B_DDR3_STR",
    {
      {
        LINE_CARD_GFA_PETRA_B_DDR3_STREAMING,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)"    Host is a Soc_petra B line card.\n\r"
          "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
          "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
          * Pointer to a function to call after symbolic value
          * has been accepted. Casting should be 'VAL_PROC_PTR'.
          * Casting to 'long' added here just to keep the stupid
          * compiler silent.
          */
          (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_gfa_FAP100A_DDR3",
    {
      {
        LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a Soc_petra B line card with Soc_petra A assembly.\n\r"
              "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
              "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "line_card_gfa_FAP100A_DDR3_STR",
    {
      {
        LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)"    Host is a Soc_petra B line card with Soc_petra A assembly.\n\r"
          "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n"
          "    Another CARD (with NP of some kind) can be attached to this card.\n\r",
          /*
          * Pointer to a function to call after symbolic value
          * has been accepted. Casting should be 'VAL_PROC_PTR'.
          * Casting to 'long' added here just to keep the stupid
          * compiler silent.
          */
          (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "line_card_gfa_BI",
    {
      {
        LINE_CARD_GFA_PETRA_B_INTERLAKEN,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)"    Host is a Soc_petra B Interlaken .\n\r"
          "    GFA Stands for Generic-Fabric-Adapter Mother Board.\r\n",
          /*
          * Pointer to a function to call after symbolic value
          * has been accepted. Casting should be 'VAL_PROC_PTR'.
          * Casting to 'long' added here just to keep the stupid
          * compiler silent.
          */
          (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "Soc_petraB_Load_Board",
    {
      {
        LOAD_BOARD_PB,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)"    Host is a Soc_petra B load board.\n\r",
          /*
          * Pointer to a function to call after symbolic value
          * has been accepted. Casting should be 'VAL_PROC_PTR'.
          * Casting to 'long' added here just to keep the stupid
          * compiler silent.
          */
          (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "FE600_Load_Board",
    {
      {
        LOAD_BOARD_FE600,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)"    Host is a SOC_SAND_FE600 load board.\n\r",
          /*
          * Pointer to a function to call after symbolic value
          * has been accepted. Casting should be 'VAL_PROC_PTR'.
          * Casting to 'long' added here just to keep the stupid
          * compiler silent.
          */
          (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "line_card_tevb",
    {
      {
        LINE_CARD_TEVB,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Host is a TEVB line card.\n\r"
              "    TEVB Stands for Timna Evaluation Board\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },

/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Front_board_type_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FRONT_BOARD_TYPE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
*  HOST_EEPROM_FRONT_CARD_TYPE
 */
EXTERN CONST
   PARAM_VAL_RULES
     Front_board_type_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "front_card_none_unknown",
    {
      {
        SOC_UNKNOWN_CARD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No front card of known type\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "front_card_type_ftg",
    {
      {
        FRONT_END_FTG,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Front card of type FTG\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "front_card_type_tgs",
    {
      {
        FRONT_END_TGS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Front card of type TGS\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "front_card_type_ptg",
    {
      {
        SOC_FRONT_END_PTG,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Front card of type PTG\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "front_card_type_tevb",
    {
      {
        FRONT_END_TEVB,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Front card of type TEVB\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_db_board_type_nvram_vals
*TYPE: BUFFER
*DATE: 07/DEC/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "HOST_DB_BOARD_TYPE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM BOARD_PARAMS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
*  HOST_EEPROM_HOST_DB_CARD_TYPE
 */
EXTERN CONST
   PARAM_VAL_RULES
     Host_db_board_type_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "host_db_none_unknown",
    {
      {
        SOC_UNKNOWN_CARD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No host_db card of known type\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_db_gddr1",
    {
      {
        FAP_MEZ_GDDR1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No host_db card of known type\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_db_fap21v",
    {
      {
        FAP_MEZ_FAP21V,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No host_db card of known type\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "host_db_ddr2",
    {
      {
        FAP_MEZ_DDR2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    No host_db card of known type\r\n",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },

/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Block_nvram_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "BLOCK"  IN THE CONTEXT
*  OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Block_nvram_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)4
  }
}
#endif
;
/********************************************************
*NAME
*  Pause_after_read_mem_read_vals
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "PAUSE_AFTER_READ"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Pause_after_read_mem_read_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000000,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Pause_after_read_mem_read_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "PAUSE_AFTER_READ"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Pause_after_read_mem_read_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;
/********************************************************
*NAME
*  Nof_times_mem_read_vals
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "NOF_TIME"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Nof_times_mem_read_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        10000,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Nof_times_mem_read_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "PAUSE_AFTER_READ"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Nof_times_mem_read_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
*NAME
*  L_defaults_nvram_vals
*TYPE: BUFFER
*DATE: 27/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LOAD_DEFAULTS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     L_defaults_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "block_2",
    {
      {
        DEFAULTS_BLOCK_2_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 2 by defaults values. Mostly setup parameters.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "boot_parameters",
    {
      {
        DEFAULTS_BLOCK_2_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 2 by defaults values. Mostly setup parameters.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "block_6",
    {
      {
        DEFAULTS_BLOCK_4_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 6 by defaults values. Reset/watchdog exception information.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "watchdog_info",
    {
      {
        DEFAULTS_BLOCK_4_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 6 by defaults values. Reset/watchdog exception information.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "block_7",
    {
      {
        DEFAULTS_BLOCK_5_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 7 by defaults values. Clear event log \'file\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "event_log",
    {
      {
        DEFAULTS_BLOCK_5_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 7 by defaults values. Clear event log \'file\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "block_5",
    {
      {
        DEFAULTS_BLOCK_3_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 5 by defaults values. Application parameters (including\r\n"
              "    DUNE chips).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "application_parameters",
    {
      {
        DEFAULTS_BLOCK_3_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 5 by defaults values. Application parameters (including\r\n"
              "    DUNE chips).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "block_4",
    {
      {
        DEFAULTS_BLOCK_101_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load block 4 by defaults values. Hardware marking variables.\r\n"
              "    Usually set once and for all.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "all",
    {
      {
        DEFAULTS_ALL_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Load all blocks by default values, except for block 1 of each device.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Mem_buff_offset_vals
*TYPE: BUFFER
*DATE: 12/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LEN" IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Mem_buff_offset_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        SIZEOF_BUFF_MEM -1 ,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Len_mem_vals
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LEN" IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Len_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Len_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "LEN"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Len_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
*NAME
*  Index_fe_gen_regs_vals
*TYPE: BUFFER
*DATE: 07/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "INDEX" IN THE
*  CONTEXT OF SUBJECT 'FE_BASIC' PLUS 'BULK'
*  PLUS "CONNECT_MAP".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Index_fe_gen_regs_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        ((SIZEOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.connectivity_reg_1) +
        SIZEOF(FE_REGS,fe_ccsb_ccs_regs.fe_ccsb_ccs_detail.connectivity_reg_2)) /
          SIZEOF(FE_REGS,fe_ccsa_ccs_regs.fe_ccsa_ccs_detail.connectivity_reg_1[0])) - 1,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST | VAL_INDEX_ALL,
    "all",
    {
      {
        ALL_INDEX_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Handle the whole \'group\', not just one item.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Index_fe_mac_rx_cntrs_vals
*TYPE: BUFFER
*DATE: 13/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "INDEX" IN THE
*  CONTEXT OF SUBJECT 'FE_BASIC' PLUS 'BULK'
*  PLUS "RX_CNTRS".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Index_fe_mac_rx_cntrs_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        ((SIZEOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_1) +
          SIZEOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_2) +
          SIZEOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_3) +
          SIZEOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_4)
          ) /
          SIZEOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_rx_cntrs_1[0])) - 1,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
  {
    VAL_SYMBOL_LAST | VAL_INDEX_ALL,
    "all",
    {
      {
        ALL_INDEX_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Handle the whole \'group\', not just one item.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;


/********************************************************
*NAME
*  Repeat_mem_vals
*TYPE: BUFFER
*DATE: 05/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "REPEAT" IN THE
*  CONTEXT OF SUBJECT 'MEM' PLUS 'WRITE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Repeat_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Repeat_mem_default
*TYPE: BUFFER
*DATE: 05/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "REPEAT" IN THE CONTEXT OF SUBJECT 'MEM'
*  PLUS 'WRITE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Repeat_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
*NAME
*  Select_fe_gen_regs_vals
*TYPE: BUFFER
*DATE: 03/JUNE/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "SELECT_FE" OF SUBJECT
*  'FE_BASIC'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  This parameter has no values. When it appears on the
*  line, a value of 'true' is implied.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Select_fe_gen_regs_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        NUM_FE_ON_BOARD,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Update_nvram_vals
*TYPE: BUFFER
*DATE: 26/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "WRITE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Update_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "run_time",
    {
      {
        RUN_TIME_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Runtime variable update (erased at reset).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "next_startup",
    {
      {
        NEXT_STARTUP_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Write variable in NVRAM so that it becomes effective after next reset.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "both",
    {
      {
        BOTH_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Write both Runtime variable and variable in NVRAM.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Update_nvram_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "UPDATE" IN THE CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Update_nvram_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  NEXT_STARTUP_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"next_startup"
  }
}
#endif
;
/********************************************************
*NAME
*  Data_cache_nvram_vals
*TYPE: BUFFER
*DATE: 26/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DATA_CACHE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Data_cache_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate data cache.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable data cache.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Background_collect_all_temperatures_enable_vals
*TYPE: BUFFER
*DATE: 26/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DATA_CACHE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Background_collect_all_temperatures_enable_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Enable background collection all temperatures.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable background collection all temperatures.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Inst_cache_nvram_vals
*TYPE: BUFFER
*DATE: 26/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "INSTRUCTION_CACHE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Inst_cache_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate instruction cache.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable instruction cache.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Auto_flash_nvram_vals
*TYPE: BUFFER
*DATE: 26/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "AUTO_FLASH_LOAD" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Auto_flash_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate automatic loading of Flash from Host at startup.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable automatic loading of Flash from Host at startup.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Ip_source_mode_nvram_vals
*TYPE: BUFFER
*DATE: 16/OCT/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "IP_SOURCE_MODE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Ip_source_mode_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "ip_from_nv",
    {
      {
        IP_FROM_NV,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Take the IP address from the NVRam.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ip_from_slot_id",
    {
      {
        IP_FROM_SLOT_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Take the IP address from 24 high bits from NRAM \r\n"
              "     and 8 lowwer bits are the slot Id.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ip_from_crate",
    {
      {
        IP_FROM_CRATE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    10.SCHEME_VER(4MSB)&CRATE_TYPE(4LSB).CRATE_ID.SLOT_ID",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Page_mode_nvram_vals
*TYPE: BUFFER
*DATE: 23/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PAGE_MODE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Page_mode_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate \'page\' mode: Stop long dumps to screen every \'page lines\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disable \'page\' mode: Print full dumps to screen without stopping.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Line_mode_nvram_vals
*TYPE: BUFFER
*DATE: 23/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LINE_MODE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Line_mode_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate \'line\' mode: Process input command line only\r\n"
              "    after \'carriage return\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Deactivate \'line\' mode: Process input command line after\r\n"
              "    each single character.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Activate_watchdog_nvram_vals
*TYPE: BUFFER
*DATE: 29/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "ACTIVATE_WATCHDOG" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Activate_watchdog_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate watchdog: Keep it running and reset periodically.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Deactivate watchdog: Stop timer and DO NOT reset periodically.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Activate_software_exception_nvram_vals
*TYPE: BUFFER
*DATE: 29/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "ACTIVATE_SOFTWARE_EXCEPTION" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Activate_software_exception_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Hook handler: On starup - hook handler.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Do no hook handler: Use default handler.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Activate_dpss_nvram_vals
*TYPE: BUFFER
*DATE: 19/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "ACTIVATE_DPSS" IN THE
*  CONTEXT OF SUBJECT 'NVRAM' (ANOTHER NAME TO PARAMETER
*  'ACTIVATE_MARVELLS_DPSS').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Activate_dpss_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        SOC_D_USR_APP_FLAVOR_NORMAL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate DPSS: Initialize and run at startup. Download config. file.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        SOC_D_USR_APP_FLAVOR_NONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Do not activate DPSS at startup.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Usr_app_flavor_nvram_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Usr_app_flavor_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "none",
    {
      {
        SOC_D_USR_APP_FLAVOR_NONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Do not activate DPSS at startup. Loads the boot, BSP and UI.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "normal",
    {
      {
        SOC_D_USR_APP_FLAVOR_NORMAL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Normal application. Loads boot, BSP, UI, FAP/NP application.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "minimal",
    {
      {
        SOC_D_USR_APP_FLAVOR_MINIMAL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Minimal application. Loads boot, BSP, UI and *minimal* FAP/NP application.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "server",
    {
      {
        SOC_D_USR_APP_FLAVOR_SERVER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Server Only Mode.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "server_and_normal",
    {
      {
       SOC_D_USR_APP_FLAVOR_NORMAL_AND_SERVER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Applicaiton and server.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },



  {
    VAL_SYMBOL,
    "server_with_init",
    {
      {
        SOC_D_USR_APP_FLAVOR_SERVER_WITH_INIT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    The local CPU will init the device, and then server.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "minimal_no_app",
    {
      {
        SOC_D_USR_APP_FLAVOR_MINIMAL_NO_APP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Minimal application. Loads boot, BSP, UI and *minimal* FAP/NP application. No App.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },

/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Conf_from_md_nvram_vals
*TYPE: BUFFER
*DATE: 23/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "CONF_FROM_MD" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Conf_from_md_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Read initial value for \'configuration\' register (on FE) from MD pins.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Read initial value for \'configuration\' register (on FE) from NVRAM.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Kernel_startup_nvram_vals
*TYPE: BUFFER
*DATE: 26/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "KERNEL_STARTUP" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Kernel_startup_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "straight_application",
    {
      {
        STRAIGHT_TO_APPLICATION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate DUNE application at startup.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "shell_then_appl_after_timeout",
    {
      {
        SHELL_THEN_APPLICATION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Activate shell at startup if user indicates so within timeout.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "shell_once_then_application",
    {
      {
        SHELL_ONCE | STRAIGHT_TO_APPLICATION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
       (long)"    Do shell at next startup but FOLLOWING startup do \'straight\r\n"
             "    application\'. Used for debuging when connected by Telnet.",
       /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "shell_once_then_shell_+_timeout",
    {
      {
        SHELL_ONCE | SHELL_THEN_APPLICATION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
          (long)"    Do shell at next startup but FOLLOWING startup do \'shell with timeout\'.\r\n"
                "    Used for debuging when connected by Telnet.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Test_at_startup_nvram_vals
*TYPE: BUFFER
*DATE: 28/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TEST_AT_STARTUP" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Test_at_startup_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        TEST_IRQ6 | TEST_NMI,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Dpss_display_level_nvram_vals
*TYPE: BUFFER
*DATE: 28/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DPSS_DISPLAY_LEVEL" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dpss_display_level_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        4,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Dpss_debug_level_nvram_vals
*TYPE: BUFFER
*DATE: 28/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DPSS_DEBUG_LEVEL" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dpss_debug_level_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        4,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Dpss_start_demo_mode_nvram_vals
*TYPE: BUFFER
*DATE: 06/AUG/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DPSS_DEBUG_LEVEL" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dpss_start_demo_mode_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "no_demo",
    {
      {
        0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Start in normal (not demo) mode.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "negev_demo",
    {
      {
        10,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Start in negev_demo mode.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Nvram_fap10m_run_mode_vals
*TYPE: BUFFER
*DATE: 06/AUG/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP10M_RUN_MODE" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Nvram_fap10m_run_mode_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "eci_access",
    {
      {
        FAP10M_INTERFACE_ECI,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All needed accesses are via the ECI (access to the I2C is done also).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "pci_access",
    {
      {
        FAP10M_INTERFACE_PCI,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All needed accesses are via the PCI (access to the I2C is done also).\n\r"
              "    HW calibrtion is done via the I2C\n\r"
              "    Afterwards its move the interface to the PCI.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "i2c_access",
    {
      {
        FAP10M_INTERFACE_I2C,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    All needed accesses are via the I2C."
              "    It is not make too much sense except debugging to use this mode.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Load_method_nvram_vals
*TYPE: BUFFER
*DATE: 26/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "APPLICATION_LOAD_METHOD" IN THE
*  CONTEXT OF SUBJECT 'NVRAM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Load_method_nvram_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "newest",
    {
      {
        METHOD_NEWEST_VERSION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    At startup, look for most up-to-date application version,\r\n"
              "    in Flash and in downloading host, and select it as the\r\n"
              "    module to run.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "file_then_newest",
    {
      {
        METHOD_FILE_THEN_NEWEST,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    At startup, look for indicated application source.\r\n"
              "    If it does not has a valid file, look for most up-to-date\r\n"
              "    application version, in Flash and in downloading host, and\r\n"
              "    select it as the module to run.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "flash_then_newest",
    {
      {
        METHOD_FLASH_THEN_NEWEST,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    At startup, look at ALL flash files. If there is no valid file,\r\n"
              "    look for file on downloading host, and select it, if it can be\r\n"
              "    found, as the module to run.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

#endif /*} LINUX */

/********************************************************
*NAME
*  Temperature_general_vals
*TYPE: BUFFER
*DATE: 10/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TEMPERATURE" OF SUBJECT
*  'GENERAL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Temperature_general_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Empty_vals
*TYPE: BUFFER
*DATE: 07/MAY/2002
*FUNCTION:
*  ARRAY CONTAINING EMPTY LIST OF VALUES
*  RELATED TO ANY PARAMETER OF ANY SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Empty_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Show_general_vals
*TYPE: BUFFER
*DATE: 10/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "READ" OF SUBJECT
*  'GENERAL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Show_general_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "regular",
    {
      {
        METHOD_REGULAR_DISPLAY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Display indicated variable once and return control to the user.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "period",
    {
      {
        METHOD_PERIODIC_DISPLAY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Display indicated variable continuously. User retains\r\n"
              "    control by typing CTRL-t or \'escape\'.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  App_file_general_vals
*TYPE: BUFFER
*DATE: 25/JUNE/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "APPLICATION_FILE" OF SUBJECT
*  'GENERAL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     App_file_general_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)app_file_id_checker
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_identifier_vals
*TYPE: BUFFER
*DATE: 09/JULY/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FPGA_INDIRECT" OF SUBJECT
*  'GENERAL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_identifier_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "fpga_a",
    {
      {
        FPGA_A_IDENTIFIER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Approach FPGA \'A\' for memory access.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "fpga_b",
    {
      {
        FPGA_B_IDENTIFIER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Approach FPGA \'B\' for memory access",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_ind_write_vals
*TYPE: BUFFER
*DATE: 09/JULY/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "WRITE" OF SUBJECT
*  'GENERAL FPGA_INDIRECT'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_ind_write_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "generator",
    {
      {
        GENERATOR_ACCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'generator\' memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "verifier",
    {
      {
        VALIDATOR_ACCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'verifier\' memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "flow_control_block",
    {
      {
        FLOW_CONTROL_ACCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'flow control\' memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_ind_sub_block_write_vals
*TYPE: BUFFER
*DATE: 20/JULY/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "SUB_BLOCK" OF SUBJECT
*  'GENERAL FPGA_INDIRECT WRITE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_ind_sub_block_write_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "calendar_table",
    {
      {
        SUB_BLOCK_CALENDAR_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'calendar_table\' generator memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "egress_stream_table",
    {
      {
        SUB_BLOCK_EGRESS_STREAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'egress_stream_table\' generator memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "egress_pattern_table",
    {
      {
        SUB_BLOCK_EGRESS_PATTERN_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'egress_pattern_table\' generator memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "channel_to_port",
    {
      {
        SUB_BLOCK_CHANNEL_TO_PORT_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'channel_to_port\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ingress_stream",
    {
      {
        SUB_BLOCK_INGRESS_STREAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'ingress_stream\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ingress_pattern",
    {
      {
        SUB_BLOCK_INGRESS_PATTERN_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'ingress_pattern\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "logical_multicast",
    {
      {
        SUB_BLOCK_LOGICAL_MULTICAST_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'logical_multicast\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ingress_multicast",
    {
      {
        SUB_BLOCK_INGRESS_MULTICAST_STREAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'ingress_multicast\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "stream_selection",
    {
      {
        SUB_BLOCK_STREAM_SELECTION_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'stream_selection\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "stream_rate",
    {
      {
        SUB_BLOCK_STREAM_RATE_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'stream_rate\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "port_rate",
    {
      {
        SUB_BLOCK_PORT_RATE_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'port_rate\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "m_delay",
    {
      {
        SUB_BLOCK_M_DELAY_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'m_delay\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "last_delay",
    {
      {
        SUB_BLOCK_LAST_DELAY_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'last_delay\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "delay_histogram",
    {
      {
        SUB_BLOCK_DELAY_HISTOGRAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'delay_histogram\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "last_timestamp",
    {
      {
        SUB_BLOCK_LAST_TIMESTAMP_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'last_timestamp\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "inter_packet_delay",
    {
      {
        SUB_BLOCK_INTER_PACKET_DELAY_HISTOGRAM,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'inter_packet_delay\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "jitter_histogram",
    {
      {
        SUB_BLOCK_JITTER_HISTOGRAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'jitter_histogram\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "port_statistics",
    {
      {
        SUB_BLOCK_PORT_STATISTICS_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'port_statistics\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "port_statistics",
    {
      {
        SUB_BLOCK_PORT_STATISTICS_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'port_statistics\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "channel_weight",
    {
      {
        SUB_BLOCK_CHANNEL_WEIGHT_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'channel_weight\' flow_control memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "channel_fifo_size",
    {
      {
        SUB_BLOCK_CHANNEL_FIFO_SIZE_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'channel_fifo_size\' flow_control memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "spill_over",
    {
      {
        SUB_BLOCK_SPILL_OVER_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'spill_over\' flow_control memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_ind_sub_block_read_vals
*TYPE: BUFFER
*DATE: 09/JULY/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "SUB_BLOCK" OF SUBJECT
*  'GENERAL FPGA_INDIRECT READ'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_ind_sub_block_read_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "calendar_table",
    {
      {
        SUB_BLOCK_CALENDAR_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'calendar_table\' generator memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "egress_stream_table",
    {
      {
        SUB_BLOCK_EGRESS_STREAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'egress_stream_table\' generator memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "egress_pattern_table",
    {
      {
        SUB_BLOCK_EGRESS_PATTERN_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'egress_pattern_table\' generator memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "log_memory",
    {
      {
        SUB_BLOCK_LOG_MEMORY_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'log_memory\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "channel_to_port",
    {
      {
        SUB_BLOCK_CHANNEL_TO_PORT_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'channel_to_port\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ingress_stream",
    {
      {
        SUB_BLOCK_INGRESS_STREAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'ingress_stream\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ingress_pattern",
    {
      {
        SUB_BLOCK_INGRESS_PATTERN_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'ingress_pattern\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "logical_multicast",
    {
      {
        SUB_BLOCK_LOGICAL_MULTICAST_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'logical_multicast\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ingress_multicast",
    {
      {
        SUB_BLOCK_INGRESS_MULTICAST_STREAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'ingress_multicast\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "stream_selection",
    {
      {
        SUB_BLOCK_STREAM_SELECTION_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'stream_selection\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "stream_rate",
    {
      {
        SUB_BLOCK_STREAM_RATE_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'stream_rate\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "port_rate",
    {
      {
        SUB_BLOCK_PORT_RATE_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'port_rate\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "m_delay",
    {
      {
        SUB_BLOCK_M_DELAY_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'m_delay\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "last_delay",
    {
      {
        SUB_BLOCK_LAST_DELAY_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'last_delay\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "delay_histogram",
    {
      {
        SUB_BLOCK_DELAY_HISTOGRAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'delay_histogram\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "last_timestamp",
    {
      {
        SUB_BLOCK_LAST_TIMESTAMP_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'last_timestamp\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "inter_packet_delay",
    {
      {
        SUB_BLOCK_INTER_PACKET_DELAY_HISTOGRAM,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'inter_packet_delay\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "jitter_histogram",
    {
      {
        SUB_BLOCK_JITTER_HISTOGRAM_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'jitter_histogram\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "port_statistics",
    {
      {
        SUB_BLOCK_PORT_STATISTICS_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'port_statistics\' verifier memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "channel_weight",
    {
      {
        SUB_BLOCK_CHANNEL_WEIGHT_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'channel_weight\' flow_control memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "channel_fifo_size",
    {
      {
        SUB_BLOCK_CHANNEL_FIFO_SIZE_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'channel_fifo_size\' flow_control memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "spill_over",
    {
      {
        SUB_BLOCK_SPILL_OVER_TABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'spill_over\' flow_control memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_ind_read_vals
*TYPE: BUFFER
*DATE: 09/JULY/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "READ" OF SUBJECT
*  'GENERAL FPGA_INDIRECT'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_ind_read_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "generator",
    {
      {
        GENERATOR_ACCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'generator\' memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "verifier",
    {
      {
        VALIDATOR_ACCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'verifier\' memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "flow_control_block",
    {
      {
        FLOW_CONTROL_ACCESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Access the \'flow control\' memory block in FTG",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Crate_type_demo_vals
*TYPE: BUFFER
*DATE: 10/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "CRATE_TYPE" OF SUBJECT
*  'GENERAL DEMO'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Crate_type_demo_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "gobi",
    {
      {
        CRATE_64_PORT_10G_STANDALONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    GOBI crate",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "negev",
    {
      {
        CRATE_TEST_EVAL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    NEGEV crate",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "this_crate",
    {
      {
        -1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Use the crate, on which this SW is running, for crate-type.\r\n"
              "    MUST be either GOBI or NEGEV!",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

#if !(defined(LINUX) || defined(UNIX))
/* { */

/********************************************************
*NAME
*  Line_card_slot_demo_vals
*TYPE: BUFFER
*DATE: 10/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "LINE_CARD_SLOT" OF SUBJECT
*  'GENERAL DEMO'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Line_card_slot_demo_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_LAST_SLOT_LINE_CARD,
        CSYS_FIRST_SLOT_LINE_CARD,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)line_card_slot_id_checker
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Fabric_card_slot_demo_vals
*TYPE: BUFFER
*DATE: 10/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FABRIC_CARD_SLOT" OF SUBJECT
*  'GENERAL DEMO'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fabric_card_slot_demo_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_LAST_SLOT_FABRIC_CARD,
        CSYS_FIRST_SLOT_FABRIC_CARD,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)fabric_card_slot_id_checker
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

#endif /*} LINUX */

#if LINK_PSS_LIBRARIES
/* { */

/********************************************************
*NAME
*  Fap_link_demo_vals
*TYPE: BUFFER
*DATE: 10/MARCH/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "FAP_LINK" OF SUBJECT
*  'GENERAL DEMO'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  None
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fap_link_demo_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP10M_NUMBER_OF_LINKS - 1,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

#endif /* } LINK_PSS_LIBRARIES */

/********************************************************
*NAME
*  Show_general_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "READ" IN THE CONTEXT
*  OF 'GENERAL' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Show_general_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  METHOD_REGULAR_DISPLAY,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"regular"
  }
}
#endif
;
/********************************************************
*NAME
*  Override_mem_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "OVERRIDE" IN THE CONTEXT OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Override_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FALSE_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"false"
  }
}
#endif
;
/********************************************************
*NAME
*  Show_free_list_default
*TYPE: BUFFER
*DATE: 26/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "SHOW_FREE_LIST" IN THE CONTEXT OF SUBJECT 'VX_SHELL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Show_free_list_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FALSE_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"false"
  }
}
#endif
;
/********************************************************
*NAME
*  Silent_mem_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "SILENT" IN THE CONTEXT OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Silent_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FALSE_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"false"
  }
}
#endif
;
/********************************************************
*NAME
*  Address_in_longs_mem_default
*TYPE: BUFFER
*DATE: 10/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "ADDRESS_IN_LONGS" IN THE CONTEXT OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Address_in_longs_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  FALSE_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"false"
  }
}
#endif
;

/********************************************************
*NAME
*  Soc_sand_system_call_val
*TYPE: BUFFER
*DATE: 24/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DFFS" IN THE
*  CONTEXT OF SUBJECT 'FLASH DFFS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Soc_sand_system_call_val[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        120,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    System command string.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;


/********************************************************
*NAME
*  Soc_sand_tcm_vals
*TYPE: BUFFER
*DATE: 06/FEB/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TCM" IN THE
*  CONTEXT OF SUBJECT 'SOC_SAND'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Soc_sand_tcm_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enable",
    {
      {
        1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Enabling TCM in the SOC_SAND-driver.\n\r"
              "    Callbacks, counter collection and interrupt mock-up are enabled.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "disable",
    {
      {
        0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Disabling TCM in the SOC_SAND-driver.\n\r"
              "    Callbacks, counter collection and interrupt mock-up are disabled.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Soc_sand_translate_proc_id_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TRANSLATE" IN THE
*  CONTEXT OF SUBJECT 'SOC_SAND' (AND 'USER_TX_TABLE').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Soc_sand_translate_proc_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0xFFFFFFFF,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Type_mem_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "TYPE".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Type_mem_vals[]
#ifdef INIT
   =
{
#if (SIMULATE_DUNE_CHIP || SIMULATE_CHIP_SIM)
/* { */
  {
    VAL_SYMBOL,
    "fap10m_1_regs",
    {
      {
        FAP10M_1_REGS_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to simulated registers of SOC_SAND_FAP10M chip number 1.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "fap10m_2_regs",
    {
      {
        FAP10M_2_REGS_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to simulated registers of SOC_SAND_FAP10M chip number 2.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "fe_1_regs",
    {
      {
        FE_1_REGS_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of FE chip number 1.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "fe_2_regs",
    {
      {
        FE_2_REGS_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of FE chip number 2.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
/* } */
#endif
  {
    VAL_SYMBOL,
    "ftg_a",
    {
      {
        GP1_FTG_A_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of FTG A.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "ftg_b",
    {
      {
        GP1_FTG_B_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of FTG B.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fap20v_1",
    {
      {
        GP1_FAP20V_1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of SOC_SAND_FAP20V chip number 1.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fap20v_fsc_1",
    {
      {
        GP1_FAP20V_FSC_1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of SOC_SAND_FAP20V-FSC chip number 1.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fap20v_epld_1",
    {
      {
        GP1_FAP20V_EPLD_1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of SOC_SAND_FAP20V-EPLD.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fap20v_flash_1",
    {
      {
        GP1_FAP20V_FLASH_1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of SOC_SAND_FAP20V-FLASH.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fap20v_strm_fpga_1",
    {
      {
        GP1_FAP20V_STRM_IF_1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of SOC_SAND_FAP20V-STREAMING FPGA.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fap10m_1",
    {
      {
        GP1_FAP10M_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to internal registers of SOC_SAND_FAP10M chip number 1.\n\r",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fe1_regs",
    {
      {
        GP1_FE1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned for direct access FE chip number 1\r\n"
              "    even while simulation of FE is active.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gp1_fe2_regs",
    {
      {
        GP1_FE2_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned for direct access FE chip number 2\r\n"
              "    even while simulation of FE is active.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "sdram",
    {
      {
        SDRAM_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to programs and variables RAM area (SDRAM).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "flash_bank_1",
    {
      {
        FLASH_BANK_1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to programs and pre-assigned variables\r\n"
        "    in non-volatile memory (FLASH). Both boot code and various application\r\n"
        "    files are kept here.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "e2prom",
    {
      {
        E2PROM_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to set up parameters in non-volatile memory\r\n"
        "   (EEPROM). Error log file is also kept here.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "epld",
    {
      {
        EPLD_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Memory area assigned to programmable logic used by various HW components.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "general_purpose_2",
    {
      {
        GP2_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    General pupose memory area. Assignment still T.B.D.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "general_purpose_3",
    {
      {
        GP3_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    General pupose memory area. Assignment still T.B.D. Only acceptable on\r\n"
              "    old mezzanine board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "dual_port_ram",
    {
      {
        INTERNAL_DPRAM_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Internal dual port ram on MPC860, including parameter ram.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "mem_buffer",
    {
      {
        MEM_BUFFER_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Local buffer, assigned from standard sdram, to contain results\r\n"
              "    of \'mem read\' operations (when so indicated by the user).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gt64131_sdram",
    {
      {
        GT64131_SDRAM_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Shared SDRAM over PCI. Only acceptable on new mezzanine board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "gt64131_regs",
    {
      {
        GT64131_REGS_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Internal GT64131 registers. Only acceptable on new mezzanine board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "pci_io",
    {
      {
        PCI_IO_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    IO space over PCI. Only acceptable on new mezzanine board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "pci_memory_block_1",
    {
      {
        PCI_MEM1_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    First memory block over PCI. Only acceptable on new mezzanine board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "pci_memory_block_2",
    {
      {
        PCI_MEM2_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Second memory block over PCI. Only acceptable on new mezzanine board",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "tevb_timna_memory_block",
    {
      {
        TEVB_TIMNA_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Timna address space. Only acceptable on TEVB.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "tevb_epld_memory_block",
    {
      {
        TEVB_EPLD_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    TEVB EPLD address space. Only acceptable on TEVB.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "tevb_flash_memory_block",
    {
      {
        TEVB_FLASH_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    TEVB FLASH address space. Only acceptable on TEVB.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)type_mem_checker
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Type_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "TYPE".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Type_mem_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  SDRAM_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"sdram"
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_ind_auto_clear_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "AUTO_CLEAR" IN THE
*  CONTEXT OF SUBJECT "GENERAL FPGA_INDIRECT READ".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Fpga_ind_auto_clear_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  OFF_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"off"
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_ind_auto_clear_vals
*TYPE: BUFFER
*DATE: 27/JULY/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "AUTO_CLEAR" IN THE
*  CONTEXT OF SUBJECT "GENERAL FPGA_INDIRECT READ".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_ind_auto_clear_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        ON_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Clear (set to \'0\') each memory location after reading.",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        OFF_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Leave as-is each memory location after reading (standard behaviour).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Fpga_ind_write_data_vals
*TYPE: BUFFER
*DATE: 20/JULY/2004
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DATA" IN THE CONTEXT OF
*  "GENERAL FPGA_INDIRECT WRITE".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Fpga_ind_write_data_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        16,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Data_mem_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "Data".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Data_mem_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        0,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        MAX_REPEATED_PARAM_VAL,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;


/********************************************************
*NAME
*  Snmp_file_name_vals
*TYPE: BUFFER
*DATE: 24/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "SNM_FILE" IN THE
*  CONTEXT OF SUBJECT 'SNMP DISPLAY_FILE'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Snmp_file_name_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        MAX_SIZE_OF_TEXT_VAR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Name of configuration file to diaplay.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Dffs_file_name_vals
*TYPE: BUFFER
*DATE: 24/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DFFS" IN THE
*  CONTEXT OF SUBJECT 'FLASH DFFS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dffs_file_name_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DFFS_FILENAME_SIZE-1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Dffs file name.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_comment_vals
*TYPE: BUFFER
*DATE: 24/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DFFS" IN THE
*  CONTEXT OF SUBJECT 'FLASH DFFS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dffs_comment_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DFFS_COMMENT_STR_SIZE-1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    A comment attached to a dffs file.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_date_vals
*TYPE: BUFFER
*DATE: 24/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DFFS" IN THE
*  CONTEXT OF SUBJECT 'FLASH DFFS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dffs_date_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DFFS_DATE_STR_SIZE-1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Dffs file creation date.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_date_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "SET_ATTR"  IN THE CONTEXT
*  OF SUBJECT 'DFFS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Dffs_date_default
#ifdef INIT
   =
{
  VAL_TEXT,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)""
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_attr_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "add_attr" IN THE
*  CONTEXT OF SUBJECT 'dffs' (AND 'USER_TX_TABLE').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dffs_attr_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;


/********************************************************
*NAME
*  Dffs_ver_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "add_attr" IN THE
*  CONTEXT OF SUBJECT 'dffs' (AND 'USER_TX_TABLE').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dffs_ver_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_attr_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DFFS"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Dffs_attr_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_dir_format_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "dir" IN THE CONTEXT
*  OF 'dffs' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dffs_dir_format_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "long",
    {
      {
        LONG_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Dffs dir - long version (more detailed).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "short",
    {
      {
        SHORT_EQUIVALENT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Dffs dir - short version (less detailed).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },

/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_dir_format_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "dir" IN THE CONTEXT
*  OF 'dffs' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Dffs_dir_format_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  SHORT_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"short"
  }
}
#endif
;

/********************************************************
*NAME
*  Dffs_prnt_size_vals
*TYPE: BUFFER
*DATE: 31/MARCH/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "print" IN THE
*  CONTEXT OF SUBJECT 'dffs' (AND 'USER_TX_TABLE').
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Dffs_prnt_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },

/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;



/********************************************************
*NAME
*  Dffs_prnt_size_default
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DFFS"  IN THE CONTEXT
*  OF SUBJECT 'MEM'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Dffs_prnt_size_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)(128*1024)
  }
}
#endif
;

/********************************************************
*NAME
*  Data_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE
*  RELATED TO THE PARAMETER "DATA".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Data_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
*NAME
*  Val_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "VAL" WITHIN  "VAL_READ" WITHIN 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Val_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;
/********************************************************
*NAME
*  Mask_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "MASK" WITHIN  "VAL_READ" WITHIN 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Mask_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)(-1)
  }
}
#endif
;
/********************************************************
*NAME
*  Timeout_mem_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE DEFAULT VALUE RELATED TO THE
*  PARAMETER "TIMEOUT" WITHIN  "VAL_READ" WITHIN 'MEM' SUBJECT.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    int numeric_equivalent -
*      Numeric value related to string_value. Only relevant
*      for val_type = VAL_SYMBOL
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Timeout_mem_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1000
  }
}
#endif
;

/********************************************************
*NAME
*  Ping_host_vals
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "WRITE".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Ping_host_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Host_ip_ping_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  Default host to ping is the 'localhost' = 127.0.0.1 = 0xff000001.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Host_ip_ping_default
#ifdef INIT
   =
{
  VAL_IP,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0x7f000001
  }
}
#endif
;
/********************************************************
*NAME
*  Nof_pkt_ping_vals
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  Number of packets to ping.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Nof_pkt_ping_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000,1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  ping_opt_dontroute_vals
*TYPE: BUFFER
*DATE: 14/AUG/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "OFFSET".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'dont_route':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  This parameter has no values. When it appears on the
*  line, a value of 'true' is implied.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     ping_opt_dontroute_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  ping_opt_nohost_vals
*TYPE: BUFFER
*DATE: 14/AUG/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "OFFSET".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'no_host':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  This parameter has no values. When it appears on the
*  line, a value of 'true' is implied.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     ping_opt_nohost_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  ping_opt_debug_vals
*TYPE: BUFFER
*DATE: 14/AUG/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "debug".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'no_host':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  This parameter has no values. When it appears on the
*  line, a value of 'true' is implied.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     ping_opt_debug_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Nof_pkt_ping_default
*TYPE: BUFFER
*DATE: 27/JAN/2002
*FUNCTION:
*  Default number of packets to send is 1.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of variable (VAL_NUMERIC, VAL_TEXT,
*      VAL_IP OR VAL_SYMBOL)
*    union value -
*      Union of one of
*        int            int_value
*        char           *string_value
*        char           val_text[MAX_SIZE_OF_TEXT_VAR] ;
*        unsigned long  ip_value ;
*      Value of default variable.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL
     Nof_pkt_ping_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
*NAME
*  Vx_shell_params
*TYPE: BUFFER
*DATE: 02/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "VX_SHELL".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      THIS FIELD IS AUTOMATICALLY FILLED IN BY THE SYSTEM
*      and does NOT need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Vx_shell_params[]
#ifdef INIT
   =
{
  {
    PARAM_VXSHELL_REBOOT_ID,
    "reboot",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Reset network devices and transfer control to boot ROMs.",
    "Examples:\r\n"
    "  vx_shell reboot\r\n"
    "Results in software 'hard' reset to the system using the watchdog mechanism.\r\n"
    " (for SW reboot add 'soft_reoot' to command line,\n\r"
    " thus calling to 'reboot()' function.)\n\r",
    "",
    {BIT(4)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_REBOOT_SOFT_ID,
    "soft_reboot",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, the CPU will be rebooted through calling to 'reboot()' function.",
    "Examples:\r\n"
    "  vx_shell reboot soft_reboot\r\n"
    "Results in SW reset to the system.\r\n",
    "",
    {BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_ROUTE_SHOW_ID,
    "route_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display host and network routing tables.",
    "Examples:\r\n"
    "  vx_shell route_show\r\n"
    "Results in display host and network routing tables. Explanation\r\n"
    "of the various values of the \'flags\' column follow.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_IF_SHOW_ID,
    "if_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display information on all attached network interfaces.",
    "Examples:\r\n"
    "  vx_shell if_show\r\n"
    "Results in display of information and statistics on attached\r\n"
    "interfaces like 'motfec' (and the local 'lo' which is always there)\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_I_NET_STAT_SHOW_ID,
    "i_net_stat_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display all active connections for Internet protocol sockets.",
    "Examples:\r\n"
    "  vx_shell i_net_stat_show\r\n"
    "Results in display of information on currently active protocol sockets.\r\n"
    "If, for example, telnet (TCP) is open then one may get:\r\n"
    "Active Internet connections (including servers)\r\n"
    "PCB      Proto Recv-Q Send-Q  Local Address      Foreign Address    (state)\r\n"
    "-------- ----- ------ ------  ------------------ ------------------ -------\r\n"
    "1e98db8  TCP        0      0  192.168.1.63.23    192.168.1.37.2915\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_ICMP_STAT_SHOW_ID,
    "icmp_stat_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display statistics for ICMP (e.g. ping, echo).",
    "Examples:\r\n"
    "  vx_shell icmp_stat_show\r\n"
    "Results in display of statistics on ICMP (Internet Control Message Protocol)\r\n"
    "protocol. Can be used to verify that a \'ping\' has been sent to this station\r\n"
    "and has been replied.",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_CHECK_STACK_SHOW_ID,
    "check_stack_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display status of stack for all active tasks.",
    "Examples:\r\n"
    "  vx_shell check_stack\r\n"
    "Results in display of status of stack for all tasks. Somethink like\r\n"
    "    NAME         ENTRY      TID    SIZE   CUR  HIGH  MARGIN\r\n"
    "------------ ------------ -------- ----- ----- ----- ------\r\n"
    "tShell       _shell       23e1c78   9208   832  3632   5576\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_PRINT_ERRNO_ID,
    "print_errno",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display the definition of a specified error status value.",
    "Examples:\r\n"
    "  vx_shell print_errno 0xD0003\r\n"
    "Results in display of definition related to this code:\r\n"
    "S_iosLib_INVALID_FILE_DESCRIPTOR\r\n"
    "Typing errorn no. \'0\' result in displaying status of current task",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_MEM_SHOW_ID,
    "mem_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display system memory partition blocks and statistics (malloc).",
    "Examples:\r\n"
    "  vx_shell mem_show\r\n"
    "Results in display of available and allocated memory in the system memory\r\r"
    "partition. It shows number of bytes, the number of blocks, and average block\r\n"
    "size in both free and allocated memory, (also the maximum block size of free\r\n"
    "memory, number of blocks currently allocated and average allocated block size).\r\n"
    "Free list may be shown using \'show_free_list\' parameter.\r\n",
    "",
    {BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_SHOW_FREE_LIST_ID,
    "show_free_list",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Show_free_list_default,
    0,0,0,0,0,0,0,
    "Show list of freed memory blocks",
    "Examples:\r\n"
    "  vx_shell mem_show show_free_list\r\n"
    " Results in all display of mem_show plus free list\r\n",
    "",
    {BIT(3)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_ARP_SHOW_ID,
    "arp_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display entries in the system ARP table.",
    "Examples:\r\n"
    "  vx_shell arp_show\r\n"
    "Results in display of current Internet-to-Ethernet address mappings in the\r\n"
    "ARP table. Example:\r\n"
    "destination      gateway            flags  Refcnt  Use           Interface\r\n"
    "--------------------------------------------------------------------------\r\n"
    "192.168.1.253    00:03:47:6b:e2:e0    405    1       0             motfec0\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_I_ID,
    "ii",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print a summary of each task\'s TCB.",
    "Examples:\r\n"
    "  vx_shell ii\r\n"
    "Results in display a summarized information line on each task.\r\n"
    "See explanation of the various fields in documentation.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_DEVS_ID,
    "devs",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "List all system-known devices.",
    "Examples:\r\n"
    "  vx_shell devs\r\n"
    "Results in display a table with information line on device.\r\n"
    "See explanation of the various fields in documentation.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_LOGOUT_ID,
    "logout",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Log out of the system.",
    "Examples:\r\n"
    "  vx_shell logout\r\n"
    "This command logs out of the shell. If a remote login\r\n"
    "is active (via rlogin or telnet), it is stopped, and standard\r\n"
    "I/O is restored to the console.\r\n"
    "See detailed explanation in documentation.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_IOSFDSHOW_ID,
    "ios_fd_show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display a list of file descriptor names in the system.",
    "Examples:\r\n"
    "  vx_shell ios_fd_show\r\n"
    "This command displays a table of file descriptor names in the system.\r\n"
    "See detailed explanation in documentation.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_PING_ID,
    "ping",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "ping the host.",
    "Examples:\r\n"
    "  vx_shell ping \r\n"
    "This command pings this specific host, default host is 'localhost' .\r\n"
    "See detailed explanation in documentation.\r\n",
    "",
    {BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_HOST_IP_ID,
    "host_ip",
    (PARAM_VAL_RULES *)&Ping_host_vals[0],
    (sizeof(Ping_host_vals) / sizeof(Ping_host_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Host_ip_ping_default,
    0,0,0,0,0,0,0,
    "ping the host.",
    "Examples:\r\n"
    "  vx_shell ping host 168.12.1.98 \r\n"
    "This command pings the specific host .\r\n"
    "See detailed explanation in documentation.\r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_PING_NOF_PKTS_ID,
    "num_packets",
    (PARAM_VAL_RULES *)&Nof_pkt_ping_vals[0],
    (sizeof(Nof_pkt_ping_vals) / sizeof(Nof_pkt_ping_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Nof_pkt_ping_default,
    0,0,0,0,0,0,0,
    "Number of packets to send with ping.",
    "Examples:\r\n"
    "  ping num_packets 100 \r\n"
    "Ping 'localhost' with 100 packets.\r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_PING_OPT_NOHOST_ID,
    "no_host",
    (PARAM_VAL_RULES *)&ping_opt_nohost_vals[0],
    (sizeof(ping_opt_nohost_vals) / sizeof(ping_opt_nohost_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Suppress host lookup (This is useful when you have the DNS resovler\n\r"
    "  but the DNS server is down and not returning host names).",
    "Examples:\r\n"
    "  ping num_packets 100 no_host \r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_PING_OPT_DONTROUTE_ID,
    "dont_route",
    (PARAM_VAL_RULES *)&ping_opt_dontroute_vals[0],
    (sizeof(ping_opt_dontroute_vals) / sizeof(ping_opt_dontroute_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do not route packets past the local network.",
    "Examples:\r\n"
    "  ping num_packets 100 dont_route \r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_PING_OPT_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Suppress output (This option is useful for applications that use"
    "  ping( ) programmatically to examine the return status).",
    "Examples:\r\n"
    "  ping num_packets 100 silent \r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_PING_OPT_DEBUG_ID,
    "debug",
    (PARAM_VAL_RULES *)&ping_opt_debug_vals[0],
    (sizeof(ping_opt_debug_vals) / sizeof(ping_opt_debug_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints ping debug info (This is useful when you more info).",
    "Examples:\r\n"
    "  ping num_packets 100 no_host debug \r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* task operations */
  {
    PARAM_VXSHELL_TASK_ID,
    "task",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Performs a task operation.",
    "Examples:\r\n"
    "  task 0x1e5e470 restart\r\n",
    "",
    {BIT(5) | BIT(6) | BIT(7) | BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_TASK_OPT_DELETE_ID,
    "delete",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deletes a task.",
    "Examples:\r\n"
    "  task 0x1e5e470 delete\r\n",
    "",
    {BIT(5)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_TASK_OPT_RESTART_ID,
    "restart",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Restarts a task.",
    "Examples:\r\n"
    "  task 0x1e5e470 restart\r\n",
    "",
    {BIT(6)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_TASK_OPT_RESUME_ID,
    "resume",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Resumes a task.",
    "Examples:\r\n"
    "  task 0x1e5e470 resume\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_VXSHELL_TASK_OPT_SUSPEND_ID,
    "suspend",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Suspends a task.",
    "Examples:\r\n"
    "  task 0x1e5e470 suspend\r\n",
    "",
    {BIT(8)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;

#if INCLUDE_NET_SNMP
/* { */
/********************************************************
*NAME
*  Snmp_params
*TYPE: BUFFER
*DATE: 24/Jul/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "SNMP".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      THIS FIELD IS AUTOMATICALLY FILLED IN BY THE SYSTEM
*      and does NOT need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Snmp_params[]
#ifdef INIT
   =
{
  {
    PARAM_SNMP_SHUTDOWN_AGENT_ID,
    "shutdown_snmp_agent",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Shuts down SNMP agent completly. The agent could not be\r\n"
    "  restarted without a copmlete system reboot.",
    "Examples:\r\n"
    "  snmp shutdown_snmp_agent\r\n"
    "Results in shutting down the SNMP agent.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_LOAD_CONFIG_FILE_ID,
    "load_configuration_file",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load snmpd.conf through tftp from the downloading host.",
    "Examples:\r\n"
    "  snmp load_configuration_file\r\n"
    "Results in restart of the snmp agent with the updated configuration file.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_DISP_FILE_ID,
    "display_file",
    (PARAM_VAL_RULES *)&Snmp_file_name_vals[0],
    (sizeof(Snmp_file_name_vals) / sizeof(Snmp_file_name_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Displays the requested (ascii) file (exactly like cat)",
    "Examples:\r\n"
    "  snmp display_file SNMP:\\snmpd.conf\r\n"
    "Results in printing the content of the file to the screen, .\r\n"
    " or printing an error message if the file doesn't exist.\r\n"
    " Only ascii chars are presented, binary symbols are replaced with '.'",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_DISP_FILE_ID,
    "cat",
    (PARAM_VAL_RULES *)&Snmp_file_name_vals[0],
    (sizeof(Snmp_file_name_vals) / sizeof(Snmp_file_name_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Displays the requested (ascii) file",
    "Examples:\r\n"
    "  snmp cat SNMP:\\snmpd.conf\r\n"
    "Results in printing the content of the file to the screen, .\r\n"
    " or printing an error message if the file doesn't exist.\r\n"
    " Only ascii chars are presented, binary symbols are replaced with '.'",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_DIR_ID,
    "dir",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Displays the list of snmp files loaded\r\n"
    "  (including MIBs and configuration files)",
    "Examples:\r\n"
    "  snmp dir\r\n"
    "Results in printing a list of files that are loaded on the device.\r\n"
    " May be configuration files or MIB files, that Net-SNMP uses.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_DIR_ID,
    "ls",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Displays the list of snmp files loaded\r\n"
    "  (including MIBs and configuration files - exactly like dir)",
    "Examples:\r\n"
    "  snmp ls\r\n"
    "Results in printing a list of files that are loaded on the device.\r\n"
    " May be configuration files or MIB files, that Net-SNMP uses.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/* } INCLUDE_NET_SNMP */
#endif


/********************************************************
*NAME
*  Soc_sand_params
*TYPE: BUFFER
*DATE: 29/Dec/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "SOC_SAND".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      THIS FIELD IS AUTOMATICALLY FILLED IN BY THE SYSTEM
*      and does NOT need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Soc_sand_params[]
#ifdef INIT
   =
{
  {
    PARAM_SAND_PRINT_WHEN_WRITING_ID,
    "print_when_writing",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print when writing to device.\r\n",
    "Examples:\r\n"
    "  soc_sand print_when_writing 0 enable\r\n"
    "Results in printing when writing to device 0.",
    "",
    {BIT(26) | BIT(27)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_CALL_SYSTEM_ID,
    "system_call",
    (PARAM_VAL_RULES *)&Soc_sand_system_call_val[0],
    (sizeof(Soc_sand_system_call_val) / sizeof(Soc_sand_system_call_val[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "system call.",
    "Examples:\r\n"
    "  soc_sand system_call \r\n",
    "",
    {0,0,BIT(21)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_PRINT_WHEN_WRITING_EN_ID,
    "enable",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print when writing to device.\r\n",
    "Examples:\r\n"
    "  soc_sand print_when_writing 0 enable\r\n"
    "Results in printing when writing to device 0.",
    "",
    {BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_PRINT_WHEN_WRITING_IND_ID,
    "ignore_indirect",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print when writing to device.\r\n",
    "Examples:\r\n"
    "  soc_sand print_when_writing 0 enable\r\n"
    "Results in printing when writing to device 0.",
    "",
    {BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_PRINT_WHEN_WRITING_ASIC_STYLE_ID,
    "asic_style",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print when writing to device.\r\n",
    "Examples:\r\n"
    "  soc_sand print_when_writing 0 enable\r\n"
    "Results in printing when writing to device 0.",
    "",
    {BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_PRINT_WHEN_WRITING_DIS_ID,
    "disable",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print when writing to device.\r\n",
    "Examples:\r\n"
    "  soc_sand print_when_writing 0 enable\r\n"
    "Results in printing when writing to device 0.",
    "",
    {BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_DEVICE_TYPE_ID,
    "get_device_type",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Return the device type and version.\r\n",
    "Examples:\r\n"
    "  soc_sand get_device_type 0\r\n"
    "Results in printing the device type and version of device 0.\n\r"
    "  'The device is SOC_SAND_FAP10M with hardware version: 1.1.' is a possible output.",
    "",
    {BIT(10)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_TCM_ID,
    "tcm",
    (PARAM_VAL_RULES *)&Soc_sand_tcm_vals[0],
    (sizeof(Soc_sand_tcm_vals) / sizeof(Soc_sand_tcm_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable/disable soc_sand Timed Callback Machine (TCM).\r\n",
    "Examples:\r\n"
    "  soc_sand tcm enable\r\n"
    "Results in enabling TCM in the SOC_SAND-driver.\n\r"
    "  Callbacks, counter collection and interrupt mock-up are enabled",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_TRANSLATE_ID,
    "translate",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Translate driver constants to strings.\r\n",
    "Examples:\r\n"
    "  soc_sand translate return_code 0x11112222\r\n"
    "Results in searching procedure identifier 0x1111 and error code 0x2222\n\r"
    "  - case find prints the procedure name and erorr code.",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_TRANSLATE_TO_PROC_ID_ID,
    "return_code",
    (PARAM_VAL_RULES *)&Soc_sand_translate_proc_id_vals[0],
    (sizeof(Soc_sand_translate_proc_id_vals) / sizeof(Soc_sand_translate_proc_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Translate driver return code to procedure and error strings.\r\n",
    "Examples:\r\n"
    "  soc_sand translate return_code 0x11112222\r\n"
    "Results in searching procedure identifier 0x1111 and error code 0x2222\n\r"
    " case find prints the procedure name and erorr code.\n\r"
    " Notice, the driver should be running to get meaningful results.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_TRACE_PRINT_ID,
    "trace_print",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints the trace from SOC_SAND.\r\n",
    "Examples:\r\n"
    "  soc_sand trace_print\r\n"
    "Results in printing SOC_SAND trace.",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_TRACE_CLEAR_ID,
    "trace_clear",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clears the trace from SOC_SAND.\r\n",
    "Examples:\r\n"
    "  soc_sand trace_clear\r\n"
    "Results in clearing SOC_SAND trace.",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_TCM_PRINT_DELTA_LIST_ID,
    "soc_tcmprint_delta_list",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints the TCM delta list.\r\n",
    "Examples:\r\n"
    "  soc_sand soc_tcmprint_delta_list\r\n"
    "Results in printing the TCM delta list.",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_OS_RESOURCE_PRINT_ID,
    "os_resource_print",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints the SOC_SAND os taken resources.\r\n",
    "Examples:\r\n"
    "  soc_sand os_resource_print\r\n"
    "Results in printing the SOC_SAND os taken resources.",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SAND_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints the SOC_SAND status.\r\n",
    "Examples:\r\n"
    "  soc_sand status\r\n"
    "Results in printing the SOC_SAND status.",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Nvram_params
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "NVRAM".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      THIS FIELD IS AUTOMATICALLY FILLED IN BY THE SYSTEM
*      and does NOT need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Nvram_params[]
#ifdef INIT
   =
{
  {
    PARAM_NVRAM_SHOW_ID,
    "show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Show information on variables contained in NVRAM blocks (exactly equivalent\r\n"
    "  to \'read\').",
    "Examples:\r\n"
    "  nvram show block boot_parameters\r\n"
    "Results in displaying all variables contained in NVRAM block no. 2\r\n",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_SHOW_ID,
    "read",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Show information on variables contained in NVRAM blocks.",
    "Examples:\r\n"
    "  nvram read block 2\r\n"
    "Results in displaying all variables contained in NVRAM block no. 2\r\n",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

#if !(defined(LINUX) || defined(UNIX))
/* { */
  {
    PARAM_NVRAM_UPDATE_ID,
    "write",
    (PARAM_VAL_RULES *)&Update_nvram_vals[0],
    (sizeof(Update_nvram_vals) / sizeof(Update_nvram_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Update_nvram_default,
    0,0,0,0,0,0,0,
    "Write variables contained in NVRAM blocks.",
    "Examples:\r\n"
    "  nvram write next_startup data_cache on\r\n"
    "Results in updating the control variable \'data_cache\' in NVRAM so\r\n"
    "that next startup, data cache will be activated.\r\n"
    "  nvram write runtime data_cache on\r\n"
    "Results in updating the control variable \'data_cache\' immediately\r\n"
    "(at runtime) so that data cache is activated. This has no effect\r\n"
    "on next strtup.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_UPDATE_ID,
    "update",
    (PARAM_VAL_RULES *)&Update_nvram_vals[0],
    (sizeof(Update_nvram_vals) / sizeof(Update_nvram_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Update_nvram_default,
    0,0,0,0,0,0,0,
    "Write variables contained in NVRAM blocks (exactly equivalent to \'write\').",
    "Examples:\r\n"
    "  nvram update next_startup data_cache on\r\n"
    "Results in updating the control variable \'data_cache\' in NVRAM so\r\n"
    "that next startup, data cache will be activated.\r\n"
    "  nvram update runtime data_cache on\r\n"
    "Results in updating the control variable \'data_cache\' immediately\r\n"
    "(at runtime) so that data cache is activated. This has no effect\r\n"
    "on next strtup.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BOARD_PARAMS_ID,
    "board_params",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Updates information on board params contained in NVRAM blocks.",
    "Examples:\r\n"
    "  nvram cpu_board_description \"CPU Mezzanine\"\r\n"
    "Results in updating the board description TEXT field in NVRAM so\r\n"
    "that next startup, boot procedures will display new description.\r\n",
    "",
    {BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_SET_CMD_LINE_ID,
    "snmp_cmd_line",
    (PARAM_VAL_RULES *)&Snmp_cmd_line_nvram_vals[0],
    (sizeof(Snmp_cmd_line_nvram_vals) / sizeof(Snmp_cmd_line_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'snmp_cmd_line\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup snmp_cmd_line_arguments -c private -Dread_config\r\n"
    "Results in updating the control variable \'snmp_cmd_line\' in NVRAM\r\n"
    "so that next startup, boot procedure will pass this command line to \r\n"
    "the Net-SNMP agent\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_DOWNLOAD_MIBS_ID,
    "snmp_download_mibs",
    (PARAM_VAL_RULES *)&Snmp_Download_mibs_nvram_vals[0],
    (sizeof(Snmp_Download_mibs_nvram_vals) / sizeof(Snmp_Download_mibs_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'download_mibs\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup snmp_download_mibs off\r\n"
    "Results in updating the control variable \'download_mibs\' in NVRAM so that\r\n"
    "next startup, no attempt will be made to download mibs from downloading host.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SNMP_INCLUDE_NETSNMP_ID,
    "snmp_load_netsnmp_at_startup",
    (PARAM_VAL_RULES *)&Snmp_load_netsnmp_nvram_vals[0],
    (sizeof(Snmp_load_netsnmp_nvram_vals) / sizeof(Snmp_load_netsnmp_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'load_netsnmp\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup snmp_load_netsnmp_at_startup off\r\n"
    "Results in updating the control variable \'load_netsnmp\' in NVRAM so that\r\n"
    "next startup, no attempt will be made to load the NetSNMP agent.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_CONF_FROM_MD_ID,
    "conf_from_md_register",
    (PARAM_VAL_RULES *)&Conf_from_md_nvram_vals[0],
    (sizeof(Conf_from_md_nvram_vals) / sizeof(Conf_from_md_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'conf_from_md_register\' variable in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup conf_from_md_register on\r\n"
    "Results in updating the control variable \'conf_from_md_register\' in\r\n"
    "NVRAM so that from next startup, \'configuration\' register (on FE chip)\r\n"
    "is read from MD pins and not from NVRAM.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_FE_CONFIGURATION_ID,
    "configuration_register",
    (PARAM_VAL_RULES *)&Fe_configuration_nvram_vals[0],
    (sizeof(Fe_configuration_nvram_vals) / sizeof(Fe_configuration_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'configuration_register\' variable in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup configuration_register 0x\r\n"
    "Results in updating the variable \'configuration_register\' in NVRAM so\r\n"
    "that from next startup, this value will be loaded into \'configuration\'\r\n"
    "register (on FE chip) provided \'conf_from_md_register\' is set to \'off\'.\r\n"
    "\'configuration_register\' register is interpreted as follows (BIT0 is LS):\r\n"
    "BITS  0-10: Chip Id.\r\n"
    "BIT     11: Mode FE2 (1) or Mode FE13 (0).\r\n"
    "BIT     12: FE to operate as repeater (1) or not (0).\r\n"
    "BITS 13-15: Number of fabric planes is 2 to the power of this value.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_LINE_MODE_ID,
    "line_mode",
    (PARAM_VAL_RULES *)&Line_mode_nvram_vals[0],
    (sizeof(Line_mode_nvram_vals) / sizeof(Line_mode_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'line_mode\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write run_time line_mode on\r\n"
    "Results in updating the run-time control variable \'line_mode\' so\r\n"
    "that from that time on, input command lines, from the user, will be\r\n"
    "interpreted only after \'carriage return\' is hit (and not after each input\r\n"
    "character. One result of that is that \'space tab\' will not complete\r\n"
  "the word to a valid option. Enother result is that the online help\r\n"
  "(by pressing \'?\') is not activated.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_ACTIVATE_WATCHDOG_ID,
    "activate_watchdog",
    (PARAM_VAL_RULES *)&Activate_watchdog_nvram_vals[0],
    (sizeof(Activate_watchdog_nvram_vals) / sizeof(Activate_watchdog_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'activate_watchdog\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup activate_watchdog on\r\n"
    "Results in updating the control variable \'activate_watchdog\' in NVRAM so\r\n"
    "that from next startup, watchdog will be automatically started by the\r\n"
    "application.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOOK_SOFTWARE_EXCEPTION_EXC_ID,
    "hook_software_exception",
    (PARAM_VAL_RULES *)&Activate_software_exception_nvram_vals[0],
    (sizeof(Activate_software_exception_nvram_vals) / sizeof(Activate_software_exception_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'hook_software_exception\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup hook_software_exception on\r\n"
    "Results in updating the control variable \'hook_software_exception\' in NVRAM so\r\n"
    "that from next startup, \'hook_software_exception\' will be hooked to the\r\n"
    " 1. Software Emulation Exception.\r\n"
    " 2. Alignment.\r\n"
    " 3. DSI.\r\n"
    " 4. ISI.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BCKG_TEMPERATURES_EN_ID,
    "background_collect_all_temperatures_enable",
    (PARAM_VAL_RULES *)&Background_collect_all_temperatures_enable_vals[0],
    (sizeof(Background_collect_all_temperatures_enable_vals) / sizeof(Background_collect_all_temperatures_enable_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write 'Enable disable background collection temperature' variable\n\r"
    "  contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup background_collect_all_temperatures_enable off\r\n"
    "Results in updating the control variable \'background_collect_all_temperatures_enable\' in NVRAM so\r\n"
    "  that from next startup, disable background collection temperature\r\n"
    "  If disabled:\r\n"
    "  1. No I2C transaction toward the temperature.\r\n"
    "  2. No valid temperature is given when asked.\r\n"
    "  3. Original intension is to reduced the I2C\r\n"
    "     transactions for hardware debugging.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_PAGE_MODE_ID,
    "page_mode",
    (PARAM_VAL_RULES *)&Page_mode_nvram_vals[0],
    (sizeof(Page_mode_nvram_vals) / sizeof(Page_mode_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'page_mode\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup page_mode on\r\n"
    "Results in updating the control variable \'page_mode\' in NVRAM so\r\n"
    "that from next startup, long dumps to screen will be stopped every\r\n"
    "so many lines and wait for user signal.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_PAGE_LINES_ID,
    "lines_per_page",
    (PARAM_VAL_RULES *)&Page_lines_nvram_vals[0],
    (sizeof(Page_lines_nvram_vals) / sizeof(Page_lines_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'lines_per_page\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup lines_per_page 40\r\n"
    "Results in updating the variable \'lines_per_page\' in NVRAM so\r\n"
    "that from next startup, long dumps to screen will be stopped every\r\n"
    "\'lines_per_page\' lines (and wait for user signal) provided \'page_mode\'\r\n"
    "is on.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_WATCHDOG_PERIOD_ID,
    "watchdog_period",
    (PARAM_VAL_RULES *)&Watchdog_period_nvram_vals[0],
    (sizeof(Watchdog_period_nvram_vals) / sizeof(Watchdog_period_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'watchdog_period\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup watchdog_period 20\r\n"
    "Results in updating the variable \'watchdog_period\' in NVRAM so\r\n"
    "that from next startup, period of watchdog timer is going to be about\r\n"
    "20*100 = 2000 milliseconds.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_TELNET_TIMEOUT_STD_ID,
    "telnet_timeout_standard",
    (PARAM_VAL_RULES *)&Tlnt_timeout_std_nvram_vals[0],
    (sizeof(Tlnt_timeout_std_nvram_vals) / sizeof(Tlnt_timeout_std_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'telnet_timeout_standard\' variable into NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup telnet_timeout_standard 20\r\n"
    "Results in updating the variable \'telnet_timeout_standard\' in NVRAM so\r\n"
    "that from next startup, timeout period for disconnecting Telnet when there\r\n"
    "is no activity is going to be 20 seconds. This is the timeout for the standard\r\n"
    "case (not for continuous/periodic display)",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_TELNET_TIMEOUT_CONT_ID,
    "telnet_timeout_periodic",
    (PARAM_VAL_RULES *)&Tlnt_timeout_cont_nvram_vals[0],
    (sizeof(Tlnt_timeout_cont_nvram_vals) / sizeof(Tlnt_timeout_cont_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'telnet_timeout_periodic\' variable into NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup telnet_timeout_periodic 60\r\n"
    "Results in updating the variable \'telnet_timeout_standard\' in NVRAM so\r\n"
    "that from next startup, timeout period for disconnecting Telnet when there\r\n"
    "is no keyboard activity (but there is a continuous display on) is going to\r\n"
    "be 60 seconds.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_EVENT_DISPLAY_LVL_ID,
    "event_display_level",
    (PARAM_VAL_RULES *)&Event_disp_level_nvram_vals[0],
    (sizeof(Event_disp_level_nvram_vals) / sizeof(Event_disp_level_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'event_display_level\' variable into NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup event_display_level warning\r\n"
    "Results in updating the variable \'event_display_level\' in NVRAM so\r\n"
    "that from next startup, events from \'warning\' level and up will be\r\n"
    "displayed on the screen.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_EVENT_NVRAM_LVL_ID,
    "event_nvram_level",
    (PARAM_VAL_RULES *)&Event_nvram_level_nvram_vals[0],
    (sizeof(Event_nvram_level_nvram_vals) / sizeof(Event_nvram_level_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'event_nvram_level\' variable into NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup event_nvram_level error\r\n"
    "Results in updating the variable \'event_nvram_level\' in NVRAM so\r\n"
    "that from next startup, events from \'error\' level and up will be\r\n"
    "stored in non volatile \'log\' file.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_EVENT_SYSLOG_LVL_ID,
    "event_syslog_level",
    (PARAM_VAL_RULES *)&Event_syslog_level_nvram_vals[0],
    (sizeof(Event_syslog_level_nvram_vals) / sizeof(Event_syslog_level_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'event_syslog_level\' variable into NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup event_syslog_level error\r\n"
    "Results in updating the variable \'event_syslog_level\' in NVRAM so\r\n"
    "that from next startup, events from \'error\' level and up will be\r\n"
    "sent to a syslog server.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_ACTIVATE_DPSS_ID,
    "activate_dpss",
    (PARAM_VAL_RULES *)&Activate_dpss_nvram_vals[0],
    (sizeof(Activate_dpss_nvram_vals) / sizeof(Activate_dpss_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'activate_dpss\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup activate_dpss off\r\n"
    "Results in updating the control variable \'activate_dpss\' in NVRAM so that\r\n"
    "next startup, no attempt will be made to initialize Marvell's DPSS and download\r\n"
    "configuration file.\r\n"
    "On SOC_SAND_FAP20V system this is 'activate application' indicator.\n\r",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_USR_APP_FLAVOR_ID,
    "usr_app_flavor",
    (PARAM_VAL_RULES *)&Usr_app_flavor_nvram_vals[0],
    (sizeof(Usr_app_flavor_nvram_vals) / sizeof(Usr_app_flavor_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'activate_dpss\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup activate_dpss off\r\n"
    "Results in updating the control variable \'activate_dpss\' in NVRAM so that\r\n"
    "next startup, no attempt will be made to initialize Marvell's DPSS and download\r\n"
    "configuration file.\r\n"
    "On SOC_SAND_FAP20V system this is 'activate application' indicator.\n\r",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_DPSS_DISPLAY_LEVEL_ID,
    "dpss_display_level",
    (PARAM_VAL_RULES *)&Dpss_display_level_nvram_vals[0],
    (sizeof(Dpss_display_level_nvram_vals) / sizeof(Dpss_display_level_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'dpss_display_level\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup dpss_display_level 3\r\n"
    "Results in updating the control variable \'dpss_display_level\' in NVRAM so that\r\n"
    "next startup, various display options of Marvell's DPSS are activated. Actual\r\n"
    "meaning of each value is still TBD but the lower the value the less the amount\r\n"
    "of display\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_RUN_UI_STARTUP_SCRIPT_ID,
    "run_cli_script_at_startup",
    (PARAM_VAL_RULES *)&Activate_dpss_nvram_vals[0],
    (sizeof(Activate_dpss_nvram_vals) / sizeof(Activate_dpss_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'run_cli_script_at_startup\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup run_cli_script_at_startup off\r\n"
    "Results in updating the control variable \'run_cli_script_at_startup\' in NVRAM so that\r\n"
    "next startup, no attempt will be made to run the cli default startup script\r\n"
    "configuration file.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_DPSS_LOAD_FROM_FLASH_NOT_TFTP_ID,
    "dpss_load_config_file_from_flash",
    (PARAM_VAL_RULES *)&Activate_dpss_nvram_vals[0],
    (sizeof(Activate_dpss_nvram_vals) / sizeof(Activate_dpss_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'dpss_load_config_file_from_flash\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup dpss_load_config_file_from_flash off\r\n"
    "Results in updating the control variable \'dpss_load_config_file_from_flash\' in NVRAM so that\r\n"
    "next startup, dpss config file (dune_rpc.cfg) will be loaded from tftp.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_DPSS_DEBUG_LEVEL_ID,
    "dpss_debug_level",
    (PARAM_VAL_RULES *)&Dpss_debug_level_nvram_vals[0],
    (sizeof(Dpss_debug_level_nvram_vals) / sizeof(Dpss_debug_level_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'dpss_debug_level\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup dpss_debug_level 3\r\n"
    "Results in updating the control variable \'dpss_debug_level\' in NVRAM so that\r\n"
    "next startup, various debug options of Marvell's DPSS are activated. Actual\r\n"
    "meaning of each value is still TBD but the lower the value the less the level\r\n"
    "of debug\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_STARTUP_DEMO_MODE_ID,
    "dpss_start_application_in_demo_mode",
    (PARAM_VAL_RULES *)&Dpss_start_demo_mode_nvram_vals[0],
    (sizeof(Dpss_start_demo_mode_nvram_vals) / sizeof(Dpss_start_demo_mode_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'start_app_in_demo_mode\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup dpss_start_application_in_demo_mode negev_demo\r\n"
    "Results in updating the control variable \'start_app_in_demo_mode\' in NVRAM so that\r\n"
    "next startup, the application will load as the negev demo.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_FAP10M_RUN_MODE_ID,
    "fap10m_run_mode",
    (PARAM_VAL_RULES *)&Nvram_fap10m_run_mode_vals[0],
    (sizeof(Nvram_fap10m_run_mode_vals) / sizeof(Nvram_fap10m_run_mode_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'fap10m_run_mode\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup fap10m_run_mode eci_access\r\n"
    "Results in updating the control variable \'fap10m_run_mode\' in NVRAM so that\r\n"
    "next startup, the SOC_SAND_FAP10M driver will access the driver via the ECI.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_TEST_AT_STARTUP_ID,
    "test_at_startup",
    (PARAM_VAL_RULES *)&Test_at_startup_nvram_vals[0],
    (sizeof(Test_at_startup_nvram_vals) / sizeof(Test_at_startup_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'test_at_startup\' bit map into NVRAM. 0x01=Test IRQ6, 0x02=test IRQ0",
    "Examples:\r\n"
    "  nvram write next_startup test_at_startup 0x3\r\n"
    "Results in updating the variable \'test_at_startup\' in NVRAM so\r\n"
    "that from next startup, both standard IRQ injection and NMI will be\r\n"
    "tested at startup\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_DATA_CACHE_ID,
    "data_cache",
    (PARAM_VAL_RULES *)&Data_cache_nvram_vals[0],
    (sizeof(Data_cache_nvram_vals) / sizeof(Data_cache_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'data_cache\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup data_cache off\r\n"
    "Results in updating the control variable \'data_cache\' in NVRAM so\r\n"
    "that next startup, data cache will not be activated.\r\n"
  "Recommanded to be \'on\'.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_INST_CACHE_ID,
    "instruction_cache",
    (PARAM_VAL_RULES *)&Inst_cache_nvram_vals[0],
    (sizeof(Inst_cache_nvram_vals) / sizeof(Inst_cache_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'instruction_cache\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup instruction_cache off\r\n"
    "Results in updating the control variable \'instruction_cache\' in NVRAM so\r\n"
    "that next startup, instruction cache will not be activated.\r\n"
  "Recommanded to be \'on\'.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_AUTO_FLASH_ID,
    "auto_flash_load",
    (PARAM_VAL_RULES *)&Auto_flash_nvram_vals[0],
    (sizeof(Auto_flash_nvram_vals) / sizeof(Auto_flash_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'auto_flash_load\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup auto_flash_load off\r\n"
    "Results in updating the control variable \'auto_flash_load\' in NVRAM so\r\n"
    "that next startup, boot procedure will not load Flash memory with\r\n"
    "up to date version from downloading host.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_IP_SOURCE_MODE_ID,
    "ip_source_mode",
    (PARAM_VAL_RULES *)&Ip_source_mode_nvram_vals[0],
    (sizeof(Ip_source_mode_nvram_vals) / sizeof(Ip_source_mode_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'ip_source_mode\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup ip_source_mode ip_from_nv\r\n"
    "Results in updating the control variable \'ip_source_mode\' in NVRAM so\r\n"
    "that next startup, boot procedure will take the ip address from NVRAM",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_CLK_MIRROR_DATA_ID,
    "clk_mirror_data",
    (PARAM_VAL_RULES *)&Pll_vals[0],
    (sizeof(Pll_vals) / sizeof(Pll_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'clk_mirror_data\' variable contained in NVRAM block.",
    "",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_NVRAM_TCP_TIMEOUT_ID,
    "tcp_timeout",
    (PARAM_VAL_RULES *)&Tcp_timeout_nvram_vals[0],
    (sizeof(Tcp_timeout_nvram_vals) / sizeof(Tcp_timeout_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'tcp_timeout\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup tcp_timeout 5\r\n"
    "Results in updating the control variable \'tcp_timeout\' in NVRAM so that\r\n"
    "next startup, timeout on TCP connections will be 5 seconds. Note that this\r\n"
    "is the \'keep-alive\' timeout. Keep-alive probes will be sent \'tcp_retries\'\r\n"
    "times before connection is dropped.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_TCP_RETRIES_ID,
    "tcp_retries",
    (PARAM_VAL_RULES *)&Tcp_retries_nvram_vals[0],
    (sizeof(Tcp_retries_nvram_vals) / sizeof(Tcp_retries_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'tcp_retries\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup tcp_retries 5\r\n"
    "Results in updating the control variable \'tcp_retries\' in NVRAM so that\r\n"
    "next startup, number or retries on TCP connections will be 5. Note that this\r\n"
    "is the \'keep-alive\' counter. Keep-alive probes will be sent \'tcp_retries\'\r\n"
    "times before connection is dropped.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_LOAD_METHOD_ID,
    "application_load_method",
    (PARAM_VAL_RULES *)&Load_method_nvram_vals[0],
    (sizeof(Load_method_nvram_vals) / sizeof(Load_method_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'application_load_method\' variable contained in NVRAM or runtime blocks",
    "Examples:\r\n"
    "  nvram write next_startup application_load_method newest\r\n"
    "Results in updating the control variable \'application_load_method\'\r\n"
    "in NVRAM so that next startup, boot procedure will look for the most\r\n"
    "up to date version (in Flash and in downloading host) and will run it.\r\n"
  "The value of \'Src of applic.\'(source_file), should be (0=host) to \r\n"
  "allow the reading of the version at the downloading host.\r\n",
    "",

    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_KERNEL_STARTUP_ID,
    "shell_startup",
    (PARAM_VAL_RULES *)&Kernel_startup_nvram_vals[0],
    (sizeof(Kernel_startup_nvram_vals) / sizeof(Kernel_startup_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'shell_startup\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup shell_startup on\r\n"
    "Results in updating the control variable \'shell_startup in\'\r\n"
    "NVRAM so that next startup, boot procedure will start the\r\n"
    "operating system shell rather than running the application,\r\n"
    "and will revert to application if user does not indicate\r\n"
    "permanent shell is required..\r\n"
    "This option is for debug mode (to allow host debugger communication).\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_APP_SOURCE_ID,
    "source_file",
    (PARAM_VAL_RULES *)&App_source_file_nvram_vals[0],
    (sizeof(App_source_file_nvram_vals) / sizeof(App_source_file_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'source_file\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup source_file 1\r\n"
    "Results in updating the control variable \'source_file\' in NVRAM\r\n"
    "so that next startup, boot procedure will first look for application\r\n"
    "source file on file no. \'1\' (first) of the Flash files.\r\n"
    "Maximal value for value of \'source file\' depends on currently\r\n"
    "implemented number of Flash files.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_CONSOLE_BAUD_ID,
    "console_baud_rate",
    (PARAM_VAL_RULES *)&Console_baud_nvram_vals[0],
    (sizeof(Console_baud_nvram_vals) / sizeof(Console_baud_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'console_baud_rate\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write run_time console_baud_rate b_38400\r\n"
    "Results in immediately updating the local console communication speed\r\n"
    "to a baud rate of 38400 bits per second. Note that console user must also\r\n"
    "change speed accordingly.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_LOC_IP_ADDR_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&Ip_address_nvram_vals[0],
    (sizeof(Ip_address_nvram_vals) / sizeof(Ip_address_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'ip_address\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup ip_address 192.168.1.90\r\n"
    "Results in changing this station\'s IP address (on the Ethernet port)\r\n"
    "to \'192.168.1.90\' (in NVRAM) so that, from next startup on, this will\r\n"
    "be it\'s IP address.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_LOC_IP_MASK_ID,
    "ip_mask",
    (PARAM_VAL_RULES *)&Ip_mask_nvram_vals[0],
    (sizeof(Ip_mask_nvram_vals) / sizeof(Ip_mask_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'ip_mask\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup ip_mask 0xFFFF0000\r\n"
    "Results in changing this station\'s IP mask (on the Ethernet port)\r\n"
    "to \'0xFFFF0000\' (in NVRAM) so that, from next startup on, this will\r\n"
    "be it\'s IP mask.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_GATEWAY_ADDR_ID,
    "default_gateway_address",
    (PARAM_VAL_RULES *)&Gw_address_nvram_vals[0],
    (sizeof(Gw_address_nvram_vals) / sizeof(Gw_address_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'default_gateway_address\' variable contained in NVRAM or\r\n"
    "  runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup default_gateway_address 192.168.1.220\r\n"
    "Results in changing this station\'s default gateway IP address to\r\n"
    "\'192.168.1.220\' (in NVRAM) so that, from next startup on, this will\r\n"
    "be it\'s default gateway address.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_DLD_HOST_ADDR_ID,
    "downloading_host_address",
    (PARAM_VAL_RULES *)&Dld_host_address_nvram_vals[0],
    (sizeof(Dld_host_address_nvram_vals) / sizeof(Dld_host_address_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'downloading_host_address\' variable contained in NVRAM\r\n"
    "  or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup downloading_host_address 192.168.1.200\r\n"
    "Results in changing this station\'s downloading host IP address to\r\n"
    "\'192.168.1.200\' (in NVRAM) so that, from next startup on, this will\r\n"
    "be it\'s downloading host. Downloading host is the station from which\r\n"
    "this station tries to get an updated software version (if instructed to).\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_DLD_FILE_NAME_ID,
    "download_file_name",
    (PARAM_VAL_RULES *)&Dld_file_name_nvram_vals[0],
    (sizeof(Dld_file_name_nvram_vals) / sizeof(Dld_file_name_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'download_file_name\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup download_file_name ver_0127\r\n"
    "Results in changing the file name (as it is expected to be at the host)\r\n"
    "to \'ver_0127\' (in NVRAM) so that, from next startup on, when and if\r\n"
    "software looks for an application file on downloading host, it will look\r\n"
    "for a file named \'ver_0127\'.\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BOARD_DESCRIPTION_ID,
    "cpu_board_description",
    (PARAM_VAL_RULES *)&Board_description_nvram_vals[0],
    (sizeof(Board_description_nvram_vals) / sizeof(Board_description_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'cpu_board_description\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram cpu_board_description \"CPU Mezzanine\"\r\n"
    "Results in updating the board description TEXT field in NVRAM so\r\n"
    "that next startup, boot procedures will display new description.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BOARD_TYPE_ID,
    "cpu_board_catalog_number",
    (PARAM_VAL_RULES *)&Board_type_nvram_vals[0],
    (sizeof(Board_type_nvram_vals) / sizeof(Board_type_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'cpu_board_catalog_number\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram cpu_board_cat_number BRD0012B\r\n"
    "Results in updating the cpu board catalog number TEXT field in NVRAM so\r\n"
    "that next startup, boot procedures will display new catalog number\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_ETH_ADDRESS_ID,
    "ethernet_address",
    (PARAM_VAL_RULES *)&Eth_addr_nvram_vals[0],
    (sizeof(Eth_addr_nvram_vals) / sizeof(Eth_addr_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'ethernet_address\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram ethernet_address 0x15\r\n"
    "Results in updating the board\'s Ethernet address (48 bits) in\r\n"
    "NVRAM so that next startup, boot procedures will use the new address.\r\n"
    "NOTE: Only the LS 12 bits may be changed so that the final address\r\n"
    "is going to be 0x 0050C2 15Bxxx where xxx goes from 0x000 to 0xFFF.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BOARD_HW_VERSION_ID,
    "cpu_board_hw_version",
    (PARAM_VAL_RULES *)&Board_version_nvram_vals[0],
    (sizeof(Board_version_nvram_vals) / sizeof(Board_version_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'cpu_board_hw_version\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram cpu_board_hw_version 301\r\n"
    "Results in updating the board\'s hardware version identifier (three digits)\r\n"
    "in NVRAM so that next startup, all procedures will use the new serial\r\n"
    "number. When displayed, this hardware version is presented as \'v3.01\'.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BOARD_SERIAL_NUM_ID,
    "cpu_board_serial_number",
    (PARAM_VAL_RULES *)&Board_serial_num_nvram_vals[0],
    (sizeof(Board_serial_num_nvram_vals) / sizeof(Board_serial_num_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'cpu_board_serial_number\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram cpu_board_serial_number 20\r\n"
    "Results in updating the board\'s serial number (four digits) in\r\n"
    "NVRAM so that next startup, all procedures will use the new serial\r\n"
    "number.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BLOCK_ID,
    "block",
    (PARAM_VAL_RULES *)&Block_nvram_vals[0],
    (sizeof(Block_nvram_vals) / sizeof(Block_nvram_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Block_nvram_default,
    0,0,0,0,0,0,0,
    "Identifying number of NVRAM block (starting from \'1\').",
    "Examples:\r\n"
    "  nvram read block 1\r\n"
    "Results in displaying all variables contained in NVRAM block no. 1.\r\n",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_L_DEFAULTS_ID,
    "load_defaults",
    (PARAM_VAL_RULES *)&L_defaults_nvram_vals[0],
    (sizeof(L_defaults_nvram_vals) / sizeof(L_defaults_nvram_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Load the specified NVRAM block(s) by default values (no runtime\r\n"
    "  variables update).",
    "Examples:\r\n"
    "  nvram load_defaults block_2\r\n"
    "Results in loading default values in NVRAM block no. 2.\r\n"
    "  nvram load_defaults all\r\n"
    "Results in loading default values in all NVRAM blocks,\r\n"
    "except for bloak 1 (first on parallel EEPROM) and block 3\r\n"
    "(first on serial EEPROM), which contains the 48-bits Ethernet\r\n"
    "address. Note that runtime variables are not changed.\r\n",
    "",
    {BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BASE_REGISTER_04_ID,
    "base_register_04",
    (PARAM_VAL_RULES *)&base_and_option_register_nvram_vals[0],
    (sizeof(base_and_option_register_nvram_vals) / sizeof(base_and_option_register_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'base_register_04\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup base_register_04 0x40007001\r\n"
    "Results in updating the variable \'base_register_04\' in NVRAM so\r\n"
    "that from next startup, the value at the CPUs base register 04 will be \r\n"
    "\'base_register_04\' ontrolling (with \'option_register_04\' some chip selects\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_BASE_REGISTER_05_ID,
    "base_register_05",
    (PARAM_VAL_RULES *)&base_and_option_register_nvram_vals[0],
    (sizeof(base_and_option_register_nvram_vals) / sizeof(base_and_option_register_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'base_register_05\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup base_register_05 0x40007001\r\n"
    "Results in updating the variable \'base_register_05\' in NVRAM so\r\n"
    "that from next startup, the value at the CPUs base register 05 will be \r\n"
    "\'base_register_05\' ontrolling (with \'option_register_05\' some chip selects\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_OPTION_REGISTER_04_ID,
    "option_register_04",
    (PARAM_VAL_RULES *)&base_and_option_register_nvram_vals[0],
    (sizeof(base_and_option_register_nvram_vals) / sizeof(base_and_option_register_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'option_register_04\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup option_register_04 0xFF8007FA\r\n"
    "Results in updating the variable \'option_register_04\' in NVRAM so\r\n"
    "that from next startup, the value at the CPUs option register 04 will be \r\n"
    "\'option_register_04\' ontrolling (with \'base_register_04\' some chip selects\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_OPTION_REGISTER_05_ID,
    "option_register_05",
    (PARAM_VAL_RULES *)&base_and_option_register_nvram_vals[0],
    (sizeof(base_and_option_register_nvram_vals) / sizeof(base_and_option_register_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'option_register_05\' variable contained in NVRAM or runtime blocks.",
    "Examples:\r\n"
    "  nvram write next_startup option_register_05 0xFF8007FA\r\n"
    "Results in updating the variable \'option_register_05\' in NVRAM so\r\n"
    "that from next startup, the value at the CPUs base register 04 will be \r\n"
    "\'option_register_05\' ontrolling (with \'base_register_05\' some chip selects\r\n",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * Items related to host board (carrying CPU board).
   * {
   */
  {
    PARAM_NVRAM_HOST_BOARD_TYPE_ID,
    "host_board_type",
    (PARAM_VAL_RULES *)&Host_board_type_nvram_vals[0],
    (sizeof(Host_board_type_nvram_vals) / sizeof(Host_board_type_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_board_type\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_type FABRIC_CARD_01\r\n"
    "Results in updating the board type numeric field in NVRAM so\r\n"
    "that next startup, boot procedures will use the new type.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID,
    "host_board_catalog_number",
    (PARAM_VAL_RULES *)&Host_board_cat_num_nvram_vals[0],
    (sizeof(Host_board_cat_num_nvram_vals) / sizeof(Host_board_cat_num_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_board_catalog_number\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_catalog_number \"BRD0002A\"\r\n"
    "Results in updating the board catalog number TEXT field in NVRAM so\r\n"
    "that next startup, boot procedures will display new catalog number.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_BOARD_DESC_ID,
    "host_board_description",
    (PARAM_VAL_RULES *)&Host_board_description_nvram_vals[0],
    (sizeof(Host_board_description_nvram_vals) /
            sizeof(Host_board_description_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_board_description\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_description \"host_board_description\"\r\n"
    "Results in updating the board description TEXT field in NVRAM so\r\n"
    "that next startup, boot procedures will display new description.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_BOARD_VER_ID,
    "host_board_version",
    (PARAM_VAL_RULES *)&Host_board_version_nvram_vals[0],
    (sizeof(Host_board_version_nvram_vals) / sizeof(Host_board_version_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'hoat_board_version\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_version v3.01\r\n"
    "Results in updating the host board\'s hardware version identifier (three digits)\r\n"
    "in NVRAM so that next startup, all procedures will use the new version\r\n"
    "number. When displayed, this hardware version is presented as \'v3.01\'.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_BOARD_SN_ID,
    "host_board_serial_number",
    (PARAM_VAL_RULES *)&Board_serial_num_nvram_vals[0],
    (sizeof(Board_serial_num_nvram_vals) / sizeof(Board_serial_num_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_board_serial_number\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_serial_number 20\r\n"
    "Results in updating the host board\'s serial number (four digits) in\r\n"
    "NVRAM so that next startup, all procedures will use the new serial\r\n"
    "number.\r\n",
    "",
    {BIT(1)|BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID,
    "host_board_fap_pp_if_clk",
    (PARAM_VAL_RULES *)&Board_fap_pp_if_clk_nvram_vals[0],
    (sizeof(Board_fap_pp_if_clk_nvram_vals) / sizeof(Board_fap_pp_if_clk_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_board_fap_pp_if_clk\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_fap_pp_if_clk 250\r\n"
    "Results in updating the host board\'s FAP-PP interface clock in\r\n"
    "NVRAM so that next startup, all procedures will use the new serial\r\n"
    "number.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID,
    "host_board_front_card_type",
    (PARAM_VAL_RULES *)&Front_end_host_board_type_nvram_vals[0],
    (sizeof(Front_end_host_board_type_nvram_vals) /
            sizeof(Front_end_host_board_type_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_board_front_card_type\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_front_card_type X10\r\n"
    "Results in updating the host board\'s front card type to X10.\r\n"
    "The parameter only has affect when working with GFA, and with front\r\n"
    "card that is not produced by Dune Networks.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID,
    "host_board_fap_use_eeprom_vals",
    (PARAM_VAL_RULES *)&Board_fap_use_eeprom_vals_nvram_vals[0],
    (sizeof(Board_fap_use_eeprom_vals_nvram_vals) /
            sizeof(Board_fap_use_eeprom_vals_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_board_fap_use_eeprom\' variable contained in NVRAM block.",
    "Examples:\r\n"
    "  nvram host_board_fap_use_eeprom 1\r\n"
    "Results in updating the host board\'s FAP indicator\r\n"
    "NVRAM so that next startup, all procedures will use the new serial\r\n"
    "number.\r\n"
    "Indicator 1: Software will not load the FAP's HW configuration values.\n\r"
    "             Namelly, Start init and DFCDLs.\n\r"
    "Indicator 0: Software will load ALL FAP's HW configuration values.\n\r",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * }
   */
  /*
   * {
   * FrontEnd Board
   * Note: Numbers of NV blocks need to be updated if the
   * following definitions are changed:
   *   BLOCK_300_EQUIVALENT
   *   BLOCK_301_EQUIVALENT
   */
  {
    PARAM_NVRAM_FRONT_BOARD_TYPE_ID,
    "front_board_type",
    (PARAM_VAL_RULES *)&Front_board_type_nvram_vals[0],
    (sizeof(Front_board_type_nvram_vals) /
            sizeof(Front_board_type_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'front_board_type\' variable into NVRAM block 12.",
    "Examples:\r\n"
    "  nvram board_params front_board_type front_card_type_ftg\r\n"
    "Results in updating the front board type numeric field in NVRAM\r\n"
    "so that next startup, all procedures will use the new type.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID,
    "front_board_catalog_number",
    (PARAM_VAL_RULES *)&Front_board_cat_num_nvram_vals[0],
    (sizeof(Front_board_cat_num_nvram_vals) /
                  sizeof(Front_board_cat_num_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'front_board_catalog_number\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params front_board_catalog_number \"X_0002\"\r\n"
    "Results in updating front board catalog number TEXT field in NVRAM so\r\n"
    "that next startup, all procedures will display new catalog number.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_FRONT_BOARD_DESC_ID,
    "front_board_description",
    (PARAM_VAL_RULES *)&Front_board_description_nvram_vals[0],
    (sizeof(Front_board_description_nvram_vals) /
            sizeof(Front_board_description_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'front_board_description\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params front_board_description \"front board description\"\r\n"
    "Results in updating front board description TEXT field in NVRAM so\r\n"
    "that next startup, all procedures will display new description.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_FRONT_BOARD_VER_ID,
    "front_board_version",
    (PARAM_VAL_RULES *)&Front_board_version_nvram_vals[0],
    (sizeof(Front_board_version_nvram_vals) /
                  sizeof(Front_board_version_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'front_board_version\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params front_board_version v3.01\r\n"
    "Results in updating front host board\'s hardware version identifier (three\r\n"
    "digits) in NVRAM so that next startup, all procedures will use the new version\r\n"
    "number. When displayed, this hardware version is presented as \'v3.01\'.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_FRONT_BOARD_SN_ID,
    "front_board_serial_number",
    (PARAM_VAL_RULES *)&Front_board_serial_num_nvram_vals[0],
    (sizeof(Front_board_serial_num_nvram_vals) /
                  sizeof(Front_board_serial_num_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'front_board_serial_number\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params front_board_serial_number 20\r\n"
    "Results in updating the front board\'s serial number (four digits) in\r\n"
    "NVRAM so that next startup, all procedures will use the new serial\r\n"
    "number.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * END FrontEnd Board
   * }
   */

  /*
   * {
   * Host_db Board
   * Note: Numbers of NV blocks need to be updated if the
   * following definitions are changed:
   *   BLOCK_400_EQUIVALENT
   *   BLOCK_401_EQUIVALENT
   */
  {
    PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID,
    "host_db_board_type",
    (PARAM_VAL_RULES *)&Host_db_board_type_nvram_vals[0],
    (sizeof(Host_db_board_type_nvram_vals) /
            sizeof(Host_db_board_type_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_db_board_type\' variable into NVRAM block 12.",
    "Examples:\r\n"
    "  nvram board_params host_db_board_type host_db_card_type_ftg\r\n"
    "Results in updating the host_db board type numeric field in NVRAM\r\n"
    "so that next startup, all procedures will use the new type.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID,
    "host_db_board_catalog_number",
    (PARAM_VAL_RULES *)&Host_db_board_cat_num_nvram_vals[0],
    (sizeof(Host_db_board_cat_num_nvram_vals) /
                  sizeof(Host_db_board_cat_num_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_db_board_catalog_number\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params host_db_board_catalog_number \"X_0002\"\r\n"
    "Results in updating host_db board catalog number TEXT field in NVRAM so\r\n"
    "that next startup, all procedures will display new catalog number.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_DB_BOARD_DESC_ID,
    "host_db_board_description",
    (PARAM_VAL_RULES *)&Host_db_board_description_nvram_vals[0],
    (sizeof(Host_db_board_description_nvram_vals) /
            sizeof(Host_db_board_description_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_db_board_description\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params host_db_board_description \"host_db board description\"\r\n"
    "Results in updating host_db board description TEXT field in NVRAM so\r\n"
    "that next startup, all procedures will display new description.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_DB_BOARD_VER_ID,
    "host_db_board_version",
    (PARAM_VAL_RULES *)&Host_db_board_version_nvram_vals[0],
    (sizeof(Host_db_board_version_nvram_vals) /
                  sizeof(Host_db_board_version_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_db_board_version\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params host_db_board_version v3.01\r\n"
    "Results in updating host_db host board\'s hardware version identifier (three\r\n"
    "digits) in NVRAM so that next startup, all procedures will use the new version\r\n"
    "number. When displayed, this hardware version is presented as \'v3.01\'.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_NVRAM_HOST_DB_BOARD_SN_ID,
    "host_db_board_serial_number",
    (PARAM_VAL_RULES *)&Host_db_board_serial_num_nvram_vals[0],
    (sizeof(Host_db_board_serial_num_nvram_vals) /
                  sizeof(Host_db_board_serial_num_nvram_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write \'host_db_board_serial_number\' variable into NVRAM block 13.",
    "Examples:\r\n"
    "  nvram board_params host_db_board_serial_number 20\r\n"
    "Results in updating the host_db board\'s serial number (four digits) in\r\n"
    "NVRAM so that next startup, all procedures will use the new serial\r\n"
    "number.\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * END Host_db Board
   * }
   */
#endif /*} LINUX */
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  General_download_host_ip
*TYPE: BUFFER
*DATE: 1/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PARAM_GENERAL_DOWNLOAD_HOST_IP_ID" IN THE
*  CONTEXT OF SUBJECT 'GENERAL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     General_download_host_ip[]
#ifdef INIT
   =
{
  {
    VAL_IP,
    SYMB_NAME_IP,
    {
      {UI_DEMY_VAL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  General_remark_vals
*TYPE: BUFFER
*DATE: 11/NOV/2003
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "PARAM_GENERAL_REMARK_ID" IN THE
*  CONTEXT OF SUBJECT 'GENERAL'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    char *symb_val -
*      ASCII presentation of the symbolic value.
*      Note: Must be small letters only!
*      For numeric variables:
*      This is a dummy variable. Enables searching
*      array of 'PARAM_VAL_RULES' for name (string)
*      matching without special case for numeric
*      variable.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     General_remark_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        MAX_CHARS_ON_SCREEN_LINE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    Free text to enter just for display purposes.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  General_params
*TYPE: BUFFER
*DATE: 22/MAR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "GENERAL".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      THIS FIELD IS AUTOMATICALLY FILLED IN BY THE SYSTEM
*      and does NOT need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     General_params[]
#ifdef INIT
   =
{
  {
    PARAM_GENERAL_MULTI_MEM_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Data_mem_vals[0],
    (sizeof(Data_mem_vals) / sizeof(Data_mem_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of times to repeat memory access for each specified address",
    "Examples:\r\n"
    "  general multi_mem write address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 1 2 data 1 5 6 7\r\n"
    "Results in writing one byte (value \'1\') into address 0x50100000, one short\r\n"
    "(value \'5\') into address 0x500A0012 and two long word (values \'6\', \'7\')\r\n"
    "into addresses 0x500A0028 and 0x500A002C\r\n",
    "",
    {BIT(27)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_MULTI_MEM_REPEAT_ID,
    "repeat",
    (PARAM_VAL_RULES *)&Data_mem_vals[0],
    (sizeof(Data_mem_vals) / sizeof(Data_mem_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of times to repeat memory access for each specified address",
    "Examples:\r\n"
    "  general multi_mem read address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 5 6\r\n"
    "Results in reading one byte from address 0x50100000, five shorts from address\r\n"
    "0x500A0012 and 6 long words from address 0x500A0028\r\n"
    "  general multi_mem write address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 1 2 data 1 5 6 7\r\n"
    "Results in writing one byte (value \'1\') into address 0x50100000, one short\r\n"
    "(value \'5\') into address 0x500A0012 and two long word (values \'6\', \'7\')\r\n"
    "into addresses 0x500A0028 and 0x500A002C\r\n",
    "",
    {BIT(26) | BIT(27)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_MULTI_MEM_FORMAT_ID,
    "format",
    (PARAM_VAL_RULES *)&Data_mem_vals[0],
    (sizeof(Data_mem_vals) / sizeof(Data_mem_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Format of memory access (1 = char, 2 = short, 4 = long, other = char)",
    "Examples:\r\n"
    "  general multi_mem read address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 5 6\r\n"
    "Results in reading one byte from address 0x50100000, five shorts from address\r\n"
    "0x500A0012 and 6 long words from address 0x500A0028\r\n"
    "  general multi_mem write address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 1 2 data 1 5 6 7\r\n"
    "Results in writing one byte (value \'1\') into address 0x50100000, one short\r\n"
    "(value \'5\') into address 0x500A0012 and two long word (values \'6\', \'7\')\r\n"
    "into addresses 0x500A0028 and 0x500A002C\r\n",
    "",
    {BIT(26) | BIT(27)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_MULTI_MEM_ADDRESS_ID,
    "address",
    (PARAM_VAL_RULES *)&Data_mem_vals[0],
    (sizeof(Data_mem_vals) / sizeof(Data_mem_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "List of addresses to use for read/write from/into memory methods.",
    "Examples:\r\n"
    "  general multi_mem read address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 5 6\r\n"
    "Results in reading one byte from address 0x50100000, five shorts from address\r\n"
    "0x500A0012 and 6 long words from address 0x500A0028\r\n"
    "  general multi_mem write address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 1 2 data 1 5 6 7\r\n"
    "Results in writing one byte (value \'1\') into address 0x50100000, one short\r\n"
    "(value \'5\') into address 0x500A0012 and two long word (values \'6\', \'7\')\r\n"
    "into addresses 0x500A0028 and 0x500A002C\r\n",
    "",
    {BIT(26) | BIT(27)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_MULTI_MEM_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out multi-memory_access-related operations (read and write)\r\n",
    "Example:\r\n"
    "  general multi_mem write address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 1 2 data 1 5 6 7\r\n"
    "Results in writing one byte (value \'1\') into address 0x50100000, one short\r\n"
    "(value \'5\') into address 0x500A0012 and two long word (values \'6\', \'7\')\r\n"
    "into addresses 0x500A0028 and 0x500A002C\r\n",
    "",
    {BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_MULTI_MEM_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out multi-memory_access-related operations (read and write)\r\n",
    "Example:\r\n"
    "  general multi_mem read address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 5 6\r\n"
    "Results in reading one byte from address 0x50100000, five shorts from address\r\n"
    "0x500A0012 and 6 long words from address 0x500A0028\r\n",
    "",
    {BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_MULTI_MEM_ID,
    "multi_mem",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out multi-memory_access-related operations (read and write)\r\n",
    "Examples:\r\n"
    "  general multi_mem read address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 5 6\r\n"
    "Results in reading one byte from address 0x50100000, five shorts from address\r\n"
    "0x500A0012 and 6 long words from address 0x500A0028\r\n"
    "  general multi_mem write address 0x50100000 0x500A0012 0x500A0028 format 1 2 4\r\n"
    "  repeat 1 1 2 data 1 5 6 7\r\n"
    "Results in writing one byte (value \'1\') into address 0x50100000, one short\r\n"
    "(value \'5\') into address 0x500A0012 and two long word (values \'6\', \'7\')\r\n"
    "into addresses 0x500A0028 and 0x500A002C\r\n",
    "",
    {BIT(26) | BIT(27)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },


  {
    PARAM_GENERAL_HOST_FILE_DELETE_ID,
    "delete",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Delete dcl host-file pair (Remove from liast of destination hosts)\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_ip 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to \'10.0.0.100\'\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(25)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_HOST_FILE_LAST_FILE_ID,
    "last_file",
    (PARAM_VAL_RULES *)&Dcl_file_name_vals[0],
    (sizeof(Dcl_file_name_vals) / sizeof(Dcl_file_name_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Last (in range) file name for \'host_ip\' to use when sending initiated msgs\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_host 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to \'10.0.0.100\'\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(24)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_HOST_FILE_FIRST_FILE_ID,
    "first_file",
    (PARAM_VAL_RULES *)&Dcl_file_name_vals[0],
    (sizeof(Dcl_file_name_vals) / sizeof(Dcl_file_name_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "First (in range) file name for \'host_ip\' to use when sending initiated msgs\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_host 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to \'10.0.0.100\'\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(24)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_HOST_FILE_DEST_HOST_ID,
    "dest_host",
    (PARAM_VAL_RULES *)&Dcl_host_ip_vals[0],
    (sizeof(Dcl_host_ip_vals) / sizeof(Dcl_host_ip_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "IP addr of host to which \'host_ip\' is required to send initiated messages to\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_host 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to \'10.0.0.100\'\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(24)| BIT(25)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_HOST_FILE_HOST_IP_ID,
    "host_ip",
    (PARAM_VAL_RULES *)&Dcl_host_ip_vals[0],
    (sizeof(Dcl_host_ip_vals) / sizeof(Dcl_host_ip_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "IP address of station to send dcl message/command to\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_host 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to \'10.0.0.100\'\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(24) | BIT(25)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_HOST_FILE_SET_ID,
    "set",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set dcl host-file pair (set file range for specified destination host)\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_host 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to \'10.0.0.100\'\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(24)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_HOST_FILE_ID,
    "host_file",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out dcl host-file setup (set file range for specified destination host)\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_host 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to \'10.0.0.100\'\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(24) | BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DCL_ID,
    "dcl",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out dcl-related operations (e.g., host-file setup)\r\n",
    "Examples:\r\n"
    "  general dcl host_file set host_ip 10.0.0.1 dest_host 10.0.0.100\r\n"
    "  first_file 0 last_file 10\r\n"
    "Results in instructing station with IP address \'10.0.0.1\' to use\r\n"
    "specified file range when sending initiated DCL messages to 10.0.0.100\r\n"
    "The file range is [0,10] and is used cyclically.\r\n",
    "",
    {BIT(24) | BIT(25)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_RATE_HOST_IP_ID,
    "host_ip",
    (PARAM_VAL_RULES *)&Fpga_rate_host_ip_vals[0],
    (sizeof(Fpga_rate_host_ip_vals) / sizeof(Fpga_rate_host_ip_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "IP address of controlling host to sending continuous reports to.\r\n",
    "Examples:\r\n"
    "  general fpga_rate fpga_a start collection_rate 5 continuous_mode on\r\n"
    "  report_rate 20 host_ip 10.0.0.100\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is continuously sent to host (at 10.0.0.100)\r\n"
    "at a rate of one report every 20 \'collections\' (i.e., every 100 ms).\r\n",
    "",
    {BIT(21)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

#if LINK_PSS_LIBRARIES
/* { */

  {
    PARAM_GENERAL_FPGA_RATE_REPORT_RATE_ID,
    "report_rate",
    (PARAM_VAL_RULES *)&Fpga_rate_report_rate_vals[0],
    (sizeof(Fpga_rate_report_rate_vals) / sizeof(Fpga_rate_report_rate_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Rate of sending continuous reports to controlling host. In collection periods.\r\n",
    "Examples:\r\n"
    "  general fpga_rate fpga_a start collection_rate 5 continuous_mode on\r\n"
    "  report_rate 20 host_ip 10.0.0.100\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is continuously sent to host (at 10.0.0.100)\r\n"
    "at a rate of one report every 20 \'collections\' (i.e., every 100 ms).\r\n",
    "",
    {BIT(21)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } LINK_PSS_LIBRARIES */

#if !(defined(LINUX) || defined(UNIX))
/* { */
  {
    PARAM_GENERAL_FPGA_RATE_CONTINUOUS_ID,
    "continuous_mode",
    (PARAM_VAL_RULES *)&Fpga_rate_continuous_mode_vals[0],
    (sizeof(Fpga_rate_continuous_mode_vals) / sizeof(Fpga_rate_continuous_mode_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Fpga_rate_continuous_mode_default,
    0,0,0,0,0,0,0,
    "Carry out rate collection from FTG and send continuous reports to host\r\n",
    "Examples:\r\n"
    "  general fpga_rate fpga_a start collection_rate 5 continuous_mode on\r\n"
    "  report_rate 20 host_ip 10.0.0.100\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is continuously sent to host (at 10.0.0.100)\r\n"
    "at a rate of one report every 20 \'collections\' (i.e., every 100 ms)\r\n",
    "",
    {BIT(21)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_RATE_COLLECTION_RATE_ID,
    "collection_rate",
    (PARAM_VAL_RULES *)&Fpga_rate_collection_rate_vals[0],
    (sizeof(Fpga_rate_collection_rate_vals) / sizeof(Fpga_rate_collection_rate_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set rate of collection from FTG. In ms.\r\n",
    "Examples:\r\n"
    "  general fpga_rate fpga_a start collection_rate 5 continuous_mode on\r\n"
    "  report_rate 20 host_ip 10.0.0.100\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is continuously sent to host (at 10.0.0.100)\r\n"
    "at a rate of one report every 20 \'collections\' (i.e., every 100 ms)\r\n"
    "  general fpga_rate fpga_a start collection_rate 5\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is held by the system until polled for by a\r\n"
    "controlling host.\r\n",
    "",
    {BIT(21)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_RATE_STOP_ID,
    "stop",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Stop rate collection from FTG (Fpga Traffic Generator)\r\n",
    "Examples:\r\n"
    "  general fpga_rate fpga_a stop\r\n"
    "Results in stopping and active real time rate collection session.\r\n",
    "",
    {BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_RATE_START_ID,
    "start",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Start rate collection from FTG (Fpga Traffic Generator)\r\n",
    "Examples:\r\n"
    "  general fpga_rate fpga_a start collection_rate 5 continuous_mode on\r\n"
    "  report_rate 20 host_ip 10.0.0.100\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is continuously sent to host (at 10.0.0.100)\r\n"
    "at a rate of one report every 20 \'collections\' (i.e., every 100 ms)\r\n"
    "  general fpga_rate fpga_a start collection_rate 5\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is held by the system until polled for by a\r\n"
    "controlling host.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_RATE_GET_CURRENT_ID,
    "get_current",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get data so far collected by rate collection mechanism (from FTG)\r\n",
    "Example:\r\n"
    "  general fpga_rate fpga_a get_current\r\n"
    "Results in getting, on response, results (data) of real time rate collection\r\n"
    "as specified on previous \'start\' command.\r\n",
    "",
    {BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_RATE_ID,
    "fpga_rate",
    (PARAM_VAL_RULES *)&Fpga_identifier_vals[0],
    (sizeof(Fpga_identifier_vals) / sizeof(Fpga_identifier_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out rate collection from FTG (Fpga Traffic Generator)\r\n",
    "Examples:\r\n"
    "  general fpga_rate fpga_a start collection_rate 5 continuous_mode on\r\n"
    "  report_rate 20 host_ip 10.0.0.100\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is continuously sent to host (at 10.0.0.100)\r\n"
    "at a rate of one report every 20 \'collections\' (i.e., every 100 ms)\r\n"
    "  general fpga_rate fpga_a start collection_rate 5\r\n"
    "Results in starting real time rate collection. Information is collected\r\n"
    "from FPGA every 5 ms and is held by the system until polled for by a\r\n"
    "controlling host.\r\n",
    "",
    {BIT(21) | BIT(22) | BIT(23)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Fpga_ind_write_vals[0],
    (sizeof(Fpga_ind_write_vals) / sizeof(Fpga_ind_write_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select indirect action (write) to carry out on FTG.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b write generator sub_block_write egress_pattern\r\n"
    "  element_number 0 data 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16\r\n"
    "Results in writing one block, of 16 long words, into the egress\r\n"
    "pattern table. On each longword, all 32 bits are significant. \r\n"
    "Values (1, 2, 3, ...) are writen to FPGA \'B\'\r\n",
    "",
    {BIT(20)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_TO_ELEMENT_ID,
    "to_element",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select number of table element to start indirect action at.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b read generator sub_block_read calendar_table\r\n"
    "  from_element 0 to_element 0\r\n"
    "Results in display of element no. \'0\', block of 8 long words, from\r\n"
    "the calendar table. On each long word, only the 18 LS bits are\r\n"
    "significant. Values are read from FPGA \'B\'\r\n",
    "",
    {BIT(19)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_FROM_ELEMENT_ID,
    "from_element",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select number of table element to start indirect action at.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b read generator sub_block_read calendar_table\r\n"
    "  from_element 0 to_element 0\r\n"
    "Results in display of element no. \'0\', block of 8 long words, from\r\n"
    "the calendar table. On each long word, only the 18 LS bits are\r\n"
    "significant. Values are read from FPGA \'B\'\r\n",
    "",
    {BIT(19)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_AUTO_CLEAR_ID,
    "auto_clear",
    (PARAM_VAL_RULES *)&Fpga_ind_auto_clear_vals[0],
    (sizeof(Fpga_ind_auto_clear_vals) / sizeof(Fpga_ind_auto_clear_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fpga_ind_auto_clear_default,
    0,0,0,0,0,0,0,
    "Clear every memory location after reading\r\n",
    "Example:\r\n"
    "  general fpga_indirect fpga_b read verifier sub_block_read stream_rate\r\n"
    "  from_element 1 to_element 1 auto_clear on\r\n"
    "Results in display of element no. \'1\', block of 8 long words, from\r\n"
    "the calendar table. On each long word, only the 18 LS bits are\r\n"
    "significant. Values are read from FPGA \'B\'. After reading, element \'1\'\r\n"
    "is filled with \'0\'s so the next time it is read, it will report \r\n"
    "*all-\'0\' values\r\n",
    "",
    {BIT(19)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_WRITE_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Fpga_ind_write_data_vals[0],
    (sizeof(Fpga_ind_write_data_vals) / sizeof(Fpga_ind_write_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Specify data to write into selected table element.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b write generator sub_block_write calendar_table\r\n"
    "  element_number 0 data 1 2 3 4 5 6 7 8\r\n"
    "Results in writing one block, of 8 long words, into the calendar\r\n"
    "table. On each longword, the ls 18 bits are significant. \r\n"
    "Values (1, 2, 3, ..., 8) are writen to FPGA \'B\'\r\n",
    "",
    {BIT(20)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GENERAL_FPGA_IND_ELEMENT_NUMBER_ID,
    "element_number",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select number of table element to carry out indirect action on.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b write generator sub_block_write calendar_table\r\n"
    "  element_number 0 data 1 2 3 4 5 6 7 8\r\n"
    "Results in writing one block, of 8 long words, into the calendar\r\n"
    "table. On each longword, the ls 18 bits are significant. \r\n"
    "Values (1, 2, 3, ..., 8) are writen to FPGA \'B\'\r\n",
    "",
    {BIT(20)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_SUB_BLOCK_WRITE_ID,
    "sub_block_write",
    (PARAM_VAL_RULES *)&Fpga_ind_sub_block_write_vals[0],
    (sizeof(Fpga_ind_sub_block_write_vals) / sizeof(Fpga_ind_sub_block_write_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select FTG sub block to carry indirect action on.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b write generator sub_block_write calendar_table\r\n"
    "  element_number 0 data 1 2 3 4 5 6 7 8\r\n"
    "Results in writing one block, of 8 long words, into the calendar\r\n"
    "table. On each longword, the ls 18 bits are significant. \r\n"
    "Values (1, 2, 3, ..., 8) are writen to FPGA \'B\'\r\n",
    "",
    {BIT(20)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_SUB_BLOCK_READ_ID,
    "sub_block_read",
    (PARAM_VAL_RULES *)&Fpga_ind_sub_block_read_vals[0],
    (sizeof(Fpga_ind_sub_block_read_vals) / sizeof(Fpga_ind_sub_block_read_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select FTG sub block to carry indirect action on.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b read generator sub_block_read calendar_table\r\n"
    "  from_element 0 to_element 0\r\n"
    "Results in display of one block, of 8 long words, from the\r\n"
    "calendar table. On each long word, only the 18 LS bits are\r\n"
    "significant. Values are read from FPGA \'B\'\r\n",
    "",
    {BIT(19)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_IND_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Fpga_ind_read_vals[0],
    (sizeof(Fpga_ind_read_vals) / sizeof(Fpga_ind_read_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select indirect action (read) to carry out on FTG.",
    "Examples:\r\n"
    "  general fpga_indirect fpga_b read generator sub_block_read calendar_table\r\n"
    "  from_element 0 to_element 0\r\n"
    "Results in display of one block, of 8 long words, from the\r\n"
    "calendar table. On each long word, only the 18 LS bits are\r\n"
    "significant. Values are read from FPGA \'B\'\r\n",
    "",
    {BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FPGA_INDIRECT_ID,
    "fpga_indirect",
    (PARAM_VAL_RULES *)&Fpga_identifier_vals[0],
    (sizeof(Fpga_identifier_vals) / sizeof(Fpga_identifier_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Carry out indirect memory access on FTG (Fpga Traffic Generator)\r\n",
    "Examples:\r\n"
    "  general fpga_indirect fpga_a read generator sub_block_read calendar_table\r\n"
    "  from_element 0 to_element 1\r\n"
    "Results in display of two blocks, from the calendar table, each of 8\r\n"
    "long words. On each long word, only the 18 LS bits are significant.\r\n"
    "Values are read from FPGA \'A\'\r\n"
    "  general fpga_indirect fpga_b write generator sub_block_write egress_pattern\r\n"
    "  element_number 0 data 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16\r\n"
    "Results in writing one block, of 16 long words, into the egress\r\n"
    "pattern table. On each longword, all 32 bits are significant. \r\n"
    "Values (1, 2, 3, ...) are writen to FPGA \'B\'\r\n",
    "",
    {BIT(19) | BIT(20)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_AUX_INPUT_ID,
    "aux_input",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Handle Aux-Input related utilities.",
    "Examples:\r\n"
    "  general aux_input print_status\r\n"
    "Results in printing aux input status.\r\n",
    "",
    { BIT(17)| BIT(18)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_PRINT_STATUS_ID,
    "print_status",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print Aux-Input status.",
    "Examples:\r\n"
    "  general aux_input print_status\r\n"
    "Results in printing aux input status.\r\n",
    "",
    { BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_AUX_INPUT_RESTART_ID,
    "restart",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Re-Start Aux-Input module.",
    "Examples:\r\n"
    "  general aux_input restart\r\n"
    "Results in restarting aux input module.\r\n",
    "",
    { BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DEMO_ID,
    "demo",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display info or perform action related to debugging of \'demo\' setup.",
    "Example:\r\n"
    "  general demo display_network\r\n"
    "Results in display of all discovered stations\r\n",
    "",
    {BIT(15) | BIT(16)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DEMO_DISPLAY_NETWORK_ID,
    "display_network",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display full network as discovered under \'demo\' setup.",
    "Example:\r\n"
    "  general demo display_network\r\n"
    "Results in display of all discovered stations\r\n",
    "",
    {BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DEMO_SHOW_FE_LINK_ID,
    "show_fe_link",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display matching FE link given FAP link under specified \'demo\' setup.",
    "Example:\r\n"
    "  general demo show_fe_link crate_type gobi line_card_slot 11\r\n"
    "    fabric_card_slot 17 fap_link 3\r\n"
    "Results in display of FE link (and FE identifier) on fabric card, sitting\r\n"
    "in slot 17, which is connected to link 3 of FAP on line card, sitting\r\n"
    "in slot 11. The whole setup is calculated for a GOBI crate.\r\n",
    "",
    {BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DEMO_CRATE_TYPE_ID,
    "crate_type",
    (PARAM_VAL_RULES *)&Crate_type_demo_vals[0],
    (sizeof(Crate_type_demo_vals) / sizeof(Crate_type_demo_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select crate type to calculate link for.",
    "Example:\r\n"
    "  general demo show_fe_link crate_type gobi line_card_slot 11\r\n"
    "    fabric_card_slot 17 fap_link 3\r\n"
    "Results in display of FE link (and FE identifier) on fabric card, sitting\r\n"
    "in slot 17, which is connected to link 3 of FAP on line card, sitting\r\n"
    "in slot 11. The whole setup is calculated for a GOBI crate.\r\n",
    "",
    {BIT(16)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DEMO_LINE_CARD_SLOT_ID,
    "line_card_slot",
    (PARAM_VAL_RULES *)&Line_card_slot_demo_vals[0],
    (sizeof(Line_card_slot_demo_vals) / sizeof(Line_card_slot_demo_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select slot on which line card is located.",
    "Example:\r\n"
    "  general demo show_fe_link crate_type gobi line_card_slot 11\r\n"
    "    fabric_card_slot 17 fap_link 3\r\n"
    "Results in display of FE link (and FE identifier) on fabric card, sitting\r\n"
    "in slot 17, which is connected to link 3 of FAP on line card, sitting\r\n"
    "in slot 11. The whole setup is calculated for a GOBI crate.\r\n",
    "",
    {BIT(16)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DEMO_FABRIC_CARD_SLOT_ID,
    "fabric_card_slot",
    (PARAM_VAL_RULES *)&Fabric_card_slot_demo_vals[0],
    (sizeof(Fabric_card_slot_demo_vals) / sizeof(Fabric_card_slot_demo_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select slot on which fabric card is located.",
    "Example:\r\n"
    "  general demo show_fe_link crate_type gobi line_card_slot 11\r\n"
    "    fabric_card_slot 17 fap_link 3\r\n"
    "Results in display of FE link (and FE identifier) on fabric card, sitting\r\n"
    "in slot 17, which is connected to link 3 of FAP on line card, sitting\r\n"
    "in slot 11. The whole setup is calculated for a GOBI crate.\r\n",
    "",
    {BIT(16)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#if LINK_PSS_LIBRARIES
/* { */
  {
    PARAM_GENERAL_DEMO_FAP_LINK_ID,
    "fap_link",
    (PARAM_VAL_RULES *)&Fap_link_demo_vals[0],
    (sizeof(Fap_link_demo_vals) / sizeof(Fap_link_demo_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Select link on FAP which connects to indicated fabric card.",
    "Example:\r\n"
    "  general demo show_fe_link crate_type gobi line_card_slot 11\r\n"
    "    fabric_card_slot 17 fap_link 3\r\n"
    "Results in display of FE link (and FE identifier) on fabric card, sitting\r\n"
    "in slot 17, which is connected to link 3 of FAP on line card, sitting\r\n"
    "in slot 11. The whole setup is calculated for a GOBI crate.\r\n",
    "",
    {BIT(16)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } LINK_PSS_LIBRARIES */
  {
    PARAM_GENERAL_REMARK_ID,
    "rem",
    (PARAM_VAL_RULES *)&General_remark_vals[0],
    (sizeof(General_remark_vals) / sizeof(General_remark_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display free text typed in by the user and do nothing.",
    "Example:\r\n"
    "  general rem \"The big brown wolf\"\r\n"
    "Results in showing the line. If spaces are not required then\r\n"
    "quotation marks may be dropped\r\n",
    "",
    {BIT(14)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FREE_UI_BITS_ID,
    "list_free_ui_bits",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "In each SUBJECT in Subjects_list_rom[], print the free bits in\r\n"
    "PARAM->mutex_control.",
    "Example:\r\n"
    "  general list_free_ui_bits\r\n"
    "\r\n",
    "",
    {BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_VERSIONS_ID,
    "versions",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display versions of all submodules in the system.",
    "Example:\r\n"
    "  general versions\r\n"
    "Results in showing all versions in full format\r\n",
    "",
    {BIT(11)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_HOST_BOARD_SN_ID,
    "get_host_board_sn",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display host board serial number.",
    "Example:\r\n"
    "  general get_host_board_sn\r\n"
    "Results in showing host board serial number\r\n",
    "",
    {BIT(11)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_TRACE_ID,
    "trace",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Handle various real-time-trace commands, including \'display\'.",
    "Example:\r\n"
    "  general trace display\r\n"
    "Results in showing the full contents of the trace fifo. Note that trace\r\n"
    "fifo operation is disabled while collecting information for this operation.\r\n",
    "",
    {BIT(12)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_TRACE_DISPLAY_N_CLEAR_ID,
    "display_and_clear",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display full contents of real-time-trace fifo and clear it.",
    "Examples:\r\n"
    "  general trace display_and_clear\r\n"
    "Results in showing the full contents of the trace fifo. Also, trace fifo is\r\n"
    "cleared!\r\n"
    "  general trace display\r\n"
    "Results in showing the full contents of the trace fifo. Trace fifo is NOT\r\n"
    "cleared!\r\n"
    "Note that trace fifo operation is disabled while collecting information\r\n"
    "for these operations.\r\n",
    "",
    {BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_TRACE_DISPLAY_ID,
    "display",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display full contents of real-time-trace fifo.",
    "Examples:\r\n"
    "  general trace display_and_clear\r\n"
    "Results in showing the full contents of the trace fifo. Also, trace fifo is\r\n"
    "cleared!\r\n"
    "  general trace display\r\n"
    "Results in showing the full contents of the trace fifo. Trace fifo is NOT\r\n"
    "cleared!\r\n"
    "Note that trace fifo operation is disabled while collecting information\r\n"
    "for these operations.\r\n",
    "",
    {BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_ID,
    "cli",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Handle various CLI-oriented commands, including \'files\'.",
    "Example:\r\n"
    "  general cli dir\r\n"
    "Results in showing \'directory\' of current CLI \'file\'s.\r\n"
    "  general cli download filename script_01.txt\r\n"
    "Results in downloading file \'script_01.txt\' from assigned TFTP server\r\n"
    "into next available cli \'file\'. If all available \'files\' are already\r\n"
    "loaded then one must be erased using \'general cli erase filename ...\'\r\n",
    "",
    {BIT(9) | BIT(10) | BIT(28)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download CLI-oriented file from assigned TFTP server.",
    "Example:\r\n"
    "  general cli download filename script_01.txt\r\n"
    "Results in downloading file \'script_01.txt\' from assigned TFTP server\r\n"
    "into next available cli \'file\'. If all available \'files\' are already\r\n"
    "loaded then one must be erased using \'general cli erase filename ...\'\r\n",
    "",
    {BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_ERASE_ID,
    "erase",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Erase CLI-oriented file from diretory on this system.",
    "Example:\r\n"
    "  general cli erase filename script_01.txt\r\n"
    "Results in erasing file \'script_01.txt\' from local RAM. This has NO\r\n"
    "influence on files on assigned TFTP server. If there is no \'file\'\r\n"
    "by this name on this system, an error message is issued.\r\n",
    "",
    {BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_DISPLAY_ID,
    "display",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display CLI-oriented file from diretory on this system.",
    "Example:\r\n"
    "  general cli display filename script_01.txt\r\n"
    "Results in displaying contents of file \'script_01.txt\' from local\r\n"
    "RAM. This has NOT related to files on assigned TFTP server.\r\n"
    "If there is no \'file\' by this name on this system, an error message\r\n"
    "is issued.\r\n",
    "",
    {BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_RUN_ID,
    "run",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Run CLI-oriented file from diretory on this system.",
    "Example:\r\n"
    "  general cli run filename script_01.txt\r\n"
    "Results in invoking CLI interpreter using contents of file \'script_01.txt\'\r\n"
    "from local RAM. This has NOT related to files on assigned TFTP server.\r\n"
    "If there is no \'file\' by this name on this system, an error message\r\n"
    "is issued. To stop execution half way through, hit \'esc\'.\r\n",
    "",
    {BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_FILENAME_ID,
    "filename",
    (PARAM_VAL_RULES *)&Cli_file_name_vals[0],
    (sizeof(Cli_file_name_vals) / sizeof(Cli_file_name_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Specify CLI-oriented file name for some action (e.g. downloading).",
    "Example:\r\n"
    "  general cli download filename script_01.txt\r\n"
    "Results in downloading file \'script_01.txt\' from assigned TFTP server\r\n"
    "into next available cli \'file\'. If all available \'files\' are already\r\n"
    "loaded then one must be erased using \'general cli erase filename ...\'\r\n",
    "",
    {BIT(9)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_DOWNLOAD_HOST_IP_ID,
    "host_ip",
    (PARAM_VAL_RULES *)&Cli_download_host_ip[0],
    (sizeof(Cli_download_host_ip) / sizeof(Cli_download_host_ip[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ip of host to download from (If host_ip is not specified, IP address\r\n"
    "of downloading host is used.",
    "Example:\r\n"
    "  general cli download filename script_01.txt host_ip 10.0.0.12\r\n"
    "Results in downloading file \'script_01.txt\' from specific host\r\n"
    "into next available cli \'file\'. If all available \'files\' are already\r\n"
    "loaded then one must be erased using \'general cli erase filename ...\'\r\n",
    "",
    {BIT(9)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GENERAL_CLI_DIR_ID,
    "dir",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get information on all CLI-oriented files in this system.",
    "Example:\r\n"
    "  general cli dir\r\n"
    "Results in display of all file descriptors allocated for CLI in this system\r\n",
    "",
    {BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_EXEC_TIME_ID,
    "exec_time",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get information on execution time of last CLI-oriented file.",
    "Example:\r\n"
    "  general cli exec_time\r\n"
    "Results in display of execution time of last\r\n"
    "CLI file (If any has been executed)\r\n",
    "",
    {BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_SHOW_ID,
    "show",
    (PARAM_VAL_RULES *)&Show_general_vals[0],
    (sizeof(Show_general_vals) / sizeof(Show_general_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Show_general_default,
    0,0,0,0,0,0,0,
    "Show info on variables contained in variables block (e.g. \'temperature\')\r\n"
    "(Exactly equivalent to \'read\')",
    "Examples:\r\n"
    "  general temperature show regular \r\n"
    "Results in displaying current temperature reading.\r\n",
    "",
    {BIT(0) | BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_SHOW_ID,
    "read",
    (PARAM_VAL_RULES *)&Show_general_vals[0],
    (sizeof(Show_general_vals) / sizeof(Show_general_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Show_general_default,
    0,0,0,0,0,0,0,
    "Show info on variables contained in variables block (e.g. \'temperature\')\r\n",
    "Examples:\r\n"
    "  general temperature read regular\r\n"
    "Results in displaying current temperature reading.\r\n",
    "",
    {BIT(0) | BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DISPLAY_ID,
    "display",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display action (e.g. RUNTIME event log \'file\', CLI error counter, ...).",
    "Examples:\r\n"
    "  general runtime_log display\r\n"
    "Results in displaying RUNTIME log \'file\'.\r\n",
    "",
    {BIT(4) | BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLEAR_ID,
    "clear",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Clear action (e.g. RUNTIME event log \'file\' or CLI error counter).",
    "Examples:\r\n"
    "  general runtime_log clear\r\n"
    "Results in clearing RUNTIME log \'file\'.\r\n",
    "",
    {BIT(4)| BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_RUNTIME_LOG_ID,
    "runtime_log",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Handle RUNTIME event log \'file\'.",
    "Examples:\r\n"
    "  general runtime_log clear\r\n"
    "Results in clearing RUNTIME log \'file\'.\r\n"
    "  general runtime_log display\r\n"
    "Results in displaying RUNTIME log \'file\'.\r\n",
    "",
    {BIT(4)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_APP_FILE_ID,
    "application_file",
    (PARAM_VAL_RULES *)&App_file_general_vals[0],
    (sizeof(App_file_general_vals) / sizeof(App_file_general_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Identify FLASH-related application \'files\'.",
    "Examples:\r\n"
    "  general flash erase application_file 1\r\n"
    "Results in erasing FLASH memory at location of application\r\n"
    "\'file\' no. 1 (first in Flash memory).\r\n",
    "",
    {BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_ERASE_ID,
    "erase",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Erase FLASH-related application \'files\'.",
    "Examples:\r\n"
    "  general flash erase application_file 1\r\n"
    "Results in erasing FLASH memory at location of application\r\n"
    "\'file\' no. 1 (first in Flash memory).\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download FLASH-related application \'files\' or boot.",
    "Examples:\r\n"
    "  general flash download application host_ip 10.0.0.12\r\n"
    "Results in downloading application from specifiec host.\r\n"
    "The downloading may acure only if there is avaliable \r\n"
    "place in the flash.\r\n",
    "",
    {BIT(5) | BIT(6) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DOWNLOAD_APP_ID,
    "application",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download FLASH-related application \'files\'.",
    "Examples:\r\n"
    "  general flash download application host_ip 10.0.0.12\r\n"
    "Results in downloading application from specifiec host.\r\n"
    "The downloading may acure only if there is avaliable \r\n"
    "place in the flash.\r\n",
    "",
    {BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DOWNLOAD_BOOT_ID,
    "boot",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download FLASH-related boot.",
    "Examples:\r\n"
    "  general flash download application boot host_ip 10.0.0.12\r\n"
    "Results in downloading application from specifiec host.\r\n"
    "The downloading may acure only if there is avaliable \r\n"
    "place in the flash.\r\n",
    "",
    {BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DOWNLOAD_HOST_IP_ID,
    "host_ip",
    (PARAM_VAL_RULES *)&General_download_host_ip[0],
    (sizeof(General_download_host_ip) / sizeof(General_download_host_ip[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ip of host to download from (If host_ip is not specified, IP address\r\n"
    "of downloading host is used. See nvram block \'boot_parameters\').",
    "Examples:\r\n"
    "  general flash download application host_ip 10.0.0.12\r\n"
    "Results in downloading application from specifiec host.\r\n"
    "The downloading may acure only if there is avaliable \r\n"
    "place in the flash.\r\n",
    "",
    {BIT(5) | BIT(6) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_SHOW_ID,
    "show",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Show information (e.g. FLASH related info).",
    "Examples:\r\n"
    "  \'general flash show\' results in display of all SW code \'files\'\r\n"
    "  currently in FLASH memory\r\n",
    "",
    {BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_ID,
    "flash",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Handle FLASH-related \'files\'.",
    "Examples:\r\n"
    "  general flash erase application_file 0\r\n"
    "Results in erasing FLASH memory at location of application \'file\' zero.\r\n",
    "",
    {
      BIT(3) | BIT(5) | BIT(6) | BIT(7),
      BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)
    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_TEMPERATURE_ID,
    "temperature",
    (PARAM_VAL_RULES *)&Temperature_vals[0],
    (sizeof(Temperature_vals) / sizeof(Temperature_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display different board temperature variable contained in GENERAL\r\n"
    "variables block.",
    "Examples:\r\n"
    "  general temperature mezzanine_board read periodic\r\n"
    "Results in continuous display of temperature reading from the mezzanine.\r\n",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CLI_ERROR_COUNTER_ID,
    "cli_error_counter",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display accumulated \'CLI error counter\' (number of user typing error).",
    "Examples:\r\n"
    "  general cli_error_counter display\r\n"
    "Results in display of accumulated number of typing/syntax errors.\r\n"
    "The user may clear this count using \'general cli_error_counter clear\'\r\n",
    "",
    {BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_WATCHDOG_ID,
    "watchdog",
    (PARAM_VAL_RULES *)&Watchdog_vals[0],
    (sizeof(Watchdog_vals) / sizeof(Watchdog_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "\'Watchdog\' - related variables and actions.",
    "Examples:\r\n"
    "  general watchdog stop_reset\r\n"
    "Results in continuous display of temperature reading.\r\n",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_EXERCISES_ID,
    "exercises",
    (PARAM_VAL_RULES *)&Exercises_vals[0],
    (sizeof(Exercises_vals) / sizeof(Exercises_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Various exercises - for debug and internal testing.",
    "Examples:\r\n"
    "  general exercises event_storm\r\n"
    "Results in inflicting a storm of events on the system.\r\n",
    "",
    {BIT(13)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_TIME_FROM_STARTUP_ID,
    "time_from_startup",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display \'time from startup\' in units of seconds.",
    "Examples:\r\n"
    "  general time_from_startup read period\r\n"
    "Results in continuous display of time reading.\r\n",
    "",
    {BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_PERIODIC_SUSPEND_TEST_ID,
    "periodic_suspend_test",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "For inner debugging of function periodic_suspend_test().",
    "Examples:\r\n"
    "  general periodic_suspend_test\r\n",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GET_BOARD_INFO,
    "get_board_info",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print the board information.",
    "Examples:\r\n"
    "  general get_board_info\r\n",
    "",
    {0,0,BIT(23)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_PETRA_TEST_CHIP_PROGRAM_LOAD_ID,
    "petra_test_chip_program_load",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "For loading the 8051 ROM block of the serdes in test-chip",
    "Examples:\r\n"
    "  general petra_test_chip_program_load chip_id 2\r\n",
    "  Will result in loading test chip 2",
    {0,0,0,0,BIT(29)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_DSDA_INIT,
    "dsda_init",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "petra_test_chip_initialization before running DSDA",
    "Initilizes FPGA & synthesizer, takes macros out of reset and transfers\n\r"
    "Control to SCIF"
    "Exmples:\r\n"
    "  general dsda_init chip_id 2\r\n",
    "  Will prepare chip 2 for SCIF connection by the DSDA application",
    {0,0,0,0,BIT(29)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_CHIP_ID_ID,
    "chip_id",
    (PARAM_VAL_RULES *)&General_generic_chip_id[0],
    (sizeof(General_generic_chip_id) / sizeof(General_generic_chip_id[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "There are 3 test-chips (choose 0,1,2)",
    "Examples:\r\n"
    "  general generic_test param1 12\r\n",
    "",
    {0,0,0,0,BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_TEST_ID,
    "generic_test",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "For inner debugging of any test",
    "Examples:\r\n"
    "  general generic_test\r\n",
    "",
    {BIT(29)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_0_ID,
    UGT_PARAM0,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM0_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_1_ID,
    UGT_PARAM1,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM1_DESC,
    "Examples:\r\n"
    "  general generic_test param1 12\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_2_ID,
    UGT_PARAM2,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM2_DESC,
    "Examples:\r\n"
    "  general generic_test param1 12 param2 13\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_3_ID,
    UGT_PARAM3,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM3_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_4_ID,
    UGT_PARAM4,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM4_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_5_ID,
    UGT_PARAM5,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM5_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_6_ID,
    UGT_PARAM6,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM6_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_7_ID,
    UGT_PARAM7,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM7_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_8_ID,
    UGT_PARAM8,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM8_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_GENERIC_PARAM_9_ID,
    UGT_PARAM9,
    (PARAM_VAL_RULES *)&General_generic_any_number[0],
    (sizeof(General_generic_any_number) / sizeof(General_generic_any_number[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&General_generic_params_default,
    0,0,0,0,0,0,0,
    UGT_PARAM9_DESC,
    "Examples:\r\n"
    "  general generic_test param0 0 param1 1 param2 2 ...\r\n",
    "",
    {BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_REGRESSION_TEST_ID,
    "regression_test",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "used for running regression tests",
    "Examples:\r\n"
    "  general regression_test all scope full\r\n",
    "",
    {BIT(30)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_REGRESSION_ALL_ID,
    "all",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "runs a full regression test",
    "Examples:\r\n"
    "  general regression_test all scope full\r\n",
    "",
    {BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_REGRESSION_AUTOLEARN_ID,
    "autolearn",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MUST_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "runs autolearning regression test",
    "Examples:\r\n"
    "  general regression_test all full\r\n",
    "",
    {BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_REGRESSION_FULL_ID,
    "full",
     (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "runs the regression test in a full mode",
    "Examples:\r\n"
    "  general regression_test all full\r\n",
    "",
    {BIT(30)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_REGRESSION_SHORT_ID,
    "short",
     (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "runs a regression test in a short mode",
    "Examples:\r\n"
    "  general regression_test all full\r\n",
    "",
    {BIT(30)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_STOP_NETWORK_LEARNING,
    "stop_network_learning",
     (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Stop network learning during init",
    "Examples:\r\n"
    "  general stop_network_learning\r\n",
    "",
    {0, 0, 0, BIT(6)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_ID,
    "dffs",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  a general purpose file system located on the embedded flash,\r\n"
    "  dffs section is separate from the application section.",
    "Examples:\r\n"
    "  general flash dffs download file_name file1.txt\r\n"
    "  results in downloading file1.txt and writing it to embedded flash\r\n",
    "",
    {
      0,
      BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_DIR_ID,
    "dir",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  list all files and their attributes.\r\n"
    "  long format adds comments listing",
    "Examples:\r\n"
    "  general flash dffs dir format long\r\n",
    "",
    {
      0,
      BIT(1)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_DIR_FORMAT_ID,
    "format",
    (PARAM_VAL_RULES *)&Dffs_dir_format_vals[0],
    (sizeof(Dffs_dir_format_vals) / sizeof(Dffs_dir_format_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Dffs_dir_format_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  list all files and their attributes.\r\n"
    "  long format adds comments listing",
    "Examples:\r\n"
    "  general flash dffs dir format long\r\n",
    "",
    {
      0,
      BIT(1)
    },
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_DIAGNOSTICS_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  print full dffs information.\r\n"
    "  can be used for diagnostics and debug",
    "Examples:\r\n"
    "  general flash dffs diagnostics\r\n",
    "",
    {
      0,
      BIT(5)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_DEL_ID,
    "delete",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  delete a file from the embedded flash",
    "Examples:\r\n"
    "  general flash dffs delete file_name file1.txt\r\n"
    "  Results in erasing file1.txt from flash\r\n",
    "",
    {
      0,
      BIT(0)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_DLD_ID,
    "download",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  download a file from host and burn it on the embedded flash",
    "Examples:\r\n"
    "  general flash dffs download file_name file1.txt\r\n"
    "  Results in downloading file1.txt from host and writing it to flash \r\n",
    "",
    {
      0,
      BIT(0)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_PRINT_ID,
    "print",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  print the binary contents of the specified file,\r\n"
    "  if that file exists\r\n"
    "  enough RAM memory must be available to load the file",
    "Examples:\r\n"
    "  general flash dffs print file_name file1.txt size_in_bytes 300\r\n"
    "  Results in printing first 300 bytes (if exist) of file1.txt \r\n",
    "",
    {
      0,
      BIT(4)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_PRINT_SIZE_ID,
    "size_in_bytes",
    (PARAM_VAL_RULES *)&Dffs_prnt_size_vals[0],
    (sizeof(Dffs_prnt_size_vals) / sizeof(Dffs_prnt_size_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  the number of bytes to print",
    "Examples:\r\n"
    "  general flash dffs write file_name file1.txt\r\n"
    "  date 05/06/06 attr1 12 attr2 1 attr3 98 \r\n",
    "",
    {
      0,
      BIT(4)
    },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_ADD_COMMENT_ID,
    "add_comment",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  add comment to dffs file\r\n"
    "  the comment should be one word - spaces should be avoided!",
    "Examples:\r\n"
    "  general flash dffs add_comment file_name file1.txt comment this_is_my_comment \r\n"
    "  Results in adding the string 'this_is_my_comment' as a comment \r\n",
    "",
    {
      0,
      BIT (2)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_SHOW_COMMENT_ID,
    "show_comment",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  show a comment of a dffs file",
    "Examples:\r\n"
    "  general flash dffs show_comment file_name file1.txt\r\n"
    "  Results in printing the comment attached to file1.txt \r\n",
    "",
    {
      0,
      BIT (0)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_NAME_ID,
    "file_name",
    (PARAM_VAL_RULES *)&Dffs_file_name_vals[0],
    (sizeof(Dffs_file_name_vals) / sizeof(Dffs_file_name_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  the name of the dffs file - maximum 24 characters long",
    "Examples:\r\n"
    "  general flash dffs print file_name file1.txt\r\n",
    "",
    {
      0,
      BIT(0) | BIT (2) | BIT (3) | BIT(4)
    },
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_COMMENT_ID,
    "comment",
    (PARAM_VAL_RULES *)&Dffs_comment_vals[0],
    (sizeof(Dffs_comment_vals) / sizeof(Dffs_comment_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  comment attached to a dffs file"
    "  the comment should be one word - spaces should be avoided!",
    "Examples:\r\n"
    "  general flash dffs add_comment file_name file1.txt comment this_is_my_comment \r\n"
    "  Results in adding the string 'this_is_my_comment' as a comment \r\n",
    "",
    {
      0,
      BIT (2)
    },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_ADD_ATTR_ID,
    "add_attribute",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  add a free numeric attribute to dffs file\r\n"
    "  maximum 6 digits allowed",
    "Examples:\r\n"
    "  general flash dffs add_attribute file_name file1.txt attr1_val 3\r\n"
    "  Results in adding 3 as the value of attr1 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT(0) | BIT (3)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_DATE_ID,
    "date_val",
    (PARAM_VAL_RULES *)&Dffs_date_vals[0],
    (sizeof(Dffs_date_vals) / sizeof(Dffs_date_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Dffs_date_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  creation date - in a dd/mm/yy format",
    "Examples:\r\n"
    "  general flash dffs add_attribute file_name file1.txt date_val 22/10/77\r\n"
    "  Results in adding 22/10/77 as the creation date for file1.txt\r\n",

    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_VER_ID,
    "ver_val",
    (PARAM_VAL_RULES *)&Dffs_ver_vals[0],
    (sizeof(Dffs_ver_vals) / sizeof(Dffs_ver_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  version of dffs file",
    "Examples:\r\n"
     "  general flash dffs add_attribute file_name file1.txt ver_val 2\r\n"
    "  Results in adding 2 as the version of file1.txt\r\n",
   "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_ATTR1_ID,
    "attr1_val",
    (PARAM_VAL_RULES *)&Dffs_attr_vals[0],
    (sizeof(Dffs_attr_vals) / sizeof(Dffs_attr_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  free attribute value - numeric value, maximum 6 digits",
    "Examples:\r\n"
     "  general flash dffs add_attribute file_name file1.txt attr1_val 3\r\n"
    "  Results in adding 3 as the value of attr1 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_ATTR2_ID,
    "attr2_val",
    (PARAM_VAL_RULES *)&Dffs_attr_vals[0],
    (sizeof(Dffs_attr_vals) / sizeof(Dffs_attr_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  free attribute value - numeric value, maximum 6 digits",
    "Examples:\r\n"
     "  general flash dffs add_attribute file_name file1.txt attr2_val 3\r\n"
    "  Results in adding 3 as the value of attr2 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GENERAL_FLASH_DFFS_ATTR3_ID,
    "attr3_val",
    (PARAM_VAL_RULES *)&Dffs_attr_vals[0],
    (sizeof(Dffs_attr_vals) / sizeof(Dffs_attr_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  free attribute value - numeric value, maximum 6 digits",
    "Examples:\r\n"
     "  general flash dffs add_attribute file_name file1.txt attr3_val 3\r\n"
    "  Results in adding 3 as the value of attr3 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
#endif /*} LINUX */
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Mem_params
*TYPE: BUFFER
*DATE: 18/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "MEM".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      This field is automatically filled in by the system
*      and does not need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Mem_params[]
#ifdef INIT
   =
{
  {
    PARAM_MEM_VAL_READ_ID,
    "val_read",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read memory using one access method, mask, compare with reference\r\n"
    "  and indicate whether a match has been found (under timeout).",
    "Examples:\r\n"
    "  mem val_read 0x103A22 val 1 mask 0xff timeout 200\r\n"
    "Results in reading one long variable (default), at absolute address\r\n"
    "0x103A22 and comparing the LS 8 bits with a value of 0x01. Once a\r\n"
    "match is found, within 200 microseconds, control returns to the user.\r\n"
    "If a match is not found then an indication plus value actually found\r\n"
    "are displayed\r\n",
    "",
    {BIT(10) | BIT(11)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_COUNTERS_ID,
    "counters",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Operate on counters related to memory operations such as \'val_read\'.",
    "Example:\r\n"
    "  mem counters get val_read_failures\r\n"
    "Results in presenting to the user the current accumulative number of\r\n"
    "failures related to the read-compare operation of \'val_read\'.\r\n"
    "This counter can be cleared using \'mem counters clear val_read_failures\'.\r\n",
    "",
    {BIT(12) | BIT(13)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read memory using a few access methods.",
    "Examples:\r\n"
    "  mem read 0x103A22\r\n"
    "Results in reading one long variable (default),\r\n"
    "starting at absolute address 0x103A22, and displaying the result\r\n"
    "in hex format (default), on the screen\r\n",
    "",
    {BIT(0) | BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_MUL_WRITE_ID,
    "mul_write",
    (PARAM_VAL_RULES *)&Mul_write_vals[0],
    (sizeof(Mul_write_vals) / sizeof(Mul_write_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write into a few memory addresses using a few access methods.",
    "Examples:\r\n"
    "  mem mul_write 1 20 40 offset type sdram data 0xAB 0xCD 0xFFFF\r\n"
    "Results in writing three long variables, of value 0x000000AB,\r\n"
    "0x000000CD and 0x0000FFFF, into addresses of SDRAM block at offsets\r\n"
    "1, 20 and 40 from the beginning of SDRAM block.\r\n",
    "",
    {BIT(14) | BIT(15)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write into memory using a few access methods.",
    "Examples:\r\n"
    "  mem write 0x103A22 data 0xAB format char\r\n"
    "Results in writing one byte, of value 0xAB,\r\n"
    "at absolute address 0x103A22.\r\n",
    "",
    {BIT(1) | BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_XOR_ID,
    "xor",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read, do xor operation and then write into memory.",
    "Examples:\r\n"
    "  mem xor 0x103A22 data 0xAB format char\r\n"
    "Results in writing at address 0x103A22,\r\n"
    " one byte, of value 0xAB xor old data that was in that address,\r\n",
    "",
    {BIT(4) | BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_AND_ID,
    "and",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read, do 'and' operation and then write into memory.",
    "Examples:\r\n"
    "  mem and 0x103A22 data 0xAB format char\r\n"
    "Results in writing at address 0x103A22,\r\n"
    " one byte, of value 0xAB 'and' old data that was in that address,\r\n",
    "",
    {BIT(6) | BIT(7)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_OR_ID,
    "or",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read, do 'or' operation and then write into memory.",
    "Examples:\r\n"
    "  mem or 0x103A22 data 0xAB format char\r\n"
    "Results in writing at address 0x103A22,\r\n"
    " one byte, of value 0xAB 'or' old data that was in that address,\r\n",
    "",
    {BIT(8) | BIT(9)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_REMOTE_OPERATION_ID,
    "remote_operation",
    (PARAM_VAL_RULES *)&Dpss_unit_num_vals[0],
    (sizeof(Dpss_unit_num_vals) / sizeof(Dpss_unit_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sends the command over the transport layer to a remote machine.\r\n",
    "Examples:\r\n"
    "  mem write 0x103A22 data 0xAB format char remote_operation 2\r\n"
    "Results in writing one byte, of value 0xAB,\r\n"
    "at absolute address 0x103A22 at board #2 (unit = 2)\r\n",
    "",
    {
      BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) | BIT(9)
    },
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#if !(defined(LINUX) || defined(UNIX))
/* { */
  {
    PARAM_MEM_NUM_OF_TIMES_ID,
    "nof_times",
    (PARAM_VAL_RULES *)&Nof_times_mem_read_vals[0],
    (sizeof(Nof_times_mem_read_vals) / sizeof(Nof_times_mem_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Nof_times_mem_read_default,
    0,0,0,0,0,0,0,
    "Re-current read. 'nof_times' times.",
    "Examples:\r\n"
    "  mem read 0x103A22 nof_times 12\r\n"
    "Results in reading, 12 times, one long variable (default),\r\n"
    "starting at absolute address 0x103A22, and displaying the result\r\n"
    "in hex format (default), on the screen\r\n",
    "",
    {BIT(0) | BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_PAUSE_AFTER_READ_ID,
    "pause_after_read",
    (PARAM_VAL_RULES *)&Pause_after_read_mem_read_vals[0],
    (sizeof(Pause_after_read_mem_read_vals) / sizeof(Pause_after_read_mem_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pause_after_read_mem_read_default,
    0,0,0,0,0,0,0,
    "Pause for amount of time after read (micro-sec). (Useful with 'nof_times' parameter.)",
    "Examples:\r\n"
    "  mem read 0x103A22 nof_times 12 pause_after_read 10\r\n"
    "Results in reading, 12 times, one long variable (default),\r\n"
    "starting at absolute address 0x103A22, and displaying the result\r\n"
    "in hex format (default), on the screen.\r\n"
    "After every read the CPU will halt for 10 micro-sec\n\r"
    "This number will be rounded UP, to the next number\n\r"
    "in system ticks. Hence, 1 micro sleep request,\n\r"
    "will result in 16,666 micro sleep.\n\r",
    "",
    {BIT(0) | BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_PAUSE_AFTER_READ_ID,
    "pause_after_read",
    (PARAM_VAL_RULES *)&Pause_after_read_mem_read_vals[0],
    (sizeof(Pause_after_read_mem_read_vals) / sizeof(Pause_after_read_mem_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Pause_after_read_mem_read_default,
    0,0,0,0,0,0,0,
    "Pause for amount of time after read (micro-sec). (Useful with 'nof_times' parameter.)",
    "Examples:\r\n"
    "  mem read 0x103A22 nof_times 12 pause_after_read 10\r\n"
    "Results in reading, 12 times, one long variable (default),\r\n"
    "starting at absolute address 0x103A22, and displaying the result\r\n"
    "in hex format (default), on the screen.\r\n"
    "After every read the CPU will halt for 10 micro-sec \n\r",
    "",
    {BIT(0) | BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_FORMAT_ID,
    "format",
    (PARAM_VAL_RULES *)&Format_mem_vals[0],
    (sizeof(Format_mem_vals) / sizeof(Format_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Format_mem_default,
    0,0,0,0,0,0,0,
    "Identifier of memory access methods.",
    "Examples:\r\n"
    "  mem format short read 0x103A22\r\n"
    "Results in reading one short variable,\r\n"
    "starting at absolute address 0x103A22, and displaying the\r\n"
    "result in hex format (default), on the screen.\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) |
    BIT(6) | BIT(7) | BIT(8) | BIT(9) | BIT(10) |
    BIT(11) | BIT(14) | BIT(15)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Override_mem_default,
    0,0,0,0,0,0,0,
    "Override memory block limits protection.",
    "Examples:\r\n"
    "  mem format short read 200 offset type epld override\r\n"
    "Results in reading one short variable,\r\n"
    "starting at absolute address 0f 200 bytes beyond start\r\n"
    "of EPLD memory block (which is out of the block!), and\r\n"
    "displaying the result in hex format (default), on the\r\n"
    "screen. Use this option with caution since it may easily\r\n"
    "result in memory access error.\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) |
    BIT(7) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(14) | BIT(15)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_COUNTERS_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&Counters_vals[0],
    (sizeof(Counters_vals) / sizeof(Counters_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "\'Get\' operation to carry out. Related to \'mem counters\'.",
    "Example:\r\n"
    "  mem counters get val_read_failures\r\n"
    "Results in presenting to the user the current accumulative number of\r\n"
    "failures related to the read-compare operation of \'val_read\'.\r\n"
    "This counter can be cleared using \'mem counters clear val_read_failures\'.\r\n",
    "",
    {BIT(12)},
    (unsigned int)2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_COUNTERS_CLEAR_ID,
    "clear",
    (PARAM_VAL_RULES *)&Counters_vals[0],
    (sizeof(Counters_vals) / sizeof(Counters_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "\'Clear\' operation to carry out. Related to \'mem counters\'.",
    "Example:\r\n"
    "  mem counters clear val_read_failures\r\n"
    "Results in clearing the current accumulative number of failures related\r\n"
    "to the read-compare operation of \'val_read\'.\r\n"
    "This counter can be read using \'mem counters get val_read_failures\'.\r\n",
    "",
    {BIT(13)},
    (unsigned int)2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Silent_mem_default,
    0,0,0,0,0,0,0,
    "prints minimal information to screen.",
    "Examples:\r\n"
    "  mem format short read 200 offset type epld override silent\r\n"
    "Results in reading one short variable but displaying only short value\r\n"
    "without extra explanatory details. First character on each line is\r\n"
    "always a \'$\' sign. Note that if \'silent\' is specified together\r\n"
    "with \'mem_buff_offset\' then no printout is sent to the screen!",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) |
    BIT(7) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) |
    BIT(13) | BIT(14) | BIT(15)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_ADDRESS_IN_LONGS_ID,
    "address_in_longs",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Address_in_longs_mem_default,
    0,0,0,0,0,0,0,
    "If appears address/offset is in longs.",
    "Examples:\r\n"
    "  mem read 0x100 address_in_longs\r\n"
    "Results in reading one long variable, from offset ***0x400***!\n\r"
    "Adress or offset is multiplied by 4.",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) |
    BIT(7) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) |
    BIT(13) | BIT(14) | BIT(15)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_LEN_ID,
    "len",
    (PARAM_VAL_RULES *)&Len_mem_vals[0],
    (sizeof(Len_mem_vals) / sizeof(Len_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Len_mem_default,
    0,0,0,0,0,0,0,
    "Number of memory access actions to carry out (\'read\' only).",
    "Examples:\r\n"
    "  mem format short read 0x103A22 len 8\r\n"
    "Results in reading eight short variable,\r\n"
    "starting at absolute address 0x103A22, and displaying the\r\n"
    "result in hex format (default), on the screen.\r\n",
    "",
    {BIT(0) | BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_BUFF_OFFSET_ID,
    "mem_buff_offset",
    (PARAM_VAL_RULES *)&Mem_buff_offset_vals[0],
    (sizeof(Mem_buff_offset_vals) / sizeof(Mem_buff_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset location, in local storage especially assigned to \'mem\',\r\n"
    "  to store results of \'read\' operation",
    "Examples:\r\n"
    "  mem format short read 0x103A22 len 8 mem_buff_offset 4\r\n"
    "Results in reading eight short variables, starting at absolute address\r\n"
    "0x103A22, displaying the result in hex format (default), on the screen,\r\n"
    "AND loading the results into local buffer starting at byte offset 4 and\r\n"
    "ending at byte offset 19 (included).\r\n",
    "",
    {BIT(0) | BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_DISP_ID,
    "disp",
    (PARAM_VAL_RULES *)&Disp_mem_vals[0],
    (sizeof(Disp_mem_vals) / sizeof(Disp_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Disp_mem_default,
    0,0,0,0,0,0,0,
    "Display format to use for \'read\' results.",
    "Examples:\r\n"
    "  mem format short read 0x103A22 len 8 disp signed_integer\r\n"
    "Results in reading eight short variable,\r\n"
    "starting at absolute address 0x103A22, and displaying the\r\n"
    "result in signed_integer format, on the screen.\r\n",
    "",
    {BIT(0) | BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&Type_mem_vals[0],
    (sizeof(Type_mem_vals) / sizeof(Type_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Type_mem_default,
    0,0,0,0,0,0,0,
    "Type of memory to access (use with \'offset\' only!).",
    "Examples:\r\n"
    "  mem type flash_bank_1 offset read 0x10\r\n"
    "Results in reading one long variable (default), from FLASH memory (bank 1),\r\n"
    "starting at offset of 0x10 bytes from the beginning of bank 1, and\r\n"
    "displaying the result in hex format (default), on the screen.\r\n",
    "",
    {BIT(2) | BIT(3) | BIT(5) | BIT(7) | BIT(9) |
    BIT(11) | BIT(15)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset of address to access (use with \'type\' only!).",
    "Examples:\r\n"
    "  mem type flash_bank_1 offset read 0x10\r\n"
    "Results in reading one long variable (default), from FLASH memory (bank 1),\r\n"
    "starting at offset of 0x10 bytes from the beginning of bank 1, and displaying the\r\n"
    "result in hex format (default), on the screen.\r\n",
    "",
    {BIT(2) | BIT(3) | BIT(5) | BIT(7) | BIT(9) |
    BIT(11) | BIT(15)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_MUL_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Data_mem_vals[0],
    (sizeof(Data_mem_vals) / sizeof(Data_mem_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Valus to write into memory methods.",
    "Examples:\r\n"
    "  mem mul_write 1 20 40 offset type sdram data 0xAB 0xCD 0xFFFF\r\n"
    "Results in writing three long variables, of value 0x000000AB,\r\n"
    "0x000000CD and 0x0000FFFF, into addresses of SDRAM block at offsets\r\n"
    "1, 20 and 40 from the beginning of SDRAM block.\r\n",
    "",
    {BIT(14) | BIT(15)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Data_mem_vals[0],
    (sizeof(Data_mem_vals) / sizeof(Data_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Data_mem_default,
    0,0,0,0,0,0,0,
    "Valus to write into memory methods.",
    "Examples:\r\n"
    "  mem format short write 0x103A22 data 0x21A\r\n"
    "Results in writing one short variable whose value is 021A,\r\n"
    "at absolute address 0x103A22\r\n",
    "",
    {BIT(1) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) |
    BIT(9)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_REPEAT_ID,
    "repeat",
    (PARAM_VAL_RULES *)&Repeat_mem_vals[0],
    (sizeof(Repeat_mem_vals) / sizeof(Repeat_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Repeat_mem_default,
    0,0,0,0,0,0,0,
    "Number of times to repeat specified write operation.",
    "Examples:\r\n"
    "  mem format short write 0x103A22 data 0x1234 0xABCD repeat 4\r\n"
    "Results in writing four pairs of short variables,\r\n"
    "0x1234 and 0xABCD,starting at absolute address 0x103A22.\r\n",
    "",
    {BIT(1) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) | BIT(9)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_VAL_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Val_mem_default,
    0,0,0,0,0,0,0,
    "Value to compare memory with, using one access method, and indicate\r\n"
    "  whether a match has been found (under timeout).",
    "Examples:\r\n"
    "  mem val_read 0x103A22 val 1 mask 0xff timeout 200 operation not_equal\r\n"
    "Results in reading one long variable (default), at absolute address\r\n"
    "0x103A22 and comparing the LS 8 bits with a value of 0x01. Once it is\r\n"
    "found to be NOT equal, within 200 microseconds, control returns to the user.\r\n"
    "If a match is not found then an indication plus value actually found\r\n"
    "are displayed\r\n",
    "",
    {BIT(10) | BIT(11)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_VAL_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Mask_mem_default,
    0,0,0,0,0,0,0,
    "Value to mask memory with, then compare, using one access method,\r\n"
    "  and indicate whether a match has been found (under timeout).",
    "Examples:\r\n"
    "  mem val_read 0x103A22 val 1 mask 0xff timeout 200 operation not_equal\r\n"
    "Results in reading one long variable (default), at absolute address\r\n"
    "0x103A22 and comparing the LS 8 bits with a value of 0x01. Once it is\r\n"
    "found to be NOT equal, within 200 microseconds, control returns to the user.\r\n"
    "If a match is not found then an indication plus value actually found\r\n"
    "are displayed\r\n",
    "",
    {BIT(10) | BIT(11)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_VAL_TIMEOUT_ID,
    "timeout",
    (PARAM_VAL_RULES *)&Timeout_mem_vals[0],
    (sizeof(Timeout_mem_vals) / sizeof(Timeout_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Timeout_mem_default,
    0,0,0,0,0,0,0,
    "Time, in microseconds, to wait for mask + compare, to indicate\r\n"
    "  whether a match has been found.",
    "Examples:\r\n"
    "  mem val_read 0x103A22 val 1 mask 0xff timeout 200 operation equal\r\n"
    "Results in reading one long variable (default), at absolute address\r\n"
    "0x103A22 and comparing the LS 8 bits with a value of 0x01. Once it is\r\n"
    "found to be equal, within 200 microseconds, control returns to the user.\r\n"
    "If a match is not found then an indication plus value actually found\r\n"
    "are displayed\r\n",
    "",
    {BIT(10) | BIT(11)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_VAL_OPERATION_ID,
    "operation",
    (PARAM_VAL_RULES *)&Operation_mem_vals[0],
    (sizeof(Operation_mem_vals) / sizeof(Operation_mem_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Operation_mem_default,
    0,0,0,0,0,0,0,
    "Operation to apply after mask + compare, to indicate\r\n"
    "  whether a match has been found.",
    "Examples:\r\n"
    "  mem val_read 0x103A22 val 1 mask 0xff timeout 200 operation equal\r\n"
    "Results in reading one long variable (default), at absolute address\r\n"
    "0x103A22 and comparing the LS 8 bits with a value of 0x01. Once it is\r\n"
    "found to be equal, within 200 microseconds, control returns to the user.\r\n"
    "If a match is not found then an indication plus value actually found\r\n"
    "are displayed\r\n",
    "",
    {BIT(10) | BIT(11)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } __DUNE_RHP */
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;

#if DEBUG_AID
/*
 * ********************************************
 * Debug only!!
 * {
 */
EXTERN CONST
   PARAM_VAL_RULES
     Abb_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
EXTERN CONST
   PARAM_VAL_RULES
     Aa_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        10,-2,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        3,
        (VAL_PROC_PTR)aa_value_checker
      }
    }
  },
  {
    VAL_SYMBOL,
    "aa_symbolic_val",
    {
      {
        1
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
EXTERN CONST
   PARAM_VAL_RULES
   Bb_vals[]
#ifdef INIT
   =
{
  {
    VAL_IP,SYMB_NAME_IP,
    {
      {
        0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;


EXTERN CONST
   PARAM_VAL_RULES
     Ab_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,SYMB_NAME_TEXT,
    {
      {
        MAX_SIZE_OF_TEXT_VAR - 1
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

EXTERN CONST
   PARAM_VAL
     Aa_default
#ifdef INIT
   =
{
  VAL_SYMBOL,1,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"aa_symbolic_val"
  }
}
#endif
;

EXTERN CONST
   PARAM
     Mex_params[]
#ifdef INIT
   =
{

  {
    PARAM_MEM_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Indefinite_num_vals[0],
    (sizeof(Indefinite_num_vals) / sizeof(Indefinite_num_vals[0])) - 1,
    FALSE,(PARAM_VAL *)0,0,0,0,0,0,0,0,
    "Read memory using a few access methods.\r\n",
    "Examples:\r\n"
    "  mem read 0x3A22\r\n"
    "Results in reading one long variable (default), starting at\r\n"
    "address 0x3A22, and displaying the result in hex format (default),\r\n"
    "on the screen\r\n",
    ""
  },
  {
    PARAM_AB_ID,
    "ab",
    (PARAM_VAL_RULES *)&Ab_vals[0],
    (sizeof(Ab_vals) / sizeof(Ab_vals[0])) - 1,
    FALSE,(PARAM_VAL *)0,0,0,0,0,0,0,0,
    "Short help on ab.\r\n",
    "Detailed help on ab\r\n",
    ""
  },
  {
    PARAM_ABB_ID,
    "abb",
    (PARAM_VAL_RULES *)&Abb_vals[0],
    (sizeof(Abb_vals) / sizeof(Abb_vals[0])) - 1,
    FALSE,(PARAM_VAL *)0,0,0,0,0,0,0,0,
    "Short help on abb.\r\n",
    "Detailed help on abb\r\n",
    ""
  },
  {
    PARAM_AA_ID,
    "aa",
    (PARAM_VAL_RULES *)&Aa_vals[0],
    (sizeof(Aa_vals) / sizeof(Aa_vals[0])) - 1,
    TRUE,(PARAM_VAL *)&Aa_default,0,0,0,0,0,0,0,
    "Short help on aa.\r\n",
    "Detailed help on aa\r\n",
    ""
  },
  {
    PARAM_BB_ID,
    "bb",
    (PARAM_VAL_RULES *)&Bb_vals[0],
    (sizeof(Bb_vals) / sizeof(Bb_vals[0])) - 1,
    FALSE,(PARAM_VAL *)0,0,0,0,0,0,0,0,
    "Short help on bb.\r\n",
    "Detailed help on bb\r\n",
    ""
  },
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;

#if !(defined(LINUX) || defined(UNIX))
/* { */

/********************************************************
*NAME
*  Sim_Chip_Ndx_vals
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  Chip in simulator.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sim_Chip_Ndx_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        0,CHIP_SIM_NOF_CHIPS,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

#endif /*} LINUX */

/********************************************************
*NAME
*  Sim_num_from_1
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  Number from 1 to 1,000,000.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sim_num_from_1[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000000, 1,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  General_num_from_0
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  Number from 1 to 1,000,000.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     General_num_from_0[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1000000, 0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  SIM_params
*TYPE: BUFFER
*DATE: 24/APR/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "sim".
*ORGANIZATION:
* look at 'PARAM' comments
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     SIM_params[]
#ifdef INIT
   =
{
  {
    PARAM_SIM_START_ID,
    "start",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Start the simulator \n\r"
    "  0 for SOC_SAND_FE200\n\r"
    "  1 for SOC_SAND_FAP10M",
    "Examples:\r\n"
    "  sim start 0 \r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_END_ID,
    "end",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ends the simulator",
    "Examples:\r\n"
    "  sim end\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_INDIRECT_DELAY_ID,
    "indirect_delay",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The delay the indirect trigger should have - the time is in milliseconds",
    "Examples:\r\n"
    "  sim indirect_delay 3000\r\n",
    "",
    {BIT(2)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_CELL_TX_DELAY_ID,
    "cell_tx_delay",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The delay the cell TX trigger should have - the time is in milliseconds",
    "Examples:\r\n"
    "  sim cell_tx_delay 3000\r\n",
    "",
    {BIT(2)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_CELL_RX_CNT_ID,
    "cell_rx_cnt",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The number of cells to receive",
    "Examples:\r\n"
    "  sim cell_rx_cnt 10\r\n",
    "",
    {BIT(2)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_TASK_WAKE_UP_ID,
    "wake_up",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Send signal to the simulator task.\r\n   Should wake it up, if on takDelay()",
    "Examples:\r\n"
    "  sim wake_up\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_TASK_SLEEP_TIME_ID,
    "sleep_time",
    (PARAM_VAL_RULES *)&Sim_num_from_1[0],
    (sizeof(Sim_num_from_1) / sizeof(Sim_num_from_1[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set simulator sleep time.\r\n"
    "  Time is miliseconds. \r\n"
    "  ****** Can not sleep less than the tick of the CPU 16 mili *****",
    "Examples:\r\n"
    "  sim sleep_time 250\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_CNTR_ENABLE_ID,
    "counter_enable",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set simulator counters module off (zero value) or on (>0)",
    "Examples:\r\n"
    "  sim counter_enable 1\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_INT_ENABLE_ID,
    "interrupt_enable",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set simulator interrupts module off (zero value) or on (>0)",
    "Examples:\r\n"
    "  sim interrupt_enable 1\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_INT_MASK_ALL_ID,
    "interrupt_mask_all",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set simulator all interrupts to the mask vale (0 or 1)",
    "Examples:\r\n"
    "  sim interrupt_mask_all 1\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_INTERRUPT_CNT_ID,
    "interrupt_cnt",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set simulator to cause #cnt interrupts",
    "Examples:\r\n"
    "  sim interrupt_cnt 10\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_TIME_MONITOR_PRINT_ID,
    "print_time_monitor",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints the time monitoring of the simultor",
    "Examples:\r\n"
    "  sim print_time_monitor\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_PRINT_CLEAR_LOG_ID,
    "print_clear_log",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints and clears the simulator log",
    "Examples:\r\n"
    "  sim print_clear_log\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_SET_SPY_LOW_MEM_LOG_ID,
    "spy_low_mem",
    (PARAM_VAL_RULES *)&General_num_from_0[0],
    (sizeof(General_num_from_0) / sizeof(General_num_from_0[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set/reset the low mem writing to log",
    "Examples:\r\n"
    "  sim spy_low_mem 1\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SIM_INTERRUPT_ASSERT_ID,
    "interrupt_assert",
    (PARAM_VAL_RULES *)&Empty_vals[0],
    (sizeof(Empty_vals) / sizeof(Empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Assert interrupt #6 - the simulator should raise it \r\n"
    " - Sim debug tool ",
    "Examples:\r\n"
    "  sim interrupt_assert\r\n",
    "",
    {BIT(1)},
    1,
    (VAL_PROC_PTR)NULL
  },
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/*
 * Example of subject without parameters.
 */
EXTERN CONST
   PARAM
     Mema_params[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/*
 * ********************************************
 * }
 */
#endif


/********************************************************
*NAME
*  Subjects_list_rom
*TYPE: BUFFER
*DATE: 15/JAN/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF SUBJECT AND
*  RELATED PARAMETERS.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'SUBJECT':
*    int   subject_id -
*      Identifier of this subject.
*    char *subj_name -
*      Ascii presentation of subject. Must be small
*      letters only! Must contain at least one character.
*    PARAM *allowed_params -
*      Array of structures of type PARAM. This is
*      the list of parameters allowed for that
*      subject.
*    unsigned int num_allowed_params -
*      Number of elements on allowed_params array.
*    char         *params_ascii_list -
*      Pointer to ascii array (null terminated)containing
*      the names of all parameters, for this subject,
*      separated by CR/LF.
*    char         *subj_short_help -
*      Short help for this subject.
*    char         *subj_detailed_help -
*      Detailed help for this subject.
*    unsigned int num_params_with_default ;
*      Number of parameters, within the allowed list, which have
*      a default value.
*    unsigned int num_params_without_default ;
*      Number of parameters, within the allowed list,
*      which do not have a default value.
*    SUBJECT_PROC subject_handler ;
*      Pointer to a procedure to call after the full
*      line has been accepted (to do the action related
*      to this subject).
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   SUBJECT
     Subjects_list_rom[]
#ifdef INIT
   =
{
#if !(defined(LINUX) || defined(UNIX))
/* { */
#if FMF_INCLUDED
/* { */
  {
    SUBJECT_FMF_SERVICES_ID,
    "fmf",
    (PARAM *)&Fmf_params[0],
    (sizeof(Fmf_params) / sizeof(Fmf_params[0])) - 1,
    (char *)0,
    "FMF - Full system management driver.",
    "Examples:\r\n"
    "  fmf \r\n"
    "This will currently reurn with an error message\r\n",
    0,0,
    (SUBJECT_PROC)subject_fmf,
    TRUE
  },
/* } */
#endif
  {
    SUBJECT_SIM_SERVICES_ID,
    "sim",
    (PARAM *)&SIM_params[0],
    (sizeof(SIM_params) / sizeof(SIM_params[0])) - 1,
    (char *)0,
    "Simulator.",
    "Examples:\r\n"
    "  sim start\r\n"
    "Do some work.\r\n",
    0,0,
    (SUBJECT_PROC)subject_sim,
    TRUE
  },
  {
    SUBJECT_GENERAL_ID,
    "general",
    (PARAM *)&General_params[0],
    (sizeof(General_params) / sizeof(General_params[0])) - 1,
    (char *)0,
    "Handling of general system parameters",
    "Examples:\r\n"
    "  general read period temperature\r\n"
    "Results in periodic (continuous) display of measured temperature.\r\n",
    0,0,
    (SUBJECT_PROC)subject_general,
    FALSE
  },
  {
    SUBJECT_NVRAM_ID,
    "nvram",
    (PARAM *)&Nvram_params[0],
    (sizeof(Nvram_params) / sizeof(Nvram_params[0])) - 1,
    (char *)0,
    "Handling of NVRAM parameters (stored in EEPROM)",
    "Examples:\r\n"
    "  nvram read block 1\r\n"
    "Results in display of contents of block no. 1 of NVRAM.\r\n"
    "  nvram\r\n"
    "Results in displaying information on all NVRAM blocks.\r\n",
    0,0,
    (SUBJECT_PROC)subject_nvram,
    TRUE
  },
  {
    SUBJECT_MEM_ID,
    "mem",
    (PARAM *)&Mem_params[0],
    (sizeof(Mem_params) / sizeof(Mem_params[0])) - 1,
    (char *)0,
    "Direct memeory access for displaying or changing",
    "Examples:\r\n"
    "  mem format long len 5 disp hex read 0x2003DA\r\n"
    "Results in reading 5 long variables, starting at address 0x2003DA,\r\n"
    "and displaying the result in hex format, on the screen.\r\n"
    "  mem\r\n"
    "Results in displaying information on all memory segments on the screen.\r\n",
    0,0,
    (SUBJECT_PROC)subject_mem,
    TRUE
  },
  {
    SUBJECT_HELP_ID,
    "help",
    (PARAM *)&Help_params[0],
    (sizeof(Help_params) / sizeof(Help_params[0])) - 1,
    (char *)0,
    "Help instructions on all system parameters and subjects",
    "Examples:\r\n"
    "  Help\r\n"
    "Results in displaying  s h o r t  help on all subjects and parameters\r\n",
    0,0,
    (SUBJECT_PROC)0,
    TRUE
  },
  {
    SUBJECT_HISTORY_ID,
    "history",
    (PARAM *)&History_params[0],
    (sizeof(History_params) / sizeof(History_params[0])) - 1,
    (char *)0,
    "Display and extract, for editing, previous command line instruction.",
    "Examples:\r\n"
    "  History len 10\r\n"
    "Results in display of last 10 command lines\r\n"
    "  History get 1\r\n"
    "Results in display of display of previous command line, with\r\n"
    "cursor at end of line, ready for editting\r\n",
    0,0,
    (SUBJECT_PROC)subject_history,
    TRUE
  },
#if INCLUDE_NET_SNMP
/* { */
  {
    SUBJECT_SNMP_ID,
    "snmp",
    (PARAM *)&Snmp_params[0],
    (sizeof(Snmp_params) / sizeof(Snmp_params[0])) - 1,
    (char *)0,
    "UI access methods to the Net-snmp agent package.",
    "Examples:\r\n"
    "  Snmp load_configuration_file\r\n"
    "Results in downloading snmpd.conf file from host\r\n"
    "  and saving it to disk (Flash)\r\n",
    0,0,
    (SUBJECT_PROC)subject_snmp,
    FALSE
  },
/* } INCLUDE_NET_SNMP */
#endif
#if LINK_FE600_LIBRARIES
  {
    SUBJECT_FE600_BSP_SERVICES_ID,
    "bsp_fe600",
    (PARAM *)&Fe600_bsp_params[0],
    (sizeof(Fe600_bsp_params) / sizeof(Fe600_bsp_params[0])) - 1,
    (char *)0,
    "UI access methods to SOC_SAND_FE600 BSP asspects.",
    "Examples:\r\n"
    "  bsp_fe600 reset_device remain_off 0\r\n"
    "Result in reseting the SOC_SAND_FE600 device\r\n",
    0,0,
    (SUBJECT_PROC)subject_fe600_bsp,
    FALSE
  },
#endif
#if LINK_FE200_LIBRARIES
  {
    SUBJECT_FE200_SERVICES_ID,
    "fe200",
    (PARAM *)&Fe200_params[0],
    (sizeof(Fe200_params) / sizeof(Fe200_params[0])) - 1,
    (char *)0,
    "UI access methods to the fe200 driver package.",
    "Examples:\r\n"
    "  fe200 start driver\r\n"
    "Results in starting the fe200 driver\r\n",
    0,0,
    (SUBJECT_PROC)subject_fe200,
    FALSE
  },
#endif
/* } */
#endif /* LINUX */
#if LINK_FE600_LIBRARIES
  {
    SUBJECT_FE600_API_SERVICES_ID,
    "fe600_api",
    (PARAM *)&Fe600_api_params[0],
    (sizeof(Fe600_api_params) / sizeof(Fe600_api_params[0])) - 1,
    (char *)0,
    "UI access methods to SOC_SAND_FE600 API asspects.",
    "Examples:\r\n"
    "  api_fe600 reset_device remain_off 0\r\n"
    "Result in reseting the SOC_SAND_FE600 device\r\n",
    0,0,
    (SUBJECT_PROC)subject_fe600_api,
    FALSE
  },
#endif
#if LINK_FAP20V_LIBRARIES
/* { */
#if !(defined(LINUX) || defined(UNIX))
/* { */
  {
    SUBJECT_FAP20V_SERVICES_ID,
    "fap20v",
    (PARAM *)&Fap20v_params[0],
    (sizeof(Fap20v_params) / sizeof(Fap20v_params[0])) - 1,
    (char *)0,
    "UI access methods to the fap20v driver package.",
    "Examples:\r\n"
    "  fap20v links get_links_status\r\n"
    "Results in getting link status of fap20v number 0 on the line-card\r\n",
    0,0,
    (SUBJECT_PROC)subject_fap20v,
    TRUE
  },
/*} LINUX */
#endif
  {
    SUBJECT_FAP20V_B_SERVICES_ID,
    "fap20v",
    (PARAM *)&Fap20v_b_params[0],
    (sizeof(Fap20v_b_params) / sizeof(Fap20v_b_params[0])) - 1,
    (char *)0,
    "UI access methods to the fap20v driver package.",
    "Examples:\r\n"
    "  fap20v links get_links_status\r\n"
    "Results in getting link status of fap20v number 0 on the line-card\r\n",
    0,0,
    (SUBJECT_PROC)subject_fap20v_b,
    TRUE
  },
/* } */
#endif /*LINK_FAP20V_LIBRARIES*/

#if LINK_PSS_LIBRARIES
/*{ */
  {
    SUBJECT_FAP10M_SERVICES_ID,
    "fap10m",
    (PARAM *)&Fap10m_params[0],
    (sizeof(Fap10m_params) / sizeof(Fap10m_params[0])) - 1,
    (char *)0,
    "UI access methods to the fap10m driver package.",
    "Examples:\r\n"
    "  fap10m links get_links_status\r\n"
    "Results in getting link status of fap10m number 0 on the line-card\r\n",
    0,0,
    (SUBJECT_PROC)subject_fap10m,
    TRUE
  },
/*
    {
    SUBJECT_FAP10M_SERVICES_ID,
    "fap10m",
    (PARAM *)&Fap10m_params[0],
    (sizeof(Fap10m_params) / sizeof(Fap10m_params[0])) - 1,
    (char *)0,
    "UI access methods to the fap10m driver package.",
    "Examples:\r\n"
    "  fap10m links get_links_status\r\n"
    "Results in getting link status of fap10m number 0 on the line-card\r\n",
    0,0,
    (SUBJECT_PROC)subject_fap10m,
    TRUE
  },
*/
/*} */
#endif


#ifdef __DUNE_HRP__
/* { */
  {
    SUBJECT_DHRP_SERVICES_ID,
    "dhrp",
    (PARAM *)&Dhrp_params[0],
    (sizeof(Dhrp_params) / sizeof(Dhrp_params[0])) - 1,
    (char *)0,
    "UI access methods to the DHRP package.",
    "Examples:\r\n"
    "  dhrp bsp\r\n"
    "Contain all the BSP related BSP pacage\r\n",
    0,0,
    (SUBJECT_PROC)subject_dhrp,
    TRUE
  },
#endif /* } __DUNE_HRP__ */
  {
    SUBJECT_SAND_SERVICES_ID,
    "soc_sand",
    (PARAM *)&Soc_sand_params[0],
    (sizeof(Soc_sand_params) / sizeof(Soc_sand_params[0])) - 1,
    (char *)0,
    "UI access methods to the SOC_SAND driver package.",
    "",
    0,0,
    (SUBJECT_PROC)subject_sand,
    FALSE
  },
#if !(defined(LINUX) || defined(UNIX))
/* { */

#if (LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)
  {
    SUBJECT_TGS_SERVICES_ID,
    "tgs",
    (PARAM *)&Tgs_params[0],
    (sizeof(Tgs_params) / sizeof(Tgs_params[0])) - 1,
    (char *)0,
    "BRD00XXX - Line-card TGS Utilities.",
    "",
    0,0,
    (SUBJECT_PROC)subject_line_tgs,
    FALSE
  },
#endif /*LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES*/
  /* fap20v sweep application */
#if LINK_FAP20V_LIBRARIES
  {
    SUBJECT_FAP20V_SWEEP_APP_SERVICES_ID,
    "sweep",
    (PARAM *)&Sweep_app_params[0],
    (sizeof(Sweep_app_params) / sizeof(Sweep_app_params[0])) - 1,
    (char *)0,
    "Fap20v Sweep application Utilities.",
    "",
    0,0,
    (SUBJECT_PROC)subject_fap20v_sweep_app,
    FALSE
  },
#endif /*LINK_FAP20V_LIBRARIES*/

  /* fap20v sweep application */
#if LINK_FAP20V_LIBRARIES
{
  SUBJECT_FAP20V_SWEEP_APP_B_SERVICES_ID,
    "sweep",
    (PARAM *)&Sweep_app_b_params[0],
    (sizeof(Sweep_app_b_params) / sizeof(Sweep_app_b_params[0])) - 1,
    (char *)0,
    "Fap20v Sweep application B Utilities.",
    "",
    0,0,
    (SUBJECT_PROC)subject_fap20v_sweep_app_b,
    FALSE
  },
#endif /*LINK_FAP20V_LIBRARIES*/
/* } */
#endif /* LINUX */

#ifdef __DUNE_HRP__
/* { */
{
  SUBJECT_DHRP_API_SERVICES_ID,
  "dhrp_api",
  (PARAM *)&dhrp_api_params[0],
  (sizeof(dhrp_api_params) / sizeof(dhrp_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_dhrp_api,
  FALSE
},
#endif /* } __DUNE_HRP__ */
#if LINK_TIMNA_LIBRARIES
{
  SUBJECT_TIMNA_ACCESS_SERVICES_ID,
  "timna_app",
  (PARAM *)&sweept20_api_params[0],
  (sizeof(sweept20_api_params) / sizeof(sweept20_api_params[0])) - 1,
  (char *)0,
  "Timna application",  /*TODO!! - Add doc.*/
  "",
  0,0,
  (SUBJECT_PROC)subject_sweept20_api,
  FALSE
},
{
  SUBJECT_TIMNA_ACCESS_SERVICES_ID,
  "timna_acc",
  (PARAM *)&Access_timna_params[0],
  (sizeof(Access_timna_params) / sizeof(Access_timna_params[0])) - 1,
  (char *)0,
  "Read and Write Timna access",  /*TODO!! - Add doc.*/
  "",
  0,0,
  (SUBJECT_PROC)subject_access_timna,
  FALSE
},
{
  SUBJECT_TIMNA_API_SERVICES_ID,
  "timna_api",
  (PARAM *)&timna_api_params[0],
  (sizeof(timna_api_params) / sizeof(timna_api_params[0])) - 1,
  (char *)0,
  "Timna APIs",  /*TODO!! - Add doc.*/
  "",
  0,0,
  (SUBJECT_PROC)subject_timna_api,
  FALSE
},

{
  SUBJECT_TEVB_SERVICES_ID,
  "tevb",
  (PARAM *)&Tevb_params[0],
  (sizeof(Tevb_params) / sizeof(Tevb_params[0])) - 1,
  (char *)0,
  "Line-card TEVB Utilities.",
  "",
  0,0,
  (SUBJECT_PROC)subject_line_tevb,
  FALSE
},
#endif /* LINK_TIMNA_LIBRARIES*/
#if LINK_PETRA_LIBRARIES
{
  SUBJECT_PETRA_API_SERVICES_ID,
  "petra_api",
  (PARAM *)&soc_petra_api_params[0],
  (sizeof(soc_petra_api_params) / sizeof(soc_petra_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_petra_api,
  FALSE
},
#if LINK_TMD_LIBRARIES
{
  SUBJECT_TMD_API_SERVICES_ID,
  "tmd_api",
  (PARAM *)&tmd_api_params[0],
  (sizeof(tmd_api_params) / sizeof(tmd_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_tmd_api,
  FALSE
},
#endif

{
  SUBJECT_PETRA_ACC_SERVICES_ID,
  "acc_petra",
  (PARAM *)&soc_petra_acc_params[0],
  (sizeof(soc_petra_acc_params) / sizeof(soc_petra_acc_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_petra_acc,
  FALSE
},
#ifndef __DUNE_SSF__
{
  SUBJECT_SWP_PETRA_SERVICES_ID,
  "swp_petra",
  (PARAM *)&swp_p_api_params[0],
  (sizeof(swp_p_api_params) / sizeof(swp_p_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_swp_p_api,
  FALSE
},
{
  SUBJECT_PTG_SERVICES_ID,
  "acc_ptg",
  (PARAM *)&ptg_params[0],
  (sizeof(ptg_params) / sizeof(ptg_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_ptg,
  FALSE
},
#endif
#if PETRA_PP
#ifndef PETRA_PP_MINIMAL
  {
  SUBJECT_PETRA_PP_API_SERVICES_ID,
  "np_petra_api",
  (PARAM *)&soc_petra_pp_api_params[0],
  (sizeof(soc_petra_pp_api_params) / sizeof(soc_petra_pp_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_petra_pp_api,
  FALSE
},
{
  SUBJECT_PETRA_PP_ACC_SERVICES_ID,
  "np_petra_acc",
  (PARAM *)&soc_petra_pp_acc_params[0],
  (sizeof(soc_petra_pp_acc_params) / sizeof(soc_petra_pp_acc_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_petra_pp_acc,
  FALSE
},
#endif
#ifndef __DUNE_SSF__
#ifndef PETRA_PP_MINIMAL
{
  SUBJECT_PETRA_PP_SWP_SERVICES_ID,
  "swp_petra_pp",
  (PARAM *)&swp_p_pp_api_params[0],
  (sizeof(swp_p_pp_api_params) / sizeof(swp_p_pp_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_swp_p_pp_api,
  FALSE
},
#endif
{
  SUBJECT_PETRA_PP_GSA_SERVICES_ID,
  "app_petra_pp",
  (PARAM *)&gsa_api_params[0],
  (sizeof(gsa_api_params) / sizeof(gsa_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_gsa_api,
  FALSE
},
{
  SUBJECT_PETRA_GSA_SERVICES_ID,
  "app_petra",
  (PARAM *)&gsa_petra_api_params[0],
  (sizeof(gsa_petra_api_params) / sizeof(gsa_petra_api_params[0])) - 1,
  (char *)0,
  "description.",
  "",
  0,0,
  (SUBJECT_PROC)subject_gsa_petra_api,
  FALSE
},

#endif /* __DUNE_SSF__ */
#endif /* PETRA_PP */
#endif/* LINK_PETRA_LIBRARIES*/
#if LINK_T20E_LIBRARIES
  {
    SUBJECT_T20E_ACC_SERVICES_ID,
    "t20e_acc",
    (PARAM *)&t20e_acc_params[0],
    (sizeof(t20e_acc_params) / sizeof(t20e_acc_params[0])) - 1,
    (char *)0,
    "description.",
    "",
    0,0,
    (SUBJECT_PROC)subject_t20e_acc,
    FALSE
  },
  {
    SUBJECT_OAM_ACC_SERVICES_ID,
    "oam_acc",
    (PARAM *)&Oam_acc_params[0],
    (sizeof(Oam_acc_params) / sizeof(Oam_acc_params[0])) - 1,
    (char *)0,
    "description.",
    "",
    0,0,
    (SUBJECT_PROC)subject_oam_acc,
    FALSE
  },
  {
    SUBJECT_OAM_API_SERVICES_ID,
    "oam_api",
    (PARAM *)&Oam_api_params[0],
    (sizeof(Oam_api_params) / sizeof(Oam_api_params[0])) - 1,
    (char *)0,
    "description.",
    "",
    0,0,
    (SUBJECT_PROC)subject_oam_api,
    FALSE
  },  
#endif /* LINK_T20E_LIBRARIES*/
#if LINK_PPD_LIBRARIES
  {
    SUBJECT_PPD_API_SERVICES_ID,
    "ppd_api",
    (PARAM *)&soc_ppd_api_params[0],
    (sizeof(soc_ppd_api_params) / sizeof(soc_ppd_api_params[0])) - 1,
    (char *)0,
    "description.",
    "",
    0,0,
    (SUBJECT_PROC)subject_ppd_api,
    FALSE
  },
#endif /* LINK_T20E_LIBRARIES*/

/*
 * Last element. Do not remove.
 */
  {
    SUBJECT_END_OF_LIST,
    "zzzzzzzzzzzzzzz"
  }
}
#endif
;
#ifdef INIT
  void Memory_block_rom_00(){}
#endif


#ifdef INIT
  void Memory_block_rom_01(){}
#endif

#ifdef INIT
  void Memory_block_rom_128(){}
#endif

#ifdef INIT
  void Memory_block_rom_02(){}
#endif

/*
 * We do not use 'sizeof(FAP20V_REGS)', because it might not
 * be complete.
 */
#define FAP20V_MEM_BYTE_SIZE  SOC_SAND_MAX( (0x10000-1), sizeof(FAP20V_REGS))
#define FAP21V_MEM_BYTE_SIZE  SOC_SAND_MAX( (0x10000-1), sizeof(FAP20V_REGS))
/*
 * This is the new PCI mezzanine block on GFA line-card.
 */
  #ifdef INIT
  void Memory_block_rom_03(){}
  #endif

/*
 * This is the new PCI mezzanine block on GFA-MB line-card.
 */
#ifdef INIT
  void Memory_block_rom_gfa_mb(){}
#endif

#ifdef INIT
  void Memory_block_rom_gfa_fap21v(){}
#endif

#ifdef INIT
  void Memory_block_rom_gfa_petra(){}
#endif

#ifdef INIT
  void Memory_block_rom_gfa_petra_streaming(){}
#endif

#ifdef INIT
  void Memory_block_rom_gfa_bi(){}
#endif

/*
 * This is the TEVB stand-alone host mode
 */
  #ifdef INIT
  void Memory_block_rom_04(){}
  #endif

