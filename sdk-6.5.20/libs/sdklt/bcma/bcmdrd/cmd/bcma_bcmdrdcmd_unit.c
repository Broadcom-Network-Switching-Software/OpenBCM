/*! \file bcma_bcmdrdcmd_unit.c
 *
 * CLI 'unit' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_pb.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/bcmdrd/bcma_bcmdrd.h>
#include <bcma/bcmdrd/bcma_bcmdrdcmd_unit.h>

#define UNIT_INFO_ID            (1 << 0)
#define UNIT_INFO_RESOURCE      (1 << 1)
#define UNIT_INFO_DEVICE        (1 << 2)
#define UNIT_INFO_CHIP          (1 << 3)
#define UNIT_INFO_BLOCK         (1 << 4)
#define UNIT_INFO_STATUS        (1 << 5)

/*! Local environment variable name for execution result. */
#define BCMA_UNIT_RESULT_VAR "0"

int
bcma_bcmdrdcmd_unit(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    int rv;
    int u, b, idx;
    char tmp[BCMDRD_PBMP_WORD_MAX * 16];
    uint32_t unit_info;
    const char *arg;
    const char *name;
    uint32_t val;
    shr_pb_t *pb;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    unit_info = 0;
    while (1) {
        arg = BCMA_CLI_ARG_GET(args);
        if (!arg) {
            break;
        }
        if (sal_strcmp(arg, "*") == 0) {
            /* Specify all units */
            unit = -1;
        } else if (bcma_cli_parse_cmp("all", arg, 0)) {
            /* Specify all info types */
            unit_info = (uint32_t)-1;
        } else if (bcma_cli_parse_cmp("id", arg, 0)) {
            unit_info |= UNIT_INFO_ID;
        } else if (bcma_cli_parse_cmp("device", arg, 0)) {
            unit_info |= UNIT_INFO_DEVICE;
        } else if (bcma_cli_parse_cmp("resource", arg, 0)) {
            unit_info |= UNIT_INFO_RESOURCE;
        } else if (bcma_cli_parse_cmp("chip", arg, 0)) {
            unit_info |= UNIT_INFO_CHIP;
        } else if (bcma_cli_parse_cmp("block", arg, 0)) {
            unit_info |= UNIT_INFO_BLOCK;
        } else if (bcma_cli_parse_cmp("stat", arg, 0)) {
            unit_info |= UNIT_INFO_STATUS;
        } else {
            cli_out("%sInvalid display option: %s.\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return BCMA_CLI_CMD_BAD_ARG;
        }
    }
    if (unit_info == 0) {
        unit_info = UNIT_INFO_ID;
    }

    pb = shr_pb_create();

    for (u = 0; u < BCMDRD_CONFIG_MAX_UNITS; u++) {
        if (bcmdrd_dev_exists(u) && (unit == -1 || unit == u)) {
            bcmdrd_dev_id_t id;
            bcmdrd_hal_io_t io;
            uint32_t pipe_map;
            bool mem_mapped = false;
            bcmdrd_pbmp_t valid_pbmp;
            const bcmdrd_pbmp_t *vpbmp, *cpbmp;
            const bcmdrd_chip_info_t *cinfo = bcmdrd_dev_chip_info_get(u);

            assert(cinfo);

            if (sal_strlen(shr_pb_str(pb)) > 0) {
                shr_pb_printf(pb, ",");
            }
            shr_pb_printf(pb, "%d", u);

            rv = bcmdrd_dev_id_get(u, &id);
            if (SHR_FAILURE(rv)) {
                cli_out("%sUnit %d: Unable to get device ID (%d).\n",
                        BCMA_CLI_CONFIG_ERROR_STR, u, rv);
                continue;
            }

            rv = bcmdrd_dev_hal_io_get(u, &io);
            if (SHR_FAILURE(rv)) {
                cli_out("%sUnit %d: Unable to get device I/O data (%d).\n",
                        BCMA_CLI_CONFIG_ERROR_STR, u, rv);
                continue;
            }

            rv = bcmdrd_dev_valid_ports_get(u, &valid_pbmp);
            if (SHR_FAILURE(rv)) {
                cli_out("%sUnit %d: Unable to get device port data (%d).\n",
                        BCMA_CLI_CONFIG_ERROR_STR, u, rv);
                continue;
            }

            rv = bcmdrd_dev_valid_pipes_get(u, &pipe_map);
            if (SHR_FAILURE(rv)) {
                cli_out("%sUnit %d: Unable to get device pipe data (%d).\n",
                        BCMA_CLI_CONFIG_ERROR_STR, u, rv);
                continue;
            }

            vpbmp = &valid_pbmp;
            cpbmp = &cinfo->valid_pbmps[BCMDRD_PND_PHYS];

            cli_out("unit %d:\n", u);

            if (unit_info & UNIT_INFO_ID) {
                cli_out("  Device: %s [%04x:%04x:%02x]\n", bcmdrd_dev_name(u),
                        id.vendor_id, id.device_id, id.revision);
            }

            if (unit_info & UNIT_INFO_RESOURCE) {
                cli_out("  Physical address:");
                for (idx = 0; idx < BCMDRD_NUM_PHYS_ADDR; idx++) {
                    if (io.phys_addr[idx]) {
                        mem_mapped = true;
                        cli_out(" %d:0x%"PRIx64, idx, io.phys_addr[idx]);
                    }
                }
                cli_out("%s\n", mem_mapped ? "" : " (none)");
            }

            if (unit_info & UNIT_INFO_DEVICE) {
                bcma_bcmdrd_port_bitmap(tmp, sizeof(tmp), vpbmp, cpbmp);
                cli_out("  Valid ports: %s\n", tmp);
                cli_out("  Valid pipes: 0x%x\n", pipe_map);
            }

            if (unit_info & UNIT_INFO_CHIP) {
                bcma_bcmdrd_port_bitmap(tmp, sizeof(tmp), cpbmp, cpbmp);
                cli_out("  Chip ports:  %s\n", tmp);
                cli_out("  Chip flags:  0x%"PRIx32"\n", cinfo->flags);
                cli_out("  Chip params:\n");
                name = NULL;
                while ((name = bcmdrd_dev_param_next(unit, name)) != NULL) {
                    if (bcmdrd_dev_param_get(unit, name, &val) == 0) {
                        cli_out("    %s = %"PRIu32" (0x%"PRIx32")\n",
                                name, val, val);
                    }
                }
            }

            if (unit_info & UNIT_INFO_STATUS) {
                bool tainted = false;
                bcmdrd_dev_tainted_get(u, &tainted);
                cli_out("  Tainted: %s\n", tainted ? "True" : "False");
            }

            if (unit_info & UNIT_INFO_BLOCK) {
                cli_out("  Block  Name           Ports\n");
                for (b = 0; b < cinfo->nblocks; b++) {
                    const bcmdrd_block_t *blkp = cinfo->blocks + b;
                    if (bcma_bcmdrd_block_name(u, blkp->blknum, tmp,
                                               sizeof(tmp)) == tmp) {
                        cli_out("  %-3d    %-14s ", blkp->blknum, tmp);
                        bcma_bcmdrd_port_bitmap(tmp, sizeof(tmp),
                                                &blkp->pbmps, cpbmp);
                        cli_out("%s\n", tmp);
                    }
                }
            }
        }
    }

    bcma_cli_var_set(cli, BCMA_UNIT_RESULT_VAR, (char *)shr_pb_str(pb), TRUE);

    shr_pb_destroy(pb);

    return BCMA_CLI_CMD_OK;
}
