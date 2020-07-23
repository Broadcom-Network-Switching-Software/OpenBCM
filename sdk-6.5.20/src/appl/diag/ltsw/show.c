/*! \file show.c
 *
 * LTSW show CLI command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(BCM_LTSW_SUPPORT)

#include <appl/diag/ltsw/cmdlist.h>
#include <appl/diag/system.h>
#include <shared/bsl.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/stat.h>
#include <soc/ltsw/config.h>

/******************************************************************************
 * Local definitions
 */

/******************************************************************************
 * Private functions
 */
static int
ltsw_show_counters_parse_args(int unit, args_t *a, bcm_pbmp_t *pbmp,
                              int *flags)
{
    int rv;
    int show_changed = 0;
    int show_same = 0;
    int show_zero = 0;
    int show_nzero = 0;
    int show_all = 0;
    int show_error = 0;
    char *subcmd = NULL;
    uint32_t port_type = 0;
    parse_table_t pt;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Changed", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &show_changed, 0);
    parse_table_add(&pt, "Same", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &show_same, 0);
    parse_table_add(&pt, "Z", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &show_zero, 0);
    parse_table_add(&pt, "Nz", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &show_nzero, 0);
    parse_table_add(&pt, "All", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &show_all, 0);
    parse_table_add(&pt, "ErDisc", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &show_error, 0);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return -1;
    }
    parse_arg_eq_done(&pt);

    if (show_changed) {
        *flags |= SHOW_CTR_CHANGED;
    }
    if (show_same) {
        *flags |= SHOW_CTR_SAME;
    }
    if (show_error) {
        *flags |= SHOW_CTR_ED;
    }
    if (show_zero) {
        *flags |= SHOW_CTR_Z;
    }
    if (show_nzero) {
        *flags |= SHOW_CTR_NZ;
    }
    if (show_all) {
        *flags |= (SHOW_CTR_CHANGED | SHOW_CTR_SAME |
                   SHOW_CTR_Z | SHOW_CTR_NZ);
    }
    if ((*flags & (SHOW_CTR_CHANGED | SHOW_CTR_SAME)) == 0) {
        *flags |= SHOW_CTR_CHANGED;
    }
    if ((*flags & (SHOW_CTR_Z | SHOW_CTR_NZ)) == 0) {
        *flags |= SHOW_CTR_NZ;
    }

    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        port_type = BCMI_LTSW_PORT_TYPE_PORT | BCMI_LTSW_PORT_TYPE_MGMT |
                    BCMI_LTSW_PORT_TYPE_CPU;
        rv = bcmi_ltsw_port_bitmap(unit, port_type, pbmp);
        if (rv < 0) {
            cli_out("ERROR: Failed to get the port bitmap.\n");
            return -1;
        }
    } else {
        rv = parse_pbmp(unit, subcmd, pbmp);
        if (rv < 0) {
            cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
            return -1;
        }
    }

    return 0;
}

static int
ltsw_clear_counters_parse_args(int unit, args_t *a, bcm_pbmp_t *pbmp)
{
    int rv;
    uint32_t port_type = 0;
    char *subcmd = NULL;

    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        port_type = BCMI_LTSW_PORT_TYPE_PORT | BCMI_LTSW_PORT_TYPE_MGMT |
                    BCMI_LTSW_PORT_TYPE_CPU;
        rv = bcmi_ltsw_port_bitmap(unit, port_type, pbmp);
        if (rv < 0) {
            cli_out("ERROR: Failed to get the port bitmap.\n");
            return -1;
        }
    } else {
        rv = parse_pbmp(unit, subcmd, pbmp);
        if (rv < 0) {
            cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
            return -1;
        }
    }

    return 0;
}

/*!
 * \brief Displays port map information.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval !CMD_OK            Command failed.
 */
static int
ltsw_show_portmap(int unit)
{
    bcm_pbmp_t pbmp;
    int port, rv;
    bcmi_ltsw_cosq_port_map_info_t portmap;

    cli_out("             pipe   logical  "
            "physical    mmu   UCQbase/Numq  MCQbase/Numq\n");

    (void)bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp);

    BCM_PBMP_ITER(pbmp, port) {
        sal_memset(&portmap, 0, sizeof(portmap));
        rv = bcmi_ltsw_cosq_port_map_info_get(unit, port, &portmap);
        if (rv == 0) {
            cli_out("  %8s   %3d     %3d       %3d      "
                    "%3d      %4d/%-4d     %4d/%-4d\n",
                    bcmi_ltsw_port_name(unit, port),
                    portmap.pipe, port, portmap.phy_port, portmap.mmu_port,
                    portmap.uc_base, portmap.num_uc_q,
                    portmap.mc_base, portmap.num_mc_q);
        }
    }

    return CMD_OK;
}

