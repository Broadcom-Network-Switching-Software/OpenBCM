/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __QAX_PP_KAPS_XPT_INCLUDED__

#define __QAX_PP_KAPS_XPT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/xpt_kaps.h>





#define KAPS_RPB_CAM_BIST_CONTROL_OFFSET   0x2a
#define KAPS_RPB_CAM_BIST_STATUS_OFFSET    0x2b
#define KAPS_RPB_GLOBAL_CONFIG_OFFSET      0x21

#define KAPS_BB_GLOBAL_CONFIG_OFFSET       0x21
#define KAPS_BB_MEM_CONFIG1_OFFSET         0x23


























uint32 qax_pp_kaps_utilize_desc_dma(int unit, uint8 enable);

uint32 qax_pp_kaps_utilize_desc_dma_get(int unit, uint8 *enable);

kbp_status qax_pp_kaps_search(void *xpt,
                                     uint8_t *key,
                                     enum kaps_search_interface search_interface,
                                     struct kaps_search_result *kaps_result);

kbp_status qax_kaps_register_read(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);

kbp_status qax_kaps_register_write(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);

kbp_status qax_pp_kaps_command(void *xpt,
                               enum kaps_cmd cmd,
                               enum kaps_func func,
                               uint32_t blk_nr,
                               uint32_t row_nr,
                               uint32_t nbytes,
                               uint8_t *bytes);

kbp_status qax_pp_kaps_read_command(void *xpt,
                                    uint32 blk_id,
                                    uint32 cmd,
                                    uint32 func,
                                    uint32 offset,
                                    uint32 n_result_bytes,
                                    uint8 *result_bytes);

kbp_status qax_pp_kaps_write_command(void *xpt,
                                     uint8 blk_id,
                                     uint32 cmd,
                                     uint32 func,
                                     uint32 offset,
                                     uint32 nbytes,
                                     uint8 *bytes);

kbp_status jer_pp_xpt_extended_command(void *xpt,
                                   uint32 blk_id,
                                   uint32 cmd,
                                   uint32 func,
                                   uint32 offset);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

