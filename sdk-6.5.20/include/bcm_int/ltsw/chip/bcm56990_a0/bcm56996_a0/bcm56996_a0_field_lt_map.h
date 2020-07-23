/*******************************************************************************
 *
 * \file field_lt_map.h
 *
 * This file is auto-generated using HSDKGen.py.
 *
 * Edits to this file will be lost when it is regenerated.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCM_LTSW_BCM56990_A0_BCM56996_A0_FIELD_LT_MAP_H
#define BCM_LTSW_BCM56990_A0_BCM56996_A0_FIELD_LT_MAP_H

#include <bcm_int/ltsw/field.h>

extern int bcm56990_a0_bcm56996_a0_ltsw_field_auto_enum_to_lt_enum_value_get(
    int unit,
    bcm_field_qualify_t qual,
    uint32 qual_enum,
    bcm_qual_field_t *qual_info);
extern int bcm56990_a0_bcm56996_a0_ltsw_field_auto_lt_enum_value_to_enum_get(
    int unit,
    bcm_field_qualify_t qual,
    bcm_qual_field_t *qual_info,
    uint32 *qual_enum);
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_ifp_qual_data[147];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_efp_qual_data[67];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_vfp_qual_data[56];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_em_qual_data[0];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_ifp_presel_qual_data[16];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_efp_presel_qual_data[0];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_vfp_presel_qual_data[0];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_em_presel_qual_data[0];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_ifp_group_qual_data[145];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_efp_group_qual_data[67];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_vfp_group_qual_data[56];
extern bcm_field_qual_map_t bcm56990_a0_bcm56996_a0_em_group_qual_data[0];
extern const bcm_field_action_map_t bcm56990_a0_bcm56996_a0_ifp_action_data[170];
extern const bcm_field_action_map_t bcm56990_a0_bcm56996_a0_efp_action_data[15];
extern const bcm_field_action_map_t bcm56990_a0_bcm56996_a0_vfp_action_data[31];
extern const bcm_field_action_map_t bcm56990_a0_bcm56996_a0_em_action_data[0];

#endif /* BCM_LTSW_BCM56990_A0_BCM56996_A0_FIELD_LT_MAP_H */
