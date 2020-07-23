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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_sa_auth.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_vid_assign.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_cos.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_trap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mymac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ptp.h>









#define ARAD_PP_ERRORS_DESC_VALIDATE(module, err_num) \
  do { \
    if (ARAD_PP_ERR_DESC_BASE_##module##_LAST <= ARAD_PP_##module##_ERR_LAST) \
    { \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERRORS_FOUND_DUPLICATES_ERR, err_num, exit); \
    } \
  } while (0)

#define ARAD_PP_PROCS_DESC_VALIDATE(module, err_num) \
  do { \
  if (ARAD_PP_PROC_DESC_BASE_##module##_LAST <= ARAD_PP_##module##_PROCEDURE_DESC_LAST) \
    { \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_PROCS_FOUND_DUPLICATES_ERR, err_num, exit); \
    } \
  } while (0)














uint32
  arad_pp_procedure_desc_add(void)
{
  uint32
    res;
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc[100];
  int32
    nof_modules = 0;
  static uint8
    Arad_pp_proc_desc_initialized  = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PP_PROCEDURE_DESC_ADD);

  if (Arad_pp_proc_desc_initialized == TRUE)
  {
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }

  ARAD_PP_PROCS_DESC_VALIDATE(GENERAL, 10);
  ARAD_PP_PROCS_DESC_VALIDATE(LLP_PARSE, 20);
  ARAD_PP_PROCS_DESC_VALIDATE(LLP_SA_AUTH, 30);
  ARAD_PP_PROCS_DESC_VALIDATE(LLP_VID_ASSIGN, 40);
  ARAD_PP_PROCS_DESC_VALIDATE(LLP_FILTER, 50);
  ARAD_PP_PROCS_DESC_VALIDATE(LLP_COS, 60);
  ARAD_PP_PROCS_DESC_VALIDATE(LLP_TRAP, 70);
  ARAD_PP_PROCS_DESC_VALIDATE(LLP_MIRROR, 80);
  ARAD_PP_PROCS_DESC_VALIDATE(MYMAC, 90);
  ARAD_PP_PROCS_DESC_VALIDATE(FRWRD_BMACT, 220);
  ARAD_PP_PROCS_DESC_VALIDATE(LAG, 330);
  ARAD_PP_PROCS_DESC_VALIDATE(METERING, 350);
  ARAD_PP_PROCS_DESC_VALIDATE(ISEM_ACCESS, 400);
  ARAD_PP_PROCS_DESC_VALIDATE(OAM, 500);
  ARAD_PP_PROCS_DESC_VALIDATE(SRC_BIND, 510);
  ARAD_PP_PROCS_DESC_VALIDATE(PTP, 520);

  procedure_desc[nof_modules++] = arad_pp_general_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_llp_parse_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_llp_sa_auth_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_llp_vid_assign_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_llp_filter_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_llp_cos_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_llp_trap_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_llp_mirror_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_mymac_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_frwrd_ilm_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_frwrd_bmact_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_lag_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_metering_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_isem_access_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_oam_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_src_bind_get_procs_ptr();
  procedure_desc[nof_modules++] = arad_pp_ptp_get_procs_ptr();

  res = soc_sand_add_proc_id_pools(procedure_desc, nof_modules) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);

  Arad_pp_proc_desc_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_procedure_desc_add()",0,0);

}

uint32
  arad_pp_errors_desc_add(void)
{
  uint32
    res;
  CONST SOC_ERROR_DESC_ELEMENT
    *error_desc[100];
  int32
    nof_modules = 0;
  static uint8
    Arad_pp_error_pool_initialized = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PP_ERRORS_DESC_ADD);

  if (Arad_pp_error_pool_initialized == TRUE)
  {
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }

  ARAD_PP_ERRORS_DESC_VALIDATE(GENERAL, 10);
  ARAD_PP_ERRORS_DESC_VALIDATE(LLP_PARSE, 20);
  ARAD_PP_ERRORS_DESC_VALIDATE(LLP_SA_AUTH, 30);
  ARAD_PP_ERRORS_DESC_VALIDATE(LLP_VID_ASSIGN, 40);
  ARAD_PP_ERRORS_DESC_VALIDATE(LLP_FILTER, 50);
  ARAD_PP_ERRORS_DESC_VALIDATE(LLP_COS, 60);
  ARAD_PP_ERRORS_DESC_VALIDATE(LLP_TRAP, 70);
  ARAD_PP_ERRORS_DESC_VALIDATE(LLP_MIRROR, 80);
  ARAD_PP_ERRORS_DESC_VALIDATE(MYMAC, 90);
  ARAD_PP_ERRORS_DESC_VALIDATE(FRWRD_BMACT, 220);
  ARAD_PP_ERRORS_DESC_VALIDATE(LAG, 330);
  ARAD_PP_ERRORS_DESC_VALIDATE(METERING, 350);
  ARAD_PP_ERRORS_DESC_VALIDATE(EG_ENCAP_ACCESS, 460);
  ARAD_PP_ERRORS_DESC_VALIDATE(OAM, 470);
  ARAD_PP_ERRORS_DESC_VALIDATE(SRC_BIND, 480);
  ARAD_PP_ERRORS_DESC_VALIDATE(PTP, 490);

  error_desc[nof_modules++] = arad_pp_general_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_llp_parse_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_llp_sa_auth_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_llp_vid_assign_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_llp_filter_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_llp_cos_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_llp_trap_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_llp_mirror_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_mymac_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_frwrd_ilm_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_frwrd_bmact_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_eg_encap_access_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_lag_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_metering_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_oam_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_src_bind_get_errs_ptr();
  error_desc[nof_modules++] = arad_pp_ptp_get_errs_ptr();

  res = soc_sand_add_error_pools(error_desc, nof_modules) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  Arad_pp_error_pool_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_errors_desc_add()",0,0);

}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

