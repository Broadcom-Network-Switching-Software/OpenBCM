/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#include <bcm/error.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <appl/diag/system.h>

#include <appl/diag/parse.h>

#ifdef NO_SAL_APPL
#include <string.h>
#else
#include <sal/appl/sal.h>
#endif

#if defined(BCM_CMICM_SUPPORT)
#include <soc/shared/knetsync.h>

extern int bcm_common_knetsync_hostlog_start(int unit);
extern int bcm_common_knetsync_hostlog_stop(int unit);
extern int bcm_common_knetsync_hostlog_level_set(int unit, uint32 log_level);
extern int bcm_common_knetsync_hostlog_level_get(int unit, uint32 *log_level);

#include <include/soc/uc.h>

#define _isprintable(_c) (((_c) > 32) && ((_c) < 127))

char cmd_knetsync_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: KNETSYNC <option> [args...]\n"
#else
    "\n"
    "  KNETSYNC HOSTLOG [ON|OFF]\n"
    "    Enable hostlog for knetsync firmware\n"
    "  KNETSYNC HOSTLOGLeVeL <log_level>\n"
    "    Hostlog level for knetsync firmware\n"
#endif
    ;

cmd_result_t cmd_knetsync(int unit, args_t *args)
{
    char *arg_string_p = NULL;

#if defined(BCM_ESW_SUPPORT) 
    /* Check to verify KnetSync Firmware is present */
    int uc;
    char *version = NULL;
    int bs_fw = 0;

    for(uc=0; uc<SOC_INFO(unit).num_ucs; uc++) {
        if (!soc_uc_in_reset(unit, uc)) {
            version = soc_uc_firmware_version(unit, uc);
            if (version) {
#ifdef NO_SAL_APPL
                if (strstr((char *)version,(char *)"KNETSYNC"))
#else
                    if (sal_strcasestr((char *)version,(char *)"KNETSYNC"))
#endif
                    {
                        bs_fw = 1;
                    }

                soc_cm_sfree(unit, version);
                if (1 == bs_fw) {
                    break;
                }
            }
        }
    }
    if (bs_fw == 0) {
        cli_out("Failed: KnetSync firmware not loaded\n");
        return CMD_FAIL;
    }
#endif

    arg_string_p = ARG_GET(args);

    if (arg_string_p == NULL) {
        return CMD_USAGE;
    }

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return CMD_FAIL;
    }


    if (parse_cmp("HOSTLOG", arg_string_p, 0)) {
#if defined(BCM_ESW_SUPPORT)
            arg_string_p = ARG_GET(args);

            if (arg_string_p == NULL) {
                return CMD_USAGE;
            }

            if (parse_cmp("ON", arg_string_p, 0)) {
                bcm_common_knetsync_hostlog_start(unit);
            } else if (parse_cmp(arg_string_p, "OFF", 0)) {
                bcm_common_knetsync_hostlog_stop(unit);
            } else {
                return CMD_USAGE;
            }
#endif
    } else if (parse_cmp("HOSTLOGLeVeL", arg_string_p, 0)) {
#if defined(BCM_ESW_SUPPORT)
            uint32 loglevel;
            arg_string_p = ARG_GET(args);

            if (arg_string_p == NULL) {
                loglevel = 0;
                bcm_common_knetsync_hostlog_level_get(unit, &loglevel);

                cli_out("KnetSync Firmware: hostlogging level set to 0x%x\n", loglevel);
            } else {
                loglevel = parse_integer(arg_string_p);
                bcm_common_knetsync_hostlog_level_set(unit, loglevel);
            }
#endif
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif
