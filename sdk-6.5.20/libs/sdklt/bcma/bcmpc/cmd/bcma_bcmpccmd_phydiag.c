/*! \file bcma_bcmpccmd_phytdiag.c
 *
 * CLI 'phydiag' command handler that deals with PHY diagnostics.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmpc/bcmpc_diag.h>
#include <bcmpc/bcmpc_lport.h>
#include <phymod/phymod_diagnostics.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmpc/bcma_bcmpc_diag.h>
#include <bcma/bcmpc/bcma_bcmpccmd_phydiag.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_APPL_PORTDIAG

#define PRBS_DIRECTION_F_RX   0x1
#define PRBS_DIRECTION_F_TX   0x2

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Parse the arguments for PRBS command.
 *
 * \param [in] cli CLI object.
 * \param [in] args Arguments from the command line.
 * \param [in] flags config flags.
 * \param [in] lbmp lane bitmap.
 * \param [out] cfg PRBS configuration from \c args.
 *
 * \retval BCMA_CLI_CMD_xxx.
 */
static int
prbs_cfg_parse(bcma_cli_t *cli, bcma_cli_args_t *args, uint32_t *flags,
               bcmdrd_pbmp_t *lbmp, bcmpc_phy_prbs_cfg_t *cfg)
{
    int rv = BCMA_CLI_CMD_OK;
    int poly = -1, invert = -1, dir = -1;
    char *str_lbmp = NULL;
    bcma_cli_parse_table_t pt;
    bcma_cli_parse_enum_t prbs_poly_enum[] = {
        { "P7",  BCMPC_PHY_PRBS_POLY_7  },
        { "0",   BCMPC_PHY_PRBS_POLY_7  },
        { "P15", BCMPC_PHY_PRBS_POLY_15 },
        { "1",   BCMPC_PHY_PRBS_POLY_15 },
        { "P23", BCMPC_PHY_PRBS_POLY_23 },
        { "2",   BCMPC_PHY_PRBS_POLY_23 },
        { "P31", BCMPC_PHY_PRBS_POLY_31 },
        { "3",   BCMPC_PHY_PRBS_POLY_31 },
        { "P9",  BCMPC_PHY_PRBS_POLY_9  },
        { "4",   BCMPC_PHY_PRBS_POLY_9  },
        { "P11", BCMPC_PHY_PRBS_POLY_11 },
        { "5",   BCMPC_PHY_PRBS_POLY_11 },
        { "P58", BCMPC_PHY_PRBS_POLY_58 },
        { "6",   BCMPC_PHY_PRBS_POLY_58 },
        { "P49", BCMPC_PHY_PRBS_POLY_49 },
        { "7",   BCMPC_PHY_PRBS_POLY_49 },
        { "P20", BCMPC_PHY_PRBS_POLY_20 },
        { "8",   BCMPC_PHY_PRBS_POLY_20 },
        { "P13", BCMPC_PHY_PRBS_POLY_13 },
        { "9",   BCMPC_PHY_PRBS_POLY_13 },
        { "P10", BCMPC_PHY_PRBS_POLY_10 },
        { "10",  BCMPC_PHY_PRBS_POLY_10 },
        { NULL,  0                      }
    };
    bcma_cli_parse_enum_t prbs_dir_enum[] = {
        { "TX",    PRBS_DIRECTION_F_TX                            },
        { "RX",    PRBS_DIRECTION_F_RX                            },
        { "TX,RX", PRBS_DIRECTION_F_TX | PRBS_DIRECTION_F_RX },
        { "RX,TX", PRBS_DIRECTION_F_TX | PRBS_DIRECTION_F_RX },
        { NULL,    0                                                   }
    };

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Polynomial", "enum", &poly, prbs_poly_enum);
    bcma_cli_parse_table_add(&pt, "Invert", "bool", &invert, NULL);
    bcma_cli_parse_table_add(&pt, "dir", "enum", &dir, prbs_dir_enum);
    bcma_cli_parse_table_add(&pt, "lane", "str", &str_lbmp, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%sInvalid option: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (poly < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    cfg->poly = poly;
    if (invert >= 0) {
        cfg->invert = invert;
    }
    if (dir >= 0) {
        *flags = dir;
    }
    if (str_lbmp) {
        if (bcmdrd_pbmp_parse(str_lbmp, lbmp) < 0) {
            rv = BCMA_CLI_CMD_BAD_ARG;
        }
        if (BCMDRD_PBMP_IS_NULL(*lbmp)) {
            rv = BCMA_CLI_CMD_BAD_ARG;
        }
    }

    bcma_cli_parse_table_done(&pt);

    return rv;
}

static int
prbs_get_parse(bcma_cli_t *cli, bcma_cli_args_t *args,
                     bcmdrd_pbmp_t *lbmp)
{
    int rv = BCMA_CLI_CMD_OK;
    char *str_lbmp = NULL;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "lane", "str", &str_lbmp, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%sInvalid option: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (str_lbmp) {
        if (bcmdrd_pbmp_parse(str_lbmp, lbmp) < 0) {
            rv = BCMA_CLI_CMD_BAD_ARG;
        }
        if (BCMDRD_PBMP_IS_NULL(*lbmp)) {
            rv = BCMA_CLI_CMD_BAD_ARG;
        }
    }

    bcma_cli_parse_table_done(&pt);

    return rv;
}

static int
prbs_set(int unit, bcmdrd_pbmp_t *pbmp, bcmdrd_pbmp_t *lbmp,
         uint32_t flags, bcmpc_phy_prbs_cfg_t *prbs_cfg)
{
    int rv;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    int lane_idx = -1;

    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        if (!BCMDRD_PBMP_IS_NULL(*lbmp)) {
            BCMDRD_PBMP_ITER(*lbmp, lane_idx) {
                rv = bcmpc_phy_prbs_config_set(unit, pport, lane_idx, flags,
                                               prbs_cfg);
                if (SHR_SUCCESS(rv)) {
                    rv = bcmpc_phy_prbs_enable_set(unit, pport, lane_idx,
                                                   flags, 1);
                    if (SHR_FAILURE(rv)) {
                        return BCMA_CLI_CMD_FAIL;
                    }
               }
            }
        } else {
            rv = bcmpc_phy_prbs_config_set(unit, pport, lane_idx, flags,
                                           prbs_cfg);
            if (SHR_SUCCESS(rv)) {
                rv = bcmpc_phy_prbs_enable_set(unit, pport, lane_idx,
                                               flags, 1);
                if (SHR_FAILURE(rv)) {
                    return BCMA_CLI_CMD_FAIL;
                }
            }
        }
    }

    return BCMA_CLI_CMD_OK;
}

static int
prbs_get(int unit, bcmdrd_pbmp_t *pbmp, bcmdrd_pbmp_t *lbmp, uint32_t flags)
{
    int rv;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_phy_prbs_status_t status;
    int lane_idx = -1;
    uint16_t idx = 0;

    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        if (!BCMDRD_PBMP_IS_NULL(*lbmp)) {
            BCMDRD_PBMP_ITER(*lbmp, idx) {
                rv = bcmpc_phy_prbs_status_get(unit, pport, idx, flags,
                                               &status);
                if (SHR_FAILURE(rv)) {
                    return BCMA_CLI_CMD_FAIL;
                }
                cli_out("%d.%d : PRBS ", lport, idx);
                if (status.prbs_lock) {
                    if (status.error_count == 0) {
                        cli_out("OK!");
                    } else {
                        cli_out("has %d error!", status.error_count);
                    }
                    if (status.prbs_lock_loss) {
                        cli_out("has been unlocked since last call");
                    }
                    cli_out("\n");
                } else {
                    cli_out("Failed!\n");
                }
            }
        } else {
            rv = bcmpc_phy_prbs_status_get(unit, pport, lane_idx, flags,
                                           &status);
            if (SHR_FAILURE(rv)) {
                return BCMA_CLI_CMD_FAIL;
            }
            cli_out("%d : PRBS ", lport);
            if (status.prbs_lock) {
                if (status.error_count == 0) {
                    cli_out("OK!");
                } else {
                    cli_out("has %d error!", status.error_count);
                }
                if (status.prbs_lock_loss) {
                    cli_out("has been unlocked since last call");
                }
                cli_out("\n");
            } else {
                cli_out("Failed!\n");
            }
        }
    }

    return BCMA_CLI_CMD_OK;
}

