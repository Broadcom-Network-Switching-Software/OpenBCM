/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_JER_PP_DIAG_INCLUDED__

#define __SOC_JER_PP_DIAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>











#define JER_PP_DIAG_FLD_READ(prm_fld, core_id, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb, prm_err_num)  \
  ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb);    \
  rv = arad_pp_diag_dbg_val_get_unsafe(      \
          unit,         \
          core_id,      \
          prm_blk,      \
          prm_fld,      \
          regs_val      \
        );              \
  SOCDNX_IF_ERR_EXIT(rv);














soc_error_t
soc_jer_diag_glem_signals_get(int unit, int core_id, soc_ppc_diag_glem_signals_t *result);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



