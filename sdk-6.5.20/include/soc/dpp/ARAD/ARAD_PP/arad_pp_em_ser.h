/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ARAD_PP_EM_SER_INCLUDED__
#define __ARAD_PP_EM_SER_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>



typedef enum
{
    ARAD_PP_EM_SER_BLOCK_IHP,
    ARAD_PP_EM_SER_BLOCK_IHB,
    ARAD_PP_EM_SER_BLOCK_EPNI,
    ARAD_PP_EM_SER_BLOCK_OAMP,
    ARAD_PP_EM_SER_NOF_BLOCKS
}ARAD_PP_EM_SER_BLOCK;

typedef enum
{
    ARAD_PP_EM_SER_TYPE_LEM_KEYT,
    ARAD_PP_EM_SER_TYPE_LEM_PLDT,
    ARAD_PP_EM_SER_TYPE_ISA_KEYT,
    ARAD_PP_EM_SER_TYPE_ISA_PLDT,
    ARAD_PP_EM_SER_TYPE_ISB_KEYT,
    ARAD_PP_EM_SER_TYPE_ISB_PLDT,
    ARAD_PP_EM_SER_TYPE_ESEM_KEYT,
    ARAD_PP_EM_SER_TYPE_ESEM_PLDT,
    ARAD_PP_EM_SER_TYPE_OEMA_KEYT,
    ARAD_PP_EM_SER_TYPE_OEMA_PLDT,
    ARAD_PP_EM_SER_TYPE_OEMB_KEYT,
    ARAD_PP_EM_SER_TYPE_OEMB_PLDT,
    ARAD_PP_EM_SER_TYPE_RMAPEM_KEYT,
    ARAD_PP_EM_SER_TYPE_RMAPEM_PLDT,
    ARAD_PP_EM_SER_TYPE_LAST
}ARAD_PP_EM_SER_TYPE;



int arad_pp_em_ser(int unit, uint32* address, ARAD_PP_EM_SER_BLOCK block);
int arad_pp_em_ser_parity_error_info_get(int unit, ARAD_PP_EM_SER_BLOCK block, uint32 *address, uint32 *address_valid, uint32 *counter, uint32 *counter_overflow);
uint32 arad_pp_em_ser_type_get(int unit, uint32 address, ARAD_PP_EM_SER_BLOCK block, ARAD_PP_EM_SER_TYPE *em_ser_type, soc_mem_t *mem, uint32 *base_address);
uint32 arad_pp_em_ser_address_get(int unit, soc_mem_t mem, uint32 array_index, uint32 index, uint32 *address, uint8 *is_em_ser);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


