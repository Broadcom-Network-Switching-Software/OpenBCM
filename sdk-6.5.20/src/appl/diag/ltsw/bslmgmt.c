/*! \file bslmgmt.c
 *
 * Bsl management definition for LTSW.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/bslmgmt.h>
#include <appl/diag/ltsw/bslmgmt.h>


/*******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Private functions
 */


/*******************************************************************************
 * Public functions
 */

int
appl_ltsw_bslmgmt_redir_hook_set(bsl_out_hook_f out_hook)
{
    return bslmgmt_redir_hook_set(out_hook);
}

