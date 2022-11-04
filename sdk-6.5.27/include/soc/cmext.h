/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_CMEXT_H
#define _SOC_CMEXT_H

#include <soc/cmtypes.h>

extern int soc_cm_init(void);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT)
extern int soc_cm_deinit(void);
#endif
/*
 * DRIVER/DEVICE INITIALIZATION
 * ----------------------------------------
 * 
 * Step One: Device/Revision Ids
 * -------------------------------
 * Each Broadcom device can be identified by 2 values -- its
 * Device ID and its Revision ID. The combination of these two values
 * specify a particular Broadcom device. 
 *
 * Before the BCM driver is assigned the task of driving 
 * a Broadcom device, you must ascertain the Device ID and the 
 * Revision ID for the device to be driven (for a PCI device, these
 * are in the PCI Configuration Space). 
 *
 * After the Device ID and Revision ID are discovered (typically at
 * system initialization time), you should query the SOC driver to 
 * discover whether it supports the discovered device using the following 
 * function:
 */

/*
 * Syntax:     int soc_cm_device_supported(uint16 device_id,
 *					   uint8 revision_id)
 *
 * Purpose:    Determine whether this driver supports a particular device. 
 * 
 * Parameters:
 *             device_id    -- The 16-bit device id for the device. 
 *             revision_id  -- The 8-bit revision id for the device. 
 * 
 * Returns:
 *             0 if the device is supported. 
 *             < 0 if the device is not supported. 
 */

extern int soc_cm_device_supported(uint16 dev_id, uint8 rev_id);

/*
 * Step Two: Device Creation
 * -----------------------------------------
 * When you have a device that the SOC driver supports, 
 * You must create a driver device for it. 
 * You create a driver device by calling cm_device_create() with
 * the device and revision ids, and the BDE/DRD device ID/handle. 
 *
 * cm_device_create() will return a handle=unit that should be used
 * when referring to the device in the future. 
 * On success, a none-negative unit=logical device ID=CM device handle
 * is returned. None-negative values are not errors.
 * Negative values indicate a SOC_E_XXX error.

 * cm_device_create_id() might be used if application want to 
 * force a speicifc handle=unit when referring to the device in the future.
 * Passing -1 as a "unit" parameter will force api to allocate a new handle.
 *
 * soc_cm_device_create_composite() is used for creating composite devices.
 */

extern int soc_cm_device_create(uint16 dev_id, uint16 rev_id, void *cookie, int bde_dev);
extern int soc_cm_device_create_id(uint16 dev_id, uint16 rev_id,
                                   void *cookie, int unit, int bde_dev);
extern int soc_cm_device_create_composite(uint32 composite_dev_type,
              int *sub_devs, void *cookie, int comp_unit);

/*
 * Step Three: Initial HA memory
 * -----------------------------------------
 * For LT-based devices, initial the HA memory for each unit before
 * attaching device.
 */
extern int soc_cm_ha_mem_init(int unit,
                              soc_cm_ha_mem_pool_alloc_f pool_alloc,
                              soc_cm_ha_mem_pool_free_f pool_free,
                              void *pool_ctx,
                              void *pool_ptr,
                              int pool_size);
extern int soc_cm_ha_mem_cleanup(int unit);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT)
extern int soc_cm_ha_mem_reset(int unit);
extern int soc_cm_default_curr_ver_get(const char **sdk_ver,
                                       const char **sdklt_ver);
extern int soc_cm_issu_auto_start(int ffb_mode, const char *dll_path);
extern int soc_cm_issu_upgrade_start(const char *from_ver, const char *to_ver,
                                     const char *dll_path);
extern int soc_cm_issu_upgrade_done(void);
extern int soc_cm_issu_curr_ver_set(const char *version);
extern int soc_cm_issu_start_ver_get(const char **version);
extern int soc_cm_ffb_start(const char *from_ver, const char *to_ver,
                            const char *dll_path);
extern int soc_cm_ffb_done(void);
extern int soc_cm_issu_dll_path_get(const char **dllpath);
#endif
extern void soc_cm_system_restore_state_set(int restore);
extern void soc_cm_system_restore_state_get(int *restore);

/*
 * Step Four: Device Initialization
 * -----------------------------------------
 * After you have initialized a device, 
 * you must provide several accessor functions that will be
 * used by the SOC driver to communicate with the device at 
 * the lowest level -- the SOC driver will access the device
 * through these routines, and these routines only. 
 *
 * You provide these access vectors to the driver
 * by filling in the 'cm_device_t' structure. See <cmtypes.h>
 */

extern int soc_cm_device_init(int unit, soc_cm_device_vectors_t *vectors);

#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT)
extern const char *soc_cm_device_name_get(uint16 dev_id, uint8 rev_id);
#endif
extern int soc_cm_device_destroy(int unit);

#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT)
extern int soc_cm_config_init(void);
extern int soc_cm_config_port_str_delete(int unit, int port, const char *name);
extern char *soc_cm_config_port_str_get(int unit, int port, const char *name);
extern int soc_cm_config_port_str_set(int unit, int port,
                      const char *name, const char *val);
extern int soc_cm_config_str_delete(int unit, const char *name);
extern char *soc_cm_config_str_get(int unit, const char *name);
extern int soc_cm_config_str_set(int unit, const char *name, const char *val);
#endif

/*
 * Function:    soc_cm_get_nof_sub_devices
 * Purpose:     Check if the given input id a composite device ID, and if so return its number of sub-devices.
 *
 * Parameters:  comp_dev_id - the composite device id of the device.
 *
 * Returns:     The number of sub devices, or zero if the composite device type was not found.
 */
extern unsigned soc_cm_get_nof_sub_devices(uint32 comp_dev_id);

#endif    /* !_SOC_CMEXT_H */
