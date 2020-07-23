/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Single entry point initialization of SDKLT-based application.
 */

#ifndef __SYSCONF_LTSW_H__
#define __SYSCONF_LTSW_H__

#ifdef BCM_LTSW_SUPPORT

extern int
sysconf_ltsw_init(void);

extern int
sysconf_ltsw_probe(void);

extern int
sysconf_ltsw_ha_init(void);

extern int
sysconf_ltsw_ha_deinit(void);

extern int
sysconf_ltsw_ha_state_dump(void);

extern char *
sysconf_ltsw_ha_state_path(void);

extern int
sysconf_ltsw_ha_state_comp(void);

extern int
sysconf_ltsw_issu_start_version_set(const char *start_ver);

extern int
sysconf_ltsw_issu_current_version_set(const char *current_ver);

extern int
sysconf_ltsw_issu_upgrade_start(void);

extern int
sysconf_ltsw_issu_upgrade_done(void);

extern int
sysconf_ltsw_core_start(void);

extern int
sysconf_ltsw_attach(int unit);

extern int
sysconf_ltsw_detach(int unit);

extern int
sysconf_ltsw_dev_inited(int unit);

extern int
sysconf_ltsw_dev_init(int unit);

extern int
sysconf_ltsw_dev_deinit(int unit);

extern int
sysconf_ltsw_dev_override(int unit, const char *dev_name);

extern void
sysconf_ltsw_dev_env_set(int unit);

extern void
sysconf_ltsw_dev_env_unset(int unit);

extern int
sysconf_ltsw_cleanup(void);

extern int
sysconf_ltsw_warmboot_set(int val);

extern int
sysconf_ltsw_emulation_set(int val);

extern int
sysconf_ltsw_config_string_set(int val, const char *valstr);

extern int
sysconf_ltsw_config_file_set(int val, const char* valstr);

extern int
sysconf_ltsw_config_init(void);

extern int
sysconf_ltsw_warmexit_set(int val);

extern int
sysconf_ltsw_phymod_sim_set(int val);

extern int
sysconf_ltsw_keep_netif_set(int val);

extern int
sysconf_ltsw_ha_check_set(int val, const char *valstr);

#endif /* BCM_LTSW_SUPPORT */
#endif	/*! __SYSCONF_LTSW_H__ */
