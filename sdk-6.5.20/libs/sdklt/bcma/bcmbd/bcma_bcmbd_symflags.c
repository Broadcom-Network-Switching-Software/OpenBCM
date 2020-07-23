/*! \file bcma_bcmbd_symflags.c
 *
 * BCMBD command symbol flag utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_enum_map.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

const char *
bcma_bcmbd_symflag_type2name(uint32_t flag)
{
    return bcmdrd_enum_sym_flag_name(flag);
}

uint32_t
bcma_bcmbd_symflag_name2type(const char *name)
{
    return bcmdrd_enum_sym_flag_val(name);
}