static int
prbs_clear(int unit, bcmdrd_pbmp_t *pbmp, bcmdrd_pbmp_t *lbmp, uint32_t flags)
{
    int rv;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    int lane_idx = -1;

    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        if (!BCMDRD_PBMP_IS_NULL(*lbmp)) {
            BCMDRD_PBMP_ITER(*lbmp, lane_idx) {
                rv = bcmpc_phy_prbs_enable_set(unit, pport, lane_idx, flags, 0);
                if (SHR_FAILURE(rv)) {
                    return BCMA_CLI_CMD_FAIL;
                }
           }
        } else {
            rv = bcmpc_phy_prbs_enable_set(unit, pport, lane_idx, flags, 0);
            if (SHR_FAILURE(rv)) {
                return BCMA_CLI_CMD_FAIL;
            }
        }
    }

    return BCMA_CLI_CMD_OK;
}

static int
eyescan_parse(bcma_cli_t *cli, bcma_cli_args_t *args,
              bcmpc_phy_eyescan_mode_t *mode)
{
    bcma_cli_parse_table_t pt;
    int estype = BCMPC_PHY_EYESCAN_MODE_FAST;
    bcma_cli_parse_enum_t eyescan_type_enum[] = {
        { "fast",    BCMPC_PHY_EYESCAN_MODE_FAST     },
        { "1",       BCMPC_PHY_EYESCAN_MODE_FAST     },
        { "berproj", BCMPC_PHY_EYESCAN_MODE_BER_PROJ },
        { "4",       BCMPC_PHY_EYESCAN_MODE_BER_PROJ },
        { NULL,      0                               }
    };

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "type", "enum", &estype, eyescan_type_enum);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%sInvalid option: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }
    bcma_cli_parse_table_done(&pt);

    *mode = estype;

    return BCMA_CLI_CMD_OK;
}

