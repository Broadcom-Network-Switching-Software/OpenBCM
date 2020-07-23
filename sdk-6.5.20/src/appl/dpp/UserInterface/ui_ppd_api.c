/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
* Utilities include file.
*/

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>

#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_parse.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_sa_auth.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_vid_assign.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_filter.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif_ing_vlan_edit.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_cos.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_llp_trap.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_mymac.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_mpls_term.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_vsi.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_rif.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif_cos.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif_table.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_mact.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_mact_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_ipv4.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_ipv6.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_ilm.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_bmact.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_trill.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_fec.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_filter.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_encap.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_ac.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_vlan_edit.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_qos.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_mirror.h> 
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_port.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lag.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_metering.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_diag.h>                                                               
/* #include <appl/dpp/UserInterface/ppd/ui_ppd_api_acl.h> */
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_fp.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_frwrd_ipv4_test.h>
#include <appl/dpp/UserInterface/ppd/ui_arad_pp_frwrd_ipv4_test.h>

static
  uint32
    Default_unit = 0;
static
  uint32
    Default_units[SOC_SAND_MAX_DEVICE] = {0};
static
  uint32
    Num_default_units = 1;

void
    soc_petra_set_default_units(uint32 dev_ids[SOC_SAND_MAX_DEVICE], uint32 nof_devices)
{
  sal_memcpy(Default_units, dev_ids, sizeof(uint32) * nof_devices);

  Num_default_units = nof_devices;
}

void
  soc_petra_get_default_units(uint32 dev_ids[SOC_SAND_MAX_DEVICE], uint32 *nof_devices)
{
  sal_memcpy(dev_ids, Default_units, sizeof(uint32) * Num_default_units);

  *nof_devices = Num_default_units;
}


void
  soc_petra_set_default_unit(uint32 dev_id)
{
  Default_unit = dev_id;

  if (Num_default_units == 0)
  {
    uint32
      dev_ids[SOC_SAND_MAX_DEVICE];

    dev_ids[0] = dev_id;
    soc_petra_set_default_units(dev_ids, 1);
  }
}

uint32
  soc_petra_get_default_unit()
{
  return Default_unit;
}

uint32
  soc_ppd_get_default_unit()
{
  return soc_petra_get_default_unit();
}

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h> 
#include <soc/dpp/ARAD/arad_api_general.h> 
/******************************************************************** 
 *  Function handler: olp_pge_mem_tbl_set (section soc_pb_ind_acc)
 */
int 
  ui_arad_reg_acc_ing_signal_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_blk,
    tmp,
    ind,
    is_eg = FALSE,
    prm_addr_msb = 0,
    prm_addr_lsb=0,
    prm_fld_msb = 0,
    out_len =8,
    prm_fld_lsb = 0,
    prm_core = 0,
    dump;
  ARAD_PP_DIAG_REG_FIELD
    prm_fld;
  int
    index;
  uint32
    val[ARAD_PP_DIAG_DBG_VAL_LEN];
   
  UI_MACROS_INIT_FUNCTION("ui_arad_reg_acc_ing_signal_get_unsafe"); 
  soc_sand_proc_name = "ui_arad_reg_acc_ing_signal_get_unsafe"; 

  unit = soc_ppd_get_default_unit(); 
  sal_memset(val,0x0,ARAD_PP_DIAG_DBG_VAL_LEN * 4);
  prm_blk = ARAD_IHP_ID;
  
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_BLOCK_ID_IS_EG,1))          
  {  
    is_eg = TRUE;
    prm_blk = ARAD_EGQ_ID;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_BLOCK_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_BLOCK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    tmp = (uint32)param_val->value.ulong_value;
    if (tmp == 1)
    {
      if (is_eg)
      {
        prm_blk = ARAD_EPNI_ID;
        out_len = 24;
      }
      else
      {
        prm_blk = ARAD_IHB_ID;
      }
    }
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_CORE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_core = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_ADDR_HIGH_ID,1))  {        
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_ADDR_HIGH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_addr_msb = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_ADDR_LOW_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_ADDR_LOW_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_addr_lsb = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_LSB_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_lsb = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_MSB_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_MSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_msb = (uint32)param_val->value.ulong_value;
  } 

    (prm_fld).base = (prm_addr_msb << 16) + prm_addr_lsb;
    (prm_fld).msb = (uint8)prm_fld_msb;
    (prm_fld).lsb= (uint8)prm_fld_lsb;


  /* This is a set function, so call GET function first */                                                                                
  ret = arad_pp_diag_dbg_val_get_unsafe(
          unit,
          prm_core,
          prm_blk,
          &prm_fld,
          val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** arad_pp_diag_dbg_val_get_unsafe - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "arad_pp_diag_dbg_val_get_unsafe");   
    goto exit; 
  } 

  for(ind = 0; ind < out_len; ++ind)
  {
    soc_sand_os_printf( "buff[%u]: 0x%08x\n\r",ind,val[ind]);
  }

  dump = (prm_fld).msb - (prm_fld).lsb + 1;  
  if (dump <= 256 && dump > 0) {
    soc_sand_os_printf("RAW: 0x");    
    for (index = (dump / 32); index >= 0; --index) {
      soc_sand_os_printf("%08x", val[index]);
    }    
    soc_sand_os_printf("\n\r");
  }  
  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: olp_pge_mem_tbl_set (section soc_pb_ind_acc)
 */
