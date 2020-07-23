/*! \file bcma_sys_conf.h
 *
 * Shared system configuration API.
 *
 * Convenience functions for initilizing various SDK components.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SYS_CONF_H
#define BCMA_SYS_CONF_H

#include <bsl/bsl.h>
#include <bcma/cli/bcma_cli.h>
#include <bcma/env/bcma_env.h>
#include <bcma/cfgdb/bcma_cfgdb.h>
#include <bcma/test/bcma_test.h>

/*! Default YAML configuration file name. */
#ifndef BCMA_SYS_CONF_CONFIG_YML
#define BCMA_SYS_CONF_CONFIG_YML "config.yml"
#endif

/*! Default CLI auto-run script file name. */
#ifndef BCMA_SYS_CONF_RCLOAD_SCRIPT
#define BCMA_SYS_CONF_RCLOAD_SCRIPT "rc.soc"
#endif

/*!
 * \brief System configuration options.
 *
 * Used to enable or disable optional features.
 */
typedef enum bcma_sys_conf_opt_e {

    /*!
     * Boolean option to turn off memory debugging, i.e. the sal_alloc
     * debug hooks will not be added.
     */
    BCMA_SYS_CONF_OPT_MEM_DEBUG = 0,

    /*!
     * Boolean option to turn on memory tracking, which means that all
     * allocation done through sal_alloc will be stored in a database,
     * which can be dumped at will.
     */
    BCMA_SYS_CONF_OPT_MEM_TRACK,

    /*!
     * Boolean option to turn on ID string checking for any calls to
     * sal_alloc. The ID string will be checked for conformance to the
     * SDK naming guidelines.
     */
    BCMA_SYS_CONF_OPT_MEM_ID_CHECK,

    /*!
     * Boolean option to run the SDK on a chip emulator instead of a
     * real device. Typically this will tweak the initialization
     * sequence and various timeout values.
     */
    BCMA_SYS_CONF_OPT_EMULATION,

    /*!
     * Boolean option to force a warm-boot initialization sequence.
     */
    BCMA_SYS_CONF_OPT_WARM_BOOT,

    /*!
     * Use this option to set the name of the SDK configuration
     * file(s). The file name must be passed as the string value and
     * the (zero-based) configuration file number must be passed as
     * the integer value.
     */
    BCMA_SYS_CONF_OPT_CONFIG_FILE,

    /*!
     * Use this option to set the string of the SDK configuration.
     * The configuration string must be passed as the string value and
     * the (zero-based) configuration string number must be passed as
     * the integer value.
     */
    BCMA_SYS_CONF_OPT_CONFIG_STRING,

    /*!
     * Bypass mode is a device-specific integer value that specified
     * the pipeline bypass mode. Various bypass modes typically lowers
     * packet latency at the expense of packet processing features.
     */
    BCMA_SYS_CONF_OPT_BYPASS_MODE,

    /*!
     * Boolean option to indicate whether the HA (High-Availability)
     * file should be deleted upon exit. This option enables the
     * testing of warm boot as the previous state (prior to warm boot)
     * will be kept in HA memory.
     */
    BCMA_SYS_CONF_OPT_KEEP_HA_FILE,

    /*!
     * Boolean option to indicate whether the SDK core network
     * interfaces should be retained upon exit. On Linux systems this
     * allows the KNET kernel driver to keep processing switch packets
     * after the user mode application has exited.
     */
    BCMA_SYS_CONF_OPT_KEEP_NETIF,

    /*!
     * Boolean option to indicate whether to use a real PHY simulator
     * instead of a simple dummy PHY driver in simulation mode.
     */
    BCMA_SYS_CONF_OPT_PHYMOD_SIM,

    /*!
     * Boolean option to indicate if ISSU should be perfromed
     */
    BCMA_SYS_CONF_ISSU_MODE,

    /*!
     * ISSU start version
     */
    BCMA_SYS_CONF_ISSU_START_VER,

    /*!
     * ISSU Target version
     */
    BCMA_SYS_CONF_ISSU_TARGET_VER,

    /*
     * Boolean option indicating if the content of the HA memory should be saved
     * into a file or compared to a previously saved content.
     * The logic of this flag is impacted by the flags
     * BCMA_SYS_CONF_OPT_KEEP_HA_FILE and BCMA_SYS_CONF_OPT_WARM_BOOT.
     * If this flag is set along with the BCMA_SYS_CONF_OPT_KEEP_HA_FILE flag
     * then the content of the HA memory will be stored in a file. If this flag
     * is set along with BCMA_SYS_CONF_OPT_WARM_BOOT then the system will
     * compare the content of the HA with a previously stored content.
     */
    BCMA_SYS_CONF_OPT_HA_CHECK,

    /*
     * Boolean option to be used when running multiple instances of NGSDK on
     * the same machine and by the same user. In such scenario without setting
     * this flag the BCMA_HA will clear out all "old" files from the system.
     * The actual deletion will fail but the new file will use the same name
     * causing two instances to share an HA file and overwrite each other.
     */
    BCMA_SYS_CONF_HA_NO_DEL,

    /*!
     * Use this option to set the name of the auto-run CLI script file after
     * the system initialization process is done. The file name must be passed
     * as the string value.
     */
    BCMA_SYS_CONF_OPT_RCLOAD,

    /*!
     * Boolean option to enable error trace logging before the system
     * is initialized. This can be turned off again at a later time
     * via the 'debug etrc' command.
     */
    BCMA_SYS_CONF_OPT_ERR_TRACE,

    /*!
     * Boolean option to enable verbose error trace logging before the
     * system is initialized. This can be turned off again at a later
     * time via the 'debug etrc' command. This option overrides \ref
     * BCMA_SYS_CONF_OPT_ERR_TRACE.
     */
    BCMA_SYS_CONF_OPT_ERR_VTRACE,

    /*!
     * Boolean option to enable INFO level logging before the system
     * is initialized. Logging levels can be restored to the default
     * settings again at a later time via the 'debug =' command.
     */
    BCMA_SYS_CONF_OPT_INFO_LOG,

    /*
     * Boolean option to disable true HA memory. Instead, HA memory
     * will be allocated from normal heap memory. Warm boot is not
     * supported when this option is enabled.
     */
    BCMA_SYS_CONF_OPT_HA_HEAP_MEM,

    /*! Should always be last. */
    BCMA_SYS_CONF_OPT_MAX

} bcma_sys_conf_opt_t;

/*!
 * \brief System configuration object.
 *
 * Used mainly to track resources allocated during system
 * initialization.
 */
typedef struct bcma_sys_conf_s {

    /*! Signature which indicates that object is initialized. */
    unsigned int sys_conf_sig;

    /*! Pointer to CLI object. */
    bcma_cli_t *cli;

    /*! Pointer to debug shell object. */
    bcma_cli_t *dsh;

    /*! CLI environment handle. */
    bcma_env_handle_t eh;

    /*! Configuration database (SDK6-style). */
    bcma_cfgdb_cfg_t cfgdb_cfg;

    /*! Call-back function for CLI prompt prefix. */
    bcma_cli_prompt_prefix_f cli_prompt_prefix_cb;

} bcma_sys_conf_t;

/*!
 * \brief Initialize system configuration data structure.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_init(bcma_sys_conf_t *sc);

/*!
 * \brief Release system configuration data structure resources.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize shared (SHR) libraries.
 *
 * Some shared libraries need to perform basic initialization
 * (resource allocation) before they can be used.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_shr_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up shared (SHR) libraries.
 *
 * This function will clean up all resources allocated by \ref
 * bcma_sys_conf_shr_init.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_shr_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize a basic CLI.
 *
 * Initialize basic CLI including terminal I/O, Ctrl-C handler, basic
 * CLI commands, shell variables, etc.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cli_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up basic CLI resources.
 *
 * This function will clean up all resources allocated by \ref
 * bcma_sys_conf_cli_init.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cli_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief CLI redirection hook for BSL output sink.
 *
 * This function should be installed into the BSL output hook to allow
 * command output to be fed back into the CLI input function.
 *
 * The call signature is identical to the BSL output hook itself.
 *
 * \param [in] meta Meta data for this log message.
 * \param [in] format A printf-style format string.
 * \param [in] args List of printf-style arguments.
 *
 * \return Negative value on error.
 */
extern int
bcma_sys_conf_cli_redir_bsl(bsl_meta_t *meta, const char *format, va_list args);

/*!
 * \brief Read CLI command from input device.
 *
 * This function configures and returns the call-back function
 * to read a command from input device for CLI.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return Call-back function pointer of CLI to read commands from input device.
 */
extern bcma_cli_gets_f
bcma_sys_conf_cli_gets(bcma_sys_conf_t *sc);

/*!
 * \brief CLI command history support.
 *
 * This function returns the optional call-back function for command history
 * support from the application.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return Call-back function pointer of CLI command history support.
 */
extern bcma_cli_history_add_f
bcma_sys_conf_cli_history_add(bcma_sys_conf_t *sc);

/*!
 * \brief Use the default prompt prefix handler.
 *
 * The default prompt prefix handler would add a leading asterisk '*' to
 * CLI prompt when the device is tainted.
 *
 * A tainted device means one or more PTs of the device are written via
 * PT passthrough mode or direct access from CLI commands.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cli_prompt_prefix_default_set(bcma_sys_conf_t *sc);

/*!
 * \brief Set CLI auto-run script file.
 *
 * \param [in] rc_file The CLI auto-run script file name.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_rcload_set(const char *rc_file);

/*!
 * \brief Run CLI script file.
 *
 * If no script file is set through \ref bcma_sys_conf_rcload_set,
 * a predefined \ref BCMA_SYS_CONF_RCLOAD_SCRIPT string will be used
 * as the CLI script if this file exists in the current working directory.
 *
 * \param [in] sc System configuration data structure.
 *
 * \result BCMA_CLI_CMD_OK if no errors, otherwise BCMA_CLI_CMD_xxx.
 */
extern int
bcma_sys_conf_rcload_run(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize CLI readline completion.
 *
 * Initialize CLI readline completion and add the commands that support
 * completion feature to the command completion table.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_clirlc_init(void);

/*!
 * \brief Initialize and populate configuration database.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cfgdb_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up configuration database.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_cfgdb_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize device resource database.
 *
 * This function will probe for supported devices and add them to the
 * device resource database.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_drd_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up device resources for installed devices.
 *
 * This function will essentially undo a device probe.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_drd_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize base driver CLI commands.
 *
 * This function will install CLI commands for operating directly on
 * the base driver (BCMBD).
 *
 * The commands will be added to the debug shell.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_bd_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up base driver CLI commands.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_bd_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up pkt I/O driver for installed devices.
 *
 * This function will clean up packet I/O resources and datapath, and detach
 * the packet I/O driver.
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_pkt_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize packet I/O driver for installed devices.
 *
 * This function will attach packet I/O driver for each device.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_pkt_init(bcma_sys_conf_t *sc);

/*!
 * \brief Initialize test component for installed devices.
 *
 * This function will probe for supported devices and initialize the
 * test conponent.
 *
 * \param [in] sc Initialized system configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_test_init(bcma_sys_conf_t *sc);

/*!
 * \brief Clean up test component for installed devices.
 *
 * This function will detach the test component .
 *
 * \param [in] sc System configuration data structure.
 *
 * \return 0 if no errors, otherwise -1.
 */
extern int
bcma_sys_conf_test_cleanup(bcma_sys_conf_t *sc);

/*!
 * \brief Add test case to test database.
 *
 * Add optional test cases to test database previously created via
 * \ref bcma_sys_conf_test_init.
 *
 * \param [in] tc Test case object.
 * \param [in] feature Device feature required for this test case.
 *
 * \retval 0 No errors.
 */
extern int
bcma_sys_conf_test_testcase_add(bcma_test_case_t *tc, int feature);

#endif /* BCMA_SYS_CONF_H */
