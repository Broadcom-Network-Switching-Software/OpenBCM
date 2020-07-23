/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JERP_PP_KAPS_XPT_INCLUDED__

#define __JERP_PP_KAPS_XPT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/xpt_kaps.h>






























kbp_status jerp_kaps_xpt_hb_read(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes);

kbp_status jerp_kaps_xpt_hb_write(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes);

kbp_status jerp_kaps_xpt_hb_dump(void *xpt, uint32_t start_blk_num, uint32_t start_row_num, uint32_t end_blk_num,
                                uint32_t end_row_num, uint8_t clear_on_read, uint8_t *data);

kbp_status jerp_kaps_xpt_hb_copy(void *xpt, uint32_t src_blk_num, uint32_t src_row_num, uint32_t dest_blk_num,
                                uint32_t dest_row_num, uint16_t source_mask, uint8_t rotate_right, uint8_t perform_clear);

kbp_status jerp_kaps_xpt_translate_blk_func_offset_to_mem_reg(int unit,
                                                          uint8     blk_id,
                                                          uint32     func,
                                                          uint32     offset,
                                                          soc_mem_t *mem,
                                                          soc_reg_t *reg,
                                                          uint32    *array_index,
                                                          int       *blk, 
                                                          int       *instance); 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

