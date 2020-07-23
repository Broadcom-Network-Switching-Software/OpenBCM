/*! \file bcma_sys_conf_sdk.c
 *
 * Single entry point initialization of SDK-based application.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bsl/bsl_names.h>

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <shr/shr_sysm_state_mon.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>
#include <bcmlt/bcmlt.h>

#include <bcma/io/bcma_io_term.h>
#include <bcma/bsl/bcma_bslmgmt.h>
#include <bcma/bsl/bcma_bslcmd.h>
#include <bcma/bsl/bcma_bslenable.h>
#include <bcma/bcmlt/bcma_bcmltcmd.h>
#include <bcma/bcmmgmt/bcma_bcmmgmt.h>
#include <bcma/bcmmgmt/bcma_bcmmgmtcmd.h>
#include <bcma/bcmmgmt/bcma_bcmmgmtcmd_config.h>
#include <bcma/bcmptm/bcma_bcmptmcmd.h>
#include <bcma/bcmpc/bcma_bcmpccmd.h>
#include <bcma/bcmtrm/bcma_bcmtrmcmd.h>
#include <bcma/ha/bcma_ha.h>
#include <bcma/ha/bcma_hacmd.h>
#include <bcma/sal/bcma_sal_alloc_debug.h>
#include <bcma/sal/bcma_salcmd.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd.h>
#include <bcma/bcmdi/bcma_bcmdicmd.h>
#include <bcma/fwm/bcma_fwmcmd.h>
#include <bcma/i2c/bcma_i2ccmd.h>
#include <bcma/hmon/bcma_hmoncmd.h>
#include <bcma/pcie/bcma_pciecmd.h>
#include <bcma/tecsup/bcma_tecsupcmd.h>
#include <bcma/board/bcma_board.h>
#include <bcma/sys/bcma_sys_conf_sdk.h>
#include <bcma/hchk/bcma_hchkcmd.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/testcase/bcma_testcase_traffic_max_power.h>
#include <bcma/test/testcase/bcma_testcase_traffic_flex.h>

#include <bcma/cint/bcma_cint_cmd.h>
#include <bcma/mcs/bcma_mcscmd.h>

/* Maximum length of string parameters from the command line */
#define MAX_STR_PARAM_LEN       255

/* Get maximum value from public define */
#define MAX_NUM_CONFIG_FILES    BCMA_BCMMGMT_MAX_NUM_CONFIG_FILES

/*******************************************************************************
 * Local variables
 */

/* System configuration structure */
static bcma_sys_conf_t sys_conf, *isc;

/* Indicate whether the BSL has been initialized */
static bool bsl_initialized = false;

/* Emulation mode indicator */
static bool emulation = false;

/* Emulation mode indicator */
static bool phymod_sim = false;

/* Warm or cold boot indicator */
static bool warm_boot = false;

/* Enable info level messages by default */
static bool info_log = false;

/* Enable error trace messages by default */
static bool err_trace = false;

/* Enable verbose error trace messages by default */
static bool err_vtrace = false;

/*
 * If set to true don't delete previous HA files. This enables running of
 * multiple instances of NGSDK on the same platform running by the same
 * user.
 */
static uint32_t ha_file_instance = BCMA_HA_INVALID_INSTANCE;

/* Start version for ISSU */
static char start_ver[MAX_STR_PARAM_LEN + 1];

/* Current version for ISSU */
static char current_ver[MAX_STR_PARAM_LEN + 1];

/* Indicator if s/w upgrade required */
static bool issu_enable = false;

/* Enable memory debug */
static bool mem_debug = true;

/* Enable memory tracking */
static bool mem_track = false;

/* Initial memory tracking prefix */
static char mem_track_prefix[MAX_STR_PARAM_LEN + 1];

/* Enable memory ID string check */
static bool mem_id_check = true;

/* Pipeline bypass mode (decoding is device-specific) */
static uint32_t bypass_mode = 0;

/* Set to true to keep the HA shared files upon exit */
static bool keep_ha_file = false;

/* Set to true to keep the kernel network interfaces active upon exit */
static bool keep_netif = false;

/* Track if devices have been probed and added to the DRD */
static bool probed = false;

