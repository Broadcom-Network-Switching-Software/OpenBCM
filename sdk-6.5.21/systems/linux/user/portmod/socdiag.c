/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * socdiag: low-level diagnostics shell for Orion (SOC) driver.
 */

#include <unistd.h>
#include <stdlib.h>

#include <sal/core/boot.h>
#include <sal/compiler.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/core/alloc.h>
#include <sal/appl/config.h>
#include <sal/appl/pci.h>

#include <shared/bsl.h>
#include <shared/shr_bprof.h>

#include <soc/debug.h>

#include <appl/diag/bslmgmt.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/bsldnx.h>

#include <appl/portmod/portmod_appl.h>


/* The bus properties are (currently) the only system specific
 * settings required. 
 * These must be defined beforehand 
 */

#ifndef SYS_BE_PIO
#error "SYS_BE_PIO must be defined for the target platform"
#endif
#ifndef SYS_BE_PACKET
#error "SYS_BE_PACKET must be defined for the target platform"
#endif
#ifndef SYS_BE_OTHER
#error "SYS_BE_OTHER must be defined for the target platform"
#endif

#if !defined(SYS_BE_PIO) || !defined(SYS_BE_PACKET) || !defined(SYS_BE_OTHER)
#error "platform bus properties not defined."
#endif


/* BSL init */
int bsl_appl_init(void)
{
    int rv = 0;

    /*logging mechanisem init*/
    rv = bslmgmt_init();
    if(rv < 0){
        return rv;
    }

    rv = bsldnx_mgmt_init(0x0 /*unit */);
    if(rv < 0){
        return rv;
    }
    /*logging settings*/
    bslenable_set(bslLayerAppl, bslSourcePort, bslSeverityNormal);
    bslenable_set(bslLayerSoc, bslSourcePort, bslSeverityNormal);
    bslenable_set(bslLayerSoc, bslSourcePhymod, bslSeverityVerbose);

    return rv;
}

/*
 * Main loop.
 */
int main(int argc, char *argv[])
{
    int i, len;
    char *envstr;
    char *config_file, *config_temp;

    if ((envstr = getenv("BCM_CONFIG_FILE")) != NULL) {
        config_file = envstr;
        len = sal_strlen(config_file);
        if ((config_temp = sal_alloc(len+5, NULL)) != NULL) {
            sal_strcpy(config_temp, config_file);
            sal_strcpy(&config_temp[len], ".tmp");
            sal_config_file_set(config_file, config_temp);
            sal_free(config_temp);
        }
    }

    if (sal_core_init() < 0 || sal_appl_init() < 0) {
        /*
         * If SAL initialization fails then printf will most
         * likely assert or fail. Try direct console access
         * instead to get the error message out.
         */
    char *estr = "SAL Initialization failed\r\n";
        sal_console_write(estr, sal_strlen(estr) + 1);
    exit(1);
    }
    
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--reload")) {
            sal_boot_flags_set(sal_boot_flags_get() | BOOT_F_RELOAD);
        }
    }

    if (bsl_appl_init() < 0) {
        exit(1);
    }

    /* 
     * Slim SDK application example
     */

    /* Dram Conf */

    /* PortMod */
    portmod_appl_main();

    return 0;
}

