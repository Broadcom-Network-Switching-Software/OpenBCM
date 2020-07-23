/*! \file ser.c
 *
 * LTSW SER command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(BCM_LTSW_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <shared/bsl.h>
#include <sal/sal_types.h>

#include <bcm_int/ltsw/ser.h>
/******************************************************************************
 * Private functions
 */
/*!
 * \brief Inject SER error to one PT.
 *
 * \param [in] unit Unit number
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 */
STATIC cmd_result_t
cmd_ltsw_ser_inject(int unit, args_t *arg)
{
    cmd_result_t rv = CMD_USAGE;
    parse_table_t pt;
    char *mem_name = "";
    int index, xor_bank = 0, inst, validation = 1;
    int rv_inject;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "PT", PQ_STRING,
                    "INVALIDm", &(mem_name), NULL);
    parse_table_add(&pt, "Index", PQ_INT,
                    (void *)0, &index, NULL);
    parse_table_add(&pt, "Inst", PQ_INT,
                    (void *)0, &(inst), NULL);
    parse_table_add(&pt, "Xor", PQ_BOOL,
                    (void *)0, &(xor_bank), NULL);
    parse_table_add(&pt, "Validation", PQ_BOOL,
                    (void *)1, &(validation), NULL);

    if(parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return rv;
    }
    if (index < 0) {
        cli_out("Invalid index selected");
        parse_arg_eq_done(&pt);
        return rv;
    }
    rv_inject = bcmi_ltsw_cmd_ser_inject(unit, mem_name, inst, index,
                                         xor_bank, validation);
    if (BCM_FAILURE(rv_inject)) {
        cli_out("Injection failed on %s at index %d inst %d, ErrNo: %d.\n",
                mem_name, index, inst, rv_inject);
        rv = CMD_FAIL;
    } else {
        cli_out("Error injected on %s at index %d inst %d\n",
                mem_name, index, inst);
        rv = CMD_OK;
    }
    parse_arg_eq_done(&pt);
    return rv;
}

/*!
 * \brief Print one SER log entry.
 *
 * \param [in] unit Unit number
 * \param [in] info SER log information.
 *
 * \retval NONE.
 */
static void
ltsw_ser_log_dump(int unit, bcm_switch_ser_log_info_t *info)
{
    int i = 0;

    cli_out("SER error detected at time stamp: 0x%x.\n", info->time);
    cli_out("%s name: %s.\n",
            (info->flags & BCM_SWITCH_SER_LOG_MEM) ? "Mem" : "Reg",
            info->name);
    if (info->block_type == bcmSwitchBlockTypeMmu) {
        cli_out("Block type of PT: MMU.\n");
        cli_out("PT index: %d.\n", info->index);
        cli_out("PT instance number: %d.\n", info->instance);
    } else if ((info->block_type == bcmSwitchBlockTypeEpipe) ||
               (info->block_type == bcmSwitchBlockTypeIpipe)) {
        cli_out("Block type of PT: %s.\n",
                (info->block_type == bcmSwitchBlockTypeEpipe) ? "Epipe" : "Ipipe");
        cli_out("PT index: %d.\n", info->index);
        cli_out("PT pipe number: %d.\n", info->pipe);
    } else if (info->block_type == bcmSwitchBlockTypePort) {
        cli_out("Block type of PT: Port.\n");
        cli_out("PT index: %d.\n", info->index);
        cli_out("PT port number: %d.\n", info->port);
    } else {
        cli_out("Block type of PT: Unknown.\n");
    }
    if (info->flags & BCM_SWITCH_SER_LOG_CORRECTED) {
        cli_out("SER error is corrected.\n");
    } else {
        cli_out("SER error is not corrected.\n");
    }
    switch(info->error_type) {
        case bcmSwitchErrorTypeParity:
            cli_out("Parity error.\n");
            break;
        case bcmSwitchErrorTypeEccDoubleBit:
            cli_out("ECC double bits error.\n");
            break;
        case bcmSwitchErrorTypeEccSingleBit:
            cli_out("ECC single bit error.\n");
            break;
        default :
            cli_out("Unknown error type.\n");
    }
    switch(info->correction_type) {
        case bcmSwitchCorrectTypeNoAction:
            cli_out("Correction type: None.\n");
            break;
        case bcmSwitchCorrectTypeEntryClear:
            cli_out("Correction type: Entry clear.\n");
            break;
        case bcmSwitchCorrectTypeCacheRestore:
        case bcmSwitchCorrectTypeSpecial:
            cli_out("Correction type: Cache restore.\n");
            break;
        default :
            cli_out("Unknown correction type.\n");
    }
    if (info->flags & BCM_SWITCH_SER_LOG_ENTRY) {
        cli_out("Erroneous entry data:\n");
        for (i = 0; i < BCM_MAX_MEM_WORDS; i++) {
            if ((i % 5) == 0) {
                cli_out("\t");
            }
            cli_out("%08x ", info->entry_data[i]);
            if ((i % 5) == 4) {
                cli_out("\n");
            }
        }
    }
}