static int
eyescan_set(int unit, bcmdrd_pbmp_t *pbmp, uint32_t flags,
            bcmpc_phy_eyescan_mode_t *mode,
            bcmpc_phy_eyescan_options_t *opt)
{
    int rv;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_LPORT;

    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        rv = bcmpc_phy_eyescan_set(unit, pport, flags, mode, opt);
        if (SHR_FAILURE(rv)) {
            return BCMA_CLI_CMD_FAIL;
        }
    }

    return BCMA_CLI_CMD_OK;
}

static int
berproj_parse(bcma_cli_t *cli, bcma_cli_args_t *args,
              bcmpc_phy_ber_proj_options_t *option)
{
    uint32_t threshold = 0, sample_time = 0;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "HistogramErrorThreshold", "int",
                             (int *)&threshold, NULL);
    bcma_cli_parse_table_add(&pt, "SampleTime", "int",
                             (int *)&sample_time, NULL);
    /* The below entries are added for backward compatibility */
    bcma_cli_parse_table_add(&pt, "hist_errcnt_threshold", "int",
                             (int *)&threshold, NULL);
    bcma_cli_parse_table_add(&pt, "sample_time", "int",
                             (int *)&sample_time, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%sInvalid option: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }
    bcma_cli_parse_table_done(&pt);

    option->ber_proj_hist_errcnt_thresh = threshold;
    option->ber_proj_timeout = sample_time;

    return BCMA_CLI_CMD_OK;
}

static void
berproj_cleanup(bcmpc_phy_prbs_errcnt_t **prbs_errcnt, int arrsz)
{
    int idx;

    if (prbs_errcnt == NULL) {
        return;
    }

    for (idx = 0; idx < arrsz; idx++) {
        if (prbs_errcnt[idx]) {
            sal_free(prbs_errcnt[idx]);
        }
    }
    sal_free(prbs_errcnt);
}

