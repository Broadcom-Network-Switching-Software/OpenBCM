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
#include <soc/shared/onesync.h>

extern int bcm_common_onesync_hostlog_start(int unit);
extern int bcm_common_onesync_hostlog_stop(int unit);
extern int bcm_common_onesync_hostlog_level_set(int unit, uint32 log_level);
extern int bcm_common_onesync_hostlog_level_get(int unit, uint32 *log_level);

#include <include/soc/uc.h>

#define _isprintable(_c) (((_c) > 32) && ((_c) < 127))

char cmd_onesync_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: BroadSync <option> [args...]\n"
#else
    "\n"
    "  ONESYNC HOSTLOG [ON|OFF]\n"
    "    Enable hostlog for onesync firmware\n"
    "  ONESYNC HOSTLOGLeVeL <log_level>\n"
    "    Hostlog level for onesync firmware\n"
#endif
    ;

cmd_result_t cmd_onesync(int unit, args_t *args)
{
    char *arg_string_p = NULL;

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    parse_pm_t log_level[] = {
        { "bs_info",            ONESYNC_HOSTLOG_LEVEL_BS_INFO       },
        { "bs_err",             ONESYNC_HOSTLOG_LEVEL_BS_ERR        },
        { "bs_verb",            ONESYNC_HOSTLOG_LEVEL_BS_VERB       },
        { "pktio_info",         ONESYNC_HOSTLOG_LEVEL_PKTIO_INFO    },
        { "pktio_err",          ONESYNC_HOSTLOG_LEVEL_PKTIO_ERR     },
        { "pktio_verb",         ONESYNC_HOSTLOG_LEVEL_PKTIO_VERB    },
        { "gpio_info",          ONESYNC_HOSTLOG_LEVEL_GPIO_INFO     },
        { "gpio_err",           ONESYNC_HOSTLOG_LEVEL_GPIO_ERR      },
        { "gpio_verb",          ONESYNC_HOSTLOG_LEVEL_GPIO_VERB     },
        { "phc",                ONESYNC_HOSTLOG_LEVEL_PHC_INFO      },
        { "config",             ONESYNC_HOSTLOG_LEVEL_CONFIG_INFO   },
        { 0, 0}
    };
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    /* Check to verify Broadsync Firmware is present */
    int uc;
    char *version = NULL;
    int bs_fw = 0;

    for(uc=0; uc<SOC_INFO(unit).num_ucs; uc++) {
        if (!soc_uc_in_reset(unit, uc)) {
            version = soc_uc_firmware_version(unit, uc);
            if (version) {
#ifdef NO_SAL_APPL
                if (strstr((char *)version,(char *)"ONESYNC"))
#else
                    if (sal_strcasestr((char *)version,(char *)"ONESYNC"))
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
        cli_out("Failed: OneSync firmware not loaded\n");
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
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
            arg_string_p = ARG_GET(args);

            if (arg_string_p == NULL) {
                return CMD_USAGE;
            }

            if (parse_cmp("ON", arg_string_p, 0)) {
                bcm_common_onesync_hostlog_start(unit);
            } else if (parse_cmp(arg_string_p, "OFF", 0)) {
                bcm_common_onesync_hostlog_stop(unit);
            } else {
                return CMD_USAGE;
            }
#endif
    } else if (parse_cmp("HOSTLOGLeVeL", arg_string_p, 0)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
            uint32 loglevel;

            arg_string_p = ARG_CUR(args);
            if (arg_string_p == NULL) {
                loglevel = 0;
                bcm_common_onesync_hostlog_level_get(unit, &loglevel);

                cli_out("Debugging is enabled for:\n\n");
                parse_mask_format(55, log_level, loglevel);
                cli_out("Debugging is disabled for:\n\n");
                parse_mask_format(55, log_level, ~loglevel);
                cli_out("\n");

                cli_out("OneSync Firmware: hostlogging level set to 0x%x\n", loglevel);
            } else {
                loglevel = 0;
                while (arg_string_p) {
                    /* extract debug mask */
                    if (arg_string_p[0] == '0' && arg_string_p[1] == 'x') {
                        loglevel = parse_integer(arg_string_p);
                    } else if (parse_mask(arg_string_p, log_level, &loglevel)) {
                        /* Not a mask arg.  Break from 'while' so other args are parsed */
                        break;
                    }
                    ARG_NEXT(args);
                    arg_string_p = ARG_CUR(args);
                }
                bcm_common_onesync_hostlog_level_set(unit, loglevel);
            }
#endif
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif

