/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef _SOC_DNXF_DRV_H
#define _SOC_DNXF_DRV_H

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/dnxf/swstate/auto_generated/types/dnxf_types.h>

#include <soc/chip.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <soc/types.h>

#include <soc/dnxc/fabric.h>

#include <sal/types.h>

#include <shared/cyclic_buffer.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_lane_map_db.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <shared/shr_thread_manager.h>

#define DNXF_LINK_INPUT_CHECK(unit, link) \
    if ( _SHR_E_NONE != MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_links_validate_link, (unit, link)) ) { \
         SHR_ERR_EXIT(_SHR_E_PORT, "%d isn't a valid link", link); \
    }

#define DNXF_LINK_INPUT_CHECK_DNXC(unit, link) \
    if ( _SHR_E_NONE != MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_links_validate_link, (unit, link)) ) { \
         SHR_ERR_EXIT(_SHR_E_PORT, "%d isn't a valid link", link); \
    }

#define DNXF_IS_PRIORITY_VALID(arg) \
    ((soc_dnxf_fabric_priority_0 == arg) \
        || (soc_dnxf_fabric_priority_1 == arg) \
        || (soc_dnxf_fabric_priority_2 == arg) \
        || (soc_dnxf_fabric_priority_3 == arg))

#define SOC_DNXF_CHECK_MODULE_ID(modid) \
        (((modid < dnxf_data_fabric.topology.nof_system_modid_get(unit)) && (modid >= 0) ) ? 1 : 0)

#define DNXF_ADD_PORT(ptype, nport) \
            si->ptype.port[si->ptype.num++] = nport; \
            if ( (si->ptype.min < 0) || (si->ptype.min > nport) ) {     \
                si->ptype.min = nport; \
            } \
            if (nport > si->ptype.max) { \
                si->ptype.max = nport; \
            }

#define DNXF_REMOVE_DYNAMIC_PORT(ptype, nport) \
            SOC_PBMP_PORT_REMOVE(si->ptype.bitmap, nport);

#define DNXF_ADD_DYNAMIC_PORT(ptype, nport) \
            SOC_PBMP_PORT_ADD(si->ptype.bitmap, nport);

#define DNXF_REMOVE_DYNAMIC_DISABLED_PORT(ptype, nport) \
            SOC_PBMP_PORT_REMOVE(si->ptype.disabled_bitmap, nport);

#define DNXF_ADD_DYNAMIC_DISABLED_PORT(ptype, nport) \
            SOC_PBMP_PORT_ADD(si->ptype.disabled_bitmap, nport);

#define SOC_DNXF_BLOCK_ITER(block_bmp, block) \
    SOC_DNXF_FMAC_ITER(block_bmp, block)

#define SOC_DNXF_FMAC_ITER(fmac_bmp, fmac) \
    _SHR_PBMP_MAX_ITER((fmac_bmp), dnxf_data_device.blocks.nof_instances_fmac_get(unit), (fmac))

#define SOC_DNXF_DCH_ITER(dch_bmp, dch) \
    _SHR_PBMP_MAX_ITER((dch_bmp), dnxf_data_device.blocks.nof_instances_dch_get(unit), (dch))

#define SOC_DNXF_SFI_ITER(sfi_port_bmp, port) \
    _SHR_PBMP_MAX_ITER((sfi_port_bmp), dnxf_data_port.general.nof_links_get(unit), (port))

#define SOC_DNXF_DRV_IS_CORE_ZERO_ACTIVE(unit, is_core_zero_active)  \
    do { \
        SHR_IF_ERR_EXIT(dnxf_state.info.active_core_bmp.bit_get(unit, 0, &is_core_zero_active)); \
    } while(0)

#define SOC_DNXF_CORE_ITER(active_cores, core) \
    SHR_BIT_ITER(active_cores, DNXF_DATA_MAX_DEVICE_GENERAL_NOF_CORES, core)

#define DNXF_DRV_CORE_ALL   (-1)

#define DNXF_DRV_CORE_VALID(core_idx) \
    ((core_idx == DNXF_DRV_CORE_ALL) || \
    ((core_idx >= 0) && (core_idx < (int)dnxf_data_device.general.nof_cores_get(unit))))

