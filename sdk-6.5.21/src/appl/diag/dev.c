/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * device CLI commands
 */
#include <soc/drv.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <appl/diag/sysconf.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/hash.h>
#include <soc/cmext.h>

#include <bcm/error.h>
#include <bcm/init.h>
#include <bcm/tunnel.h>
#include <bcm/ipmc.h>
#include <bcm/debug.h>
#include <ibde.h>


/*
 * Function:
 *      _device_cmd_detach
 * Description:
 * 	    Service routine used to remove bcm device.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_device_cmd_detach(int unit, args_t *a) 
{
    parse_table_t   pt;
    int             rv = BCM_E_NONE;
    cmd_result_t    retCode;
    int             dunit = unit;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Unit",    PQ_DFL | PQ_INT,  0, (void *)&dunit, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }
    if (!soc_attached(dunit)) {
        cli_out("%s: unit %d already detached\n", 
                ARG_CMD(a), dunit);
        return (CMD_FAIL);
    }

    rv = sysconf_detach(dunit);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: detach unit %d error : %s\n", 
                ARG_CMD(a), dunit, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return (CMD_OK);

}

/*
 * Function:
 *      _device_cmd_attach
 * Description:
 * 	    Service routine used to add and initialize bcm device.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments. 
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
_device_cmd_attach(int unit, args_t *a) 
{
    parse_table_t   pt;
    int             rv;
    cmd_result_t    retCode;
    int             dunit = unit;
    int             dev_idx, found = 0;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "Unit",    PQ_DFL | PQ_INT,  0, (void *)&dunit, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    rv = sysconf_probe();

    if (BCM_FAILURE(rv)) {
        cli_out("%s: sysconf_probe error\n", 
                ARG_CMD(a));
        return (CMD_FAIL);
    }
    for(dev_idx = 0; dev_idx < soc_ndev ; ++dev_idx) {
        if(SOC_NDEV_IDX2DEV(dev_idx) == dunit) {
            found = 1;
            break;
        }
    }
    
    if (!found) {
        cli_out("%s: Error: Unit number out of range (%d - %d)\n",
                ARG_CMD(a), 0, soc_ndev - 1);
        return(CMD_FAIL);
    } else if (soc_attached(dunit)) {
        cli_out("%s: Error: Unit already attached: %d\n", ARG_CMD(a), dunit);
        return(CMD_FAIL);
    } else if (sysconf_attach(dunit) < 0) {
        cli_out("%s: Error: Could not sysconf_attach unit: %d\n", ARG_CMD(a), dunit);
        return(CMD_FAIL);
    }

#if defined(BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(dunit)) {    

        rv = soc_init(dunit);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: soc_init error : %s\n", 
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
        
        rv = soc_misc_init(dunit);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: soc_misc_init error : %s\n", 
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
        
        rv = soc_mmu_init(dunit);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: soc_mmu_init error : %s\n", 
                    ARG_CMD(a), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(dunit)) {
#ifdef INCLUDE_PKTIO
            if (soc_feature(unit, soc_feature_sdklt_pktio)) {
                rv = bcm_attach(dunit, "tomahawk3x", NULL, 0);
            } else
#endif
            {
                rv = bcm_attach(dunit, "tomahawk3", NULL, 0);
            }
        } else
#endif
        {
            rv = bcm_attach(dunit, "esw", NULL, 0);
        }
    } else 
#endif

    if (BCM_FAILURE(rv)) {
        cli_out("%s: bcm_attach error : %s\n", 
                ARG_CMD(a), bcm_errmsg(rv));
        return (CMD_FAIL);
    }
    return (CMD_OK);
}

char cmd_device_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  device <option> [args...]\n"
#else
    "  device attach [<unit>=<id>]\n\t"
    "  device detach [<unit>=<id>]\n"
#endif
    ;
cmd_result_t
cmd_device(int unit, args_t *a)
{
    char           *table;

    if ((table = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    /* Device create*/
    if (sal_strcasecmp(table, "attach") == 0) {
         return _device_cmd_attach(unit, a); 
    }
    if (sal_strcasecmp(table, "detach") == 0) {
         return _device_cmd_detach(unit, a); 
    }
    return CMD_USAGE;
}
