/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INCLUDE_SOC_DNX_VISIBILITY_H_
#define INCLUDE_SOC_DNX_VISIBILITY_H_

#include <shared/shrextend/shrextend_error.h>

#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>

#include <bcm/instru.h>
#include <bcm/types.h>

#define DNX_VISIBILITY_RETRY_READY_COUNT        3
#define DNX_VISIBILITY_BLOCK_UNDEFINED          -1

#define DNX_VISIBILITY_BLOCK_MAX_NOF_LINES         1000

#define DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY       (0)

#define DNX_VISIBILITY_SIG_CACHE_STATUS_VALID       (1)

#define DNX_VISIBILITY_SIG_CACHE_STATUS_VALUE_NA    (2)

char *dnx_debug_mem_name(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block);

shr_error_e dnx_visibility_selective_fetch_all(
    int unit,
    int core);

shr_error_e dnx_debug_mem_lines(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int *size_p);

shr_error_e dnx_debug_mem_cache(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    int *status_p);

shr_error_e dnx_debug_mem_is_ready(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block);

shr_error_e dnx_debug_mem_line_is_ready(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index);

shr_error_e dnx_debug_block_is_ready(
    int unit,
    int core,
    dnx_pp_block_e pp_block);

shr_error_e dnx_debug_reg_read(
    int unit,
    int core,
    char *hw_name,
    int size,
    uint32 *value_array);

shr_error_e dnx_debug_pema_read(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int direction,
    dbal_enum_value_field_pema_id_e pema_id,
    int window,
    uint32 *value_array);

shr_error_e dnx_debug_mem_read(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    uint32 **data_ptr);

#define         DNX_VISIBILITY_RESUME_ZERO       0x02

shr_error_e dnx_visibility_resume(
    int unit,
    int core,
    uint32 flags);

shr_error_e dnx_visibility_mode_set(
    int unit,
    bcm_instru_vis_mode_control_t mode);

shr_error_e dnx_visibility_mode_get(
    int unit,
    bcm_instru_vis_mode_control_t * mode_p,
    int full_flag);

shr_error_e dnx_visibility_sampling_set(
    int unit,
    uint32 direction,
    uint32 period);

shr_error_e dnx_visibility_sampling_get(
    int unit,
    uint32 direction,
    uint32 *period_p);

shr_error_e dnx_visibility_pp_port_enable_set(
    int unit,
    int core,
    int pp_port,
    int enable);

shr_error_e dnx_visibility_pp_port_enable_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p);

shr_error_e dnx_visibility_port_enable_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable);

shr_error_e dnx_visibility_port_enable_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p);

shr_error_e dnx_visibility_pp_port_force_set(
    int unit,
    int core,
    int pp_port,
    int enable);

shr_error_e dnx_visibility_pp_port_force_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p);

shr_error_e dnx_visibility_port_force_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable);

shr_error_e dnx_visibility_port_force_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p);

shr_error_e dnx_visibility_prt_qualifier_get(
    int unit,
    uint32 *prt_qualifier_p);

shr_error_e dnx_visibility_port_egress_enable(
    int unit);

shr_error_e dnx_visibility_port_egress_disable(
    int unit);

shr_error_e dnx_visibility_init(
    int unit);

shr_error_e dnx_visibility_deinit(
    int unit);

shr_error_e dnx_visibility_engine_clear(
    int unit,
    int core);

#define DISCERN_STATUS(pers_status, temp_status, status_str)                                    \
    if(pers_status == SAL_UINT32_MAX)                                                           \
    {                                                                                           \
        pers_status = temp_status;                                                              \
    }                                                                                           \
    else                                                                                        \
    {                                                                                           \
        if(pers_status != temp_status)                                                          \
        {                                                                                       \
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Inconsistent %s\n", status_str);                     \
        }                                                                                       \
    }

typedef struct
{

    char *name;

    dbal_tables_e table;

    dbal_fields_e valids_field;

    uint32 direction;
} dnx_visibility_block_map_t;

#define CHUNK_SIZE_BITS 512

typedef struct
{

    int line_nof;

    int width;

    int line_size;

    int data_nof;

    uint32 *sig_cash_p[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_VISIBILITY_BLOCK_MAX_NOF_LINES];

    int sig_cash_status[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_VISIBILITY_BLOCK_MAX_NOF_LINES];
} dnx_visibility_block_info_t;

typedef struct
{

    bcm_instru_vis_mode_control_t mode;
} dnx_visibility_status_t;

#define DNX_PP_VERIFY_DIRECTION(unit, direction_m)                                                   \
    if (!(direction_m & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS) &&                                \
                                         !(direction_m & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS))  \
    {                                                                                                \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal PP Direction value:%d\n", direction_m);                  \
    }

#endif