/*
 * Indicates if the HA file need to be saved or compared to previously saved
 * content. The meaning of this field is dependent on the values for
 * keep_ha_file and warm_boot. If both false then the value of this variable
 * has no meaning.
 */
static bool ha_check = false;

/* Store the name of the output file where the HA state will be written to */
static char ha_dump_file[MAX_STR_PARAM_LEN + 1];

/* Use heap memory for HA storage */
static int ha_heap_mem = false;

/*******************************************************************************
 * Simple BSL hook used before the core BSL is initialized
 */

static int
bsl_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    char buf[32];

    sal_snprintf(buf, sizeof(buf), "%s: ", bsl_severity2str(meta->severity));
    bcma_io_term_write(buf, sal_strlen(buf));
    return bcma_io_term_vprintf(format, args);
}

static int
bsl_check_hook(bsl_packed_meta_t meta_pack)
{
    int source, severity;

    source = BSL_SOURCE_GET(meta_pack);
    severity = BSL_SEVERITY_GET(meta_pack);

    if (source == BSL_SRC_SHELL && severity <= BSL_SEV_INFO) {
        return 1;
    }
    if (severity <= BSL_SEV_WARN) {
        return 1;
    }
    return 0;
}

/*******************************************************************************
 * Helper function for parsing string options
 */

static int
str_opt_set(char *str_opt, const char *new_val)
{
    size_t len;

    if (new_val) {
        len = sal_strlen(new_val);
        if (len == 0 || len > MAX_STR_PARAM_LEN) {
            LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META("Failed to set option string: %s\n"), new_val));
            return -1;
        }
        sal_memcpy(str_opt, new_val, len + 1);
    }
    return 0;
}

/*******************************************************************************
 * Local CLI commands
 */

#include <sal/sal_assert.h>
static int
bcma_syscmd_probe(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int unit;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        bcma_sys_conf_sdk_init(BCMA_INIT_STAGE_PROBE);
    } else if (sal_strcmp(arg, "clean") == 0) {
        bcma_sys_conf_drd_cleanup(isc);
        probed = false;
    } else if (sal_strcmp(arg, "list") == 0) {
        for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            if (bcmdrd_dev_exists(unit)) {
                cli_out("Unit %d: %s\n", unit, bcmdrd_dev_name(unit));
            }
        }
    } else {
        cli_out("Unsupported option: %s\n", arg);
    }
    return BCMA_CLI_CMD_OK;
}

static int
bcma_syscmd_attach(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        bcma_sys_conf_sdk_init(BCMA_INIT_STAGE_SYSM);
        bcma_sys_conf_sdk_init(BCMA_INIT_STAGE_ATTACH);
    } else if (sal_strcmp(arg, "nodev") == 0) {
        bcma_sys_conf_sdk_init(BCMA_INIT_STAGE_SYSM);
    } else {
        cli_out("Unsupported option: %s\n", arg);
    }
    return BCMA_CLI_CMD_OK;
}

static int
bcma_syscmd_assert(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    assert(0);

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t shcmd_probe = {
    .name = "probe",
    .func = bcma_syscmd_probe,
    .desc = "Probe for devices.",
    .synop = "[clean|list]",
};

static bcma_cli_command_t shcmd_attach = {
    .name = "attach",
    .func = bcma_syscmd_attach,
    .desc = "Initialize and start system manager.",
    .synop = "[nodev]",
};

static bcma_cli_command_t shcmd_assert = {
    .name = "assert",
    .func = bcma_syscmd_assert,
    .desc = "Test assert function.",
};

static bcma_cli_command_t shcmd_config = {
    .name = "DebugConFiG",
    .func = bcma_bcmmgmtcmd_config,
    .desc = BCMA_BCMMGMTCMD_CONFIG_DESC,
    .synop = BCMA_BCMMGMTCMD_CONFIG_SYNOP,
    .examples = BCMA_BCMMGMTCMD_CONFIG_EXAMPLES,
    .help = { BCMA_BCMMGMTCMD_CONFIG_HELP }
};

static void
bcma_sdk_add_sys_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_probe, 0);
    bcma_cli_add_command(cli, &shcmd_attach, 0);
    bcma_cli_add_command(cli, &shcmd_assert, 0);
    bcma_cli_add_command(cli, &shcmd_config, 0);
}

