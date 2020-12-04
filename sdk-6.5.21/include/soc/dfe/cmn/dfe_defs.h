/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE DEFS H
 */
#ifndef _SOC_DFE_FABRIC_DEFS_H_
#define _SOC_DFE_FABRIC_DEFS_H_

#include <soc/error.h>
#include <shared/port.h>
#include <shared/fabric.h>




#define SOC_DFE_MODID_NOF           2048
#define SOC_DFE_MAX_NOF_PIPES       3
#define SOC_DFE_NOF_DCH             4
#define SOC_DFE_NOF_DCM             2
#define SOC_DFE_NOF_DCL             4
#define SOC_DFE_NOF_CCS             2
#define SOC_DFE_NOF_FIFOS_PER_DCM   4
#define SOC_DFE_MAX_NOF_LINKS       (144)
#define SOC_DFE_MAX_NOF_MAC         (36)
#define SOC_DFE_NOF_LCPLL           4

#define SOC_DFE_MODID_NOF_UINT32_SIZE   (SOC_DFE_MODID_NOF/32)




typedef enum soc_dfe_fabric_link_cell_size_e
{
    soc_dfe_fabric_link_cell_size_VSC256_V1 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V1,
    soc_dfe_fabric_link_cell_size_VSC128 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC128,
    soc_dfe_fabric_link_cell_size_VSC256_V2 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V2
} soc_dfe_fabric_link_cell_size_t;

typedef enum soc_dfe_fabric_link_fifo_type_index_e
{
    soc_dfe_fabric_link_fifo_type_index_0 = 0,
    soc_dfe_fabric_link_fifo_type_index_1 = 1,
    soc_dfe_fabric_nof_link_fifo_types = 2
} soc_dfe_fabric_link_fifo_type_index_t;

typedef enum soc_dfe_fabric_device_mode_e
{
    soc_dfe_fabric_device_mode_single_stage_fe2 = 0,
    soc_dfe_fabric_device_mode_multi_stage_fe2, 
    soc_dfe_fabric_device_mode_multi_stage_fe13,
    soc_dfe_fabric_device_mode_repeater,
    soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric
} soc_dfe_fabric_device_mode_t;

typedef enum soc_dfe_fabric_priority_e
{
    soc_dfe_fabric_priority_0 = 0,
    soc_dfe_fabric_priority_1, 
    soc_dfe_fabric_priority_2,
    soc_dfe_fabric_priority_3,
    soc_dfe_fabric_priority_nof
} soc_dfe_fabric_priority_t;

typedef enum soc_dfe_multicast_mode_e
{
    soc_dfe_multicast_mode_direct = 0,
    soc_dfe_multicast_mode_indirect = 1
} soc_dfe_multicast_mode_t;

typedef enum soc_dfe_load_balancing_mode_e{
    soc_dfe_load_balancing_mode_normal    = 0,
    soc_dfe_load_balancing_mode_destination_unreachable,    
    soc_dfe_load_balancing_mode_balanced_input    
} soc_dfe_load_balancing_mode_t;


typedef enum soc_dfe_multicast_table_mode_e{
    soc_dfe_multicast_table_mode_64k    = 0, 
    soc_dfe_multicast_table_mode_128k   = 1,     
	soc_dfe_multicast_table_mode_128k_half = 2 
} soc_dfe_multicast_table_mode_t;



typedef enum soc_dfe_fabric_isolate_type_e {
    soc_dfe_fabric_isolate_type_none = 0,
    soc_dfe_fabric_isolate_type_isolate = 1,
    soc_dfe_fabric_isolate_type_shutdown = 2
} soc_dfe_fabric_isolate_type_t;

typedef enum soc_dfe_cosq_weight_mode_e
{
    soc_dfe_cosq_weight_mode_regular = 0,
    soc_dfe_cosq_weight_mode_dynamic_0 = 1,
    soc_dfe_cosq_weight_mode_dynamic_1 = 2
} soc_dfe_cosq_weight_mode_t;

typedef enum soc_dfe_cosq_shaper_mode_e
{
    soc_dfe_cosq_shaper_mode_byte = 0,
    soc_dfe_cosq_shaper_mode_packet = 1
} soc_dfe_cosq_shaper_mode_t;

#define SOC_DFE_MODID_LOCAL_NOF(unit)   (SOC_DFE_IS_FE13(unit) ? SOC_DFE_DEFS_GET(unit, nof_local_modid_fe13) : SOC_DFE_DEFS_GET(unit, nof_local_modid))
#define SOC_DFE_MODID_GROUP_NOF(unit)   (SOC_DFE_IS_FE13(unit) ? SOC_DFE_DEFS_GET(unit, nof_local_modid_fe13) : SOC_DFE_DEFS_GET(unit, nof_local_modid))

#endif 

