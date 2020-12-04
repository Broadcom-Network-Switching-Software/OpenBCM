/*! \file bcm56990_b0_cth_ctr_eflex_obj_bit_offset.c
 *
 * Chip stub for BCMCTH enhanced flex counter.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif

#include <bcmcth/bcmcth_ctr_eflex_internal.h>

static const bcmcth_ctr_eflex_obj_bit_offset_t ing_eflex_obj_bus_bit_offset[] =
{
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 3},
    {4 , 5},
    {8 , 15},
    {6 , 15},
    {0 , 5},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
};

static const bcmcth_ctr_eflex_obj_bit_offset_t egr_eflex_obj_bus_bit_offset[] =
{
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 11},
    {12 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 7},
    {8 , 9},
    {10 , 13},
    {14 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 15},
    {0 , 2},
    {3 , 5},
    {0 , 15},
    {0 , 15},
};

const bcmcth_ctr_eflex_obj_bit_offset_t *
bcm56990_b0_ctr_egr_eflex_cmd_bus_lts_bit_offset_get(int index)
{
    return NULL;
}

const bcmcth_ctr_eflex_obj_bit_offset_t *
bcm56990_b0_ctr_egr_eflex_obj_bus_bit_offset_get(int index)
{
    const bcmcth_ctr_eflex_obj_bit_offset_t *obj_bit_offset = NULL;

    if (index < COUNTOF(egr_eflex_obj_bus_bit_offset)) {
        obj_bit_offset = &egr_eflex_obj_bus_bit_offset[index];
    }

    return obj_bit_offset;
}

const bcmcth_ctr_eflex_obj_bit_offset_t *
bcm56990_b0_ctr_ing_eflex_obj_bus_bit_offset_get(int index)
{
    const bcmcth_ctr_eflex_obj_bit_offset_t *obj_bit_offset = NULL;

    if (index < COUNTOF(ing_eflex_obj_bus_bit_offset)) {
        obj_bit_offset = &ing_eflex_obj_bus_bit_offset[index];
    }

    return obj_bit_offset;
}

const bcmcth_ctr_eflex_obj_bit_offset_t *
bcm56990_b0_flex_state_egr_obj_bus_bit_offset_get(int index)
{
    return NULL;
}

const bcmcth_ctr_eflex_obj_bit_offset_t *
bcm56990_b0_flex_state_post_lkup_obj_bus_bit_offset_get(int index)
{
    return NULL;
}
