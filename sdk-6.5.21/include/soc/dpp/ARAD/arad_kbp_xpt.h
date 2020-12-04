/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_KBP_XPT_INCLUDED__

#define __ARAD_KBP_XPT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>



























#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/xpt_12k.h>
#include <soc/kbp/alg_kbp/include/xpt_op.h>

struct kbp_search_result;


#define NlmXpt kbp_xpt
#define NlmXptRqt kbp_xpt_rqt
#define NlmRequestId kbp_xpt_rqt_id
#define NlmXpt_operations kbp_xpt_operations

extern NlmXpt*
arad_kbp_xpt_init(
    int                     unit,
    uint32                   core,
    NlmCmAllocator          *alloc,        
    uint32                 max_rqt_count, 
    uint32                 max_rslt_count, 
    uint32                 chan_id   
    ) ;



extern void
arad_kbp_xpt_destroy(
    NlmXpt* self 
) ;


extern NlmXpt*
arad_Nlm_Xpt_Create(
    int                     unit,
    uint32                  core,
    NlmCmAllocator          *alloc,        
    uint32                 max_rqt_count, 
    uint32                 max_rslt_count, 
    uint32                 chan_id   
    ) ;



extern void
arad_Nlm_Xpt_destroy(
    NlmXpt* self 
) ;



extern kbp_status 
arad_op_search(
    void *hdl,
    uint32_t ltr,
    uint32_t ctx,
    const uint8_t *key,
    uint32_t key_len,
    struct kbp_search_result *result);



extern void *
arad_kbp_op_xpt_init(
    int unit,
    nlm_u32 core,
    struct kbp_allocator *alloc);


extern void
arad_kbp_op_xpt_destroy(
    void* self
);



#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif
