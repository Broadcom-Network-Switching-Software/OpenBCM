/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:      er_tcam_h
 */

#ifndef _SOC_ER_TCAM_H
#define _SOC_ER_TCAM_H

#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/triumph3.h>
#endif

#define TCAM_SIZE_18MB          0x00
#define TCAM_REQ_RSP_LATENCY    0x00
#define TCAM_BLOCK_ENTRIES  0x800
#define MAX_TCAMS 2

typedef enum soc_tcam_partition_type_e {
    TCAM_PARTITION_RAW,
    TCAM_PARTITION_ACL,          /* Easyrider */
    TCAM_PARTITION_LPM,          /* Easyrider */
    TCAM_PARTITION_FWD_L2,       /* Triumph */
    TCAM_PARTITION_FWD_L2_WIDE,  /* Triumph3 */
    TCAM_PARTITION_FWD_IP4_UCAST,/* Triumph3 */
    TCAM_PARTITION_FWD_IP4_UCAST_WIDE, /* Triumph3 */
    TCAM_PARTITION_FWD_IP4,      /* Triumph */
    TCAM_PARTITION_FWD_IP6U,     /* Triumph */
    TCAM_PARTITION_FWD_IP6_128_UCAST, /* Triumph3 */
    TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE, /* Triumph3 */
    TCAM_PARTITION_FWD_IP6,      /* Triumph */
    TCAM_PARTITION_ACL_L2,       /* Triumph */
    TCAM_PARTITION_ACL_IP4,      /* Triumph */
    TCAM_PARTITION_ACL_IP6S,     /* Triumph */
    TCAM_PARTITION_ACL_IP6F,     /* Triumph */
    TCAM_PARTITION_ACL_L2C,      /* Triumph */
    TCAM_PARTITION_ACL_IP4C,     /* Triumph */
    TCAM_PARTITION_ACL_IP6C,     /* Triumph */
    TCAM_PARTITION_ACL_L2IP4,    /* Triumph */
    TCAM_PARTITION_ACL_L2IP6,    /* Triumph */
    TCAM_PARTITION_DEV0_TBL72,   /* Triumph */
    TCAM_PARTITION_DEV0_TBL144,  /* Triumph */
    TCAM_PARTITION_DEV1_TBL72,   /* Triumph */
    TCAM_PARTITION_DEV1_TBL144,  /* Triumph */
    TCAM_PARTITION_ACL80,        /* Triumph3 */
    TCAM_PARTITION_ACL160,       /* Triumph3 */
    TCAM_PARTITION_ACL320,       /* Triumph3 */
    TCAM_PARTITION_ACL480,       /* Triumph3 */
    TCAM_PARTITION_FWD_L2_DUP,   /* Triumph3 */
    TCAM_PARTITION_FWD_IP4_DUP,  /* Triumph3 */
    TCAM_PARTITION_FWD_IP6U_DUP, /* Triumph3 */
    TCAM_PARTITION_FWD_IP6_DUP,  /* Triumph3 */
    TCAM_PARTITION_COUNT
} soc_tcam_partition_type_t;

#define TCAM_PARTITION_FLAGS_TYPE_L2          0x01
#define TCAM_PARTITION_FLAGS_TYPE_L3          0x02
#define TCAM_PARTITION_FLAGS_TYPE_ACL         0x03
#define TCAM_PARTITION_FLAGS_TYPE_MASK        0x03
#define TCAM_PARTITION_FLAGS_AD_IN_SRAM0      0x08 /* forward or policy tbl */
#define TCAM_PARTITION_FLAGS_AD_IN_SRAM1      0x10 /* forward or policy tbl */
#define TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM0 0x20
#define TCAM_PARTITION_FLAGS_COUNTER_IN_SRAM1 0x40

/*
 * For TR3. If this flag is set, then it indicates multiple partitions should
 * co-exist in same superblock if necessary. E.g, L2, L2 wide.
 */
