/*! \file bcmltx_subfield_rxfrm_table_commit.c
 *
 * Prepare/Commit/Abort interfaces for the FLTG generated multiple
 * memory reverse subfield transform.
 *
 * The reverse subfield transform consolidates data for a logical
 * table field that has portions of the value stored in different
 * physical memories. Because transforms are specified on a per
 * physical memory basis, such consolidation requires state to know
 * when to return the consolidated result.
 *
 * The 'prepare' interface is used to clear the consolidation state.
 * Commit and abort also clear state, but are considered of secondary
 * importance.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmltx/bcmltx_subfield_rxfrm_table_commit.h>
#include <bcmltx/bcmltx_field_demux_acc.h>

/*******************************************************************************
 * Public functions
 */
int
bcmltx_subfield_rxfrm_table_prepare(int unit,
                                    uint32_t trans_id,
                                    const bcmltd_generic_arg_t *arg)
{
    return bcmltx_field_mux_acc_transform_clear_state(arg);
}

int
bcmltx_subfield_rxfrm_table_commit(int unit,
                                   uint32_t trans_id,
                                   const bcmltd_generic_arg_t *arg)
{
    return bcmltx_field_mux_acc_transform_clear_state(arg);
}

int
bcmltx_subfield_rxfrm_table_abort(int unit,
                                  uint32_t trans_id,
                                  const bcmltd_generic_arg_t *arg)
{
    return bcmltx_field_mux_acc_transform_clear_state(arg);
}