int 
  ui_arad_reg_acc_signal_dump(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_arad_reg_acc_signal_dump"); 
  soc_sand_proc_name = "ui_arad_reg_acc_signal_dump"; 

  /* Get parameters */ 
  unit = soc_ppd_get_default_unit(); 
  /* This is a set function, so call GET function first */                                                                                
  ret = arad_diag_signals_dump(
          unit,
          0,
          0
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** arad_diag_signals_dump - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "arad_diag_signals_dump");   
    goto exit; 
  } 
  
  goto exit; 
exit: 
  return ui_ret; 
} 

int 
  ui_arad_reg_access( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_arad_reg_access"); 

  if (!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNAL_ING_READ_ID,1))
  {
    ret = ui_arad_reg_acc_ing_signal_get_unsafe(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_ARAD_REG_ACCESS_SIGNALS_DUMP_ID,1)) 
  { 
    ret = ui_arad_reg_acc_signal_dump(current_line); 
  }
  
  ui_ret = ret; 

  goto exit; 
exit:        
  return ui_ret; 
}         


/*****************************************************
*NAME
*  subject_ppd_api
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'subject_ppd_api' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_ppd_api(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_ppd_api(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ret;

  ret = FALSE ;

  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\n\r",FALSE) ;
    send_string_to_screen("'subject_ppd_api()' function was called with no parameters.\n\r",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_ppd_api').
   */

  send_array_to_screen("\n\r",2) ;

#ifdef UI_LLP_PARSE
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LLP_PARSE_ID,1))
  {
    ret = ui_ppd_api_llp_parse(current_line);
  }
#endif /* UI_LLP_PARSE */
#ifdef UI_LLP_SA_AUTH
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LLP_SA_AUTH_ID,1))
  {
    ret = ui_ppd_api_llp_sa_auth(current_line);
  }
#endif /* UI_LLP_SA_AUTH */
#ifdef UI_LLP_VID_ASSIGN
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LLP_VID_ASSIGN_ID,1))
  {
    ret = ui_ppd_api_llp_vid_assign(current_line);
  }
#endif /* UI_LLP_VID_ASSIGN */
#ifdef UI_LLP_FILTER
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LLP_FILTER_ID,1))
  {
    ret = ui_ppd_api_llp_filter(current_line);
  }
#endif /* UI_LLP_FILTER */
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LIF_ING_VLAN_EDIT_ID,1))
  {
    ret = ui_ppd_api_lif_ing_vlan_edit(current_line);
  }
#ifdef UI_LLP_COS
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LLP_COS_ID,1))
  {
    ret = ui_ppd_api_llp_cos(current_line);
  }
#endif /* UI_LLP_COS */
#ifdef UI_LLP_TRAP
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LLP_TRAP_ID,1))
  {
    ret = ui_ppd_api_llp_trap(current_line);
  }
#endif /* UI_LLP_TRAP */
#ifdef UI_MYMAC
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_MYMAC_ID,1))
  {
    ret = ui_ppd_api_mymac(current_line);
  }
#endif /* UI_MYMAC */
#ifdef UI_MPLS_TERM
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_MPLS_TERM_ID,1))
  {
    ret = ui_ppd_api_mpls_term(current_line);
  }
#endif /* UI_MPLS_TERM */
#ifdef UI_VSI
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_VSI_ID,1))
  {
    ret = ui_ppd_api_vsi(current_line);
  }
#endif /* UI_VSI */
#ifdef UI_LIF
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LIF_ID,1))
  {
    ret = ui_ppd_api_lif(current_line);
  }
#endif /* UI_LIF */
#ifdef UI_RIF
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_RIF_ID,1))
  {
    ret = ui_ppd_api_rif(current_line);
  }
#endif /* UI_RIF */
#ifdef UI_LIF_COS
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LIF_COS_ID,1))
  {
    ret = ui_ppd_api_lif_cos(current_line);
  }
#endif /* UI_LIF_COS */
#ifdef UI_FRWRD_MACT
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_MACT_ID,1))
  {
    ret = ui_ppd_api_frwrd_mact(current_line);
  }
#endif /* UI_FRWRD_MACT */
#ifdef UI_FRWRD_MACT_MGMT
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_MACT_MGMT_ID,1))
  {
    ret = ui_ppd_api_frwrd_mact_mgmt(current_line);
  }
