/**
 * \file diag_sand_framework_internal.h
 *
 * Framework utilities, structures and definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_SAND_FRAMEWORK_INTERNAL_H_INCLUDED
#define DIAG_SAND_FRAMEWORK_INTERNAL_H_INCLUDED

#include <sal/types.h>
#include <shared/utilex/utilex_rhlist.h>
#ifdef INCLUDE_AUTOCOMPLETE
#include <sal/appl/editline/autocomplete.h>
#endif
#include <appl/diag/sand/diag_sand_framework.h>

typedef struct
{
    rhentry_t entry;
    char short_key[SH_SAND_MAX_SHORTCUT_SIZE];
} sh_sand_keyword_var_t;

shr_error_e sh_sand_option_valid_range_get(
    int unit,
    sh_sand_option_t * option_p,
    char *start_str,
    char *end_str);

/**
 * \brief Routine serves to initialize legacy shell command tree
 * \param [in] unit                 - unit id
 * \param [in] root_n               - name of root command that will handle legacy
 * \param [in] sh_sand_cmd_a        - array of commands, to which legacy commands will be attached
 * \param [in] sh_sand_legacy_cmd_a - array of legacy commands for device
 * \param [in] cmd_callback - command to be called by general shell, serving entry point to the the framework
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_legacy_init(
    int unit,
    char *root_n,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_legacy_cmd_t * sh_sand_legacy_cmd_a,
    cmd_func_t cmd_callback);

/**
 * \brief Routine serves to de-initialize legacy shell command tree
 * \param [in] unit                 - unit id
 * \param [in] root_n               - name of root command that will handle legacy
 * \param [in] sh_sand_cmd_a        - array of commands, to which legacy commands will be attached
 * \param [in] cmd_callback - command to be called by general shell, serving entry point to the the framework
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_legacy_deinit(
    int unit,
    char *root_n,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback);

/**
 * \brief This macro is for local usage only to make external macros SH_SAND_GET* more transparent
 */

#ifdef INCLUDE_AUTOCOMPLETE
 /**
 * \brief Routine serves to add autocomplete support
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] parent pointer to parent autocomplete node
 *     \param [in] cmd commands tree
 */
void sh_sand_cmd_autocomplete_init(
    int unit,
    autocomplete_node_t * parent,
    sh_sand_cmd_t * cmd);

void sh_sand_cmd_autocomplete_deinit(
    int unit,
    sh_sand_cmd_t * cmd);

#endif

/*
 * Manual page of sys_arguments is used only for manual document generation. It is internal to framework module
 */
extern sh_sand_man_t sh_sand_sys_arguments_man;
extern sh_sand_option_t sh_sand_sys_arguments[];

/**
 * \brief Initialize keyword list, create shortcut and plural versions
 * \param [in] unit - unit id
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - Bad keyword in the list
 *   \retval Other            - Other errors as per shr_error_e
 *   Flow goes to exit in any case of failure
 */
shr_error_e sh_sand_keyword_list_init(
    int unit);

/**
 * \brief DeInitialize keyword list, freeing all dynamically allocated resources
 * \param [in] unit - unit id
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - Bad keyword in the list
 *   \retval Other            - Other errors as per shr_error_e
 */
shr_error_e sh_sand_keyword_list_deinit(
    int unit);

/**
 * \brief       Generated shortcut string from the keyword
 * \param [in] keyword - pointer to full string as provided by user
 * \param [out] short_key - pointer, where short_key string should be set
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other            - Other errors as per shr_error_e
 * \remark
 */
shr_error_e sh_sand_keyword_shortcut_get(
    char *keyword,
    char *short_key);

/**
 * \brief Verify existence of keyword and return shortcut and form for printing
 * \param [in] unit - unit id
 * \param [in] command - pointer to command, this keyword belong to
 * \param [in] keyword_in - pointer to string to be verified on keyword list
 * \param [out] shortcut_out_p - pointer, where pointer to shortcut is to be placed
 * \param [out] keyword_out_p - pointer, where string to be used for print will be assigned
 * \param [in] flags - misc flags, usually verification options
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_NOT_FOUND - Keyword was not found in the list
 *   \retval Other            - Other errors as per shr_error_e
 *   Flow goes to exit in any case of failure
 * \remark
 *   This routine is a primary one to be used for communication between keyword list and its users
 *   Default are assigned to shortcut and keyword when verification is not required
 */
shr_error_e sh_sand_keyword_fetch(
    int unit,
    char *command,
    char *keyword_in,
    char **shortcut_out_p,
    char **keyword_out_p,
    int flags);

#ifndef NO_FILEIO
/**
 * \brief Compare file to the gold result stored in DB - tools/sand/db per device
 * \param [in] unit - unit id
 * \param [in] local_filename - local path/name of the file to be compared
 * \param [in] gold_name - name of gold file to be compared with
 * \param [in] filter_n - set of tokens that will cause comparison to skip the line
 * \param [in] flags - different flags managing command flow and output
 * \return
 *   \retval _SHR_E_NONE  - On success
 *   \retval _SHR_E_PARAM - If two files are not identical
 *   \retval Other            - Other errors as per shr_error_e
 * \remark
 */
shr_error_e sh_sand_gold_compare(
    int unit,
    char *local_filename,
    char *gold_name,
    char *filter_n,
    int flags);

/**
 * \brief
 * List all defined tests for node or leaf
 * \param [in] unit         - device id - important because path will depend on it
 * \param [in] flags        - Flags
 * \param [in] gold_name    - gold file name or output XML file name
 * \param [out] gold_path   - full absolute path for actual gold file for specific device
 *
 * \remark
 *     Routine does not return the failure - if something went wrong test will fail
 */
shr_error_e sh_sand_gold_path(
    int unit,
    int flags,
    char *gold_name,
    char *gold_path);

/**
 * \brief
 * List all defined tests for node or leaf
 * \param [in] command     - command which output XML file name we need
 * \param [out] filename    - pointer to allocated space where resulting test name will be formed
 *
 * \remark
 *     Routine does not return the failure - if something went wrong test will fail
 */
void sh_sand_gold_filename(
    char *command,
    char *filename);

#endif

/**
 * \brief Iterate over command array looking for valid commands, commands that are not supported for this device will
 *        be ignored, if they are not support in specific SW/HW configuration, they are not ignore and it allowed to
 *        calling routine to handle such cases on its own
 * \param [in] sh_sand_cmd_curr  - pointer to command that holds current
 * \param [in] sh_sand_cmd_array - pointer to command array that is being iterated
 * \remark Use only if you need to perform certain actions for command not support in current configuration,
 *  like usage, Manual, special warnings
 */
#define SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd_curr, sh_sand_cmd_array)                                                  \
        for (sh_sand_cmd_curr = sh_sand_cmd_array;                                                                     \
                        sh_sand_cmd_curr != NULL && sh_sand_cmd_curr->keyword != NULL; sh_sand_cmd_curr++)             \
            if(!(sh_sand_cmd_curr->flags & SH_CMD_CONDITIONAL) ||                                                      \
                (sh_sand_cmd_curr->invoke_cb  == NULL) ||                                                              \
                (sh_sand_cmd_curr->invoke_cb(unit, NULL) != _SHR_E_UNIT))

#endif /* DIAG_SAND_FRAMEWORK_H_INCLUDED */
