/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firelight.h
 */

#ifndef _SOC_FIRELIGHT_H_
#define _SOC_FIRELIGHT_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <bcm/types.h>
#include <shared/sram.h>

enum fl_l2_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    FL_L2_HASH_KEY_TYPE_BRIDGE = 0,
    FL_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT = 1,
    FL_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT = 2,
    FL_L2_HASH_KEY_TYPE_VFI = 3,
    FL_L2_HASH_KEY_TYPE_VIF = 5,
    FL_L2_HASH_KEY_TYPE_PE_VID = 6,
    FL_L2_HASH_KEY_TYPE_VFI_MULTICAST = 7,
    FL_L2_HASH_KEY_TYPE_COUNT
};

enum soc_fl_port_mode_e {
    /* WARNING: values given match hardware register; do not modify */
    SOC_FL_PORT_MODE_QUAD = 0,
    SOC_FL_PORT_MODE_TRI_012 = 1,
    SOC_FL_PORT_MODE_TRI_023 = 2,
    SOC_FL_PORT_MODE_DUAL = 3,
    SOC_FL_PORT_MODE_SINGLE = 4,
    SOC_FL_PORT_MODE_TDM_DISABLE = 5
};

enum soc_fl_port_ratio_e {
    SOC_FL_PORT_RATIO_SINGLE,      /* portN: 4 lanes */
    SOC_FL_PORT_RATIO_SINGLE_XAUI, /* portN: 4 lanes, XAUI mode */
    SOC_FL_PORT_RATIO_DUAL_2_2,    /* portN: 2 lanes, portN+1: 2 lanes */
    SOC_FL_PORT_RATIO_TRI_012,     /* portN: 1 lane,  portN+1: 1 lane,
                                       portN+2: 2 lanes */
    SOC_FL_PORT_RATIO_TRI_023,     /* portN: 2 lanes, portN+1: 1 lane,
                                       portN+2: 1 lane */
    SOC_FL_PORT_RATIO_QUAD,        /* portN: 1 lane, portN+1: 1 lane,
                                       portN+2: 1 lane, portN+3: 1 lane */
    SOC_FL_PORT_RATIO_COUNT
};

enum soc_fl_port_core_type_e {
    SOC_FL_PORT_CORE_TYPE_TSCF,
    SOC_FL_PORT_CORE_TYPE_TSCE,
    SOC_FL_PORT_CORE_TYPE_QTC,
    SOC_FL_PORT_CORE_TYPE_COUNT
};

enum fl_vlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    FL_VLXLT_HASH_KEY_TYPE_IVID_OVID = 0,
    FL_VLXLT_HASH_KEY_TYPE_OTAG = 1,
    FL_VLXLT_HASH_KEY_TYPE_ITAG = 2,
    FL_VLXLT_HASH_KEY_TYPE_VLAN_MAC = 3,
    FL_VLXLT_HASH_KEY_TYPE_OVID = 4,
    FL_VLXLT_HASH_KEY_TYPE_IVID = 5,
    FL_VLXLT_HASH_KEY_TYPE_PRI_CFI = 6,
    FL_VLXLT_HASH_KEY_TYPE_HPAE = 7,
    FL_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID = 8,
    FL_VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG = 9,
    FL_VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG = 10,
    FL_VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID = 11,
    FL_VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID = 12,
    FL_VLXLT_HASH_KEY_TYPE_VNID = 13,
    FL_VLXLT_HASH_KEY_TYPE_PAYLOAD_COUNT
};

#define SOC_FL_PORT_BLOCK_BASE_PORT(port)                           \
        SOC_INFO(unit).port_l2p_mapping[port] <= 49 ?               \
        (2 + ((SOC_INFO(unit).port_l2p_mapping[port] - 2) & ~0xF)) :\
        (2 + ((SOC_INFO(unit).port_l2p_mapping[port] - 2) & ~0x3));

/* MMU port index 58~65 in Firelight is with 64 COSQ */
#define SOC_FL_64Q_MMU_PORT_IDX_MIN 58
#define SOC_FL_64Q_MMU_PORT_IDX_MAX 65

/* Per port COSQ and QGROUP number */
#define SOC_FL_QGROUP_PER_PORT_NUM 8
#define SOC_FL_QLAYER_COSQ_PER_QGROUP_NUM 8
#define SOC_FL_QLAYER_COSQ_PER_PORT_NUM (SOC_FL_QGROUP_PER_PORT_NUM * \
                                          SOC_FL_QLAYER_COSQ_PER_QGROUP_NUM)

#define SOC_FL_LEGACY_QUEUE_NUM 8
#define SOC_FL_2LEVEL_QUEUE_NUM SOC_FL_QLAYER_COSQ_PER_PORT_NUM

/* extern int
soc_fl_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key); */

extern int
soc_fl_64q_port_check(int unit, soc_port_t port, int *is_64q_port);
extern int
soc_fl_mmu_bucket_qlayer_index(int unit, int p, int q, int *idx);
extern int
soc_fl_mmu_bucket_qgroup_index(int unit, int p, int g, int *idx);