static int
bcma_hacmd_warmexit(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        cli_out("Warm exit is %s.\n",
                keep_ha_file ? "on" : "off");
        cli_out("Network interfaces will be %s.\n",
                keep_netif ? "retained" : "shut down");
        return BCMA_CLI_CMD_OK;
    }

    while (arg) {
        if (sal_strcasecmp("on", arg) == 0) {
            keep_ha_file = true;
        } else if (sal_strcasecmp("off", arg) == 0) {
            keep_ha_file = false;
        } else if (sal_strcasecmp("net", arg) == 0) {
            keep_netif = true;
        } else if (sal_strcasecmp("nonet", arg) == 0) {
            keep_netif = false;
        } else {
            return BCMA_CLI_CMD_USAGE;
        }
        arg = BCMA_CLI_ARG_GET(args);
    }

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t shcmd_warmexit = {
    .name = "WarmEXIT",
    .func = bcma_hacmd_warmexit,
    .desc = "Set or display the \"warm shutdown\" status.",
    .synop = "[on|off] [net|nonet]",
    .help = {
        "Use on/off to enable/disable warm-exit and net/nonet to keep or\n"
        "shut down network interfaces upon exit."
    }
};

static void
bcma_sdk_add_ha_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_warmexit, 0);
}

/*******************************************************************************
 * BSL init/cleanup
 */

static int
init_bsl(bcma_sys_conf_t *sc)
{
    int rv;
    int layer, source;

    bsl_initialized = true;
    /* Initialize system log output */
    rv = bcma_bslmgmt_init();
    if (SHR_SUCCESS(rv)) {
        if (info_log) {
            for (layer = 0; layer < BSL_LAY_COUNT; layer++) {
                for (source = 0; source < BSL_SRC_COUNT; source++) {
                    bcma_bslenable_set(layer, source, BSL_SEV_INFO);
                }
            }
        }
        if (err_vtrace) {
            bcma_bslenable_set(BSL_LAY_SHR, BSL_SRC_ETRACE, BSL_SEV_DEBUG);
        } else if (err_trace) {
            bcma_bslenable_set(BSL_LAY_SHR, BSL_SRC_ETRACE, BSL_SEV_VERBOSE);
        }
    }
    return rv;
}

static int
cleanup_bsl(bcma_sys_conf_t *sc)
{
    bsl_initialized = false;
    return bcma_bslmgmt_cleanup();
}

/*******************************************************************************
 * SAL init/cleanup
 */

static int
init_sal(bcma_sys_conf_t *sc)
{
    /* Add SAL debug hooks */
    if (mem_debug) {
        bcma_sal_alloc_debug_init(mem_track);
        bcma_sal_alloc_debug_track_prefix_set(mem_track_prefix);
        bcma_sal_alloc_debug_idchk_enable_set(mem_id_check);
    }

    return SHR_E_NONE;
}

static int
cleanup_sal(bcma_sys_conf_t *sc)
{
    if (mem_track) {
        bcma_sal_alloc_debug_dump();
    }

    bcma_sal_alloc_debug_cleanup();

    return SHR_E_NONE;
}

/*******************************************************************************
 * CLI init/cleanup
 */

