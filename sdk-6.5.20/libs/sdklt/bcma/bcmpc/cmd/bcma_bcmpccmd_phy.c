/*! \file bcma_bcmpccmd_phy.c
 *
 * CLI debug command for access to internal PHY registers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>
#include <shr/shr_pb.h>
#include <shr/shr_util.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmpc/bcmpc_lport.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmpc/bcma_bcmpc.h>
#include <bcma/bcmpc/bcma_bcmpccmd_phy.h>
#include <bcma/bcmpc/bcma_bcmpccmd_phydiag.h>

/*******************************************************************************
 * Private functions
 */

static int
bcmpccmd_phy_info(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int rv;
    int unit = cli->cmd_unit;
    bcmdrd_pbmp_t ppbmp, lpbmp;
    bcmpc_pport_t pport;
    bcmpc_lport_t lport;
    int lstart = -1, lend = -1;
    bcmpc_phy_info_t phy_info;
    int port_cnt = 0, err_cnt = 0;
    int pcount = 0, mod = 0, range_start = 0, lane = 0, is_first_lane;
    uint32_t lane_mask;
    char lane_range[32] = {0};
    char serdes_ver[3] = {0}, serdes_name[20] = {0}, serdes_info[32] = {0};
    shr_pb_t *pb;
    size_t len = 0;

    /* When using sim without physim, driver is unavailable */
    if (!bcmdrd_feature_is_real_hw(unit) &&
        !bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        cli_out("%sDriver unavailable. Run sim with -r option to enable "
                "PHYSIM\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg != NULL) {
        if (sal_strcasecmp(arg, "all") != 0) {
            rv = bcma_cli_parse_int_range(arg, &lstart, &lend);
            if (rv < 0) {
                return BCMA_CLI_CMD_BAD_ARG;
            }
        }
    }

    BCMDRD_PBMP_CLEAR(ppbmp);
    BCMDRD_PBMP_CLEAR(lpbmp);
    (void)bcmdrd_dev_valid_ports_get(unit, &ppbmp);

    /* Get the logical ports bitmap. */
    BCMDRD_PBMP_ITER(ppbmp, pport) {
        lport = bcmpc_pport_to_lport(unit, pport);
        if (lport == BCMPC_INVALID_LPORT) {
            continue;
        }
        BCMDRD_PBMP_PORT_ADD(lpbmp, lport);
    }

    if (BCMDRD_PBMP_IS_NULL(lpbmp)) {
        cli_out("%sNo ports are available.\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    pb = shr_pb_create();

    shr_pb_printf(pb, "port  mdio    serdes_info\n");
    /* Display the PHY information in logical port order. */
    BCMDRD_PBMP_ITER(lpbmp, lport) {
        if ((lstart != -1 && lport < (bcmpc_lport_t)lstart) ||
            (lend != -1 && lport > (bcmpc_lport_t)lend)) {
            continue;
        }
        port_cnt++;

        /* Create a reset point for skipping ports */
        shr_pb_mark(pb, -1);
        shr_pb_printf(pb, " %3d: ", lport);
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            shr_pb_printf(pb, "Fail to get the mapped physical port.\n");
            err_cnt++;
            continue;
        }
        rv = bcmpc_phy_info_get(unit, pport, &phy_info);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_PORT && (lstart == -1 || lstart != lend)) {
                /*
                 * Presume to be CPU or LPBK ports when the return value is
                 * SHR_E_PORT. Skip silently for these ports when ports are
                 * not specified or specified in range.
                 */
                shr_pb_reset(pb);
                port_cnt--;
                continue;
            }
            shr_pb_printf(pb, "Fail to get PHY information.\n");
            err_cnt++;
            continue;
        }
        mod = phy_info.num_lanes;
        is_first_lane = 1;
        lane_mask = phy_info.pbmp;
        lane = 0;
        pcount = 0;
        while (lane_mask) {
            if (lane_mask & 1) {
                if (is_first_lane) {
                    range_start = lane;
                    is_first_lane = 0;
                }
                pcount++;
            }
            lane_mask >>= 1;
            lane++;
        }
        if (pcount == 8) {
            sal_snprintf(lane_range, sizeof(lane_range), "%d", pcount);
        } else if (pcount == 4) {
            if (mod == 8) {
                sal_snprintf(lane_range, sizeof(lane_range), "%d-%d",
                             (range_start % mod), (range_start % mod) + 3);
            } else {
                sal_snprintf(lane_range, sizeof(lane_range), "%d", pcount);
            }
        } else if (pcount == 2) {
            sal_snprintf(lane_range, sizeof(lane_range), "%d-%d",
                         (range_start % mod), (range_start % mod) + 1);
        } else {
            sal_snprintf(lane_range, sizeof(lane_range), "%d",
                         (range_start) % mod);
        }
        sal_memset(serdes_ver, 0,  sizeof(serdes_ver));
        sal_memset(serdes_name, 0, sizeof(serdes_name));
        sal_memset(serdes_info, 0, sizeof(serdes_info));
        len = sal_strlen(phy_info.name);
        sal_strlcpy(serdes_ver, phy_info.name + len - 2, sizeof(serdes_ver));
        sal_strlcpy(serdes_name, phy_info.name, sizeof(serdes_name));
        sal_snprintf(serdes_info, sizeof(serdes_info),
                     "%s-%s", serdes_name, serdes_ver);
        sal_strupr(serdes_info);
        shr_pb_printf(pb, "0x%04x  %s/%02d/%s\n",
                      phy_info.addr, serdes_info, phy_info.pm_id, lane_range);
    }
    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);

    if (port_cnt == 0) {
        cli_out("%sNo available PHY information.\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    return (err_cnt == 0) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

static int
bcmpccmd_phy_raw(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int pm_id = 0;
    int unit = cli->cmd_unit;
    uint32_t reg, data;
    uint32_t *data_ptr = NULL;
    bcma_bcmpc_parse_desc_t parse_desc, *desc = &parse_desc;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    } else if (bcma_cli_parse_int(arg, &pm_id) < 0) {
        if (sal_strncasecmp(arg, "p", 1) != 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        while (*++arg != '\0') {
            if (sal_isdigit(*arg)) {
                if (bcma_cli_parse_int(arg, &pm_id) < 0) {
                    return BCMA_CLI_CMD_USAGE;
                }
                break;
            }
        }
    }

    bcma_bcmpc_parse_desc_t_init(desc);
    arg = BCMA_CLI_ARG_GET(args);

    /* <reg_name>|<reg_addr>[.<lane_index>][.<pll_index>] */
    if (bcma_bcmpc_parse_desc(arg, desc) < 0) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    /* Access through raw address */
    reg = desc->name32;
    /* Check if write data is specified */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        if (bcma_cli_parse_uint32(arg, &data) < 0) {
            return BCMA_CLI_CMD_USAGE;
        }
        data_ptr = &data;
    }
    if (SHR_FAILURE(bcma_bcmpc_phy_raw_reg(unit, pm_id, desc, reg, data_ptr))) {
        return BCMA_CLI_CMD_FAIL;
    }
    return BCMA_CLI_CMD_OK;
}

static int
bcmpccmd_phy_list(bcma_cli_t *cli, bcma_cli_args_t *args, bcmdrd_pbmp_t *pbmp)
{
    const char *arg;
    int unit = cli->cmd_unit;
    int lport = -1;
    uint32_t flags = BCMA_BCMPC_PHY_CMD_FLAG_SYM_LIST;
    const char *name = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    int pbmp_count = 0;

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    /* Look through our arguments */
    while (arg) {
        if (sal_strcasecmp(arg, "raw") == 0) {
            flags |= BCMA_BCMPC_PHY_CMD_FLAG_SYM_RAW;
            arg = BCMA_CLI_ARG_GET(args);
        } else if (name == NULL) {
            /* Symbol expression */
            name = arg;
            break;
        }
    }
    if (name == NULL) {
        bcma_bcmpc_parse_error("symbol", NULL);
        return BCMA_CLI_CMD_FAIL;
    }

    /* loop for each port */
    BCMDRD_PBMP_ITER(*pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        pbmp_count++;
        if (pbmp_count > 1) {
            /* "phy <port> list" support <port> only */
            return BCMA_CLI_CMD_USAGE;
        }
        if (SHR_FAILURE(bcma_bcmpc_phy_sym_list(unit, pport, name, flags))) {
            return BCMA_CLI_CMD_FAIL;
        }
    }
    return BCMA_CLI_CMD_OK;
}

static int
bcmpccmd_phy_acc(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int unit = cli->cmd_unit;
    int lport = -1;
    uint32_t flags = 0;
    uint32_t reg = 0, data = 0;
    uint32_t *data_ptr = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcma_bcmpc_parse_desc_t parse_desc, *desc = &parse_desc;
    bcmdrd_pbmp_t pbmp;
    int rv = 0;
    int ctoks_num = 0;
    bcma_cli_tokens_t *ctoks = NULL;

    arg = BCMA_CLI_ARG_GET(args);

    if (sal_strcasecmp(arg, "raw") == 0) {
        return bcmpccmd_phy_raw(cli, args);
    }

    /* parse <port>|<port_range> number */
    if (bcmdrd_pbmp_parse(arg, &pbmp) < 0 || BCMDRD_PBMP_IS_NULL(pbmp)) {
       return BCMA_CLI_CMD_USAGE;
    }

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* List PHY symbol(s) */
    if (sal_strcasecmp(arg, "list") == 0) {
        return bcmpccmd_phy_list(cli, args, &pbmp);
    }

    /*
     * PHY registers read/write
     */

    if (sal_strcasecmp(arg, "raw") == 0) {
        flags |= BCMA_BCMPC_PHY_CMD_FLAG_SYM_RAW;
        if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
            return BCMA_CLI_CMD_USAGE;
        }
    }

    bcma_bcmpc_parse_desc_t_init(desc);

    /* <reg_name>|<reg_addr>[.<lane_index>][.<pll_index>] */
    if (bcma_bcmpc_parse_desc(arg, desc) < 0) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    if (desc->name[0]) {
        /* Access through register symbol */
        if (BCMA_CLI_ARG_CNT(args) > 0) {
            /* Check for symbol write operation. */
            ctoks_num = BCMA_CLI_CONFIG_ARGS_CNT_MAX;
            ctoks = sal_alloc(ctoks_num * sizeof(bcma_cli_tokens_t),
                              "bcmaCliPhyCtoks");
            if (ctoks == NULL) {
                return BCMA_CLI_CMD_FAIL;
            }
            /* Parse remaining input arguments for field assignments */
            if ((arg = bcma_bcmpc_parse_args(args, ctoks, ctoks_num)) != NULL) {
                /* Error in argument i */
                bcma_bcmpc_parse_error("symbol", arg);
                sal_free(ctoks);
                return BCMA_CLI_CMD_FAIL;
            }
        }
    } else {
        /* Access through raw address */
        reg = desc->name32;
        /* Check if write data is specified */
        arg = BCMA_CLI_ARG_GET(args);
        if (arg) {
            if (bcma_cli_parse_uint32(arg, &data) < 0) {
                return BCMA_CLI_CMD_USAGE;
            }
            data_ptr = &data;
        }
    }

    /* loop for each port */
    BCMDRD_PBMP_ITER(pbmp, lport) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            continue;
        }
        if (desc->name[0]) {
            /* Access through register symbol */
            rv = bcma_bcmpc_phy_sym(unit, pport, desc, flags,
                                    ctoks, ctoks_num);
            if (SHR_FAILURE(rv)) {
                break;
            }
        } else {
            rv = bcma_bcmpc_phy_reg(unit, pport, desc, reg, data_ptr);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
    }
    if (ctoks) {
        sal_free(ctoks);
    }
    return SHR_SUCCESS(rv) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmpccmd_phy_cleanup(bcma_cli_t *cli)
{
    bcma_bcmpccmd_phydiag_cleanup(cli);
    return 0;
}

int
bcma_bcmpccmd_phy(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;
    int unit;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Diagnostics commands  */
    if (sal_strcasecmp(arg, "diag") == 0) {
        return bcma_bcmpccmd_phydiag(cli, args);
    }

    if (sal_strcasecmp(arg, "info") == 0) {
        return bcmpccmd_phy_info(cli, args);
    }

    /* PHY access commands */
    BCMA_CLI_ARG_PREV(args);
    return bcmpccmd_phy_acc(cli, args);
}