typedef struct _soc_fl_ser_route_block_s{
    uint32          cmic_bit;
    soc_block_t     blocktype;
    uint8           id;
    soc_reg_t       enable_reg;
    soc_reg_t       status_reg;
    void            *info;
} _soc_fl_ser_route_block_t;

typedef enum {
    _SOC_PARITY_TYPE_NONE,
    _SOC_PARITY_TYPE_ECC,
    _SOC_PARITY_TYPE_ECC_DUAL,
    _SOC_PARITY_TYPE_PARITY,
    _SOC_PARITY_TYPE_PARITY_DUAL,
    _SOC_PARITY_TYPE_COUNTER,
    _SOC_PARITY_TYPE_PORT_XL,
    _SOC_PARITY_TYPE_MMU_SER,
    _SOC_PARITY_TYPE_MMU_ECC,
    _SOC_PARITY_TYPE_MMU_ECC_CBPH,/* 2-level ecc */
    _SOC_PARITY_TYPE_MMU_ECC_XQ, /* 2-level ecc */
    _SOC_PARITY_TYPE_MMU_PAR_E2EFC, /* par */
    _SOC_PARITY_TYPE_MMU_PAR_WRED, /* 2-level par */
    _SOC_PARITY_TYPE_MMU_ECC_IPMCGROUP, /* 2-level ecc_correctable */
    _SOC_PARITY_TYPE_MMU_ECC_IPMCVLAN, /* ecc_correctable */
    _SOC_PARITY_TYPE_MACSEC_ECC,
    _SOC_PARITY_TYPE_SER
} _soc_fl_ser_info_type_t;

typedef struct _soc_fl_ser_reg_s {
    soc_reg_t reg;
    char      *str;
    soc_mem_t mem;
} _soc_fl_ser_reg_t;

typedef struct _soc_fl_ser_info_s {
    _soc_fl_ser_info_type_t    type;
    struct _soc_fl_ser_info_s  *info; /* used for MMU */
    /* registers sets for parity/ecc group related */
    soc_field_t                 group_reg_enable_field;
    soc_field_t                 group_reg_status_field;
    soc_mem_t                   mem;
    char                        *mem_str;
    /* registers sets for parity/ecc enable */
    soc_reg_t                   enable_reg;
    soc_field_t                 enable_field;
    soc_field_t                 *enable_field_list;
    /* registers sets for parity/ecc interrupt related */
    soc_reg_t                   intr_enable_reg;
    soc_field_t                 intr_enable_field;
    soc_field_t                 *intr_enable_field_list;
    soc_reg_t                   intr_status_reg;
    _soc_fl_ser_reg_t          *intr_status_reg_list;
    soc_field_t                 intr_status_field;
    soc_reg_t                   intr_clr_reg;
    soc_field_t                 intr_clr_field;
    soc_reg_t                   intr2_valid_reg;
    _soc_fl_ser_reg_t          *intr2_valid_reg_list;
    soc_reg_t                   intr2_multi_err_reg;
    soc_reg_t                   intr2_clr_reg;
    uint32                      sku_skip;
}_soc_fl_ser_info_t;

/* Used by Macsec code. */
typedef struct soc_fl_ecc_data_s {
    int block_info_idx;
    int port;
    _soc_fl_ser_info_t *info;
    char *mem_str;
    soc_block_t blocktype;
    int parity_type;
} soc_fl_ecc_data_t;

extern void soc_fl_ser_cmic_interrupt_enable(int unit, int enable);
extern void soc_fl_ser_fail(int unit);
extern int soc_fl_ser_enable_all(int unit, int enable);
extern int soc_fl_oam_ser_process(int unit, soc_mem_t mem, int index);
/* extern int soc_fl_chip_sku_get(int unit, int *sku);  */
extern int soc_fl_tcam_ser_init(int unit);
extern int _soc_fl_mem_parity_control(int unit, soc_mem_t mem,
                                      int copyno, int enable);
extern int
soc_fl_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                        soc_reg_t group_reg, uint64 *group_rval,
                        _soc_fl_ser_info_t *info_list,
                        soc_mem_t mem, int enable);
extern _soc_fl_ser_route_block_t *soc_fl_ser_route_blocks_get(int rb);
#ifdef SER_TR_TEST_SUPPORT
extern void soc_fl_ser_test_register(int unit);
extern int ser_test_fl_mem_index_remap(int unit, ser_test_data_t *test_data,
                                       int *mem_has_ecc);
extern int soc_fl_ser_inject_error(int unit, uint32 flags, soc_mem_t mem,
                                   int pipeTarget, int block, int index);
extern int soc_fl_ser_test_mem(int unit, soc_mem_t mem,
                               _soc_ser_test_t test_type, int cmd);
extern int soc_fl_ser_test(int unit, _soc_ser_test_t test_type);
extern soc_error_t
ser_fl_ser_error_injection_support(int unit, soc_mem_t mem, int pipe_target);
extern _soc_generic_ser_info_t *soc_fl_ser_tcam_ser_info_get(int unit);
#endif /* SER_TR_TEST_SUPPORT */

