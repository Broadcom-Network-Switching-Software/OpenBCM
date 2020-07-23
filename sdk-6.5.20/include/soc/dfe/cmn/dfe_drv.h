/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef _SOC_DFE_DRV_H
#define _SOC_DFE_DRV_H



#include <soc/chip.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/scache.h>

#include <soc/dcmn/fabric.h>

#include <sal/types.h>

#include <shared/cyclic_buffer.h>

#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_defs.h>

#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/cmn/dfe_config_imp_defs.h>

#include <soc/dfe/fe1600/fe1600_defs.h>

#include <bcm/debug.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/common/debug.h>


#define SOC_DFE_NUM_OF_ROUTES (1024)
#define SOC_DFE_MAX_NUM_OF_ROUTES_PER_ROUTE_GROUP (8)
#define SOC_DFE_MAX_NUM_OF_PRIORITIES (4)
#define SOC_DFE_NUM_OF_FABRIC_PIPE_VALID_CONFIGURATIONS (4)
#define SOC_DFE_MAX_LEN_NAME_PRIORITY_CONFIG (30)
#define SOC_DFE_WARMBOOT_TIME_OUT            (5000000) 
typedef cyclic_buffer_t dfe_cells_buffer_t;
typedef cyclic_buffer_t dfe_captured_buffer_t;





#define DFE_MAX_MODULES 2048

#define SOC_DFE_DRV_MULTIPLIER_MAX_LINK_SCORE               (120)

#define DFE_LINK_INPUT_CHECK(unit, link) \
    if ( SOC_E_NONE != MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_validate_link, (unit, link)) ) { \
         SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("%d isn't a valid link"), link)); \
    }

#define DFE_LINK_INPUT_CHECK_SOCDNX(unit, link) \
    if ( SOC_E_NONE != MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_validate_link, (unit, link)) ) { \
         SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("%d isn't a valid link"), link)); \
    }

#define DFE_IS_PRIORITY_VALID(arg) \
    ((soc_dfe_fabric_priority_0 == arg) \
        || (soc_dfe_fabric_priority_1 == arg) \
        || (soc_dfe_fabric_priority_2 == arg) \
        || (soc_dfe_fabric_priority_3 == arg))

#define SOC_CHECK_MODULE_ID(modid) \
        (modid < DFE_MAX_MODULES ? 1 : 0)

#define DFE_ADD_PORT(ptype, nport) \
            si->ptype.port[si->ptype.num++] = nport; \
            if ( (si->ptype.min < 0) || (si->ptype.min > nport) ) {     \
                si->ptype.min = nport; \
            } \
            if (nport > si->ptype.max) { \
                si->ptype.max = nport; \
            }

#define DFE_REMOVE_DYNAMIC_PORT(ptype, nport) \
            SOC_PBMP_PORT_REMOVE(si->ptype.bitmap, nport);

#define DFE_ADD_DYNAMIC_PORT(ptype, nport) \
            SOC_PBMP_PORT_ADD(si->ptype.bitmap, nport);

#define DFE_REMOVE_DYNAMIC_DISABLED_PORT(ptype, nport) \
            SOC_PBMP_PORT_REMOVE(si->ptype.disabled_bitmap, nport);

#define DFE_ADD_DYNAMIC_DISABLED_PORT(ptype, nport) \
            SOC_PBMP_PORT_ADD(si->ptype.disabled_bitmap, nport);





#define DFE_FABRIC_DEVICE_MODE_DEFAULT                                  soc_dfe_fabric_device_mode_single_stage_fe2
#define DFE_FABRIC_MULTICAST_MODE_DEFAULT                               soc_dfe_multicast_mode_direct
#define DFE_FABRIC_LOAD_BALANCING_MODE_DEFAULT                          soc_dfe_load_balancing_mode_normal
#define DFE_VCS128_UNICAST_PRIORITY_DEFAULT                             soc_dfe_fabric_priority_2
#define DFE_VCS128_UNICAST_PRIORITY_TDM_OVER_PRIMARY_PIPE_DEFAULT       soc_dfe_fabric_priority_1
#define DFE_FABRIC_MAC_BUCKET_FILL_RATE_DEFAULT                         6
#define DFE_PORT_TX_LANE_MAP_DEFAULT                                    0x0123
#define DFE_PORT_RX_LANE_MAP_DEFAULT                                    0x3210
#define DFE_TDM_PRIORITY_OVER_PRIMARY_PIPE                              0x2
#define DFE_TDM_PRIORITY_OVER_SECONDARY_PIPE                            0x3
#define SOC_DFE_FABRIC_TDM_PRIORITY_NONE                                (-2)
#define SOC_DFE_FABRIC_TDM_PRIORITY_DEFAULT                             (3)




