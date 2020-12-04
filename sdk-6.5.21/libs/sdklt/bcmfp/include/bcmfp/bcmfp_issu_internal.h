/*! \file bcmfp_issu_internal.h
 *
 * BCMFP ISSU handling functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_ISSU_INTERNAL_H
#define BCMFP_ISSU_INTERNAL_H

#include <bcmfp/bcmfp_trans_internal.h>

/*!
 * \brief Resize the stage operation info HA block to the new size
 *  and also update the different segment offsets in stage
 *  operational information.
 *
 * \param [in] unit         Logical device id
 * \param [in] sstage_id    BCMFP stage ID.
 * \param [in] sub_id       HA block id.
 * \param [in] new_blk_size New block size.
 * \param [in] issu_offset  Offsets and sizes of different
 *                          segements in HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_info_update(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint16_t blk_id,
                             uint32_t new_blk_size,
                             bcmfp_ha_struct_offset_t *issu_offset);
/*!
 * Even some elements are removed from the existing structures,
 * ISSU will not change the offsets of segements. But offsets
 * generated in bcmfp_ha_stage_oper_size_get are using the new
 * structures. So there will be difference in offsets generated
 * in the bcmfp_ha_stage_oper_size_get and actuall segment
 * offsets. This function will rectify the offsets of sub
 * segments offsets saved in stage operational information.
 *
 * \param [in] unit         Logical device id
 * \param [in] sstage_id    BCMFP stage ID.
 * \param [in] sub_id       HA block id.
 * \param [in] issu_offset  Offsets and sizes of different
 *                          segements in HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_issu_offsets_rectify(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint16_t blk_id,
                           bcmfp_ha_struct_offset_t *issu_offset);
#endif /* BCMFP_ISSU_INTERNAL_H */