static int
berproj_run(int unit, bcmdrd_pbmp_t *pbmp,
            bcmpc_phy_ber_proj_options_t *option)
{
    int rv;
    int idx, num_lanes, num_ports = 0;
    uint32_t time_remaining = 0;
    bcmpc_phy_ber_proj_mode_t mode;
    bcmpc_phy_prbs_errcnt_t **prbs_errcnt = NULL;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_LPORT;

    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        num_ports++;
    }

    /* Allocate memories to collect error count */
    prbs_errcnt = sal_alloc(sizeof(bcmpc_phy_prbs_errcnt_t *) * num_ports,
                            "bcmaBcmpccmdBerErrorCountArr");
    if (prbs_errcnt == NULL) {
        cli_out("%sFailed to allocate BER error count array\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    /* Initialize memory space */
    for (idx = 0; idx < num_ports; idx++) {
        prbs_errcnt[idx] = NULL;
    }

    idx = 0;
    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        num_lanes = 8;
        prbs_errcnt[idx] = sal_alloc(sizeof(bcmpc_phy_prbs_errcnt_t) * num_lanes,
                                     "bcmaBcmpccmdBerErrorCount");
        if (prbs_errcnt[idx] == NULL) {
            cli_out("%sFailed to allocate BER error count\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
            berproj_cleanup(prbs_errcnt, num_ports);
            return BCMA_CLI_CMD_FAIL;
        }
        sal_memset(prbs_errcnt[idx], 0,
                   sizeof(bcmpc_phy_prbs_errcnt_t) * num_lanes);
        idx++;
    }

    mode = BCMPC_PHY_BERPROJ_MODE_POST_FEC;
    if (option->ber_proj_timeout == 0) {
        option->ber_proj_timeout = 60;
    }
    if (option->ber_proj_hist_errcnt_thresh == 0) {
        /* Pre-stage, only needed when using optimized errcnt threshold. */
        cli_out("Getting optimized threshold for all the lanes...\n");
        idx = 0;
        BCMDRD_PBMP_ITER(*pbmp, lport) {
            pport = bcmpc_lport_to_pport(unit, lport);
            if (pport == BCMPC_INVALID_PPORT) {
                continue;
            }
            option->ber_proj_phase = BCMPC_PORT_PHY_BER_PROJ_P_PRE;
            option->ber_proj_fec_size = BCMPC_PORT_FEC_FRAME_SIZE_RS_544;
            option->ber_proj_prbs_errcnt = prbs_errcnt[idx];
            rv = bcmpc_phy_ber_proj(unit, pport, mode, option);
            if (SHR_FAILURE(rv)) {
                berproj_cleanup(prbs_errcnt, num_ports);
                return BCMA_CLI_CMD_FAIL;
            }
            idx++;
        }
        /* 99 is used to generate CEIL function of 5% of timeout */
        time_remaining = (option->ber_proj_timeout * 5 + 99)/ 100;
        sal_sleep(time_remaining);
    }

    /* Stage 1: Config PRBS checker. */
    idx = 0;
    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        option->ber_proj_phase = BCMPC_PORT_PHY_BER_PROJ_P_CONFIG;
        option->ber_proj_fec_size = BCMPC_PORT_FEC_FRAME_SIZE_RS_544;
        option->ber_proj_prbs_errcnt = prbs_errcnt[idx];
        rv = bcmpc_phy_ber_proj(unit, pport, mode, option);
        if (SHR_FAILURE(rv)) {
            berproj_cleanup(prbs_errcnt, num_ports);
            return BCMA_CLI_CMD_FAIL;
        }
        idx++;
    }

    /* Stage 2: Start Error accumulation. */
    idx = 0;
    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        option->ber_proj_phase = BCMPC_PORT_PHY_BER_PROJ_P_START;
        option->ber_proj_fec_size = BCMPC_PORT_FEC_FRAME_SIZE_RS_544;
        option->ber_proj_prbs_errcnt = prbs_errcnt[idx];
        rv = bcmpc_phy_ber_proj(unit, pport, mode, option);
        if (SHR_FAILURE(rv)) {
            berproj_cleanup(prbs_errcnt, num_ports);
            return BCMA_CLI_CMD_FAIL;
        }
        idx++;
    }

    /* Stage 3: Collect PRBS error count */
    time_remaining = option->ber_proj_timeout;
    while (time_remaining > 0) {
        if (time_remaining > 5) {
            sal_sleep(5);
            time_remaining = time_remaining - 5;
            cli_out(".");
        } else {
            sal_sleep(time_remaining);
            time_remaining = 0;
            cli_out(".\n");
        }
        idx = 0;
        BCMDRD_PBMP_ITER(*pbmp, lport) {
            pport = bcmpc_lport_to_pport(unit, lport);
            if (pport == BCMPC_INVALID_PPORT) {
                continue;
            }
            option->ber_proj_phase = BCMPC_PORT_PHY_BER_PROJ_P_COLLECT;
            option->ber_proj_prbs_errcnt = prbs_errcnt[idx];
            rv = bcmpc_phy_ber_proj(unit, pport, mode, option);
            if (SHR_FAILURE(rv)) {
                berproj_cleanup(prbs_errcnt, num_ports);
                return BCMA_CLI_CMD_FAIL;
            }
            idx++;
        }
    }

    /* Stage 4: BER Calculate. */
    idx = 0;
    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        option->ber_proj_phase = BCMPC_PORT_PHY_BER_PROJ_P_CALC;
        option->ber_proj_fec_size = BCMPC_PORT_FEC_FRAME_SIZE_RS_544;
        option->ber_proj_prbs_errcnt = prbs_errcnt[idx];
        rv = bcmpc_phy_ber_proj(unit, pport, mode, option);
        if (SHR_FAILURE(rv)) {
            berproj_cleanup(prbs_errcnt, num_ports);
            return BCMA_CLI_CMD_FAIL;
        }
        idx++;
    }

    berproj_cleanup(prbs_errcnt, num_ports);
    return BCMA_CLI_CMD_OK;
}

