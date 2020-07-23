/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is a diagnostics module related to rollback journal
 */

#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_er_threading.h>
#endif


typedef int dnx_rollback_journal_diagnostics_c_make_compilers_happy;

#undef _ERR_MSG_MODULE_NAME
