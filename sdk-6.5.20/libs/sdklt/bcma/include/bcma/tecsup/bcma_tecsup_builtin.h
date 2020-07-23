/*! \file bcma_tecsup_builtin.h
 *
 * Data types for built-in mode.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TECSUP_BUILTIN_H
#define BCMA_TECSUP_BUILTIN_H

#include <bcma/tecsup/bcma_tecsup_drv.h>

/*! Command object for built-in mode. */
typedef const char * bcma_tecsup_builtin_cmd_t;

/*! Command list for built-in mode. */
typedef struct bcma_tecsup_builtin_cmdlist_s {
    /*! Category name. */
    const char *name;

    /*! Command list. */
    bcma_tecsup_builtin_cmd_t *cmds;
} bcma_tecsup_builtin_cmdlist_t;

/*! Feature object for built-in mode. */
typedef struct bcma_tecsup_builtin_feature_s {
    /*! Feature name. */
    const char *name;

    /*! Category command list. */
    bcma_tecsup_builtin_cmdlist_t *cmd_list;
} bcma_tecsup_builtin_feature_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcma_tecsup_builtin_feature_t * _bd##_vu##_va##_bcma_tecsup_feature_list_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>

#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcma_tecsup_builtin_cmd_t * _bd##_vu##_va##_bcma_tecsup_category_list_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMA_TECSUP_BUILTIN_H */
