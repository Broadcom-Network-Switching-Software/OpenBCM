/*! \file bcma_sys_conf_hybrid_sdk.c
 *
 * Single entry point initialization of SDK-based application.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <shr/shr_sysm_state_mon.h>
#include <shr/shr_lmem_mgr.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt.h>
#include <bcmmgmt/bcmmgmt_sysm_default.h>
#include <bcmlt/bcmlt.h>

#include <bcma/bcmmgmt/bcma_bcmmgmt.h>
#include <bcma/ha/bcma_ha.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_knet.h>
#include <bcma/bcmpkt/bcma_bcmpktcmd_pmddecode.h>
#include <bcma/sys/bcma_sys_conf_hybrid_sdk.h>

/* Maximum length of string parameters from the command line */
#define MAX_STR_PARAM_LEN       255

/* Get maximum value from public define */
#define MAX_NUM_CONFIG_FILES    BCMA_BCMMGMT_MAX_NUM_CONFIG_FILES

#define MAX_NUM_CONFIG_STRINGS  MAX_NUM_CONFIG_FILES

/*******************************************************************************
 * Local variables
 */

/* System configuration structure */
static bcma_sys_conf_t sys_conf, *isc;

/* Emulation mode indicator */
static bool emulation = false;

/* Emulation mode indicator */
static bool phymod_sim = false;

/* Warm or cold boot indicator */
static bool warm_boot = false;

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

/* Configuration string. */
static char *cfg_string[MAX_NUM_CONFIG_STRINGS] = { NULL };

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

static void
lmm_stats_header_format(shr_pb_t *pb)
{
    shr_pb_printf(pb, "Statistics in number of elements:\n");
    shr_pb_printf(pb, "C: elements number per Chunk, A: Allocated, I: In-use"
                      ", H: High-water-mark\nE: Element-size (in byte)\n");
    shr_pb_printf(pb, "%-25s %9s %5s %5s %5s %5s %4s %8s\n",
                  "ID", "Available", "C", "A", "I", "H", "E",
                  "Allocated-size");
    shr_pb_printf(pb, "=================================================="
                      "==============================\n");
}

static void
lmm_stats_format(shr_pb_t *pb, shr_lmm_stat_t *stats, size_t cnt)
{
    size_t idx = 0;

    do {
        uint32_t bytes, hbytes;
        char *hstr;

        bytes = (uint32_t)stats->element_size * stats->allocated;
        hbytes = bytes;
        hbytes /= 1024;
        hstr = "K";
        if (hbytes >= 1024) {
            hbytes /= 1024;
            hstr = "M";
        }

        shr_pb_printf(pb, "%-30s", stats->label);
        if ((stats->avail + stats->in_use) == 0xffffffff) {
            /* The maximum number of elements is undefined */
            shr_pb_printf(pb, " %4s", "n/a");
        } else {
            shr_pb_printf(pb, " %4"PRIu32, stats->avail);
        }
        shr_pb_printf(pb, " %5"PRIu32" %5"PRIu32" %5"PRIu32" %5"PRIu32,
                      (uint32_t)stats->chunk_size, stats->allocated,
                      stats->in_use, stats->hwtr_mark);
        shr_pb_printf(pb, " %4"PRIu32" %8"PRIu32" (%"PRIu32"%s)\n",
                      (uint32_t)stats->element_size, bytes, hbytes, hstr);
        stats++;
    } while (++idx < cnt);
}

static int
lmm_stats(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    shr_pb_t *pb;
    shr_lmm_stat_t *lmm_stat;
    size_t stats_cnt = 0;
    bool get_all = false;
    int rv = SHR_E_NONE;

    stats_cnt = BCMA_CLI_ARG_CNT(args);
    if (stats_cnt == 0) {
        shr_lmm_stat_get_all(0, NULL, &stats_cnt);
        if (stats_cnt <= 0) {
            cli_out("%sFailed to get statistics count\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            return BCMA_CLI_CMD_FAIL;
        }
        get_all = true;
    }
    lmm_stat = sal_alloc(stats_cnt * sizeof(shr_lmm_stat_t), "bcmaLmmStats");
    if (lmm_stat == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(lmm_stat, 0, stats_cnt * sizeof(shr_lmm_stat_t));

    if (get_all) {
        rv = shr_lmm_stat_get_all(stats_cnt, lmm_stat, NULL);
    } else {
        shr_lmm_stat_t *stat = lmm_stat;

        while ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
            rv = shr_lmm_stat_by_id_get(arg, stat++);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
    }

    if (SHR_SUCCESS(rv)) {
        pb = shr_pb_create();

        lmm_stats_header_format(pb);
        lmm_stats_format(pb, lmm_stat, stats_cnt);
        cli_out("%s", shr_pb_str(pb));

        shr_pb_destroy(pb);
    } else {
        cli_out("%sFailed to get statistics for %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, get_all ? "all" : arg);
    }

    sal_free(lmm_stat);

    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
shr_cmd_lmm(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "stats") == 0) {
        return lmm_stats(cli, args);
    }

    return BCMA_CLI_CMD_USAGE;
}

static bcma_cli_command_t shcmd_lmm = {
    .name = "LMM",
    .func = shr_cmd_lmm,
    .desc = "Memory usage statistics from Local Memory Manager.",
    .synop = "stats [<ID-string1> [<ID-string2>] ...]",
};

static void
add_sal_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_lmm, 0);
}

