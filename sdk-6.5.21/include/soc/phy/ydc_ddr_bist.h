/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef YDC_DDR_BIST_BIST_H__
#define YDC_DDR_BIST_BIST_H__

#ifndef YDC_DDR_BIST_BIST_REG_BASE
#define YDC_DDR_BIST_BIST_REG_BASE                  0x1023BC00
#endif

#ifndef YDC_DDR_BIST_BIST_PHY_BITWITDH_IS_32
#define YDC_DDR_BIST_BIST_PHY_BITWITDH_IS_32        1
#endif

#define YDC_DDR_BIST_BIST_POLL_INTERVAL_US          10
#define YDC_DDR_BIST_BIST_POLL_COUNT_LIMIT          100000

typedef struct ydc_ddr_bist_info_s
{ 
    uint32 write_weight;
    uint32 read_weight;
    uint32 bist_timer_us;        /* if bist_timer_us != 0, ignore bist_num_actions and wait bist_timer_us before stopping bist */
    uint32 bist_num_actions;
    uint32 bist_start_address;
    uint32 bist_end_address;
    uint32 prbs_mode;
    uint32 mpr_mode;
    uint32 addr_prbs_mode;
} ydc_ddr_bist_info_t;

typedef struct ydc_ddr_bist_err_cnt_s
{ 
    uint32 bist_err_occur;
    uint32 bist_full_err_cnt;
    uint32 bist_single_err_cnt;
    uint32 bist_global_err_cnt;
} ydc_ddr_bist_err_cnt_t;

extern int soc_ydc_ddr_bist_config_set(int unit, int phy_ndx, ydc_ddr_bist_info_t *bist_info);
extern int soc_ydc_ddr_bist_run(int unit, int phy_ndx, ydc_ddr_bist_err_cnt_t *error_count);

/**
 * m = memory, c = core, r = register, f = field, d = data.
 */
#if !defined(GET_FIELD) && !defined(SET_FIELD)
#define BRCM_ALIGN(c,r,f)   c##_##r##_##f##_ALIGN
#define BRCM_BITS(c,r,f)    c##_##r##_##f##_BITS
#define BRCM_MASK(c,r,f)    c##_##r##_##f##_MASK
#define BRCM_SHIFT(c,r,f)   c##_##r##_##f##_SHIFT

#define GET_FIELD(m,c,r,f) \
	((((m) & BRCM_MASK(c,r,f)) >> BRCM_SHIFT(c,r,f)) << BRCM_ALIGN(c,r,f))

#define SET_FIELD(m,c,r,f,d) \
	((m) = (((m) & ~BRCM_MASK(c,r,f)) | ((((d) >> BRCM_ALIGN(c,r,f)) << \
	 BRCM_SHIFT(c,r,f)) & BRCM_MASK(c,r,f))) \
	)

#define SET_TYPE_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,c##_##d)
#define SET_NAME_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,c##_##r##_##f##_##d)
#define SET_VALUE_FIELD(m,c,r,f,d) SET_FIELD(m,c,r,f,d)

#endif /* GET & SET */

#define YDC_DDR_BIST_BIST_REG_READ(_unit, _pc, flags, _reg_addr, _val) \
            *(uint32 *)_val = REGRD((_pc) + (_reg_addr))
/*            soc_ddr40_phy_reg_ci_read((_unit), (_pc), (_reg_addr), (_val)) */
#define YDC_DDR_BIST_BIST_REG_WRITE(_unit, _pc, _flags, _reg_addr, _val) \
            REGWR((_pc) + (_reg_addr), (_val))
/*            soc_ddr40_phy_reg_ci_write((_unit), (_pc), (_reg_addr), (_val)) */
#define YDC_DDR_BIST_BIST_REG_MODIFY(_unit, _pc, _flags, _reg_addr, _val, _mask) \
            REGWR((_pc) + (_reg_addr), (REGRD((_pc) + (_reg_addr)) & ~(_mask)) | ((_val) & (_mask)))
/*            soc_ddr40_phy_reg_ci_modify((_unit), (_pc), (_reg_addr), (_val), (_mask)) */
#define YDC_DDR_BIST_BIST_GET_FIELD(m,c,r,f) \
            GET_FIELD(m,c,r,f)
#define YDC_DDR_BIST_BIST_SET_FIELD(m,c,r,f,d) \
            SET_FIELD(m,c,r,f,d)

/****************************************************************************
 * Core Enums.
 */
#define YDC_DDR_BIST_BIST_CONFIG                                         0x00000000
#define YDC_DDR_BIST_BIST_CONFIG_2                                       0x00000004
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS                         0x00000008
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES                               0x0000000C
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0                               0x00000010
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1                               0x00000014
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0                     0x00000018
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32                    0x0000001C
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64                    0x00000020
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96                   0x00000024
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0                     0x00000028
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32                    0x0000002C
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64                    0x00000030
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96                   0x00000034
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED                                 0x00000038
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS                            0x0000003C
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS                           0x00000040
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0                           0x00000044
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1                           0x00000048
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0                   0x0000004C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32                  0x00000050
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64                  0x00000054
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96                 0x00000058
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0                   0x0000005C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32                  0x00000060
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64                  0x00000064
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96                 0x00000068
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0                   0x0000006C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32                  0x00000070
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64                  0x00000074
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96                 0x00000078
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0                   0x0000007C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32                  0x00000080
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64                  0x00000084
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96                 0x00000088
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0                   0x0000008C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32                  0x00000090
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64                  0x00000094
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96                 0x00000098
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0                   0x0000009C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32                  0x000000A0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64                  0x000000A4
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96                 0x000000A8
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0                   0x000000AC
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32                  0x000000B0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64                  0x000000B4
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96                 0x000000B8
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0                   0x000000BC
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32                  0x000000C0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64                  0x000000C4
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96                 0x000000C8
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0                   0x000000CC
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32                  0x000000D0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64                  0x000000D4
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96                 0x000000D8
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0                   0x000000DC
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32                  0x000000E0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64                  0x000000E4
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96                 0x000000E8
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0                   0x000000EC
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32                  0x000000F0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64                  0x000000F4
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96                 0x000000F8
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0                   0x000000FC
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32                  0x00000100
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64                  0x00000104
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96                 0x00000108
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0                   0x0000010C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32                  0x00000110
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64                  0x00000114
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96                 0x00000118
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0                   0x0000011C
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32                  0x00000120
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64                  0x00000124
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96                 0x00000128
#define YDC_DDR_BIST_BIST_RATE_LIMITER                                   0x0000012C
#define YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS                              0x00000130
#define YDC_DDR_BIST_BIST_START_ADDRESS                                  0x00000134
#define YDC_DDR_BIST_BIST_END_ADDRESS                                    0x00000138
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK                                0x0000013C
#define YDC_DDR_BIST_BIST_PATTERN_WORD_7                                 0x00000140
#define YDC_DDR_BIST_BIST_PATTERN_WORD_6                                 0x00000144
#define YDC_DDR_BIST_BIST_PATTERN_WORD_5                                 0x00000148
#define YDC_DDR_BIST_BIST_PATTERN_WORD_4                                 0x0000014C
#define YDC_DDR_BIST_BIST_PATTERN_WORD_3                                 0x00000150
#define YDC_DDR_BIST_BIST_PATTERN_WORD_2                                 0x00000154
#define YDC_DDR_BIST_BIST_PATTERN_WORD_1                                 0x00000158
#define YDC_DDR_BIST_BIST_PATTERN_WORD_0                                 0x0000015C
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_7                               0x00000160
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_6                               0x00000164
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_5                               0x00000168
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_4                               0x0000016C
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_3                               0x00000170
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_2                               0x00000174
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_1                               0x00000178
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_0                               0x0000017C
#define YDC_DDR_BIST_BIST_STATUSES                                       0x00000180
#define YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER                        0x00000184
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER                  0x00000188
#define YDC_DDR_BIST_BIST_ERROR_OCCURRED                                 0x0000018C
#define YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER                           0x00000190
#define YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL                             0x00000194
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED                                0x00000198
#define YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL                             0x0000019C
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED                                0x000001A0
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR                                  0x000001A4
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0                      0x000001A8
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32                     0x000001AC
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64                     0x000001B0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96                    0x000001B4
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0                      0x000001B8
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32                     0x000001BC
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64                     0x000001C0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96                    0x000001C4
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0                     0x000001C8
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32                    0x000001CC
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64                    0x000001D0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96                   0x000001D4
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0                     0x000001D8
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32                    0x000001DC
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64                    0x000001E0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96                   0x000001E4
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD                               0x000001E8
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR                                0x000001EC

