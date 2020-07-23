/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bcm_xmod_pe_api.h
 */

#include "bcm_xmod_api.h"


#define SBUS_LOCK_INIT if (sbus_lock == NULL) \
            SOC_IF_ERROR_RETURN(bcm_sbus_mutex_init())
#define SBUS_LOCK bcm_mutex_take(sbus_lock, sal_mutex_FOREVER)
#define SBUS_UNLOCK bcm_mutex_give(sbus_lock)

#define AXI_LOCK_INIT if (axi_lock == NULL) \
            SOC_IF_ERROR_RETURN(bcm_axi_mutex_init())
#define AXI_LOCK bcm_mutex_take(axi_lock, sal_mutex_FOREVER)
#define AXI_UNLOCK bcm_mutex_give(axi_lock)

#define XMOD_LOCK_INIT if (sbus_lock == NULL) \
            SOC_IF_ERROR_RETURN(bcm_xmod_mutex_init())
#define XMOD_LOCK bcm_mutex_take(xmod_lock, sal_mutex_FOREVER)
#define XMOD_UNLOCK bcm_mutex_give(xmod_lock)

extern int bcm_initialize(void);
extern int bcm_sbus_mutex_init(void);
extern void bcm_sbus_mutex_cleanup(void);
extern int bcm_axi_mutex_init(void);
extern void bcm_axi_mutex_cleanup(void);
extern int bcm_xmod_mutex_init(void);
extern void bcm_xmod_mutex_cleanup(void);
extern int bcm_firmware_download(int unit, int module, void *data, int len);
extern int bcm_port_lane_mask(int unit, int module, int sys_port);
extern int bcm_xmod_pe_init(int unit, int module, xmod_pe_config_t *config);
extern int bcm_xmod_pe_status_get(int unit, int module, xmod_pe_status_t *status);
extern int bcm_xmod_pe_port_stats_get(int unit, int module, int pe_port, xmod_pe_port_stats_t *stats, int reset);
extern int bcm_xmod_pe_fw_ver_get(int unit, int module, xmod_pe_fw_ver_t *ver);
extern int bcm_device_rev_id(int unit, int module, int *rev_id);
extern int bcm_device_die_temp(int unit, int module, int *temp);
extern int bcm_tsc_reg_access(int unit, int module, int sys_port, int write, int reg, int *val);
extern int bcm_mdio_access(int unit, int module, int write, int reg, int *val);
extern int bcm_sbus_access(int unit, int module, uint32 reg, uint32 cmd, int ring, uint32 arg[]);
extern int bcm_axi_reg_access(int unit, int module, int write, uint32 reg, uint32 *val);
extern int bcm_top_reg_access(int unit, int module, int write, uint32 reg, uint32 *val);
extern int bcm_eyescan(int unit, int module, int sys_port);
extern int bcm_dscdump(int unit, int module, int sys_port);
extern int bcm_prbs(int unit, int module, int sys_port, int action, int dir, int poly, int invert);

