/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_PP_KAPS_XPT_INCLUDED__

#define __JER_PP_KAPS_XPT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/kbp/alg_kbp/include/xpt_kaps.h>







#define KAPS_REVISION_REGISTER_ADDR (0x0)

#define KAPS_JERICHO_REVISION_REG_VALUE (0x00010000)

#define KAPS_QUMRAN_AX_REVISION_REG_VALUE (0x00020000)

#define KAPS_JERICHO_PLUS_FM0_REVISION_REG_VALUE (0x00030000)

#define KAPS_JERICHO_PLUS_FM4_REVISION_REG_VALUE (0x00030001)

#define KAPS_JERICHO_PLUS_JER_MODE_FM0_REVISION_REG_VALUE (0x00030002)

#define KAPS_JERICHO_PLUS_JER_MODE_FM4_REVISION_REG_VALUE (0x00030003)

#define KAPS_QUX_REVISION_REG_VALUE (0x00040000)
















struct jericho_data {
    int unit;
};

typedef struct kaps_jericho_xpt {
    struct kaps_xpt jer_kaps_xpt;
    struct jericho_data jer_data; 
} JER_KAPS_XPT;






    







uint32 jer_pp_xpt_init(int unit, void **xpt);

uint32 jer_pp_xpt_deinit(int unit, void *xpt);

kbp_status jer_pp_kaps_read_command(void *xpt,  
                                    uint32 blk_id, 
                                    uint32 cmd,
                                    uint32 func,
                                    uint32 offset,
                                    uint32 n_result_bytes, 
                                    uint8 *result_bytes);

kbp_status jer_pp_kaps_write_command(void *xpt,  
                                     uint8 blk_id, 
                                     uint32 cmd,
                                     uint32 func,
                                     uint32 offset,
                                     uint32 nbytes, 
                                     uint8 *bytes);

uint32 jer_pp_xpt_dma_state(int unit, uint32 print_status, uint32 enable_dma_thread, uint32 wait_arm);


uint32 jer_pp_xpt_arm_init(int unit);
uint32 jer_pp_xpt_arm_deinit(int unit);
uint32 jer_pp_xpt_arm_start_halt(int unit, int halt);
uint32 jer_pp_xpt_arm_load_file_entry(int unit, uint32 *input_32, int entry_num);
uint32 jer_pp_xpt_wait_dma_done(int unit);
uint32 jer_pp_xpt_kaps_arm_image_load_default(int unit);

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