/****************************************************************************
 * YDC_DDR_BIST_BIST_YDC_DDR_BIST
 */

/* BIST Configuration Register */
#define READ_YDC_DDR_BIST_BIST_CONFIGr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIG, (_val))
#define WRITE_YDC_DDR_BIST_BIST_CONFIGr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIG, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_CONFIGr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIG, (_val), (_mask))

/* BIST Configuration Register 2 */
#define READ_YDC_DDR_BIST_BIST_CONFIG_2r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIG_2, (_val))
#define WRITE_YDC_DDR_BIST_BIST_CONFIG_2r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIG_2, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_CONFIG_2r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIG_2, (_val), (_mask))

/* BIST General Configurations Register */
#define READ_YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS, (_val))
#define WRITE_YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONSr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS, (_val), (_mask))

/* DRAM Special Features Register */
#define READ_YDC_DDR_BIST_DRAM_SPECIAL_FEATURESr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_DRAM_SPECIAL_FEATURES, (_val))
#define WRITE_YDC_DDR_BIST_DRAM_SPECIAL_FEATURESr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_DRAM_SPECIAL_FEATURES, (_val))
#define MODIFY_YDC_DDR_BIST_DRAM_SPECIAL_FEATURESr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_DRAM_SPECIAL_FEATURES, (_val), (_mask))

/* BIST Configurations 0 Register */
#define READ_YDC_DDR_BIST_BIST_CONFIGURATIONS_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIGURATIONS_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_CONFIGURATIONS_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIGURATIONS_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_CONFIGURATIONS_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIGURATIONS_0, (_val), (_mask))

/* BIST Configurations 1 Register */
#define READ_YDC_DDR_BIST_BIST_CONFIGURATIONS_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIGURATIONS_1, (_val))
#define WRITE_YDC_DDR_BIST_BIST_CONFIGURATIONS_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIGURATIONS_1, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_CONFIGURATIONS_1r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_CONFIGURATIONS_1, (_val), (_mask))

/* BIST PRBS Data Seed MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0, (_val), (_mask))

/* BIST PRBS Data Seed MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32, (_val), (_mask))

/* BIST PRBS Data Seed MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64, (_val), (_mask))

/* BIST PRBS Data Seed MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96, (_val), (_mask))

/* BIST PRBS Data Seed LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0, (_val), (_mask))

/* BIST PRBS Data Seed LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32, (_val), (_mask))

/* BIST PRBS Data Seed LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64, (_val), (_mask))

/* BIST PRBS Data Seed LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96, (_val), (_mask))

/* BIST PRBS Address Seed Register */
#define READ_YDC_DDR_BIST_BIST_PRBS_ADDR_SEEDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_ADDR_SEED, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PRBS_ADDR_SEEDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_ADDR_SEED, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PRBS_ADDR_SEEDr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PRBS_ADDR_SEED, (_val), (_mask))

/* BIST MPR Stagger Weights Register */
#define READ_YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS, (_val))
#define WRITE_YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTSr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS, (_val), (_mask))

/* BIST MPR Stagger Patterns Register */
#define READ_YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS, (_val))
#define WRITE_YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNSr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS, (_val), (_mask))

/* BIST Pattern Repetition 0 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_REPETITION_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_REPETITION_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_REPETITION_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_REPETITION_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_REPETITION_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_REPETITION_0, (_val), (_mask))

/* BIST Pattern Repetition 1 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_REPETITION_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_REPETITION_1, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_REPETITION_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_REPETITION_1, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_REPETITION_1r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_REPETITION_1, (_val), (_mask))

/* BIST Repeat Pattern 1 MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 1 MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 1 MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 1 MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 1 LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 1 LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 1 LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 1 LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 2 MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 2 MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 2 MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 2 MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 2 LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 2 LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 2 LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 2 LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 3 MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 3 MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 3 MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 3 MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 3 LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 3 LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 3 LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 3 LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 4 MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 4 MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 4 MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 4 MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 4 LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 4 LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 4 LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 4 LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 5 MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 5 MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 5 MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 5 MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 5 LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 5 LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 5 LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 5 LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 6 MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 6 MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 6 MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 6 MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 6 LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 6 LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 6 LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 6 LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 7 MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 7 MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 7 MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 7 MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96, (_val), (_mask))

/* BIST Repeat Pattern 7 LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0, (_val), (_mask))

/* BIST Repeat Pattern 7 LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32, (_val), (_mask))

/* BIST Repeat Pattern 7 LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64, (_val), (_mask))

/* BIST Repeat Pattern 7 LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96, (_val), (_mask))

/* BIST Rate Limiter Register */
#define READ_YDC_DDR_BIST_BIST_RATE_LIMITERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_RATE_LIMITER, (_val))
#define WRITE_YDC_DDR_BIST_BIST_RATE_LIMITERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_RATE_LIMITER, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_RATE_LIMITERr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_RATE_LIMITER, (_val), (_mask))

/* BIST Number of Actions Register */
#define READ_YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS, (_val))
#define WRITE_YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONSr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS, (_val), (_mask))

/* BIST Start Address Register */
#define READ_YDC_DDR_BIST_BIST_START_ADDRESSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_START_ADDRESS, (_val))
#define WRITE_YDC_DDR_BIST_BIST_START_ADDRESSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_START_ADDRESS, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_START_ADDRESSr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_START_ADDRESS, (_val), (_mask))

/* BIST End Address Register */
#define READ_YDC_DDR_BIST_BIST_END_ADDRESSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_END_ADDRESS, (_val))
#define WRITE_YDC_DDR_BIST_BIST_END_ADDRESSr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_END_ADDRESS, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_END_ADDRESSr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_END_ADDRESS, (_val), (_mask))

/* BIST Single Bit Mask Register */
#define READ_YDC_DDR_BIST_BIST_SINGLE_BIT_MASKr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_SINGLE_BIT_MASK, (_val))
#define WRITE_YDC_DDR_BIST_BIST_SINGLE_BIT_MASKr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_SINGLE_BIT_MASK, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_SINGLE_BIT_MASKr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_SINGLE_BIT_MASK, (_val), (_mask))

