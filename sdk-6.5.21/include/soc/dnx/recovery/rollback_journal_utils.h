/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is a utility module common for all error recovery features
 */
#ifndef _DNX_ROLLBACK_JOURNAL_UTILS_H

#define _DNX_ROLLBACK_JOURNAL_UTILS_H

#ifdef BCM_DNX_SUPPORT

#define DNX_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH 100

#define DNX_ERR_RECOVERY_VALIDATION_BASIC

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC

#define DNX_ERR_RECOVERY_REGRESSION_TESTING

#endif

#if !defined(DNX_ERR_RECOVERY_REGRESSION_TESTING) && !defined(DNX_ERR_RECOVERY_VALIDATION)
#error "DNX_ERR_RECOVERY_REGRESSION_TESTING or DNX_ERR_RECOVERY_VALIDATION must be ENABLED"
#endif

#if !defined(DNX_ERR_RECOVERY_REGRESSION_TESTING) && defined(DNX_ERR_RECOVERY_VALIDATION)
#error "DNX_ERR_RECOVERY_VALIDATION cannot be enabled without DNX_ERR_RECOVERY_REGRESSION_TESTING"
#endif

typedef enum
{
    DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK = 0,
    DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
    DNX_ROLLBACK_JOURNAL_TYPE_NOF
} dnx_rollback_journal_type_e;

typedef enum dnx_rollback_journal_subtype_d
{
    DNX_ROLLBACK_JOURNAL_SUBTYPE_INVALID,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_GENERIC,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_ALL
} dnx_rollback_journal_subtype_e;
#endif

#endif
