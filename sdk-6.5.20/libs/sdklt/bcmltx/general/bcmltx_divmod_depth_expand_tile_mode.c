/*! \file bcmltx_divmod_depth_expand_tile_mode.c
 *
 * Stub tile mode multi-entry L2P map table depth expansion key transform.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/general/bcmltx_divmod_depth_expand_tile_mode.h>
#include <bcmptm/bcmptm_itx.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_divmod_depth_expand_tile_mode_transform(int unit,
                                               const bcmltd_fields_t *in,
                                               bcmltd_fields_t *out,
                                               const bcmltd_transform_arg_t *arg)
{
    /* Wrapper for ITX implementation */
    return bcmptm_itx_multi_index_select_transform(unit, in, out, arg);
}

int
bcmltx_divmod_depth_expand_tile_mode_rev_transform(int unit,
                                                   const bcmltd_fields_t *in,
                                                   bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    /* Wrapper for ITX implementation */
    return bcmptm_itx_multi_index_select_rev_transform(unit, in, out, arg);
}

int
bcmltx_divmod_depth_expand_tile_mode_prepare(int unit,
                                             uint32_t trans_id,
                                             const bcmltd_generic_arg_t *arg)
{
    /* Wrapper for ITX implementation */
    return bcmptm_itx_prepare(unit, trans_id, arg);
}

int
bcmltx_divmod_depth_expand_tile_mode_commit(int unit,
                                            uint32_t trans_id,
                                            const bcmltd_generic_arg_t *arg)
{
    /* Wrapper for ITX implementation */
    return bcmptm_itx_commit(unit, trans_id, arg);
}

int
bcmltx_divmod_depth_expand_tile_mode_abort(int unit,
                                           uint32_t trans_id,
                                           const bcmltd_generic_arg_t *arg)
{
    /* Wrapper for ITX implementation */
    return bcmptm_itx_abort(unit, trans_id, arg);
}