/* BIST Pattern Word 7 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_7r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_7, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_7r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_7, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_7r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_7, (_val), (_mask))

/* BIST Pattern Word 6 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_6r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_6, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_6r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_6, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_6r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_6, (_val), (_mask))

/* BIST Pattern Word 5 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_5r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_5, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_5r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_5, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_5r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_5, (_val), (_mask))

/* BIST Pattern Word 4 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_4r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_4, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_4r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_4, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_4r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_4, (_val), (_mask))

/* BIST Pattern Word 3 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_3r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_3, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_3r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_3, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_3r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_3, (_val), (_mask))

/* BIST Pattern Word 2 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_2r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_2, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_2r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_2, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_2r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_2, (_val), (_mask))

/* BIST Pattern Word 1 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_1, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_1, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_1r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_1, (_val), (_mask))

/* BIST Pattern Word 0 Register */
#define READ_YDC_DDR_BIST_BIST_PATTERN_WORD_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_PATTERN_WORD_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_PATTERN_WORD_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_PATTERN_WORD_0, (_val), (_mask))

/* BIST Full Mask Word 7 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_7r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_7, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_7r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_7, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_7r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_7, (_val), (_mask))

/* BIST Full Mask Word 6 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_6r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_6, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_6r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_6, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_6r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_6, (_val), (_mask))

/* BIST Full Mask Word 5 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_5r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_5, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_5r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_5, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_5r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_5, (_val), (_mask))

/* BIST Full Mask Word 4 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_4r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_4, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_4r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_4, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_4r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_4, (_val), (_mask))

/* BIST Full Mask Word 3 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_3r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_3, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_3r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_3, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_3r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_3, (_val), (_mask))

/* BIST Full Mask Word 2 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_2r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_2, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_2r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_2, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_2r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_2, (_val), (_mask))

/* BIST Full Mask Word 1 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_1, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_1r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_1, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_1r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_1, (_val), (_mask))

/* BIST Full Mask Word 0 Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_WORD_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_WORD_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_WORD_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_WORD_0, (_val), (_mask))

/* BIST Status Register */
#define READ_YDC_DDR_BIST_BIST_STATUSESr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_STATUSES, (_val))
#define WRITE_YDC_DDR_BIST_BIST_STATUSESr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_STATUSES, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_STATUSESr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_STATUSES, (_val), (_mask))

/* BIST Full Mask Error Counter Register */
#define READ_YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER, (_val))
#define WRITE_YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTERr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER, (_val), (_mask))

/* BIST Single Bit Mask Error Counter Register */
#define READ_YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER, (_val))
#define WRITE_YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER, (_val), (_mask))

/* BIST Error Occurred Register */
#define READ_YDC_DDR_BIST_BIST_ERROR_OCCURREDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_ERROR_OCCURRED, (_val))
#define WRITE_YDC_DDR_BIST_BIST_ERROR_OCCURREDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_ERROR_OCCURRED, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_ERROR_OCCURREDr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_ERROR_OCCURRED, (_val), (_mask))

/* BIST Global Error Counter Register */
#define READ_YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER, (_val))
#define WRITE_YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTERr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTERr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER, (_val), (_mask))

/* Count BIST DBI Error Global Register */
#define READ_YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBALr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL, (_val))
#define WRITE_YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBALr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL, (_val))
#define MODIFY_YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBALr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL, (_val), (_mask))

/* BIST DBI Error Occurred Register */
#define READ_YDC_DDR_BIST_BIST_DBI_ERR_OCCUREDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_DBI_ERR_OCCURED, (_val))
#define WRITE_YDC_DDR_BIST_BIST_DBI_ERR_OCCUREDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_DBI_ERR_OCCURED, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_DBI_ERR_OCCUREDr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_DBI_ERR_OCCURED, (_val), (_mask))

/* Count BIST EDC Error Global Register */
#define READ_YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBALr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL, (_val))
#define WRITE_YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBALr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL, (_val))
#define MODIFY_YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBALr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL, (_val), (_mask))

/* BIST EDC Error Occurred Register */
#define READ_YDC_DDR_BIST_BIST_EDC_ERR_OCCUREDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_EDC_ERR_OCCURED, (_val))
#define WRITE_YDC_DDR_BIST_BIST_EDC_ERR_OCCUREDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_EDC_ERR_OCCURED, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_EDC_ERR_OCCUREDr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_EDC_ERR_OCCURED, (_val), (_mask))

/* BIST Last Error Address Register */
#define READ_YDC_DDR_BIST_BIST_LAST_ADDR_ERRr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_ADDR_ERR, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_ADDR_ERRr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_ADDR_ERR, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_ADDR_ERRr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_ADDR_ERR, (_val), (_mask))

/* BIST Last Data Error MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0, (_val), (_mask))

/* BIST Last Data Error MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32, (_val), (_mask))

/* BIST Last Data Error MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64, (_val), (_mask))

/* BIST Last Data Error MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96, (_val), (_mask))

/* BIST Last Data Error LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0, (_val), (_mask))

/* BIST Last Data Error LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32, (_val), (_mask))

/* BIST Last Data Error LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64, (_val), (_mask))

/* BIST Last Data Error LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96, (_val), (_mask))

/* BIST Last Read Data MSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0, (_val), (_mask))

/* BIST Last Read Data MSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32, (_val), (_mask))

/* BIST Last Read Data MSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64, (_val), (_mask))

/* BIST Last Read Data MSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96, (_val), (_mask))

/* BIST Last Read Data LSB 31 to 0 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0, (_val), (_mask))

/* BIST Last Read Data LSB 63 to 32 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32, (_val), (_mask))

/* BIST Last Read Data LSB 95 to 64 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64, (_val), (_mask))

/* BIST Last Read Data LSB 127 to 96 Register */
#define READ_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96, (_val))
#define WRITE_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96r(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96, (_val))
#define MODIFY_YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96r(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96, (_val), (_mask))

/* GDDR5 BIST Finish Period Register */
#define READ_YDC_DDR_BIST_GDDR5_BIST_FINISH_PRDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD, (_val))
#define WRITE_YDC_DDR_BIST_GDDR5_BIST_FINISH_PRDr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD, (_val))
#define MODIFY_YDC_DDR_BIST_GDDR5_BIST_FINISH_PRDr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD, (_val), (_mask))

/* Initial Calibration Use MPR Register */
#define READ_YDC_DDR_BIST_INTIAL_CALIB_USE_MPRr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_READ((_unit), (_pc), 0x00, YDC_DDR_BIST_INTIAL_CALIB_USE_MPR, (_val))
#define WRITE_YDC_DDR_BIST_INTIAL_CALIB_USE_MPRr(_unit, _pc, _val) \
             YDC_DDR_BIST_BIST_REG_WRITE((_unit), (_pc), 0x00, YDC_DDR_BIST_INTIAL_CALIB_USE_MPR, (_val))
#define MODIFY_YDC_DDR_BIST_INTIAL_CALIB_USE_MPRr(_unit, _pc, _val, _mask) \
             YDC_DDR_BIST_BIST_REG_MODIFY((_unit), (_pc), 0x00, YDC_DDR_BIST_INTIAL_CALIB_USE_MPR, (_val), (_mask))


/****************************************************************************
 * YDC_DDR_BIST_BIST_YDC_DDR_BIST
 */
/****************************************************************************
 * YDC_DDR_BIST :: BIST_CONFIG
 */