#endif /* UI_FRWRD_MACT_MGMT */
#ifdef UI_FRWRD_IPV4
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_IPV4_ID,1))
  {
    ret = ui_ppd_api_frwrd_ipv4(current_line);
  }
#endif /* UI_FRWRD_IPV4 */
#ifdef UI_FRWRD_IPV6
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_IPV6_ID,1))
  {
    ret = ui_ppd_api_frwrd_ipv6(current_line);
  }
#endif /* UI_FRWRD_IPV6 */
#ifdef UI_FRWRD_ILM
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_ILM_ID,1))
  {
    ret = ui_ppd_api_frwrd_ilm(current_line);
  }
#endif /* UI_FRWRD_ILM */
#ifdef UI_FRWRD_BMACT
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_BMACT_ID,1))
  {
    ret = ui_ppd_api_frwrd_bmact(current_line);
  }
#endif /* UI_FRWRD_BMACT */
#ifdef UI_FRWRD_TRILL
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_TRILL_ID,1))
  {
    ret = ui_ppd_api_frwrd_trill(current_line);
  }
#endif /* UI_FRWRD_TRILL */
#ifdef UI_FRWRD_EXTEND_P2P
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_EXTEND_P2P_ID,1))
  {
    ret = ui_ppd_api_frwrd_extend_p2p(current_line);
  }
#endif /* UI_FRWRD_EXTEND_P2P */
#ifdef UI_FRWRD_FEC
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FRWRD_FEC_ID,1))
  {
    ret = ui_ppd_api_frwrd_fec(current_line);
  }
#endif /* UI_FRWRD_FEC */
#ifdef UI_EG_FILTER
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_EG_FILTER_ID,1))
  {
    ret = ui_ppd_api_eg_filter(current_line);
  }
#endif /* UI_EG_FILTER */
#ifdef UI_EG_ENCAP
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_EG_ENCAP_ID,1))
  {
    ret = ui_ppd_api_eg_encap(current_line);
  }
#endif /* UI_EG_ENCAP */
#ifdef UI_EG_AC
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_EG_AC_ID,1))
  {
    ret = ui_ppd_api_eg_ac(current_line);
  }
#endif /* UI_EG_AC */
#ifdef UI_EG_VLAN_EDIT
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_EG_VLAN_EDIT_ID,1))
  {
    ret = ui_ppd_api_eg_vlan_edit(current_line);
  }
#endif /* UI_EG_VLAN_EDIT */
#ifdef UI_EG_QOS
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_EG_QOS_ID,1))
  {
    ret = ui_ppd_api_eg_qos(current_line);
  }
#endif /* UI_EG_QOS */
#ifdef UI_EG_MIRROR
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_EG_MIRROR_ID,1)) 
  { 
    ret = ui_ppd_api_eg_mirror(current_line); 
  } 
#endif /* UI_EG_MIRROR */
#ifdef UI_PORT
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_PORT_ID,1))
  {
    ret = ui_ppd_api_port(current_line);
  }
#endif /* UI_PORT */
#ifdef UI_LAG
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_LAG_ID,1)) 
  { 
    ret = ui_ppd_api_lag(current_line); 
  } 
#endif /* UI_LAG */
#ifdef UI_TRAP_MGMT
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_TRAP_MGMT_ID,1))
  {
    ret = ui_ppd_api_trap_mgmt(current_line);
  }
#endif /* UI_TRAP_MGMT */
#ifdef UI_METERING
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_METERING_ID,1))
  {
    ret = ui_ppd_api_metering(current_line);
  }
#endif /* UI_METERING */
#ifdef UI_DIAG
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_DIAG_ID,1)) 
  { 
    ret = ui_ppd_api_diag(current_line); 
  } 
#endif /* UI_DIAG */
#ifdef UI_FP
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_FP_ID,1)) 
  { 
    ret = ui_ppd_api_fp(current_line); 
  } 
#endif /* UI_FP */
#ifdef UI_ARAD_SIG_READ_ACC
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_API_ARAD_REG_ACCESS_ID,1)) 
  { 
    ret = ui_arad_reg_access(current_line); 
  } 
#endif /* UI_ARAD_SIG_READ_ACC */
#ifdef UI_IPV4_UC_TEST
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST3_ID,1)) 
  { 

#ifdef BCM_PETRA_SUPPORT
    ret = ui_ppd_frwrd_ipv4_test_arad_pp(current_line); 
#endif /* BCM_PETRA_SUPPORT */

  } 
#endif /* UI_ipv4_test*/
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\n\r"
      "*** soc_ppd_api command with unknown parameters'.\n\r"
      "    Syntax error/sw error...\n\r",
      TRUE)  ;
    ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ret);
}

