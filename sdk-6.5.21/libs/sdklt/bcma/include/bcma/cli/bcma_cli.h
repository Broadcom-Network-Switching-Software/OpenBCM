/*! \file bcma_cli.h
 *
 * CLI core functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_H
#define BCMA_CLI_H

#include <sal/sal_types.h>

#include <bcma/cli/bcma_cli_config.h>
#include <bcma/cli/bcma_cli_arg.h>

struct bcma_cli_s;

/*!
 * \brief Call-back to read from command input device.
 *
 * When the CLI is started, it must be provided a function for reading
 * user input. The function should display the supplied \c prompt and
 * return the null-terminated user input in \c str, which is a buffer
 * of size \c max.
 *
 * Multiple CLI instances might be created but will still share the same
 * input device. The \c cli is used to differentiate the CLI instance
 * which is currently running on the input device.
 *
 * \param [in] cli CLI object.
 * \param [in] prompt Command prompt to display.
 * \param [in] max Size of raw command line buffer.
 * \param [out] str Raw command line.
 *
 * \retval BCMA_CLI_CMD_OK No errors.
 * \retval BCMA_CLI_CMD_INTR Interrupt indicator is received from input device.
 * \retval BCMA_CLI_CMD_EXIT End of transmission is received from input device.
 * \retval BCMA_CLI_CMD_BAD_ARG User input exceeds command line buffer size.
 */
typedef int (*bcma_cli_gets_f)(struct bcma_cli_s *cli,
                               const char *prompt, int max, char *str);

/*!
 * \brief Call-back for command history support.
 *
 * This optional call-back function will be invoked for each command line
 * entered into the CLI. The application may use this call-back to add
 * the input command to a command history.
 *
 * If the command input function supports some form of history expansion,
 * the command string may be modified accordingly, and the CLI will process
 * the modified string instead of the original one.
 *
 * Note that this function is only invoked for command input, as this prevents
 * interactive CLI command input from being added to the command history.
 *
 * \param [in] max Size of raw command line buffer.
 * \param [in,out] str Raw command line.
 */
typedef void (*bcma_cli_history_add_f)(int max, char *str);

/*!
 * \brief Handler for unrecognized CLI commands.
 *
 * If a CLI command is not recognized, the command can optionally
 * be passed to an application-provided exception handler for special
 * processing.
 *
 * The return value should be BCMA_CLI_CMD_NOT_FOUND if the command
 * is not processed in the exception handler.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
typedef int (*bcma_cli_cmd_except_f)(struct bcma_cli_s *cli,
                                     struct bcma_cli_args_s *args);

/*!
 * \brief Handler for customizing the prompt prefix.
 *
 * \param [in] cli CLI object.
 *
 * \return The prefix string if any.
 */
typedef const char* (*bcma_cli_prompt_prefix_f)(struct bcma_cli_s *cli);

/*!
 * \brief Call-back for reading environment variable.
 *
 * This function must return the value of the specified environment
 * variable or NULL if the variable does not exist.
 *
 * \param [in] cookie Pass-through environment data.
 * \param [in] name Environment variable to retrieve.
 *
 * \return Value of environment variable or NULL if not found.
 */
typedef const char *(*bcma_cli_var_get_f)(void *cookie, const char *name);

/*!
 * \brief Call-back for setting an environment variable.
 *
 * This function will set the value of the specified environment variable.
 * If \c value is NULL, environment variable \c name will be cleared.
 * If the environment variable does not already exist, it will be created.
 *
 * \param [in] cookie Pass-through environment data.
 * \param [in] name Environment variable to set.
 * \param [in] value Value to set to environment variable.
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
typedef int (*bcma_cli_var_set_f)(void *cookie, const char *name,
                                  const char *value, int local);

/*!
 * \brief Call-back to push scope of local environment variables.
 *
 * The environment variable scope is pushed whenever the command input
 * source changes. This typically happens when a new command script is
 * loaded. The new scope inherits the global variables, but not the
 * local ones.
 *
 * \param [in] cookie Pass-through environment data.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
typedef int (*bcma_cli_var_scope_push_f)(void *cookie);

/*!
 * \brief Call-back to pop scope of local environment variables.
 *
 * The environment variable scope is popped whenever the current
 * command input source is exhausted. This typically happens when a
 * command script is complete.
 *
 * \param [in] cookie Pass-through environment data.
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
typedef int (*bcma_cli_var_scope_pop_f)(void *cookie);

/*!
 * \brief CLI environment variable call-back vectors.
 *
 * This structure can be used to add environment variable support to
 * the basic CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_var_cb_set().
 */