/* YDC_DDR_BIST :: BIST_CONFIG :: reserved0 [31:16] */
#define YDC_DDR_BIST_BIST_CONFIG_RESERVED0_MASK                         0xffff0000
#define YDC_DDR_BIST_BIST_CONFIG_RESERVED0_ALIGN                        0
#define YDC_DDR_BIST_BIST_CONFIG_RESERVED0_BITS                         16
#define YDC_DDR_BIST_BIST_CONFIG_RESERVED0_SHIFT                        16
#define YDC_DDR_BIST_BIST_CONFIG_RESERVED0_DEFAULT                      0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BUS16_MODE [15:15] */
#define YDC_DDR_BIST_BIST_CONFIG_BUS16_MODE_MASK                        0x00008000
#define YDC_DDR_BIST_BIST_CONFIG_BUS16_MODE_ALIGN                       0
#define YDC_DDR_BIST_BIST_CONFIG_BUS16_MODE_BITS                        1
#define YDC_DDR_BIST_BIST_CONFIG_BUS16_MODE_SHIFT                       15
#define YDC_DDR_BIST_BIST_CONFIG_BUS16_MODE_DEFAULT                     0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: ENABLE_8_BANKS_MODE [14:14] */
#define YDC_DDR_BIST_BIST_CONFIG_ENABLE_8_BANKS_MODE_MASK               0x00004000
#define YDC_DDR_BIST_BIST_CONFIG_ENABLE_8_BANKS_MODE_ALIGN              0
#define YDC_DDR_BIST_BIST_CONFIG_ENABLE_8_BANKS_MODE_BITS               1
#define YDC_DDR_BIST_BIST_CONFIG_ENABLE_8_BANKS_MODE_SHIFT              14
#define YDC_DDR_BIST_BIST_CONFIG_ENABLE_8_BANKS_MODE_DEFAULT            0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: DISABLE_COL_BANK_SWAPPING [13:13] */
#define YDC_DDR_BIST_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_MASK         0x00002000
#define YDC_DDR_BIST_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_ALIGN        0
#define YDC_DDR_BIST_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_BITS         1
#define YDC_DDR_BIST_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_SHIFT        13
#define YDC_DDR_BIST_BIST_CONFIG_DISABLE_COL_BANK_SWAPPING_DEFAULT      0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_ARPRIORITY [12:10] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARPRIORITY_MASK                   0x00001c00
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARPRIORITY_ALIGN                  0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARPRIORITY_BITS                   3
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARPRIORITY_SHIFT                  10
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARPRIORITY_DEFAULT                0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_ARAPCMD [09:09] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARAPCMD_MASK                      0x00000200
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARAPCMD_ALIGN                     0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARAPCMD_BITS                      1
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARAPCMD_SHIFT                     9
#define YDC_DDR_BIST_BIST_CONFIG_BIST_ARAPCMD_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_AWUSER [08:08] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWUSER_MASK                       0x00000100
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWUSER_ALIGN                      0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWUSER_BITS                       1
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWUSER_SHIFT                      8
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWUSER_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_AWPRIORITY [07:05] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWPRIORITY_MASK                   0x000000e0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWPRIORITY_ALIGN                  0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWPRIORITY_BITS                   3
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWPRIORITY_SHIFT                  5
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWPRIORITY_DEFAULT                0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_AWCOBUF [04:04] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCOBUF_MASK                      0x00000010
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCOBUF_ALIGN                     0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCOBUF_BITS                      1
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCOBUF_SHIFT                     4
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCOBUF_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_AWAPCMD [03:03] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWAPCMD_MASK                      0x00000008
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWAPCMD_ALIGN                     0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWAPCMD_BITS                      1
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWAPCMD_SHIFT                     3
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWAPCMD_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_AWCACHE_0 [02:02] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCACHE_0_MASK                    0x00000004
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCACHE_0_ALIGN                   0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCACHE_0_BITS                    1
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCACHE_0_SHIFT                   2
#define YDC_DDR_BIST_BIST_CONFIG_BIST_AWCACHE_0_DEFAULT                 0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: AXI_PORT_SEL [01:01] */
#define YDC_DDR_BIST_BIST_CONFIG_AXI_PORT_SEL_MASK                      0x00000002
#define YDC_DDR_BIST_BIST_CONFIG_AXI_PORT_SEL_ALIGN                     0
#define YDC_DDR_BIST_BIST_CONFIG_AXI_PORT_SEL_BITS                      1
#define YDC_DDR_BIST_BIST_CONFIG_AXI_PORT_SEL_SHIFT                     1
#define YDC_DDR_BIST_BIST_CONFIG_AXI_PORT_SEL_DEFAULT                   0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG :: BIST_RESETB [00:00] */
#define YDC_DDR_BIST_BIST_CONFIG_BIST_RESETB_MASK                       0x00000001
#define YDC_DDR_BIST_BIST_CONFIG_BIST_RESETB_ALIGN                      0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_RESETB_BITS                       1
#define YDC_DDR_BIST_BIST_CONFIG_BIST_RESETB_SHIFT                      0
#define YDC_DDR_BIST_BIST_CONFIG_BIST_RESETB_DEFAULT                    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_CONFIG_2
 */
/* YDC_DDR_BIST :: BIST_CONFIG_2 :: reserved0 [31:19] */
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED0_MASK                       0xfff80000
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED0_ALIGN                      0
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED0_BITS                       13
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED0_SHIFT                      19
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED0_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG_2 :: FIFO_RADDR_ALMOST_FULL_LEVEL [18:16] */
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_MASK    0x00070000
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_ALIGN   0
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_BITS    3
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_SHIFT   16
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_RADDR_ALMOST_FULL_LEVEL_DEFAULT 0x00000004

/* YDC_DDR_BIST :: BIST_CONFIG_2 :: reserved1 [15:11] */
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED1_MASK                       0x0000f800
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED1_ALIGN                      0
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED1_BITS                       5
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED1_SHIFT                      11
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED1_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG_2 :: FIFO_CMD_ALMOST_FULL_LEVEL [10:08] */
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_MASK      0x00000700
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_ALIGN     0
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_BITS      3
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_SHIFT     8
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_CMD_ALMOST_FULL_LEVEL_DEFAULT   0x00000002

/* YDC_DDR_BIST :: BIST_CONFIG_2 :: reserved2 [07:03] */
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED2_MASK                       0x000000f8
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED2_ALIGN                      0
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED2_BITS                       5
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED2_SHIFT                      3
#define YDC_DDR_BIST_BIST_CONFIG_2_RESERVED2_DEFAULT                    0x00000000

/* YDC_DDR_BIST :: BIST_CONFIG_2 :: FIFO_WDATA_ALMOST_FULL_LEVEL [02:00] */
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_MASK    0x00000007
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_ALIGN   0
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_BITS    3
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_SHIFT   0
#define YDC_DDR_BIST_BIST_CONFIG_2_FIFO_WDATA_ALMOST_FULL_LEVEL_DEFAULT 0x00000002


/****************************************************************************
 * YDC_DDR_BIST :: BIST_GENERAL_CONFIGURATIONS
 */
/* YDC_DDR_BIST :: BIST_GENERAL_CONFIGURATIONS :: reserved0 [31:16] */
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED0_MASK         0xffff0000
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED0_ALIGN        0
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED0_BITS         16
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED0_SHIFT        16
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED0_DEFAULT      0x00000000

/* YDC_DDR_BIST :: BIST_GENERAL_CONFIGURATIONS :: BANK_GROUP_TOPO [15:14] */
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_BANK_GROUP_TOPO_MASK    0x0000c000
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_BANK_GROUP_TOPO_ALIGN   0
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_BANK_GROUP_TOPO_BITS    2
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_BANK_GROUP_TOPO_SHIFT   14
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_BANK_GROUP_TOPO_DEFAULT 0x00000000

/* YDC_DDR_BIST :: BIST_GENERAL_CONFIGURATIONS :: reserved1 [13:07] */
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED1_MASK         0x00003f80
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED1_ALIGN        0
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED1_BITS         7
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED1_SHIFT        7
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED1_DEFAULT      0x00000000

