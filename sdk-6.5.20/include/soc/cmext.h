/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_CMEXT_H
#define _SOC_CMEXT_H

#include <soc/cmtypes.h>

extern int soc_cm_init(void);
extern int soc_cm_deinit(void);
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
 * the device and revision ids. 
 *
 * cm_device_create() will return a handle that should be used
 * when referring to the device in the future. 

 * cm_device_create_id() might be used if application want to 
 * force a speicifc handle when referring to the device in the future.
 * Passing -1 as a "dev" parameter will force api to allocate a new handle.
 *
 * soc_cm_device_create_composite() is used for creating composite devices.
 *
 * cm_dev_num_set() might be used to associate the CM device to a
 * BDE/DRD handle if the CM handle and the BDE/DRD handle are not consistent.
 */

extern int soc_cm_device_create(uint16 dev_id, uint16 rev_id, void *cookie);
extern int soc_cm_device_create_id(uint16 dev_id, uint16 rev_id, 
                                   void *cookie, int dev);
extern int soc_cm_device_create_composite(uint32 composite_dev_type,
              int *sub_devs, void *cookie, int comp_dev);
extern int soc_cm_dev_num_set(int dev, int kdev);


/*
 * Step Three: Device Initialization
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

extern int soc_cm_device_init(int dev, soc_cm_device_vectors_t *vectors);

extern const char *soc_cm_device_name_get(uint16 dev_id, uint8 rev_id);
extern int soc_cm_device_destroy(int dev);

extern int soc_cm_config_init(void);
extern int soc_cm_config_str_set(int dev, const char *name, const char *val);
extern char *soc_cm_config_str_get(int dev, const char *name);
extern int soc_cm_config_port_str_set(int dev, int port,
                                      const char *name, const char *val);
extern char *soc_cm_config_port_str_get(int dev, int port, const char *name);

/*
 * Function:    soc_cm_get_nof_sub_devices
 * Purpose:     Check if the given input id a composite device ID, and if so return its number of sub-devices.
 *
 * Parameters:  comp_dev_id - the composite device id of the device.
 *
 * Returns:     The number of sub devices, or zero if the composite device type was not found.
 */
extern unsigned soc_cm_get_nof_sub_devices(uint32 comp_dev_id);

#endif	/* !_SOC_CMEXT_H */
