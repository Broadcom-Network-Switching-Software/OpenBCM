
/*! \file bcmfp_ctr.c
 *
 * APIs to get the counter hardware index bitmap and its mapping with counter
 * ID.
 *
 * This file contains functions to get the hardware index bitmap and mappings of
 * counter ID to hardware index mappping.
 *
 * This file also contains functions to process
 * flex counter parameters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_cth_common.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_ctr_hw_index_get(int unit,
                       bcmfp_stage_id_t stage_id,
                       uint32_t ctr_id,
                       uint32_t *hw_index)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    void *ha_mem = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint16_t *ctr_egr_id_to_hw_idx_map  = NULL;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
        ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_id_to_hw_index_map_seg;
    ctr_egr_id_to_hw_idx_map =  (uint16_t *)((uint8_t *)ha_mem + offset);
    *hw_index = ctr_egr_id_to_hw_idx_map[ctr_id - 1];

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_hw_idx_bmp_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint32_t **ctr_egr_hw_idx_bmp)
{
    uint32_t offset = 0;
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_hw_index_bitmap_seg;
    *ctr_egr_hw_idx_bmp =  (uint32_t *)((uint8_t *)ha_mem + offset);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_hw_idx_map_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint16_t **ctr_egr_id_to_hw_idx_map)
{
    uint32_t offset = 0;
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->ctr_id_to_hw_index_map_seg;
    *ctr_egr_id_to_hw_idx_map =  (uint16_t *)((uint8_t *)ha_mem + offset);

exit:
    SHR_FUNC_EXIT();
}