typedef struct soc_dfe_modid_local_map_s
{
  uint32 valid;
  uint32 module_id;
} soc_dfe_modid_local_map_t;

typedef struct soc_dfe_modid_group_map_s
{
  uint32 raw[SOC_DFE_MODID_NOF_UINT32_SIZE];
} soc_dfe_modid_group_map_t;

typedef uint32 soc_dfe_fifo_type_handle_t;


typedef struct soc_dfe_mgmt_mode_s {

    int fabric_device_mode; 
    int is_dual_mode; 
    int system_is_vcs_128_in_system; 
    int system_is_dual_mode_in_system; 
    int system_is_single_mode_in_system; 
    int system_is_fe600_in_system;  
    int system_ref_core_clock; 
    int core_clock_speed; 
    int fabric_merge_cells; 
    int fabric_multicast_mode; 
    int fabric_load_balancing_mode; 
    int fabric_TDM_fragment; 
    int fabric_TDM_over_primary_pipe; 
    int fabric_tdm_priority_min; 
    int fabric_optimize_patial_links; 
    int vcs128_unicast_priority; 
    int run_mbist; 
    int fabric_mac_bucket_fill_rate; 
    int serdes_mixed_rate_enable;


    int fe_mc_id_range; 
    int fe_mc_priority_map_enable; 

    int system_contains_multiple_pipe_device;
    soc_dcmn_fabric_pipe_map_t fabric_pipe_mapping; 
    int custom_feature_lab;
    int mesh_topology_fast; 
    int mdio_int_dividend;
    int mdio_int_divisor; 

    int fabric_port_lcpll_in[SOC_DFE_NOF_LCPLL];
    int fabric_port_lcpll_out[SOC_DFE_NOF_LCPLL];

    int fabric_clk_freq_in_quad_26; 
    int fabric_clk_freq_in_quad_35; 
    int fabric_clk_freq_out_quad_26; 
    int fabric_clk_freq_out_quad_35; 


    
    uint32 fabric_cell_fifo_dma_enable; 
    uint32 fabric_cell_fifo_dma_threshold; 
    uint32 fabric_cell_fifo_dma_timeout; 
    uint32 fabric_cell_fifo_dma_buffer_size; 

	int fabric_local_routing_enable; 

} soc_dfe_mgmt_mode_t;

typedef struct soc_dfe_control_s {

  soc_dfe_mgmt_mode_t       cfg;
  soc_fabric_inband_route_t inband_routes[SOC_DFE_NUM_OF_ROUTES];
  dfe_cells_buffer_t        sr_cells_buffer;
  dfe_captured_buffer_t     captured_cells_buffer;
  int                       rx_reception_thread_running;
  int                       rx_reception_thread_closed;
  sal_sem_t                 rx_thread_fifo_dma_semaphore;

} soc_dfe_control_t;

typedef struct soc_dfe_drv_mem_reset_s
{
    soc_mem_t memory;
    soc_reg_above_64_val_t entry_value;
} soc_dfe_drv_mem_reset_t;





typedef struct soc_dfe_drv_dch_default_thresholds_s{

	uint32 fifo_size[SOC_DFE_MAX_NOF_PIPES]; 
    uint32 full_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 llfc_threshold[SOC_DFE_MAX_NOF_PIPES]; 
    uint32 mc_low_prio_threshold[SOC_DFE_MAX_NOF_PIPES];

} soc_dfe_drv_dch_default_thresholds_t;


typedef struct soc_dfe_drv_dcm_default_thresholds_s{

	uint32 fifo_size[SOC_DFE_MAX_NOF_PIPES];
    uint32 prio_0_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 prio_1_threshold[SOC_DFE_MAX_NOF_PIPES]; 
    uint32 prio_2_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 prio_3_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 full_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 almost_full_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 gci_low_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 gci_med_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 gci_high_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 rci_low_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 rci_med_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 rci_high_threshold[SOC_DFE_MAX_NOF_PIPES];

    
    uint32 local_switch_fifo_size[SOC_DFE_MAX_NOF_PIPES];
    uint32 local_switch_almost_full_0_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 local_switch_almost_full_1_threshold[SOC_DFE_MAX_NOF_PIPES];

} soc_dfe_drv_dcm_default_thresholds_t;

typedef struct soc_dfe_drv_dcl_default_thresholds_s{

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

} soc_dfe_drv_dcl_default_thresholds_t;