typedef struct bcma_cli_var_cb_s {

    /*! Function for shell variable get. */
    bcma_cli_var_get_f var_get;

    /*! Function for shell variable set. */
    bcma_cli_var_set_f var_set;

    /*! Function to push local environment variable scope. */
    bcma_cli_var_scope_push_f var_scope_push;

    /*! Function to pop local environment variable scope. */
    bcma_cli_var_scope_pop_f var_scope_pop;

} bcma_cli_var_cb_t;

/*!
 * \brief Call-back for reading tagged environment variable.
 *
 * This function must return the value of the specified environment
 * variable or NULL if the variable does not exist.
 *
 * \param [in] cookie Pass-through environment data.
 * \param [in] name Environment variable to retrieve.
 * \param [out] tag Tag value of the environment variable if not NULL.
 *
 * \return Value of environment variable or NULL if not found.
 */
typedef const char *(*bcma_cli_tvar_get_f)(void *cookie, const char *name,
                                           uint32_t *tag);

/*!
 * \brief Call-back for setting an environment variable with tag.
 *
 * This function will set the value of the specified environment variable
 * and set the tag associate with this environment variable as well.
 * If \c value is NULL, environment variable \c name will be cleared.
 * If the environment variable does not already exist, it will be created.
 *
 * \param [in] cookie Pass-through environment data.
 * \param [in] name Environment variable to set.
 * \param [in] value Value to set to environment variable.
 * \param [in] tag Tag value for the specified environment variable.
 * \param [in] local Set variable in local scope (TRUE/FALSE).
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong.
 */
typedef int (*bcma_cli_tvar_set_f)(void *cookie, const char *name,
                                   const char *value, uint32_t tag, int local);

/*!
 * \brief Call-back for unsetting environment variables with the same tag.
 *
 * This function deletes the environment variables with the same tag to the
 * specified tag value \c tag.
 *
 * \param [in] cookie Pass-through environment data.
 * \param [in] tag Tag to be matched for unsetting environment variables.
 * \param [in] local Unset variable in local scope (TRUE/FALSE).
 *
 * \retval 0 No errors.
 * \retval -1 Something went wrong or tag is not supported.
 */
typedef int (*bcma_cli_tvar_unset_by_tag_f)(void *cookie, uint32_t tag,
                                            int local);

/*!
 * \brief CLI tagged environment variable call-back vectors.
 *
 * This structure can be used to add tagged environment variable support to
 * the basic CLI.
 *
 * Should be added to an initialized CLI object using function
 * bcma_cli_tvar_cb_set() in bcma_cli_tvar.h.
 */
typedef struct bcma_cli_tvar_cb_s {

    /*! Function for tagged shell variable get. */
    bcma_cli_tvar_get_f tvar_get;

    /*! Function for tagged shell variable set. */
    bcma_cli_tvar_set_f tvar_set;

    /*! Function for deleting tagged shell variables with the same tag. */
    bcma_cli_tvar_unset_by_tag_f tvar_unset_by_tag;

} bcma_cli_tvar_cb_t;

/*!
 * \brief Call-back to enable/disable Ctrl-C traps.
 *
 * When enabled, a Ctrl-C trap handler is installed, and when
 * disabled, the original Ctrl-C behavior is restored.
 *
 * \param [in] cookie Pass-through Ctrl-C data.
 * \param [in] enable Enable (1) or disable (0) Ctrl-C traps.
 *
 * \retval 0 No errors.
 */