static int
bcmpccmd_phydiag_dsc_dump(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp,
                          bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    int rv = BCMA_CLI_CMD_OK;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    const char *type = BCMA_CLI_ARG_GET(args);

    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        rv = bcmpc_phy_pmd_info_dump(unit, pport, type);
        if (rv < 0) {
             return BCMA_CLI_CMD_FAIL;
        }
    }
    return rv;
}

static int
bcmpccmd_phydiag_prbs(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp,
                      bcma_cli_args_t *args)
{
    const char *arg;
    int unit = cli->cmd_unit;
    uint32_t dir_flags = 0;
    bcmpc_phy_prbs_cfg_t prbs_cfg = {0};
    bcmdrd_pbmp_t lbmp;
    int rv;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    BCMDRD_PBMP_CLEAR(lbmp);

    if (sal_strcasecmp(arg, "set") == 0) {
        rv = prbs_cfg_parse(cli, args, &dir_flags, &lbmp, &prbs_cfg);
        if (rv != BCMA_CLI_CMD_OK) {
            return rv;
        }
        return prbs_set(unit, pbmp, &lbmp, dir_flags, &prbs_cfg);
    } else if (sal_strcasecmp(arg, "get") == 0) {
        rv = prbs_get_parse(cli, args, &lbmp);
        if (rv != BCMA_CLI_CMD_OK) {
            return rv;
        }
        return prbs_get(unit, pbmp, &lbmp, dir_flags);
    } else if (sal_strcasecmp(arg, "clear") == 0) {
        rv = prbs_get_parse(cli, args, &lbmp);
        if (rv != BCMA_CLI_CMD_OK) {
            return rv;
        }
        return prbs_clear(unit, pbmp, &lbmp, dir_flags);
    }

    return BCMA_CLI_CMD_USAGE;
}

static int
bcmpccmd_phydiag_eyescan(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp,
                         bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bcmpc_phy_eyescan_mode_t mode;
    bcmpc_phy_eyescan_options_t opt;
    uint32_t flags = 0;

    rv = eyescan_parse(cli, args, &mode);
    if (rv != BCMA_CLI_CMD_OK) {
        return rv;
    }

    opt.sample_time = 1000;
    opt.horz_max = 31;
    opt.horz_min = -31;
    opt.hstep = 1;
    opt.vert_max = 31;
    opt.vert_min = -31;
    opt.vstep = 1;
    opt.ber_proj_scan_mode = 0;
    opt.ber_proj_timer_cnt = 96;
    opt.ber_proj_err_cnt = 8;

    SHR_BITSET(&flags, BCMPC_PHY_EYESCAN_F_ENABLE);

    SHR_BITSET(&flags, BCMPC_PHY_EYESCAN_F_PROCESS);

    SHR_BITSET(&flags, BCMPC_PHY_EYESCAN_F_DONE);

    return eyescan_set(unit, pbmp, flags, &mode, &opt);
}