/* YDC_DDR_BIST :: BIST_GENERAL_CONFIGURATIONS :: NUM_COLS [06:04] */
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_MASK          0x00000070
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_ALIGN         0
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_BITS          3
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_SHIFT         4
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_NUM_COLS_DEFAULT       0x00000002

/* YDC_DDR_BIST :: BIST_GENERAL_CONFIGURATIONS :: reserved2 [03:00] */
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED2_MASK         0x0000000f
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED2_ALIGN        0
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED2_BITS         4
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED2_SHIFT        0
#define YDC_DDR_BIST_BIST_GENERAL_CONFIGURATIONS_RESERVED2_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: DRAM_SPECIAL_FEATURES
 */
/* YDC_DDR_BIST :: DRAM_SPECIAL_FEATURES :: reserved0 [31:04] */
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED0_MASK         0xfffffff0
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED0_ALIGN        0
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED0_BITS         28
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED0_SHIFT        4
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED0_DEFAULT      0x00000000

/* YDC_DDR_BIST :: DRAM_SPECIAL_FEATURES :: READ_DBI [03:03] */
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_READ_DBI_MASK          0x00000008
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_READ_DBI_ALIGN         0
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_READ_DBI_BITS          1
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_READ_DBI_SHIFT         3
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_READ_DBI_DEFAULT       0x00000000

/* YDC_DDR_BIST :: DRAM_SPECIAL_FEATURES :: reserved1 [02:00] */
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED1_MASK         0x00000007
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED1_ALIGN        0
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED1_BITS         3
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED1_SHIFT        0
#define YDC_DDR_BIST_DRAM_SPECIAL_FEATURES_RESERVED1_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_CONFIGURATIONS_0
 */
/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: MPR_READOUT_MPR_LOCATION [31:28] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MPR_LOCATION_MASK    0xf0000000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MPR_LOCATION_ALIGN   0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MPR_LOCATION_BITS    4
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MPR_LOCATION_SHIFT   28
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MPR_LOCATION_DEFAULT 0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: MPR_READOUT_MODE [27:27] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MODE_MASK          0x08000000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MODE_ALIGN         0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MODE_BITS          1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MODE_SHIFT         27
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_READOUT_MODE_DEFAULT       0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: MPR_STAGGER_INCREMENT_MODE [26:26] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_INCREMENT_MODE_MASK      0x04000000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_INCREMENT_MODE_ALIGN     0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_INCREMENT_MODE_BITS      1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_INCREMENT_MODE_SHIFT     26
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_INCREMENT_MODE_DEFAULT   0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: MPR_STAGGER_MODE [25:25] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_MODE_MASK          0x02000000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_MODE_ALIGN         0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_MODE_BITS          1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_MODE_SHIFT         25
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_MPR_STAGGER_MODE_DEFAULT       0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: DATA_ADDR_MODE [24:24] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_ADDR_MODE_MASK            0x01000000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_ADDR_MODE_ALIGN           0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_ADDR_MODE_BITS            1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_ADDR_MODE_SHIFT           24
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_ADDR_MODE_DEFAULT         0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: DATA_SHIFT_MODE [23:23] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_SHIFT_MODE_MASK           0x00800000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_SHIFT_MODE_ALIGN          0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_SHIFT_MODE_BITS           1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_SHIFT_MODE_SHIFT          23
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_DATA_SHIFT_MODE_DEFAULT        0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: ADDRESS_SHIFT_MODE [22:22] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_ADDRESS_SHIFT_MODE_MASK        0x00400000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_ADDRESS_SHIFT_MODE_ALIGN       0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_ADDRESS_SHIFT_MODE_BITS        1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_ADDRESS_SHIFT_MODE_SHIFT       22
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_ADDRESS_SHIFT_MODE_DEFAULT     0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: REPEAT_MODE [21:21] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_REPEAT_MODE_MASK               0x00200000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_REPEAT_MODE_ALIGN              0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_REPEAT_MODE_BITS               1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_REPEAT_MODE_SHIFT              21
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_REPEAT_MODE_DEFAULT            0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: SINGLE_BANK_TEST [20:20] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_SINGLE_BANK_TEST_MASK          0x00100000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_SINGLE_BANK_TEST_ALIGN         0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_SINGLE_BANK_TEST_BITS          1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_SINGLE_BANK_TEST_SHIFT         20
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_SINGLE_BANK_TEST_DEFAULT       0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: BG_INTERLEAVE [19:19] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_BG_INTERLEAVE_MASK             0x00080000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_BG_INTERLEAVE_ALIGN            0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_BG_INTERLEAVE_BITS             1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_BG_INTERLEAVE_SHIFT            19
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_BG_INTERLEAVE_DEFAULT          0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: PRBS_MODE [18:18] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PRBS_MODE_MASK                 0x00040000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PRBS_MODE_ALIGN                0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PRBS_MODE_BITS                 1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PRBS_MODE_SHIFT                18
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PRBS_MODE_DEFAULT              0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: TWO_ADDR_MODE [17:17] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_TWO_ADDR_MODE_MASK             0x00020000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_TWO_ADDR_MODE_ALIGN            0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_TWO_ADDR_MODE_BITS             1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_TWO_ADDR_MODE_SHIFT            17
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_TWO_ADDR_MODE_DEFAULT          0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: PATTERN_BIT_MODE [16:16] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PATTERN_BIT_MODE_MASK          0x00010000
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PATTERN_BIT_MODE_ALIGN         0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PATTERN_BIT_MODE_BITS          1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PATTERN_BIT_MODE_SHIFT         16
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_PATTERN_BIT_MODE_DEFAULT       0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: READ_WEIGHT [15:08] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_READ_WEIGHT_MASK               0x0000ff00
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_READ_WEIGHT_ALIGN              0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_READ_WEIGHT_BITS               8
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_READ_WEIGHT_SHIFT              8
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_READ_WEIGHT_DEFAULT            0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_0 :: WRITE_WEIGHT [07:00] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_WRITE_WEIGHT_MASK              0x000000ff
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_WRITE_WEIGHT_ALIGN             0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_WRITE_WEIGHT_BITS              8
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_WRITE_WEIGHT_SHIFT             0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_0_WRITE_WEIGHT_DEFAULT           0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_CONFIGURATIONS_1
 */
/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_1 :: reserved0 [31:04] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED0_MASK                 0xfffffff0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED0_ALIGN                0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED0_BITS                 28
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED0_SHIFT                4
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED0_DEFAULT              0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_1 :: BIST_EN [03:03] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_EN_MASK                   0x00000008
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_EN_ALIGN                  0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_EN_BITS                   1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_EN_SHIFT                  3
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_EN_DEFAULT                0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_1 :: BIST_8_BANKS_CYCLE [02:02] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_8_BANKS_CYCLE_MASK        0x00000004
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_8_BANKS_CYCLE_ALIGN       0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_8_BANKS_CYCLE_BITS        1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_8_BANKS_CYCLE_SHIFT       2
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_BIST_8_BANKS_CYCLE_DEFAULT     0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_1 :: reserved1 [01:01] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED1_MASK                 0x00000002
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED1_ALIGN                0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED1_BITS                 1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED1_SHIFT                1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_RESERVED1_DEFAULT              0x00000000

