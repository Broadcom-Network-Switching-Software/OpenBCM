
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_H_INCLUDED

#define _BCMINT_DNX_DRAM_H_INCLUDED

#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>

#define DNX_DRAM_BMP_FIRST(bm, max_dram_count, first_dram) \
    do {\
        SHR_BIT_ITER(bm, max_dram_count, first_dram) {break;} \
        if (first_dram == max_dram_count) first_dram = -1; \
    } while(0)

#define DNX_DRAM_BLOCK_CONFIG_PARAM_INVALID         (-1)

typedef struct
{
    hbm_dram_vendor_info_hbm_t hbm_info;
    gddr6_dram_vendor_info_t gddr6_info;
} dnx_dram_vendor_info_t;

typedef enum
{
    DNX_DRAM_BLOCK_MECHANISM_FIRST,
    DNX_DRAM_BLOCK_MECHANISM_WRITE_MINUS_READ = DNX_DRAM_BLOCK_MECHANISM_FIRST,
    DNX_DRAM_BLOCK_MECHANISM_WRITE_PLUS_READ,
    DNX_DRAM_BLOCK_MECHANISM_WRITE,
    DNX_DRAM_BLOCK_MECHANISM_AVERAGE_READ_INFLIGHTS,
    DNX_DRAM_BLOCK_MECHANISM_LAST = DNX_DRAM_BLOCK_MECHANISM_AVERAGE_READ_INFLIGHTS,
    DNX_DRAM_BLOCK_MECHANISM_NOF
} dnx_dram_block_mechanism_e;

shr_error_e dnx_dram_init(
    int unit);

shr_error_e dnx_dram_deinit(
    int unit);

shr_error_e dnx_dram_power_down_cb_register(
    int unit,
    uint32 flags,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);

shr_error_e dnx_dram_power_down_cb_unregister(
    int unit,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);

shr_error_e dnx_dram_power_down(
    int unit,
    uint32 flags);

shr_error_e dnx_dram_reinit(
    int unit,
    uint32 flags);

shr_error_e dnx_dram_traffic_enable_set(
    int unit,
    uint32 flags,
    uint32 enable);

shr_error_e dnx_dram_is_traffic_allowed_into_dram(
    int unit,
    uint8 *allow_traffic_to_dram);

shr_error_e dnx_dram_thermo_sensor_read(
    int unit,
    int interface_id,
    bcm_switch_thermo_sensor_t * sensor_data);

shr_error_e dnx_dram_vendor_info_get(
    int unit,
    int interface_id,
    dnx_dram_vendor_info_t * vendor_info);

shr_error_e dnx_dram_count(
    int unit,
    int *nof_drams);

shr_error_e dnx_dram_index_is_valid(
    int unit,
    int dram_index,
    int *is_valid);

shr_error_e dnx_dram_blocks_enable_set(
    int unit);

shr_error_e dnx_dram_initial_config(
    int unit);

shr_error_e dnx_dram_block_configure(
    int unit);

#endif