/*!
 * \brief Inject SER error to one PT.
 *
 * \param [in] unit Unit number
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 */
STATIC cmd_result_t
cmd_ltsw_ser_log_print(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    int log_id;
    bcm_switch_ser_log_info_t log_info;

    if (ARG_CNT(arg) == 0) {
        log_id = 0;
        rv = bcm_ltsw_switch_ser_log_info_get(unit, log_id, &log_info);
        while (BCM_SUCCESS(rv)) {
            ltsw_ser_log_dump(unit, &log_info);
            log_id++;
            rv = bcm_ltsw_switch_ser_log_info_get(unit, log_id, &log_info);
        }
    } else {
        parse_table_init(unit, &pt);

        parse_table_add(&pt, "ID", PQ_INT, (void *)0, &log_id, NULL);

        if(parse_arg_eq(arg, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        parse_arg_eq_done(&pt);

        if (log_id < 0) {
            cli_out("Invalid id selected!\n");
            return CMD_USAGE;
        }
        rv = bcm_ltsw_switch_ser_log_info_get(unit, log_id, &log_info);
        if (BCM_FAILURE(rv)) {
            cli_out("Fail to get SER log according to ID [%d]!\n", log_id);
            return CMD_USAGE;
        }
        ltsw_ser_log_dump(unit, &log_info);
    }
    return CMD_OK;
}

/*!
 * \brief SER control configuration.
 *
 * \param [in] unit Unit number
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 */
STATIC cmd_result_t
cmd_ltsw_ser_config(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    bcmi_ltsw_ser_control_t ser_ctrl;
    int i, sqi = 0;
    uint32 flags = 0;

    rv = bcmi_ltsw_cmd_ser_control_get(unit, &ser_ctrl);
    if (BCM_FAILURE(rv)) {
        cli_out("Fail to get SER control\n");
        return CMD_FAIL;
    }

    if (ARG_CNT(arg) == 0) {
        cli_out("Squash duplicated interval: %u ms\n",
                ser_ctrl.squash_duplicated_interval);
    } else {
        parse_table_init(unit, &pt);

        parse_table_add(&pt, "SquashDuplicatedInterval", PQ_INT, (void *)0,
                        &sqi, NULL);

        if(parse_arg_eq(arg, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(arg), ARG_CUR(arg));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                flags |= (1 << i);
            }
        }

        if (flags & 0x1) {
            ser_ctrl.squash_duplicated_interval = sqi;
        }

        parse_arg_eq_done(&pt);

        rv = bcmi_ltsw_cmd_ser_control_set(unit, &ser_ctrl);
        if (BCM_FAILURE(rv)) {
            cli_out("Fail to set SER control\n");
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

/******************************************************************************
 * Public Functions
 */
/*!
 * \brief SER cmd handler.
 *
 * \param [in] unit Unit number
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 */
cmd_result_t
cmd_ltsw_ser(int unit, args_t *arg)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }
    if (!sal_strcasecmp(subcmd, "inject")) {
        return cmd_ltsw_ser_inject(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "log")) {
        return cmd_ltsw_ser_log_print(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "config")) {
        return cmd_ltsw_ser_config(unit, arg);
    } else {
        return CMD_NOTIMPL;
    }
    return CMD_OK;
}

/*!
 * \brief SramSCAN cmd handler.
 *
 * \param [in] unit Unit number
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 */
cmd_result_t
cmd_ltsw_sram_scan(int unit, args_t *arg)
{
    parse_table_t pt;
    int interval = 0, rate = 0, enable = 0, rv = 0;
    int interval_cur = 0, rate_cur = 0;
    char *c = NULL;

    rv = bcmi_ltsw_cmd_ser_scan_get(unit, 0, &rate_cur, &interval_cur, &enable);

    if (BCM_FAILURE(rv)) {
        cli_out("soc_sram_scan_running %d: ERROR: %s\n",
                unit, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    if (ARG_CNT(arg) == 0) {
        cli_out("%s: %s on unit %d\n",
                ARG_CMD(arg), enable ? "Running" : "Not running", unit);
        cli_out("%s:   Interval: %d usec\n",
                ARG_CMD(arg), interval_cur);
        cli_out("%s:   Rate: %d\n",
                ARG_CMD(arg), rate_cur);
        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_INT | PQ_DFL, 0, &interval, 0);
    parse_table_add(&pt, "Rate", PQ_INT | PQ_DFL, 0, &rate, 0);

    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    enable = 1;
    if ((c = ARG_GET(arg)) != NULL) {
        if (sal_strcasecmp(c, "on") == 0) {
            enable = 1;
        } else if (sal_strcasecmp(c, "off") == 0 ||
                   interval == 0) {
            enable = 0;
        } else {
            return CMD_USAGE;
        }
    }
    if (interval == 0) {
        interval = interval_cur;
    }
    if (rate == 0) {
        rate = rate_cur;
    }
    rv = bcmi_ltsw_cmd_ser_scan_config(unit, 0, rate, interval, enable);
    if (BCM_FAILURE(rv)) {
        cli_out("ERR: configuring value of Rate or Interval is invalid!!!\n");
        cli_out("Rate: Min [1,000,000us] Max [1,000,000,000us].\n"
                "Interval: Min [0x400] Max [0x7fffffff]\n");
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
}

/*!
 * \brief MemSCAN cmd handler.
 *
 * \param [in] unit Unit number
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 */
cmd_result_t
cmd_ltsw_mem_scan(int unit, args_t *arg)
{
    int interval = 0, rate = 0, enable = 0, rv = 0;
    char *c = NULL;

    rv = bcmi_ltsw_cmd_ser_scan_get(unit, 1, &rate, &interval, &enable);

    if (BCM_FAILURE(rv)) {
        cli_out("soc_tcam_scan_running %d: ERROR: %s\n",
                unit, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    if (ARG_CNT(arg) == 0) {
        cli_out("%s: %s on unit %d\n",
                ARG_CMD(arg), enable ? "Running" : "Not running", unit);
        return CMD_OK;
    }
    enable = 1;
    if ((c = ARG_GET(arg)) != NULL) {
        if (sal_strcasecmp(c, "on") == 0) {
            enable = 1;
        } else if (sal_strcasecmp(c, "off") == 0 ||
                   interval == 0) {
            enable = 0;
        } else {
            return CMD_USAGE;
        }
    }
    rv = bcmi_ltsw_cmd_ser_scan_config(unit, 1, rate, interval, enable);
    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
}

#endif /* BCM_LTSW_SUPPORT */