extern int soc_firelight_port_config_init(int unit, uint16 dev_id);
extern int soc_firelight_mem_config(int unit, int dev_id);
extern int soc_firelight_chip_reset(int unit);
extern int soc_firelight_sbus_tsc_block(int unit, int phy_port, int *blk);
extern int soc_firelight_tsc_reset(int unit);
extern int soc_fl_macsec_port_reg_blk_idx_get(int unit, int port, int blktype,
    int *block, int *index);
extern int soc_fl_macsec_idx_get(int unit, int port);
extern int soc_fl_macsec_ctrl_reset(int unit, int port, int enable);
extern int _soc_firelight_features(int unit, soc_feature_t feature);

extern int
soc_fl_temperature_monitor_get(int unit, int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count);


extern void soc_fl_lp_intr_dpc(void *unit_vp, void *d1, void *d2,
                               void *d3, void *d4);
extern int soc_fl_ser_process_all(int unit, int reg_type, int bit);
extern void soc_hu3_l2_overflow_interrupt_handler(int unit);
extern int soc_hu3_l2_overflow_disable(int unit);
extern int soc_hu3_l2_overflow_enable(int unit);
extern int soc_hu3_l2_overflow_fill(int unit, uint8 zeros_or_ones);
extern int soc_hu3_l2_overflow_stop(int unit);
extern int soc_hu3_l2_overflow_start(int unit);
extern int soc_hu3_l2_overflow_sync(int unit);
extern void soc_fl_port_qsgmii_mode_get(int unit, soc_port_t port, int* qsgmii_mode);
extern void soc_fl_port_qsgmii_mode_get_by_qtc_num(int unit, int qtc_num, int* qsgmii_mode);



typedef int (*soc_fl_cosq_event_handler_t)(int unit);
extern int
soc_fl_cosq_event_handler_register(int unit,
                                   soc_fl_cosq_event_handler_t handler);
typedef int (*soc_fl_tsn_event_handler_t)(int, uint8, uint8, uint8);
extern int
soc_fl_tsn_event_handler_register(int unit, soc_fl_tsn_event_handler_t handler);
extern void
soc_fl_process_macsec_intr(int unit);
typedef int (*soc_fl_taf_event_handler_t)(int unit
    );
extern int
soc_fl_taf_event_handler_register(int unit, soc_fl_taf_event_handler_t handler);
typedef void (*soc_fl_macsec_handler_t)(int unit);
extern int
soc_fl_macsec_handler_register(int unit,
                               soc_fl_macsec_handler_t handler);
extern soc_functions_t soc_firelight_drv_funs;

extern soc_error_t
soc_firelight_granular_speed_get(int unit, soc_port_t port, int *speed);

extern int
soc_firelight_port_speed_update(int unit, soc_port_t port, int speed);
extern int
soc_firelight_port_hg_speed_get(int unit, soc_port_t port, int *speed);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int soc_fl_port_init_speed_scache_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
soc_firelight_bond_info_init(int unit);
typedef int (*soc_fl_oam_handler_t)(int unit,
        soc_field_t fault_field);
typedef int (*soc_fl_oam_ser_handler_t)(int unit,
        soc_mem_t mem, int index);
extern void soc_fl_oam_ser_handler_register(int unit, soc_fl_oam_ser_handler_t handler);
extern void soc_fl_oam_handler_register(int unit, soc_fl_oam_handler_t handler);

extern int soc_firelight_pgw_encap_field_modify(int unit, 
                                soc_port_t lport, 
                                soc_field_t field, 
                                uint32 val);
extern int soc_firelight_pgw_encap_field_get(int unit, 
                                soc_port_t lport, 
                                soc_field_t field, 
                                uint32 *val);
extern int soc_firelight_port_mode_get(int unit, int logical_port, int *mode);

extern int soc_fl_tdm_size_get(int unit, int *tdm_size);
extern int soc_fl_tdm_size_set(int unit, int tdm_size);

extern int soc_firelight_support_speeds(int unit, int port, uint32 *speed_mask);

/* Library-private functions exported from hash.c */
/* extern uint32
soc_fl_vlan_xlate_hash(
    int unit,
    int hash_sel,
    int key_nbits,
    void *base_entry,
    uint8 *key);

extern int
soc_fl_vlan_xlate_base_entry_to_key(
    int unit,
    uint32 *entry,
    uint8 *key); */

/* The first physical port number of each core */
#define FL_PHY_PORT_OFFSET_TSCE0 50
#define FL_PHY_PORT_OFFSET_TSCE1 54
#define FL_PHY_PORT_OFFSET_TSCE2 58
#define FL_PHY_PORT_OFFSET_TSCF0 62
#define FL_PHY_PORT_OFFSET_TSCF1 66
#define FL_PHY_PORT_OFFSET_TSCF2 70
#define FL_PHY_PORT_OFFSET_TSCF3 74

#define _FL_PORTS_PER_PBLK             4
#define _FL_PORTS_PER_PMQ_PBLK         16

#define FL_NUM_PHY_PORT                78
#define FL_MIN_PHY_PORT_NUM            2

#endif /* !_SOC_FIRELIGHT_H_ */
