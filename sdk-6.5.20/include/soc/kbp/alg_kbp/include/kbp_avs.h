/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __KBP_AVS_H
#define __KBP_AVS_H



#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DEFAULT_VMAX_AVS    (920)
#define DEFAULT_VMIN_AVS    (750)



struct kbp_avs_config
{
    void *handle;                    

    

    int32_t (*mdio_read)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value);

    

    int32_t (*mdio_write)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value);

    int32_t max_avs_voltage; 
    int32_t min_avs_voltage; 
};


kbp_status kbp_device_init_avs(struct kbp_device *device, int32_t vmax, int32_t vmin, FILE *fp);


kbp_status kbp_device_interface_init_avs(enum kbp_device_type type, struct kbp_avs_config *config,
                                         FILE *fp);


kbp_status kbp_device_interface_is_avs_enabled(enum kbp_device_type type, struct kbp_avs_config *config,
                                               uint32_t *is_enabled);

#ifdef __cplusplus
}
#endif
#endif 
