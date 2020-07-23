/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __JER_PP_EM_SER_INCLUDED__
#define __JER_PP_EM_SER_INCLUDED__


#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dcmn/dcmn_intr_corr_act_func.h>



typedef enum
{
    JER_PP_EM_SER_BLOCK_IHB,
    JER_PP_EM_SER_BLOCK_PPDB_A,
    JER_PP_EM_SER_BLOCK_PPDB_B,
    JER_PP_EM_SER_BLOCK_EDB,
    JER_PP_EM_SER_BLOCK_OAMP,
    JER_PP_EM_SER_NOF_BLOCKS
}JER_PP_EM_SER_BLOCK;



int dcmn_pp_em_ser(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg);
#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif

#endif 