#define SOC_DNXF_FABRIC_TDM_PRIORITY_NONE                   (-2)
#define SOC_DNXF_MAX_NUM_OF_PRIORITIES                      (4)
#define SOC_DNXF_DRV_MULTIPLIER_MAX_LINK_SCORE              (120)

typedef struct soc_dnxf_control_s
{

    cyclic_buffer_t sr_cells_buffer;
    cyclic_buffer_t captured_cells_buffer;
    shr_thread_manager_handler_t rx_reception_tmngr_hndlr;

} soc_dnxf_control_t;

typedef struct soc_dnxf_drv_dch_default_thresholds_s
{
    uint32 fifo_size[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 full_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 llfc_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 mc_low_prio_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];

} soc_dnxf_drv_dch_default_thresholds_t;

typedef struct soc_dnxf_drv_dcm_default_thresholds_s
{
    uint32 fifo_size[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 prio_0_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 prio_1_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 prio_2_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 prio_3_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 full_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 almost_full_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 gci_low_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 gci_med_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 gci_high_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 rci_low_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 rci_med_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 rci_high_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];

    uint32 local_switch_fifo_size[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 local_switch_almost_full_0_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    uint32 local_switch_almost_full_1_threshold[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];

} soc_dnxf_drv_dcm_default_thresholds_t;

typedef struct soc_dnxf_drv_dcl_default_thresholds_s
{

    uint32 fifo_size[3];
    uint32 llfc_threshold[3];
    uint32 almost_full_threshold[3];
    uint32 gci_low_threshold[3];
    uint32 gci_med_threshold[3];
    uint32 gci_high_threshold[3];
    uint32 rci_low_threshold[3];
    uint32 rci_med_threshold[3];
    uint32 rci_high_threshold[3];
    uint32 prio_0_threshold[3];
    uint32 prio_1_threshold[3];
    uint32 prio_2_threshold[3];
    uint32 prio_3_threshold[3];

} soc_dnxf_drv_dcl_default_thresholds_t;

typedef enum soc_dnxf_drv_soft_reset_e
{
    soc_dnxf_drv_soft_reset_in,
    soc_dnxf_drv_soft_reset_out,
    soc_dnxf_drv_soft_reset_inout
} soc_dnxf_drv_soft_reset_t;

typedef enum soc_dnxf_drv_block_type_e
{
    SOC_DNXF_DRV_BLOCK_TYPE_INVALID = -1,
    SOC_DNXF_DRV_BLOCK_TYPE_FMAC = 0,
    SOC_DNXF_DRV_BLOCK_TYPE_FSRD,
    SOC_DNXF_DRV_BLOCK_TYPE_DCH,
    SOC_DNXF_DRV_BLOCK_TYPE_DCML,
    SOC_DNXF_DRV_BLOCK_TYPE_DTM,
    SOC_DNXF_DRV_BLOCK_TYPE_DFL,
    SOC_DNXF_DRV_BLOCK_TYPE_DFL_FBC,
    SOC_DNXF_DRV_BLOCK_TYPE_CCH,
    SOC_DNXF_DRV_BLOCK_TYPE_CCML,
    SOC_DNXF_DRV_BLOCK_TYPE_DTL,
    SOC_DNXF_DRV_BLOCK_TYPE_QRH,
    SOC_DNXF_DRV_BLOCK_TYPE_LCM,
    SOC_DNXF_DRV_BLOCK_TYPE_RTP,
    SOC_DNXF_DRV_BLOCK_TYPE_NOF
} soc_dnxf_drv_block_type_t;

#define SOC_DNXF_IS_FE13(unit) \
    (soc_dnxf_fabric_device_mode_multi_stage_fe13 == dnxf_data_fabric.general.device_mode_get(unit))
#define SOC_DNXF_IS_FE2(unit) \
    ((soc_dnxf_fabric_device_mode_multi_stage_fe2 == dnxf_data_fabric.general.device_mode_get(unit)) || \
     (soc_dnxf_fabric_device_mode_single_stage_fe2 == dnxf_data_fabric.general.device_mode_get(unit)))
#define SOC_DNXF_IS_SINGLESTAGE_FE2(unit) \
    (soc_dnxf_fabric_device_mode_single_stage_fe2 == dnxf_data_fabric.general.device_mode_get(unit))
#define SOC_DNXF_IS_MULTISTAGE_FE2(unit) \
    (soc_dnxf_fabric_device_mode_multi_stage_fe2 == dnxf_data_fabric.general.device_mode_get(unit))
#define SOC_DNXF_IS_MULTISTAGE(unit)     \
    ((soc_dnxf_fabric_device_mode_multi_stage_fe13 == dnxf_data_fabric.general.device_mode_get(unit)) || \
     (soc_dnxf_fabric_device_mode_multi_stage_fe2 == dnxf_data_fabric.general.device_mode_get(unit)))

#define SOC_DNXF_IS_FE13_SYMMETRIC(unit) \
    (soc_dnxf_fabric_device_mode_multi_stage_fe13 == dnxf_data_fabric.general.device_mode_get(unit))

extern sal_mutex_t _soc_dnxf_lock[BCM_MAX_NUM_UNITS];

#define SOC_DNXF_LOCK(unit) \
        sal_mutex_take(_soc_dnxf_lock[unit], sal_mutex_FOREVER)

#define SOC_DNXF_UNLOCK(unit) \
        sal_mutex_give(_soc_dnxf_lock[unit])

#define DNXF_UNIT_LOCK_INIT(unit) \
        int _lock_taken = 0

#define DNXF_UNIT_LOCK_TAKE(unit) \
    do{ \
        if (SOC_DNXF_LOCK(unit)) { \
            LOG_ERROR_EX(BSL_LOG_MODULE, "unable to obtain unit lock %s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);\
            SHR_EXIT(); \
        } \
        _lock_taken=1;    \
    }while (0)

#define DNXF_UNIT_LOCK_RELEASE(unit) \
    do { \
        if (1 == _lock_taken) { \
            if (SOC_DNXF_UNLOCK(unit)) { \
                LOG_ERROR_EX(BSL_LOG_MODULE, "unable to obtain unit release on unit %d%s%s%s\n", unit, EMPTY, EMPTY, EMPTY);\
                SHR_EXIT(); \
            }  \
            _lock_taken=0;    \
        } \
    } while (0)

#define INT_DEVIDE(num,dev) (num-(num%dev))/dev;

#define SOC_DNXF_DRV_INIT_LOG(_unit_, _msg_str_)\
                LOG_VERBOSE_EX(BSL_LOG_MODULE, "    + %d: %s %s%s\n", _unit_ , _msg_str_, EMPTY, EMPTY)

#define SOC_DNXF_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, soc_prop_name, suffix, num, int_val)  \
        do {                                                                                                \
            if (int_val != SOC_DNXF_PROPERTY_UNAVAIL)                                                       \
            {                                                                                               \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s_%s%d=%d\n"), soc_prop_name, suffix, num, int_val));                 \
            }                                                                                               \
        } while (0)

#define SOC_DNXF_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_INT_DUMP(unit, soc_prop_name, suffix, int_val)           \
        do {                                                                                                \
            if (int_val != SOC_DNXF_PROPERTY_UNAVAIL)                                                       \
            {                                                                                               \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s_%s=%d\n"), soc_prop_name, suffix, int_val));                        \
            }                                                                                               \
        } while (0)

#define SOC_DNXF_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, soc_prop_name, int_val)                          \
        do {                                                                                                \
            if (int_val != SOC_DNXF_PROPERTY_UNAVAIL)                                                       \
            {                                                                                               \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s=%d\n"), soc_prop_name, int_val));                                   \
            }                                                                                               \
        } while (0)

#define SOC_DNXF_DRV_SUPPORTED_SOC_PROPERTY_STR_DUMP(unit, soc_prop_name, prop_str_enum, int_val)           \
        do {                                                                                                \
            char *str_val;                                                                                  \
            int rv;                                                                                         \
            if (int_val != SOC_DNXF_PROPERTY_UNAVAIL)                                                       \
            {                                                                                               \
                rv = soc_dnxf_property_enum_to_str(unit, soc_prop_name, prop_str_enum, int_val, &str_val);  \
                if (SOC_FAILURE(rv))                                                                        \
                {                                                                                           \
                    str_val = "unknown";                                                                    \
                }                                                                                           \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s=%s\n"), soc_prop_name, str_val));                                   \
            }                                                                                               \
        } while (0)

#define SOC_DNXF_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_STR_DUMP(unit, soc_prop_name, suffix_str, suffix_num, prop_str_enum, int_val)     \
        do {                                                                                                                            \
            char *str_val;                                                                                                              \
            int rv;                                                                                                                     \
            if (int_val != SOC_DNXF_PROPERTY_UNAVAIL)                                                                                    \
            {                                                                                                                           \
                rv = soc_dnxf_property_enum_to_str(unit, soc_prop_name, prop_str_enum, int_val, &str_val);                               \
                if (SOC_FAILURE(rv))                                                                                                    \
                {                                                                                                                       \
                    str_val = "unknown";                                                                                                \
                }                                                                                                                       \
                if (suffix_num != -1)                                                                                                   \
                {                                                                                                                       \
                    LOG_CLI((BSL_META_U(unit,                                                                                           \
                                        "%s_%s%d=%s\n"), soc_prop_name, suffix_str, suffix_num, str_val));                              \
                } else {                                                                                                                \
                    LOG_CLI((BSL_META_U(unit,                                                                                           \
                                        "%s_%s=%s\n"), soc_prop_name, suffix_str, str_val));                                            \
                }                                                                                                                       \
            }                                                                                                                           \
        } while (0)

#define SOC_DNXF_ALLOW_WARMBOOT_WRITE(operation, _rv) \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv)

int soc_dnxf_lock_init(
    int unit);

int soc_dnxf_lock_deinit(
    int unit);

int soc_dnxf_tbl_is_dynamic(
    int unit,
    soc_mem_t mem);

int soc_dnxf_info_config(
    int unit);

int soc_dnxf_chip_type_set(
    int unit,
    uint16 dev_id);

int soc_dnxf_drv_soc_properties_fabric_pipes_read(
    int unit);

int soc_dnxf_drv_soc_properties_general_read(
    int unit);

int soc_dnxf_drv_soc_properties_parse_lane_to_serdes_mapping(
    int unit,
    int nof_lanes,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

int soc_dnxf_drv_soc_properties_port_read(
    int unit);

int soc_dnxf_drv_soc_properties_chip_read(
    int unit);

int soc_dnxf_drv_soc_properties_access_read(
    int unit);

int soc_dnxf_drv_soc_properties_multicast_read(
    int unit);

int soc_dnxf_drv_soc_properties_repeater_read(
    int unit);

int soc_dnxf_drv_soc_properties_fabric_routing_read(
    int unit);

void soc_dnxf_drv_soc_properties_dump(
    int unit);

int soc_dnxf_drv_soc_property_serdes_qrtt_read(
    int unit);

int soc_dnxf_attach(
    int unit);

int soc_dnxf_detach(
    int unit);

int soc_dnxf_init(
    int unit);

int soc_dnxf_deinit(
    int unit);

int soc_dnxf_soft_reset_init(
    int unit);

int soc_dnxf_device_reset(
    int unit,
    int mode,
    int action);

int soc_dnxf_dump(
    int unit,
    char *pfx);

void soc_dnxf_free_cache_memory(
    int unit);

int soc_dnxf_nof_interrupts(
    int unit,
    int *nof_interrupts);

int soc_dnxf_nof_block_instances(
    int unit,
    int block_type,
    int *nof_block_instances);

int soc_dnxf_drv_graceful_shutdown_set(
    int unit,
    soc_pbmp_t active_links,
    int shutdown,
    soc_pbmp_t unisolated_links,
    int isolate_device);

int soc_dnxf_drv_mbist(
    int unit,
    int skip_errors);

int soc_dnxf_drv_update_valid_block_database(
    int unit,
    int block_type,
    int block_number,
    int enable);

shr_error_e soc_dnxf_drv_ms_to_cycles(
    int unit,
    int nof_ms,
    int *nof_cycles);

int soc_dnxf_compiler_64_div_32(
    uint64 x,
    uint32 y,
    uint32 *result);

void soc_dnxf_tbl_mark_cachable(
    int unit);

int soc_dnxf_cache_enable_init(
    int unit);

int soc_dnxf_drv_link_to_block_mapping(
    int unit,
    bcm_port_t link,
    int *block_id,
    int *inner_link,
    int block_type);

int soc_dnxf_drv_block_to_link_mapping(
    int unit,
    int block_id,
    int inner_link,
    int block_type,
    bcm_port_t * link);

int soc_dnxf_drv_block_enable_set(
    int unit,
    int block_type,
    int block_number,
    int enable);

int soc_dnxf_soc_info_get(
    int unit);

int soc_dnxf_init_link_mapping(
    int unit);

int soc_dnxf_init_blocks_config(
    int unit);

int soc_dnxf_soft_init_in(
    int unit);

int soc_dnxf_soft_init_out(
    int unit);

int soc_dnxf_counter_attach(
    int unit);

int soc_dnxf_soc_init_done(
    int unit);

int soc_dnxf_soc_init_done_deinit(
    int unit);

int soc_dnxf_counter_dettach(
    int unit);

shr_error_e soc_dnxf_is_sku(
    int unit,
    int *is_sku);

int soc_dnxf_drv_access_block_convert(
    int unit,
    int access_block_type,
    int *soc_block_type);

int soc_dnxf_link_device_mapping_bmp_get(
    int unit,
    int block_id,
    int block_type,
    soc_pbmp_t * links);

int soc_dnxf_drv_supported_lanes_get(
    int unit,
    soc_pbmp_t * supported_lanes);

int soc_dnxf_block_instance_bmp_per_device_stage_get(
    int unit,
    int block_type,
    soc_dnxf_fabric_device_stage_t block_stage,
    soc_pbmp_t * block_bmp);

shr_error_e soc_dnxf_fabric_link_device_stage_get(
    int unit,
    soc_port_t port,
    int is_rx,
    soc_dnxf_fabric_device_stage_t * link_device_stage);

shr_error_e soc_dnxf_block_active_instance_bmp_get(
    int unit,
    int block_type,
    soc_pbmp_t * block_bmp);

int soc_dnxf_drv_block_to_fmac_lane_mapping(
    int unit,
    int block_id,
    int inner_link,
    int block_type,
    int *fmac_lane);

int soc_dnxf_drv_fmac_lane_to_block_mapping(
    int unit,
    int lane,
    int *block_id,
    int *inner_link,
    int block_type);

int soc_dnxf_drv_fmac_lane_aligned_get(
    int unit,
    int lane_id,
    int *aligned_lane_id);

int soc_dnxf_drv_fmac_lane_real_get(
    int unit,
    int aligned_lane_id,
    int *real_lane_id);

int soc_dnxf_fmac_bmp_to_internal_block_bmp_convert(
    int unit,
    soc_pbmp_t fmac_bmp,
    soc_pbmp_t * internal_block_bmp,
    int block_type,
    int return_only_full);

int soc_dnxf_dma_deinit(
    int unit);

int soc_dnxf_dma_init(
    int unit);

int soc_dnxf_counters_init(
    int unit);

int soc_dnxf_control_init(
    int unit);

int soc_dnxf_control_deinit(
    int unit);

shr_error_e soc_dnxf_drv_virtual_fmac_lane_get(
    int unit,
    int fmac_lane,
    int *virtual_fmac_lane);

shr_error_e soc_dnxf_drv_virtual_block_get(
    int unit,
    int block_type,
    int block_idx,
    int inner_link,
    int *virtual_block_idx,
    int *virtual_inner_link);

shr_error_e soc_dnxf_drv_device_reset_init(
    int unit);

shr_error_e soc_dnxf_drv_device_reset_deinit(
    int unit);

shr_error_e soc_dnxf_drv_interrupts_init(
    int unit);

shr_error_e soc_dnxf_drv_interrupts_deinit(
    int unit);

shr_error_e soc_dnxf_drv_ser_init(
    int unit);

shr_error_e soc_dnxf_drv_block_valid_get(
    int unit,
    int blktype,
    int blockid,
    int *valid);

#endif