typedef enum soc_dfe_drv_soft_reset_e
{
    soc_dfe_drv_soft_reset_in,
    soc_dfe_drv_soft_reset_out,
    soc_dfe_drv_soft_reset_inout
} soc_dfe_drv_soft_reset_t ;



#ifdef BCM_WARM_BOOT_SUPPORT
#define SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit) \
            SOC_CONTROL_LOCK(unit); \
            SOC_CONTROL(unit)->scache_dirty = 1; \
            SOC_CONTROL_UNLOCK(unit);
#else
#define SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit) \
            do { \
            } while(0)
#endif

#define SOC_DFE_CONTROL(unit)               ((soc_dfe_control_t *)SOC_CONTROL(unit)->drv)
#define SOC_DFE_CONFIG(unit)                (SOC_DFE_CONTROL(unit)->cfg)
#define SOC_DFE_FABRIC_PIPES_CONFIG(unit)   (SOC_DFE_CONTROL(unit)->cfg.fabric_pipe_mapping)

#define SOC_DFE_IS_REPEATER(unit)       (soc_dfe_fabric_device_mode_repeater == SOC_DFE_CONFIG(unit).fabric_device_mode)
#define SOC_DFE_IS_FE13(unit)           ((soc_dfe_fabric_device_mode_multi_stage_fe13 == SOC_DFE_CONFIG(unit).fabric_device_mode) || \
                                         (soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric == SOC_DFE_CONFIG(unit).fabric_device_mode))
#define SOC_DFE_IS_FE2(unit)            ((soc_dfe_fabric_device_mode_multi_stage_fe2 == SOC_DFE_CONFIG(unit).fabric_device_mode) || \
                                         (soc_dfe_fabric_device_mode_single_stage_fe2 == SOC_DFE_CONFIG(unit).fabric_device_mode))
#define SOC_DFE_IS_MULTISTAGE_FE2(unit) (soc_dfe_fabric_device_mode_multi_stage_fe2 == SOC_DFE_CONFIG(unit).fabric_device_mode)
#define SOC_DFE_IS_MULTISTAGE(unit)     ((soc_dfe_fabric_device_mode_multi_stage_fe13 == SOC_DFE_CONFIG(unit).fabric_device_mode) || \
                                         (soc_dfe_fabric_device_mode_multi_stage_fe2 == SOC_DFE_CONFIG(unit).fabric_device_mode) || \
                                         (soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric == SOC_DFE_CONFIG(unit).fabric_device_mode))

#define SOC_DFE_IS_FE13_SYMMETRIC(unit)  (soc_dfe_fabric_device_mode_multi_stage_fe13 == SOC_DFE_CONFIG(unit).fabric_device_mode)
#define SOC_DFE_IS_FE13_ASYMMETRIC(unit) (soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric == SOC_DFE_CONFIG(unit).fabric_device_mode)

#define DFE_UNIT_LOCK_TAKE(unit) \
    do{ \
        if (BCM_LOCK(unit)) { \
            _bsl_error(_BSL_BCM_MSG("unable to obtain unit lock"));\
            BCM_EXIT; \
        } \
        _lock_taken = 1; \
    }while (0)

#define DFE_UNIT_LOCK_TAKE_SOCDNX(unit) \
    do{ \
        if (BCM_LOCK(unit)) { \
            _bsl_error(_BSL_BCM_MSG("unable to obtain unit lock"));\
            BCM_EXIT; \
        } \
        _lock_taken = 1; \
    }while (0)


#define DFE_UNIT_LOCK_RELEASE(unit) \
    do { \
        if(1 == _lock_taken) { \
            _lock_taken = 0; \
            if (BCM_UNLOCK(unit)) { \
                _bsl_error(_BSL_BCM_MSG("unable to obtain unit release on unit %d\n"), unit);\
                BCM_EXIT; \
            }  \
        } \
    } while (0)

#define DFE_UNIT_LOCK_RELEASE_SOCDNX(unit) \
    do { \
        if(1 == _lock_taken) { \
            _lock_taken = 0; \
            if (BCM_UNLOCK(unit)) { \
                _bsl_error(_BSL_BCM_MSG("unable to obtain unit release on unit %d\n"), unit);\
                BCM_EXIT; \
            }  \
        } \
    } while (0)

#define INT_DEVIDE(num,dev) (num-(num%dev))/dev;


#define SOC_DFE_DRV_INIT_LOG(_unit_, _msg_str_)\
                _bsl_verbose(_ERR_MSG_MODULE_NAME, _unit_, "    + %d: %s\n", _unit_ , _msg_str_)

