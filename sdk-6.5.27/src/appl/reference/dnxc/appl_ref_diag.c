/** \file appl_ref_diag.c
 * 
 *
 * Diag application procedures for DNX.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * Include files.
 * {
 */

#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/dbx/dbx_file.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/reference/dnxc/appl_ref_diag.h>

/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
appl_dnxc_diag_get(
    int unit,
    uint32 flags,
    char *diag,
    uint32 result_buffer_size,
    char *result_buffer)
{
    char *local_filename = "TMP_FLE.xml";
    char full_command[RHSTRING_MAX_SIZE];
    char current_line[SH_SAND_MAX_TOKEN_SIZE];
    FILE *local_result = NULL;
    int num_chars_copied = 0, num_chars_to_copy;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(result_buffer, _SHR_E_PARAM, "result_buffer");

    /*
     * Add temporary local file to command to caputure output.
     */
    sal_snprintf(full_command, RHSTRING_MAX_SIZE - 1, "%s file=\"%s\"", diag, local_filename);
    /*
     * Execute provided diag command.
     */
    SHR_IF_ERR_EXIT(sh_sand_execute(unit, full_command));

    /*
     * Open the temporary local file containing the output of the requested diag command.
     */
    if ((local_result = sal_fopen(local_filename, "r")) == NULL)
    {
        /*
         * Could not open the file, usually means that path was illegal.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Local file to be loaded into buffer:'%s' cannot be opened\n", local_filename);
    }

    /*
     * Iterate through the output file line by line, copying the contents to the result_buffer.
     */
    while (NULL != sal_fgets(current_line, SH_SAND_MAX_TOKEN_SIZE - 1, local_result))
    {
        num_chars_to_copy = sal_strnlen(current_line, SH_SAND_MAX_TOKEN_SIZE);
        /*
         * If adding entirety of current_line to result_buffer will result in overflow, copy as many chars as possible without overflowing buffer.
         */
        if (num_chars_copied + num_chars_to_copy >= result_buffer_size)
        {
            sal_strncpy(result_buffer + num_chars_copied, current_line, result_buffer_size - num_chars_copied);
            num_chars_copied = result_buffer_size;
            break;
        }

        sal_strncpy(result_buffer + num_chars_copied, current_line, num_chars_to_copy);
        num_chars_copied += num_chars_to_copy;
    }

    SHR_EXIT();
exit:
    /*
     * Close the temporary local file containing the output of the requested diag command.
     */
    if ((local_result != NULL) && (sal_fclose(local_result) != 0))
    {
        LOG_CLI((BSL_META("ERROR: Unable to close the %s file.\n"), local_filename));
    }

    /*
     * Delete the temporary local file used to capture the output of the provided diag command.
     */
    if (dbx_file_exists(local_filename) && (dbx_file_remove(local_filename) != _SHR_E_NONE))
    {
        LOG_CLI((BSL_META("ERROR: Unable to delete temporary file: '%s'\n"), local_filename));
    }

    SHR_FUNC_EXIT;
}
