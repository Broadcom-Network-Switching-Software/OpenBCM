/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * socdiag: low-level diagnostics shell for Orion (SOC) driver.
 */

#include <shared/bsl.h>

#include <appl/diag/system.h>
#include <unistd.h>

#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <soc/debug.h>

#include <bde/pli/plibde.h>

ibde_t *bde;

int
bde_create(void)
{	
    return plibde_create(&bde);
}

int devlist(const char* arg); 

/*
 * Main loop.
 */
int main(int argc, char *argv[])
{
    char *socrc = SOC_INIT_RC;
    char *config_file = NULL, *config_temp = NULL;
    int len = 0;

    if ((config_file = getenv("BCM_CONFIG_FILE")) != NULL) {
        len = sal_strlen(config_file);
        if ((config_temp = sal_alloc(len+5, NULL)) != NULL) {
            sal_strncpy(config_temp, config_file, len);
            sal_strncpy(&config_temp[len], ".tmp", 5);
            sal_config_file_set(config_file, config_temp);
            /* coverity[tainted_data] */            
            sal_free(config_temp);
        } else {
            LOG_CLI((BSL_META("sal_alloc failed. \n")));
            exit(1);
        }
    }

    if (sal_core_init() < 0 || sal_appl_init() < 0) {
	LOG_CLI((BSL_META("SAL Initialization failed\n")));
	exit(1);
    }

#ifdef DEBUG_STARTUP
    debugk_select(DEBUG_STARTUP);
#endif

    LOG_CLI((BSL_META("Broadcom Command Monitor: "
                      "Copyright (c) 1998-2010 Broadcom Corp.\n")));
    LOG_CLI((BSL_META("Version %s built %s\n"), _build_release, _build_date));

    switch (argc) {
    case 3:
	socrc = argv[2];
    case 2:
	if (!strcmp(argv[1], "-testbios") || !strcmp(argv[1], "-tb")) {
	    diag_init();
	    if (socrc) {
		sh_rcload_file(0, NULL, socrc, 0);
	    }
	} else if (!strcmp(argv[1], "-reload") || !strcmp(argv[1], "-r")) {
	    sal_boot_flags_set(sal_boot_flags_get() | BOOT_F_RELOAD);
	    diag_shell();
        } else if (!strcmp(argv[1], "-devlist")) {
            devlist(argv[2]); 
            return 0; 
	} else {
	    LOG_CLI((BSL_META("Unknown option: %s\n"), argv[1]));
	    exit(1);
	}
	break;

    default:
	diag_shell();
	break;
    }

    return 0;
}

#include <soc/cm.h>
#include <appl/diag/sysconf.h>

int
devlist(const char* arg)
{
    sysconf_init(); 
    soc_cm_display_known_devices();
    return 0; 
}       
    