#define SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, soc_prop_name, suffix, num, int_val)   \
        do {                                                                                                \
            if (int_val != SOC_DFE_PROPERTY_UNAVAIL)                                                        \
            {                                                                                               \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s_%s%d=%d\n"), soc_prop_name, suffix, num, int_val));                 \
            }                                                                                               \
        } while (0) 

#define SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_INT_DUMP(unit, soc_prop_name, suffix, int_val)            \
        do {                                                                                                \
            if (int_val != SOC_DFE_PROPERTY_UNAVAIL)                                                        \
            {                                                                                               \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s_%s=%d\n"), soc_prop_name, suffix, int_val));                        \
            }                                                                                               \
        } while (0) 

#define SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, soc_prop_name, int_val)                           \
        do {                                                                                                \
            if (int_val != SOC_DFE_PROPERTY_UNAVAIL)                                                        \
            {                                                                                               \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s=%d\n"), soc_prop_name, int_val));                                   \
            }                                                                                               \
        } while (0)

#define SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_STR_DUMP(unit, soc_prop_name, prop_str_enum, int_val)            \
        do {                                                                                                \
            char *str_val;                                                                                  \
            int rv;                                                                                         \
            if (int_val != SOC_DFE_PROPERTY_UNAVAIL)                                                        \
            {                                                                                               \
                rv = soc_dfe_property_enum_to_str(unit, soc_prop_name, prop_str_enum, int_val, &str_val);   \
                if (SOC_FAILURE(rv))                                                                        \
                {                                                                                           \
                    str_val = "unknown";                                                                    \
                }                                                                                           \
                LOG_CLI((BSL_META_U(unit,                                                                   \
                                    "%s=%s\n"), soc_prop_name, str_val));                                   \
            }                                                                                               \
        } while (0)

#define SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_STR_DUMP(unit, soc_prop_name, suffix_str, suffix_num, prop_str_enum, int_val)     \
        do {                                                                                                                            \
            char *str_val;                                                                                                              \
            int rv;                                                                                                                     \
            if (int_val != SOC_DFE_PROPERTY_UNAVAIL)                                                                                    \
            {                                                                                                                           \
                rv = soc_dfe_property_enum_to_str(unit, soc_prop_name, prop_str_enum, int_val, &str_val);                               \
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


#define SOC_DFE_ALLOW_WARMBOOT_WRITE(operation, _rv) \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv)





void dfe_local_soc_intr(void *unit);
int soc_dfe_tbl_is_dynamic(int unit, soc_mem_t mem);
soc_driver_t* soc_dfe_chip_driver_find(int unit, uint16 pci_dev_id, uint8 pci_rev_id);
int soc_dfe_info_config(int unit,int dev_id);
int soc_dfe_chip_type_set(int unit, uint16 dev_id);

int soc_dfe_drv_soc_properties_fabric_pipes_read(int unit);
int soc_dfe_drv_soc_properties_general_read(int unit);
int soc_dfe_drv_soc_properties_fabric_cell_read(int unit);
int soc_dfe_drv_soc_properties_port_read(int unit);
int soc_dfe_drv_soc_properties_chip_read(int unit);
int soc_dfe_drv_soc_properties_access_read(int unit);
int soc_dfe_drv_soc_properties_multicast_read(int unit);
int soc_dfe_drv_soc_properties_repeater_read(int unit);
int soc_dfe_drv_soc_properties_fabric_routing_read(int unit);
void soc_dfe_drv_soc_properties_dump(int unit);
int _soc_dfe_drv_soc_property_serdes_qrtt_read(int unit);
    
int soc_dfe_attach(int unit);
int soc_dfe_detach(int unit);
int soc_dfe_init(int unit, int reset);
int soc_dfe_deinit(int unit);
int soc_dfe_device_reset(int unit, int mode, int action);
int soc_dfe_dump(int unit, char *pfx);
void soc_dfe_chip_dump(int unit, soc_driver_t *d);

int soc_dfe_nof_interrupts(int unit, int* nof_interrupts);
int soc_dfe_nof_block_instances(int unit,  soc_block_types_t block_types, int *nof_block_instances);
int soc_dfe_avs_value_get(int unit, uint32 *avs_value);
int soc_dfe_drv_graceful_shutdown_set(int unit, soc_pbmp_t active_links, int shutdown, soc_pbmp_t unisolated_links, int isolate_device);
int soc_dfe_drv_mbist(int unit, int skip_errors);
int soc_dfe_drv_update_valid_block_database(int unit, int block_type, int block_number, int enable);
int soc_dfe_drv_link_to_dch_block(int unit, int link, int *block_instance, int *inner_link);


int soc_dfe_compiler_64_div_32(uint64 x, uint32 y, uint32 *result);



#endif  
