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
#ifndef BCM_LTSW_BCM56780_A0_INA_2_4_13_FIELD_LT_MAP_H
#define BCM_LTSW_BCM56780_A0_INA_2_4_13_FIELD_LT_MAP_H

#include <bcm_int/ltsw/field.h>

extern int bcm56780_a0_ina_2_4_13_ltsw_field_auto_enum_to_lt_enum_value_get(
    int unit,
    bcm_field_qualify_t qual,
    uint32 qual_enum,
    bcm_qual_field_t *qual_info);
extern int bcm56780_a0_ina_2_4_13_ltsw_field_auto_lt_enum_value_to_enum_get(
    int unit,
    bcm_field_qualify_t qual,
    bcm_qual_field_t *qual_info,
    uint32 *qual_enum);
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_ifp_qual_data[222];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_efp_qual_data[83];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_vfp_qual_data[147];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_em_qual_data[0];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_ifp_presel_qual_data[51];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_efp_presel_qual_data[22];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_vfp_presel_qual_data[22];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_em_presel_qual_data[0];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_ifp_group_qual_data[0];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_efp_group_qual_data[0];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_vfp_group_qual_data[0];
extern bcm_field_qual_map_t bcm56780_a0_ina_2_4_13_em_group_qual_data[0];
extern const bcm_field_action_map_t bcm56780_a0_ina_2_4_13_ifp_action_data[126];
extern const bcm_field_action_map_t bcm56780_a0_ina_2_4_13_efp_action_data[44];
extern const bcm_field_action_map_t bcm56780_a0_ina_2_4_13_vfp_action_data[23];
extern const bcm_field_action_map_t bcm56780_a0_ina_2_4_13_em_action_data[17];

#endif /* BCM_LTSW_BCM56780_A0_INA_2_4_13_FIELD_LT_MAP_H */
