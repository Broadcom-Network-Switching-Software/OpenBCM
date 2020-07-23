/*! \file bcmlrd_cleanup.c
 *
 * \brief Logical Table Resource Database cleanup
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_init.h>

int
bcmlrd_cleanup(void)
{
    return bcmlrd_unit_cleanup();
}

