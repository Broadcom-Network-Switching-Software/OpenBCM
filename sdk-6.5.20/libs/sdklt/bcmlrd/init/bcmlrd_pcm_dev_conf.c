/*! \file bcmlrd_pcm_dev_conf.c
 *
 * Definition of PCM configuration data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>

/* Declare conf externs. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
    extern const bcmlrd_pcm_conf_compact_rep_t *_bd##_vu##_va##_lrd_pcm_conf_get(void);
#include <bcmlrd/chip/bcmlrd_variant.h>

/* Define conf list. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
    { .pcm_conf_get = &_bd##_vu##_va##_lrd_pcm_conf_get },
bcmlrd_dev_pcm_conf_t bcmlrd_default_dev_pcm_conf[] = {
    { 0 }, /* dummy entry for type 'none' */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 }, /* end-of-list */
};