typedef int (*bcma_cli_ctrlc_enable_set_f)(void *cookie, int enable);

/*!
 * \brief Call-back to execute a function with Ctrl-C hook
 *
 * This function is used to allow a Ctrl-C keypress to interrupt an
 * active CLI command, e.g. if it was invoked in error or the user
 * does not want to wait for normal command completion.
 *
 * If not NULL, function \c ctrlc_sig_cb will be called when Ctrl-C is
 * pressed.
 *
 * Returning 0 from \c ctrlc_sig_cb will prevent the Ctrl-C handler
 * from aborting function \c func immediately, thus eliminating the
 * possibility of unreleased resources. If \c ctrlc_sig_cb is NULL or
 * returning -1, the active CLI command will be aborted immediately and
 * the allocated resources might be lost.
 *
 * Typically the \c ctrlc_sig_cb function will request a controlled
 * abort of \c func (e.g. by setting a flag) to ensure that all
 * resources are properly released.
 *
 * \param [in] cookie Pass-through Ctrl-C data.
 * \param [in] func Function to call.
 * \param [in] data Function context.
 * \param [in] ctrlc_sig_cb Callback function when Ctrl-C is pressed.
 * \param [in] cb_data User data for callback function \c ctrlc_sig_cb.
 * \param [in] rv_intr Return value when interrupted by Ctrl-C.
 *
 * \return The function \c func returned value if not interrupted by Ctrl-C.
 *         Otherwise \c rv_intr.
 */
typedef int (*bcma_cli_ctrlc_exec_f)(void *cookie,
                                     int (*func)(void *), void *data,
                                     int (*ctrlc_sig_cb)(void *), void *cb_data,
                                     int rv_intr);

/*!
 * \brief CLI environment variable call-back vectors.
 *
 * This structure can be used to add Ctrl-C trap support to the basic
 * CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_ctrlc_cb_set().
 */
typedef struct bcma_cli_ctrlc_cb_s {

    /*! Function for setting Ctrl-C response mode. */
    bcma_cli_ctrlc_enable_set_f ctrlc_enable_set;

    /*! Function for executing a function with Ctrl-C trap. */
    bcma_cli_ctrlc_exec_f ctrlc_exec;

} bcma_cli_ctrlc_cb_t;

/*!
 * \brief Call-back to get the number of maximum devices.
 *
 * This function is used to support the ability to issue a command to
 * all devices through the CLI.
 *
 * \param [in] cookie Pass-through unit data.
 *
 * \retval The number of maximum devices.
 */
typedef int (*bcma_cli_unit_max_f)(void *cookie);

/*!
 * \brief Call-back to check if a device is present.
 *
 * \param [in] cookie Pass-through unit data.
 * \param [in] unit Unit number.
 *
 * \retval TRUE Unit is valid.
 * \retval FALSE Unit is not valid.
 */
typedef int (*bcma_cli_unit_valid_f)(void *cookie, int unit);

/*!
 * \brief Call-back to check if a device feature is present.
 *
 * This function is used to support different sets of CLI commands on
 * different devices or to have different implementations of the same
 * command on different devices.
 *
 * \param [in] cookie Pass-through unit data.
 * \param [in] unit Unit number.
 * \param [in] feature Feature to check for.
 *
 * \retval TRUE Feature is enabled for this unit.
 * \retval FALSE Feature is not enabled for this unit.
 */
typedef int (*bcma_cli_unit_feature_enabled_f)(void *cookie,
                                               int unit, int feature);

/*!
 * \brief CLI environment unit call-back vectors.
 *
 * This structure can be used for specialized unit (device) management
 * in the CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_unit_cb_set().
 */
typedef struct bcma_cli_unit_cb_s {

    /*! Function for retrieving the number of maximum devices. */
    bcma_cli_unit_max_f unit_max;

    /*! Function for checking if a device is present. */
    bcma_cli_unit_valid_f unit_valid;

    /*! Function for checking if a device feature is present. */
    bcma_cli_unit_feature_enabled_f unit_feature_enabled;

} bcma_cli_unit_cb_t;

