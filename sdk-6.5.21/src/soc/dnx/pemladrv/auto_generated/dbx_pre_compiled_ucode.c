/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <sal/appl/config.h>
#include <sal/core/libc.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/pemladrv/auto_generated/dbx_pre_compiled_ucode.h>


#include "src/soc/dnx/pemladrv/auto_generated/jericho2p_a0_jr2-comp-jr1-mode_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/jericho2p_a0_jer2pemla_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/qumran2a_b0_jer2pemla_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/qumran2a_a0_jer2pemla_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/jericho2c_a0_jr2-comp-jr1-mode_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/jericho2c_a0_jer2pemla_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/jericho2_b0_jr2-comp-jr1-mode_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/jericho2_b0_jer2pemla_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/jericho2_a0_jr2-comp-jr1-mode_standard_1_dbx_pre_compiled.h"
#include "src/soc/dnx/pemladrv/auto_generated/jericho2_a0_jer2pemla_standard_1_dbx_pre_compiled.h"





#define PRE_COMPILED_PEMLA_UCODE_LINE_MAX_NOF_BYTES 512

uint8
pre_compiled_pemla_db_string_exist(
    int unit,
    char *filepath)
{
    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };
    char _filepath[RHFILE_MAX_SIZE] = { 0 };

    if (dbx_file_get_db(unit, db_name, base_db_name) != _SHR_E_NONE)
    {
        return FALSE;
    }
    sal_snprintf(_filepath, RHFILE_MAX_SIZE - 1, "%s/%s", db_name, filepath);

    

    if(!sal_strncmp("jericho2p_a0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("jericho2p_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("qumran2a_b0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("qumran2a_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("jericho2c_a0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("jericho2c_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("jericho2_b0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("jericho2_b0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("jericho2_a0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    if(!sal_strncmp("jericho2_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return TRUE;
    }

    

    return FALSE;
}

char **
pre_compiled_pemla_db_ucode_get(
    int unit,
    const char *filepath)
{
    char db_name[RHNAME_MAX_SIZE] = { 0 };
    char base_db_name[RHNAME_MAX_SIZE] = { 0 };
    char _filepath[RHFILE_MAX_SIZE] = { 0 };

    if (dbx_file_get_db(unit, db_name, base_db_name) != _SHR_E_NONE)
    {
        return FALSE;
    }
    sal_snprintf(_filepath, RHFILE_MAX_SIZE - 1, "%s/%s", db_name, filepath);

    

    if(!sal_strncmp("jericho2p_a0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_j2p_a0_jr2_comp_jr1_mode_standard_1;
    }

    if(!sal_strncmp("jericho2p_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_j2p_a0_jer2pemla_standard_1;
    }

    if(!sal_strncmp("qumran2a_b0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_q2a_b0_jer2pemla_standard_1;
    }

    if(!sal_strncmp("qumran2a_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_q2a_a0_jer2pemla_standard_1;
    }

    if(!sal_strncmp("jericho2c_a0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_j2c_a0_jr2_comp_jr1_mode_standard_1;
    }

    if(!sal_strncmp("jericho2c_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_j2c_a0_jer2pemla_standard_1;
    }

    if(!sal_strncmp("jericho2_b0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_jr2_b0_jr2_comp_jr1_mode_standard_1;
    }

    if(!sal_strncmp("jericho2_b0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_jr2_b0_jer2pemla_standard_1;
    }

    if(!sal_strncmp("jericho2_a0/pemla/ucode/standard_1/jr2-comp-jr1-mode/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_jr2_a0_jr2_comp_jr1_mode_standard_1;
    }

    if(!sal_strncmp("jericho2_a0/pemla/ucode/standard_1/jer2pemla/u_code_db2pem.txt", _filepath, RHFILE_MAX_SIZE))
    {
        return (char **)pre_compiled_pemla_db_string_jr2_a0_jer2pemla_standard_1;
    }

    

    return NULL;
}


