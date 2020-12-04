/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        greyhound2.h
 */

#ifndef _SOC_GREYHOUND2_H_
#define _SOC_GREYHOUND2_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <bcm/types.h>
#include <shared/sram.h>

enum soc_gh2_chip_sku_e {
    SOC_GH2_SKU_HURRICANE3MG = 0,
    SOC_GH2_SKU_QUARTZ       = 1,
    SOC_GH2_SKU_EMULATION    = 2
};

enum gh2_l2_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    GH2_L2_HASH_KEY_TYPE_BRIDGE = 0,
    GH2_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    GH2_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    GH2_L2_HASH_KEY_TYPE_VFI = 3,
    GH2_L2_HASH_KEY_TYPE_VIF = 5,
    GH2_L2_HASH_KEY_TYPE_PE_VID = 6,
    GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST = 7,
    GH2_L2_HASH_KEY_TYPE_COUNT
};

enum soc_gh2_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_GH2_PORT_MODE_QUAD = 0,
    SOC_GH2_PORT_MODE_TRI_012 = 1,
    SOC_GH2_PORT_MODE_TRI_023 = 2,
    SOC_GH2_PORT_MODE_DUAL = 3,
    SOC_GH2_PORT_MODE_SINGLE = 4,
    SOC_GH2_PORT_MODE_TDM_DISABLE = 5
};

enum soc_gh2_port_ratio_e {
    SOC_GH2_PORT_RATIO_SINGLE,      /* portN: 4 lanes */
    SOC_GH2_PORT_RATIO_SINGLE_XAUI, /* portN: 4 lanes, XAUI mode */
    SOC_GH2_PORT_RATIO_DUAL_2_2,    /* portN: 2 lanes, portN+1: 2 lanes */
    SOC_GH2_PORT_RATIO_TRI_012,     /* portN: 1 lane,  portN+1: 1 lane,
                                       portN+2: 2 lanes */
    SOC_GH2_PORT_RATIO_TRI_023,     /* portN: 2 lanes, portN+1: 1 lane,
                                       portN+2: 1 lane */
    SOC_GH2_PORT_RATIO_QUAD,        /* portN: 1 lane, portN+1: 1 lane,
                                       portN+2: 1 lane, portN+3: 1 lane */
    SOC_GH2_PORT_RATIO_COUNT
};

enum soc_gh2_port_core_type_e {
    SOC_GH2_PORT_CORE_TYPE_TSC,
    SOC_GH2_PORT_CORE_TYPE_QTC,
    SOC_GH2_PORT_CORE_TYPE_COUNT
};

enum gh2_vlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    GH2_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    GH2_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    GH2_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    GH2_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    GH2_VLXLT_HASH_KEY_TYPE_OVID = 4,
    GH2_VLXLT_HASH_KEY_TYPE_IVID = 5,
    GH2_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    GH2_VLXLT_HASH_KEY_TYPE_HPAE = 7,
    GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID = 8,
    GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG = 9,
    GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG = 10,
    GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID = 11,
    GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID = 12,
    GH2_VLXLT_HASH_KEY_TYPE_VNID = 13,
    GH2_VLXLT_HASH_KEY_TYPE_PAYLOAD_COUNT
};

/* MMU port index 58~65 in GH2 is with 64 COSQ */
#define SOC_GH2_64Q_MMU_PORT_IDX_MIN 58
#define SOC_GH2_64Q_MMU_PORT_IDX_MAX 65

/* Per port COSQ and QGROUP number */
#define SOC_GH2_QGROUP_PER_PORT_NUM 8
#define SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM 8
#define SOC_GH2_QLAYER_COSQ_PER_PORT_NUM (SOC_GH2_QGROUP_PER_PORT_NUM * \
                                          SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)

#define SOC_GH2_LEGACY_QUEUE_NUM 8
#define SOC_GH2_2LEVEL_QUEUE_NUM SOC_GH2_QLAYER_COSQ_PER_PORT_NUM

extern void soc_esw_timesync_intr(int unit);

extern int
soc_gh2_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key);

extern int
soc_gh2_64q_port_check(int unit, soc_port_t port, int *is_64q_port);
extern int
soc_gh2_mmu_bucket_qlayer_index(int unit, int p, int q, int *idx);
extern int
soc_gh2_mmu_bucket_qgroup_index(int unit, int p, int g, int *idx);

extern void soc_gh2_ser_cmic_interrupt_enable(int unit, int enable);
extern void soc_gh2_ser_fail(int unit);
extern int soc_gh2_ser_process_all(int unit, uint32 cmic_rval);
extern int soc_gh2_ser_enable_all(int unit, int enable);
extern int soc_gh2_oam_ser_process(int unit, soc_mem_t mem, int index);
extern int soc_gh2_chip_sku_get(int unit, int *sku);
extern int soc_gh2_tcam_ser_init(int unit);
extern int soc_gh2_oam_ser_process(int unit, soc_mem_t mem, int index);
extern int _soc_gh2_process_l2_overflow(int unit);
extern int _soc_gh2_mem_parity_control(int unit, soc_mem_t mem,
                int copyno, int enable);