static int
init_cli(bcma_sys_conf_t *sc)
{
    /* Use default CLI prompt prefix function */
    if (bcma_sys_conf_cli_prompt_prefix_default_set(sc) < 0) {
        return SHR_E_FAIL;
    }

    /* Initialize basic CLI */
    if (bcma_sys_conf_cli_init(sc) < 0) {
        return SHR_E_FAIL;
    }

    /* Enable CLI redirection in BSL output hook */
    bcma_bslmgmt_redir_hook_set(bcma_sys_conf_cli_redir_bsl);

    /* Add CLI commands for controlling the system log */
    bcma_bslcmd_add_cmds(sc->cli);
    bcma_bslcmd_add_cmds(sc->dsh);

    /* Add bcmlt commands */
    bcma_bcmltcmd_add_cmds(sc->cli);
    /* Add bcmlt application commands */
    bcma_bcmltcmd_add_appl_cmds(sc->cli);

    /* Add bcmmgmt commands */
    bcma_bcmmgmtcmd_add_cmds(sc->cli);

    /* Add HA commands */
    bcma_sdk_add_ha_cmds(sc->cli);

    /* Add tecsup commands */
    bcma_tecsupcmd_add_cmds(sc->cli);

    /* Add Port Control commands */
    bcma_bcmpccmd_add_cmds(sc->cli);

    /* Add Port Control debug commands */
    bcma_bcmpccmd_add_dbg_cmds(sc->dsh);

    /* Add TRM commands */
    bcma_bcmtrmcmd_add_cmds(sc->dsh);

    /* Add SAL debug commands to debug shell*/
    bcma_salcmd_add_sal_cmds(sc->dsh);

    /* Add local system commands to debug shell */
    bcma_sdk_add_sys_cmds(sc->dsh);

    /* Add bcmptm debug commands to debug shell */
    bcma_bcmptmcmd_add_cmds(sc->dsh);

    /* Add Firmware Loader commands to debug shell */
    bcma_bcmdicmd_add_cmds(sc->dsh);

    /* Add MicroController Subsystem commands */
    bcma_mcscmd_add_cmds(sc->dsh);

    /* Add Firmware Management commands */
    bcma_fwmcmd_add_cmds(sc->dsh);

    /* Add I2C commands */
    bcma_i2ccmd_add_cmds(sc->dsh);

    /* Add Health Monitor commands */
    bcma_hmoncmd_add_cmds(sc->dsh);

    /* Add PCIe commands */
    bcma_pciecmd_add_cmds(sc->dsh);

    /* Add health check commands */
    bcma_hchkcmd_add_cmds(sc->dsh);

    /* Add CLI command complete support */
    bcma_sys_conf_clirlc_init();

    return SHR_E_NONE;
}

static int
cleanup_cli(bcma_sys_conf_t *sc)
{
    bcma_sys_conf_cli_cleanup(sc);

    return SHR_E_NONE;
}

/*******************************************************************************
 * DRD init/cleanup
 */