#define TCAM_PARTITION_FLAGS_COALESCE         0x80
#define TCAM_PARTITION_FLAGS_NO_AD            0x100

typedef struct soc_tcam_partition_s {
    int num_entries;
    int num_entries_include_pad;
    int tcam_width_shift;
    int tcam_base;
    int sram_width_shift;
    int sram_base;
    int counter_base;
    int hbit_base;
    uint32 flags;
#ifdef BCM_TRIUMPH3_SUPPORT
    soc_et_pa_xlat_t ad_info;
#endif
} soc_tcam_partition_t;

typedef struct soc_tcam_info_s {
    int subtype;
    int subtype_override;
    int num_tcams;
    int num_tcams_override;
    int monolith_dev;
    int mode;
    int tcam_freq;
    int sram_freq;
    int mdio_port_tcam0;  /* TR3 tcam mdio port no. */
    int mdio_port_tcam1;
    soc_tcam_partition_t partitions[TCAM_PARTITION_COUNT];
    int tcam_init_done[MAX_TCAMS];
    int blks_per_dev;
} soc_tcam_info_t;

typedef struct er_tcam_config_s {
    int tcams;
    int acl_entries;
    int lpm_entries;
} er_tcam_config_t;

#ifdef BCM_EASYRIDER_SUPPORT

extern er_tcam_config_t configs[];

extern int er_tcam_init_type1(int u, int ext_table_cfg);
extern int er_tcam_init_type2(int u, int ext_table_cfg);
extern int soc_er_tcam_type1_read_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data, int * valid);
extern int soc_er_tcam_type2_read_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data, int * valid);
extern int soc_er_tcam_type1_write_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data);
extern int soc_er_tcam_type2_write_entry(int u, int cfg, int part, int index,
        uint32 * mask, uint32 * data);
extern int soc_er_tcam_type1_set_valid(int u, int cfg, int part, int index,
        int valid);
extern int soc_er_tcam_type2_set_valid(int u, int cfg, int part, int index,
        int valid);
extern int soc_er_tcam_type1_search_entry(int u, int cfg, int part, int upper,
                                          uint32 *data, int *index);
extern int soc_er_tcam_type1_write_ib(int unit, uint32 addr, uint32 data);
extern int soc_er_tcam_type1_write_ima(int unit, uint32 addr,
                                      uint32 d0_msb, uint32 d1_lsb);
extern int soc_er_tcam_type1_read_ima(int unit, uint32 addr,
                                      uint32 *d0_msb, uint32 *d1_lsb);
extern int soc_er_tcam_type1_write_dbreg(int unit, uint32 addr,
                                         uint32 d0_msb, uint32 d1_lsb);
extern int soc_er_tcam_type1_read_dbreg(int unit, uint32 addr,
                                        uint32 *d0_msb, uint32 *d1_lsb);
extern int soc_er_tcam_type2_parity_diagnose(int unit);
extern int soc_er_tcam_type1_memtest(int unit, uint32 pattern1,
                                     uint32 pattern2);

#endif

#ifdef BCM_TRIUMPH_SUPPORT

#define TCAM_TR_WORDS_PER_ENTRY     3

enum {
  TCAM_TR_OP_WRITE = 0,
  TCAM_TR_OP_SINGLE_SEARCH1 = 1,
  TCAM_TR_OP_SINGLE_SEARCH0 = 2,
  TCAM_TR_OP_PARALLEL_SEARCH = 3,
  TCAM_TR_OP_READ = 4,
  TCAM_TR_OP_READ_MASK = 5
};

enum {
  TCAM_TR3_OP_NOP = 0,
  TCAM_TR3_OP_WRITE = 1,
  TCAM_TR3_OP_REG_DB_X_READ = 2,
  TCAM_TR3_OP_DB_Y_READ = 3,
  TCAM_TR3_OP_CB_WR_CMP1 = 4,
  TCAM_TR3_OP_CB_WR_CMP2 = 5,
  TCAM_TR3_OP_CB_WRITE = 6
};

