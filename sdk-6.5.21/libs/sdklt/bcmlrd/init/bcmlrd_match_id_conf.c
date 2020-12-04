/*! \file bcmlrd_match_id_conf.c
 *
 * Definition of Match ID conf data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_types.h>

/* Declare conf externs. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
extern int \
_bd##_vu##_va##_lrd_match_id_db_get(int unit, \
                                const bcmlrd_sid_t sid, \
                                const bcmlrd_fid_t fid, \
                                const bcmlrd_match_id_db_t **info); \
extern const bcmlrd_match_id_db_info_t \
_bd##_vu##_va##_lrd_match_id_db_info;
#include <bcmlrd/chip/bcmlrd_variant.h>

/* Define conf list. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
    &_bd##_vu##_va##_lrd_match_id_db_get,
bcmlrd_match_id_db_get_t bcmlrd_default_match_id_db_get_ptrs[] = {
    NULL, /* dummy entry for type 'none' */
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL, /* end-of-list */
};
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)\
    &_bd##_vu##_va##_lrd_match_id_db_info,
const bcmlrd_match_id_db_info_t *bcmlrd_default_match_id_data_info_all[] = {
    NULL, /* dummy entry for type 'none' */
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL, /* end-of-list */
};