/* YDC_DDR_BIST :: BIST_CONFIGURATIONS_1 :: ADDRESS_PRBS_MODE [00:00] */
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_ADDRESS_PRBS_MODE_MASK         0x00000001
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_ADDRESS_PRBS_MODE_ALIGN        0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_ADDRESS_PRBS_MODE_BITS         1
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_ADDRESS_PRBS_MODE_SHIFT        0
#define YDC_DDR_BIST_BIST_CONFIGURATIONS_1_ADDRESS_PRBS_MODE_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_PRBS_DATA_SEED_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_PRBS_DATA_SEED_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PRBS_ADDR_SEED
 */
/* YDC_DDR_BIST :: BIST_PRBS_ADDR_SEED :: reserved0 [31:27] */
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_RESERVED0_MASK                  0xf8000000
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_RESERVED0_ALIGN                 0
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_RESERVED0_BITS                  5
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_RESERVED0_SHIFT                 27
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_RESERVED0_DEFAULT               0x00000000

/* YDC_DDR_BIST :: BIST_PRBS_ADDR_SEED :: VAL [26:00] */
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_VAL_MASK     0x07ffffff
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_VAL_BITS     27
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_PRBS_ADDR_SEED_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_MPR_STAGGER_WEIGHTS
 */
/* YDC_DDR_BIST :: BIST_MPR_STAGGER_WEIGHTS :: BIST_MPR_STAGGER_WEIGHT_3 [31:24] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_3_MASK    0xff000000
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_3_ALIGN   0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_3_BITS    8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_3_SHIFT   24
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_3_DEFAULT 0x00000001

/* YDC_DDR_BIST :: BIST_MPR_STAGGER_WEIGHTS :: BIST_MPR_STAGGER_WEIGHT_2 [23:16] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_2_MASK    0x00ff0000
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_2_ALIGN   0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_2_BITS    8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_2_SHIFT   16
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_2_DEFAULT 0x00000001

/* YDC_DDR_BIST :: BIST_MPR_STAGGER_WEIGHTS :: BIST_MPR_STAGGER_WEIGHT_1 [15:08] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_1_MASK    0x0000ff00
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_1_ALIGN   0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_1_BITS    8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_1_SHIFT   8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_1_DEFAULT 0x00000001

/* YDC_DDR_BIST :: BIST_MPR_STAGGER_WEIGHTS :: BIST_MPR_STAGGER_WEIGHT_0 [07:00] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_0_MASK    0x000000ff
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_0_ALIGN   0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_0_BITS    8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_0_SHIFT   0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_WEIGHTS_BIST_MPR_STAGGER_WEIGHT_0_DEFAULT 0x00000001


/****************************************************************************
 * YDC_DDR_BIST :: BIST_MPR_STAGGER_PATTERNS
 */
/* YDC_DDR_BIST :: BIST_MPR_STAGGER_PATTERNS :: BIST_MPR_STAGGER_PATTERN_3 [31:24] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_3_MASK      0xff000000
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_3_ALIGN     0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_3_BITS      8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_3_SHIFT     24
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_3_DEFAULT   0x000000ff

/* YDC_DDR_BIST :: BIST_MPR_STAGGER_PATTERNS :: BIST_MPR_STAGGER_PATTERN_2 [23:16] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_2_MASK      0x00ff0000
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_2_ALIGN     0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_2_BITS      8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_2_SHIFT     16
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_2_DEFAULT   0x00000000

/* YDC_DDR_BIST :: BIST_MPR_STAGGER_PATTERNS :: BIST_MPR_STAGGER_PATTERN_1 [15:08] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_1_MASK      0x0000ff00
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_1_ALIGN     0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_1_BITS      8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_1_SHIFT     8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_1_DEFAULT   0x000000ff

/* YDC_DDR_BIST :: BIST_MPR_STAGGER_PATTERNS :: BIST_MPR_STAGGER_PATTERN_0 [07:00] */
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_0_MASK      0x000000ff
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_0_ALIGN     0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_0_BITS      8
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_0_SHIFT     0
#define YDC_DDR_BIST_BIST_MPR_STAGGER_PATTERNS_BIST_MPR_STAGGER_PATTERN_0_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_REPETITION_0
 */
/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_0 :: NUM_OF_REPEAT_3 [31:24] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_3_MASK     0xff000000
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_3_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_3_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_3_SHIFT    24
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_3_DEFAULT  0x000000ff

/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_0 :: NUM_OF_REPEAT_2 [23:16] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_2_MASK     0x00ff0000
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_2_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_2_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_2_SHIFT    16
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_2_DEFAULT  0x00000000

/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_0 :: NUM_OF_REPEAT_1 [15:08] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_1_MASK     0x0000ff00
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_1_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_1_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_1_SHIFT    8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_1_DEFAULT  0x000000ff

/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_0 :: NUM_OF_REPEAT_0 [07:00] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_0_MASK     0x000000ff
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_0_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_0_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_0_SHIFT    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_0_NUM_OF_REPEAT_0_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_REPETITION_1
 */
/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_1 :: NUM_OF_REPEAT_7 [31:24] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_7_MASK     0xff000000
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_7_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_7_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_7_SHIFT    24
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_7_DEFAULT  0x000000ff

/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_1 :: NUM_OF_REPEAT_6 [23:16] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_6_MASK     0x00ff0000
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_6_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_6_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_6_SHIFT    16
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_6_DEFAULT  0x00000000

/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_1 :: NUM_OF_REPEAT_5 [15:08] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_5_MASK     0x0000ff00
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_5_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_5_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_5_SHIFT    8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_5_DEFAULT  0x000000ff

/* YDC_DDR_BIST :: BIST_PATTERN_REPETITION_1 :: NUM_OF_REPEAT_4 [07:00] */
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_4_MASK     0x000000ff
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_4_ALIGN    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_4_BITS     8
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_4_SHIFT    0
#define YDC_DDR_BIST_BIST_PATTERN_REPETITION_1_NUM_OF_REPEAT_4_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_1_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_1_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_2_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_2_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_3_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_3_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_4_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_4_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_5_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_5_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_6_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_6_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_REPEAT_PATTERN_7_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_REPEAT_PATTERN_7_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_RATE_LIMITER
 */
/* YDC_DDR_BIST :: BIST_RATE_LIMITER :: reserved0 [31:09] */
#define YDC_DDR_BIST_BIST_RATE_LIMITER_RESERVED0_MASK               0xfffffe00
#define YDC_DDR_BIST_BIST_RATE_LIMITER_RESERVED0_ALIGN              0
#define YDC_DDR_BIST_BIST_RATE_LIMITER_RESERVED0_BITS               6
#define YDC_DDR_BIST_BIST_RATE_LIMITER_RESERVED0_SHIFT              23
#define YDC_DDR_BIST_BIST_RATE_LIMITER_RESERVED0_DEFAULT            0x00000000

/* YDC_DDR_BIST :: BIST_RATE_LIMITER :: BIST_CMD_WAIT_PRD [08:00] */
#define YDC_DDR_BIST_BIST_RATE_LIMITER_BIST_CMD_WAIT_PRD_MASK       0x000001ff
#define YDC_DDR_BIST_BIST_RATE_LIMITER_BIST_CMD_WAIT_PRD_ALIGN      0
#define YDC_DDR_BIST_BIST_RATE_LIMITER_BIST_CMD_WAIT_PRD_BITS       9
#define YDC_DDR_BIST_BIST_RATE_LIMITER_BIST_CMD_WAIT_PRD_SHIFT      0
#define YDC_DDR_BIST_BIST_RATE_LIMITER_BIST_CMD_WAIT_PRD_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_NUMBER_OF_ACTIONS
 */
/* YDC_DDR_BIST :: BIST_NUMBER_OF_ACTIONS :: BIST_NUM_ACTIONS [31:00] */
#define YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_ALIGN      0
#define YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_BITS       32
#define YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_SHIFT      0
#define YDC_DDR_BIST_BIST_NUMBER_OF_ACTIONS_BIST_NUM_ACTIONS_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_START_ADDRESS
 */