extern int tr3_tcam_init(int unit);
extern int soc_tr3_tcam_write_entry(int unit, int part, int index, uint32 *mask,
                                    uint32 *data, int valid);
extern int soc_tr3_tcam_read_entry(int unit, int part, int index, uint32 *mask,
                                    uint32 *data, int *valid);
extern int soc_tr3_tcam_search_entry(int unit, int part1, int part2, 
                                     uint32 *data, int *index1, int *index2);
extern int soc_tr3_tcam_write_reg(int unit, uint32 addr, uint32 d0_msb,
                                       uint32 d1, uint32 d2_lsb);
extern int soc_tr3_tcam_read_reg(int unit, uint32 addr, uint32 *d0_msb,
                                      uint32 *d1, uint32 *d2_lsb);

extern int tr_tcam_init_type1(int unit);
extern int soc_tr_tcam_type1_write_entry(int unit, int part, int index,
                                         uint32 *mask, uint32 *data);
extern int soc_tr_tcam_type1_read_entry(int unit, int part, int index,
                                        uint32 *mask, uint32 *data,
                                        int *valid);
extern int soc_tr_tcam_type1_delete_entry(int unit, int part, int index);
extern int soc_tr_tcam_type1_search_entry(int unit, int part0, int part1,
                                          uint32 *data, int *index0,
                                          int *index1);
extern int soc_tr_tcam_type1_write_reg(int unit, uint32 addr, uint32 d0_msb,
                                       uint32 d1, uint32 d2_lsb);
extern int soc_tr_tcam_type1_read_reg(int unit, uint32 addr, uint32 *d0_msb,
                                      uint32 *d1, uint32 *d2_lsb);
extern int soc_tr_tcam_type1_memtest_dpeo(int unit, int num_entries,
                                          uint32 oe_map, uint32 *data);


#endif /* BCM_TRIUMPH_SUPPORT */

extern int soc_tcam_init(int unit);
extern int soc_tcam_get_info(int unit, int *type, int *subtype, int *dc_val,
                             soc_tcam_info_t **tcam_info);
extern int soc_tcam_get_part_size(int unit, soc_tcam_partition_type_t part,
                                  int *num_entries,
                                  int *num_entries_include_pad);
extern int soc_tcam_part_index_to_mem_index(int unit,
                                            soc_tcam_partition_type_t part,
                                            int part_index,
                                            soc_mem_t mem, int *mem_index);
extern int soc_tcam_mem_index_to_raw_index(int unit, soc_mem_t mem,
                                           int mem_index, soc_mem_t *real_mem,
                                           int *raw_index);
extern int soc_tcam_raw_index_to_mem_index(int unit, int raw_index, 
                                            soc_mem_t *mem, int *mem_index);
extern int soc_tcam_read_entry(int unit, int part, int index, uint32 *mask,
                               uint32 *data, int *valid);
extern int soc_tcam_write_entry(int unit, int part, int index, uint32 *mask,
                                uint32 *data);
extern int soc_tcam_set_valid(int unit, int part, int index, int valid);
extern int soc_tcam_search_entry(int unit, int part0, int part1, uint32 *data,
                                 int *index0, int *index1);
extern int soc_tcam_write_ib(int unit, uint32 addr, uint32 data);
extern int soc_tcam_write_ima(int unit, uint32 addr, uint32 d0_msb,
                              uint32 d1_lsb);
extern int soc_tcam_read_ima(int unit, uint32 addr, uint32 *d0_msb,
                             uint32 *d1_lsb);
extern int soc_tcam_write_dbreg(int unit, uint32 addr, uint32 d0_msb,
                                uint32 d1, uint32 d2_lsb);
extern int soc_tcam_read_dbreg(int unit, uint32 addr, uint32 *d0_msb,
                               uint32 *d1, uint32 *d2_lsb);
#endif /* !_SOC_ER_TCAM_H */
