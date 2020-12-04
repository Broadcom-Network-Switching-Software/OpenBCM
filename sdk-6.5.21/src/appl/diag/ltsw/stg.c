/*! \file stg.c
 *
 * LTSW STG CLI command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <bcm/stg.h>
#include <bcm/error.h>
#include <appl/diag/ltsw/cmdlist.h>

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Show VLAN list in a specified STG.
 *
 * \param [in] unit           Unit number
 * \param [in] stg            STG ID.
 *
 * \retval BCM_E_NONE         No errors.
 * \retval !BCM_E_NONE        Failure.
 */
static int
do_show_stg_vlans(int unit, bcm_stg_t stg)
{
    bcm_vlan_t *list;
    int count, i, rv;
    int first = 1;

    if ((rv = bcm_stg_vlan_list(unit, stg, &list, &count)) < 0) {
        cli_out("Error: listing STG %d: %s\n", stg, bcm_errmsg(rv));
        return rv;
    }

    cli_out("STG %d: contains %d VLAN%s%s",
            stg, count, (count == 1) ? "" : "s", (count == 0) ? "" : " (");

    for (i = 0; i < count; i++) {
        int span;

        cli_out("%s%d", first ? "" : ",", list[i]);
        first = 0;

        for (span = 1; i < count - 1 && list[i + 1] == list[i] + 1; span++) {
            i++;
        }

        if (span > 1) {
            cli_out("-%d", list[i]);
        }
    }

    cli_out("%s\n", (count == 0) ? "" : ")");

    if ((rv = bcm_stg_vlan_list_destroy(unit, list, count)) < 0) {
        cli_out("Error: Destroying STG %d: %s\n", stg, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*!
 * \brief Show span tree state in a specified STG.
 *
 * \param [in] unit           Unit number
 * \param [in] stg            STG ID.
 *
 * \retval BCM_E_NONE         No errors.
 * \retval !BCM_E_NONE        Failure.
 */
static int
do_show_stg_stp(int unit, bcm_stg_t stg)
{
    bcm_pbmp_t pbmps[BCM_STG_STP_COUNT];
    bcm_port_config_t pcfg;
    char buf[FORMAT_PBMP_MAX];
    int state, rv;
    int port;

    sal_memset(pbmps, 0, sizeof(bcm_pbmp_t) * BCM_STG_STP_COUNT);

    bcm_port_config_t_init(&pcfg);

    if ((rv = bcm_port_config_get(unit, &pcfg)) < 0) {
        cli_out("Error: Get port config: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_PBMP_ITER(pcfg.port, port) {
        if ((rv = bcm_stg_stp_get(unit, stg, port, &state)) < 0) {
            cli_out("Error: Geting STP for PORT %d in STG %d: %s\n",
                    port, stg, bcm_errmsg(rv));
            return rv;
        }

        BCM_PBMP_PORT_ADD(pbmps[state], port);
    }

    /* In current chips, LISTEN is not distinguished from BLOCK. */
    for (state = 0; state < BCM_STG_STP_COUNT; state++) {
        if (!BCM_PBMP_IS_NULL(pbmps[state])) {
            format_bcm_pbmp(unit, buf, sizeof(buf), pbmps[state]);
            cli_out("  %7s: %s\n", FORWARD_MODE(state), buf);
        }
    }

    return BCM_E_NONE;
}

/*******************************************************************************
 * Public Functions
 */

/*!
 * \brief STG CLI command handler.
 *
 * \param [in] unit           Unit number
 * \param [in] a              Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval !CMD_OK            Command failed.
 */
cmd_result_t
cmd_ltsw_stg(int unit, args_t *a)
{
    char *subcmd, *c;
    int rv = BCM_E_NONE;
    bcm_stg_t stg = BCM_STG_INVALID;
    bcm_vlan_t vid;
    int state;
    bcm_pbmp_t pbmp;
    bcm_port_config_t pcfg;
    int port;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {

        if ((c = ARG_GET(a)) != NULL) {
            stg = parse_integer(c);
        }

        if (stg == BCM_STG_INVALID) {
            rv = bcm_stg_create(unit, &stg);
            cli_out("Created spanning tree group %d\n", stg);
        } else {
            rv = bcm_stg_create_id(unit, stg);
        }

        if (rv < 0) {
            goto bcm_err;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "destroy") == 0) {

        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        stg = parse_integer(c);

        if ((rv = bcm_stg_destroy(unit, stg)) < 0) {
            goto bcm_err;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {

        bcm_stg_t *list;
        int count, i;

        if ((c = ARG_GET(a)) != NULL) {
            stg = parse_integer(c);
        }

        if (stg != BCM_STG_INVALID) {
            if ((rv = do_show_stg_vlans(unit, stg)) < 0) {
                goto bcm_err;
            } else {
                return CMD_OK;
            }
        }

        if ((rv = bcm_stg_list(unit, &list, &count)) < 0) {
            goto bcm_err;
        }

        /* Force print STG 0 */
        (void) do_show_stg_vlans(unit, 0);
        (void) do_show_stg_stp(unit, 0);

        for (i = 0; i < count; i++) {
            if ((rv = do_show_stg_vlans(unit, list[i])) < 0) {
                (void) bcm_stg_list_destroy(unit, list, count);
                goto bcm_err;
            }

            if ((rv = do_show_stg_stp(unit, list[i])) < 0) {
                (void) bcm_stg_list_destroy(unit, list, count);
                goto bcm_err;
            }
        }

        if ((rv = bcm_stg_list_destroy(unit, list, count)) < 0) {
            goto bcm_err;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {

        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        stg = parse_integer(c);

        while ((c = ARG_GET(a)) != NULL) {
            vid = parse_integer(c);

            if ((rv = bcm_stg_vlan_add(unit, stg, vid)) < 0) {
                goto bcm_err;
            }
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "remove") == 0) {

        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        stg = parse_integer(c);

        while ((c = ARG_GET(a)) != NULL) {
            vid = parse_integer(c);

            if ((rv = bcm_stg_vlan_remove(unit, stg, vid)) < 0) {
                goto bcm_err;
            }
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "stp") == 0) {

        if ((c = ARG_GET(a)) == NULL) {
            bcm_stg_t *list;
            int count, i;

            if ((rv = bcm_stg_list(unit, &list, &count)) < 0) {
                goto bcm_err;
            }

            for (i = 0; i < count; i++) {
                cli_out("STG %d:\n", list[i]);

                if ((rv = do_show_stg_stp(unit, list[i])) < 0) {
                    (void) bcm_stg_list_destroy(unit, list, count);
                    goto bcm_err;
                }
            }

            if ((rv = bcm_stg_list_destroy(unit, list, count)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }

        stg = parse_integer(c);

        if ((c = ARG_GET(a)) == NULL) {
            cli_out("STG %d:\n", stg);

            if ((rv = do_show_stg_stp(unit, stg)) < 0) {
                goto bcm_err;
            }

            return CMD_OK;
        }

        if (parse_pbmp(unit, c, &pbmp) < 0) {
             goto bcm_err;
        }

        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        for (state = 0; state < BCM_STG_STP_COUNT; state++) {
            if (parse_cmp(forward_mode[state], c, '\0')) {
                break;
            }
        }

        if (state == BCM_STG_STP_COUNT) {
            return CMD_USAGE;
        }

        bcm_port_config_t_init(&pcfg);

        if ((rv = bcm_port_config_get(unit, &pcfg)) < 0) {
            goto bcm_err;
        }

        BCM_PBMP_AND(pbmp, pcfg.port);

        BCM_PBMP_ITER(pbmp, port) {
            if ((rv = bcm_stg_stp_set(unit, stg, port, state)) < 0) {
                goto bcm_err;
            }
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "default") == 0) {

        if ((c = ARG_GET(a)) != NULL) {
            stg = parse_integer(c);
        }

        if (stg == BCM_STG_INVALID) {
            if ((rv = bcm_stg_default_get(unit, &stg)) < 0) {
                goto bcm_err;
            }

            cli_out("Default STG is %d\n", stg);
        } else {
            if ((rv = bcm_stg_default_set(unit, stg)) < 0) {
                goto bcm_err;
            }

            cli_out("Default STG set to %d\n", stg);
        }

        return CMD_OK;
    }

    return CMD_USAGE;

bcm_err:
    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
    return CMD_FAIL;
}
