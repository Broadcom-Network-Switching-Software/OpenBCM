/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_pp_oam_mep_db.h
 */

#ifndef __QAX_PP_OAM_MEP_DB_INCLUDED__
#define __QAX_PP_OAM_MEP_DB_INCLUDED__






#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>













typedef struct {
    SHR_BITDCL  *flexible_verification_use_indicator;
} qax_pp_oam_mep_db_info_t;










soc_error_t
soc_qax_pp_oam_oamp_lm_dm_pointed_shared_find(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


soc_error_t
soc_qax_pp_oam_oamp_lm_dm_set(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


soc_error_t
soc_qax_pp_oam_oamp_lm_dm_delete(
   int unit,
   ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO *lm_dm_info
   );


soc_error_t
soc_qax_pp_oam_oamp_lm_dm_search(
   int unit,
   uint32 endpoint_id,
   uint32 *found_bmp
   );



soc_error_t soc_qax_pp_oam_oamp_dm_get(
   int unit,
   SOC_PPC_OAM_OAMP_DM_INFO_GET *dm_info,
   uint8 *is_1DM
   );


soc_error_t soc_qax_pp_oam_oamp_lm_get(
    int unit,
    SOC_PPC_OAM_OAMP_LM_INFO_GET *lm_info
    );



soc_error_t
qax_pp_oam_bfd_flexible_verification_init(int unit);


soc_error_t 
qax_pp_oam_bfd_flexible_verification_set(
   int unit, 
   SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info
   );



soc_error_t 
qax_pp_oam_bfd_flexible_verification_get(
   int unit, 
   SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info
   );


soc_error_t
qax_pp_oam_bfd_flexible_verification_delete(int unit, SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info);


soc_error_t
qax_pp_oam_bfd_mep_db_ext_data_set(int unit, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO *info);



soc_error_t
qax_pp_oam_bfd_mep_db_ext_data_get(int unit, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO *info);




soc_error_t
qax_pp_oam_bfd_mep_db_ext_data_delete(int unit, int extension_idx);


#endif 
