
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_ERR_RECOVERY_COMMON_MANAGER_H

#define _DNX_ERR_RECOVERY_COMMON_MANAGER_H

#include <soc/dnx/recovery/rollback_journal_utils.h>

#define DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED          SAL_BIT(0)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_IN_TRANSACTION       SAL_BIT(1)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_SUPPRESSED       SAL_BIT(2)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED      SAL_BIT(3)
#define DNX_ERR_RECOVERY_COMMON_IS_ON_ALL                 (SAL_BIT(4) - 1)

typedef struct dnx_err_recovery_common_manager_d
{

    uint32 bitmap;
} dnx_err_recovery_common_manager_t;

typedef struct dnx_err_recovery_common_root_d
{

    uint32 is_enable;

    uint32 dbal_access_counter;

    dnx_err_recovery_common_manager_t managers[DNX_ROLLBACK_JOURNAL_TYPE_NOF];

} dnx_err_recovery_common_root_t;

shr_error_e dnx_err_recovery_common_init(
    int unit);

shr_error_e dnx_err_recovery_common_enable_disable(
    int unit,
    uint32 is_enable);

uint8 dnx_err_recovery_common_is_enabled(
    int unit);

uint8 dnx_err_recovery_common_is_on(
    int unit,
    dnx_rollback_journal_type_e mngr_id);

uint8 dnx_err_recovery_common_flag_is_on(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

uint8 dnx_err_recovery_common_flag_mask_is_on(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 mask);

shr_error_e dnx_err_recovery_common_flag_set(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

shr_error_e dnx_err_recovery_common_flag_clear(
    int unit,
    dnx_rollback_journal_type_e mngr_id,
    uint32 flag);

uint8 dnx_err_recovery_common_is_dbal_access_region(
    int unit);

shr_error_e dnx_err_recovery_common_dbal_access_region_start(
    int unit);

shr_error_e dnx_err_recovery_common_dbal_access_region_end(
    int unit);

#endif
