/*! \file bcmlrd_init.c
 *
 * \brief Logical Table Resource Database initialization
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
bcmlrd_init(void)
{
    return bcmlrd_unit_init();
}