static bcma_cli_command_t shcmd_knet = {
    .name = "knet",
    .func = bcma_bcmpktcmd_knet,
    .desc = BCMA_BCMPKTCMD_KNET_DESC,
    .synop = BCMA_BCMPKTCMD_KNET_SYNOP,
    .help = { BCMA_BCMPKTCMD_KNET_HELP },
    .examples = BCMA_BCMPKTCMD_KNET_EXAMPLES
};

static bcma_cli_command_t shcmd_pmddecode = {
    .name = "pmddecode",
    .func = bcma_bcmpktcmd_pmddecode,
    .desc = BCMA_BCMPKTCMD_PMDDECODE_DESC,
    .synop = BCMA_BCMPKTCMD_PMDDECODE_SYNOP,
    .help = { BCMA_BCMPKTCMD_PMDDECODE_HELP },
    .examples = BCMA_BCMPKTCMD_PMDDECODE_EXAMPLES
};

static void
add_pkt_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_knet, 0);
    bcma_cli_add_command(cli, &shcmd_pmddecode, 0);
}

/*******************************************************************************
 * CLI init/cleanup
 */

static int
init_cli(bcma_sys_conf_t *sc)
{
    int rv;

    /* Initialize CLI infracstructure with default switch command set */
    rv = bcma_sys_conf_cli_default(sc);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Add local commands for packet I/O driver */
    add_pkt_cmds(sc->cli);

    /* Add local SAL commands to debug shell */
    add_sal_cmds(sc->dsh);

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
        }
    }

    /* Initialize CLI unit callback after probing devices */
    bcma_sys_conf_drd_cli_init(sc);

    return ndev;
}

static int
cleanup_drd(bcma_sys_conf_t *sc)
{
    bcma_sys_conf_drd_cli_cleanup(sc);

    bcma_sys_conf_drd_cleanup(sc);
    probed = false;

    return SHR_E_NONE;
}

/*******************************************************************************
 * Test init/cleanup
 */