/* YDC_DDR_BIST :: BIST_START_ADDRESS :: BIST_START_ADDRESS [31:00] */
#define YDC_DDR_BIST_BIST_START_ADDRESS_BIST_START_ADDRESS_MASK         0xffffffff
#define YDC_DDR_BIST_BIST_START_ADDRESS_BIST_START_ADDRESS_ALIGN        0
#define YDC_DDR_BIST_BIST_START_ADDRESS_BIST_START_ADDRESS_BITS         32
#define YDC_DDR_BIST_BIST_START_ADDRESS_BIST_START_ADDRESS_SHIFT        0
#define YDC_DDR_BIST_BIST_START_ADDRESS_BIST_START_ADDRESS_DEFAULT      0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_END_ADDRESS
 */
/* YDC_DDR_BIST :: BIST_END_ADDRESS :: BIST_END_ADDRESS [31:00] */
#define YDC_DDR_BIST_BIST_END_ADDRESS_BIST_END_ADDRESS_MASK             0xffffffff
#define YDC_DDR_BIST_BIST_END_ADDRESS_BIST_END_ADDRESS_ALIGN            0
#define YDC_DDR_BIST_BIST_END_ADDRESS_BIST_END_ADDRESS_BITS             32
#define YDC_DDR_BIST_BIST_END_ADDRESS_BIST_END_ADDRESS_SHIFT            0
#define YDC_DDR_BIST_BIST_END_ADDRESS_BIST_END_ADDRESS_DEFAULT          0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_SINGLE_BIT_MASK
 */
/* YDC_DDR_BIST :: BIST_SINGLE_BIT_MASK :: BIST_SINGLE_MASK [31:00] */
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_MASK         0xffffffff
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_ALIGN        0
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_BITS         32
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_SHIFT        0
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_BIST_SINGLE_MASK_DEFAULT      0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_7
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_7 :: BIST_PATTERN_7 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_7_BIST_PATTERN_7_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_7_BIST_PATTERN_7_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_7_BIST_PATTERN_7_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_7_BIST_PATTERN_7_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_7_BIST_PATTERN_7_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_6
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_6 :: BIST_PATTERN_6 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_6_BIST_PATTERN_6_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_6_BIST_PATTERN_6_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_6_BIST_PATTERN_6_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_6_BIST_PATTERN_6_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_6_BIST_PATTERN_6_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_5
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_5 :: BIST_PATTERN_5 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_5_BIST_PATTERN_5_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_5_BIST_PATTERN_5_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_5_BIST_PATTERN_5_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_5_BIST_PATTERN_5_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_5_BIST_PATTERN_5_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_4
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_4 :: BIST_PATTERN_4 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_4_BIST_PATTERN_4_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_4_BIST_PATTERN_4_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_4_BIST_PATTERN_4_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_4_BIST_PATTERN_4_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_4_BIST_PATTERN_4_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_3
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_3 :: BIST_PATTERN_3 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_3_BIST_PATTERN_3_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_3_BIST_PATTERN_3_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_3_BIST_PATTERN_3_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_3_BIST_PATTERN_3_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_3_BIST_PATTERN_3_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_2
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_2 :: BIST_PATTERN_2 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_2_BIST_PATTERN_2_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_2_BIST_PATTERN_2_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_2_BIST_PATTERN_2_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_2_BIST_PATTERN_2_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_2_BIST_PATTERN_2_DEFAULT         0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_1
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_1 :: BIST_PATTERN_1 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_1_BIST_PATTERN_1_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_1_BIST_PATTERN_1_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_1_BIST_PATTERN_1_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_1_BIST_PATTERN_1_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_1_BIST_PATTERN_1_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_PATTERN_WORD_0
 */