/*!
 * \brief Displays unit information.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval !CMD_OK            Command failed.
 */
static int
ltsw_show_unit(int unit)
{
    int u;

    for (u = 0; u < soc_ndev; u++) {
        if (!SOC_UNIT_VALID(SOC_NDEV_IDX2DEV(u))) {
            continue;
        }
        cli_out("Unit %d chip %s%s\n",
                SOC_NDEV_IDX2DEV(u),
                soc_dev_name(SOC_NDEV_IDX2DEV(u)),
                SOC_NDEV_IDX2DEV(u) == unit ? " (current)" : "");
    }
    return CMD_OK;
}

/*!
 * \brief Displays threads information.
 *
 * \param [in]  data          Callback data.
 * \param [in]  tid           Thread ID.
 * \param [in]  name          Thread name.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval !CMD_OK            Command failed.
 */
static int
ltsw_threads_info(void *data, int tid, char *name)
{
    cli_out("%-8d %s\n", tid, name);
    return CMD_OK;
}

/*******************************************************************************
 * Public Functions
 */

/*!
 * \brief Show CLI command handler.
 *
 * \param [in] unit           Unit number
 * \param [in] a              Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval !CMD_OK            Command failed.
 */
cmd_result_t
cmd_ltsw_show(int unit, args_t *a)
{
    int rv;
    char *subcmd = NULL;
    bcm_pbmp_t pbmp;
    int flags = 0;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

#if defined(BROADCOM_DEBUG)
    if (!sal_strcasecmp(subcmd, "feature") ||
        !sal_strcasecmp(subcmd, "features")) {
        ltsw_feature_t ft;
        char *ft_n[] = LTSW_FEAT_NAME_LIST;

        cli_out("Unit %d features:\n", unit);

        subcmd = ARG_GET(a);
        for (ft = 0; ft < LTSW_FT_COUNT; ft++) {
            if (ltsw_feature(unit, ft)) {
                cli_out("\t%s\n", ft_n[ft]);
            } else if (subcmd != NULL) {
                cli_out("\t[%s]\n", ft_n[ft]);
            }
        }
        return CMD_OK;
    }
#endif /* BROADCOM_DEBUG */

    if (parse_cmp("Counters", subcmd, 0)) {
        rv = ltsw_show_counters_parse_args(unit, a, &pbmp, &flags);
        if (rv < 0) {
            return CMD_FAIL;
        }

        rv = ltsw_show_counters_proc(unit, pbmp, flags);
        if (rv < 0) {
            return CMD_FAIL;
        }

        return CMD_OK;
    }

    if (parse_cmp("PortMAP", subcmd, 0)) {
        rv = ltsw_show_portmap(unit);
        if (rv < 0) {
            return CMD_FAIL;
        }

        return CMD_OK;
    }

    if (parse_cmp("unit", subcmd, 0) || parse_cmp("units", subcmd, 0)) {
        rv = ltsw_show_unit(unit);
        if (rv < 0) {
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (parse_cmp("CONFig", subcmd, 0)) {
        soc_ltsw_show_config(bcmi_ltsw_dev_dunit(unit));
        return CMD_OK;
    }

    if (parse_cmp("Threads", subcmd, 0)) {
        cli_out("TID      Name\n");
        sal_thread_traverse(ltsw_threads_info, INT_TO_PTR(unit));
        ltsw_threads_info(INT_TO_PTR(unit), sal_thread_id_get(), "*Main");
        return CMD_OK;
    }

    cli_out("%s: Error: Invalid option %s\n", ARG_CMD(a), subcmd);
    return CMD_FAIL;
}

cmd_result_t
cmd_ltsw_clear(int unit, args_t *a)
{
    int rv;
    char *subcmd = NULL;
    bcm_pbmp_t pbmp;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "counters") ||
        !sal_strcasecmp(subcmd, "c")) {

        rv = ltsw_clear_counters_parse_args(unit, a, &pbmp);
        if (rv < 0) {
            return CMD_FAIL;
        }
        rv = ltsw_clear_counters_proc(unit, pbmp);
        if (rv < 0) {
            return CMD_FAIL;
        }

        return CMD_OK;
    }

    cli_out("%s: Error: Invalid option %s\n", ARG_CMD(a), subcmd);
    return CMD_FAIL;
}

#endif /* BCM_LTSW_SUPPORT */