static int
init_drd(bcma_sys_conf_t *sc)
{
    int rv;
    int ndev;
    int unit;
    shr_pb_t *pb;

    if (probed) {
        LOG_WARN(BSL_LS_APPL_SHELL,
                 (BSL_META("Already probed.\n")));
        return SHR_E_NONE;
    }

    /* Probe/create devices */
    if ((ndev = bcma_sys_conf_drd_init(sc)) < 0) {
        return SHR_E_FAIL;
    }
    probed = true;

    /* Log probe info */
    pb = shr_pb_create();
    shr_pb_printf(pb, "Found %d device%s.\n", ndev, (ndev == 1) ? "" : "s");
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            shr_pb_printf(pb, "Unit %d: %s\n", unit, bcmdrd_dev_name(unit));
        }
    }
    LOG_INFO(BSL_LS_APPL_SHELL,
             (BSL_META("%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (bcmdrd_dev_exists(unit)) {
            /* Set emulation mode if requested */
            if (emulation) {
                bcmdrd_feature_enable(unit, BCMDRD_FT_EMUL);
            }
            /* Set bypass mode */
            bcmdrd_dev_bypass_mode_set(unit, bypass_mode);
            /* Use PHYMOD instead of NULL driver */
            if (phymod_sim) {
                bcmdrd_feature_enable(unit, BCMDRD_FT_PHYMOD_SIM);
            }
            /* Initialize board parameters via callback */
            rv = bcmbd_dev_reset_cb_set(unit, bcma_board_init);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
    }

    return SHR_E_NONE;
}

static int
cleanup_drd(bcma_sys_conf_t *sc)
{
    bcma_sys_conf_drd_cleanup(sc);
    probed = false;

    return SHR_E_NONE;
}

/*******************************************************************************
 * BD init/cleanup
 */

static int
init_bd(bcma_sys_conf_t *sc)
{
    if (bcma_sys_conf_bd_init(sc) < 0) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

static int
cleanup_bd(bcma_sys_conf_t *sc)
{
    if (bcma_sys_conf_bd_cleanup(sc) < 0) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Test init/cleanup
 */

/*
 * WARNING!
 *
 * The maximum power traffic test may damage the chip/board and should
 * never be run by customers. Although it is a "traffic" test, we use
 * a special test group "max_power" to avoid that developers execute
 * this test by accident.
 */
static bcma_test_case_t testcase_traffic_max_power = {
    "max_power",
    517,
    "Max power traffic",
    BCMA_TESTCASE_TRAFFIC_MAX_POWER_DESC,
    BCMA_TESTCASE_TRAFFIC_MAX_POWER_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_max_power_op_get,
};

/*
 * WARNING!
 *
 * The flex port traffic test may cause crash issue due to
 * combination flexibility and validation, and should
 * never be run by customers. Although it is a "traffic" test, we use
 * a special test group "flex_port" to avoid that developers execute
 * this test by accident.
 */
static bcma_test_case_t testcase_traffic_flex = {
    "flex_port",
    514,
    "Flex port traffic",
    BCMA_TESTCASE_TRAFFIC_FLEX_DESC,
    BCMA_TESTCASE_TRAFFIC_FLEX_FLAGS,
    1,
    NULL,
    bcma_testcase_traffic_flex_op_get,
};

static int
init_test(bcma_sys_conf_t *sc)
{
    /* Initialize test component */
    if (bcma_sys_conf_test_init(sc) < 0) {
        return SHR_E_FAIL;
    }
    bcma_sys_conf_test_testcase_add(&testcase_traffic_max_power, 0);
    bcma_sys_conf_test_testcase_add(&testcase_traffic_flex, 0);

    return SHR_E_NONE;
}

static int
cleanup_test(bcma_sys_conf_t *sc)
{
    /* Cleanup test component */
    bcma_sys_conf_test_cleanup(sc);

    return SHR_E_NONE;
}

/*******************************************************************************
 * BCMPKT init/cleanup
 */

static int
init_pkt(bcma_sys_conf_t *sc)
{
    /* Add CLI commands for packet I/O driver */
    bcma_bcmpktcmd_add_cmds(sc->cli);

    return SHR_E_NONE;
}

static int
cleanup_pkt(bcma_sys_conf_t *sc)
{
    return SHR_E_NONE;
}

/*******************************************************************************
 * HA I/O init/cleanup
 */

static int
init_ha(bcma_sys_conf_t *sc)
{
    int rv = SHR_E_NONE;

    /* Add CLI commands for HA memory diagnostics */
    bcma_hacmd_add_cmds(sc->dsh);

    if (!ha_heap_mem) {
        rv = bcma_ha_mem_init_all(warm_boot, ha_file_instance);
    }

    return rv;
}

static int
cleanup_ha(bcma_sys_conf_t *sc)
{
    int rv = SHR_E_NONE;

    if (!ha_heap_mem) {
        rv = bcma_ha_mem_cleanup_all(keep_ha_file);
    }

    return rv;
}

/*******************************************************************************
 * SYSM init/cleanup
 */

static void
state_monitor_func(int unit, shr_sysm_states_t state, void *context)
{
    if (state == SHR_SYSM_RUN && unit != -1) {
        bcma_ha_mem_state_comp(unit, ha_dump_file);
    }
}

static int
init_sysm(bcma_sys_conf_t *sc)
{
    int rv;
    bool core_only;
    bcmmgmt_issu_info_t issu_info;

    issu_info.start_ver = start_ver;
    issu_info.current_ver = current_ver;

    if (ha_check && warm_boot) {
        rv = shr_sysm_state_monitor_register(state_monitor_func, NULL);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    /*
     * Several ISSU-related boot scenarios require that we start all
     * devices immediately after the core is started, so we always set
     * core_only to false in order to get consistent behavior and
     * results.
     *
     * In order to test device hot-plug behavior, the system should be
     * booted to the CLI stage, and the system manager should be
     * started before we probe for devices. Once probing is done, each
     * device can be attached via the system manager.
     */
    core_only = false;

    rv = bcma_bcmmgmt_init(warm_boot, core_only, BCMA_SYS_CONF_CONFIG_YML,
                           issu_enable ? &issu_info : NULL);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    if (ha_check && warm_boot) {
        shr_sysm_state_monitor_unregister(state_monitor_func);
    }
    return rv;
}

static int
cleanup_sysm(bcma_sys_conf_t *sc)
{
    int rv;

    rv = bcma_bcmmgmt_shutdown(true, keep_netif);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    if (ha_check && keep_ha_file) {
        return bcma_ha_mem_state_dump_all(ha_dump_file);
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * Device init/cleanup
 */

static int
init_devs(bcma_sys_conf_t *sc)
{
    return bcmmgmt_dev_attach_all(warm_boot);
}

static int
cleanup_devs(bcma_sys_conf_t *sc)
{
    /*
     * Defer this to shutdown in order to honor the keep_netif flag.
     * Please refer to cleanup_sysm() above.
     */
    return SHR_E_NONE;
}

/*******************************************************************************
 * Local data
 */

/*
 * The array below determines the order in which components are
 * initialized and cleaned up.
 *
 * The clean-up sequence will be done in reverse order of the
 * initialization sequence.
 */
static struct sdk_init_s {
    const char *name;
    int (*init)(bcma_sys_conf_t *sc);
    int (*cleanup)(bcma_sys_conf_t *sc);
    bcma_sys_init_stage_t init_stage;
} sdk_init[] = {
    /*
     * The elements in the array below can be customized based on the
     * application needs. Next to each line there is an indication
     * whether the functions are optional (O) or mandatory
     * (M). However even mandatory initialization function can be
     * customized.
     */
    { "SAL",    init_sal,     cleanup_sal,    BCMA_INIT_STAGE_BASE },   /* O */
    { "BSL",    init_bsl,     cleanup_bsl,    BCMA_INIT_STAGE_BASE },   /* O */
    { "CLI",    init_cli,     cleanup_cli,    BCMA_INIT_STAGE_CLI },    /* O */
    { "BD",     init_bd,      cleanup_bd,     BCMA_INIT_STAGE_CLI },    /* O */
    { "PKT",    init_pkt,     cleanup_pkt,    BCMA_INIT_STAGE_CLI },    /* O */
    { "Test",   init_test,    cleanup_test,   BCMA_INIT_STAGE_CLI },    /* O */
    { "DRD",    init_drd,     cleanup_drd,    BCMA_INIT_STAGE_PROBE },  /* M */
    { "HA",     init_ha,      cleanup_ha,     BCMA_INIT_STAGE_SYSM },   /* M */
    { "SYSM",   init_sysm,    cleanup_sysm,   BCMA_INIT_STAGE_SYSM },   /* M */
    { "Attach", init_devs,    cleanup_devs,   BCMA_INIT_STAGE_ATTACH }, /* O */
};

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_sdk_init(bcma_sys_init_stage_t init_stage)
{
    struct sdk_init_s *si;
    int rv;
    int idx;

    /* Initialize system configuration structure */
    if (!isc) {
        isc = &sys_conf;
        bcma_sys_conf_init(isc);
    }

    for (idx = 0; idx < COUNTOF(sdk_init); idx++) {
        si = &sdk_init[idx];
        if (!si->init) {
            continue;
        }

        if (init_stage != si->init_stage &&
            init_stage != BCMA_INIT_STAGE_ALL) {
            continue;
        }
        rv = si->init(isc);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META(
                          "Failed to initialize %s: %s\n"),
                       si->name, shr_errmsg(rv)));
            return -1;
        }
    }

    bcma_cintcmd_add_cint_cmd(isc->cli);

    /* Auto-run CLI script */
    bcma_sys_conf_rcload_run(isc);

    return 0;
}

int
bcma_sys_conf_sdk_run(void)
{
    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* CLI main loop */
    bcma_cli_cmd_loop(isc->cli);

    return SHR_E_NONE;
}

int
bcma_sys_conf_sdk_cleanup(void)
{
    struct sdk_init_s *si;
    int rv;
    int idx;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    for (idx = COUNTOF(sdk_init) - 1; idx >= 0; idx--) {
        si = &sdk_init[idx];
        if (!si->cleanup) {
            continue;
        }
        rv = si->cleanup(isc);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META(
                              "Failed to clean up %s: %s\n"),
                       si->name, shr_errmsg(rv)));
            return -1;
        }
    }

    /* Release system configuration structure */
    bcma_sys_conf_cleanup(isc);
    isc = NULL;

    return 0;
}