/*!
 * \brief Call-back to control CLI I/O redirection.
 *
 * If a CLI command is followed by '|', the I/O redirection will be
 * enabled, otherwise it will be disabled.
 *
 * \param [in] cookie Pass-through redirection data.
 * \param [in] enable Enable (1) or disable (0) CLI I/O redirection.
 *
 * \retval 0 No errors.
 */
typedef int (*bcma_redir_enable_set_f)(void *cookie, int enable);

/*!
 * \brief Call-back for indicating the completion of a CLI command.
 *
 * \param [in] cookie Pass-through redirection data.
 *
 * \retval 0 No errors.
 */
typedef int (*bcma_redir_cmd_done_f)(void *cookie);

/*!
 * \brief Call-back to get input source of CLI I/O redirection.
 *
 * The input source is normally a file or a named pipe.
 *
 * \param [in] cookie Pass-through redirection data.
 *
 * \return Pointer to name of input source.
 */
typedef const char *(*bcma_redir_input_name_get_f)(void *cookie);

/*!
 * \brief Call-back to get output source of CLI I/O redirection.
 *
 * The output source is normally a file or a named pipe.
 *
 * \param [in] cookie Pass-through redirection data.
 *
 * \return Pointer to name of output source.
 */
typedef const char *(*bcma_redir_output_name_get_f)(void *cookie);

/*!
 * \brief CLI I/O redirection call-back vectors.
 *
 * This structure can be used to add I/O redirection support to the
 * CLI.
 *
 * Should be added to an initialized CLI object using \ref
 * bcma_cli_redir_cb_set().
 */
typedef struct bcma_cli_redir_cb_s {

    /*! Function for setting CLI I/O redirection. */
    bcma_redir_enable_set_f redir_enable_set;

    /*! Function for indicating completion of a CLI command. */
    bcma_redir_cmd_done_f redir_cmd_done;

    /*! Function for getting CLI input source. */
    bcma_redir_input_name_get_f redir_input_name_get;

    /*! Function for getting CLI output source. */
    bcma_redir_output_name_get_f redir_output_name_get;

} bcma_cli_redir_cb_t;

struct bcma_cli_command_s;

/*!
 * \brief CLI command list object.
 *
 * All installed CLI commands are stored in a list of this type.
 *
 * Each CLI object maintains its own list of commands.
 */
typedef struct bcma_cli_cmd_list_s {

    /*! Next CLI command in list. */
    struct bcma_cli_cmd_list_s *next;

    /*! CLI command object. */
    struct bcma_cli_command_s *cmd;

    /*!
     * Command feature condition.
     *
     * If non-zero, this value is checked against a list of device
     * features, and only if the feature is enabled for the current
     * device, the command will be active/visible. This field can also
     * be used to implement multiple versions of a particular command,
     * and version executed will depend on the devices it is being
     * executed on. For example, a register "get" command may need
     * different implementations for different device architectures.
     */
    int feature;

} bcma_cli_cmd_list_t;

/*!
 * \brief CLI object.
 *
 * The object is designed to provide minimal functionality by default.
 *
 * Support for environment variables can bee added through a set of
 * call-back functions. Environment variables can be useful for
 * scripting and other types of automation.
 *
 * Support for Ctrl-C can also be added through a set of call-back
 * functions. This functionality is intended to allow a Ctrl-C
 * keypress to terminate the execution of a CLI command
 * immediately.
 */