/* YDC_DDR_BIST :: BIST_PATTERN_WORD_0 :: BIST_PATTERN_0 [31:00] */
#define YDC_DDR_BIST_BIST_PATTERN_WORD_0_BIST_PATTERN_0_MASK            0xffffffff
#define YDC_DDR_BIST_BIST_PATTERN_WORD_0_BIST_PATTERN_0_ALIGN           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_0_BIST_PATTERN_0_BITS            32
#define YDC_DDR_BIST_BIST_PATTERN_WORD_0_BIST_PATTERN_0_SHIFT           0
#define YDC_DDR_BIST_BIST_PATTERN_WORD_0_BIST_PATTERN_0_DEFAULT         0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_7
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_7 :: BIST_FULL_MASK_7 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_7_BIST_FULL_MASK_7_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_6
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_6 :: BIST_FULL_MASK_6 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_6_BIST_FULL_MASK_6_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_5
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_5 :: BIST_FULL_MASK_5 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_5_BIST_FULL_MASK_5_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_4
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_4 :: BIST_FULL_MASK_4 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_4_BIST_FULL_MASK_4_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_3
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_3 :: BIST_FULL_MASK_3 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_3_BIST_FULL_MASK_3_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_2
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_2 :: BIST_FULL_MASK_2 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_2_BIST_FULL_MASK_2_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_1
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_1 :: BIST_FULL_MASK_1 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_1_BIST_FULL_MASK_1_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_WORD_0
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_WORD_0 :: BIST_FULL_MASK_0 [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_MASK        0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_ALIGN       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_BITS        32
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_SHIFT       0
#define YDC_DDR_BIST_BIST_FULL_MASK_WORD_0_BIST_FULL_MASK_0_DEFAULT     0xffffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_STATUSES
 */
/* YDC_DDR_BIST :: BIST_STATUSES :: BIST_RD_ACT_CNT [31:01] */
#define YDC_DDR_BIST_BIST_STATUSES_BIST_RD_ACT_CNT_MASK                 0xfffffffe
#define YDC_DDR_BIST_BIST_STATUSES_BIST_RD_ACT_CNT_ALIGN                0
#define YDC_DDR_BIST_BIST_STATUSES_BIST_RD_ACT_CNT_BITS                 31
#define YDC_DDR_BIST_BIST_STATUSES_BIST_RD_ACT_CNT_SHIFT                1
#define YDC_DDR_BIST_BIST_STATUSES_BIST_RD_ACT_CNT_DEFAULT              0x00000000

/* YDC_DDR_BIST :: BIST_STATUSES :: BIST_FINISHED [00:00] */
#define YDC_DDR_BIST_BIST_STATUSES_BIST_FINISHED_MASK                   0x00000001
#define YDC_DDR_BIST_BIST_STATUSES_BIST_FINISHED_ALIGN                  0
#define YDC_DDR_BIST_BIST_STATUSES_BIST_FINISHED_BITS                   1
#define YDC_DDR_BIST_BIST_STATUSES_BIST_FINISHED_SHIFT                  0
#define YDC_DDR_BIST_BIST_STATUSES_BIST_FINISHED_DEFAULT                0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_FULL_MASK_ERROR_COUNTER
 */
/* YDC_DDR_BIST :: BIST_FULL_MASK_ERROR_COUNTER :: FULL_ERR_CNT [31:00] */
#define YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_ALIGN    0
#define YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_BITS     32
#define YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_SHIFT    0
#define YDC_DDR_BIST_BIST_FULL_MASK_ERROR_COUNTER_FULL_ERR_CNT_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_SINGLE_BIT_MASK_ERROR_COUNTER
 */
/* YDC_DDR_BIST :: BIST_SINGLE_BIT_MASK_ERROR_COUNTER :: SINGLE_ERR_CNT [31:00] */
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_ALIGN     0
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_BITS      32
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_SHIFT     0
#define YDC_DDR_BIST_BIST_SINGLE_BIT_MASK_ERROR_COUNTER_SINGLE_ERR_CNT_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_ERROR_OCCURRED
 */
/* YDC_DDR_BIST :: BIST_ERROR_OCCURRED :: ERR_OCCURRED [31:00] */
#define YDC_DDR_BIST_BIST_ERROR_OCCURRED_ERR_OCCURRED_MASK              0xffffffff
#define YDC_DDR_BIST_BIST_ERROR_OCCURRED_ERR_OCCURRED_ALIGN             0
#define YDC_DDR_BIST_BIST_ERROR_OCCURRED_ERR_OCCURRED_BITS              32
#define YDC_DDR_BIST_BIST_ERROR_OCCURRED_ERR_OCCURRED_SHIFT             0
#define YDC_DDR_BIST_BIST_ERROR_OCCURRED_ERR_OCCURRED_DEFAULT           0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_GLOBAL_ERROR_COUNTER
 */
/* YDC_DDR_BIST :: BIST_GLOBAL_ERROR_COUNTER :: GLOBAL_ERR_CNT [31:00] */
#define YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_ALIGN     0
#define YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_BITS      32
#define YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_SHIFT     0
#define YDC_DDR_BIST_BIST_GLOBAL_ERROR_COUNTER_GLOBAL_ERR_CNT_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: CNT_BIST_DBI_ERR_GLOBAL
 */
/* YDC_DDR_BIST :: CNT_BIST_DBI_ERR_GLOBAL :: VAL [31:00] */
#define YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL_VAL_ALIGN      0
#define YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL_VAL_BITS       32
#define YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL_VAL_SHIFT      0
#define YDC_DDR_BIST_CNT_BIST_DBI_ERR_GLOBAL_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_DBI_ERR_OCCURED
 */
/* YDC_DDR_BIST :: BIST_DBI_ERR_OCCURED :: reserved0 [31:04] */
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_RESERVED0_MASK    0xfffffff0
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_RESERVED0_ALIGN   0
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_RESERVED0_BITS    28
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_RESERVED0_SHIFT   4
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_RESERVED0_DEFAULT 0x00000000

/* YDC_DDR_BIST :: BIST_DBI_ERR_OCCURED :: VAL [03:00] */
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_VAL_MASK          0x0000000f
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_VAL_ALIGN         0
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_VAL_BITS          4
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_VAL_SHIFT         0
#define YDC_DDR_BIST_BIST_DBI_ERR_OCCURED_VAL_DEFAULT       0x03ffffff


/****************************************************************************
 * YDC_DDR_BIST :: CNT_BIST_EDC_ERR_GLOBAL
 */
/* YDC_DDR_BIST :: CNT_BIST_EDC_ERR_GLOBAL :: VAL [31:00] */
#define YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL_VAL_ALIGN      0
#define YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL_VAL_BITS       32
#define YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL_VAL_SHIFT      0
#define YDC_DDR_BIST_CNT_BIST_EDC_ERR_GLOBAL_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_EDC_ERR_OCCURED
 */
/* YDC_DDR_BIST :: BIST_EDC_ERR_OCCURED :: reserved0 [31:04] */
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_RESERVED0_MASK    0xfffffff0
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_RESERVED0_ALIGN   0
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_RESERVED0_BITS    28
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_RESERVED0_SHIFT   4
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_RESERVED0_DEFAULT 0x00000000

/* YDC_DDR_BIST :: BIST_EDC_ERR_OCCURED :: VAL [03:00] */
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_VAL_MASK          0x0000000f
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_VAL_ALIGN         0
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_VAL_BITS          4
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_VAL_SHIFT         0
#define YDC_DDR_BIST_BIST_EDC_ERR_OCCURED_VAL_DEFAULT       0x03ffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_ADDR_ERR
 */
/* YDC_DDR_BIST :: BIST_LAST_ADDR_ERR :: reserved0 [31:27] */
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_RESERVED0_MASK      0xf8000000
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_RESERVED0_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_RESERVED0_BITS      5
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_RESERVED0_SHIFT     27
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_RESERVED0_DEFAULT   0x00000000

/* YDC_DDR_BIST :: BIST_LAST_ADDR_ERR :: VAL [26:00] */
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_VAL_MASK            0x07ffffff
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_VAL_ALIGN           0
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_VAL_BITS            27
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_VAL_SHIFT           0
#define YDC_DDR_BIST_BIST_LAST_ADDR_ERR_VAL_DEFAULT         0x03ffffff


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_LAST_DATA_ERR_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_LAST_DATA_ERR_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_MSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_MSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_31_TO_0
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_31_TO_0 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0_VAL_MASK       0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0_VAL_ALIGN      0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0_VAL_BITS       32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0_VAL_SHIFT      0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_31_TO_0_VAL_DEFAULT    0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_63_TO_32
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_63_TO_32 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_63_TO_32_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_95_TO_64
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_95_TO_64 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64_VAL_MASK      0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64_VAL_ALIGN     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64_VAL_BITS      32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64_VAL_SHIFT     0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_95_TO_64_VAL_DEFAULT   0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_127_TO_96
 */
/* YDC_DDR_BIST :: BIST_LAST_READ_DATA_LSB_127_TO_96 :: VAL [31:00] */
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96_VAL_MASK     0xffffffff
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96_VAL_ALIGN    0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96_VAL_BITS     32
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96_VAL_SHIFT    0
#define YDC_DDR_BIST_BIST_LAST_READ_DATA_LSB_127_TO_96_VAL_DEFAULT  0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: GDDR5_BIST_FINISH_PRD
 */
/* YDC_DDR_BIST :: GDDR5_BIST_FINISH_PRD :: reserved0 [31:08] */
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_RESERVED0_MASK       0xffffff00
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_RESERVED0_ALIGN      0
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_RESERVED0_BITS       24
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_RESERVED0_SHIFT      8
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_RESERVED0_DEFAULT    0x00000000

/* YDC_DDR_BIST :: GDDR5_BIST_FINISH_PRD :: VAL [07:00] */
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_VAL_MASK             0x000000ff
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_VAL_ALIGN            0
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_VAL_BITS             8
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_VAL_SHIFT            0
#define YDC_DDR_BIST_GDDR5_BIST_FINISH_PRD_VAL_DEFAULT          0x00000000


/****************************************************************************
 * YDC_DDR_BIST :: INTIAL_CALIB_USE_MPR
 */
/* YDC_DDR_BIST :: INTIAL_CALIB_USE_MPR :: reserved0 [31:08] */
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_RESERVED0_MASK                0xfffffffe
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_RESERVED0_ALIGN               0
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_RESERVED0_BITS                31
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_RESERVED0_SHIFT               1
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_RESERVED0_DEFAULT             0x00000000

/* YDC_DDR_BIST :: INTIAL_CALIB_USE_MPR :: INTIAL_CALIB_READ_MPR [00:00] */
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_INTIAL_CALIB_READ_MPR_MASK    0x00000001
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_INTIAL_CALIB_READ_MPR_ALIGN   0
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_INTIAL_CALIB_READ_MPR_BITS    1
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_INTIAL_CALIB_READ_MPR_SHIFT   0
#define YDC_DDR_BIST_INTIAL_CALIB_USE_MPR_INTIAL_CALIB_READ_MPR_DEFAULT 0x00000000


/****************************************************************************
 * Datatype Definitions.
 */
#endif /* #ifndef YDC_DDR_BIST_BIST_H__ */

/* End of File */