int
bcma_sys_conf_sdk_option_set(bcma_sys_conf_opt_t opt,
                             int val, const char *valstr)
{
    int rv;

    /* Allow console output before the core BSL is initialized */
    if (!bsl_initialized) {
        bsl_config_t bsl_config;

        bsl_config_t_init(&bsl_config);
        bsl_config.out_hook = bsl_out_hook;
        bsl_config.check_hook = bsl_check_hook;
        bsl_init(&bsl_config);
    }

    switch (opt) {
    case BCMA_SYS_CONF_OPT_MEM_DEBUG:
        mem_debug = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_MEM_TRACK:
        mem_track = (val != 0);
        if (str_opt_set(mem_track_prefix, valstr) < 0) {
            return -1;
        }
        break;
    case BCMA_SYS_CONF_OPT_MEM_ID_CHECK:
        mem_id_check = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_EMULATION:
        emulation = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_WARM_BOOT:
        warm_boot = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_CONFIG_FILE:
        if (valstr == NULL) {
            valstr = BCMA_SYS_CONF_CONFIG_YML;
        }
        rv = bcma_bcmmgmt_cfgyml_file_set(valstr, val);
        if (SHR_FAILURE(rv)) {
            switch (rv) {
            case SHR_E_FULL:
                LOG_WARN(BSL_LS_APPL_SHELL,
                         (BSL_META("Too many configuration files - skipping %s\n"),
                          valstr));
                break;
            case SHR_E_PARAM:
                LOG_WARN(BSL_LS_APPL_SHELL,
                         (BSL_META("Invalid configuration file: %s\n"),
                          valstr));
                break;
            case SHR_E_NOT_FOUND:
                LOG_WARN(BSL_LS_APPL_SHELL,
                         (BSL_META("Configuration file %s does not exist\n"),
                          valstr));
                break;
            default:
                break;
            }
            return -1;
        }
        break;
    case BCMA_SYS_CONF_OPT_BYPASS_MODE:
        bypass_mode = val;
        break;
    case BCMA_SYS_CONF_OPT_KEEP_HA_FILE:
        keep_ha_file = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_KEEP_NETIF:
        keep_netif = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_PHYMOD_SIM:
        phymod_sim = (val != 0);
        break;
    case BCMA_SYS_CONF_ISSU_MODE:
        warm_boot = true;
        issu_enable = true;
        break;
    case BCMA_SYS_CONF_ISSU_START_VER:
        if (str_opt_set(start_ver, valstr) < 0) {
            return -1;
        }
        break;
    case BCMA_SYS_CONF_ISSU_TARGET_VER:
        if (str_opt_set(current_ver, valstr) < 0) {
            return -1;
        }
        break;
    case BCMA_SYS_CONF_OPT_HA_CHECK:
        ha_check = (val != 0);
        if (ha_check) {
            if (str_opt_set(ha_dump_file, valstr) < 0) {
                return -1;
            }
        }
        break;
    case BCMA_SYS_CONF_OPT_HA_HEAP_MEM:
        ha_heap_mem = (val != 0);
        break;
    case BCMA_SYS_CONF_HA_NO_DEL:
        ha_file_instance = val;
        break;
    case BCMA_SYS_CONF_OPT_RCLOAD:
        if (bcma_sys_conf_rcload_set(valstr) < 0) {
            return -1;
        }
        break;
    case BCMA_SYS_CONF_OPT_INFO_LOG:
        info_log = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_ERR_TRACE:
        err_trace = (val != 0);
        break;
    case BCMA_SYS_CONF_OPT_ERR_VTRACE:
        err_vtrace = (val != 0);
        break;
    default:
        return -1;
    }
    return 0;
}

int
bcma_sys_conf_sdk_debug_shell(void)
{
    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return -1;
    }

    /* Debug shell main loop */
    bcma_cli_cmd_loop(isc->dsh);

    return SHR_E_NONE;
}
