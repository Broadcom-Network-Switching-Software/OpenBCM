
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_ERR_RECOVERY_COMMON_MANAGER_H

#define _DNX_ERR_RECOVERY_COMMON_MANAGER_H

#include <soc/dnx/recovery/rollback_journal_utils.h>

#define DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED          SAL_BIT(0)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION       SAL_BIT(1)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED       SAL_BIT(2)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED      SAL_BIT(3)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_ALL                 (SAL_BIT(4) - 1)

#define DNX_ERR_RECOVERY_COMMON_EXIT_IF_DISABLED(_unit)\
    if (dnx_err_recovery_common_is_enabled(unit) == FALSE)\
    {                                                     \
        SHR_EXIT();                                       \
    }

shr_error_e dnx_err_recovery_common_init(
    int unit);

shr_error_e dnx_err_recovery_common_enable_disable_set(
    int unit,
    uint32 is_enable);

uint8 dnx_err_recovery_common_is_enabled(
    int unit);

#endif
