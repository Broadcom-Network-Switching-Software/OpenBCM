/*! \file bcmfp_grp_selcode.h
 *
 * APIs to manage selcode related functionality.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_GRP_SELCODE_H
#define BCMFP_GRP_SELCODE_H

#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

extern int
bcmfp_group_selcode_assign(int unit,
                           bcmfp_group_t *fg,
                           bcmfp_stage_t *stage,
                           int qual_count,
                           uint8_t parts_count,
                           uint8_t clear_flag);

extern int
bcmfp_group_slice_mode_to_group_mode(int unit,
                                     bcmfp_group_slice_mode_t grp_slice_mode,
                                     bcmfp_group_mode_t *grp_mode);

#endif /* BCMFP_GRP_SELCODE_H */