typedef struct bcma_cli_s {

    /*! Signature which indicates that object is initialized. */
    unsigned int cli_sig;

    /*! Signature for dynamically allocated objects. */
    unsigned int dyn_sig;

    /*! List of installed commands. */
    struct bcma_cli_cmd_list_s *command_root;

    /*! Input buffer from console device. */
    char ibuf[BCMA_CLI_CONFIG_INPUT_BUFFER_MAX];

    /*! Command prompt. */
    const char *prompt;

    /*! Function to read console input. */
    bcma_cli_gets_f gets;

    /*! Function for command history support. */
    bcma_cli_history_add_f history_add;

    /*! Optional exception handler for unrecognized CLI commands. */
    bcma_cli_cmd_except_f cmd_except;

    /*! Current default device number. */
    int cur_unit;

    /*! Current command device number. */
    int cmd_unit;

    /*! Call-backs for environment variable management. */
    const bcma_cli_var_cb_t *var_cb;

    /*! Cookie for environment variable operations. */
    void *var_cb_data;

    /*! Call-backs for tagged environment variable management. */
    const bcma_cli_tvar_cb_t *tvar_cb;

    /*! Cookie for tagged environment variable operations. */
    void *tvar_cb_data;

    /*! Call-backs for Ctrl-C management. */
    const bcma_cli_ctrlc_cb_t *ctrlc_cb;

    /*! Cookie for Ctrl-C operations. */
    void *ctrlc_cb_data;

    /*! Call-backs for unit management */
    const bcma_cli_unit_cb_t *unit_cb;

    /*! Cookie for unit operations. */
    void *unit_cb_data;

    /*! Call-backs for I/O redirection management. */
    const bcma_cli_redir_cb_t *redir_cb;

    /*! Cookie for I/O redirection. */
    void *redir_cb_data;

    /*! Call-back function for prompt prefix. */
    bcma_cli_prompt_prefix_f prefix_cb;

    /*! Enable to allow Ctrl-D (EOF) to exit the CLI. */
    bool eof_exit;

} bcma_cli_t;

/*!
 * \brief CLI command structure.
 *
 * This structure is used to define commands for the CLI.
 */
typedef struct bcma_cli_command_s {

    /*! Command name (mandatory). */
    char *name;

    /*! Command function (mandatory). */
    int (*func)(bcma_cli_t *cli, bcma_cli_args_t *arg);

    /*! Command cleanup function (optional). */
    int (*cleanup)(bcma_cli_t *cli);

    /*! Brief command description (mandatory). */
    char *desc;

    /*! Command syntax (recommended). */
    char *synop;

    /*! Detailed command help (recommended). */
    char *help[BCMA_CLI_CONFIG_MAX_HELP_LINES];

    /*! Command examples (recommended). */
    char *examples;

} bcma_cli_command_t;

/*! Possible results for CLI commands. */
typedef enum bcma_cmd_result_e {
    BCMA_CLI_CMD_OK          =  0,
    BCMA_CLI_CMD_FAIL        = -1,
    BCMA_CLI_CMD_USAGE       = -2,
    BCMA_CLI_CMD_NOT_FOUND   = -3,
    BCMA_CLI_CMD_EXIT        = -4,
    BCMA_CLI_CMD_INTR        = -5,
    BCMA_CLI_CMD_NO_SYM      = -6,
    BCMA_CLI_CMD_BAD_ARG     = -7,
    BCMA_CLI_CMD_AMBIGUOUS   = -8
} bcma_cmd_result_t;

/*!
 * \brief Allocate CLI object.
 *
 * \return Newly created CLI object or NULL if error.
 */
extern bcma_cli_t *
bcma_cli_create(void);

/*!
 * \brief Free previously allocated CLI object.
 *
 * This function will also free all resources associated with the CLI
 * object.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 * \retval -1 Not dynamically allocated.
 */
extern int
bcma_cli_destroy(bcma_cli_t *cli);

/*!
 * \brief Initialize CLI object.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_init(bcma_cli_t *cli);

/*!
 * \brief Deinitialize CLI object.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_cleanup(bcma_cli_t *cli);

/*!
 * \brief Clear command list.
 *
 * This function will remove all installed CLI commands and free the
 * associated resources.
 *
 * \param [in] cli CLI object.
 *
 * \retval 0 No errors.
 */
extern int
bcma_cli_cmd_list_clear(bcma_cli_t *cli);

/*!
 * \brief Create CLI command list entry.
 *
 * \return Newly created command list entry or NULL if error.
 */