static int
bcmpccmd_phydiag_berproj(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp,
                         bcma_cli_args_t *args)
{
    int rv;
    int unit = cli->cmd_unit;
    bcmpc_phy_ber_proj_options_t opt;

    sal_memset(&opt, 0, sizeof(opt));

    rv = berproj_parse(cli, args, &opt);
    if (rv != BCMA_CLI_CMD_OK) {
        return rv;
    }

    return berproj_run(unit, pbmp, &opt);
}

static int
bcmpccmd_phydiag_prbsstat(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp,
                          bcma_cli_args_t *args)
{
    int rv = SHR_E_PARAM;
    int unit = cli->cmd_unit;
    const char *arg;
    uint32_t interval_time = 0;

    bcma_bcmpc_diag_prbsstat_init(unit);

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        rv = bcma_bcmpc_diag_prbsstat_status(unit);
    } else if (bcma_cli_parse_cmp("STArt", arg, 0)) {
        if (BCMA_CLI_ARG_CNT(args) > 0) {
            bcma_cli_parse_table_t pt;

            bcma_cli_parse_table_init(cli, &pt);
            bcma_cli_parse_table_add(&pt, "Interval", "int",
                                     (int *)&interval_time, NULL);
            if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
                cli_out("%sInvalid option: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CUR(args));
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_FAIL;
            }
            bcma_cli_parse_table_done(&pt);
            rv = bcma_bcmpc_diag_prbsstat_start(unit, pbmp, interval_time);
        } else {
            rv = bcma_bcmpc_diag_prbsstat_start(unit, NULL, 0);
        }
    } else if (bcma_cli_parse_cmp("STOp", arg, 0)) {
        rv = bcma_bcmpc_diag_prbsstat_stop(unit);
    } else if (bcma_cli_parse_cmp("Counters", arg, 0)) {
        rv = bcma_bcmpc_diag_prbsstat_counter(unit, pbmp);
    } else if (bcma_cli_parse_cmp("Ber", arg, 0)) {
        rv = bcma_bcmpc_diag_prbsstat_ber(unit, pbmp);
    } else if (bcma_cli_parse_cmp("CLear", arg, 0)) {
        rv = bcma_bcmpc_diag_prbsstat_clear(unit, pbmp);
    }

    if (rv == SHR_E_PARAM) {
        return BCMA_CLI_CMD_USAGE;
    }
    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
bcmpccmd_phydiag_prbsstat_cleanup(bcma_cli_t *cli)
{
    bcma_bcmpc_diag_prbsstat_cleanup(-1);
    return 0;
}

static int
bcmpccmd_phydiag_fecstat(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp,
                         bcma_cli_args_t *args)
{
    int rv = SHR_E_PARAM;
    int unit = cli->cmd_unit;
    const char *arg;
    uint32_t interval_time = 0;

    bcma_bcmpc_diag_fecstat_init(unit);

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        rv = bcma_bcmpc_diag_fecstat_status(unit);
    } else if (bcma_cli_parse_cmp("STArt", arg, 0)) {
        if (BCMA_CLI_ARG_CNT(args) > 0) {
            bcma_cli_parse_table_t pt;

            bcma_cli_parse_table_init(cli, &pt);
            bcma_cli_parse_table_add(&pt, "Interval", "int",
                                     (int *)&interval_time, NULL);
            if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
                cli_out("%sInvalid option: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CUR(args));
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_FAIL;
            }
            bcma_cli_parse_table_done(&pt);
            rv = bcma_bcmpc_diag_fecstat_start(unit, pbmp, interval_time);
        } else {
            rv = bcma_bcmpc_diag_fecstat_start(unit, NULL, 0);
        }
    } else if (bcma_cli_parse_cmp("STOp", arg, 0)) {
        rv = bcma_bcmpc_diag_fecstat_stop(unit);
    } else if (bcma_cli_parse_cmp("Counters", arg, 0)) {
        rv = bcma_bcmpc_diag_fecstat_counter(unit, pbmp);
    } else if (bcma_cli_parse_cmp("Ber", arg, 0)) {
        rv = bcma_bcmpc_diag_fecstat_ber(unit, pbmp);
    } else if (bcma_cli_parse_cmp("CLear", arg, 0)) {
        rv = bcma_bcmpc_diag_fecstat_clear(unit, pbmp);
    }

    if (rv == SHR_E_PARAM) {
        return BCMA_CLI_CMD_USAGE;
    }
    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
