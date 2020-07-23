/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Error translation
 */

#ifndef _BCM_INT_DPP_ERROR_H_
#define _BCM_INT_DPP_ERROR_H_

#include <bcm/debug.h>
#include <bcm/error.h>
#include <soc/dpp/error.h>

#define BCM_SAND_IF_ERR_EXIT(_sand_ret) \
    BCMDNX_IF_ERR_EXIT(handle_sand_result(_sand_ret))

#define BCM_SAND_IF_ERR_EXIT_NO_UNIT(_sand_ret) \
    BCMDNX_IF_ERR_EXIT_NO_UNIT(handle_sand_result(_sand_ret))


#endif /* _BCM_INT_DPP_ERROR_H_ */

