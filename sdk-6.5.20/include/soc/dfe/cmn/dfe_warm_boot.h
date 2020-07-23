/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE WARM BOOT H
 */

#ifndef _SOC_DFE_WARM_BOOT_H
#define _SOC_DFE_WARM_BOOT_H





#include <soc/error.h>
#include <soc/wb_engine.h>





#define SOC_DFE_WARM_BOOT_ENGINE            (SOC_WB_ENGINE_PRIMARY)

#define SOC_DFE_WB_ANY                      (0)


#define SOC_DFE_WARM_BOOT_VAR_SET(_unit_, _var_, _value_) \
    soc_wb_engine_var_set(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, SOC_DFE_WB_ANY, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_VAR_GET(_unit_, _var_, _value_) \
    soc_wb_engine_var_get(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, SOC_DFE_WB_ANY, (uint8 *) _value_)


#define SOC_DFE_WARM_BOOT_ARR_VAR_SET(_unit_, _var_, _inner_index_ , _value_) \
    soc_wb_engine_var_set(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, _inner_index_, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_ARR_VAR_GET(_unit_, _var_, _inner_index_ , _value_) \
    soc_wb_engine_var_get(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , SOC_DFE_WB_ANY, _inner_index_, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_ARR_MEMSET(_unit_, _var_, _value_) \
            SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , _value_);


#define SOC_DFE_WARM_BOOT_2ARR_VAR_SET(_unit_, _var_, _outer_index_, _inner_index_ , _value_) \
    soc_wb_engine_var_set(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , _outer_index_, _inner_index_, (uint8 *) _value_)

#define SOC_DFE_WARM_BOOT_2ARR_VAR_GET(_unit_, _var_, _outer_index_, _inner_index_ , _value_) \
    soc_wb_engine_var_get(_unit_, SOC_DFE_WARM_BOOT_ENGINE, SOC_DFE_WARM_BOOT_VAR_##_var_ , _outer_index_, _inner_index_, (uint8 *) _value_)
        






typedef enum soc_dfe_warm_boot_buffer_e
{
    SOC_DFE_WARM_BOOT_BUFFER_MODID = 0,
    SOC_DFE_WARM_BOOT_BUFFER_MC = 1,
    SOC_DFE_WARM_BOOT_BUFFER_FIFO = 2,
    SOC_DFE_WARM_BOOT_BUFFER_PORT = 3,
    SOC_DFE_WARM_BOOT_BUFFER_INTR = 4,
    SOC_DFE_WARM_BOOT_BUFFER_ISOLATE = 5,
    SOC_DFE_WARM_BOOT_BUFFER_NOF
} soc_dfe_warm_boot_buffer_t;



typedef enum soc_dfe_warm_boot_var_e
{
    SOC_DFE_WARM_BOOT_VAR_MODID_LOCAL_MAP = 0,
    SOC_DFE_WARM_BOOT_VAR_MODID_GROUP_MAP = 1,
    SOC_DFE_WARM_BOOT_VAR_MODID_MODID_TO_GROUP_MAP = 2,

    SOC_DFE_WARM_BOOT_VAR_MC_MODE = 3,
    SOC_DFE_WARM_BOOT_VAR_MC_ID_MAP = 4,

    SOC_DFE_WARM_BOOT_VAR_FIFO_HANDLERS = 5,
    
    SOC_DFE_WARM_BOOT_VAR_PORT_COMMA_BURST_CONF = 6,
    SOC_DFE_WARM_BOOT_VAR_PORT_CTRL_BURST_CONF = 7,
    SOC_DFE_WARM_BOOT_VAR_PORT_CL72_CONF = 8,
    
    SOC_DFE_WARM_BOOT_VAR_INTR_STORM_TIMED_COUNT = 9,
    SOC_DFE_WARM_BOOT_VAR_INTR_STORM_TIMED_PERIOD = 10,
    SOC_DFE_WARM_BOOT_VAR_INTR_STORM_NOMINAL = 11,
    SOC_DFE_WARM_BOOT_VAR_INTR_FLAGS = 12,

    SOC_DFE_WARM_BOOT_VAR_ISOLATE_UNISOLATED_LINKS = 13,
    SOC_DFE_WARM_BOOT_VAR_ISOLATE_ACTIVE_LINKS = 14,
    SOC_DFE_WARM_BOOT_VAR_ISOLATE_ISOLATE_DEVICE = 15,
    SOC_DFE_WARM_BOOT_VAR_ISOLATE_TYPE = 16,

    
    SOC_DFE_WARM_BOOT_VAR_NOF
} soc_dfe_warm_boot_var_t;






soc_error_t soc_dfe_warm_boot_sync(int unit);
soc_error_t soc_dfe_warm_boot_init(int unit);
soc_error_t soc_dfe_warm_boot_deinit(int unit);
soc_error_t soc_dfe_warm_boot_engine_init_buffer_struct(int unit, int buffer_id);
soc_error_t soc_dfe_warm_boot_buffer_id_create(int unit, int buffer_id);





#endif 