extern bcma_cli_cmd_list_t *
bcma_cli_cmd_entry_create(void);

/*!
 * \brief Free previously allocated CLI command list entry.
 *
 * \param [in] cl CLI command list entry.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid entry.
 */
extern int
bcma_cli_cmd_entry_destroy(bcma_cli_cmd_list_t *cl);

/*!
 * \brief Set prompt and input call-back.
 *
 * Install function to read from input device.
 *
 * \param [in] cli CLI object.
 * \param [in] prompt String to show when prompting for commands.
 * \param [in] cb_gets Function to read commands from input device.
 * \param [in] cb_history_add Optional function to support history of
 *                            commands read from input device.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_input_cb_set(bcma_cli_t *cli, const char *prompt,
                      bcma_cli_gets_f cb_gets,
                      bcma_cli_history_add_f cb_history_add);

/*!
 * \brief Show list of available CLI commands.
 *
 * The list will be filtered based on the current device and an
 * optional prefix string. The prefix string is typically used to show
 * a list of possible commands, when an ambiguous command string has
 * been entered. Only commands starting with the supplied prefix
 * string will be listed.
 *
 * \param [in] cli CLI object.
 * \param [in] prefix Optional prefix string for comparison.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_show_cmds_avail(bcma_cli_t *cli, char *prefix);

/*!
 * \brief Lookup command in list of installed commands.
 *
 * BCMA_CLI_CMD_OK will be returned on an exact match even if one or more
 * partial matches exist. In other words, an exact match overrides
 * command ambiguity.
 *
 * \param [in] cli CLI object.
 * \param [in] name Command string token from input device.
 * \param [out] clicmd CLI command structure for matching command.
 *
 * \retval BCMA_CLI_CMD_OK Unique match was found.
 * \retval BCMA_CLI_CMD_AMBIGUOUS Two or more partial matches.
 * \retval BCMA_CLI_CMD_NOT_FOUND No matches.
 */
extern int
bcma_cli_cmd_lookup(bcma_cli_t *cli, const char *name,
                    bcma_cli_command_t **clicmd);

/*!
 * \brief Process and execute command line.
 *
 * \param [in] cli CLI object.
 * \param [in] str Raw command string.
 * \param [in] args Arguments buffer for CLI command parsing.
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_NOT_FOUND Command was not recognized.
 * \retval BCMA_CLI_CMD_AMBIGUOUS Command was not unique.
 * \retval BCMA_CLI_CMD_EXIT Command requested shell to terminate.
 * \retval BCMA_CLI_CMD_* Command returned an error.
 */
extern int
bcma_cli_cmd_args_process(bcma_cli_t *cli, const char *str,
                          bcma_cli_args_t *args);

/*!
 * \brief Process and execute command line.
 *
 * This function handles CLI command arguments implicitly and
 * runs \ref bcma_cli_cmd_args_process with Ctrl-C hook.
 *
 * \param [in] cli CLI object.
 * \param [in] str Raw command string.
 *
 * \retval BCMA_CLI_CMD_OK Command completed successfully.
 * \retval BCMA_CLI_CMD_NOT_FOUND Command was not recognized.
 * \retval BCMA_CLI_CMD_AMBIGUOUS Command was not unique.
 * \retval BCMA_CLI_CMD_EXIT Command requested shell to terminate.
 * \retval BCMA_CLI_CMD_* Command returned an error.
 */
extern int
bcma_cli_cmd_process(bcma_cli_t *cli, const char *str);

/*!
 * \brief Main CLI command loop.
 *
 * Returns only after a "quit" command has been entered.
 *
 * \param [in] cli CLI object.
 */
extern int
bcma_cli_cmd_loop(bcma_cli_t *cli);

