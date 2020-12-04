
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __QAX_PP_EG_ENCAP_ACCESS_INCLUDED__

#define __QAX_PP_EG_ENCAP_ACCESS_INCLUDED__





#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>




#define EDB_ARRAY_AS_CORE_BLOCK (SOC_IS_QAX(unit) ? 1 : 2)




typedef struct
{
  uint32 remark_profile;
  uint32 oam_lif_set;
  uint32 drop;
  uint32 outlif_profile;
} QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT;









soc_error_t
  soc_qax_pp_eg_rif_profile_set(
    int                 unit,
    uint32              rif_profile_index,
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data
  );

soc_error_t
  soc_qax_pp_eg_rif_profile_get(
    int                 unit,
    uint32              rif_profile_index,
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data
  );

soc_error_t
  soc_qax_pp_eg_rif_profile_delete(
    int                 unit,
    uint32              rif_profile_index
  );

uint32
  qax_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
    int                 unit,
    uint32              outlif,
    ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  );

uint32
  qax_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
    int                 unit,
    uint32              outlif,
    ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