bcmpccmd_phydiag_fecstat_cleanup(bcma_cli_t *cli)
{
    bcma_bcmpc_diag_fecstat_cleanup(-1);
    return 0;
}

static int
bcmpccmd_phydiag_linkcat(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp,
                         bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    int unit = cli->cmd_unit;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmdrd_pbmp_t lane_bmp;
    int mode_val = phymodlinkCAT_LPBK_MODE; /* Default mode */
    int lane;
    char *lane_str = NULL;
    bcma_cli_parse_table_t pt;
    bcma_cli_parse_enum_t linkcat_opts[] = {
        { "tx",   phymodlinkCAT_TX_MODE   },
        { "rx",   phymodlinkCAT_RX_MODE   },
        { "lpbk", phymodlinkCAT_LPBK_MODE },
        { NULL,   0                       }
    };

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "lane", "str", &lane_str, NULL);
    bcma_cli_parse_table_add(&pt, "mode", "enum", &mode_val, linkcat_opts);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%sInvalid option: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_CLI_ARG_CUR(args));
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_FAIL;
    }

    if (lane_str != NULL) {
        BCMDRD_PBMP_CLEAR(lane_bmp);
        if (bcmdrd_pbmp_parse(lane_str, &lane_bmp) < 0 ) {
            rv = BCMA_CLI_CMD_BAD_ARG;
        }
        if (BCMDRD_PBMP_IS_NULL(lane_bmp)) {
            rv = BCMA_CLI_CMD_BAD_ARG;
        }
    } else {
        rv = BCMA_CLI_CMD_USAGE;
    }
    bcma_cli_parse_table_done(&pt);
    if (rv != BCMA_CLI_CMD_OK) {
        return rv;
    }

    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        BCMDRD_PBMP_ITER(lane_bmp, lane) {
            bcmpc_phy_linkcat(unit, pport, lane, mode_val);
        }
    }
    return BCMA_CLI_CMD_OK;
}

static struct {
    const char *name;
    int (*func)(bcma_cli_t *cli, bcmdrd_pbmp_t *pbmp, bcma_cli_args_t *arg);
    int (*cleanup)(bcma_cli_t *cli);
} phydiag_cmds[] = {
    {"dsc",      bcmpccmd_phydiag_dsc_dump, NULL},
    {"prbs",     bcmpccmd_phydiag_prbs,     NULL},
    {"eyescan",  bcmpccmd_phydiag_eyescan,  NULL},
    {"berproj",  bcmpccmd_phydiag_berproj,  NULL},
    {"prbsstat", bcmpccmd_phydiag_prbsstat, bcmpccmd_phydiag_prbsstat_cleanup},
    {"fecstat",  bcmpccmd_phydiag_fecstat,  bcmpccmd_phydiag_fecstat_cleanup},
    {"linkcat",  bcmpccmd_phydiag_linkcat,  NULL}
};

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmpccmd_phydiag_cleanup(bcma_cli_t *cli)
{
    int idx;

    for (idx = 0; idx < COUNTOF(phydiag_cmds); idx++) {
        if (phydiag_cmds[idx].cleanup) {
            phydiag_cmds[idx].cleanup(cli);
        }
    }
    return 0;
}

int
bcma_bcmpccmd_phydiag(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    bcmdrd_pbmp_t pbmp;
    int idx;

    arg = BCMA_CLI_ARG_GET(args);
    if (bcmdrd_pbmp_parse(arg, &pbmp) < 0) {
       return BCMA_CLI_CMD_USAGE;
    }
    if (BCMDRD_PBMP_IS_NULL(pbmp)) {
       return BCMA_CLI_CMD_USAGE;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
       return BCMA_CLI_CMD_USAGE;
    }

    for (idx = 0; idx < COUNTOF(phydiag_cmds); idx++) {
        if (sal_strcasecmp(arg, phydiag_cmds[idx].name) == 0) {
            return phydiag_cmds[idx].func(cli, &pbmp, args);
        }
    }
    return BCMA_CLI_CMD_USAGE;
}
