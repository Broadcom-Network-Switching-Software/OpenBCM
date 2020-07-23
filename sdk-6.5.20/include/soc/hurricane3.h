/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane3.h
 */

#ifndef _SOC_HURRICANE3_H_
#define _SOC_HURRICANE3_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <shared/sram.h>

enum soc_hr3_chip_sku_e {
    SOC_HR3_SKU_HURRICANE3 = 0,
    SOC_HR3_SKU_HURRICANE3LITE = 1,
    SOC_HR3_SKU_BUCKHOUND = 2,
    SOC_HR3_SKU_FOXHOUND2 = 3
};

enum soc_hr3_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_HR3_PORT_MODE_QUAD = 0,
    SOC_HR3_PORT_MODE_TRI_012 = 1,
    SOC_HR3_PORT_MODE_TRI_023 = 2,
    SOC_HR3_PORT_MODE_DUAL = 3,
    SOC_HR3_PORT_MODE_SINGLE = 4
};

enum soc_hr3_port_ratio_e {
    SOC_HR3_PORT_RATIO_SINGLE,
    SOC_HR3_PORT_RATIO_SINGLE_XAUI,
    SOC_HR3_PORT_RATIO_DUAL_1_1,
    SOC_HR3_PORT_RATIO_DUAL_2_1,
    SOC_HR3_PORT_RATIO_DUAL_1_2,
    SOC_HR3_PORT_RATIO_DUAL_2_2,
    SOC_HR3_PORT_RATIO_TRI_012_1_1_2,
    SOC_HR3_PORT_RATIO_QUAD,
    SOC_HR3_PORT_RATIO_OCTAL,
    SOC_HR3_PORT_RATIO_NONE,
    SOC_HR3_PORT_RATIO_COUNT
};

enum soc_hr3_port_core_type_e {
    SOC_HR3_PORT_CORE_TYPE_TSC,
    SOC_HR3_PORT_CORE_TYPE_QTC,
    SOC_HR3_PORT_CORE_TYPE_COUNT
};


enum HR3_VLXL_HASH_KEY_TYPE {
    /* WARNING: values given match hardware register; do not modify */
    HR3_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    HR3_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    HR3_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    HR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    HR3_VLXLT_HASH_KEY_TYPE_OVID = 4,
    HR3_VLXLT_HASH_KEY_TYPE_IVID = 5,
    HR3_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    HR3_VLXLT_HASH_KEY_TYPE_HPAE = 7,
    HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID = 8,
    HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG = 9,
    HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG = 10,
    HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID = 11,
    HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID = 12,
    HR3_VLXLT_HASH_KEY_TYPE_PAYLOAD_COUNT
};

#define HR3LITE_LPM_WAR

#ifdef HR3LITE_LPM_WAR
#define NEED_HR3LITE_LPM_SHADOW(_mem)    \
        (((_mem) == L3_DEFIPm) || ((_mem) == L3_DEFIP_DATA_ONLYm) || \
        ((_mem) == L3_DEFIP_HIT_ONLYm))
#define IS_HR3LITE_LPM_SHADOW_HIT_INDEX(_id)    \
        (((_id) >= 32) && ((_id) <= 63))
#define HR3LITE_LPM_HIT_INDEX2(_id)    \
        (IS_HR3LITE_LPM_SHADOW_HIT_INDEX((_id)) ? (512 + ((_id) - 32)) : (-1))
#endif  /* HR3LITE_LPM_WAR*/

#define HR3_L3_IIF_HW_SIZE 512

extern int soc_hurricane3_chip_sku_get(int unit, int *sku);
extern int soc_hurricane3_port_config_init(int unit, uint16 dev_id);
extern int soc_hurricane3_mem_config(int unit, int dev_id);

extern int soc_hurricane3_chip_reset(int unit);
extern int soc_greyhound_tsc_reset(int unit);

extern int soc_hr3_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);

extern void soc_hr3_l2_overflow_interrupt_handler(int unit);
extern int soc_hr3_l2_overflow_intr_disable(int unit);
extern int soc_hr3_l2_overflow_intr_enable(int unit);
extern int soc_hr3_l2_overflow_fill(int unit, uint8 zeros_or_ones);
extern int soc_hr3_l2_overflow_sync_init(int unit);
extern void soc_hr3_l2_overflow_sync(int unit);
extern int soc_hr3_l2_overflow_sync_cleanup(int unit);
extern int soc_hr3_l2_overflow_start(int unit);
extern int soc_hr3_l2_overflow_stop(int unit);


extern soc_functions_t soc_hurricane3_drv_funs;

extern int _soc_hurricane3_features(int unit, soc_feature_t feature);

typedef int (*soc_hr3_oam_handler_t)(int unit, 
    soc_field_t fault_field);
typedef int (*soc_hr3_oam_ser_handler_t)(int unit, 
    soc_mem_t mem, int index);
extern void soc_hr3_oam_ser_handler_register(int unit, soc_hr3_oam_ser_handler_t handler);
extern void soc_hr3_oam_handler_register(int unit, soc_hr3_oam_handler_t handler);
extern void soc_hr3_ser_register(int unit);
extern int soc_hr3_ser_enable_all(int unit, int enable);
extern int soc_hr3_oam_ser_process(int unit, soc_mem_t mem, int index);
extern int _soc_hr3_process_l2_overflow(int unit);
extern int soc_hr3_tcam_ser_init(int unit);
extern int soc_hr3_process_oam_interrupt(int unit);
extern int _soc_hr3_mem_parity_control(int unit, soc_mem_t mem, 
                int copyno, int enable);
#ifdef SER_TR_TEST_SUPPORT
extern void soc_hr3_ser_test_register(int unit);
extern int ser_test_hr3_mem_index_remap(
    int unit, ser_test_data_t *test_data, int *mem_has_ecc);
#endif /* SER_TR_TEST_SUPPORT */


extern uint32 soc_hr3_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, 
                    void *base_entry, uint8 *key);

extern int soc_hr3_vlan_xlate_base_entry_to_key(int unit, uint32 *entry, 
                    uint8 *key);
#ifdef INCLUDE_AVS
extern int soc_hr3_avs_init(int unit);
#endif

extern int soc_hurricane3_sbus_tsc_block(int unit, int phy_port, int *blk);
extern int soc_hurricane3_tsc_reset(int unit);

extern int soc_hurricane3_pgw_rx_fifo_reset(int unit, int port, int reset);

extern int soc_hurricane3_tsc_core_reset(int unit, int port, int reset_delay_us);

#endif /* !_SOC_HURRICANE3_H_ */