#ifdef SER_TR_TEST_SUPPORT
extern void soc_gh2_ser_test_register(int unit);
extern int ser_test_greyhound2_mem_index_remap(
    int unit, ser_test_data_t *test_data, int *mem_has_ecc);
#endif /* SER_TR_TEST_SUPPORT */

extern int soc_greyhound2_port_config_init(int unit, uint16 dev_id);
extern int soc_greyhound2_mem_config(int unit, int dev_id);
extern int soc_greyhound2_chip_reset(int unit);
extern int soc_greyhound2_sbus_tsc_block(int unit, int phy_port, int *blk);
extern int soc_greyhound2_tsc_reset(int unit);
extern int _soc_greyhound2_features(int unit, soc_feature_t feature);

extern int
soc_gh2_temperature_monitor_get(int unit, int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);


extern void soc_hu3_l2_overflow_interrupt_handler(int unit);
extern int soc_hu3_l2_overflow_disable(int unit);
extern int soc_hu3_l2_overflow_enable(int unit);
extern int soc_hu3_l2_overflow_fill(int unit, uint8 zeros_or_ones);
extern int soc_hu3_l2_overflow_stop(int unit);
extern int soc_hu3_l2_overflow_start(int unit);
extern int soc_hu3_l2_overflow_sync(int unit);

typedef int (*soc_gh2_cosq_event_handler_t)(int unit);
extern int
soc_gh2_cosq_event_handler_register(int unit,
                                    soc_gh2_cosq_event_handler_t handler);
typedef int (*soc_gh2_tsn_event_handler_t)(int, uint8, uint8, uint8);
extern int
soc_gh2_tsn_event_handler_register(int unit,
                                   soc_gh2_tsn_event_handler_t handler);
typedef int (*soc_gh2_taf_event_handler_t)(int unit
    );
extern int
soc_gh2_taf_event_handler_register(int unit,
                                    soc_gh2_taf_event_handler_t handler);


extern soc_functions_t soc_greyhound2_drv_funs;

extern soc_error_t
soc_greyhound2_granular_speed_get(int unit, soc_port_t port, int *speed);

extern int 
soc_greyhound2_port_speed_update(int unit, soc_port_t port, int speed);
extern int
soc_greyhound2_port_hg_speed_get(int unit, soc_port_t port, int *speed);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_gh2_port_init_speed_scache_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
soc_greyhound2_bond_info_init(int unit);
typedef int (*soc_gh2_oam_handler_t)(int unit,
        soc_field_t fault_field);
typedef int (*soc_gh2_oam_ser_handler_t)(int unit,
        soc_mem_t mem, int index);
extern void soc_gh2_oam_ser_handler_register(int unit, soc_gh2_oam_ser_handler_t handler);
extern void soc_gh2_oam_handler_register(int unit, soc_gh2_oam_handler_t handler);

extern int soc_greyhound2_pgw_encap_field_modify(int unit, 
                                soc_port_t lport, 
                                soc_field_t field, 
                                uint32 val);
extern int soc_greyhound2_pgw_encap_field_get(int unit, 
                                soc_port_t lport, 
                                soc_field_t field, 
                                uint32 *val);

extern int soc_gh2_tdm_size_get(int unit, int *tdm_size);
extern int soc_gh2_tdm_size_set(int unit, int tdm_size);

/* Library-private functions exported from hash.c */
extern uint32
soc_gh2_vlan_xlate_hash(
    int unit,
    int hash_sel,
    int key_nbits,
    void *base_entry,
    uint8 *key);

extern int
soc_gh2_vlan_xlate_base_entry_to_key(
    int unit,
    uint32 *entry,
    uint8 *key);

/* The first physical port number of each core */
#define GH2_PHY_PORT_OFFSET_QTC0 26
#define GH2_PHY_PORT_OFFSET_QTC1 42
#define GH2_PHY_PORT_OFFSET_TSCE0 58
#define GH2_PHY_PORT_OFFSET_TSCE1 62
#define GH2_PHY_PORT_OFFSET_TSCE2 66
#define GH2_PHY_PORT_OFFSET_TSCE3 70
#define GH2_PHY_PORT_OFFSET_TSCE4 74
#define GH2_PHY_PORT_OFFSET_TSCE5 78
#define GH2_PHY_PORT_OFFSET_TSCE6 82
#define GH2_PHY_PORT_OFFSET_TSCF0 86

/* Validate if port configuration require TDM reallocation. */
extern int
soc_greyhound2_auto_tdm_realloc(int unit);

#endif /* !_SOC_GREYHOUND2_H_ */