/*!
 * \brief Add command to shell command table.
 *
 * If NULL is passed as \c clicmd, the command table is cleared.
 *
 * \param [in] cli CLI object.
 * \param [in] clicmd CLI command structure.
 * \param [in] feature Optional feature check to be performed on this command.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_add_command(bcma_cli_t *cli, bcma_cli_command_t *clicmd, int feature);

/*!
 * \brief Check if a CLI command is enabled.
 *
 * This function will check if an installed CLI command is valid for
 * the current device and configuration. The check is performed
 * through an application-provided call-back.
 *
 * \param [in] cli CLI object.
 * \param [in] feature Optional command feature.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_cmd_enabled(bcma_cli_t *cli, int feature);

/*!
 * \brief Wrapper for the call-back to read from command input device.
 *
 * When the CLI is started, it must be provided a function for reading
 * user input. The function should display the supplied \c prompt and
 * return the null-terminated user input in \c str, which is a buffer
 * of size \c max.
 *
 * \param [in] cli CLI object.
 * \param [in] prompt Command prompt to display.
 * \param [in] max Size of raw command line buffer.
 * \param [out] str Raw command line.
 *
 * \retval BCMA_CLI_CMD_OK No errors.
 * \retval BCMA_CLI_CMD_INTR Interrupt indicator is received from input device.
 * \retval BCMA_CLI_CMD_EXIT End of transmission is received from input device.
 * \retval BCMA_CLI_CMD_BAD_ARG User input exceeds command line buffer size.
 */
extern int
bcma_cli_gets(bcma_cli_t *cli, const char *prompt, int max, char *str);

/*!
 * \brief Wrapper for the Call-back for command history support.
 *
 * This function will be invoked for each command line entered into the CLI.
 *
 * If the command input function supports some form of history expansion,
 * the command string may be modified accordingly, and the CLI will process
 * the modified string instead of the original one.
 *
 * Note that this function is only invoked for command input, as this prevents
 * interactive CLI command input from being added to the command history.
 *
 * \param [in] cli CLI object.
 * \param [in] max Size of raw command line buffer.
 * \param [in,out] str Raw command line.
 */
extern void
bcma_cli_history_add(bcma_cli_t *cli, int max, char *str);

/*!
 * \brief Set exception handler for unrecognized CLI command.
 *
 * Install function to execute while CLI command is invalid.
 *
 * \param [in] cli CLI object.
 * \param [in] cmd_except Function to be called for unrecognized CLI commands.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_cmd_exception_set(bcma_cli_t *cli, bcma_cli_cmd_except_f cmd_except);

/*!
 * \brief Enable Ctrl-D (EOF) to exit CLI.
 *
 * \param [in] cli CLI object.
 * \param [in] enable Set to enable Ctrl-D to exit.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object.
 */
extern int
bcma_cli_eof_exit_set(bcma_cli_t *cli, bool enable);

/*!
 * \brief Display usage message of a CLI command.
 *
 * \param [in] cli CLI object.
 * \param[in] cmd Target CLI command.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object or bad CLI command.
 */
extern int
bcma_cli_cmd_usage_show(bcma_cli_t *cli, bcma_cli_command_t *cmd);

/*!
 * \brief Display usage message of a CLI command syntax.
 *
 * \param [in] cli CLI object.
 * \param[in] cmd Target CLI command.
 *
 * \retval 0 No errors.
 * \retval -1 Bad CLI object or bad CLI command.
 */
extern int
bcma_cli_cmd_usage_brief_show(bcma_cli_t *cli, bcma_cli_command_t *cmd);

/*!
 * \brief Register prompt prefix function.
 *
 * \param [in] cli CLI object.
 * \param[in] func Function to generate the prefix string.
 *
 * \retval 0 No errors.
 * \retval -1 Failed to register the callback function.
 */
extern int
bcma_cli_prompt_prefix_cb_set(bcma_cli_t *cli, bcma_cli_prompt_prefix_f func);

/*!
 * \brief Get prompt prefix.
 *
 * \param [in] cli CLI object
 *
 * \return The prefix string, or "" if no prefix is specified.
 */
const char *
bcma_cli_prompt_prefix(struct bcma_cli_s *cli);

#endif /* BCMA_CLI_H */