static int
init_test(bcma_sys_conf_t *sc)
{
    /* Initialize test component */
    if (bcma_sys_conf_test_init(sc) < 0) {
        return SHR_E_FAIL;
    }

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
 * HA I/O init/cleanup
 */

static int
init_ha(bcma_sys_conf_t *sc)
{
    int rv = SHR_E_NONE;

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

    rv = bcmmgmt_core_init();
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Load the configuration strings if no filesystem and cold boot. */
    if (!warm_boot) {
        int idx;
        for (idx = 0; idx < MAX_NUM_CONFIG_STRINGS; idx++) {
            if (cfg_string[idx] == NULL) {
                continue;
            }
            rv = bcmmgmt_core_config_parse(cfg_string[idx]);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LS_APPL_SHELL,
                         (BSL_META("Failed to parse cfg string: %s\n"),
                         cfg_string[idx]));
            }
            SHR_FREE(cfg_string[idx]);
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
    core_only = true;

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
    { "CLI",    init_cli,     cleanup_cli,    BCMA_INIT_STAGE_CLI },    /* O */
    { "Test",   init_test,    cleanup_test,   BCMA_INIT_STAGE_CLI },    /* O */
    { "DRD",    init_drd,     cleanup_drd,    BCMA_INIT_STAGE_PROBE },  /* M */
    { "HA",     init_ha,      cleanup_ha,     BCMA_INIT_STAGE_SYSM },   /* M */
    { "SYSM",   init_sysm,    cleanup_sysm,   BCMA_INIT_STAGE_SYSM },   /* M */
};

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_hybrid_sdk_init(bcma_sys_init_stage_t init_stage)
{
    struct sdk_init_s *si;
    int rv = -1;
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

    return (init_stage == BCMA_INIT_STAGE_PROBE ? rv : 0);
}

int
bcma_sys_conf_hybrid_sdk_cli_redir_bsl(bsl_meta_t *meta, const char *format,
                                       va_list args)
{
    return bcma_sys_conf_cli_redir_bsl(meta, format, args);
}

int
bcma_sys_conf_hybrid_sdk_run(int unit)
{
    return bcma_sys_conf_hybrid_sdk_bsh(unit);
}

int
bcma_sys_conf_hybrid_sdk_bsh(int unit)
{
    bcma_cli_t *bsh;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    bsh = isc->cli;

    /* Sanity check */
    if (!bsh) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Sync current unit and command unit for debug shell */
    bsh->cur_unit = unit;
    bsh->cmd_unit = unit;

    /* CLI main loop */
    bcma_cli_cmd_loop(bsh);

    return BCMA_CLI_CMD_OK;
}

int
bcma_sys_conf_hybrid_sdk_bsh_process(int unit, const char *str)
{
    bcma_cli_t *bsh;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    bsh = isc->cli;

    /* Sanity check */
    if (!bsh) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Sync current unit and command unit for debug shell */
    bsh->cur_unit = unit;
    bsh->cmd_unit = unit;

    /* Execute debug command */
    return bcma_cli_cmd_process(bsh, str);
}

int
bcma_sys_conf_hybrid_sdk_dsh(int unit)
{
    bcma_cli_t *dsh;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    dsh = isc->dsh;

    /* Sanity check */
    if (!dsh) {
        return BCMA_CLI_CMD_FAIL;
    }

    /* Sync current unit and command unit for debug shell */
    dsh->cur_unit = unit;
    dsh->cmd_unit = unit;

    /* CLI main loop */
    bcma_cli_cmd_loop(dsh);

    return BCMA_CLI_CMD_OK;
}

int
bcma_sys_conf_hybrid_sdk_dsh_process(int unit, const char *str)
{
    bcma_cli_t *dsh;

    /* Check for valid sys_conf structure */
    if (isc == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    dsh = isc->dsh;

    /* Sanity check */
    if (!dsh) {
        return BCMA_CLI_CMD_FAIL;
    }
    /* Sync current unit and command unit for debug shell */
    dsh->cur_unit = unit;
    dsh->cmd_unit = unit;

    /* Execute debug command */
    return bcma_cli_cmd_process(dsh, str);
}

int
bcma_sys_conf_hybrid_sdk_cleanup(bcma_sys_init_stage_t init_stage)
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

        if ((init_stage != si->init_stage) &&
            (init_stage != BCMA_INIT_STAGE_ALL)) {
            continue;
        }
        rv = si->cleanup(isc);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META(
                              "Failed to clean up %s: %s\n"),
                       si->name, shr_errmsg(rv)));
            continue;
        }
    }

    if (init_stage == BCMA_INIT_STAGE_ALL) {
        /* Release system configuration structure */
        bcma_sys_conf_cleanup(isc);
        isc = NULL;
    }

    return 0;
}

bool
bcma_sys_conf_hybrid_sdk_attached(int unit)
{
    return bcmmgmt_dev_attached(unit);
}

int
bcma_sys_conf_hybrid_sdk_attach(int unit)
{
    return bcmmgmt_dev_attach(unit, warm_boot);
}

int
bcma_sys_conf_hybrid_sdk_detach(int unit)
{
    return bcmmgmt_dev_detach(unit);
}

int
bcma_sys_conf_hybrid_sdk_option_set(bcma_sys_conf_opt_t opt,
                                    int val, const char *valstr)
{
    int rv;

    switch (opt) {
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
    case BCMA_SYS_CONF_OPT_CONFIG_STRING:
        if (valstr == NULL) {
            return -1;
        }
        if ((unsigned int)val >= MAX_NUM_CONFIG_STRINGS) {
            LOG_WARN(BSL_LS_APPL_SHELL,
                     (BSL_META("Too many configuration strings - skipping %s\n"),
                     valstr));
            break;
        }
        SHR_FREE(cfg_string[val]);
        cfg_string[val] = sal_strdup(valstr);
        if (cfg_string[val] == NULL) {
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
    default:
        return -1;
    }
    return 0;
}
