/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#ifdef BCM_LTSW_SUPPORT
#include <appl/diag/sysconf_ltsw.h>
#endif

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
    int ac = 0;
#ifdef BCM_LTSW_SUPPORT
    int override_dev = 0;
    int cfg_file_idx = 0;
#endif

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

    ac = 0;
    while (++ac < argc) {
#ifdef BCM_LTSW_SUPPORT
        if (strcmp(argv[ac], "-y") == 0) {
            if (++ac >= argc) {
                printf("No YAML configuration file specified\n");
                exit(1);
            }
            if (sysconf_ltsw_config_file_set(cfg_file_idx, argv[ac]) < 0) {
                printf("Invalid YAML configuration file: %s\n", argv[ac]);
                exit(1);
            }
            cfg_file_idx++;
        }
#endif
    }

    if (sal_core_init() < 0 || sal_appl_init() < 0) {
	LOG_CLI((BSL_META("SAL Initialization failed\n")));
	exit(1);
    }

#ifdef DEBUG_STARTUP
    debugk_select(DEBUG_STARTUP);
#endif

    LOG_CLI((BSL_META("Broadcom Command Monitor: "
                      "Copyright (c) 1998-2018 Broadcom Corp.\n")));
    LOG_CLI((BSL_META("Version %s built %s\n"), _build_release, _build_date));

    ac = 0;
    while (++ac < argc) {
        if (strcmp(argv[ac], "-testbios") == 0 ||
            strcmp(argv[ac], "-tb") == 0) {
            diag_init();
            if (++ac < argc && argv[ac]) {
                socrc = argv[ac];
            }
            sh_rcload_file(0, NULL, socrc, 0);
            return 0;
        } else if (strcmp(argv[ac], "-reload") == 0 ||
                   strcmp(argv[ac], "-r") == 0) {
            sal_boot_flags_set(sal_boot_flags_get() | BOOT_F_RELOAD);
            break;
        } else if (strcmp(argv[ac], "-devlist") == 0) {
            devlist(argv[ac+1]);
            return 0;
#ifdef BCM_LTSW_SUPPORT
        } else if (strcmp(argv[ac], "-a") == 0) {
            if (++ac >= argc) {
                printf("No path specified for storing HA state file.\n");
                exit(1);
            }
            if (sysconf_ltsw_ha_check_set(1, argv[ac]) < 0) {
                printf("Can not set SDK HA check status.\n");
                exit(1);
            }
        } else if (strcmp(argv[ac], "-o") == 0) {
            if (++ac >= argc) {
                printf("No override chip name specified\n");
                exit(1);
            }
            if (sysconf_ltsw_dev_override(override_dev, argv[ac]) < 0) {
                printf("Unsupported device: %s\n", argv[ac]);
                exit(1);
            }
            override_dev++;
        } else if (strcmp(argv[ac], "-s") == 0) {
            if (++ac >= argc) {
                printf("No SDK ISSU start version specified\n");
                exit(1);
            }
            if (sysconf_ltsw_issu_start_version_set(argv[ac]) < 0) {
                printf("Can not set SDK ISSU start version\n");
                exit(1);
            }
        } else if (strcmp(argv[ac], "-v") == 0) {
            if (++ac >= argc) {
                printf("No SDK ISSU current version specified\n");
                exit(1);
            }
            if (sysconf_ltsw_issu_current_version_set(argv[ac]) < 0) {
                printf("Can not set SDK ISSU current version\n");
                exit(1);
            }
        } else if (strcmp(argv[ac], "-phymodsim") == 0) {
            if (sysconf_ltsw_phymod_sim_set(1) < 0) {
                printf("Can not active the PHYMOD real SIMUL mode!\n");
                exit(1);
            }
#endif
        }
    }

    diag_shell();

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
    
