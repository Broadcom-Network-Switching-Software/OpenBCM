/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        katana.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <soc/katana.h>
#include <soc/katana2.h>
#include <soc/saber2.h>
#include <soc/trident.h>
#include <soc/bradley.h>
#include <soc/enduro.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/shmoo_ddr40.h>
#include <soc/phy/ddr40.h>
#include <soc/mspi.h>
#include <soc/post.h>
#include <soc/soc_ser_log.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_KATANA_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME SOC_DBG_COSQ

/* Min/Max threshold settings - 33 Gbps */
#define KT_QUEUE_FLUSH_RATE_MANTISSA    1023
#define KT_QUEUE_FLUSH_RATE_EXP         14
#define KT_QUEUE_FLUSH_BURST_MANTISSA   124
#define KT_QUEUE_FLUSH_BURST_EXP        14
#define KT_QUEUE_FLUSH_CYCLE_SEL        0

#define KT_OAM_INTR_MASK               0xFF
#define KT_OAM_CMIC_INTR_MASK          0x00000010

#define MAX_BUCKET_VAL 0x1fffff
static katanaShaperInfo_t    *shaper_info[SOC_MAX_NUM_DEVICES];
extern soc_kt_oam_handler_t en_oam_handler[SOC_MAX_NUM_DEVICES];

typedef struct _soc_katana_ci_parama_s {
    uint32          speed;
    uint32          grade;
} _soc_katana_ci_parama_t;

typedef enum {
    _SOC_PARITY_TYPE_NONE,
    _SOC_PARITY_TYPE_GENERIC,
    _SOC_PARITY_TYPE_PARITY,
    _SOC_PARITY_TYPE_ECC,
    _SOC_PARITY_TYPE_HASH,
    _SOC_PARITY_TYPE_EDATABUF,
    _SOC_PARITY_TYPE_COUNTER,
    _SOC_PARITY_TYPE_MXQPORT,
    _SOC_PARITY_TYPE_SER
} _soc_katana_parity_info_type_t;

typedef enum {
    _SOC_KT_MMU_SUBBLOCK_MEM1,
    _SOC_KT_MMU_SUBBLOCK_INTFI,
    _SOC_KT_MMU_SUBBLOCK_INTFI_ECC,
    _SOC_KT_MMU_SUBBLOCK_E2EFC,
    _SOC_KT_MMU_SUBBLOCK_ITE,
    _SOC_KT_MMU_SUBBLOCK_ITE_CFG,
    _SOC_KT_MMU_SUBBLOCK_THDI,
    _SOC_KT_MMU_SUBBLOCK_ENQ_CFG,
    _SOC_KT_MMU_SUBBLOCK_ENQ,
    _SOC_KT_MMU_SUBBLOCK_ENQ_FAP,
    _SOC_KT_MMU_SUBBLOCK_CI2,
    _SOC_KT_MMU_SUBBLOCK_CI1,
    _SOC_KT_MMU_SUBBLOCK_CI0,
    _SOC_KT_MMU_SUBBLOCK_WRED,
    _SOC_KT_MMU_SUBBLOCK_THDO_STATUS1,
    _SOC_KT_MMU_SUBBLOCK_THDO_STATUS2,
    _SOC_KT_MMU_SUBBLOCK_RQE_SER,
    _SOC_KT_MMU_SUBBLOCK_RQE_EXTQ,
    _SOC_KT_MMU_SUBBLOCK_EMC,
    _SOC_KT_MMU_SUBBLOCK_DEQ,
    _SOC_KT_MMU_SUBBLOCK_CTR,
    _SOC_KT_MMU_SUBBLOCK_CFAPI,
    _SOC_KT_MMU_SUBBLOCK_CFAPE,
    _SOC_KT_MMU_SUBBLOCK_CCP,
    _SOC_KT_MMU_SUBBLOCK_AGING_INT,
    _SOC_KT_MMU_SUBBLOCK_AGING_EXT,
    _SOC_KT_MMU_SUBBLOCK_LLS,
    _SOC_KT_MMU_SUBBLOCK_LLS_PORT,
    _SOC_KT_MMU_SUBBLOCK_LLS_L0_ECC,
    _SOC_KT_MMU_SUBBLOCK_LLS_UPD2,
    _SOC_KT_MMU_SUBBLOCK_LLS_L1_ECC,
    _SOC_KT_MMU_SUBBLOCK_LLS_L2_ECC,
    _SOC_KT_MMU_SUBBLOCK_LLS_MISC_ECC,
    _SOC_KT_MMU_SUBBLOCK_QSTRUCT,
    _SOC_KT_MMU_SUBBLOCK_TOQ,
    _SOC_KT_MMU_SUBBLOCK_MAX
} _soc_katana_mmu_subblock_type_t;


typedef enum {
    _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_0,
    _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_1,
    _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_2,
    _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_0,
    _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_1,
    _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_2,
    _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_3,
    _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_U,
    _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_L,
    _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QBLOCK,
    _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_FAP,
    _SOC_KT_MMU_LVL2_SUBBLOCK_MAX
} _soc_katana_mmu_lvl2_subblock_type_t;

typedef struct _soc_katana_parity_reg_s {
    soc_reg_t               reg;
    char                    *mem_str;
} _soc_katana_parity_reg_t;

typedef struct _soc_katana_parity_info_s {
    _soc_katana_parity_info_type_t   type;
    struct _soc_katana_parity_info_s *info;
    int                               id;
    soc_field_t                       group_reg_enable_field;
    soc_field_t                       group_reg_status_field;
    soc_mem_t                         mem;
    char                              *mem_str;
    soc_reg_t                         enable_reg;
    soc_field_t                       enable_field;
    soc_reg_t                         intr_status_reg;
    _soc_katana_parity_reg_t         *intr_status_reg_list;
    soc_reg_t                         nack_status_reg;
    _soc_katana_parity_reg_t         *nack_status_reg_list;
} _soc_katana_parity_info_t;

typedef struct _soc_katana_parity_route_block_s {
    uint32             cmic_bit;
    soc_block_t        blocktype;
    soc_reg_t          enable_reg;
    soc_reg_t          status_reg;
    _soc_katana_parity_info_t *info;
    uint8              id;
} _soc_katana_parity_route_block_t;

typedef struct _soc_katana_parity_mmu_route_block_s {
    uint32             intr_bit;
    _soc_katana_mmu_subblock_type_t sub_block;
    soc_reg_t          enable_reg;
    soc_reg_t          status_reg;
    _soc_katana_parity_info_t *info;
} _soc_katana_parity_mmu_route_block_t;

typedef struct _soc_katana_mmu_sub_block_type_1_info_s {
    uint32            intr_bit;
    soc_field_t       sub_block_field;
    soc_field_t       sub_block_en_field;
} _soc_katana_mmu_sub_block_type_1_info_t;

typedef struct _soc_katana_mmu_sub_block_type_2_info_s {
    uint32            intr_bit;
    uint32            en_bit;
    soc_field_t       sub_block_field;
    soc_field_t       sub_block_en_field;
} _soc_katana_mmu_sub_block_type_2_info_t;

typedef struct _soc_katana_mmu_sub_block_info_s {
    uint32            intr_bit;
    soc_reg_t         sub_block_reg;
    soc_reg_t         sub_block_en_reg;
    soc_field_t       sub_block_field;
    soc_field_t       sub_block_en_field;
    _soc_katana_mmu_sub_block_type_2_info_t *info;
} _soc_katana_mmu_sub_block_info_t;

typedef struct _soc_katana_mmu_sub_block_type_3_info_s {
    uint32            intr_bit;
    _soc_katana_mmu_lvl2_subblock_type_t lvl2_sub_block;
    soc_reg_t         sub_block_reg;
    soc_reg_t         sub_block_en_reg;
    soc_field_t       sub_block_field;
    soc_field_t       sub_block_en_field;
    _soc_katana_mmu_sub_block_type_1_info_t *info;
} _soc_katana_mmu_sub_block_type_3_info_t;


typedef struct _soc_katana_mmu_sub_block_type_1_s {
    uint32             mmu_bit;
    _soc_katana_mmu_subblock_type_t mmu_sub_block;
    soc_reg_t         sub_block_en_reg;
    soc_reg_t         sub_block_reg;
    _soc_katana_mmu_sub_block_type_1_info_t *info;
} _soc_katana_mmu_sub_block_type_1_t;

typedef struct _soc_katana_mmu_sub_block_type_2_s {
    uint32             mmu_bit;
    _soc_katana_mmu_subblock_type_t mmu_sub_block;
    soc_reg_t         sub_block_en_reg;
    soc_reg_t         sub_block_reg;
    _soc_katana_mmu_sub_block_type_2_info_t *info;
} _soc_katana_mmu_sub_block_type_2_t;

typedef struct _soc_katana_mmu_sub_block_type_3_s {
    uint32             mmu_bit;
    _soc_katana_mmu_subblock_type_t mmu_sub_block;
    soc_reg_t         sub_block_en_reg;
    soc_reg_t         sub_block_reg;
    _soc_katana_mmu_sub_block_type_3_info_t *info;
} _soc_katana_mmu_sub_block_type_3_t;

typedef struct _soc_katana_mmu_sub_block_s {
    uint32             mmu_bit;
    _soc_katana_mmu_subblock_type_t mmu_sub_block;
    soc_reg_t         sub_block_reg;
    soc_reg_t         sub_block_en_reg;
    _soc_katana_mmu_sub_block_info_t *info;
} _soc_katana_mmu_sub_block_t;


typedef enum {
    _SOC_COUNTER_TYPE_DROP_ENQ,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ,
    _SOC_COUNTER_TYPE_DROP_ENQ_GREEN,
    _SOC_COUNTER_TYPE_DROP_ENQ_YELLOW,
    _SOC_COUNTER_TYPE_DROP_ENQ_RED,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ_YELLOW,
    _SOC_COUNTER_TYPE_ACCEPT_ENQ_RED,
    _SOC_COUNTER_TYPE_ACCEPT_DEQ,
    _SOC_COUNTER_TYPE_MAX
} _soc_katana_counter_type_t;

typedef struct _soc_katana_counter_info_s {
    _soc_katana_counter_type_t type;
    int index;
    int segment;
}_soc_katana_counter_info_t;
    
STATIC _soc_katana_parity_reg_t _soc_katana_egr_vlan_xlate_intr_reg[] = {
    { EGR_VLAN_XLATE_PARITY_STATUS_INTR_0r, NULL },
    { EGR_VLAN_XLATE_PARITY_STATUS_INTR_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_egr_vlan_xlate_nack_reg[] = {
    { EGR_VLAN_XLATE_PARITY_STATUS_NACK_0r, NULL },
    { EGR_VLAN_XLATE_PARITY_STATUS_NACK_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_vlan_xlate_intr_reg[] = {
    { VLAN_XLATE_PARITY_STATUS_INTR_0r, NULL },
    { VLAN_XLATE_PARITY_STATUS_INTR_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_vlan_xlate_nack_reg[] = {
    { VLAN_XLATE_PARITY_STATUS_NACK_0r, NULL },
    { VLAN_XLATE_PARITY_STATUS_NACK_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_mpls_entry_intr_reg[] = {
    { MPLS_ENTRY_PARITY_STATUS_INTR_0r, NULL },
    { MPLS_ENTRY_PARITY_STATUS_INTR_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_mpls_entry_nack_reg[] = {
    { MPLS_ENTRY_PARITY_STATUS_NACK_0r, NULL },
    { MPLS_ENTRY_PARITY_STATUS_NACK_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_l3_entry_ipv6_unicast_intr_reg[] = {
    { L3_ENTRY_PARITY_STATUS_INTR_0r, NULL },
    { L3_ENTRY_PARITY_STATUS_INTR_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_l3_entry_ipv6_unicast_nack_reg[] = {
    { L3_ENTRY_PARITY_STATUS_NACK_0r, NULL },
    { L3_ENTRY_PARITY_STATUS_NACK_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_l2_entry_only_intr_reg[] = {
    { L2_ENTRY_PARITY_STATUS_INTR_0r, NULL },
    { L2_ENTRY_PARITY_STATUS_INTR_1r, NULL } };
STATIC _soc_katana_parity_reg_t _soc_katana_l2_entry_only_nack_reg[] = {
    { L2_ENTRY_PARITY_STATUS_NACK_0r, NULL },
    { L2_ENTRY_PARITY_STATUS_NACK_1r, NULL } };

#ifdef _KATANA_DEBUG
STATIC _soc_katana_parity_info_t _soc_katana_xport_parity_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        TXFIFO_MEM_ERRf,
        TXFIFO_MEM_ERRf,
        INVALIDm, "TX FIFO",
        INVALIDr, INVALIDf,
        XPORT_TXFIFO_MEM_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TSC_MEM0_ERRf,
        MIB_TSC_MEM0_ERRf,
        INVALIDm, "TX MIB MEM0",
        INVALIDr, INVALIDf,
        XPORT_MIB_TSC_MEM0_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TSC_MEM1_ERRf,
        MIB_TSC_MEM1_ERRf,
        INVALIDm, "TX MIB MEM1",
        INVALIDr, INVALIDf,
        XPORT_MIB_TSC_MEM1_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TSC_MEM2_ERRf,
        MIB_TSC_MEM2_ERRf,
        INVALIDm, "TX MIB MEM2",
        INVALIDr, INVALIDf,
        XPORT_MIB_TSC_MEM2_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TSC_MEM3_ERRf,
        MIB_TSC_MEM3_ERRf,
        INVALIDm, "TX MIB MEM3",
        INVALIDr, INVALIDf,
        XPORT_MIB_TSC_MEM3_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RSC_MEM0_ERRf,
        MIB_RSC_MEM0_ERRf,
        INVALIDm, "RX MIB MEM0",
        INVALIDr, INVALIDf,
        XPORT_MIB_RSC_MEM0_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RSC_MEM1_ERRf,
        MIB_RSC_MEM1_ERRf,
        INVALIDm, "RX MIB MEM1",
        INVALIDr, INVALIDf,
        XPORT_MIB_RSC_MEM1_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RSC_MEM2_ERRf,
        MIB_RSC_MEM2_ERRf,
        INVALIDm, "RX MIB MEM2",
        INVALIDr, INVALIDf,
        XPORT_MIB_RSC_MEM2_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RSC_MEM3_ERRf,
        MIB_RSC_MEM3_ERRf,
        INVALIDm, "RX MIB MEM3",
        INVALIDr, INVALIDf,
        XPORT_MIB_RSC_MEM3_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RSC_MEM4_ERRf,
        MIB_RSC_MEM4_ERRf,
        INVALIDm, "RX MIB MEM4",
        INVALIDr, INVALIDf,
        XPORT_MIB_RSC_MEM4_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        RXFIFO_MEM0_ERRf,
        RXFIFO_MEM0_ERRf,
        INVALIDm, "RX FIFO MEM0",
        INVALIDr, INVALIDf,
        XPORT_RXFIFO_MEM0_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        RXFIFO_MEM1_ERRf,
        RXFIFO_MEM1_ERRf,
        INVALIDm, "RX FIFO MEM1",
        INVALIDr, INVALIDf,
        XPORT_RXFIFO_MEM1_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        RXFIFO_MEM2_ERRf,
        RXFIFO_MEM2_ERRf,
        INVALIDm, "RX FIFO MEM2",
        INVALIDr, INVALIDf,
        XPORT_RXFIFO_MEM2_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        RXFIFO_MEM3_ERRf,
        RXFIFO_MEM3_ERRf,
        INVALIDm, "RX FIFO MEM3",
        INVALIDr, INVALIDf,
        XPORT_RXFIFO_MEM3_ECC_STATUSr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};
#endif

#ifdef _KATANA_DEBUG
STATIC _soc_katana_parity_info_t _soc_katana_mmu_parity_info[] = {
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        AGING_CTR_PAR_ERR_ENf,
        AGING_CTR_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        AGING_EXP_PAR_ERR_ENf,
        AGING_EXP_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        CCP_PAR_ERR_ENf,
        CCP_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        CFAP_PAR_ERR_ENf,
        CFAP_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        CFAP_MEM_FAIL_ENf,
        CFAP_MEM_FAILf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        DEQ_MPB_ERR_ENf,
        DEQ_MPB_ERRf,
        INVALIDm, "",
        MEM_FAIL_INT_ENr, DEQ_MPB_ERR_ENf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        DEQ_PKTHDR_ERR_ENf,
        DEQ_PKTHDR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        DEQ_NOT_IP_ERR_ENf,
        DEQ_NOT_IP_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        CTR_UC_CNT_ENf,
        CTR_UC_CNTf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        START_BY_START_ERR_ENf,
        START_BY_START_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        CTR_MC_CNT_ENf,
        CTR_MC_CNTf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        CTR_COLOR_CNT_ERR_ENf,
        CTR_COLOR_CNT_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_UCQ_RP_PAR_ERR_ENf,
        TOQ0_UCQ_RP_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_UCQ_WP_PAR_ERR_ENf,
        TOQ0_UCQ_WP_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_PKTLINK_PAR_ERR_ENf,
        TOQ0_PKTLINK_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_CELLLINK_PAR_ERR_ENf,
        TOQ0_CELLLINK_PAR_ERRf,
         INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_CPQLINK_PAR_ERR_ENf,
        TOQ0_CPQLINK_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_IPMC_MC_FIFO_PAR_ERR_ENf,
        TOQ0_IPMC_MC_FIFO_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_IPMC_TBL_PAR_ERR_ENf,
        TOQ0_IPMC_TBL_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        TOQ0_VLAN_TBL_PAR_ERR_ENf,
        TOQ0_VLAN_TBL_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        MTRO_PAR_ERR_ENf,
        MTRO_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        WRED_PAR_ERR_ENf,
        WRED_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        OVQ_PAR_ERR_ENf,
        OVQ_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        PQE_PAR_ERR_ENf,
        PQE_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        QCN_PAR_ERR_ENf,
        QCN_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        THDO_PAR_ERR_ENf,
        THDO_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        THDI_PAR_ERR_ENf,
        THDI_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        ES_PAR_ERR_ENf,
        ES_PAR_ERRf,
        INVALIDm, "",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};
#endif


STATIC 
_soc_katana_mmu_sub_block_type_1_info_t _soc_katana_mmu_sb_lls_parity_info[] = {
    /* LLS_ERROR */
    { 0x10000000, ENQ_TO_ACTIVE_QUEUE_VIOL_INTERRUPTf,
       ENQ_TO_ACTIVE_QUEUE_VIOL_INTERRUPT_DISINTf},
    { 0x8000000, PORT_1_IN_4_DEQ_VIOL_INTERRUPTf,
       PORT_1_IN_4_DEQ_VIOL_INTERRUPT_DISINTf},
    { 0x4000000, PORT_MAX_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     PORT_MAX_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x2000000, L0_MAX_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L0_MAX_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x1000000, L1_MAX_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L1_MAX_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x800000, L2_UPPER_MAX_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L2_UPPER_MAX_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x400000, L2_LOWER_MAX_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L2_LOWER_MAX_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x200000, L0_MIN_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L0_MIN_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x100000, L1_MIN_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L1_MIN_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x80000, L2_UPPER_MIN_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L2_UPPER_MIN_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},
    { 0x40000, L2_LOWER_MIN_SHAPER_REFRESH_CYCLE_INTERRUPTf,
	     L2_LOWER_MIN_SHAPER_REFRESH_CYCLE_INTERRUPT_DISINTf},

    /* The following fields need to be left with reset value in
     * lls_error_mask as recommended. These fields are used only
     * for debugging */ 

    /*
    { 0x20000, MAX_SHAPER_BUCKET_UNDERFLOW_PORTf,
	     MAX_SHAPER_BUCKET_UNDERFLOW_PORT_DISINTf},
    { 0x10000, MAX_SHAPER_BUCKET_UNDERFLOW_L0f,
	     MAX_SHAPER_BUCKET_UNDERFLOW_L0_DISINTf},
    { 0x8000, MAX_SHAPER_BUCKET_UNDERFLOW_L1f,
	     MAX_SHAPER_BUCKET_UNDERFLOW_L1_DISINTf},
    { 0x4000, MAX_SHAPER_BUCKET_UNDERFLOW_L2_UPPERf,
	     MAX_SHAPER_BUCKET_UNDERFLOW_L2_UPPER_DISINTf},
    { 0x2000, MAX_SHAPER_BUCKET_UNDERFLOW_L2_LOWERf,
	     MAX_SHAPER_BUCKET_UNDERFLOW_L2_LOWER_DISINTf},
    { 0x1000, MIN_SHAPER_BUCKET_UNDERFLOW_L0f,
	     MIN_SHAPER_BUCKET_UNDERFLOW_L0_DISINTf},
    { 0x0800, MIN_SHAPER_BUCKET_UNDERFLOW_L1f,
	     MIN_SHAPER_BUCKET_UNDERFLOW_L1_DISINTf},
    { 0x0400, MIN_SHAPER_BUCKET_UNDERFLOW_L2_UPPERf,
	     MIN_SHAPER_BUCKET_UNDERFLOW_L2_UPPER_DISINTf},
    { 0x0200, MIN_SHAPER_BUCKET_UNDERFLOW_L2_LOWERf,
	     MIN_SHAPER_BUCKET_UNDERFLOW_L2_LOWER_DISINTf},
    */
    { 0x0100, MAX_SHAPER_BUCKET_OVERFLOW_PORTf,
	     MAX_SHAPER_BUCKET_OVERFLOW_PORT_DISINTf},
    { 0x0080, MAX_SHAPER_BUCKET_OVERFLOW_L0f,
	     MAX_SHAPER_BUCKET_OVERFLOW_L0_DISINTf},
    { 0x0040, MAX_SHAPER_BUCKET_OVERFLOW_L1f,
	     MAX_SHAPER_BUCKET_OVERFLOW_L1_DISINTf},
    { 0x0020, MAX_SHAPER_BUCKET_OVERFLOW_L2_UPPERf,
	     MAX_SHAPER_BUCKET_OVERFLOW_L2_UPPER_DISINTf},
    { 0x0010, MAX_SHAPER_BUCKET_OVERFLOW_L2_LOWERf,
	     MAX_SHAPER_BUCKET_OVERFLOW_L2_LOWER_DISINTf},
    { 0x0008, MIN_SHAPER_BUCKET_OVERFLOW_L0f,
	     MIN_SHAPER_BUCKET_OVERFLOW_L0_DISINTf},
    { 0x0004, MIN_SHAPER_BUCKET_OVERFLOW_L1f,
	     MIN_SHAPER_BUCKET_OVERFLOW_L1_DISINTf},
    { 0x0002, MIN_SHAPER_BUCKET_OVERFLOW_L2_UPPERf,
	     MIN_SHAPER_BUCKET_OVERFLOW_L2_UPPER_DISINTf},
    { 0x0001, MIN_SHAPER_BUCKET_OVERFLOW_L2_LOWERf,
	     MIN_SHAPER_BUCKET_OVERFLOW_L2_LOWER_DISINTf},
    { 0 } /* table terminator */ 
};
       /* LLS_ERROR_UPD2 */
STATIC _soc_katana_mmu_sub_block_type_1_info_t 
                  _soc_katana_mmu_sb_lls_upd2_parity_info [] = {
    { 0x0100, UPD2_PORT_1_IN_4_VIOLATED_INTERRUPTf,
	     UPD2_PORT_1_IN_4_VIOLATED_INTERRUPT_DISINTf},

    /* The following fields need to be left with reset value in
     * lls_error_upd2_mask as recommended. These fields are used only
     * for debugging */ 

/*
    { 0x0080, UPD2_PORT_ERROR_UNDERRUN_INTERRUPTf,
	     UPD2_PORT_ERROR_UNDERRUN_INTERRUPT_DISINTf},
    { 0x0040, UPD2_PORT_ERROR_OVERFLOW_INTERRUPTf,
	     UPD2_PORT_ERROR_OVERFLOW_INTERRUPT_DISINTf},
    { 0x0020, UPD2_L0_ERROR_UNDERRUN_INTERRUPTf,
	     UPD2_L0_ERROR_UNDERRUN_INTERRUPT_DISINTf},
    { 0x0010, UPD2_L0_ERROR_OVERFLOW_INTERRUPTf,
	     UPD2_L0_ERROR_OVERFLOW_INTERRUPT_DISINTf},
    { 0x0008, UPD2_L1_ERROR_UNDERRUN_INTERRUPTf,
	     UPD2_L1_ERROR_UNDERRUN_INTERRUPT_DISINTf},
    { 0x0004, UPD2_L1_ERROR_OVERFLOW_INTERRUPTf,
	     UPD2_L1_ERROR_OVERFLOW_INTERRUPT_DISINTf},
    { 0x0002, UPD2_L2_ERROR_UNDERRUN_INTERRUPTf,
	     UPD2_L2_ERROR_UNDERRUN_INTERRUPT_DISINTf},
    { 0x0001, UPD2_L2_ERROR_OVERFLOW_INTERRUPTf,
	     UPD2_L2_ERROR_OVERFLOW_INTERRUPT_DISINTf},
*/
    { 0 } /* table terminator */

};
STATIC _soc_katana_mmu_sub_block_type_1_info_t 
                  _soc_katana_mmu_sb_lls_port_parity_info [] = {
       /* LLS_PORT_ECC_ERROR */
    { 0x0020, DB_PORT_TDM_CORRECTED_ERRORf,
  	      DB_PORT_TDM_CORRECTED_ERROR_DISINTf},
    { 0x0010, DB_PORT_TDM_UNCORRECTED_ERRORf,
  	      DB_PORT_TDM_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, DB_PORT_SHAPER_CONFIG_C_CORRECTED_ERRORf,
  	      DB_PORT_SHAPER_CONFIG_C_CORRECTED_ERROR_DISINTf},
    { 0x0004, DB_PORT_SHAPER_CONFIG_C_UNCORRECTED_ERRORf,
  	      DB_PORT_SHAPER_CONFIG_C_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, DB_PORT_WERR_MAX_SC_CORRECTED_ERRORf,
  	      DB_PORT_WERR_MAX_SC_CORRECTED_ERROR_DISINTf},
    { 0x0001, DB_PORT_WERR_MAX_SC_UNCORRECTED_ERRORf,
  	      DB_PORT_WERR_MAX_SC_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */

};
STATIC _soc_katana_mmu_sub_block_type_1_info_t
                _soc_katana_mmu_sb_lls_l0_ecc_parity_info [] = {
         /* LLS_L0_ECC_ERROR1 */
    { 0x8000000, DB_L0_MIN_NEXT_CORRECTED_ERRORf,
  	      DB_L0_MIN_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x4000000, DB_L0_MIN_NEXT_UNCORRECTED_ERRORf,
  	      DB_L0_MIN_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0x2000000, DB_L0_PARENT_CORRECTED_ERRORf,
  	      DB_L0_PARENT_CORRECTED_ERROR_DISINTf},
    { 0x1000000, DB_L0_PARENT_UNCORRECTED_ERRORf,
  	      DB_L0_PARENT_UNCORRECTED_ERROR_DISINTf},
    { 0x800000, DB_L0_CONFIG_CORRECTED_ERRORf,
  	      DB_L0_CONFIG_CORRECTED_ERROR_DISINTf},
    { 0x400000, DB_L0_CONFIG_UNCORRECTED_ERRORf,
  	      DB_L0_CONFIG_UNCORRECTED_ERROR_DISINTf},
    { 0x200000, DB_L0_MIN_CONFIG_C_CORRECTED_ERRORf,
  	      DB_L0_MIN_CONFIG_C_CORRECTED_ERROR_DISINTf},
    { 0x100000, DB_L0_MIN_CONFIG_C_UNCORRECTED_ERRORf,
  	      DB_L0_MIN_CONFIG_C_UNCORRECTED_ERROR_DISINTf},
    { 0x80000, DB_L0_PARENT_STATE_CORRECTED_ERRORf,
  	      DB_L0_PARENT_STATE_CORRECTED_ERROR_DISINTf},
    { 0x40000, DB_L0_PARENT_STATE_UNCORRECTED_ERRORf,
  	      DB_L0_PARENT_STATE_UNCORRECTED_ERROR_DISINTf},
    { 0x20000, DB_L0_HEADS_TAILS_CORRECTED_ERRORf,
  	      DB_L0_HEADS_TAILS_CORRECTED_ERROR_DISINTf},
    { 0x10000, DB_L0_HEADS_TAILS_UNCORRECTED_ERRORf,
  	      DB_L0_HEADS_TAILS_UNCORRECTED_ERROR_DISINTf},
    { 0x8000, DB_L0_WERR_MAX_SC_CORRECTED_ERRORf,
  	      DB_L0_WERR_MAX_SC_CORRECTED_ERROR_DISINTf},
    { 0x4000, DB_L0_WERR_MAX_SC_UNCORRECTED_ERRORf,
  	      DB_L0_WERR_MAX_SC_UNCORRECTED_ERROR_DISINTf},
    { 0x2000, DB_L0_MIN_BUCKET_C_CORRECTED_ERRORf,
  	      DB_L0_MIN_BUCKET_C_CORRECTED_ERROR_DISINTf},
    { 0x1000, DB_L0_MIN_BUCKET_C_UNCORRECTED_ERRORf,
  	      DB_L0_MIN_BUCKET_C_UNCORRECTED_ERROR_DISINTf},
    { 0x0800, DB_L0_CHILD_STATE1_CORRECTED_ERRORf,
  	      DB_L0_CHILD_STATE1_CORRECTED_ERROR_DISINTf},
    { 0x0400, DB_L0_CHILD_STATE1_UNCORRECTED_ERRORf,
  	      DB_L0_CHILD_STATE1_UNCORRECTED_ERROR_DISINTf},
    { 0x0200, DB_L0_CHILD_WEIGHT_CFG_CNT_CORRECTED_ERRORf,
  	      DB_L0_CHILD_WEIGHT_CFG_CNT_CORRECTED_ERROR_DISINTf},
    { 0x0100, DB_L0_CHILD_WEIGHT_CFG_CNT_UNCORRECTED_ERRORf,
  	      DB_L0_CHILD_WEIGHT_CFG_CNT_UNCORRECTED_ERROR_DISINTf},
    { 0x0080, DB_L0_WERR_NEXT_CORRECTED_ERRORf,
  	      DB_L0_WERR_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x0040, DB_L0_WERR_NEXT_UNCORRECTED_ERRORf,
  	      DB_L0_WERR_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0x0020, DB_L0_EF_NEXT_CORRECTED_ERRORf,
  	      DB_L0_EF_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x0010, DB_L0_EF_NEXT_UNCORRECTED_ERRORf,
  	      DB_L0_EF_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, DB_L0_SHAPER_CONFIG_C_CORRECTED_ERRORf,
  	      DB_L0_SHAPER_CONFIG_C_CORRECTED_ERROR_DISINTf},
    { 0x0004, DB_L0_SHAPER_CONFIG_C_UNCORRECTED_ERRORf,
  	      DB_L0_SHAPER_CONFIG_C_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, DB_L0_SHAPER_BUCKET_C_CORRECTED_ERRORf,
  	      DB_L0_SHAPER_BUCKET_C_CORRECTED_ERROR_DISINTf},
    { 0x0001, DB_L0_SHAPER_BUCKET_C_UNCORRECTED_ERRORf,
  	      DB_L0_SHAPER_BUCKET_C_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */

};
STATIC _soc_katana_mmu_sub_block_type_1_info_t 
                     _soc_katana_mmu_sb_lls_l1_ecc_parity_info [] = {
             /* LLS_L1_ECC_ERROR1 */
    { 0x8000000, DB_L1_MIN_NEXT_CORRECTED_ERRORf,
  	      DB_L1_MIN_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x4000000, DB_L1_MIN_NEXT_UNCORRECTED_ERRORf,
  	      DB_L1_MIN_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0x2000000, DB_L1_PARENT_CORRECTED_ERRORf,
  	      DB_L1_PARENT_CORRECTED_ERROR_DISINTf},
    { 0x1000000, DB_L1_PARENT_UNCORRECTED_ERRORf,
  	      DB_L1_PARENT_UNCORRECTED_ERROR_DISINTf},
    { 0x800000, DB_L1_CONFIG_CORRECTED_ERRORf,
  	      DB_L1_CONFIG_CORRECTED_ERROR_DISINTf},
    { 0x400000, DB_L1_CONFIG_UNCORRECTED_ERRORf,
  	      DB_L1_CONFIG_UNCORRECTED_ERROR_DISINTf},
    { 0x200000, DB_L1_MIN_CONFIG_C_CORRECTED_ERRORf,
  	      DB_L1_MIN_CONFIG_C_CORRECTED_ERROR_DISINTf},
    { 0x100000, DB_L1_MIN_CONFIG_C_UNCORRECTED_ERRORf,
  	      DB_L1_MIN_CONFIG_C_UNCORRECTED_ERROR_DISINTf},
    { 0x80000, DB_L1_PARENT_STATE_CORRECTED_ERRORf,
  	      DB_L1_PARENT_STATE_CORRECTED_ERROR_DISINTf},
    { 0x40000, DB_L1_PARENT_STATE_UNCORRECTED_ERRORf,
  	      DB_L1_PARENT_STATE_UNCORRECTED_ERROR_DISINTf},
    { 0x20000, DB_L1_HEADS_TAILS_CORRECTED_ERRORf,
  	      DB_L1_HEADS_TAILS_CORRECTED_ERROR_DISINTf},
    { 0x10000, DB_L1_HEADS_TAILS_UNCORRECTED_ERRORf,
  	      DB_L1_HEADS_TAILS_UNCORRECTED_ERROR_DISINTf},
    { 0x8000, DB_L1_WERR_MAX_SC_CORRECTED_ERRORf,
  	      DB_L1_WERR_MAX_SC_CORRECTED_ERROR_DISINTf},
    { 0x4000, DB_L1_WERR_MAX_SC_UNCORRECTED_ERRORf,
  	      DB_L1_WERR_MAX_SC_UNCORRECTED_ERROR_DISINTf},
    { 0x2000, DB_L1_MIN_BUCKET_C_CORRECTED_ERRORf,
  	      DB_L1_MIN_BUCKET_C_CORRECTED_ERROR_DISINTf},
    { 0x1000, DB_L1_MIN_BUCKET_C_UNCORRECTED_ERRORf,
  	      DB_L1_MIN_BUCKET_C_UNCORRECTED_ERROR_DISINTf},
    { 0x0800, DB_L1_CHILD_STATE1_CORRECTED_ERRORf,
  	      DB_L1_CHILD_STATE1_CORRECTED_ERROR_DISINTf},
    { 0x0400, DB_L1_CHILD_STATE1_UNCORRECTED_ERRORf,
  	      DB_L1_CHILD_STATE1_UNCORRECTED_ERROR_DISINTf},
    { 0x0200, DB_L1_CHILD_WEIGHT_CFG_CNT_CORRECTED_ERRORf,
  	      DB_L1_CHILD_WEIGHT_CFG_CNT_CORRECTED_ERROR_DISINTf},
    { 0x0100, DB_L1_CHILD_WEIGHT_CFG_CNT_UNCORRECTED_ERRORf,
  	      DB_L1_CHILD_WEIGHT_CFG_CNT_UNCORRECTED_ERROR_DISINTf},
    { 0x0080, DB_L1_WERR_NEXT_CORRECTED_ERRORf,
  	      DB_L1_WERR_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x0040, DB_L1_WERR_NEXT_UNCORRECTED_ERRORf,
  	      DB_L1_WERR_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0x0020, DB_L1_EF_NEXT_CORRECTED_ERRORf,
  	      DB_L1_EF_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x0010, DB_L1_EF_NEXT_UNCORRECTED_ERRORf,
  	     DB_L1_EF_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, DB_L1_SHAPER_CONFIG_C_CORRECTED_ERRORf,
  	      DB_L1_SHAPER_CONFIG_C_CORRECTED_ERROR_DISINTf},
    { 0x0004, DB_L1_SHAPER_CONFIG_C_UNCORRECTED_ERRORf,
  	      DB_L1_SHAPER_CONFIG_C_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, DB_L1_SHAPER_BUCKET_C_CORRECTED_ERRORf,
  	      DB_L1_SHAPER_BUCKET_C_CORRECTED_ERROR_DISINTf},
    { 0x0001, DB_L1_SHAPER_BUCKET_C_UNCORRECTED_ERRORf,
  	      DB_L1_SHAPER_BUCKET_C_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC _soc_katana_mmu_sub_block_type_1_info_t 
                     _soc_katana_mmu_sb_lls_l2_ecc_parity_info [] = {
             /*   LLS_L2_ECC_ERROR1 */
    { 0x2000000, DB_L2_MIN_CONFIG_UPPER_C_CORRECTED_ERRORf,
  	      DB_L2_MIN_CONFIG_UPPER_C_CORRECTED_ERROR_DISINTf},
    { 0x1000000, DB_L2_MIN_CONFIG_UPPER_C_UNCORRECTED_ERRORf,
  	      DB_L2_MIN_CONFIG_UPPER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x800000, DB_L2_MIN_BUCKET_UPPER_C_CORRECTED_ERRORf,
  	      DB_L2_MIN_BUCKET_UPPER_C_CORRECTED_ERROR_DISINTf},
    { 0x400000, DB_L2_MIN_BUCKET_UPPER_C_UNCORRECTED_ERRORf,
  	      DB_L2_MIN_BUCKET_UPPER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x200000, DB_L2_SHAPER_CONFIG_UPPER_C_CORRECTED_ERRORf,
  	      DB_L2_SHAPER_CONFIG_UPPER_C_CORRECTED_ERROR_DISINTf},
    { 0x100000, DB_L2_SHAPER_CONFIG_UPPER_C_UNCORRECTED_ERRORf,
  	      DB_L2_SHAPER_CONFIG_UPPER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x80000, DB_L2_SHAPER_BUCKET_UPPER_C_CORRECTED_ERRORf,
  	      DB_L2_SHAPER_BUCKET_UPPER_C_CORRECTED_ERROR_DISINTf},
    { 0x40000, DB_L2_SHAPER_BUCKET_UPPER_C_UNCORRECTED_ERRORf,
  	      DB_L2_SHAPER_BUCKET_UPPER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x20000, DB_L2_MIN_CONFIG_LOWER_C_CORRECTED_ERRORf,
  	      DB_L2_MIN_CONFIG_LOWER_C_CORRECTED_ERROR_DISINTf},
    { 0x10000, DB_L2_MIN_CONFIG_LOWER_C_UNCORRECTED_ERRORf,
  	      DB_L2_MIN_CONFIG_LOWER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x8000, DB_L2_MIN_BUCKET_LOWER_C_CORRECTED_ERRORf,
  	      DB_L2_MIN_BUCKET_LOWER_C_CORRECTED_ERROR_DISINTf},
    { 0x4000, DB_L2_MIN_BUCKET_LOWER_C_UNCORRECTED_ERRORf,
  	      DB_L2_MIN_BUCKET_LOWER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x2000, DB_L2_SHAPER_CONFIG_LOWER_C_CORRECTED_ERRORf,
  	      DB_L2_SHAPER_CONFIG_LOWER_C_CORRECTED_ERROR_DISINTf},
    { 0x1000, DB_L2_SHAPER_CONFIG_LOWER_C_UNCORRECTED_ERRORf,
  	      DB_L2_SHAPER_CONFIG_LOWER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x0800, DB_L2_SHAPER_BUCKET_LOWER_C_CORRECTED_ERRORf,
  	      DB_L2_SHAPER_BUCKET_LOWER_C_CORRECTED_ERROR_DISINTf},
    { 0x0400, DB_L2_SHAPER_BUCKET_LOWER_C_UNCORRECTED_ERRORf,
  	      DB_L2_SHAPER_BUCKET_LOWER_C_UNCORRECTED_ERROR_DISINTf},
    { 0x0200, DB_L2_CHILD_STATE1_CORRECTED_ERRORf,
  	      DB_L2_CHILD_STATE1_CORRECTED_ERROR_DISINTf},
    { 0x0100, DB_L2_CHILD_STATE1_UNCORRECTED_ERRORf,
  	      DB_L2_CHILD_STATE1_UNCORRECTED_ERROR_DISINTf},
    { 0x0080, DB_L2_PARENT_CORRECTED_ERRORf,
  	      DB_L2_PARENT_CORRECTED_ERROR_DISINTf},
    { 0x0040, DB_L2_PARENT_UNCORRECTED_ERRORf,
  	      DB_L2_PARENT_UNCORRECTED_ERROR_DISINTf},
    { 0x0020, DB_L2_CHILD_WEIGHT_CFG_CNT_CORRECTED_ERRORf,
  	      DB_L2_CHILD_WEIGHT_CFG_CNT_CORRECTED_ERROR_DISINTf},
    { 0x0010, DB_L2_CHILD_WEIGHT_CFG_CNT_UNCORRECTED_ERRORf,
  	      DB_L2_CHILD_WEIGHT_CFG_CNT_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, DB_L2_WERR_NEXT_CORRECTED_ERRORf,
  	      DB_L2_WERR_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x0004, DB_L2_WERR_NEXT_UNCORRECTED_ERRORf,
  	      DB_L2_WERR_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, DB_L2_MIN_NEXT_CORRECTED_ERRORf,
  	      DB_L2_MIN_NEXT_CORRECTED_ERROR_DISINTf},
    { 0x0001, DB_L2_MIN_NEXT_UNCORRECTED_ERRORf,
  	      DB_L2_MIN_NEXT_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC _soc_katana_mmu_sub_block_type_1_info_t 
                     _soc_katana_mmu_sb_lls_misc_ecc_parity_info [] = {
              /* LLS_MISC_ECC_ERROR1 */
    { 0x0800, DB_L2_ACT_XON_CORRECTED_ERRORf,
  	      DB_L2_ACT_XON_CORRECTED_ERROR_DISINTf},
    { 0x0400, DB_L2_ACT_XON_UNCORRECTED_ERRORf,
  	      DB_L2_ACT_XON_UNCORRECTED_ERROR_DISINTf},
    { 0x0200, DB_L2_ACT_SHAPER_CORRECTED_ERRORf,
  	      DB_L2_ACT_SHAPER_CORRECTED_ERROR_DISINTf},
    { 0x0100, DB_L2_ACT_SHAPER_UNCORRECTED_ERRORf,
  	      DB_L2_ACT_SHAPER_UNCORRECTED_ERROR_DISINTf},
    { 0x0080, DB_L2_ACT_MIN_CORRECTED_ERRORf,
  	      DB_L2_ACT_MIN_CORRECTED_ERROR_DISINTf},
    { 0x0040, DB_L2_ACT_MIN_UNCORRECTED_ERRORf,
  	      DB_L2_ACT_MIN_UNCORRECTED_ERROR_DISINTf},
    { 0x0020, DB_L0_ERROR_CORRECTED_ERRORf,
  	      DB_L0_ERROR_CORRECTED_ERROR_DISINTf},
    { 0x0010, DB_L0_ERROR_UNCORRECTED_ERRORf,
  	      DB_L0_ERROR_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, DB_L1_ERROR_CORRECTED_ERRORf,
  	      DB_L1_ERROR_CORRECTED_ERROR_DISINTf},
    { 0x0004, DB_L1_ERROR_UNCORRECTED_ERRORf,
  	      DB_L1_ERROR_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, DB_L2_ERROR_CORRECTED_ERRORf,
  	      DB_L2_ERROR_CORRECTED_ERROR_DISINTf},
    { 0x0001, DB_L2_ERROR_UNCORRECTED_ERRORf,
  	      DB_L2_ERROR_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */

};


STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                        _soc_katana_mmu_sb_mem1_parity_info [] = {
    { 0x0008, CLINKE_CORRECTED_ERRORf,
        CLINKE_CORRECTED_ERROR_DISINTf },
    { 0x0004, CLINKE_UNCORRECTED_ERRORf,
        CLINKE_UNCORRECTED_ERROR_DISINTf },
    { 0x0002, CLINKI_CORRECTED_ERRORf,
        CLINKI_CORRECTED_ERROR_DISINTf },
    { 0x0001, CLINKI_UNCORRECTED_ERRORf,
        CLINKI_UNCORRECTED_ERROR_DISINTf },
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t
                      _soc_katana_mmu_sb_e2efc_parity_info [] = {
    { 0x0200, E2EFC_QEN_CNT_UNDERRUNf,
        E2EFC_QEN_CNT_UNDERRUN_DISINTf},
    { 0x0100, E2EFC_QEN_CNT_OVERFLOWf,
        E2EFC_QEN_CNT_OVERFLOW_DISINTf},
    { 0x0080, E2EFC_RQE_CNT_UNDERRUNf,
        E2EFC_RQE_CNT_UNDERRUN_DISINTf},
    { 0x0040, E2EFC_RQE_CNT_OVERFLOWf,
        E2EFC_RQE_CNT_OVERFLOW_DISINTf},
    { 0x0020, E2EFC_EMA_CNT_UNDERRUNf,
        E2EFC_EMA_CNT_UNDERRUN_DISINTf},
    { 0x0010, E2EFC_EMA_CNT_OVERFLOWf,
        E2EFC_EMA_CNT_OVERFLOW_DISINTf},
    { 0x0008, E2EFC_EXT_CNT_UNDERRUNf,
        E2EFC_EXT_CNT_UNDERRUN_DISINTf},
    { 0x0004, E2EFC_EXT_CNT_OVERFLOWf,
        E2EFC_EXT_CNT_OVERFLOW_DISINTf},
    { 0x0002, E2EFC_INT_CNT_UNDERRUNf,
        E2EFC_INT_CNT_UNDERRUN_DISINTf},
    { 0x0001, E2EFC_INT_CNT_OVERFLOWf,
        E2EFC_INT_CNT_OVERFLOW_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                     _soc_katana_mmu_sb_thdi_parity_info [] = {
    { 0x0010, THDI_INTR_4f, THDI_INTR_4_DISINTf},
    { 0x0008, THDI_INTR_3f, THDI_INTR_3_DISINTf},
    { 0x0004, THDI_INTR_2f, THDI_INTR_2_DISINTf},
    { 0x0002, THDI_INTR_1f, THDI_INTR_1_DISINTf},
    { 0x0001, THDI_INTR_0f, THDI_INTR_0_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                      _soc_katana_mmu_sb_ctr_parity_info [] = {
    { 0x0008, CTR_ENQ_WRAP_ERRORf,
        CTR_ENQ_WRAP_ERROR_DISINTf},
    { 0x0004, CTR_DEQ_WRAP_ERRORf,
        CTR_DEQ_WRAP_ERROR_DISINTf},
    { 0x0002, CTR_FLEX_CNT_CORRECTED_ERRORf,
        CTR_FLEX_CNT_CORRECTED_ERROR_DISINTf},
    { 0x0001, CTR_FLEX_CNT_UNCORRECTED_ERRORf,
        CTR_FLEX_CNT_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                    _soc_katana_mmu_sb_ccp_parity_info [] = {
    { 0x0800, CCPE_SOP_HOLDUP_OVERLOADf,
         CCPE_SOP_HOLDUP_OVERLOAD_DISINTf},
    { 0x0400, CCPI_SOP_HOLDUP_OVERLOADf,
         CCPI_SOP_HOLDUP_OVERLOAD_DISINTf},
    { 0x0200, CCPE_BUFFER_TRACK_OVERLOADf,
         CCPE_BUFFER_TRACK_OVERLOAD_DISINTf},
    { 0x0100, CCPI_BUFFER_TRACK_OVERLOADf,
         CCPI_BUFFER_TRACK_OVERLOAD_DISINTf},
    { 0x0080, CCPI_ILLEGAL_SRCPG_INTRf,
         CCPI_ILLEGAL_SRCPG_INTR_DISINTf},
    { 0x0040, CCPI_RQE_UPD_NEGATIVEf,
         CCPI_RQE_UPD_NEGATIVE_DISINTf},
    { 0x0020, CCPE_ILLEGAL_SRCPG_INTRf,
         CCPE_ILLEGAL_SRCPG_INTR_DISINTf},
    { 0x0010, CCPE_RQE_UPD_NEGATIVEf,
         CCPE_RQE_UPD_NEGATIVE_DISINTf},
    { 0x0008, CCPE_CORRECTED_ERRORf,
         CCPE_CORRECTED_ERROR_DISINTf},
    { 0x0004, CCPE_UNCORRECTED_ERRORf,
         CCPE_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, CCPI_CORRECTED_ERRORf,
         CCPI_CORRECTED_ERROR_DISINTf},
    { 0x0001, CCPI_UNCORRECTED_ERRORf,
         CCPI_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                        _soc_katana_mmu_sb_toq_parity_info [] = {
    { 0x10000, QEN_ALLOC_UNDERRUNf,
         QEN_ALLOC_UNDERRUN_DISINTf},
    { 0x8000, QEN_ALLOC_OVERFLOWf,
         QEN_ALLOC_OVERFLOW_DISINTf},
    { 0x4000, FLUSH_COMPLETEf,
         FLUSH_COMPLETE_DISINTf},
    { 0x2000, QDIS_FIFO_OVERFLOWf,
         QDIS_FIFO_OVERFLOW_DISINTf},
    { 0x1000, TRACE_DEQ_EVENTf,
         TRACE_DEQ_EVENT_DISINTf},
    { 0x0800, TRACE_ENQ_EVENTf,
         TRACE_ENQ_EVENT_DISINTf},
    { 0x0400, DEQ_TO_EMPTY_QUEUE_ERRORf,
         DEQ_TO_EMPTY_QUEUE_ERROR_DISINTf},
    { 0x0200, QSTRUCT_EMPTY_ERRORf,
         QSTRUCT_EMPTY_ERROR_DISINTf},
    { 0x0100, QUEUE_OVELOAD_ERRORf,
         QUEUE_OVELOAD_ERROR_DISINTf},
    { 0x0080, TDM_VIOLATION_ERRORf,
         TDM_VIOLATION_ERROR_DISINTf},
    { 0x0040, RQE_FIFO_OVERFLOWf,
         RQE_FIFO_OVERFLOW_DISINTf},
    { 0x0020, RQE_FIFO_CORRECTED_ERRORf,
         RQE_FIFO_CORRECTED_ERROR_DISINTf},
    { 0x0010, RQE_FIFO_UNCORRECTED_ERRORf,
         RQE_FIFO_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, IPMC_VLAN_TBL_CORRECTED_ERRORf,
         IPMC_VLAN_TBL_CORRECTED_ERROR_DISINTf},
    { 0x0004, IPMC_VLAN_TBL_UNCORRECTED_ERRORf,
         IPMC_VLAN_TBL_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, TOQ_STATE_CORRECTED_ERRORf,
         TOQ_STATE_CORRECTED_ERROR_DISINTf},
    { 0x0001, TOQ_STATE_UNCORRECTED_ERRORf,
         TOQ_STATE_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t
                         _soc_katana_mmu_sb_ite_parity_info [] = {
    { 0x0008, ITE_WORK_QUEUE_RD_EMPTYf,
        ITE_WORK_QUEUE_RD_EMPTY_DISINTf},
    { 0x0004, ITE_WORK_QUEUE_WR_FULLf,
        ITE_WORK_QUEUE_WR_FULL_DISINTf},
    { 0x0002, ITE_CTRL_RD_EMPTYf,
        ITE_CTRL_RD_EMPTY_DISINTf},
    { 0x0001, ITE_CTRL_WR_FULLf,
        ITE_CTRL_WR_FULL_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t
                         _soc_katana_mmu_sb_ite_cfg_parity_info [] = {
    { 0x0200, PACKET_PTR_STORE_CORRECTED_ERRORf,
        PACKET_PTR_STORE_FORCE_CORRECTED_ERROR_DISINTf},
    { 0x0100, PACKET_PTR_STORE_UNCORRECTED_ERRORf,
        PACKET_PTR_STORE_FORCE_UNCORRECTED_ERROR_DISINTf},
    { 0x0080, ITE_WORK_QUEUE_CORRECTED_ERRORf,
        ITE_WORK_QUEUE_FORCE_CORRECTED_ERROR_DISINTf},
    { 0x0040, ITE_WORK_QUEUE_UNCORRECTED_ERRORf,
        ITE_WORK_QUEUE_FORCE_UNCORRECTED_ERROR_DISINTf},
    { 0x0020, ITE_CTRL_CORRECTED_ERRORf,
        ITE_CTRL_CORRECTED_ERROR_DISINTf},
    { 0x0010, ITE_CTRL_UNCORRECTED_ERRORf,
        ITE_CTRL_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, ITE_QMGR_FLL_CORRECTED_ERRORf,
        ITE_QMGR_FLL_FORCE_CORRECTED_ERROR_DISINTf},
    { 0x0004, ITE_QMGR_FLL_UNCORRECTED_ERRORf,
        ITE_QMGR_FLL_FORCE_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, ITE_QMGR_QLL_CORRECTED_ERRORf,
        ITE_QMGR_QLL_CORRECTED_ERROR_DISINTf},
    { 0x0001, ITE_QMGR_QLL_UNCORRECTED_ERRORf,
        ITE_QMGR_QLL_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                           _soc_katana_mmu_sb_enq_cfg_parity_info [] = {
    { 0x0020, CBI_CORRECTED_ERRORf,
            CBI_CORRECTED_ERROR_DISINTf},
    { 0x0010, CBI_UNCORRECTED_ERRORf,
            CBI_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, RQE_WR_COMPLETE_CORRECTED_ERRORf,
            RQE_WR_COMPLETE_CORRECTED_ERROR_DISINTf},
    { 0x0004, RQE_WR_COMPLETE_UNCORRECTED_ERRORf,
            RQE_WR_COMPLETE_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, CBP_32B_WR_STORE_CORRECTED_ERRORf,
            CBP_32B_WR_STORE_CORRECTED_ERROR_DISINTf},
    { 0x0001, CBP_32B_WR_STORE_UNCORRECTED_ERRORf,
            CBP_32B_WR_STORE_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                            _soc_katana_mmu_sb_enq_parity_info [] = {
    { 0x0010, FAP_DUPLICATE_PTRf,
            FAP_DUPLICATE_PTR_DISINTf},
    { 0x0008, ENQ_TRACE_STATUSf,
            ENQ_TRACE_STATUS_DISINTf},
    { 0x0004, ILLEGAL_CELL_PBIf,
            ILLEGAL_CELL_PBI_DISINTf},
    { 0x0002, MISSING_START_ERRf,
            MISSING_START_ERR_DISINTf},
    { 0x0001, START_BY_START_ERRf,
            START_BY_START_ERR_DISINTf},
    { 0 } /* table terminator */
};


STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                     _soc_katana_mmu_sb_enq_fap_parity_info [] = {
    { 0x0008, FAP_STACK_CORRECTED_ERRORf,
            FAP_STACK_CORRECTED_ERROR_DISINTf},
    { 0x0004, FAP_STACK_UNCORRECTED_ERRORf,
            FAP_STACK_UNCORRECTED_ERROR_ERROR_DISINTf},
    { 0x0002, FAP_BITMAP_CORRECTED_ERRORf,
            FAP_BITMAP_CORRECTED_ERROR_DISINTf},
    { 0x0001, FAP_BITMAP_UNCORRECTED_ERRORf,
            FAP_BITMAP_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                     _soc_katana_mmu_sb_thdo_stat1_parity_info [] = {
    { 0x3ffff, ECC_1B_ERROR_STATUSf, ECC_1B_MASKf},
    { 0 } /* table terminator */
};
STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                     _soc_katana_mmu_sb_thdo_stat2_parity_info [] = {
    { 0x3ffff, ECC_2B_ERROR_STATUSf, ECC_2B_MASKf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                    _soc_katana_mmu_sb_cfapi_parity_info [] = {
    { 0x0010, DUPLICATE_PTRf,
        DUPLICATE_PTR_DISINTf},
    { 0x0008, BITMAP_CORRECTED_ERRORf,
        BITMAP_CORRECTED_ERROR_DISINTf},
    { 0x0004, BITMAP_UNCORRECTED_ERRORf,
        BITMAP_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, STACK_CORRECTED_ERRORf,
        STACK_CORRECTED_ERROR_DISINTf},
    { 0x0001, STACK_UNCORRECTED_ERRORf,
        STACK_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                    _soc_katana_mmu_sb_cfape_parity_info [] = {
    { 0x0010, DUPLICATE_PTRf,
        DUPLICATE_PTR_DISINTf},
    { 0x0008, BITMAP_CORRECTED_ERRORf,
        BITMAP_CORRECTED_ERROR_DISINTf},
    { 0x0004, BITMAP_UNCORRECTED_ERRORf,
        BITMAP_UNCORRECTED_ERROR_DISINTf},
    { 0x0002, STACK_CORRECTED_ERRORf,
        STACK_CORRECTED_ERROR_DISINTf},
    { 0x0001, STACK_UNCORRECTED_ERRORf,
        STACK_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                      _soc_katana_mmu_sb_age_int_parity_info [] = {
    { 0x0020, LMT_CORRECTED_ERRORf, LMT_CORRECTED_ERROR_DISINTf}, 
    { 0x0010, LMT_UNCORRECTED_ERRORf, LMT_UNCORRECTED_ERROR_DISINTf}, 
    { 0x0008, CTR_CORRECTED_ERRORf, CTR_CORRECTED_ERROR_DISINTf}, 
    { 0x0004, CTR_UNCORRECTED_ERRORf, CTR_UNCORRECTED_ERROR_DISINTf}, 
    { 0x0002, EXP_CORRECTED_ERRORf, EXP_CORRECTED_ERROR_DISINTf}, 
    { 0x0001, EXP_UNCORRECTED_ERRORf, EXP_UNCORRECTED_ERROR_DISINTf}, 
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                                _soc_katana_mmu_sb_age_ext_parity_info [] = {
    { 0x0020, LMT_CORRECTED_ERRORf, LMT_CORRECTED_ERROR_DISINTf}, 
    { 0x0010, LMT_UNCORRECTED_ERRORf, LMT_UNCORRECTED_ERROR_DISINTf}, 
    { 0x0008, CTR_CORRECTED_ERRORf, CTR_CORRECTED_ERROR_DISINTf}, 
    { 0x0004, CTR_UNCORRECTED_ERRORf, CTR_UNCORRECTED_ERROR_DISINTf}, 
    { 0x0002, EXP_CORRECTED_ERRORf, EXP_CORRECTED_ERROR_DISINTf}, 
    { 0x0001, EXP_UNCORRECTED_ERRORf, EXP_UNCORRECTED_ERROR_DISINTf}, 
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                                _soc_katana_mmu_sb_intfi_parity_info [] = {
    { 0x0004, HCFC_EARLY_SYNC_DETECTf,
         HCFC_EARLY_SYNC_DETECT_DISINTf},
    { 0x0002, HCFC_MSG_DROPPEDf,
         HCFC_MSG_DROPPED_DISINTf},
    { 0x0001, HCFC_CRC_DROP_VALIDf,
         HCFC_CRC_DROP_VALID_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_2_info_t 
                                _soc_katana_mmu_sb_intfi_ecc_parity_info [] = {
     /*  error status mask is actually at bit positon 4 */
    { 0x0002, 0x0010, FC_BASE_CORRECTED_ERRORf,
         FC_BASE_CORRECTED_ERROR_DISINTf},
     /*  error status mask is actually at bit positon 3 */
    { 0x0001, 0x0008, FC_BASE_UNCORRECTED_ERRORf,
         FC_BASE_UNCORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */

};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                _soc_katana_mmu_sb_ci2_parity_info [] = {
    { 0x0100, PHY_READY_EVENTf,
       PHY_READYf},
    { 0x0080, PHY_BL1_RD_FIFO_ERRORf,
       PHY_BL1_RD_FIFO_ERRORf},
    { 0x0040, PHY_BL0_RD_FIFO_ERRORf,
       PHY_BL0_RD_FIFO_ERRORf},
    { 0x0020, WFIFO_CTL_CORRECTED_ERRORf,
       WFIFO_CTL_CORRECTED_ERROR_DISINTf},
    { 0x0010, WFIFO_CTL_UNCORRECTED_ERRORf,
       WFIFO_CTL_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, WFIFO_OVERFLOWf,
       WB_OVERFLOW_DISINTf},
    { 0x0004, RFIFO_CTL_CORRECTED_ERRORf,
       RFIFO_CTL_CORRECTED_ERROR_DISINTf},
    { 0x0002, RFIFO_CTL_UNCORRECTED_ERRORf,
       RFIFO_CTL_UNCORRECTED_ERROR_DISINTf},
    { 0x0001, RFIFO_OVERFLOWf,
       RFIFO_OVERFLOW_DISINTf},
    { 0 } /* table terminator */

};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                _soc_katana_mmu_sb_ci1_parity_info [] = {
    { 0x0100, PHY_READY_EVENTf,
       PHY_READYf},
    { 0x0080, PHY_BL1_RD_FIFO_ERRORf,
       PHY_BL1_RD_FIFO_ERRORf},
    { 0x0040, PHY_BL0_RD_FIFO_ERRORf,
       PHY_BL0_RD_FIFO_ERRORf},
    { 0x0020, WFIFO_CTL_CORRECTED_ERRORf,
       WFIFO_CTL_CORRECTED_ERROR_DISINTf},
    { 0x0010, WFIFO_CTL_UNCORRECTED_ERRORf,
       WFIFO_CTL_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, WFIFO_OVERFLOWf,
       WB_OVERFLOW_DISINTf},
    { 0x0004, RFIFO_CTL_CORRECTED_ERRORf,
       RFIFO_CTL_CORRECTED_ERROR_DISINTf},
    { 0x0002, RFIFO_CTL_UNCORRECTED_ERRORf,
       RFIFO_CTL_UNCORRECTED_ERROR_DISINTf},
    { 0x0001, RFIFO_OVERFLOWf,
       RFIFO_OVERFLOW_DISINTf},
    { 0 } /* table terminator */

};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                 _soc_katana_mmu_sb_ci0_parity_info [] = {
    { 0x0100, PHY_READY_EVENTf,
       PHY_READYf},
    { 0x0080, PHY_BL1_RD_FIFO_ERRORf,
       PHY_BL1_RD_FIFO_ERRORf},
    { 0x0040, PHY_BL0_RD_FIFO_ERRORf,
       PHY_BL0_RD_FIFO_ERRORf},
    { 0x0020, WFIFO_CTL_CORRECTED_ERRORf,
       WFIFO_CTL_CORRECTED_ERROR_DISINTf},
    { 0x0010, WFIFO_CTL_UNCORRECTED_ERRORf,
       WFIFO_CTL_UNCORRECTED_ERROR_DISINTf},
    { 0x0008, WFIFO_OVERFLOWf,
       WB_OVERFLOW_DISINTf},
    { 0x0004, RFIFO_CTL_CORRECTED_ERRORf,
       RFIFO_CTL_CORRECTED_ERROR_DISINTf},
    { 0x0002, RFIFO_CTL_UNCORRECTED_ERRORf,
       RFIFO_CTL_UNCORRECTED_ERROR_DISINTf},
    { 0x0001, RFIFO_OVERFLOWf,
       RFIFO_OVERFLOW_DISINTf},
    { 0 } /* table terminator */
};


STATIC  
_soc_katana_mmu_sub_block_type_1_info_t 
                         _soc_katana_mmu_sb_emc0_parity_info [] = {
    { 0x0004, EMC_WTAC_ILLEGAL_SRCPG_ERRORf, 
         EMC_WTAC_ILLEGAL_SRCPG_ERROR_DISINTf },
    { 0x0002, EMC_IRRB_BUFFER_OVERFLOW_ERRORf, 
         EMC_IRRB_BUFFER_OVERFLOW_ERROR_DISINTf },
    { 0x0001, EMC_IWRB_BUFFER_OVERFLOW_ERRORf, 
         EMC_IWRB_BUFFER_OVERFLOW_ERROR_DISINTf },
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_1_info_t
                                 _soc_katana_mmu_sb_emc1_parity_info [] = {
    { 0x8000000, EMC_CSDB_2_UPPER_BUFFER_UNCORRECTED_ERRORf,
        EMC_CSDB_2_UPPER_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x4000000, EMC_CSDB_2_LOWER_BUFFER_UNCORRECTED_ERRORf,
        EMC_CSDB_2_LOWER_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x2000000, EMC_CSDB_1_UPPER_BUFFER_UNCORRECTED_ERRORf,
        EMC_CSDB_1_UPPER_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x1000000, EMC_CSDB_1_LOWER_BUFFER_UNCORRECTED_ERRORf,
        EMC_CSDB_1_LOWER_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x800000, EMC_CSDB_0_UPPER_BUFFER_UNCORRECTED_ERRORf,
        EMC_CSDB_0_UPPER_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x400000, EMC_CSDB_0_LOWER_BUFFER_UNCORRECTED_ERRORf,
        EMC_CSDB_0_LOWER_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x200000, EMC_ERRB_BUFFER_UNCORRECTED_ERRORf,    
        EMC_ERRB_BUFFER_UNCORRECTED_ERROR_DISINTf},    
    { 0x100000, EMC_ERRB_BUFFER_CORRECTED_ERRORf,      
        EMC_ERRB_BUFFER_CORRECTED_ERROR_DISINTf},      
    { 0x80000, EMC_RFCQ_BUFFER_UNCORRECTED_ERRORf,    
        EMC_RFCQ_BUFFER_UNCORRECTED_ERROR_DISINTf},    
    { 0x40000, EMC_RFCQ_BUFFER_CORRECTED_ERRORf,      
        EMC_RFCQ_BUFFER_CORRECTED_ERROR_DISINTf},      
    { 0x20000, EMC_RSFP_BUFFER_UNCORRECTED_ERRORf,    
        EMC_RSFP_BUFFER_UNCORRECTED_ERROR_DISINTf},    
    { 0x10000, EMC_RSFP_BUFFER_CORRECTED_ERRORf,      
        EMC_RSFP_BUFFER_CORRECTED_ERROR_DISINTf},      
    { 0x8000, EMC_IRRB_BUFFER_UNCORRECTED_ERRORf,    
        EMC_IRRB_BUFFER_UNCORRECTED_ERROR_DISINTf},    
    { 0x4000, EMC_IRRB_BUFFER_CORRECTED_ERRORf,      
        EMC_IRRB_BUFFER_CORRECTED_ERROR_DISINTf},      
    { 0x2000, EMC_EWRB_BUFFER_1_UNCORRECTED_ERRORf,    
        EMC_EWRB_BUFFER_1_UNCORRECTED_ERROR_DISINTf},    
    { 0x1000, EMC_EWRB_BUFFER_1_CORRECTED_ERRORf,      
        EMC_EWRB_BUFFER_1_CORRECTED_ERROR_DISINTf},      
    { 0x0800, EMC_EWRB_BUFFER_0_UNCORRECTED_ERRORf,    
        EMC_EWRB_BUFFER_0_UNCORRECTED_ERROR_DISINTf},    
    { 0x0400, EMC_EWRB_BUFFER_0_CORRECTED_ERRORf,      
        EMC_EWRB_BUFFER_0_CORRECTED_ERROR_DISINTf},      
    { 0x0200, EMC_WCMT_BUFFER_UNCORRECTED_ERRORf,     
        EMC_WCMT_BUFFER_UNCORRECTED_ERROR_DISINTf},     
    { 0x0100, EMC_WCMT_BUFFER_CORRECTED_ERRORf,       
        EMC_WCMT_BUFFER_CORRECTED_ERROR_DISINTf},       
    { 0x0080, EMC_SWAT_BUFFER_UNCORRECTED_ERRORf,     
        EMC_SWAT_BUFFER_UNCORRECTED_ERROR_DISINTf},     
    { 0x0040, EMC_SWAT_BUFFER_CORRECTED_ERRORf,       
        EMC_SWAT_BUFFER_CORRECTED_ERROR_DISINTf},       
    { 0x0020, EMC_WTOQ_BUFFER_UNCORRECTED_ERRORf,     
        EMC_WTOQ_BUFFER_UNCORRECTED_ERROR_DISINTf},     
    { 0x0010, EMC_WTOQ_BUFFER_CORRECTED_ERRORf,       
        EMC_WTOQ_BUFFER_CORRECTED_ERROR_DISINTf},       
    { 0x0008, EMC_WTFP_BUFFER_UNCORRECTED_ERRORf,     
        EMC_WTFP_BUFFER_UNCORRECTED_ERROR_DISINTf},     
    { 0x0004, EMC_WTFP_BUFFER_CORRECTED_ERRORf,       
        EMC_WTFP_BUFFER_CORRECTED_ERROR_DISINTf},       
    { 0x0002, EMC_IWRB_BUFFER_UNCORRECTED_ERRORf,     
        EMC_IWRB_BUFFER_UNCORRECTED_ERROR_DISINTf},     
    { 0x0001, EMC_IWRB_BUFFER_CORRECTED_ERRORf,       
        EMC_IWRB_BUFFER_CORRECTED_ERROR_DISINTf},       
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_1_info_t
                          _soc_katana_mmu_sb_emc2_parity_info [] = {
    { 0x0800000, EMC_WLCT2_UPPER_B_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT2_UPPER_B_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0400000, EMC_WLCT2_UPPER_B_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT2_UPPER_B_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0200000, EMC_WLCT2_UPPER_A_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT2_UPPER_A_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0100000, EMC_WLCT2_UPPER_A_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT2_UPPER_A_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0080000, EMC_WLCT2_LOWER_B_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT2_LOWER_B_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0040000, EMC_WLCT2_LOWER_B_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT2_LOWER_B_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0020000, EMC_WLCT2_LOWER_A_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT2_LOWER_A_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0010000, EMC_WLCT2_LOWER_A_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT2_LOWER_A_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0008000, EMC_WLCT1_UPPER_B_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT1_UPPER_B_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0004000, EMC_WLCT1_UPPER_B_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT1_UPPER_B_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0002000, EMC_WLCT1_UPPER_A_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT1_UPPER_A_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0001000, EMC_WLCT1_UPPER_A_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT1_UPPER_A_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0000800, EMC_WLCT1_LOWER_B_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT1_LOWER_B_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0000400, EMC_WLCT1_LOWER_B_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT1_LOWER_B_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0000200, EMC_WLCT1_LOWER_A_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT1_LOWER_A_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0000100, EMC_WLCT1_LOWER_A_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT1_LOWER_A_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0000080, EMC_WLCT0_UPPER_B_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT0_UPPER_B_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0000040, EMC_WLCT0_UPPER_B_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT0_UPPER_B_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0000020, EMC_WLCT0_UPPER_A_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT0_UPPER_A_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0000010, EMC_WLCT0_UPPER_A_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT0_UPPER_A_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0000008, EMC_WLCT0_LOWER_B_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT0_LOWER_B_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0000004, EMC_WLCT0_LOWER_B_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT0_LOWER_B_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0x0000002, EMC_WLCT0_LOWER_A_BUFFER_UNCORRECTED_ERRORf,
        EMC_WLCT0_LOWER_A_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x0000001, EMC_WLCT0_LOWER_A_BUFFER_CORRECTED_ERRORf,
        EMC_WLCT0_LOWER_A_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_3_info_t
               _soc_katana_mmu_sb_emc_parity_info [] = {
    { 0x0004, _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_2, EMC_ERRORr, 0,
      EMC_ERROR_2f, 0, _soc_katana_mmu_sb_emc2_parity_info },
    { 0x0002, _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_1, EMC_ERRORr, 0,
      EMC_ERROR_1f, 0, _soc_katana_mmu_sb_emc1_parity_info },
    { 0x0001, _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_0, EMC_ERRORr, 0,
      EMC_ERROR_0f, 0, _soc_katana_mmu_sb_emc0_parity_info },
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_1_info_t 
                                  _soc_katana_mmu_sb_deq0_parity_info [] = {
    { 0x0020, DEQ_TRACE_EVENTf, DEQ_TRACE_EVENT_DISINTf},
    { 0x0010, CCPE_BUFFER_OVERFLOW_ERRORf, CCPE_BUFFER_OVERFLOW_ERROR_DISINTf},
    { 0x0008, EGRESS_FIFO_OVERFLOW_ERRORf,
        EGRESS_FIFO_OVERFLOW_ERROR_DISINTf},
    { 0x0004, EGRESS_FIFO_UNDERRUN_ERRORf,
        EGRESS_FIFO_UNDERRUN_ERROR_DISINTf},
    { 0x0002, TOQ_TO_DEQ_CELL_REP_INFO_BUFFER_RD_REQ_ERRORf,
        DEQ_TO_CFG_TOQ_TO_DEQ_CELL_REP_INFO_BUFFER_RD_REQ_ERROR_DISINTf}, 
    { 0x0001, TOQ_TO_DEQ_CELL_REP_INFO_BUFFER_WR_REQ_ERRORf,
        DEQ_TO_CFG_TOQ_TO_DEQ_CELL_REP_INFO_BUFFER_WR_REQ_ERROR_DISINTf}, 
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_1_info_t 
                                  _soc_katana_mmu_sb_deq1_parity_info [] = {
    { 0x0008, PORT_35_EFIFO_UNDERRUN_ERRORf,         
        PORT_35_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x0004, PORT_34_EFIFO_UNDERRUN_ERRORf,         
        PORT_34_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x0002, PORT_33_EFIFO_UNDERRUN_ERRORf,         
        PORT_33_EFIFO_UNDERRUN_ERROR_DISINTf},     
    { 0x0001, PORT_32_EFIFO_UNDERRUN_ERRORf,
        PORT_32_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_1_info_t 
                                  _soc_katana_mmu_sb_deq2_parity_info [] = {
    { 0x80000000, PORT_31_EFIFO_UNDERRUN_ERRORf,
         PORT_31_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x40000000, PORT_30_EFIFO_UNDERRUN_ERRORf,
         PORT_30_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x20000000, PORT_29_EFIFO_UNDERRUN_ERRORf,
         PORT_29_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x10000000, PORT_28_EFIFO_UNDERRUN_ERRORf,
         PORT_28_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x08000000, PORT_27_EFIFO_UNDERRUN_ERRORf,
         PORT_27_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x04000000, PORT_26_EFIFO_UNDERRUN_ERRORf,
         PORT_26_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x02000000, PORT_25_EFIFO_UNDERRUN_ERRORf,
         PORT_25_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x01000000, PORT_24_EFIFO_UNDERRUN_ERRORf,
         PORT_24_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00800000, PORT_23_EFIFO_UNDERRUN_ERRORf,
         PORT_23_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00400000, PORT_22_EFIFO_UNDERRUN_ERRORf,
         PORT_22_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00200000, PORT_21_EFIFO_UNDERRUN_ERRORf,
         PORT_21_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00100000, PORT_20_EFIFO_UNDERRUN_ERRORf,
         PORT_20_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00080000, PORT_19_EFIFO_UNDERRUN_ERRORf,
         PORT_19_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00040000, PORT_18_EFIFO_UNDERRUN_ERRORf,
         PORT_18_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00020000, PORT_17_EFIFO_UNDERRUN_ERRORf,
         PORT_17_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00010000, PORT_16_EFIFO_UNDERRUN_ERRORf,
         PORT_16_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00008000, PORT_15_EFIFO_UNDERRUN_ERRORf,
         PORT_15_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00004000, PORT_14_EFIFO_UNDERRUN_ERRORf,
         PORT_14_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00002000, PORT_13_EFIFO_UNDERRUN_ERRORf,
         PORT_13_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00001000, PORT_12_EFIFO_UNDERRUN_ERRORf,
         PORT_12_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000800, PORT_11_EFIFO_UNDERRUN_ERRORf,
         PORT_11_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000400, PORT_10_EFIFO_UNDERRUN_ERRORf,
         PORT_10_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000200, PORT_09_EFIFO_UNDERRUN_ERRORf,
         PORT_09_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000100, PORT_08_EFIFO_UNDERRUN_ERRORf,
         PORT_08_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000080, PORT_07_EFIFO_UNDERRUN_ERRORf,
         PORT_07_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000040, PORT_06_EFIFO_UNDERRUN_ERRORf,
         PORT_06_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000020, PORT_05_EFIFO_UNDERRUN_ERRORf,
         PORT_05_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000010, PORT_04_EFIFO_UNDERRUN_ERRORf,
         PORT_04_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000008, PORT_03_EFIFO_UNDERRUN_ERRORf,
         PORT_03_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000004, PORT_02_EFIFO_UNDERRUN_ERRORf,
         PORT_02_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000002, PORT_01_EFIFO_UNDERRUN_ERRORf,
         PORT_01_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0x00000001, PORT_00_EFIFO_UNDERRUN_ERRORf,
         PORT_00_EFIFO_UNDERRUN_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_1_info_t 
                                  _soc_katana_mmu_sb_deq3_parity_info [] = {
    { 0x00020000, DEQ_EGRESS_FIFO_ECC_VERIFICATION_400_UNCORRECTED_ERRORf,
          DEQ_EGRESS_FIFO_ECC_VERIFICATION_400_UNCORRECTED_ERROR_DISINTf},
    { 0x00010000, DEQ_EGRESS_FIFO_ECC_VERIFICATION_400_CORRECTED_ERRORf,
          DEQ_EGRESS_FIFO_ECC_VERIFICATION_400_CORRECTED_ERROR_DISINTf},
    { 0x00008000, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_1520_UNCORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_1520_UNCORRECTED_ERROR_DISINTf},
    { 0x00004000, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_1520_CORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_1520_CORRECTED_ERROR_DISINTf},
    { 0x00002000, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_247_UNCORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_247_UNCORRECTED_ERROR_DISINTf},
    { 0x00001000, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_247_CORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_EXT_247_CORRECTED_ERROR_DISINTf},
    { 0x00000800, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_1520_UNCORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_1520_UNCORRECTED_ERROR_DISINTf},
    { 0x00000400, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_1520_CORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_1520_CORRECTED_ERROR_DISINTf},
    { 0x00000200, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_247_UNCORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_247_UNCORRECTED_ERROR_DISINTf},
    { 0x00000100, DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_247_CORRECTED_ERRORf,
          DEQ_PRE_EGRESS_FIFO_ECC_VERIF_INT_247_CORRECTED_ERROR_DISINTf},
    { 0x00000080, DEQ_EGRESS_FIFO_CONTROL_DATA_MEMORY_UNCORRECTED_ERRORf,
          DEQ_EGRESS_FIFO_CONTROL_DATA_MEMORY_UNCORRECTED_ERROR_DISINTf},
    { 0x00000040, DEQ_EGRESS_FIFO_CONTROL_DATA_MEMORY_CORRECTED_ERRORf,
          DEQ_EGRESS_FIFO_CONTROL_DATA_MEMORY_CORRECTED_ERROR_DISINTf},
    { 0x00000020, DEQ_EGRESS_FIFO_AGING_WRED_MEMORY_UNCORRECTED_ERRORf,
          DEQ_EGRESS_FIFO_AGING_WRED_MEMORY_UNCORRECTED_ERROR_DISINTf},
    { 0x00000010, DEQ_EGRESS_FIFO_AGING_WRED_MEMORY_CORRECTED_ERRORf,
          DEQ_EGRESS_FIFO_AGING_WRED_MEMORY_CORRECTED_ERROR_DISINTf},
    { 0x00000008, DEQ_AGING_MASK_MEMORY_UNCORRECTED_ERRORf,
          DEQ_AGING_MASK_MEMORY_UNCORRECTED_ERROR_DISINTf},
    { 0x00000004, DEQ_AGING_MASK_MEMORY_CORRECTED_ERRORf,
          DEQ_AGING_MASK_MEMORY_CORRECTED_ERROR_DISINTf},
    { 0x00000002, DEQ_TOQ_CELL_REP_INFO_BUFFER_UNCORRECTED_ERRORf,
  	  DEQ_TOQ_CELL_REP_INFO_BUFFER_UNCORRECTED_ERROR_DISINTf},
    { 0x00000001, DEQ_TOQ_CELL_REP_INFO_BUFFER_CORRECTED_ERRORf,
          DEQ_TOQ_CELL_REP_INFO_BUFFER_CORRECTED_ERROR_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_3_info_t
                           _soc_katana_mmu_sb_deq_parity_info [] = {
    { 0x0008, _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_3, DEQ_ERRORr, 0,
      DEQ_ERROR_2f, 0, _soc_katana_mmu_sb_deq3_parity_info },
    { 0x0004, _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_2, DEQ_ERRORr, 0,
      DEQ_ERROR_2f, 0, _soc_katana_mmu_sb_deq2_parity_info },
    { 0x0002, _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_1, DEQ_ERRORr, 0,
      DEQ_ERROR_1f, 0, _soc_katana_mmu_sb_deq1_parity_info },
    { 0x0001, _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_0, DEQ_ERRORr, 0,
      DEQ_ERROR_0f, 0, _soc_katana_mmu_sb_deq0_parity_info },
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t
                      _soc_katana_mmu_sb_qstruct_fap_parity_info [] = {
    { 0x100000, FAP3_DUPLICATE_PTR_ERRORf,
  	    FAP3_DUPLICATE_PTR_ERROR_DISINTf},
    { 0x80000, FAP2_DUPLICATE_PTR_ERRORf,
  	    FAP2_DUPLICATE_PTR_ERROR_DISINTf},
    { 0x40000, FAP1_DUPLICATE_PTR_ERRORf,
  	    FAP1_DUPLICATE_PTR_ERROR_DISINTf},
    { 0x20000, FAP0_DUPLICATE_PTR_ERRORf,
  	    FAP0_DUPLICATE_PTR_ERROR_DISINTf},
    { 0x10000, FAP_LOADING_ERRORf,
  	    FAP_LOADING_ERROR_DISINTf},
    { 0x8000, CORRECTED_BITMAP_ERROR_3f,
  	    CORRECTED_BITMAP_ERROR_3_DISINTf},
    { 0x4000, UNCORRECTED_BITMAP_ERROR_3f,
  	    UNCORRECTED_BITMAP_ERROR_3_DISINTf},
    { 0x2000, CORRECTED_STACK_ERROR_3f,
  	    CORRECTED_STACK_ERROR_3_DISINTf},
    { 0x1000, UNCORRECTED_STACK_ERROR_3f,
  	    UNCORRECTED_STACK_ERROR_3_DISINTf},
    { 0x0800, CORRECTED_BITMAP_ERROR_2f,
            CORRECTED_BITMAP_ERROR_2_DISINTf},
    { 0x0400, UNCORRECTED_BITMAP_ERROR_2f,
  	    UNCORRECTED_BITMAP_ERROR_2_DISINTf},
    { 0x0200, CORRECTED_STACK_ERROR_2f,
  	    CORRECTED_STACK_ERROR_2_DISINTf},
    { 0x0100, UNCORRECTED_STACK_ERROR_2f,
  	    UNCORRECTED_STACK_ERROR_2_DISINTf},
    { 0x0080, CORRECTED_BITMAP_ERROR_1f,
            CORRECTED_BITMAP_ERROR_1_DISINTf},
    { 0x0040, UNCORRECTED_BITMAP_ERROR_1f,
  	    UNCORRECTED_BITMAP_ERROR_1_DISINTf},
    { 0x0020, CORRECTED_STACK_ERROR_1f,
  	    CORRECTED_STACK_ERROR_1_DISINTf},
    { 0x0010, UNCORRECTED_STACK_ERROR_1f,
  	    UNCORRECTED_STACK_ERROR_1_DISINTf},
    { 0x0008, CORRECTED_BITMAP_ERROR_0f,
            CORRECTED_BITMAP_ERROR_0_DISINTf},
    { 0x0004, UNCORRECTED_BITMAP_ERROR_0f,
            UNCORRECTED_BITMAP_ERROR_0_DISINTf},
    { 0x0002, CORRECTED_STACK_ERROR_0f,
            CORRECTED_STACK_ERROR_0_DISINTf},
    { 0x0001, UNCORRECTED_STACK_ERROR_0f,
            UNCORRECTED_STACK_ERROR_0_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t
                      _soc_katana_mmu_sb_qstruct_qentry_upper_parity_info [] = {
    { 0x8000, CORRECTED_ERROR_7f,
        CORRECTED_ERROR_7_DISINTf},
    { 0x4000, UNCORRECTED_ERROR_7f,
        UNCORRECTED_ERROR_7_DISINTf},
    { 0x2000, CORRECTED_ERROR_6f,
        CORRECTED_ERROR_6_DISINTf},
    { 0x1000, UNCORRECTED_ERROR_6f,
        UNCORRECTED_ERROR_6_DISINTf},
    { 0x0800, CORRECTED_ERROR_5f,
        CORRECTED_ERROR_5_DISINTf},
    { 0x0400, UNCORRECTED_ERROR_5f,
        UNCORRECTED_ERROR_5_DISINTf},
    { 0x0200, CORRECTED_ERROR_4f,
        CORRECTED_ERROR_4_DISINTf},
    { 0x0100, UNCORRECTED_ERROR_4f,
        UNCORRECTED_ERROR_4_DISINTf},
    { 0x0080, CORRECTED_ERROR_3f,
        CORRECTED_ERROR_3_DISINTf},
    { 0x0040, UNCORRECTED_ERROR_3f,
        UNCORRECTED_ERROR_3_DISINTf},
    { 0x0020, CORRECTED_ERROR_2f,
        CORRECTED_ERROR_2_DISINTf},
    { 0x0010, UNCORRECTED_ERROR_2f,
        UNCORRECTED_ERROR_2_DISINTf},
    { 0x0008, CORRECTED_ERROR_1f,
        CORRECTED_ERROR_1_DISINTf},
    { 0x0004, UNCORRECTED_ERROR_1f,
        UNCORRECTED_ERROR_1_DISINTf},
    { 0x0002, CORRECTED_ERROR_0f,
        CORRECTED_ERROR_0_DISINTf},
    { 0x0001, UNCORRECTED_ERROR_0f,
        UNCORRECTED_ERROR_0_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t
                      _soc_katana_mmu_sb_qstruct_qentry_lower_parity_info [] = {
    { 0x8000, CORRECTED_ERROR_7f,
         CORRECTED_ERROR_7_DISINTf},
    { 0x4000, UNCORRECTED_ERROR_7f,
         UNCORRECTED_ERROR_7_DISINTf},
    { 0x2000, CORRECTED_ERROR_6f,
         CORRECTED_ERROR_6_DISINTf},
    { 0x1000, UNCORRECTED_ERROR_6f,
         UNCORRECTED_ERROR_6_DISINTf},
    { 0x0800, CORRECTED_ERROR_5f,
         CORRECTED_ERROR_5_DISINTf},
    { 0x0400, UNCORRECTED_ERROR_5f,
         UNCORRECTED_ERROR_5_DISINTf},
    { 0x0200, CORRECTED_ERROR_4f,
         CORRECTED_ERROR_4_DISINTf},
    { 0x0100, UNCORRECTED_ERROR_4f,
         UNCORRECTED_ERROR_4_DISINTf},
    { 0x0080, CORRECTED_ERROR_3f,
         CORRECTED_ERROR_3_DISINTf},
    { 0x0040, UNCORRECTED_ERROR_3f,
         UNCORRECTED_ERROR_3_DISINTf},
    { 0x0020, CORRECTED_ERROR_2f,
         CORRECTED_ERROR_2_DISINTf},
    { 0x0010, UNCORRECTED_ERROR_2f,
         UNCORRECTED_ERROR_2_DISINTf},
    { 0x0008, CORRECTED_ERROR_1f,
         CORRECTED_ERROR_1_DISINTf},
    { 0x0004, UNCORRECTED_ERROR_1f,
         UNCORRECTED_ERROR_1_DISINTf},
    { 0x0002, CORRECTED_ERROR_0f,
         CORRECTED_ERROR_0_DISINTf},
    { 0x0001, UNCORRECTED_ERROR_0f,
         UNCORRECTED_ERROR_0_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_1_info_t
                _soc_katana_mmu_sb_qstruct_qentry_qblock_nxt_parity_info [] = {
    { 0x8000, CORRECTED_ERROR_7f,
         CORRECTED_ERROR_7_DISINTf},
    { 0x4000, UNCORRECTED_ERROR_7f,
         UNCORRECTED_ERROR_7_DISINTf},
    { 0x2000, CORRECTED_ERROR_6f,
         CORRECTED_ERROR_6_DISINTf},
    { 0x1000, UNCORRECTED_ERROR_6f,
         UNCORRECTED_ERROR_6_DISINTf},
    { 0x0800, CORRECTED_ERROR_5f,
         CORRECTED_ERROR_5_DISINTf},
    { 0x0400, UNCORRECTED_ERROR_5f,
         UNCORRECTED_ERROR_5_DISINTf},
    { 0x0200, CORRECTED_ERROR_4f,
         CORRECTED_ERROR_4_DISINTf},
    { 0x0100, UNCORRECTED_ERROR_4f,
         UNCORRECTED_ERROR_4_DISINTf},
    { 0x0080, CORRECTED_ERROR_3f,
         CORRECTED_ERROR_3_DISINTf},
    { 0x0040, UNCORRECTED_ERROR_3f,
         UNCORRECTED_ERROR_3_DISINTf},
    { 0x0020, CORRECTED_ERROR_2f,
         CORRECTED_ERROR_2_DISINTf},
    { 0x0010, UNCORRECTED_ERROR_2f,
         UNCORRECTED_ERROR_2_DISINTf},
    { 0x0008, CORRECTED_ERROR_1f,
         CORRECTED_ERROR_1_DISINTf},
    { 0x0004, UNCORRECTED_ERROR_1f,
         UNCORRECTED_ERROR_1_DISINTf},
    { 0x0002, CORRECTED_ERROR_0f,
         CORRECTED_ERROR_0_DISINTf},
    { 0x0001, UNCORRECTED_ERROR_0f,
         UNCORRECTED_ERROR_0_DISINTf},
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_3_info_t
                           _soc_katana_mmu_sb_qstruct_parity_info [] = {
    { 0x0008, _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_FAP,
      QSTRUCT_INTERRUPTr, QSTRUCT_INTERRUPT_MASKr,
      QENTRY_FAP_ERRORf, QENTRY_FAP_ERROR_DISINTf,
      _soc_katana_mmu_sb_qstruct_fap_parity_info }, 
    { 0x0004, _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_U,
      QSTRUCT_INTERRUPTr, QSTRUCT_INTERRUPT_MASKr,
      QENTRY_UPPER_ERRORf, QENTRY_UPPER_ERROR_DISINTf,
      _soc_katana_mmu_sb_qstruct_qentry_upper_parity_info }, 
    { 0x0002, _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_L,
      QSTRUCT_INTERRUPTr, QSTRUCT_INTERRUPT_MASKr,
      QENTRY_LOWER_ERRORf, QENTRY_LOWER_ERROR_DISINTf,
      _soc_katana_mmu_sb_qstruct_qentry_lower_parity_info }, 
    { 0x0001, _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QBLOCK,
      QSTRUCT_INTERRUPTr, QSTRUCT_INTERRUPT_MASKr,
      QBLOCK_NEXT_ERRORf, QBLOCK_NEXT_ERROR_DISINTf,
      _soc_katana_mmu_sb_qstruct_qentry_qblock_nxt_parity_info }, 
    { 0 } /* table terminator */
};

STATIC  
_soc_katana_mmu_sub_block_type_1_info_t 
                             _soc_katana_mmu_sb_wred_parity_info [] = {
    { 0x4000000, UPDATE_INTRPT_STATUSf,
       UPDATE_INTRPT_MASKf },
    { 0x3ffe000, ECC_ERROR_1Bf,
       ECC_1B_ERROR_MASKf },
    { 0x1fff, ECC_ERROR_2Bf,
       ECC_2B_ERROR_MASKf },
    { 0 } /* table terminator */

};


STATIC  _soc_katana_mmu_sub_block_type_1_info_t 
                _soc_katana_mmu_sb_rqe_ser_parity_info [] = {
    { 0x4000000, NULL_REPL_PKTf, NULL_REPL_PKT_MASKf }, 
    { 0x2000000, FLL_EMPTYf, FLL_EMPTY_MASKf }, 
    { 0x1000000, THDIRQE_SRCPG_ILLEGALf , THDIRQE_SRCPG_ILLEGAL_MASKf }, 
    { 0x0800000, THDIQEN_SRCPG_ILLEGALf, THDIQEN_SRCPG_ILLEGAL_MASKf }, 
    { 0x0400000, EXTQ_LL_ERRORf, EXTQ_LL_ERROR_MASKf }, 
    { 0x03ff100, ECC_1B_BITMAPf, ECC_1B_MASKf }, 
    { 0x00007ff, ECC_2B_BITMAPf, ECC_2B_MASKf }, 
    { 0 } /* table terminator */
};

STATIC  _soc_katana_mmu_sub_block_type_2_info_t 
                _soc_katana_mmu_sb_rqe_extq_parity_info [] = {
    { 0x4000000, 0x8000, REPLICATION_SRCH_FAILf, REPLICATION_SRCH_FAIL_MASKf }, 
    { 0x2000000, 0x4000, REPLICATION_OVER_LIMITf, REPLICATION_OVER_LIMIT_MASKf},
    { 0x1ffe000, 0, REPLICATION_FAIL_MGIDf , 0 }, 
    { 0x0001fff, 0, REPLICATION_COUNTf, 0 }, 
    { 0 } /* table terminator */
};

STATIC const
_soc_katana_mmu_sub_block_type_1_t  _soc_katana_mmu_sub_blocks_type_1[] = {
    { 0x00100000, _SOC_KT_MMU_SUBBLOCK_MEM1,
        MEM1_INTR_DISINTf, MEM1_INTRf,
        _soc_katana_mmu_sb_mem1_parity_info },

    { 0x00040000, _SOC_KT_MMU_SUBBLOCK_E2EFC,
        E2EFC_INTR_DISINTf, E2EFC_INTRf,
        _soc_katana_mmu_sb_e2efc_parity_info},

    { 0x00010000, _SOC_KT_MMU_SUBBLOCK_THDI,
        THDI_INTR_DISINTf, THDI_INTRf,
        _soc_katana_mmu_sb_thdi_parity_info},

    { 0x00000800, _SOC_KT_MMU_SUBBLOCK_WRED,
        WRED_INTR_DISINTf, WRED_INTRf,
        _soc_katana_mmu_sb_wred_parity_info},

    { 0x00000040, _SOC_KT_MMU_SUBBLOCK_CTR,
        CTR_INTR_DISINTf, CTR_INTRf,
        _soc_katana_mmu_sb_ctr_parity_info},

    { 0x00000010, _SOC_KT_MMU_SUBBLOCK_CCP,
        CCP_INTR_DISINTf, CCP_INTRf,
        _soc_katana_mmu_sb_ccp_parity_info},

    { 0x00000001, _SOC_KT_MMU_SUBBLOCK_TOQ,
        TOQ_INTR_DISINTf, TOQ_INTRf,
        _soc_katana_mmu_sb_toq_parity_info},

    /* interrupt is OR of 2 or more registers */
    { 0x00020000, _SOC_KT_MMU_SUBBLOCK_ITE,
        ITE_INTR_DISINTf, ITE_INTRf,
        _soc_katana_mmu_sb_ite_parity_info},

    { 0x00020000, _SOC_KT_MMU_SUBBLOCK_ITE_CFG,
        ITE_INTR_DISINTf, ITE_INTRf,
        _soc_katana_mmu_sb_ite_cfg_parity_info},

    { 0x00008000, _SOC_KT_MMU_SUBBLOCK_ENQ_CFG,
        ENQ_INTR_DISINTf, ENQ_INTRf,
        _soc_katana_mmu_sb_enq_cfg_parity_info},

    { 0x00008000, _SOC_KT_MMU_SUBBLOCK_ENQ,
        ENQ_INTR_DISINTf, ENQ_INTRf,
        _soc_katana_mmu_sb_enq_parity_info},

    { 0x00008000, _SOC_KT_MMU_SUBBLOCK_ENQ_FAP,
        ENQ_INTR_DISINTf, ENQ_INTRf,
        _soc_katana_mmu_sb_enq_fap_parity_info},

    { 0x00000400, _SOC_KT_MMU_SUBBLOCK_THDO_STATUS1,
        THDO_INTR_DISINTf, THDO_INTRf,
        _soc_katana_mmu_sb_thdo_stat1_parity_info},

    { 0x00000400, _SOC_KT_MMU_SUBBLOCK_THDO_STATUS2,
        THDO_INTR_DISINTf, THDO_INTRf,
        _soc_katana_mmu_sb_thdo_stat2_parity_info},

    { 0x00000020, _SOC_KT_MMU_SUBBLOCK_CFAPI,
        CFAP_INTR_DISINTf, CFAP_INTRf,
        _soc_katana_mmu_sb_cfapi_parity_info},

    { 0x00000020, _SOC_KT_MMU_SUBBLOCK_CFAPE,
        CFAP_INTR_DISINTf, CFAP_INTRf,
        _soc_katana_mmu_sb_cfape_parity_info},

    { 0x00000008, _SOC_KT_MMU_SUBBLOCK_AGING_INT,
        AGING_INTR_DISINTf, AGING_INTRf,
        _soc_katana_mmu_sb_age_int_parity_info},

    { 0x00000008, _SOC_KT_MMU_SUBBLOCK_AGING_EXT,
        AGING_INTR_DISINTf, AGING_INTRf,
        _soc_katana_mmu_sb_age_ext_parity_info},

    { 0x00004000, _SOC_KT_MMU_SUBBLOCK_CI2,
        CI2_INTR_DISINTf, CI2_INTRf,
        _soc_katana_mmu_sb_ci2_parity_info},

    { 0x00002000, _SOC_KT_MMU_SUBBLOCK_CI1,
        CI1_INTR_DISINTf, CI1_INTRf,
        _soc_katana_mmu_sb_ci1_parity_info},

    { 0x00001000, _SOC_KT_MMU_SUBBLOCK_CI0,
        CI0_INTR_DISINTf, CI0_INTRf,
        _soc_katana_mmu_sb_ci0_parity_info},

    { 0x00000004, _SOC_KT_MMU_SUBBLOCK_LLS,
        LLS_INTR_DISINTf, LLS_INTRf,
        _soc_katana_mmu_sb_lls_parity_info},

    { 0x00000004, _SOC_KT_MMU_SUBBLOCK_LLS_PORT,
        LLS_INTR_DISINTf, LLS_INTRf,
        _soc_katana_mmu_sb_lls_port_parity_info},

    { 0x00000004, _SOC_KT_MMU_SUBBLOCK_LLS_UPD2,
        LLS_INTR_DISINTf, LLS_INTRf,
        _soc_katana_mmu_sb_lls_upd2_parity_info},

    { 0x00000004, _SOC_KT_MMU_SUBBLOCK_LLS_L0_ECC,
        LLS_INTR_DISINTf, LLS_INTRf,
        _soc_katana_mmu_sb_lls_l0_ecc_parity_info},

    { 0x00000004, _SOC_KT_MMU_SUBBLOCK_LLS_L1_ECC,
        LLS_INTR_DISINTf, LLS_INTRf,
        _soc_katana_mmu_sb_lls_l1_ecc_parity_info},

    { 0x00000004, _SOC_KT_MMU_SUBBLOCK_LLS_L2_ECC,
        LLS_INTR_DISINTf, LLS_INTRf,
        _soc_katana_mmu_sb_lls_l2_ecc_parity_info},

    { 0x00000004, _SOC_KT_MMU_SUBBLOCK_LLS_MISC_ECC,
        LLS_INTR_DISINTf, LLS_INTRf,
        _soc_katana_mmu_sb_lls_misc_ecc_parity_info},

    { 0x00000200, _SOC_KT_MMU_SUBBLOCK_RQE_SER,
        RQE_INTR_DISINTf, RQE_INTRf,
        _soc_katana_mmu_sb_rqe_ser_parity_info},

    { 0x00080000, _SOC_KT_MMU_SUBBLOCK_INTFI,
        INTF1_INTR_DISINTf, INTFI_INTRf,
        _soc_katana_mmu_sb_intfi_parity_info},
    { 0 } /* table terminator */
};

STATIC const
_soc_katana_mmu_sub_block_type_2_t  _soc_katana_mmu_sub_blocks_type_2[] = {
    { 0x00080000, _SOC_KT_MMU_SUBBLOCK_INTFI_ECC,
        INTF1_INTR_DISINTf, INTFI_INTRf,
        _soc_katana_mmu_sb_intfi_ecc_parity_info},
    { 0x00000200, _SOC_KT_MMU_SUBBLOCK_RQE_EXTQ,
        RQE_INTR_DISINTf, RQE_INTRf,
        _soc_katana_mmu_sb_rqe_extq_parity_info},
    { 0 } /* table terminator */
};

STATIC const
_soc_katana_mmu_sub_block_type_3_t  _soc_katana_mmu_sub_blocks_type_3[] = {
    { 0x00000100, _SOC_KT_MMU_SUBBLOCK_EMC,
        EMC_INTR_DISINTf, EMC_INTRf,
        _soc_katana_mmu_sb_emc_parity_info},

    { 0x00000080, _SOC_KT_MMU_SUBBLOCK_DEQ,
        DEQ_INTR_DISINTf, DEQ_INTRf,
        _soc_katana_mmu_sb_deq_parity_info},

    { 0x00000002, _SOC_KT_MMU_SUBBLOCK_QSTRUCT,
        QSTRUCT_INTR_DISINTf, QSTRUCT_INTRf,
        _soc_katana_mmu_sb_qstruct_parity_info},
    { 0 } /* table terminator */
};

STATIC _soc_katana_parity_info_t _soc_katana_ep0_parity_info[] = {
    /* EGR_EL3_ECC_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_NHOP_PAR_ERRf,
        EGR_NHOP_PAR_ERRf,
        EGR_L3_NEXT_HOPm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_L3_NEXT_HOP_PARITY_ENf,
        EGR_L3_NEXT_HOP_PARITY_STATUS_INTRr, NULL,
        EGR_L3_NEXT_HOP_PARITY_STATUS_NACKr, NULL },
    /* EGR_EL3_ECC_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_L3_INTF_PAR_ERRf,
        EGR_L3_INTF_PAR_ERRf,
        EGR_L3_INTFm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_L3_INTF_PARITY_ENf,
        EGR_L3_INTF_PARITY_STATUS_INTRr, NULL,
        EGR_L3_INTF_PARITY_STATUS_NACKr, NULL },
    /* EGR_EL3_ECC_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_MPLS_VC_AND_SWAP_LABEL_TABLE_PAR_ERRf,
        EGR_MPLS_VC_AND_SWAP_LABEL_TABLE_PAR_ERRf,
        EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_MPLS_VC_AND_SWAP_LABEL_TABLE_PARITY_ENf,
        EGR_MPLS_VC_AND_SWAP_LABEL_TABLE_PARITY_STATUS_INTRr, NULL,
        EGR_MPLS_VC_AND_SWAP_LABEL_TABLE_PARITY_STATUS_NACKr, NULL },
    /* EGR_EL3_ECC_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_MAC_DA_PROFILE_PAR_ERRf,
        EGR_MAC_DA_PROFILE_PAR_ERRf,
        EGR_MAC_DA_PROFILEm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_MAC_DA_PROFILE_PARITY_ENf,
        EGR_MAC_DA_PROFILE_PARITY_STATUS_INTRr, NULL,
        EGR_MAC_DA_PROFILE_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_DVP_ATTRIBUTE_PAR_ERRf,
        EGR_DVP_ATTRIBUTE_PAR_ERRf,
        EGR_DVP_ATTRIBUTEm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_DVP_ATTRIBUTE_PARITY_ENf,
        EGR_DVP_ATTRIBUTE_PARITY_STATUS_INTRr, NULL,
        EGR_DVP_ATTRIBUTE_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_VFI_PAR_ERRf,
        EGR_VFI_PAR_ERRf,
        EGR_VFIm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_VFI_PARITY_ENf,
        EGR_VFI_PARITY_STATUS_INTRr, NULL,
        EGR_VFI_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_IPMC_PAR_ERRf,
        EGR_IPMC_PAR_ERRf,
        EGR_IPMCm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_IPMC_PARITY_ENf,
        EGR_IPMC_PARITY_STATUS_INTRr, NULL,
        EGR_IPMC_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_PORT_PAR_ERRf,
        EGR_PORT_PAR_ERRf,
        EGR_PORTm, NULL,
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_PORT_PARITY_ENf,
        EGR_PORT_PARITY_STATUS_INTRr, NULL,
        EGR_PORT_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        EGR_MPB_ECC_ERRf,
        EGR_MPB_ECC_ERRf,
        INVALIDm, "EGR_MPB",
        EGR_EL3_ECC_PARITY_CONTROLr, EGR_MPB_ECC_ENf,
        EGR_MPB_ECC_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    /* EGR_VLAN_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_VLAN_PAR_ERRf,
        EGR_VLAN_PAR_ERRf,
        EGR_VLANm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_VLAN_PARITY_ENf,
        EGR_VLAN_PARITY_STATUS_INTRr, NULL,
        EGR_VLAN_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_VLAN_STG_PAR_ERRf,
        EGR_VLAN_STG_PAR_ERRf,
        EGR_VLAN_STGm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_VLAN_STG_PARITY_ENf,
        EGR_VLAN_STG_PARITY_STATUS_INTRr, NULL,
        EGR_VLAN_STG_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_HASH, NULL, 0,
        EGR_VLAN_XLATE_PAR_ERRf,
        EGR_VLAN_XLATE_PAR_ERRf,
        EGR_VLAN_XLATEm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_VLAN_XLATE_PARITY_ENf,
        INVALIDr, _soc_katana_egr_vlan_xlate_intr_reg,
        INVALIDr, _soc_katana_egr_vlan_xlate_nack_reg },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_IP_TUNNEL_PAR_ERRf,
        EGR_IP_TUNNEL_PAR_ERRf,
        EGR_IP_TUNNELm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_IP_TUNNEL_PARITY_ENf,
        EGR_IP_TUNNEL_PARITY_STATUS_INTRr, NULL,
        EGR_IP_TUNNEL_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_MPLS_EXP_MAPPING_2_PAR_ERRf,
        EGR_MPLS_EXP_MAPPING_2_PAR_ERRf,
        EGR_MPLS_EXP_MAPPING_2m, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_MPLS_EXP_MAPPING_2_PARITY_ENf,
        EGR_MPLS_EXP_MAPPING_2_PARITY_STATUS_INTRr, NULL,
        EGR_MPLS_EXP_MAPPING_2_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_MPLS_PRI_MAPPING_PAR_ERRf,
        EGR_MPLS_PRI_MAPPING_PAR_ERRf,
        EGR_MPLS_PRI_MAPPINGm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_MPLS_PRI_MAPPING_PARITY_ENf,
        EGR_MPLS_PRI_MAPPING_PARITY_STATUS_INTRr, NULL,
        EGR_MPLS_PRI_MAPPING_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_PRI_CNG_MAP_PAR_ERRf,
        EGR_PRI_CNG_MAP_PAR_ERRf,
        EGR_PRI_CNG_MAPm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_PRI_CNG_MAP_PARITY_ENf,
        EGR_PRI_CNG_MAP_PARITY_STATUS_INTRr, NULL,
        EGR_PRI_CNG_MAP_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_DSCP_TABLE_PAR_ERRf,
        EGR_DSCP_TABLE_PAR_ERRf,
        EGR_DSCP_TABLEm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_DSCP_TABLE_PARITY_ENf,
        EGR_DSCP_TABLE_PARITY_STATUS_INTRr, NULL,
        EGR_DSCP_TABLE_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_FRAGMENT_ID_TABLE_PAR_ERRf,
        EGR_FRAGMENT_ID_TABLE_PAR_ERRf,
        EGR_FRAGMENT_ID_TABLEm, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_FRAGMENT_ID_TABLE_PARITY_ENf,
        EGR_FRAGMENT_ID_TABLE_PARITY_STATUS_INTRr, NULL,
        EGR_FRAGMENT_ID_TABLE_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_MPLS_EXP_MAPPING_1_PAR_ERRf,
        EGR_MPLS_EXP_MAPPING_1_PAR_ERRf,
        EGR_MPLS_EXP_MAPPING_1m, NULL,
        EGR_VLAN_PARITY_CONTROLr, EGR_MPLS_EXP_MAPPING_1_PARITY_ENf,
        EGR_MPLS_EXP_MAPPING_1_PARITY_STATUS_INTRr, NULL,
        EGR_MPLS_EXP_MAPPING_1_PARITY_STATUS_NACKr, NULL },
    /* EGR_EHCPM_ECC_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        EGR_INITBUF_ECC_ERRf,
        EGR_INITBUF_ECC_ERRf,
        INVALIDm, "EGR_INITBUF",
        EGR_EHCPM_ECC_PARITY_CONTROLr, INITBUF_ECC_ENf,
        EGR_INITBUF_ECC_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_MOD_MAP_TABLE_PAR_ERRf,
        EGR_MOD_MAP_TABLE_PAR_ERRf,
        EGR_MOD_MAP_TABLEm, NULL,
        EGR_EHCPM_ECC_PARITY_CONTROLr, MOD_MAP_PARITY_ENf,
        EGR_MOD_MAP_PARITY_STATUS_INTRr, NULL,
        EGR_MOD_MAP_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC _soc_katana_parity_info_t _soc_katana_ep1_parity_info[] = {
    /* EGR_EDATABUF_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        CM_ECC_ERRf,
        CM_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_CM_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, CM_ECC_ENf,
        EGR_CM_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        GP0_ECC_ERRf,
        GP0_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_GP0_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, GP0_ECC_ENf,
        EGR_GP0_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        GP1_ECC_ERRf,
        GP1_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_GP1_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, GP1_ECC_ENf,
        EGR_GP1_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        GP2_ECC_ERRf,
        GP2_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_GP2_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, GP2_ECC_ENf,
        EGR_GP2_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        XQ0_ECC_ERRf,
        XQ0_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_XQ0_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, XQ0_ECC_ENf,
        EGR_XQ0_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        XQ1_ECC_ERRf,
        XQ1_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_XQ1_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, XQ1_ECC_ENf,
        EGR_XQ1_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        XQ2_ECC_ERRf,
        XQ2_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_XQ2_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, XQ2_ECC_ENf,
        EGR_XQ2_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        XQ3_ECC_ERRf,
        XQ3_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_XQ3_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, XQ3_ECC_ENf,
        EGR_XQ3_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
#ifdef _KATANA_DEBUG
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        RESI_ECC_ERRf,
        RESI_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_RESI_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, RESI_ECC_ENf,
        EGR_RESI_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
#endif
    { _SOC_PARITY_TYPE_EDATABUF, NULL, 0,
        LP_ECC_ERRf,
        LP_ECC_ERRf,
        INVALIDm, "EP_EDATABUF_LP_MEM",
        EGR_EDATABUF_PARITY_CONTROLr, LP_ECC_ENf,
        EGR_LP_BUFFER_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_COUNTER, NULL, 0,
        EGR_STATS_COUNTER_TABLE_PAR_ERRf,
        EGR_STATS_COUNTER_TABLE_PAR_ERRf,
        INVALIDm, "TX Debug Counter",
        EGR_EDATABUF_PARITY_CONTROLr, STATS_PAR_ENf,
        EGR_STATS_COUNTER_TABLE_PARITY_STATUS_INTRr, NULL,
        EGR_STATS_COUNTER_TABLE_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_EFP_COUNTER_TABLE_PAR_ERRf,
        EGR_EFP_COUNTER_TABLE_PAR_ERRf,
        EFP_COUNTER_TABLEm, NULL,
        EGR_EDATABUF_PARITY_CONTROLr, EFPCTR_PAR_ENf,
        EGR_EFP_COUNTER_TABLE_PARITY_STATUS_INTRr, NULL,
        EGR_EFP_COUNTER_TABLE_PARITY_STATUS_NACKr, NULL },
    /* EFP_METER_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_EFP_METER_TABLE_PAR_ERRf,
        EGR_EFP_METER_TABLE_PAR_ERRf,
        EFP_METER_TABLEm, NULL,
        EFP_METER_PARITY_CONTROLr, PARITY_ENf,
        EFP_METER_PARITY_STATUS_INTRr, NULL,
        EFP_METER_PARITY_STATUS_NACKr, NULL },
    /* EFP_POLICY_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_EFP_POLICY_TABLE_PAR_ERRf,
        EGR_EFP_POLICY_TABLE_PAR_ERRf,
        EFP_POLICY_TABLEm, NULL,
        EFP_POLICY_PARITY_CONTROLr, PARITY_ENf,
        EFP_POLICY_PARITY_STATUS_INTRr, NULL,
        EFP_POLICY_PARITY_STATUS_NACKr, NULL },
    /* EGR_PW_INIT_COUNTERS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_EFP_PW_INIT_COUNTERS_PAR_ERRf,
        EGR_EFP_PW_INIT_COUNTERS_PAR_ERRf,
        EGR_PW_INIT_COUNTERSm, NULL,
        EGR_PW_INIT_COUNTERS_PARITY_CONTROLr, PARITY_ENf,
        EGR_PW_INIT_COUNTERS_PARITY_STATUS_INTRr, NULL,
        EGR_PW_INIT_COUNTERS_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC _soc_katana_parity_info_t _soc_katana_ip0_parity_info[] = {
    /* Block-IPIPE, Stage-IARB */
    /* LMEP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        LMEP_PAR_ERRf,
        LMEP_PAR_ERRf,
        LMEPm, NULL,
        LMEP_PARITY_CONTROLr, PARITY_ENf,
        LMEP_PARITY_STATUS_INTRr, NULL,
        LMEP_PARITY_STATUS_NACKr, NULL },
    /* LMEP_1_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IARB_LMEP1_PAR_ERRf,
        IARB_LMEP1_PAR_ERRf,
        LMEP_1m, NULL,
        LMEP_1_PARITY_CONTROLr, PARITY_ENf,
        LMEP_1_PARITY_STATUS_INTRr, NULL,
        LMEP_1_PARITY_STATUS_NACKr, NULL },
    /* IARB_PKT_ECC_CONTROLr */
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        IARB_PKT_ERRf,
        IARB_PKT_ERRf,
        INVALIDm, "IARB_PKT_BUF",
        IARB_PKT_ECC_CONTROLr, ECC_ENf,
        IARB_PKT_ECC_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    /* IARB_HDR_ECC_CONTROLr */
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        IARB_HDR_ERRf,
        IARB_HDR_ERRf,
        INVALIDm, "IARB_PKT_HDR",
        IARB_HDR_ECC_CONTROLr, ECC_ENf,
        IARB_HDR_ECC_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        IARB_PIPE_X_LEARN_FIFO_ECC_ERRf,
        IARB_PIPE_X_LEARN_FIFO_ECC_ERRf,
        INVALIDm, "IARB_LEARN_FIFO_PIPE_X",
        IARB_LEARN_FIFO_ECC_CONTROLr, ECC_ENf,
        IARB_PIPE_X_LERAN_FIFO_ECC_STATUS_INTRr, NULL,
        INVALIDr, NULL },

    /* Block-IPIPE; Stage-IVXLT */

    /* VLAN_XLATE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_HASH, NULL, 0,
        VXLT_PAR_ERRf,
        VXLT_PAR_ERRf,
        VLAN_XLATEm, NULL,
        VLAN_XLATE_PARITY_CONTROLr, PARITY_ENf,
        INVALIDr, _soc_katana_vlan_xlate_intr_reg,
        INVALIDr, _soc_katana_vlan_xlate_nack_reg },
    /* VFP_POLICY_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VFP_POLICY_PAR_ERRf,
        VFP_POLICY_PAR_ERRf,
        VFP_POLICY_TABLEm, NULL,
        VFP_POLICY_PARITY_CONTROLr, PARITY_ENf,
        VFP_POLICY_PARITY_STATUS_INTRr, NULL,
        VFP_POLICY_PARITY_STATUS_NACKr, NULL },
    /* VLAN_PROT_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_PROT_PAR_ERRf,
        VLAN_PROT_PAR_ERRf,
        VLAN_PROTOCOL_DATAm, NULL,
        VLAN_PROT_PARITY_CONTROLr, PARITY_ENf,
        VLAN_PROT_PARITY_STATUS_INTRr, NULL,
        VLAN_PROT_PARITY_STATUS_NACKr, NULL },
    /* VLAN_SUBNET_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_SUBNET_PAR_ERRf,
        VLAN_SUBNET_PAR_ERRf,
        VLAN_SUBNETm, NULL,
        VLAN_SUBNET_PARITY_CONTROLr, PARITY_ENf,
        VLAN_SUBNET_PARITY_STATUS_INTRr, NULL,
        VLAN_SUBNET_PARITY_STATUS_NACKr, NULL },
    /* MPLS_ENTRY_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_HASH, NULL, 0,
        MPLS_ENTRY_PAR_ERRf,
        MPLS_ENTRY_PAR_ERRf,
        MPLS_ENTRYm, NULL,
        MPLS_ENTRY_PARITY_CONTROLr, PARITY_ENf,
        INVALIDr, _soc_katana_mpls_entry_intr_reg,
        INVALIDr, _soc_katana_mpls_entry_nack_reg },

    /* Block-IPIPE; Stage-IPARS */

    /* CPU_TS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        CPU_TS_POLICY_PAR_ERRf,
        CPU_TS_POLICY_PAR_ERRf,
        CPU_TS_MAPm, NULL,
        CPU_TS_PARITY_CONTROLr, PARITY_ENf,
        CPU_TS_PARITY_STATUS_INTRr, NULL,
        CPU_TS_PARITY_STATUS_NACKr, NULL },
    /* VLAN_RANGE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_RANGE_PAR_ERRf,
        VLAN_RANGE_PAR_ERRf,
        ING_VLAN_RANGEm, NULL,
        VLAN_RANGE_PARITY_CONTROLr, PARITY_ENf,
        VLAN_RANGE_PARITY_STATUS_INTRr, NULL,
        VLAN_RANGE_PARITY_STATUS_NACKr, NULL },
    /* MOD_MAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MOD_MAP_PAR_ERRf,
        MOD_MAP_PAR_ERRf,
        ING_MOD_MAP_TABLEm, NULL,
        MOD_MAP_PARITY_CONTROLr, PARITY_ENf,
        MOD_MAP_PARITY_STATUS_INTRr, NULL,
        MOD_MAP_PARITY_STATUS_NACKr, NULL },
    /* FP_UDF_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        FP_UDF_PAR_ERRf,
        FP_UDF_PAR_ERRf,
        FP_UDF_OFFSETm, NULL,
        FP_UDF_PARITY_CONTROLr, PARITY_ENf,
        FP_UDF_PARITY_STATUS_INTRr, NULL,
        FP_UDF_PARITY_STATUS_NACKr, NULL },
    /* L3_TUNNEL_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L3_TUNNEL_PAR_ERRf,
        L3_TUNNEL_PAR_ERRf,
        L3_TUNNELm, NULL,
        L3_TUNNEL_PARITY_CONTROLr, PARITY_ENf,
        L3_TUNNEL_PARITY_STATUS_INTRr, NULL,
        L3_TUNNEL_PARITY_STATUS_NACKr, NULL },
    /* SRC_TRUNK_ECC_CONTROLr  */
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        SOURCE_TRUNK_MAP_PARITY_ERRf,
        SOURCE_TRUNK_MAP_PARITY_ERRf,
        SOURCE_TRUNK_MAP_TABLEm, NULL,
        SRC_TRUNK_ECC_CONTROLr, ECC_ENf,
        SRC_TRUNK_ECC_STATUS_INTRr, NULL,
        SRC_TRUNK_ECC_STATUS_NACKr, NULL },
    /*     LPORT_ECC_CONTROLr */
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        LPORT_PAR_ERRf,
        LPORT_PAR_ERRf,
        LPORT_TABm, NULL,
        LPORT_ECC_CONTROLr, ECC_ENf,
        LPORT_ECC_STATUS_INTRr, NULL,
        LPORT_ECC_STATUS_NACKr, NULL },

    /* Block-IPIPE; Stage-ICFG */

    /* PORT_TABLE_ECC_CONTROLr */
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PORT_TABLE_PAR_ERRf,
        PORT_TABLE_PAR_ERRf,
        PORT_TABm, NULL,
        PORT_TABLE_ECC_CONTROLr, ECC_ENf,
        PORT_TABLE_ECC_STATUS_INTRr, NULL,
        PORT_TABLE_ECC_STATUS_NACKr, NULL },
    /* SYSTEM_CONFIG_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SYS_CONFIG_PAR_ERRf,
        SYS_CONFIG_PAR_ERRf,
        SYSTEM_CONFIG_TABLEm, NULL,
        SYSTEM_CONFIG_PARITY_CONTROLr, PARITY_ENf,
        SYSTEM_CONFIG_PARITY_STATUS_INTRr, NULL,
        SYSTEM_CONFIG_PARITY_STATUS_NACKr, NULL },
    /* SYSTEM_CONFIG_MODVIEW_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SYSTEM_CONFIG_MODVIEW_PAR_ERRf,
        SYSTEM_CONFIG_MODVIEW_PAR_ERRf,
        SYSTEM_CONFIG_TABLE_MODBASEm, NULL,
        SYSTEM_CONFIG_MODVIEW_PARITY_CONTROLr, PARITY_ENf,
        SYSTEM_CONFIG_MODVIEW_PARITY_STATUS_INTRr, NULL,
        SYSTEM_CONFIG_MODVIEW_PARITY_STATUS_NACKr, NULL },
    /* SOURCE_TRUNK_MAP_MODVIEW_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SOURCE_TRUNK_MAP_MODVIEW_PAR_ERRf,
        SOURCE_TRUNK_MAP_MODVIEW_PAR_ERRf,
        SOURCE_TRUNK_MAP_MODBASEm, NULL,
        SOURCE_TRUNK_MAP_MODVIEW_PARITY_CONTROLr, PARITY_ENf,
        SOURCE_TRUNK_MAP_MODVIEW_PARITY_STATUS_INTRr, NULL,
        SOURCE_TRUNK_MAP_MODVIEW_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC _soc_katana_parity_info_t _soc_katana_ip1_parity_info[] = {
    /* Block-IPIPE; Stage- IPMLS */
    /* SOURCE_VP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SOURCE_VP_PAR_ERRf,
        SOURCE_VP_PAR_ERRf,
        SOURCE_VPm, NULL,
        SOURCE_VP_PARITY_CONTROLr, PARITY_ENf,
        SOURCE_VP_PARITY_STATUS_INTRr, NULL,
        SOURCE_VP_PARITY_STATUS_NACKr, NULL },
    /* VFI_1_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VFI_1_PAR_ERRf,
        VFI_1_PAR_ERRf,
        VFI_1m, NULL,
        VFI_1_PARITY_CONTROLr, PARITY_ENf,
        VFI_1_PARITY_STATUS_INTRr, NULL,
        VFI_1_PARITY_STATUS_NACKr, NULL },
    /* L3_IIF_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L3_IIF_PAR_ERRf,
        L3_IIF_PAR_ERRf,
        L3_IIFm, NULL,
        L3_IIF_PARITY_CONTROLr, PARITY_ENf,
        L3_IIF_PARITY_STATUS_INTRr, NULL,
        L3_IIF_PARITY_STATUS_NACKr, NULL },
    /* VFI_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VFI_PAR_ERRf,
        VFI_PAR_ERRf,
        VFIm, NULL,
        VFI_PARITY_CONTROLr, PARITY_ENf,
        VFI_PARITY_STATUS_INTRr, NULL,
        VFI_PARITY_STATUS_NACKr, NULL },
    /* VRF_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VRF_PAR_ERRf,
        VRF_PAR_ERRf,
        VRFm, NULL,
        VRF_PARITY_CONTROLr, PARITY_ENf,
        VRF_PARITY_STATUS_INTRr, NULL,
        VRF_PARITY_STATUS_NACKr, NULL },
    /* VLAN_MPLS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_MPLS_PAR_ERRf,
        VLAN_MPLS_PAR_ERRf,
        VLAN_MPLSm, NULL,
        VLAN_MPLS_PARITY_CONTROLr, PARITY_ENf,
        VLAN_MPLS_PARITY_STATUS_INTRr, NULL,
        VLAN_MPLS_PARITY_STATUS_NACKr, NULL },
    /* VLAN_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_PAR_ERRf,
        VLAN_PAR_ERRf,
        VLAN_TABm, NULL,
        VLAN_PARITY_CONTROLr, PARITY_ENf,
        VLAN_PARITY_STATUS_INTRr, NULL,
        VLAN_PARITY_STATUS_NACKr, NULL },
    /* VLAN_STG_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_STG_PAR_ERRf,
        VLAN_STG_PAR_ERRf,
        STG_TABm, NULL,
        VLAN_STG_PARITY_CONTROLr, PARITY_ENf,
        VLAN_STG_PARITY_STATUS_INTRr, NULL,
        VLAN_STG_PARITY_STATUS_NACKr, NULL },
    
    /* Block-IPIPE; Stage-IL3LU */
    /* L3_ENTRY_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_HASH, NULL, 0,
        L3_ENTRY_PAR_ERRf,
        L3_ENTRY_PAR_ERRf,
        L3_ENTRY_IPV6_UNICASTm, NULL,
        L3_ENTRY_PARITY_CONTROLr, PARITY_ENf,
        INVALIDr, _soc_katana_l3_entry_ipv6_unicast_intr_reg,
        INVALIDr, _soc_katana_l3_entry_ipv6_unicast_nack_reg },

    
    /* Block-IPIPE; Stage-ILPM */
    /* L3_DEFIP_DATA_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L3_DEFIP_PAR_ERRf,
        L3_DEFIP_PAR_ERRf,
        L3_DEFIPm, NULL,
        L3_DEFIP_DATA_PARITY_CONTROLr, PARITY_ENf,
        L3_DEFIP_DATA_PARITY_STATUS_INTRr, NULL,
        L3_DEFIP_DATA_PARITY_STATUS_NACKr, NULL },
    /* L3_DEFIP_128_DATA_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L3_DEFIP_128_PAR_ERRf,
        L3_DEFIP_128_PAR_ERRf,
        L3_DEFIP_128m, NULL,
        L3_DEFIP_128_DATA_PARITY_CONTROLr, PARITY_ENf,
        L3_DEFIP_128_DATA_PARITY_STATUS_INTRr, NULL,
        L3_DEFIP_128_DATA_PARITY_STATUS_NACKr, NULL },

    /* Block-IPIPE; Stage-ISW1 */

    /* FP_FIELD_SEL_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        FP_FIELD_SEL_PAR_ERRf,
        FP_FIELD_SEL_PAR_ERRf,
        FP_PORT_FIELD_SELm, NULL,
        FP_FIELD_SEL_PARITY_CONTROLr, PARITY_ENf,
        FP_FIELD_SEL_PARITY_STATUS_INTRr, NULL,
        FP_FIELD_SEL_PARITY_STATUS_NACKr, NULL },
    /* TTL_FN_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        TTL_FN_PAR_ERRf,
        TTL_FN_PAR_ERRf,
        TTL_FNm, NULL,
        TTL_FN_PARITY_CONTROLr, PARITY_ENf,
        TTL_FN_PARITY_STATUS_INTRr, NULL,
        TTL_FN_PARITY_STATUS_NACKr, NULL },
    /* TOS_FN_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        TOS_FN_PAR_ERRf,
        TOS_FN_PAR_ERRf,
        TOS_FNm, NULL,
        TOS_FN_PARITY_CONTROLr, PARITY_ENf,
        TOS_FN_PARITY_STATUS_INTRr, NULL,
        TOS_FN_PARITY_STATUS_NACKr, NULL },
    /* ING_PRI_CNG_MAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_PRI_CNG_MAP_PAR_ERRf,
        ING_PRI_CNG_MAP_PAR_ERRf,
        ING_PRI_CNG_MAPm, NULL,
        ING_PRI_CNG_MAP_PARITY_CONTROLr, PARITY_ENf,
        ING_PRI_CNG_MAP_PARITY_STATUS_INTRr, NULL,
        ING_PRI_CNG_MAP_PARITY_STATUS_NACKr, NULL },
    /* ING_UNTAGGED_PHB_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_UNTAGGED_PHB_PAR_ERRf,
        ING_UNTAGGED_PHB_PAR_ERRf,
        ING_UNTAGGED_PHBm, NULL,
        ING_UNTAGGED_PHB_PARITY_CONTROLr, PARITY_ENf,
        ING_UNTAGGED_PHB_PARITY_STATUS_INTRr, NULL,
        ING_UNTAGGED_PHB_PARITY_STATUS_NACKr, NULL },
    /* DSCP_TABLE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        DSCP_TABLE_PAR_ERRf,
        DSCP_TABLE_PAR_ERRf,
        DSCP_TABLEm, NULL,
        DSCP_TABLE_PARITY_CONTROLr, PARITY_ENf,
        DSCP_TABLE_PARITY_STATUS_INTRr, NULL,
        DSCP_TABLE_PARITY_STATUS_NACKr, NULL },

    /* Block-IPIPE; Stage-IL2LU */
    /* L2_ENTRY_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_HASH, NULL, 0,
        L2_ENTRY_PAR_ERRf,
        L2_ENTRY_PAR_ERRf,
        L2Xm, NULL,
        L2_ENTRY_PARITY_CONTROLr, PARITY_ENf,
        INVALIDr, _soc_katana_l2_entry_only_intr_reg,
        INVALIDr, _soc_katana_l2_entry_only_nack_reg },

    /* Block-IPIPE; Stage-ISREL1 */
    /* INITIAL_ING_L3_NEXT_HOP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        NHOP_PAR_ERRf,
        NHOP_PAR_ERRf,
        INITIAL_ING_L3_NEXT_HOPm, NULL,
        INITIAL_ING_L3_NEXT_HOP_PARITY_CONTROLr, PARITY_ENf,
        INITIAL_ING_L3_NEXT_HOP_PARITY_STATUS_INTRr, NULL,
        INITIAL_ING_L3_NEXT_HOP_PARITY_STATUS_NACKr, NULL },
    /* INITIAL_L3_ECMP_GROUP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ECMP_GRP_PAR_ERRf,
        ECMP_GRP_PAR_ERRf,
        INITIAL_L3_ECMP_GROUPm, NULL,
        INITIAL_L3_ECMP_GROUP_PARITY_CONTROLr, PARITY_ENf,
        INITIAL_L3_ECMP_GROUP_PARITY_STATUS_INTRr, NULL,
        INITIAL_L3_ECMP_GROUP_PARITY_STATUS_NACKr, NULL },
    /* INITIAL_L3_ECMP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L3_ECMP_PAR_ERRf,
        L3_ECMP_PAR_ERRf,
        INITIAL_L3_ECMPm, NULL,
        INITIAL_L3_ECMP_PARITY_CONTROLr, PARITY_ENf,
        INITIAL_L3_ECMP_PARITY_STATUS_INTRr, NULL,
        INITIAL_L3_ECMP_PARITY_STATUS_NACKr, NULL },
    /* ING_DVP_TABLE_PARITY_STATUS_INTRr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_DVP_PAR_ERRf,
        ING_DVP_PAR_ERRf,
        ING_DVP_TABLEm, NULL,
        ING_DVP_TABLE_PARITY_CONTROLr, PARITY_ENf,
        ING_DVP_TABLE_PARITY_STATUS_INTRr, NULL,
        ING_DVP_TABLE_PARITY_STATUS_NACKr, NULL },
    /* INITIAL_PROT_NHI_TABLE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        PROT_NHI_PAR_ERRf,
        PROT_NHI_PAR_ERRf,
        INITIAL_PROT_NHI_TABLEm, NULL,
        INITIAL_PROT_NHI_TABLE_PARITY_CONTROLr, PARITY_ENf,
        INITIAL_PROT_NHI_TABLE_PARITY_STATUS_INTRr, NULL,
        INITIAL_PROT_NHI_TABLE_PARITY_STATUS_NACKr, NULL },
    /* PORT_CBL_TABLE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        PORT_CBL_PAR_ERRf,
        PORT_CBL_PAR_ERRf,
        PORT_CBL_TABLEm, NULL,
        PORT_CBL_TABLE_PARITY_CONTROLr, PARITY_ENf,
        PORT_CBL_TABLE_PARITY_STATUS_INTRr, NULL,
        PORT_CBL_TABLE_PARITY_STATUS_NACKr, NULL },
    /* PORT_CBL_TABLE_MODBASE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        PORT_CBL_MODBASE_PAR_ERRf,
        PORT_CBL_MODBASE_PAR_ERRf,
        PORT_CBL_TABLE_MODBASEm, NULL,
        PORT_CBL_TABLE_MODBASE_PARITY_CONTROLr, PARITY_ENf,
        PORT_CBL_TABLE_MODBASE_PARITY_STATUS_INTRr, NULL,
        PORT_CBL_TABLE_MODBASE_PARITY_STATUS_NACKr, NULL },
    /* L3_IPMC_1_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L3_IPMC_1_PAR_ERRf,
        L3_IPMC_1_PAR_ERRf,
        L3_IPMC_1m, NULL,
        L3_IPMC_1_PARITY_CONTROLr, PARITY_ENf,
        L3_IPMC_1_PARITY_STATUS_INTRr, NULL,
        L3_IPMC_1_PARITY_STATUS_NACKr, NULL },
    /* MA_INDEX_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MA_INDEX_PAR_ERRf,
        MA_INDEX_PAR_ERRf,
        MA_INDEXm, NULL,
        MA_INDEX_PARITY_CONTROLr, PARITY_ENf,
        MA_INDEX_PARITY_STATUS_INTRr, NULL,
        MA_INDEX_PARITY_STATUS_NACKr, NULL },
    /* RMEP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        RMEP_PAR_ERRf,
        RMEP_PAR_ERRf,
        RMEPm, NULL,
        RMEP_PARITY_CONTROLr, PARITY_ENf,
        RMEP_PARITY_STATUS_INTRr, NULL,
        RMEP_PARITY_STATUS_NACKr, NULL },
    /* MAID_REDUCTION_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MAID_REDUCTION_PAR_ERRf,
        MAID_REDUCTION_PAR_ERRf,
        MAID_REDUCTIONm, NULL,
        MAID_REDUCTION_PARITY_CONTROLr, PARITY_ENf,
        MAID_REDUCTION_PARITY_STATUS_INTRr, NULL,
        MAID_REDUCTION_PARITY_STATUS_NACKr, NULL },
    /* MA_STATE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MA_STATE_PAR_ERRf,
        MA_STATE_PAR_ERRf,
        MA_STATEm, NULL,
        MA_STATE_PARITY_CONTROLr, PARITY_ENf,
        MA_STATE_PARITY_STATUS_INTRr, NULL,
        MA_STATE_PARITY_STATUS_NACKr, NULL },


    /* Block-IPIPE; Stage-IL2LU */
    /* VLAN_OR_VFI_MAC_COUNT_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_HASH, NULL, 0,
        VLAN_OR_VFI_MAC_COUNT_PAR_ERRf,
        VLAN_OR_VFI_MAC_COUNT_PAR_ERRf,
        VLAN_OR_VFI_MAC_COUNTm, NULL,
        VLAN_OR_VFI_MAC_COUNT_PARITY_CONTROLr, PARITY_ENf,
        VLAN_OR_VFI_MAC_COUNT_PARITY_STATUS_INTRr, NULL,
        VLAN_OR_VFI_MAC_COUNT_PARITY_STATUS_INTRr, NULL },
    /* VLAN_OR_VFI_MAC_LIMIT_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_HASH, NULL, 0,
        VLAN_OR_VFI_MAC_LIMIT_PAR_ERRf,
        VLAN_OR_VFI_MAC_LIMIT_PAR_ERRf,
        VLAN_OR_VFI_MAC_LIMITm, NULL,
        VLAN_OR_VFI_MAC_LIMIT_PARITY_CONTROLr, PARITY_ENf,
        VLAN_OR_VFI_MAC_LIMIT_PARITY_STATUS_INTRr, NULL,
        VLAN_OR_VFI_MAC_LIMIT_PARITY_STATUS_INTRr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC _soc_katana_parity_info_t _soc_katana_ip1_1_parity_info[] = {
    /* Block-IPIPE; Stage-IL2LU */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L2_MOD_FIFO_INTRf,
        L2_MOD_FIFO_INTRf,
        L2_MOD_FIFOm, NULL,
        L2_MOD_FIFO_PARITY_CONTROLr, PARITY_ENf,
        L2_MOD_FIFO_PARITY_STATUS_INTRr, NULL,
        L2_MOD_FIFO_PARITY_STATUS_NACKr, NULL },
    /* Block-IPIPE; Stage-IMPLS */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MY_STATION_TCAM_DATA_ONLY_PAR_ERRf,
        MY_STATION_TCAM_DATA_ONLY_PAR_ERRf,
        MY_STATION_TCAM_DATA_ONLYm, NULL,
        MY_STATION_DATA_PARITY_CONTROLr, PARITY_ENf,
        MY_STATION_DATA_PARITY_STATUS_INTRr, NULL,
        MY_STATION_DATA_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};


STATIC _soc_katana_parity_info_t _soc_katana_ip2_parity_info[] = {
    /* Block-IPIPE; Stage-IDLB */
    /* DLB_HGT_FLOWSET_PORT_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        DLB_HGT_FLOWSET_PORT_PAR_ERRf,
        DLB_HGT_FLOWSET_PORT_PAR_ERRf,
        DLB_HGT_FLOWSET_PORTm, NULL,
        DLB_HGT_FLOWSET_PORT_PARITY_CONTROLr, PARITY_ENf,
        DLB_HGT_FLOWSET_PORT_PARITY_STATUS_INTRr, NULL,
        DLB_HGT_FLOWSET_PORT_PARITY_STATUS_NACKr, NULL },
    /* DLB_HGT_FLOWSET_TIMESTAMP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        DLB_HGT_FLOWSET_TIMESTAMP_PAR_ERRf,
        DLB_HGT_FLOWSET_TIMESTAMP_PAR_ERRf,
        DLB_HGT_FLOWSET_TIMESTAMPm, NULL,
        DLB_HGT_FLOWSET_TIMESTAMP_PARITY_CONTROLr, PARITY_ENf,
        DLB_HGT_FLOWSET_TIMESTAMP_PARITY_STATUS_INTRr, NULL,
        DLB_HGT_FLOWSET_TIMESTAMP_PARITY_STATUS_NACKr, NULL },
    /* DLB_HGT_FLOWSET_TIMESTAMP_PAGE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        DLB_HGT_FLOWSET_TIMESTAMP_PAGE_PAR_ERRf,
        DLB_HGT_FLOWSET_TIMESTAMP_PAGE_PAR_ERRf,
        DLB_HGT_FLOWSET_TIMESTAMP_PAGEm, NULL,
        DLB_HGT_FLOWSET_TIMESTAMP_PAGE_PARITY_CONTROLr, PARITY_ENf,
        DLB_HGT_FLOWSET_TIMESTAMP_PAGE_PARITY_STATUS_INTRr, NULL,
        DLB_HGT_FLOWSET_TIMESTAMP_PAGE_PARITY_STATUS_NACKr, NULL },


    /* Block-IPIPE; Stage-ISREL2 */
    /* ING_L3_NEXT_HOP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_ING_L3_NEXT_HOP_PAR_ERRf,
        IRSEL2_ING_L3_NEXT_HOP_PAR_ERRf,
        ING_L3_NEXT_HOPm, NULL,
        ING_L3_NEXT_HOP_PARITY_CONTROLr, PARITY_ENf,
        ING_L3_NEXT_HOP_PARITY_STATUS_INTRr, NULL,
        ING_L3_NEXT_HOP_PARITY_STATUS_NACKr, NULL },
    /* L3_IPMC_REMAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_L3_IPMC_REMAP_PAR_ERRf,
        IRSEL2_L3_IPMC_REMAP_PAR_ERRf,
        L3_IPMC_REMAPm, NULL,
        L3_IPMC_REMAP_PARITY_CONTROLr, PARITY_ENf,
        L3_IPMC_REMAP_PARITY_STATUS_INTRr, NULL,
        L3_IPMC_REMAP_PARITY_STATUS_NACKr, NULL },
    /* L3_IPMC_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_L3_IPMC_PAR_ERRf,
        IRSEL2_L3_IPMC_PAR_ERRf,
        L3_IPMCm, NULL,
        L3_IPMC_PARITY_CONTROLr, PARITY_ENf,
        L3_IPMC_PARITY_STATUS_INTRr, NULL,
        L3_IPMC_PARITY_STATUS_NACKr, NULL },
    /* L2MC_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_L2MC_PAR_ERRf,
        IRSEL2_L2MC_PAR_ERRf,
        L2MCm, NULL,
        L2MC_PARITY_CONTROLr, PARITY_ENf,
        L2MC_PARITY_STATUS_INTRr, NULL,
        L2MC_PARITY_STATUS_NACKr, NULL },
    /* IFP_REDIRECTION_PROFILE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_IFP_REDIRECTION_PROFILE_PAR_ERRf,
        IRSEL2_IFP_REDIRECTION_PROFILE_PAR_ERRf,
        IFP_REDIRECTION_PROFILEm, NULL,
        IFP_REDIRECTION_PROFILE_PARITY_CONTROLr, PARITY_ENf,
        IFP_REDIRECTION_PROFILE_PARITY_STATUS_INTRr, NULL,
        IFP_REDIRECTION_PROFILE_PARITY_STATUS_NACKr, NULL },
    /* L3_ECMP_GROUP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_L3_ECMP_GROUP_PAR_ERRf,
        IRSEL2_L3_ECMP_GROUP_PAR_ERRf,
        L3_ECMP_COUNTm, NULL,
        L3_ECMP_GROUP_PARITY_CONTROLr, PARITY_ENf,
        L3_ECMP_GROUP_PARITY_STATUS_INTRr, NULL,
        L3_ECMP_GROUP_PARITY_STATUS_NACKr, NULL },
    /* L3_ECMP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_L3_ECMP_PAR_ERRf,
        IRSEL2_L3_ECMP_PAR_ERRf,
        L3_ECMPm, NULL,
        L3_ECMP_PARITY_CONTROLr, PARITY_ENf,
        L3_ECMP_PARITY_STATUS_INTRr, NULL,
        L3_ECMP_PARITY_STATUS_NACKr, NULL },
    /* EGR_MASK_MODBASE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IRSEL2_EGR_MASK_MODBASE_PAR_ERRf,
        IRSEL2_EGR_MASK_MODBASE_PAR_ERRf,
        EGR_MASK_MODBASEm, NULL,
        EGR_MASK_MODBASE_PARITY_CONTROLr, PARITY_ENf,
        EGR_MASK_MODBASE_PARITY_STATUS_INTRr, NULL,
        EGR_MASK_MODBASE_PARITY_STATUS_NACKr, NULL },
    
    /* Block-IPIPE; Stage-IFP */

    /* IFP_COUNTER_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IFP_COUNTER_PAR_ERRf,
        IFP_COUNTER_PAR_ERRf,
        FP_COUNTER_TABLEm, NULL,
        IFP_COUNTER_PARITY_CONTROLr, PARITY_ENf,
        IFP_COUNTER_PARITY_STATUS_INTRr, NULL,
        IFP_COUNTER_PARITY_STATUS_NACKr, NULL },
    /* IFP_METER_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IFP_METER_PAR_ERRf,
        IFP_METER_PAR_ERRf,
        FP_METER_TABLEm, NULL,
        IFP_METER_PARITY_CONTROLr, PARITY_ENf,
        IFP_METER_PARITY_STATUS_INTRr, NULL,
        IFP_METER_PARITY_STATUS_NACKr, NULL },
    /* IFP_POLICY_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IFP_POLICY_PAR_ERRf,
        IFP_POLICY_PAR_ERRf,
        FP_POLICY_TABLEm, NULL,
        IFP_POLICY_PARITY_CONTROLr, PARITY_ENf,
        IFP_POLICY_PARITY_STATUS_INTRr, NULL,
        IFP_POLICY_PARITY_STATUS_NACKr, NULL },
    /* IFP_STORM_CONTROL_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IFP_STORM_CONTROL_PAR_ERRf,
        IFP_STORM_CONTROL_PAR_ERRf,
        FP_STORM_CONTROL_METERSm, NULL,
        IFP_STORM_CONTROL_PARITY_CONTROLr, PARITY_ENf,
        IFP_STORM_CONTROL_PARITY_STATUS_INTRr, NULL,
        IFP_STORM_CONTROL_PARITY_STATUS_NACKr, NULL },

    /* Block-ING_FLEX_CTR_BLOCKS;  Stage-FLEX_CTR */

    /* ING_FLEX_CTR_OFFSET_TABLE_CONTROL_Xr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_0_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_0_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_0m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_0r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_0r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_0r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_1_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_1_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_1m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_1r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_1r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_1r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_2_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_2_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_2m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_2r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_2r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_2r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_3_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_3_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_3m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_3r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_3r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_3r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_4_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_4_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_4m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_4r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_4r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_4r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_5_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_5_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_5m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_5r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_5r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_5r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_6_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_6_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_6m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_6r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_6r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_6r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_7_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_7_OFFSET_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_OFFSET_TABLE_7m, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_CONTROL_7r, PARITY_ENf,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_INTR_7r, NULL,
        ING_FLEX_CTR_OFFSET_TABLE_PARITY_STATUS_NACK_7r, NULL },
    /* ING_FLEX_CTR_COUNTER_TABLE_CONTROL_Xr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_0_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_0_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_0m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_0r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_0r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_0r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_1_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_1_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_1m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_1r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_1r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_1r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_2_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_2_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_2m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_2r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_2r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_2r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_3_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_3_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_3m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_3r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_3r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_3r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_4_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_4_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_4m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_4r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_4r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_4r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_5_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_5_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_5m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_5r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_5r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_5r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_6_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_6_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_6m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_6r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_6r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_6r, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_FLEX_CTR_POOL_7_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_POOL_7_COUNTER_TABLE_INTR_STATUSf,
        ING_FLEX_CTR_COUNTER_TABLE_7m, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_CONTROL_7r, PARITY_ENf,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_INTR_7r, NULL,
        ING_FLEX_CTR_COUNTER_TABLE_PARITY_STATUS_NACK_7r, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC _soc_katana_parity_info_t _soc_katana_ip2_1_parity_info[] = {
    /* Block-IPIPE; Stage-ISW2 */
    /* EGR_MASK_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EGR_MASK_PAR_ERRf,
        EGR_MASK_PAR_ERRf,
        EGR_MASKm, NULL,
        EGR_MASK_PARITY_CONTROLr, PARITY_ENf,
        EGR_MASK_PARITY_STATUS_INTRr, NULL,
        EGR_MASK_PARITY_STATUS_NACKr, NULL },
    /* TRUNK_BITMAP_TABLE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        TRUNK_BITMAP_PAR_ERRf,
        TRUNK_BITMAP_PAR_ERRf,
        TRUNK_BITMAPm, NULL,
        TRUNK_BITMAP_TABLE_PARITY_CONTROLr, PARITY_ENf,
        TRUNK_BITMAP_TABLE_PARITY_STATUS_INTRr, NULL,
        TRUNK_BITMAP_TABLE_PARITY_STATUS_NACKr, NULL },
    /* NONUCAST_TRUNK_BLOCK_MASK_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        NONUCAST_TRUNK_BLOCK_MASK_PAR_ERRf,
        NONUCAST_TRUNK_BLOCK_MASK_PAR_ERRf,
        NONUCAST_TRUNK_BLOCK_MASKm, NULL,
        NONUCAST_TRUNK_BLOCK_MASK_PARITY_CONTROLr, PARITY_ENf,
        NONUCAST_TRUNK_BLOCK_MASK_PARITY_STATUS_INTRr, NULL,
        NONUCAST_TRUNK_BLOCK_MASK_PARITY_STATUS_NACKr, NULL },
    /* MAC_BLOCK_TABLE_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MAC_BLOCK_TABLE_PAR_ERRf,
        MAC_BLOCK_TABLE_PAR_ERRf,
        MAC_BLOCKm, NULL,
        MAC_BLOCK_TABLE_PARITY_CONTROLr, PARITY_ENf,
        MAC_BLOCK_TABLE_PARITY_STATUS_INTRr, NULL,
        MAC_BLOCK_TABLE_PARITY_STATUS_NACKr, NULL },
    /* MODPORT_MAP_SW_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MODPORT_MAP_SW_PAR_ERRf,
        MODPORT_MAP_SW_PAR_ERRf,
        MODPORT_MAP_SWm, NULL,
        MODPORT_MAP_SW_PARITY_CONTROLr, PARITY_ENf,
        MODPORT_MAP_SW_PARITY_STATUS_INTRr, NULL,
        MODPORT_MAP_SW_PARITY_STATUS_NACKr, NULL },
    /* MODPORT_MAP_MIRROR_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MODPORT_MAP_M0_PAR_ERRf,
        MODPORT_MAP_M0_PAR_ERRf,
        MODPORT_MAP_M0m, NULL,
        MODPORT_MAP_MIRROR_PARITY_CONTROLr, PARITY_ENf,
        MODPORT_MAP_M0_PARITY_STATUS_INTRr, NULL,
        MODPORT_MAP_M0_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MODPORT_MAP_M1_PAR_ERRf,
        MODPORT_MAP_M1_PAR_ERRf,
        MODPORT_MAP_M1m, NULL,
        MODPORT_MAP_MIRROR_PARITY_CONTROLr, PARITY_ENf,
        MODPORT_MAP_M1_PARITY_STATUS_INTRr, NULL,
        MODPORT_MAP_M1_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MODPORT_MAP_M2_PAR_ERRf,
        MODPORT_MAP_M2_PAR_ERRf,
        MODPORT_MAP_M2m, NULL,
        MODPORT_MAP_MIRROR_PARITY_CONTROLr, PARITY_ENf,
        MODPORT_MAP_M2_PARITY_STATUS_INTRr, NULL,
        MODPORT_MAP_M2_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        MODPORT_MAP_M3_PAR_ERRf,
        MODPORT_MAP_M3_PAR_ERRf,
        MODPORT_MAP_M3m, NULL,
        MODPORT_MAP_MIRROR_PARITY_CONTROLr, PARITY_ENf,
        MODPORT_MAP_M3_PARITY_STATUS_INTRr, NULL,
        MODPORT_MAP_M3_PARITY_STATUS_NACKr, NULL },
    /* SRC_MODID_INGRESS_BLOCK_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SRC_MODID_INGRESS_BLOCK_PAR_ERRf,
        SRC_MODID_INGRESS_BLOCK_PAR_ERRf,
        SRC_MODID_INGRESS_BLOCKm, NULL,
        SRC_MODID_INGRESS_BLOCK_PARITY_CONTROLr, PARITY_ENf,
        SRC_MODID_INGRESS_BLOCK_PARITY_STATUS_INTRr, NULL,
        SRC_MODID_INGRESS_BLOCK_PARITY_STATUS_NACKr, NULL },
    /* SRC_MODID_EGRESS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SRC_MODID_EGRESS_PAR_ERRf,
        SRC_MODID_EGRESS_PAR_ERRf,
        SRC_MODID_EGRESSm, NULL,
        SRC_MODID_EGRESS_PARITY_CONTROLr, PARITY_ENf,
        SRC_MODID_EGRESS_PARITY_STATUS_INTRr, NULL,
        SRC_MODID_EGRESS_PARITY_STATUS_NACKr, NULL },
    /* ALTERNATE_EMIRROR_BITMAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ALTERNATE_EMIRROR_BITMAP_PAR_ERRf,
        ALTERNATE_EMIRROR_BITMAP_PAR_ERRf,
        ALTERNATE_EMIRROR_BITMAPm, NULL,
        ALTERNATE_EMIRROR_BITMAP_PARITY_CONTROLr, PARITY_ENf,
        ALTERNATE_EMIRROR_BITMAP_PARITY_STATUS_INTRr, NULL,
        ALTERNATE_EMIRROR_BITMAP_PARITY_STATUS_NACKr, NULL },
    /* PORT_LAG_FAILOVER_SET_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        PORT_LAG_FAILOVER_SET_PAR_ERRf,
        PORT_LAG_FAILOVER_SET_PAR_ERRf,
        PORT_LAG_FAILOVER_SETm, NULL,
        PORT_LAG_FAILOVER_SET_PARITY_CONTROLr, PARITY_ENf,
        PORT_LAG_FAILOVER_SET_PARITY_STATUS_INTRr, NULL,
        PORT_LAG_FAILOVER_SET_PARITY_STATUS_NACKr, NULL },
    /* VLAN_PROFILE_2_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_PROFILE_2_PAR_ERRf,
        VLAN_PROFILE_2_PAR_ERRf,
        VLAN_PROFILE_2m, NULL,
        VLAN_PROFILE_2_PARITY_CONTROLr, PARITY_ENf,
        VLAN_PROFILE_2_PARITY_STATUS_INTRr, NULL,
        VLAN_PROFILE_2_PARITY_STATUS_NACKr, NULL },
    /* L3_MTU_VALUES_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        L3_MTU_VALUES_PAR_ERRf,
        L3_MTU_VALUES_PAR_ERRf,
        L3_MTU_VALUESm, NULL,
        L3_MTU_VALUES_PARITY_CONTROLr, PARITY_ENf,
        L3_MTU_VALUES_PARITY_STATUS_INTRr, NULL,
        L3_MTU_VALUES_PARITY_STATUS_NACKr, NULL },
    /* ING_PW_TERM_SEQ_NUM_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_PW_TERM_SEQ_NUM_PAR_ERRf,
        ING_PW_TERM_SEQ_NUM_PAR_ERRf,
        ING_PW_TERM_SEQ_NUMm, NULL,
        ING_PW_TERM_SEQ_NUM_PARITY_CONTROLr, PARITY_ENf,
        ING_PW_TERM_SEQ_NUM_PARITY_STATUS_INTRr, NULL,
        ING_PW_TERM_SEQ_NUM_PARITY_STATUS_NACKr, NULL },
    /* VLAN_COS_MAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        VLAN_COS_MAP_PAR_ERRf,
        VLAN_COS_MAP_PAR_ERRf,
        VLAN_COS_MAPm, NULL,
        VLAN_COS_MAP_PARITY_CONTROLr, PARITY_ENf,
        VLAN_COS_MAP_PARITY_STATUS_INTRr, NULL,
        VLAN_COS_MAP_PARITY_STATUS_NACKr, NULL },
    /* E2E_HOL_STATUS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        E2E_HOL_STATUS_PAR_ERRf,
        E2E_HOL_STATUS_PAR_ERRf,
        E2E_HOL_STATUSm, NULL,
        E2E_HOL_STATUS_PARITY_CONTROLr, PARITY_ENf,
        E2E_HOL_STATUS_PARITY_STATUS_INTRr, NULL,
        E2E_HOL_STATUS_PARITY_STATUS_NACKr, NULL },
    /* E2E_HOL_STATUS_1_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        E2E_HOL_STATUS_1_PAR_ERRf,
        E2E_HOL_STATUS_1_PAR_ERRf,
        E2E_HOL_STATUS_1m, NULL,
        E2E_HOL_STATUS_1_PARITY_CONTROLr, PARITY_ENf,
        E2E_HOL_STATUS_1_PARITY_STATUS_INTRr, NULL,
        E2E_HOL_STATUS_1_PARITY_STATUS_NACKr, NULL },
    /* UNKNOWN_UCAST_BLOCK_MASK_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        UNKNOWN_UCAST_BLOCK_MASK_PAR_ERRf,
        UNKNOWN_UCAST_BLOCK_MASK_PAR_ERRf,
        UNKNOWN_UCAST_BLOCK_MASKm, NULL,
        UNKNOWN_UCAST_BLOCK_MASK_PARITY_CONTROLr, PARITY_ENf,
        UNKNOWN_UCAST_BLOCK_MASK_PARITY_STATUS_INTRr, NULL,
        UNKNOWN_UCAST_BLOCK_MASK_PARITY_STATUS_NACKr, NULL },
    /* UNKNOWN_MCAST_BLOCK_MASK_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        UNKNOWN_MCAST_BLOCK_MASK_PAR_ERRf,
        UNKNOWN_MCAST_BLOCK_MASK_PAR_ERRf,
        UNKNOWN_MCAST_BLOCK_MASKm, NULL,
        UNKNOWN_MCAST_BLOCK_MASK_PARITY_CONTROLr, PARITY_ENf,
        UNKNOWN_MCAST_BLOCK_MASK_PARITY_STATUS_INTRr, NULL,
        UNKNOWN_MCAST_BLOCK_MASK_PARITY_STATUS_NACKr, NULL },
    /* BCAST_BLOCK_MASK_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        BCAST_BLOCK_MASK_PAR_ERRf,
        BCAST_BLOCK_MASK_PAR_ERRf,
        BCAST_BLOCK_MASKm, NULL,
        BCAST_BLOCK_MASK_PARITY_CONTROLr, PARITY_ENf,
        BCAST_BLOCK_MASK_PARITY_STATUS_INTRr, NULL,
        BCAST_BLOCK_MASK_PARITY_STATUS_NACKr, NULL },
    /* KNOWN_MCAST_BLOCK_MASKm */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        KNOWN_MCAST_BLOCK_MASK_PAR_ERRf,
        KNOWN_MCAST_BLOCK_MASK_PAR_ERRf,
        KNOWN_MCAST_BLOCK_MASKm, NULL,
        KNOWN_MCAST_BLOCK_MASK_PARITY_CONTROLr, PARITY_ENf,
        KNOWN_MCAST_BLOCK_MASK_PARITY_STATUS_INTRr, NULL,
        KNOWN_MCAST_BLOCK_MASK_PARITY_STATUS_NACKr, NULL },
    /* ING_EGRMSKBMAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_EGRMSKBMAP_PAR_ERRf,
        ING_EGRMSKBMAP_PAR_ERRf,
        ING_EGRMSKBMAPm, NULL,
        ING_EGRMSKBMAP_PARITY_CONTROLr, PARITY_ENf,
        ING_EGRMSKBMAP_PARITY_STATUS_INTRr, NULL,
        ING_EGRMSKBMAP_PARITY_STATUS_NACKr, NULL },
    /* LOCAL_SW_DISABLE_DEFAULT_PBM_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        LOCAL_SW_DISABLE_DEFAULT_PBM_PAR_ERRf,
        LOCAL_SW_DISABLE_DEFAULT_PBM_PAR_ERRf,
        LOCAL_SW_DISABLE_DEFAULT_PBMm, NULL,
        LOCAL_SW_DISABLE_DEFAULT_PBM_PARITY_CONTROLr, PARITY_ENf,
        LOCAL_SW_DISABLE_DEFAULT_PBM_PARITY_STATUS_INTRr, NULL,
        LOCAL_SW_DISABLE_DEFAULT_PBM_PARITY_STATUS_NACKr, NULL },
    /* LOCAL_SW_DISABLE_DEFAULT_PBM_MIRR_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        LOCAL_SW_DISABLE_DEFAULT_PBM_MIRR_PAR_ERRf,
        LOCAL_SW_DISABLE_DEFAULT_PBM_MIRR_PAR_ERRf,
        LOCAL_SW_DISABLE_DEFAULT_PBM_MIRRm, NULL,
        LOCAL_SW_DISABLE_DEFAULT_PBM_MIRR_PARITY_CONTROLr, PARITY_ENf,
        LOCAL_SW_DISABLE_DEFAULT_PBM_MIRR_PARITY_STATUS_INTRr, NULL,
        LOCAL_SW_DISABLE_DEFAULT_PBM_MIRR_PARITY_STATUS_NACKr, NULL },
    /* IMIRROR_BITMAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        IMIRROR_BITMAP_PAR_ERRf,
        IMIRROR_BITMAP_PAR_ERRf,
        IMIRROR_BITMAPm, NULL,
        IMIRROR_BITMAP_PARITY_CONTROLr, PARITY_ENf,
        IMIRROR_BITMAP_PARITY_STATUS_INTRr, NULL,
        IMIRROR_BITMAP_PARITY_STATUS_NACKr, NULL },
    /* UNKNOWN_HGI_BITMAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        UNKNOWN_HGI_BITMAP_PAR_ERRf,
        UNKNOWN_HGI_BITMAP_PAR_ERRf,
        UNKNOWN_HGI_BITMAPm, NULL,
        UNKNOWN_HGI_BITMAP_PARITY_CONTROLr, PARITY_ENf,
        UNKNOWN_HGI_BITMAP_PARITY_STATUS_INTRr, NULL,
        UNKNOWN_HGI_BITMAP_PARITY_STATUS_NACKr, NULL },
    /* TRUNK_MEMBER_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        TRUNK_MEMBER_PAR_ERRf,
        TRUNK_MEMBER_PAR_ERRf,
        TRUNK_MEMBERm, NULL,
        TRUNK_MEMBER_PARITY_CONTROLr, PARITY_ENf,
        TRUNK_MEMBER_PARITY_STATUS_INTRr, NULL,
        TRUNK_MEMBER_PARITY_STATUS_NACKr, NULL },
    /* PHB2_COS_MAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        PHB2_COS_MAP_PAR_ERRf,
        PHB2_COS_MAP_PAR_ERRf,
        PHB2_COS_MAPm, NULL,
        PHB2_COS_MAP_PARITY_CONTROLr, PARITY_ENf,
        PHB2_COS_MAP_PARITY_STATUS_INTRr, NULL,
        PHB2_COS_MAP_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC _soc_katana_parity_info_t _soc_katana_ip2_2_parity_info[] = {
    /* Block-IPIPE; Stage-ISW2 */

    /* EMIRROR_CONTROL_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EMIRROR_CONTROL_PAR_ERRf,
        EMIRROR_CONTROL_PAR_ERRf,
        EMIRROR_CONTROLm, NULL,
        EMIRROR_CONTROL_PARITY_CONTROLr, PARITY_ENf,
        EMIRROR_CONTROL_PARITY_STATUS_INTRr, NULL,
        EMIRROR_CONTROL_PARITY_STATUS_NACKr, NULL },
    /* EMIRROR_CONTROL1_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EMIRROR_CONTROL1_PAR_ERRf,
        EMIRROR_CONTROL1_PAR_ERRf,
        EMIRROR_CONTROL1m, NULL,
        EMIRROR_CONTROL1_PARITY_CONTROLr, PARITY_ENf,
        EMIRROR_CONTROL1_PARITY_STATUS_INTRr, NULL,
        EMIRROR_CONTROL1_PARITY_STATUS_NACKr, NULL },
    /* EMIRROR_CONTROL2_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EMIRROR_CONTROL2_PAR_ERRf,
        EMIRROR_CONTROL2_PAR_ERRf,
        EMIRROR_CONTROL2m, NULL,
        EMIRROR_CONTROL2_PARITY_CONTROLr, PARITY_ENf,
        EMIRROR_CONTROL2_PARITY_STATUS_INTRr, NULL,
        EMIRROR_CONTROL2_PARITY_STATUS_NACKr, NULL },
    /* EMIRROR_CONTROL3_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        EMIRROR_CONTROL3_PAR_ERRf,
        EMIRROR_CONTROL3_PAR_ERRf,
        EMIRROR_CONTROL3m, NULL,
        EMIRROR_CONTROL3_PARITY_CONTROLr, PARITY_ENf,
        EMIRROR_CONTROL3_PARITY_STATUS_INTRr, NULL,
        EMIRROR_CONTROL3_PARITY_STATUS_NACKr, NULL },
    /* SW2_EOP_BUFFER_A_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SW2_EOP_BUFFER_A_PAR_ERRf,
        SW2_EOP_BUFFER_A_PAR_ERRf,
        INVALIDm, "SW2_EOP_BUFFER_A",
        SW2_EOP_BUFFER_A_PARITY_CONTROLr, PARITY_ENf,
        SW2_EOP_BUFFER_A_PARITY_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    /* SW2_EOP_BUFFER_B_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SW2_EOP_BUFFER_B_PAR_ERRf,
        SW2_EOP_BUFFER_B_PAR_ERRf,
        INVALIDm, "SW2_EOP_BUFFER_B",
        SW2_EOP_BUFFER_B_PARITY_CONTROLr, PARITY_ENf,
        SW2_EOP_BUFFER_B_PARITY_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    /* OAM_LM_COUNTERS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_COUNTER, NULL, 0,
        OAM_LM_COUNTERS_PAR_ERRf,
        OAM_LM_COUNTERS_PAR_ERRf,
        OAM_LM_COUNTERSm, "Ingress Pipeline Counter",
        OAM_LM_COUNTERS_PARITY_CONTROLr, PARITY_ENf,
        OAM_LM_COUNTERS_PARITY_STATUS_INTRr, NULL,
        OAM_LM_COUNTERS_PARITY_STATUS_NACKr, NULL },
    /* IP_COUNTERS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_COUNTER, NULL, 0,
        IP_COUNTERS_PAR_ERRf,
        IP_COUNTERS_PAR_ERRf,
        INVALIDm, "Ingress Pipeline Counter",
        IP_COUNTERS_PARITY_CONTROLr, PARITY_ENf,
        IP_COUNTERS_PARITY_STATUS_INTRr, NULL,
        IP_COUNTERS_PARITY_STATUS_NACKr, NULL },
    /* RDBGC_MEM_INST0_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_COUNTER, NULL, 0,
        RDBGC_MEM_INST0_PAR_ERRf,
        RDBGC_MEM_INST0_PAR_ERRf,
        INVALIDm, "Ingress Pipeline Counter",
        RDBGC_MEM_INST0_PARITY_CONTROLr, PARITY_ENf,
        RDBGC_MEM_INST0_PARITY_STATUS_INTRr, NULL,
        RDBGC_MEM_INST0_PARITY_STATUS_NACKr, NULL },
    /* RDBGC_MEM_INST1_PARITY_STATUS_INTRr */
    { _SOC_PARITY_TYPE_COUNTER, NULL, 0,
        RDBGC_MEM_INST1_PAR_ERRf,
        RDBGC_MEM_INST1_PAR_ERRf,
        INVALIDm, "RX Debug Counter",
        RDBGC_MEM_INST1_PARITY_CONTROLr, PARITY_ENf,
        RDBGC_MEM_INST1_PARITY_STATUS_INTRr, NULL,
        RDBGC_MEM_INST1_PARITY_STATUS_NACKr, NULL },
    /* RDBGC_MEM_INST2_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_COUNTER, NULL, 0,
        RDBGC_MEM_INST2_PAR_ERRf,
        RDBGC_MEM_INST2_PAR_ERRf,
        INVALIDm, "RX Debug Counter",
        RDBGC_MEM_INST2_PARITY_CONTROLr, PARITY_ENf,
        RDBGC_MEM_INST2_PARITY_STATUS_INTRr, NULL,
        RDBGC_MEM_INST2_PARITY_STATUS_NACKr, NULL },
    /* HG_COUNTERS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_COUNTER, NULL, 0,
        HG_COUNTERS_PAR_ERRf,
        HG_COUNTERS_PAR_ERRf,
        INVALIDm, "Ingress Pipeline HG Counter",
        HG_COUNTERS_PARITY_CONTROLr, PARITY_ENf,
        HG_COUNTERS_PARITY_STATUS_INTRr, NULL,
        HG_COUNTERS_PARITY_STATUS_NACKr, NULL },
    /* NIV_ERROR_DROP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        NIV_ERROR_DROP_PAR_ERRf,
        NIV_ERROR_DROP_PAR_ERRf,
        INVALIDm, "ING_NIV_RX_FRAMES_ERROR_DROP",
        NIV_ERROR_DROP_PARITY_CONTROLr, PARITY_ENf,
        NIV_ERROR_DROP_PARITY_STATUS_INTRr, NULL,
        NIV_ERROR_DROP_PARITY_STATUS_NACKr, NULL },
    /* NIV_FORWARDING_DROP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        NIV_FORWARDING_DROP_PAR_ERRf,
        NIV_FORWARDING_DROP_PAR_ERRf,
        INVALIDm, "ING_NIV_RX_FRAMES_FORWARDING_DROP",
        NIV_FORWARDING_DROP_PARITY_CONTROLr, PARITY_ENf,
        NIV_FORWARDING_DROP_PARITY_STATUS_INTRr, NULL,
        NIV_FORWARDING_DROP_PARITY_STATUS_NACKr, NULL },
    /* NIV_VLAN_TAGGED_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        NIV_VLAN_TAGGED_PAR_ERRf,
        NIV_VLAN_TAGGED_PAR_ERRf,
        INVALIDm, "ING_NIV_RX_FRAMES_VLAN_TAGGED",
        NIV_VLAN_TAGGED_PARITY_CONTROLr, PARITY_ENf,
        NIV_VLAN_TAGGED_PARITY_STATUS_INTRr, NULL,
        NIV_VLAN_TAGGED_PARITY_STATUS_NACKr, NULL },
    /* TRILL_RX_PKTS_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        TRILL_RX_PKTS_PAR_ERRf,
        TRILL_RX_PKTS_PAR_ERRf,
        INVALIDm, "ING_TRILL_RX_PKTS",
        TRILL_RX_PKTS_PARITY_CONTROLr, PARITY_ENf,
        TRILL_RX_PKTS_PARITY_STATUS_INTRr, NULL,
        TRILL_RX_PKTS_PARITY_STATUS_NACKr, NULL },
    /* TRILL_RX_ACCESS_PORT_TRILL_PKTS_DISCARDED_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        TRILL_RX_ACCESS_PORT_TRILL_PKTS_DISCARDED_PAR_ERRf,
        TRILL_RX_ACCESS_PORT_TRILL_PKTS_DISCARDED_PAR_ERRf,
        INVALIDm, "ING_TRILL_RX_ACCESS_PORT_TRILL_PKTS_DISCARDED",
        TRILL_RX_ACCESS_PORT_TRILL_PKTS_DISCARDED_PARITY_CONTROLr, PARITY_ENf,
        TRILL_RX_ACCESS_PORT_TRILL_PKTS_DISCARDED_PARITY_STATUS_INTRr, NULL,
        TRILL_RX_ACCESS_PORT_TRILL_PKTS_DISCARDED_PARITY_STATUS_NACKr, NULL },
    /* TRILL_RX_NETWORK_PORT_NON_TRILL_PKTS_DISCARDED_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        TRILL_RX_NETWORK_PORT_NON_TRILL_PKTS_DISCARDED_PAR_ERRf,
        TRILL_RX_NETWORK_PORT_NON_TRILL_PKTS_DISCARDED_PAR_ERRf,
        INVALIDm, "ING_TRILL_RX_NETWORK_PORT_NON_TRILL_PKTS_DISCARDED",
        TRILL_RX_NETWORK_PORT_NON_TRILL_PKTS_DISCARDED_PARITY_CONTROLr, PARITY_ENf,
        TRILL_RX_NETWORK_PORT_NON_TRILL_PKTS_DISCARDED_PARITY_STATUS_INTRr, NULL,
        TRILL_RX_NETWORK_PORT_NON_TRILL_PKTS_DISCARDED_PARITY_STATUS_NACKr, NULL },
    /* CPB_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        CPB_PAR_ERRf,
        CPB_PAR_ERRf,
        INVALIDm, "CPB",
        CPB_PARITY_CONTROLr, PARITY_ENf,
        CPB_PARITY_STATUS_INTRr, NULL,
        INVALIDr, NULL },
    /* ING_QUEUE_MAP_PARITY_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        ING_QUEUE_MAP_PAR_ERRf,
        ING_QUEUE_MAP_PAR_ERRf,
        ING_QUEUE_MAPm, "ING_QUEUE_MAP",
        ING_QUEUE_MAP_PARITY_CONTROLr, PARITY_ENf,
        ING_QUEUE_MAP_PARITY_STATUS_INTRr, NULL,
        ING_QUEUE_MAP_PARITY_STATUS_NACKr, NULL },
    /* ING_SVM_MACROFLOW_INDEX_TABLE_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SVC_MACROFLOW_TABLE_PAR_ERRf,
        SVC_MACROFLOW_TABLE_PAR_ERRf,
        SVM_MACROFLOW_INDEX_TABLEm, "SVM MACROFLOW",
        ING_SVM_MACROFLOW_INDEX_TABLE_CONTROLr, PARITY_ENf,
        ING_SVM_MACROFLOW_INDEX_TABLE_PARITY_STATUS_INTRr, NULL,
        ING_SVM_MACROFLOW_INDEX_TABLE_PARITY_STATUS_NACKr, NULL },
    /* ING_SVM_OFFSET_TABLE_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SVC_OFFSET_TABLE_PAR_ERRf,
        SVC_OFFSET_TABLE_PAR_ERRf,
        SVM_OFFSET_TABLEm, "SVM OFFSET",
        ING_SVM_OFFSET_TABLE_CONTROLr, PARITY_ENf,
        ING_SVM_OFFSET_TABLE_PARITY_STATUS_INTRr, NULL,
        ING_SVM_OFFSET_TABLE_PARITY_STATUS_NACKr, NULL },
    /* ING_SVM_OFFSET_TABLE_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SVC_METER_TABLE_PAR_ERRf,
        SVC_METER_TABLE_PAR_ERRf,
        SVM_METER_TABLEm, "SVM METER",
        ING_SVM_METER_TABLE_CONTROLr, PARITY_ENf,
        ING_SVM_METER_TABLE_PARITY_STATUS_INTRr, NULL,
        ING_SVM_METER_TABLE_PARITY_STATUS_NACKr, NULL },
    /* ING_SVM_OFFSET_TABLE_CONTROLr */
    { _SOC_PARITY_TYPE_PARITY, NULL, 0,
        SVC_POLICY_TABLE_PAR_ERRf,
        SVC_POLICY_TABLE_PAR_ERRf,
        SVM_POLICY_TABLEm, "SVM POLICY",
        ING_SVM_POLICY_TABLE_CONTROLr, PARITY_ENf,
        ING_SVM_POLICY_TABLE_PARITY_STATUS_INTRr, NULL,
        ING_SVM_POLICY_TABLE_PARITY_STATUS_NACKr, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC const
_soc_katana_parity_route_block_t  _soc_katana_parity_route_blocks[] = {
    { 0x00000001, /* MMU_TO_CMIC_MEMFAIL_INTR */
        SOC_BLK_MMU, MMU_INTR_MASKr, MMU_INTRr,
        NULL, 0},
    { 0x00000002, /* EP1_TO_CMIC_PERR_INTR */
        SOC_BLK_EPIPE, EGR_INTR0_ENABLEr, EGR_INTR0_STATUSr,
        _soc_katana_ep0_parity_info },
    { 0x00000004, /* EP2_TO_CMIC_PERR_INTR */
        SOC_BLK_EPIPE, EGR_INTR1_ENABLEr, EGR_INTR1_STATUSr,
        _soc_katana_ep1_parity_info, 0},
    { 0x00000008, /* IP0_TO_CMIC_PERR_INTR */
        SOC_BLK_IPIPE, IP0_INTR_ENABLEr, IP0_INTR_STATUSr,
        _soc_katana_ip0_parity_info, 0},
    { 0x00000010, /* IP1_TO_CMIC_PERR_INTR */
        SOC_BLK_IPIPE, IP1_INTR_ENABLEr, IP1_INTR_STATUSr,
        _soc_katana_ip1_parity_info, 0},
    { 0x00000010, /* IP1_TO_CMIC_PERR_INTR */
        SOC_BLK_IPIPE, IP1_INTR_ENABLE_1r, IP1_INTR_STATUS_1r,
        _soc_katana_ip1_1_parity_info, 0},
    { 0x00000020, /* IP2_TO_CMIC_PERR_INTR */
        SOC_BLK_IPIPE, IP2_INTR_ENABLEr, IP2_INTR_STATUSr,
        _soc_katana_ip2_parity_info, 0},
    { 0x00000020, /* IP2_TO_CMIC_PERR_INTR */
        SOC_BLK_IPIPE, IP2_INTR_ENABLE_1r, IP2_INTR_STATUS_1r,
        _soc_katana_ip2_1_parity_info, 0},
    { 0x00000020, /* IP2_TO_CMIC_PERR_INTR */
        SOC_BLK_IPIPE, IP2_INTR_ENABLE_2r, IP2_INTR_STATUS_2r,
        _soc_katana_ip2_2_parity_info, 0},
    /*  0x00000040 PCIE_REPLAY_PERR */
#ifdef _KATANA_DEBUG
    /* MXQ0_TO_CMIC_PERR_INTR */
    { 0x00000080,
        SOC_BLK_MXQPORT, XPORT_INTR_ENABLEr, XPORT_INTR_STATUSr,
        _soc_katana_xport_parity_info, 0},
    /* MXQ1_TO_CMIC_PERR_INTR */
    { 0x00000100,
        SOC_BLK_MXQPORT, XPORT_INTR_ENABLEr, XPORT_INTR_STATUSr,
        _soc_katana_xport_parity_info, 1},
    /* MXQ2_TO_CMIC_PERR_INTR */
    { 0x00000200,
        SOC_BLK_MXQPORT, XPORT_INTR_ENABLEr, XPORT_INTR_STATUSr,
        _soc_katana_xport_parity_info, 2},
    /* MXQ3_TO_CMIC_PERR_INTR */
    { 0x00000400,
        SOC_BLK_MXQPORT, XPORT_INTR_ENABLEr, XPORT_INTR_STATUSr,
        _soc_katana_xport_parity_info, 3},
    /* 0x00000800 PULL_DOWN */
#endif
    { 0 } /* table terminator */
};

STATIC _soc_katana_counter_info_t _soc_katana_counter_info[] = {
    { _SOC_COUNTER_TYPE_DROP_ENQ, 0x0, 0 },
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ, 0x0, 1 },
    /* green pkt drops, index = ACCEPT=1'b0, COLOR=2'b00 */
    { _SOC_COUNTER_TYPE_DROP_ENQ_GREEN, 0x0, 2 },
    /* yellow pkt drops, index = ACCEPT=1'b0, COLOR=2'b11 */
    { _SOC_COUNTER_TYPE_DROP_ENQ_YELLOW, 0x3, 3 },
    /* red pkt drops, index = ACCEPT=1'b0, COLOR=2'b01 */
    { _SOC_COUNTER_TYPE_DROP_ENQ_RED, 0x1, 4 },
    /* green accepted pkts, index = ACCEPT=1'b1, COLOR=2'b00 */
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN, 0x4, 5 },
    /* yellow accepted pkts, index = ACCEPT=1'b1, COLOR=2'b11 */
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ_YELLOW, 0x7, 6 },
    /* red accepted pkts, index = ACCEPT=1'b1, COLOR=2'b01 */
    { _SOC_COUNTER_TYPE_ACCEPT_ENQ_RED, 0x5, 7 },
    { _SOC_COUNTER_TYPE_ACCEPT_DEQ, 0, 8 } 
};

/* KT OAM */
STATIC
soc_field_t _soc_kt_oam_interrupt_fields[] = {
    SOME_RMEP_CCM_DEFECTf,
    SOME_RDI_DEFECTf,
    ERROR_CCM_DEFECTf,
    XCON_CCM_DEFECTf,
    ANY_RMEP_TLV_INTERFACE_UPf,
    ANY_RMEP_TLV_INTERFACE_DOWNf,
    ANY_RMEP_TLV_PORT_UPf,
    ANY_RMEP_TLV_PORT_DOWNf,
    INVALIDf
};

static soc_ser_functions_t _kt_ser_functions;
static soc_oam_event_functions_t _kt_oam_event_functions;


#define _SOC_KT_SER_REG 1
#define _SOC_KT_SER_MEM 0

typedef union _kt_ser_nack_reg_mem_u {
    soc_reg_t reg;
    soc_mem_t mem;
} _kt_ser_nack_reg_mem_t;

static int _stat_error_fixed[SOC_MAX_NUM_DEVICES];

STATIC int
soc_kt_oam_interrupt_process(int unit);

STATIC int
_soc_katana_parity_enable_info(int unit, uint8 id, 
                                soc_reg_t group_reg,
                                uint32 *group_rval,
                                const _soc_katana_parity_info_t *info_list,
                                int enable);

extern sal_mutex_t cosq_sync_lock[SOC_MAX_NUM_DEVICES];
#define KT_COSQ_LOCK(unit)      do { \
                                    if (cosq_sync_lock[unit]) { \
                                        sal_mutex_take(cosq_sync_lock[unit], \
                                                       sal_mutex_FOREVER); \
                                    } \
                                } while (0)
#define KT_COSQ_UNLOCK(unit)    do { \
                                    if (cosq_sync_lock[unit]) { \
                                        sal_mutex_give(cosq_sync_lock[unit]); \
                                    } \
                                } while (0)
void
soc_kt_oam_handler_register(int unit, soc_kt_oam_handler_t handler)
{
    int rv, fidx = 0;
    uint32 rval;

    en_oam_handler[unit] = handler;

    rv = READ_IP1_INTR_ENABLE_1r(unit, &rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error reading %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_ENABLE_1r)));
    }
    while (_soc_kt_oam_interrupt_fields[fidx] != INVALIDf) {
        soc_reg_field_set(unit, IP1_INTR_ENABLE_1r, &rval,
                          _soc_kt_oam_interrupt_fields[fidx], 1);
        fidx++;
    }
    rv = WRITE_IP1_INTR_ENABLE_1r(unit, rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error writing %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_ENABLE_1r)));
    }
    /* Write CMIC enable register for oam event*/
    (void)soc_cmicm_intr2_enable(unit, KT_OAM_CMIC_INTR_MASK);
}

STATIC
int _soc_katana_process_sb_type_1_ci_mmu_parity_enable(int unit, 
                      soc_block_t block,
                      soc_reg_t  en_reg, 
                      int enable,
                      _soc_katana_mmu_sub_block_type_1_info_t * sub_block_info)
{
    uint32 en_rval;
    uint32 intr_bit;
    int sb_index = 0;
    soc_port_t port = 0;
    _soc_katana_mmu_sub_block_type_1_info_t *sub_block;

    port = SOC_BLOCK_PORT(unit, block);
    SOC_IF_ERROR_RETURN(READ_CI_ERROR_MASKr(unit, port, &en_rval));

    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            break;
        }
        soc_reg_field_set(unit, en_reg, &en_rval, sub_block->sub_block_en_field,
                                                 enable ? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_CI_ERROR_MASKr(unit, port, en_rval));
    }
    return SOC_E_NONE;
}

STATIC
int _soc_katana_process_sb_type_1_mmu_parity_enable(int unit, 
                      soc_reg_t  en_reg, 
                      int enable,
                      _soc_katana_mmu_sub_block_type_1_info_t * sub_block_info)
{
    uint32 addr,  en_rval;
    uint32 intr_bit;
    int sb_index = 0;
    _soc_katana_mmu_sub_block_type_1_info_t *sub_block;

    addr = soc_reg_addr(unit, en_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));

    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            break;
        }
        if (sub_block->sub_block_en_field == FLUSH_COMPLETE_DISINTf) {
            continue;
        }
        soc_reg_field_set(unit, en_reg, &en_rval, sub_block->sub_block_en_field,
                                                 enable ? 0 : 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));
    }
    return SOC_E_NONE;
}

STATIC
int _soc_katana_process_sb_type_2_mmu_parity_enable(int unit, 
                      soc_reg_t  en_reg, 
                      int enable,
                      _soc_katana_mmu_sub_block_type_2_info_t * sub_block_info)
{
    uint32 addr,  en_rval;
    uint32 intr_bit;
    int sb_index = 0;
    _soc_katana_mmu_sub_block_type_2_info_t *sub_block;

    addr = soc_reg_addr(unit, en_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));

    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            break;
        }
        if (SOC_REG_FIELD_VALID(unit, en_reg, sub_block->sub_block_en_field)) {
            soc_reg_field_set(unit, en_reg, &en_rval, 
                              sub_block->sub_block_en_field, enable ? 0 : 1);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));
        }
    }
    return SOC_E_NONE;
}

STATIC
int _soc_katana_process_sb_mmu_lvl1_parity_enable(int unit, 
                      soc_reg_t  en_reg,
                      int enable,
                      _soc_katana_mmu_sub_block_type_3_info_t * sub_block_info)
{
    uint32 addr = 0;
    uint32 en_rval;
    uint32 intr_bit;
    int sb_index = 0;
    _soc_katana_mmu_sub_block_type_3_info_t *sub_block;
    _soc_katana_mmu_lvl2_subblock_type_t lvl2_sub_block;

    if (en_reg != 0) {
        addr = soc_reg_addr(unit, en_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));
    }

    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            return SOC_E_NONE;
        }
        if ((en_reg != 0) && (sub_block->sub_block_en_field != 0)) {
            soc_reg_field_set(unit, en_reg, &en_rval, 
                              sub_block->sub_block_en_field, enable ? 0 : 1);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));
        }
        lvl2_sub_block = sub_block->lvl2_sub_block;
        switch (lvl2_sub_block) {
            case _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_2:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               EMC_ERROR_MASK_2r,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_1:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                               EMC_ERROR_MASK_1r,
                                               enable,
                                               sub_block->info);
                break;   
            case _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_0:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                               EMC_ERROR_MASK_0r,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_3:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               DEQ_ERROR_MASK_3r,
                                               enable,
                                               sub_block->info);
                    break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_2:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               DEQ_ERROR_MASK_2r,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_1:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               DEQ_ERROR_MASK_1r,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_0:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               DEQ_ERROR_MASK_0r,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_U:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               QSTRUCT_QENTRY_UPPER_ERROR_MASKr,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_L:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               QSTRUCT_QENTRY_LOWER_ERROR_MASKr,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QBLOCK:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               QSTRUCT_QBLOCK_NEXT_ERROR_MASKr,
                                               enable,
                                               sub_block->info);
                break;   

            case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_FAP:
                _soc_katana_process_sb_type_1_mmu_parity_enable(unit,
                                               QSTRUCT_FAP_MEM_ERROR_MASKr,
                                               enable,
                                               sub_block->info);
                break;   

            default:
                break;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_enable_mmu(int unit, soc_reg_t mmu_en_reg, int enable)
{
    /* Go through each sub-block and enable the interrupt */

    uint32 addr, en_rval;
    int sub_block_index;
    uint32 mmu_bit;
    const _soc_katana_mmu_sub_block_type_1_t *sub_block;
    const _soc_katana_mmu_sub_block_type_2_t *sub_block_type2;
    const _soc_katana_mmu_sub_block_type_3_t *lvl1_sub_block;
    _soc_katana_mmu_subblock_type_t sub_block_type;

    for (sub_block_index = 0; ; sub_block_index++) {
        sub_block = &_soc_katana_mmu_sub_blocks_type_1[sub_block_index];
        sub_block_type = sub_block->mmu_sub_block;
        mmu_bit = sub_block->mmu_bit;
        if(mmu_bit == 0) {
            /* End of table */
            break;
        }
        addr = soc_reg_addr(unit, mmu_en_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));
        soc_reg_field_set(unit, mmu_en_reg, &en_rval, 
                          sub_block->sub_block_en_reg, enable ? 0 : 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));

        switch (sub_block_type) {

            case _SOC_KT_MMU_SUBBLOCK_MEM1:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  CLINK_ERROR_MASKr, 
                                                  enable, 
                                                  sub_block->info);
                break;

            case _SOC_KT_MMU_SUBBLOCK_E2EFC:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  MMU_E2EFC_ERROR_0_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_THDI:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  MMU_THDI_INTR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

   
            case _SOC_KT_MMU_SUBBLOCK_CTR:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  CTR_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_CCP:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  CCP_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_TOQ:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  TOQ_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   


            case _SOC_KT_MMU_SUBBLOCK_CFAPI:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  CFAPI_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_CFAPE:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  CFAPE_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ITE:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  MMU_ITE_ERROR_0_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ITE_CFG:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  MMU_ITE_CFG_ECC_ERROR_0_MASKr,
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ENQ_CFG:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  MMU_ENQ_CFG_ECC_ERROR_0_MASKr,
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ENQ:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  MMU_ENQ_ERROR_0_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ENQ_FAP:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  MMU_ENQ_FAP_ECC_ERROR_0_MASKr,
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_THDO_STATUS1:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  THDO_PARITY_ERROR_MASK1r,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_THDO_STATUS2:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  THDO_PARITY_ERROR_MASK2r,  
                                                  enable, 
                                                  sub_block->info);
                break;   


            case _SOC_KT_MMU_SUBBLOCK_AGING_INT:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  AGING_ERROR_MASK_INTr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_AGING_EXT:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  AGING_ERROR_MASK_EXTr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_WRED:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                             WRED_PARITY_ERROR_MASKr, 
                                             enable, 
                                             sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_INTFI:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  INTFI_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

        /* Access to CI_ERROR_MASK register is resulting in S-Chan timeout on 
           some KATANA devices which do not have ddr3 memory  */
                /* for the 'i'th memory bank mask it if the bank is not present
                   setting the bit of MMU_INTR_MASK register masks interrupts coming
                   for CI'i' bank.
                   We hence do that when all parity is being enabled through this
                   function and that bank does not exist */
            case _SOC_KT_MMU_SUBBLOCK_CI2:
                if (soc_feature(unit, soc_feature_ddr3)) {
                    if ((SOC_DDR3_NUM_MEMORIES(unit) >= 3)) {
                        _soc_katana_process_sb_type_1_ci_mmu_parity_enable(unit, 
                                                      CI2_BLOCK(unit),
                                                      CI_ERROR_MASKr,  
                                                      enable, 
                                                      sub_block->info);
                    } else if (enable) {
                        soc_reg_field_set(unit, mmu_en_reg, &en_rval, 
                                          sub_block->sub_block_en_reg, 1);
                        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));
                    }
                }
                break;   
            case _SOC_KT_MMU_SUBBLOCK_CI1:
                if (soc_feature(unit, soc_feature_ddr3)) {
                    if ((SOC_DDR3_NUM_MEMORIES(unit) >= 2)) {
                        _soc_katana_process_sb_type_1_ci_mmu_parity_enable(unit, 
                                                      CI1_BLOCK(unit),
                                                      CI_ERROR_MASKr,  
                                                      enable, 
                                                      sub_block->info);
                    } else if (enable) {
                        soc_reg_field_set(unit, mmu_en_reg, &en_rval, 
                                          sub_block->sub_block_en_reg, 1);
                        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));
                    }
                }
                break;   
            case _SOC_KT_MMU_SUBBLOCK_CI0:
                if (soc_feature(unit, soc_feature_ddr3)) {
                    if ((SOC_DDR3_NUM_MEMORIES(unit) >= 1)) {
                        _soc_katana_process_sb_type_1_ci_mmu_parity_enable(unit, 
                                                      CI0_BLOCK(unit),
                                                      CI_ERROR_MASKr,  
                                                      enable, 
                                                      sub_block->info);
                    } else if (enable) {
                        soc_reg_field_set(unit, mmu_en_reg, &en_rval, 
                                          sub_block->sub_block_en_reg, 1);
                        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));
                    }
                }
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  LLS_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_PORT:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  LLS_PORT_ECC_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_L0_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  LLS_L0_ECC_ERROR1_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_UPD2:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  LLS_ERROR_UPD2_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_L1_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  LLS_L1_ECC_ERROR1_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_L2_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  LLS_L2_ECC_ERROR1_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_LLS_MISC_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  LLS_MISC_ECC_ERROR1_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   
#ifdef _KATANA_DEBUG
            /*  RQE Ser gives MMU parity error while accessing IPMC group
                tables. Hence, disabling parity for this */
            case _SOC_KT_MMU_SUBBLOCK_RQE_SER:
                    _soc_katana_process_sb_type_1_mmu_parity_enable(unit, 
                                                  RQE_SER_MASKr,  
                                                  enable, 
                                                  sub_block->info);
                break;   
#endif /* _KATANA_DEBUG */

            default:
                break;   

        }
    }
    for (sub_block_index = 0; ; sub_block_index++) {
        sub_block_type2 = &_soc_katana_mmu_sub_blocks_type_2[sub_block_index];
        mmu_bit = sub_block_type2->mmu_bit;
        sub_block_type = sub_block_type2->mmu_sub_block;
        if(mmu_bit == 0) {
            /* End of table */
            break;
        }
        switch (sub_block_type) {
            case _SOC_KT_MMU_SUBBLOCK_RQE_EXTQ:
                    _soc_katana_process_sb_type_2_mmu_parity_enable(unit, 
                                                  RQE_EXTQ_REPLICATION_LIMITr, 
                                                  enable, 
                                                  sub_block_type2->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_INTFI_ECC:
                    _soc_katana_process_sb_type_2_mmu_parity_enable(unit, 
                                                  INTFI_ERROR_MASKr,  
                                                  enable, 
                                                  sub_block_type2->info);
                break;   

            default:
                break;   

        }
    }

    for (sub_block_index = 0; ; sub_block_index++) {
        lvl1_sub_block = &_soc_katana_mmu_sub_blocks_type_3[sub_block_index];
        mmu_bit = lvl1_sub_block->mmu_bit;
        sub_block_type = lvl1_sub_block->mmu_sub_block;
        if(mmu_bit == 0) {
            /* End of table */
            break;
        }

        addr = soc_reg_addr(unit, mmu_en_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));
        soc_reg_field_set(unit, mmu_en_reg, &en_rval, 
                          lvl1_sub_block->sub_block_en_reg, enable ? 0 : 1);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, en_rval));

        switch (sub_block_type) {
        /* Access to EMC_ERROR_X_MASK register is resulting is S-Chan timeout on
           some KATANA devices which do not have ddr3 memory  */
            case _SOC_KT_MMU_SUBBLOCK_EMC:
                if (soc_feature(unit, soc_feature_ddr3)) {
                    _soc_katana_process_sb_mmu_lvl1_parity_enable(unit, 
                                                  0,
                                                  enable, 
                                                  lvl1_sub_block->info);
                }
                break;   
            case _SOC_KT_MMU_SUBBLOCK_DEQ:
                    _soc_katana_process_sb_mmu_lvl1_parity_enable(unit, 
                                                  0,
                                                  enable, 
                                                  lvl1_sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_QSTRUCT:
                    _soc_katana_process_sb_mmu_lvl1_parity_enable(unit, 
                                                  QSTRUCT_INTERRUPT_MASKr, 
                                                  enable, 
                                                  lvl1_sub_block->info);
                break;   

            default:
                break;   

        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_enable_mxqport(int unit, uint8 id,
                                  const _soc_katana_parity_info_t *info,
                                  int enable)
{
    soc_reg_t mxqport_reg;
    uint32 addr, mxqport_rval;
    int blk, port;

    port = -1;
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
        if (SOC_BLOCK_INFO(unit, blk).number == id) {
            port = SOC_BLOCK_PORT(unit, blk);
            break;
        }
    }
    if (port < 0) {
        return SOC_E_NONE;
    }

    mxqport_reg = info->enable_reg;
    addr = soc_reg_addr(unit, mxqport_reg, port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &mxqport_rval));

    SOC_IF_ERROR_RETURN
        (_soc_katana_parity_enable_info(unit, id, mxqport_reg, &mxqport_rval,
                                         info->info, enable));

    addr = soc_reg_addr(unit, mxqport_reg, port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, mxqport_rval));

    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_enable_info(int unit, uint8 id,
                                soc_reg_t group_reg,
                                uint32 *group_rval,
                                const _soc_katana_parity_info_t *info_list,
                                int enable)
{
    const _soc_katana_parity_info_t *info;
    int info_index;
    soc_reg_t reg;
    uint32 addr, rval;

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }

        /* Enable the info entry in the group register */
        soc_reg_field_set(unit, group_reg, group_rval,
                          info->group_reg_enable_field, enable ? 1 : 0);

        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_PARITY_TYPE_PARITY:
        case _SOC_PARITY_TYPE_ECC:
        case _SOC_PARITY_TYPE_HASH:
        case _SOC_PARITY_TYPE_EDATABUF:
        case _SOC_PARITY_TYPE_COUNTER:
            reg = info->enable_reg;
            if (!SOC_REG_IS_VALID(unit, reg)) {
                continue;
            }
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
            soc_reg_field_set(unit, reg, &rval, info->enable_field,
                              enable ? 1 : 0);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
            break;
        case _SOC_PARITY_TYPE_MXQPORT:
            /* One more level of report tree structure */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_enable_mxqport(unit, id, info, enable));
            break;
        case _SOC_PARITY_TYPE_GENERIC:
        default:
            break;
        } /* Handle different parity error reporting style */
    } /* Loop through each info entry in the route block */

    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_enable_all(int unit, int enable)
{
    const _soc_katana_parity_route_block_t *route_block;
    int route_block_index;
    soc_reg_t route_block_en_reg;
    uint32 cmic_rval, route_block_rval;
    uint32 addr, cmic_bit;
    uint32 rval;

    cmic_rval = 0;
    /* Loop through each place-and-route block entry */
    for (route_block_index = 0; ; route_block_index++) {
        route_block = &_soc_katana_parity_route_blocks[route_block_index];
        cmic_bit = route_block->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        /* Enable the route block entry in the CMIC register */
        if (enable) {
            cmic_rval |= cmic_bit;
        }

        route_block_en_reg = route_block->enable_reg;
        addr = soc_reg_addr(unit, route_block_en_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &route_block_rval));

        if (route_block->blocktype == SOC_BLK_MMU) {
            SOC_IF_ERROR_RETURN
              (_soc_katana_parity_enable_mmu(unit, route_block_en_reg, enable));
        } else {
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_enable_info(unit,
                                                 route_block->id,
                                                 route_block_en_reg,
                                                 &route_block_rval,
                                                 route_block->info, enable));

            /* Write per route block parity enable register */
            addr = soc_reg_addr(unit, route_block_en_reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, route_block_rval));
        }
    } /* Loop through each place-and-route block entry */

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    if (enable) {
        /* MMU enables */
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_CHK_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_GEN_ENf, 1);
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
        /* Write CMIC enable register */
        (void)soc_cmicm_intr2_enable(unit, cmic_rval);
    } else {
        /* MMU disables */
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_CHK_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_GEN_ENf, 0);
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
        /* Write CMIC disable register */
        (void)soc_cmicm_intr2_disable(unit, cmic_rval);
    }
    return SOC_E_NONE;
}

void
_soc_katana_mem_rename(soc_mem_t *memory)
{
    soc_mem_t mem = *memory;
    
    switch(*memory) {
    case VLAN_SUBNET_DATA_ONLYm:
        mem = VLAN_SUBNETm;
        break;
    case L2_ENTRY_ONLYm:
        mem = L2Xm;
        break;
    case L2_USER_ENTRY_ONLYm:
    case L2_USER_ENTRY_DATA_ONLYm:
        mem = L2_USER_ENTRYm;
        break;
    case L3_DEFIP_DATA_ONLYm:
    case L3_DEFIP_HIT_ONLYm:
        mem = L3_DEFIPm;
        break;
    case L3_DEFIP_128_DATA_ONLYm:
        mem = L3_DEFIP_128m;
        break;
    case EGR_IP_TUNNEL_IPV6m:
    case EGR_IP_TUNNEL_MPLSm:
        mem = EGR_IP_TUNNELm;
        break;
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
        mem = L3_ENTRY_ONLYm;
        break;
    case VLAN_MACm:
        mem = VLAN_XLATEm;
        break;
    default:
        /* Do nothing, keep memory as provided */
        break;
    }
    *memory = mem;
}

int
_soc_katana_mem_parity_control(int unit, soc_mem_t mem, int copyno,
                                int enable)
{
    const _soc_katana_parity_route_block_t *route_block;
    int route_block_index;
    uint32 cmic_bit;
    int info_index;
    soc_reg_t reg;
    uint32 addr, rval;
    const _soc_katana_parity_info_t *info;

    if (!soc_property_get(unit, spn_PARITY_ENABLE, FALSE)) {
        /* Parity checking is not enabled, nothing to do */
        return SOC_E_NONE;
    }

    /* Convert component/aggregate memories to the table for which
     * the parity registers correspond. */
    _soc_katana_mem_rename(&mem);

    /* Loop through each place-and-route block entry */
    for (route_block_index = 0; ; route_block_index++) {
        route_block = &_soc_katana_parity_route_blocks[route_block_index];
        cmic_bit = route_block->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }

        if (route_block->blocktype == SOC_BLK_MMU) {
            SOC_IF_ERROR_RETURN(_soc_katana_parity_enable_mmu(unit, 
                                         route_block->enable_reg, enable));
            continue;
        }

        /* Loop through each info entry in the list */
        for (info_index = 0; ; info_index++) {
            info = &route_block->info[info_index];
            if (info->type == _SOC_PARITY_TYPE_NONE) {
                /* End of table */
                break;
            }
            if (mem != INVALIDm && info->mem != mem) {
                continue;
            }

            /* Handle different parity error reporting style */
            switch (info->type) {
            case _SOC_PARITY_TYPE_PARITY:
            case _SOC_PARITY_TYPE_ECC:
            case _SOC_PARITY_TYPE_HASH:
            case _SOC_PARITY_TYPE_EDATABUF:
            case _SOC_PARITY_TYPE_COUNTER:
                reg = info->enable_reg;
                if (!SOC_REG_IS_VALID(unit, reg)) {
                    continue;
                }
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
                soc_reg_field_set(unit, reg, &rval, info->enable_field,
                                  enable ? 1 : 0);
                SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
                break;
            case _SOC_PARITY_TYPE_MXQPORT:
                /* One more level of report tree structure */
                SOC_IF_ERROR_RETURN
                    (_soc_katana_parity_enable_mxqport(unit, 
                                          route_block->id, info, enable));
                break;
            case _SOC_PARITY_TYPE_GENERIC:
            default:
                break;
            }
        }
    }

    return SOC_E_NONE;
}


STATIC int
_soc_katana_parity_process_info(int unit, uint8 id, soc_reg_t reg,
                                 uint32 status,
                                 uint32 *handled_status,
                                 const _soc_katana_parity_info_t *info_list,
                                 char *prefix_str, soc_block_t block);

STATIC int
_soc_katana_parity_process_parity(int unit,
                                   const _soc_katana_parity_info_t *info,
                                   int schan, char *prefix_str, char *mem_str, 
                                   soc_block_t block)
{
    _soc_katana_parity_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    uint32 addr, rval;
    uint32 multiple, entry_idx, idx, has_error;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci = {0};

    if (schan) {
        /* Some table does not have NACK register */
        if (info->nack_status_reg == INVALIDr &&
            info->nack_status_reg_list == NULL) {
            return SOC_E_NONE;
        }
        reg_entry[0].reg = info->nack_status_reg;
        reg_entry[0].mem_str = NULL;
        reg_entry[1].reg = INVALIDr;
        reg_ptr = reg_entry;
    } else {
        if (info->intr_status_reg != INVALIDr) {
            reg_entry[0].reg = info->intr_status_reg;
            reg_entry[0].mem_str = NULL;
            reg_entry[1].reg = INVALIDr;
            reg_ptr = reg_entry;
        } else if (info->intr_status_reg_list != NULL) {
            reg_ptr = info->intr_status_reg_list;
        } else {
            return SOC_E_NONE;
        }
    }

    has_error = FALSE;
    for (idx = 0; reg_ptr[idx].reg != INVALIDr; idx++) {
        reg = reg_ptr[idx].reg;
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ?
            reg_ptr[idx].mem_str : mem_str;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));

        if (soc_reg_field_get(unit, reg, rval, PARITY_ERRf)) {
            has_error = TRUE;
            multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
            entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s entry %d parity error\n"),
                       prefix_str, mem_str_ptr, entry_idx));
            if (multiple) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s has multiple parity errors\n"),
                           prefix_str, mem_str_ptr));
            }
            if (info->mem != INVALIDm) {
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = info->mem;
                spci.blk_type = block;
                spci.index = entry_idx;
                (void)soc_ser_correction(unit, &spci);
            }
            /* Clear parity status */
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        }

    }

    if (!has_error) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s parity hardware inconsistency\n"),
                   prefix_str, mem_str));
    }

    return SOC_E_NONE;
} /*_soc_katana_parity_process_parity*/

STATIC int
_soc_katana_parity_process_ecc(int unit,
                                const _soc_katana_parity_info_t *info,
                                int schan, char *prefix_str, char *mem_str, 
                                soc_block_t block)
{
    _soc_katana_parity_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    uint32 addr, rval;
    uint32 multiple, double_bit, entry_idx, idx, has_error;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci = {0};

    if (schan) {
        /* Some table does not have NACK register */
        if (info->nack_status_reg == INVALIDr &&
            info->nack_status_reg_list == NULL) {
            return SOC_E_NONE;
        }
        reg_entry[0].reg = info->nack_status_reg;
        reg_entry[0].mem_str = NULL;
        reg_entry[1].reg = INVALIDr;
        reg_ptr = reg_entry;
    } else {
        if (info->intr_status_reg != INVALIDr) {
            reg_entry[0].reg = info->intr_status_reg;
            reg_entry[0].mem_str = NULL;
            reg_entry[1].reg = INVALIDr;
            reg_ptr = reg_entry;
        } else if (info->intr_status_reg_list != NULL) {
            reg_ptr = info->intr_status_reg_list;
        } else {
            return SOC_E_NONE;
        }
    }

    has_error = FALSE;
    for (idx = 0; reg_ptr[idx].reg != INVALIDr; idx++) {
        reg = reg_ptr[idx].reg;
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ?
            reg_ptr[idx].mem_str : mem_str;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));

        multiple = 0;
        double_bit = 0;
        entry_idx = 0;

        if (soc_reg_field_get(unit, reg, rval, ECC_ERRf)) {
            has_error = TRUE;
            if (SOC_REG_FIELD_VALID(unit, reg, MULTIPLE_ERRf)) {
                multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
            }
            if (SOC_REG_FIELD_VALID(unit, reg, DOUBLE_BIT_ERRf)) {
                double_bit = soc_reg_field_get(unit, reg, rval, DOUBLE_BIT_ERRf);
            }
            if (SOC_REG_FIELD_VALID(unit, reg, ENTRY_IDXf)) {
                entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            }
            if (double_bit) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s entry %d double-bit ECC error\n"),
                           prefix_str, mem_str_ptr, entry_idx));
                spci.double_bit = 1;
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s entry %d ECC error\n"),
                           prefix_str, mem_str_ptr, entry_idx));
            }

            if (multiple) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s has multiple ECC errors\n"),
                           prefix_str, mem_str_ptr));
            }

            if (info->mem != INVALIDm) {
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = info->mem; 
                spci.blk_type = block;
                spci.index = entry_idx;
                spci.parity_type = SOC_PARITY_TYPE_ECC;
                (void)soc_ser_correction(unit, &spci);
            }
            /* Clear parity status */
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        }

    }

    if (!has_error) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s ECC hardware inconsistency\n"),
                   prefix_str, mem_str));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_process_hash(int unit,
                                 const _soc_katana_parity_info_t *info,
                                 int schan, char *prefix_str, char *mem_str,
                                 soc_block_t block)
{
    _soc_katana_parity_reg_t *reg_ptr;
    soc_reg_t reg;
    uint32 addr, rval;
    uint32 bitmap, multiple, bucket_idx;
    int bucket_size, entry_idx = -1, idx, bank_bkt_idx, bits, has_error;
    _soc_ser_correct_info_t spci = {0};
    soc_field_t efield = PARITY_ERR_BMf;
    soc_field_t bfield = BUCKET_IDXf;
    soc_field_t mfield = MULTIPLE_ERRf;

    if (schan) {
        reg_ptr = info->nack_status_reg_list;
    } else {
        reg_ptr = info->intr_status_reg_list;
    }

    has_error = FALSE;
    for (idx = 0; idx < 2; idx ++) {
        reg = reg_ptr[idx].reg;
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));

        /* Some memories have this field named differently */
        if (soc_reg_field_valid(unit, reg, PARITY_ERR_BMf)) {
            efield = PARITY_ERR_BMf;
            bfield = BUCKET_IDXf;
            mfield = MULTIPLE_ERRf;
        } else if (soc_reg_field_valid(unit, reg, PARITY_ERR_BM_0f)) {
            efield = PARITY_ERR_BM_0f;
            bfield = BUCKET_IDX_0f;
            mfield = MULTIPLE_ERR_0f;
        } else if (soc_reg_field_valid(unit, reg, PARITY_ERR_BM_1f)) {
            efield = PARITY_ERR_BM_1f;
            bfield = BUCKET_IDX_1f;
            mfield = MULTIPLE_ERR_1f;
        }        

        bitmap = soc_reg_field_get(unit, reg, rval, efield);
        if (bitmap != 0) {
            has_error = TRUE;
            multiple = soc_reg_field_get(unit, reg, rval, mfield);
            bucket_size = soc_reg_field_length(unit, reg, efield);
            bucket_idx = soc_reg_field_get(unit, reg, rval, bfield) * 
                bucket_size * 2;
            bank_bkt_idx = idx * bucket_size + bucket_idx;
            for (bits = 0; bits < bucket_size; bits++) {
                if (bitmap & (1 << bits)) {
                    entry_idx = bank_bkt_idx + bits;
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s %s entry %d parity error\n"),
                                          prefix_str, mem_str, entry_idx));
                }
            }
            if (multiple) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s has multiple parity errors\n"),
                           prefix_str, mem_str));
            }

            if ((info->mem != INVALIDm) && (entry_idx != -1)) {
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = info->mem; 
                spci.blk_type = block;
                spci.index = entry_idx;
                (void)soc_ser_correction(unit, &spci);
            }

            /* Clear parity status */
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));
        }
    }

    if (!has_error) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s parity hardware inconsistency\n"),
                   prefix_str, mem_str));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_process_edatabuf(int unit,
                                     const _soc_katana_parity_info_t *info,
                                     int schan, char *prefix_str,
                                     char *mem_str)
{
    soc_reg_t reg;
    uint32 addr, rval;
    uint32 double_bit, multiple;

    if (schan) {
        /* Some table may not have NACK status register */
        if (info->nack_status_reg == INVALIDr) {
            return SOC_E_NONE;
        }
        reg = info->nack_status_reg;
    } else {
        reg = info->intr_status_reg;
    }
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));

    if (soc_reg_field_get(unit, reg, rval, ECC_ERR_MGRPf)) {
        double_bit = soc_reg_field_get(unit, reg, rval, ECC_ERR_2B_MGRPf);
        multiple = soc_reg_field_get(unit, reg, rval, ECC_MULTI_MGRPf);
        if (double_bit) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s double-bit ECC error\n"),
                       prefix_str, mem_str));
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s ECC error\n"),
                       prefix_str, mem_str));
        }
        if (multiple) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s has multiple ECC errors\n"),
                       prefix_str, mem_str));
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s ECC hardware inconsistency\n"),
                   prefix_str, mem_str));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));

    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_process_counter(int unit,
                                    const _soc_katana_parity_info_t *info,
                                    int schan, char *prefix_str, char *mem_str,
                                    soc_block_t block)
{
    soc_cmap_t *cmap;
    soc_reg_t reg, counter_reg;
    uint32 addr, rval;
    uint32 multiple, counter_idx, port_idx, entry_idx;
    char *counter_name;
    _soc_ser_correct_info_t spci = {0};

    if (schan) {
        reg = info->nack_status_reg;
    } else {
        reg = info->intr_status_reg;
    }
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));

    if (soc_reg_field_get(unit, reg, rval, PARITY_ERRf)) {
        multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
        counter_idx = soc_reg_field_get(unit, reg, rval, COUNTER_IDXf);
        port_idx = soc_reg_field_get(unit, reg, rval, PORT_IDXf);
        entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);

        /* TDBGC starts at index 0x1A of counter DMA table */
        if (info->group_reg_status_field == EGR_STATS_COUNTER_TABLE_PAR_ERRf) {
            counter_idx += 0x1A;
        }
        cmap = soc_port_cmap_get(unit, port_idx);
        counter_reg = cmap->cmap_base[counter_idx].reg;
        if (SOC_REG_IS_VALID(unit, counter_reg)) {
            counter_name = SOC_REG_NAME(unit, counter_reg);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s port %d %s entry %d parity error\n"),
                       prefix_str, mem_str, port_idx, counter_name,
                       entry_idx));
            if (multiple) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s has multiple parity errors\n"),
                           prefix_str, mem_str));
            }
            spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_KNOWN;
            spci.reg = counter_reg;
            spci.mem = INVALIDm;
            spci.blk_type = block;
            spci.port = port_idx;
            (void)soc_ser_correction(unit, &spci);
            _stat_error_fixed[unit]++;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s parity hardware inconsistency\n"),
                       prefix_str, mem_str));
        }
    } else if (!schan) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s parity hardware inconsistency\n"),
                   prefix_str, mem_str));
    }

    /* Clear parity status */
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, 0));

    return SOC_E_NONE;
}


STATIC
int _soc_katana_process_sb_type_1_mmu_parity_error(int unit, 
                      _soc_katana_mmu_subblock_type_t sub_block_type, 
                      soc_reg_t  stat_reg, soc_reg_t  en_reg,
                      _soc_katana_mmu_sub_block_type_1_info_t * sub_block_info)
{
    uint32 addr,  stat_rval, en_rval;
    uint32 intr_bit;
    int sb_index = 0;
    _soc_katana_mmu_sub_block_type_1_info_t *sub_block;
    char prefix_str[10];
    char *reg_str;
    char *field_str;

    sal_sprintf(prefix_str, "unit %d", unit);
    addr = soc_reg_addr(unit, stat_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &stat_rval));

    addr = soc_reg_addr(unit, en_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));
    stat_rval &= (~en_rval);
    if (stat_rval == 0) {
        /* How did this interrupt get asserted then??? */
        return SOC_E_NONE; 
    }
    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            return SOC_E_NONE;
        }
        if (stat_rval & intr_bit) {
             field_str = SOC_FIELD_NAME(unit, sub_block->sub_block_field);
             reg_str = SOC_REG_NAME(unit, stat_reg);
             soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                                SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                stat_reg, sub_block->sub_block_field);
             LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "%s MMU subblock %x   reg %s field %s(value = 0x%x) has \
                                   MMU parity error\n"),
                        prefix_str, sub_block_type, reg_str, 
                        field_str, stat_rval));
             /* clear the parity error bit */
             addr = soc_reg_addr(unit, stat_reg, REG_PORT_ANY, 0);
             SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, intr_bit));
        }
    }
    return SOC_E_NONE;
}

STATIC
int _soc_katana_process_sb_type_1_ci_mmu_parity_error(int unit, 
                      soc_block_t block,
                      _soc_katana_mmu_subblock_type_t sub_block_type, 
                      soc_reg_t  stat_reg, soc_reg_t  en_reg,
                      _soc_katana_mmu_sub_block_type_1_info_t * sub_block_info)
{
    uint32  stat_rval, en_rval;
    uint32 intr_bit;
    int sb_index = 0;
    _soc_katana_mmu_sub_block_type_1_info_t *sub_block;
    char prefix_str[10];
    char *reg_str;
    char *field_str;
    soc_port_t port = 0;
    
    sal_sprintf(prefix_str, "unit %d", unit);
    port = SOC_BLOCK_PORT(unit, block);
    SOC_IF_ERROR_RETURN(READ_CI_ERRORr(unit, port, &stat_rval));

    SOC_IF_ERROR_RETURN(READ_CI_ERROR_MASKr(unit, port, &en_rval));
    stat_rval &= (~en_rval);
    if (stat_rval == 0) {
        /* How did this interrupt get asserted then??? */
        return SOC_E_NONE; 
    }
    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            return SOC_E_NONE;
        }
        if (stat_rval & intr_bit) {
             field_str = SOC_FIELD_NAME(unit, sub_block->sub_block_field);
             reg_str = SOC_REG_NAME(unit, stat_reg);
             soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                                SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                stat_reg, sub_block->sub_block_field);
             LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "%s MMU subblock %x   reg %s field %s (value = 0x%x) has \
                                   MMU parity error\n"),
                        prefix_str, sub_block_type, reg_str, 
                        field_str, stat_rval));
             /* clear the parity error bit */
             SOC_IF_ERROR_RETURN(WRITE_CI_ERRORr(unit, port, intr_bit));
        }
    }
    return SOC_E_NONE;
}

STATIC
int _soc_katana_process_sb_type_2_mmu_parity_error(int unit, 
                      _soc_katana_mmu_subblock_type_t sub_block_type, 
                      soc_reg_t  stat_reg, soc_reg_t  en_reg,
                      _soc_katana_mmu_sub_block_type_2_info_t * sub_block_info)
{
    uint32 addr,  stat_rval, en_rval;
    uint32 intr_bit;
    int sb_index = 0;
    _soc_katana_mmu_sub_block_type_2_info_t *sub_block;
    char prefix_str[10];
    char *reg_str;
    char *field_str;

    sal_sprintf(prefix_str, "unit %d", unit);
    addr = soc_reg_addr(unit, stat_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &stat_rval));

    addr = soc_reg_addr(unit, en_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));
    if (stat_reg == INTFI_ECC_STATUSr) { 
        stat_rval &= ((~en_rval) >> 3);
    } else if (stat_reg == RQE_EXTQ_REPLICATION_COUNTr) {
        stat_rval &= ((~en_rval) << 11);
    }
    if (stat_rval == 0) {
        /* How did this interrupt get asserted then??? */
        return SOC_E_NONE; 
    }
    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            return SOC_E_NONE;
        }
        if (stat_rval & intr_bit) {
             field_str = SOC_FIELD_NAME(unit, sub_block->sub_block_field);
             reg_str = SOC_REG_NAME(unit, stat_reg);
             soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                                SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                stat_reg, sub_block->sub_block_field);
             LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "%s MMU subblock %x   reg %s field %s (value=0x%x) has \
                                   MMU parity error\n"),
                        prefix_str, sub_block_type, reg_str, 
                        field_str, stat_rval));
             /* clear the parity error bit */
             addr = soc_reg_addr(unit, stat_reg, REG_PORT_ANY, 0);
             SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, intr_bit));
        }
    }
    return SOC_E_NONE;
}

STATIC
int _soc_katana_process_sb_mmu_lvl1_parity_error(int unit, 
                      soc_reg_t  stat_reg, soc_reg_t  en_reg,
                      _soc_katana_mmu_sub_block_type_3_info_t * sub_block_info)
{
    uint32 addr,  stat_rval, en_rval, lvl1_stat_rval, lvl1_en_rval ;
    uint32 intr_bit;
    int sb_index = 0;
    _soc_katana_mmu_sub_block_type_3_info_t *sub_block;

    addr = soc_reg_addr(unit, stat_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &stat_rval));

    if (en_reg != 0) {
        addr = soc_reg_addr(unit, en_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));
        stat_rval &= (~en_rval);
    }
    if (stat_rval == 0) {
        /* How did this interrupt get asserted then??? */
        return SOC_E_NONE; 
    }

    for (sb_index = 0; ; sb_index++) {
        sub_block = &sub_block_info[sb_index];
        intr_bit = sub_block->intr_bit;
        if(intr_bit == 0) {
            /* End of table */
            return SOC_E_NONE;
        }
        addr = soc_reg_addr(unit, sub_block->sub_block_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &lvl1_stat_rval));

        if (sub_block->sub_block_en_reg != 0) {
            addr = soc_reg_addr(unit, sub_block->sub_block_en_reg,
                            REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &lvl1_en_rval));
            lvl1_stat_rval &= (~lvl1_en_rval);
        }
        if (lvl1_stat_rval == 0) {
            /* How did this interrupt get asserted then??? */
            return SOC_E_NONE; 
        }
        if (lvl1_stat_rval & intr_bit) {
             switch (sub_block->lvl2_sub_block) {
                case _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_2:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_EMC, 
                                               EMC_ERROR_2r, EMC_ERROR_MASK_2r,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_1:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                               _SOC_KT_MMU_SUBBLOCK_EMC, 
                                               EMC_ERROR_1r, EMC_ERROR_MASK_1r,
                                               sub_block->info);
                    break;   
                case _SOC_KT_MMU_LVL2_SUBBLOCK_EMC_ERROR_0:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                               _SOC_KT_MMU_SUBBLOCK_EMC, 
                                               EMC_ERROR_0r, EMC_ERROR_MASK_0r,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_3:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_DEQ, 
                                               DEQ_ERROR_3r, DEQ_ERROR_MASK_3r,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_2:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_DEQ, 
                                               DEQ_ERROR_2r, DEQ_ERROR_MASK_2r,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_1:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_DEQ, 
                                               DEQ_ERROR_1r, DEQ_ERROR_MASK_1r,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_DEQ_ERROR_0:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_DEQ, 
                                               DEQ_ERROR_0r, DEQ_ERROR_MASK_0r,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_U:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_QSTRUCT, 
                                               QSTRUCT_QENTRY_UPPER_ERRORr,
                                               QSTRUCT_QENTRY_UPPER_ERROR_MASKr,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_L:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_QSTRUCT, 
                                               QSTRUCT_QENTRY_LOWER_ERRORr,
                                               QSTRUCT_QENTRY_LOWER_ERROR_MASKr,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QBLOCK:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_QSTRUCT, 
                                               QSTRUCT_QBLOCK_NEXT_ERRORr,
                                               QSTRUCT_QBLOCK_NEXT_ERROR_MASKr,
                                               sub_block->info);
                    break;   

                case _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_FAP:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit,
                                               _SOC_KT_MMU_SUBBLOCK_QSTRUCT, 
                                               QSTRUCT_FAP_MEM_ERRORr,
                                               QSTRUCT_FAP_MEM_ERROR_MASKr,
                                               sub_block->info);
                    break;   

                default:
                    break;
             }
             /* stat_reg and sub_block->sub_block_reg is the same register */
             /* EMC_ERRORr and DEQ_ERRORr are read only, cannot be cleared */
             if (sub_block->lvl2_sub_block == _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_U ||
                 sub_block->lvl2_sub_block == _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QENTRY_L ||
                 sub_block->lvl2_sub_block == _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_QBLOCK ||
                 sub_block->lvl2_sub_block == _SOC_KT_MMU_LVL2_SUBBLOCK_QSTRUCT_FAP) {
                 /* clear the Level1 error bit */
                 addr = soc_reg_addr(unit, sub_block->sub_block_reg, REG_PORT_ANY, 0);
                 SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, intr_bit));
             }
            
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_process_mmu(int unit, soc_reg_t mmu_en_reg, 
                               soc_reg_t mmu_stat_reg, char *prefix_str)
{
    uint32 addr, en_rval, stat_rval;
    int sub_block_index;
    uint32 mmu_bit;
    const _soc_katana_mmu_sub_block_type_1_t *sub_block;
    const _soc_katana_mmu_sub_block_type_2_t *sub_block_type2;
    const _soc_katana_mmu_sub_block_type_3_t *lvl1_sub_block;
    _soc_katana_mmu_subblock_type_t sub_block_type;

    addr = soc_reg_addr(unit, mmu_en_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &en_rval));

    addr = soc_reg_addr(unit, mmu_stat_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &stat_rval));

    stat_rval &= (~en_rval);

    if (stat_rval == 0) {
        /* How did this interrupt get asserted then??? */
        return SOC_E_NONE;
    }

    for (sub_block_index = 0; ; sub_block_index++) {
        sub_block = &_soc_katana_mmu_sub_blocks_type_1[sub_block_index];
        mmu_bit = sub_block->mmu_bit;
        sub_block_type = sub_block->mmu_sub_block;
        if(mmu_bit == 0) {
            /* End of table */
            break;
        }
        if ((mmu_bit & (stat_rval)) == 0) {
            /* No interrupts in this sub-block */
            continue;
        }
        switch (sub_block_type) {

            case _SOC_KT_MMU_SUBBLOCK_MEM1:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  CLINK_ERRORr, 
                                                  CLINK_ERROR_MASKr,  
                                                  sub_block->info);
                break;

            case _SOC_KT_MMU_SUBBLOCK_E2EFC:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  MMU_E2EFC_ERROR_0r, 
                                                  MMU_E2EFC_ERROR_0_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_THDI:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  MMU_THDI_INTRr, 
                                                  MMU_THDI_INTR_MASKr,  
                                                  sub_block->info);
                break;   

   
            case _SOC_KT_MMU_SUBBLOCK_CTR:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  CTR_ERRORr, 
                                                  CTR_ERROR_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_CCP:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  CCP_ERRORr, 
                                                  CCP_ERROR_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_TOQ:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  TOQ_ERRORr, 
                                                  TOQ_ERROR_MASKr,  
                                                  sub_block->info);
                break;   


            case _SOC_KT_MMU_SUBBLOCK_CFAPI:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  CFAPI_ECC_ERRORr, 
                                                  CFAPI_ERROR_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_CFAPE:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  CFAPE_ECC_ERRORr, 
                                                  CFAPE_ERROR_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ITE:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  MMU_ITE_ERROR_0r, 
                                                  MMU_ITE_ERROR_0_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ITE_CFG:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  MMU_ITE_CFG_ECC_ERROR_0r, 
                                                  MMU_ITE_CFG_ECC_ERROR_0_MASKr,
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ENQ_CFG:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  MMU_ENQ_CFG_ECC_ERROR_0r, 
                                                  MMU_ENQ_CFG_ECC_ERROR_0_MASKr,
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ENQ:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  MMU_ENQ_ERROR_0r, 
                                                  MMU_ENQ_ERROR_0_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_ENQ_FAP:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  MMU_ENQ_FAP_ECC_ERROR_0r, 
                                                  MMU_ENQ_FAP_ECC_ERROR_0_MASKr,
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_THDO_STATUS1:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  THDO_PARITY_ERROR_STATUS1r, 
                                                  THDO_PARITY_ERROR_MASK1r,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_THDO_STATUS2:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  THDO_PARITY_ERROR_STATUS2r, 
                                                  THDO_PARITY_ERROR_MASK2r,  
                                                  sub_block->info);
                break;   


            case _SOC_KT_MMU_SUBBLOCK_AGING_INT:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  AGING_ERROR_INTr, 
                                                  AGING_ERROR_MASK_INTr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_AGING_EXT:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  AGING_ERROR_EXTr, 
                                                  AGING_ERROR_MASK_EXTr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_WRED:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                             sub_block_type, 
                                             WRED_PARITY_ERROR_BITMAPr, 
                                             WRED_PARITY_ERROR_MASKr, 
                                             sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_INTFI:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  INTFI_ERROR_STATUSr, 
                                                  INTFI_ERROR_MASKr,  
                                                  sub_block->info);
                break;   


            case _SOC_KT_MMU_SUBBLOCK_CI2:
        /* Access to CI_ERROR and CI_ERROR_MASK register is resulting is S-Chan 
           timeout on some KATANA devices which do not have ddr3 memory  */
                if (soc_feature(unit, soc_feature_ddr3)) {
                    _soc_katana_process_sb_type_1_ci_mmu_parity_error(unit, 
                                                  CI2_BLOCK(unit),
                                                  sub_block_type, 
                                                  CI_ERRORr, 
                                                  CI_ERROR_MASKr,  
                                                  sub_block->info);
                }
                break;   
            case _SOC_KT_MMU_SUBBLOCK_CI1:
                if (soc_feature(unit, soc_feature_ddr3)) {
                    _soc_katana_process_sb_type_1_ci_mmu_parity_error(unit, 
                                                  CI1_BLOCK(unit),
                                                  sub_block_type, 
                                                  CI_ERRORr, 
                                                  CI_ERROR_MASKr,  
                                                  sub_block->info);
                }
                break;   
            case _SOC_KT_MMU_SUBBLOCK_CI0:
                if (soc_feature(unit, soc_feature_ddr3)) {
                    _soc_katana_process_sb_type_1_ci_mmu_parity_error(unit, 
                                                  CI0_BLOCK(unit),
                                                  sub_block_type, 
                                                  CI_ERRORr, 
                                                  CI_ERROR_MASKr,  
                                                  sub_block->info);
                }
                break;   

            case _SOC_KT_MMU_SUBBLOCK_LLS:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  LLS_ERRORr, 
                                                  LLS_ERROR_MASKr,  
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_PORT:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  LLS_PORT_ECC_ERRORr, 
                                                  LLS_PORT_ECC_ERROR_MASKr,  
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_L0_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  LLS_L0_ECC_ERROR1r, 
                                                  LLS_L0_ECC_ERROR1_MASKr,  
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_UPD2:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  LLS_ERROR_UPD2r, 
                                                  LLS_ERROR_UPD2_MASKr,  
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_L1_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  LLS_L1_ECC_ERROR1r, 
                                                  LLS_L1_ECC_ERROR1_MASKr,  
                                                  sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_LLS_L2_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  LLS_L2_ECC_ERROR1r, 
                                                  LLS_L2_ECC_ERROR1_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_LLS_MISC_ECC:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  LLS_MISC_ECC_ERROR1r, 
                                                  LLS_MISC_ECC_ERROR1_MASKr,  
                                                  sub_block->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_RQE_SER:
                    _soc_katana_process_sb_type_1_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  RQE_SER_STATUSr, 
                                                  RQE_SER_MASKr,  
                                                  sub_block->info);
                break;   

            default:
                break;   

        }
        /* clear the MMU parity error bit */
        addr = soc_reg_addr(unit, mmu_stat_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, mmu_bit));
    }

    for (sub_block_index = 0; ; sub_block_index++) {
        sub_block_type2 = &_soc_katana_mmu_sub_blocks_type_2[sub_block_index];
        mmu_bit = sub_block_type2->mmu_bit;
        sub_block_type = sub_block_type2->mmu_sub_block;
        if(mmu_bit == 0) {
            /* End of table */
            break;
        }
        if ((mmu_bit & stat_rval) == 0) {
            /* No interrupts in this sub-block */
            continue;
        }
        switch (sub_block_type) {
            case _SOC_KT_MMU_SUBBLOCK_RQE_EXTQ:
                    _soc_katana_process_sb_type_2_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  RQE_EXTQ_REPLICATION_COUNTr, 
                                                  RQE_EXTQ_REPLICATION_LIMITr,  
                                                  sub_block_type2->info);
                break;   

            case _SOC_KT_MMU_SUBBLOCK_INTFI_ECC:
                    _soc_katana_process_sb_type_2_mmu_parity_error(unit, 
                                                  sub_block_type, 
                                                  INTFI_ECC_STATUSr, 
                                                  INTFI_ERROR_MASKr,  
                                                  sub_block_type2->info);
                break;   

            default:
                break;   

        }
        /* clear the MMU parity error bit */
        addr = soc_reg_addr(unit, mmu_stat_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, mmu_bit));
    }

    for (sub_block_index = 0; ; sub_block_index++) {
        lvl1_sub_block = &_soc_katana_mmu_sub_blocks_type_3[sub_block_index];
        mmu_bit = lvl1_sub_block->mmu_bit;
        sub_block_type = lvl1_sub_block->mmu_sub_block;
        if(mmu_bit == 0) {
            /* End of table */
            break;
        }
        if ((mmu_bit & stat_rval) == 0) {
            /* No interrupts in this sub-block */
            continue;
        }

        switch (sub_block_type) {
            case _SOC_KT_MMU_SUBBLOCK_EMC:
        /* Access to EMC_ERROR register is resulting is S-Chan timeout on some
           KATANA devices which do not have ddr3 memory  */
                if (soc_feature(unit, soc_feature_ddr3)) {
                    _soc_katana_process_sb_mmu_lvl1_parity_error(unit, 
                                                  EMC_ERRORr, 0,
                                                  lvl1_sub_block->info);
                }
                break;   
            case _SOC_KT_MMU_SUBBLOCK_DEQ:
                    _soc_katana_process_sb_mmu_lvl1_parity_error(unit, 
                                                  DEQ_ERRORr, 0,
                                                  lvl1_sub_block->info);
                break;   
            case _SOC_KT_MMU_SUBBLOCK_QSTRUCT:
                    _soc_katana_process_sb_mmu_lvl1_parity_error(unit, 
                                                  QSTRUCT_INTERRUPTr, 
                                                  QSTRUCT_INTERRUPT_MASKr, 
                                                  lvl1_sub_block->info);
                break;   

            default:
                break;   

        }
        /* clear the MMU parity error bit */
        addr = soc_reg_addr(unit, mmu_stat_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, mmu_bit));
    }
    return SOC_E_NONE;
}




STATIC int
_soc_katana_parity_process_mxqport(int unit, uint8 id,
                                   const _soc_katana_parity_info_t *info,
                                   soc_block_t block)
{
    soc_reg_t mxqport_reg;
    uint32 addr, mxqport_rval;
    int blk, port;
    char prefix_str[24];
    uint32 handled_status = 0;

    if (info->intr_status_reg == INVALIDr) {
        return SOC_E_NONE;
    }

    port = -1;
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
        if (SOC_BLOCK_INFO(unit, blk).number == id) {
            port = SOC_BLOCK_PORT(unit, blk);
            break;
        }
    }
    if (port < 0) {
        return SOC_E_NONE;
    }

    mxqport_reg = info->intr_status_reg;
    addr = soc_reg_addr(unit, mxqport_reg, port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &mxqport_rval));
    if (mxqport_rval == 0) {
        return SOC_E_NONE;
    }

    sal_sprintf(prefix_str, "unit %d MXQPORT%d", unit, id);
    SOC_IF_ERROR_RETURN
        (_soc_katana_parity_process_info(unit, id, mxqport_reg, mxqport_rval,
                                          &handled_status, info->info, prefix_str,
                                           block));

    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_process_info(int unit, uint8 id,
                                 soc_reg_t group_reg,
                                 uint32 group_rval,
                                 uint32 *handled_group_rval,
                                 const _soc_katana_parity_info_t *info_list,
                                 char *prefix_str,
                                 soc_block_t block)
{
    const _soc_katana_parity_info_t *info;
    int info_index;
    char *mem_str;

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }

        /* Check status for the info entry in the group register */
        if (!soc_reg_field_get(unit, group_reg, group_rval,
                               info->group_reg_status_field)) {
            continue;
        } else {
            /* Mark this as handled */
            soc_reg_field_set(unit, group_reg, handled_group_rval,
                               info->group_reg_status_field, 1);
        }

        if (info->mem_str) {
            mem_str = info->mem_str;
        } else if (info->mem != INVALIDm) {
            mem_str = SOC_MEM_NAME(unit, info->mem);
        } else {
            mem_str = SOC_FIELD_NAME(unit, info->group_reg_status_field);
        }

        switch (info->type) {
        case _SOC_PARITY_TYPE_GENERIC:
        case _SOC_PARITY_TYPE_PARITY:
        case _SOC_PARITY_TYPE_HASH:
        case _SOC_PARITY_TYPE_COUNTER:
        case _SOC_PARITY_TYPE_SER:
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                               info->mem, info->group_reg_status_field);
            break;
        case _SOC_PARITY_TYPE_ECC:
        case _SOC_PARITY_TYPE_EDATABUF:
        case _SOC_PARITY_TYPE_MXQPORT:
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                               info->mem, info->group_reg_status_field);
            break;
        default:
            break;
        }

        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_PARITY_TYPE_GENERIC:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s asserted\n"), prefix_str, mem_str));
            break;
        case _SOC_PARITY_TYPE_PARITY:
            /* PARITY_ERRf, MULTIPLE_ERRf, ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_parity(unit, info, FALSE,
                                                   prefix_str, mem_str, block));
            break;
        case _SOC_PARITY_TYPE_ECC:
            /* ECC_ERRf, MULTIPLE_ERRf, DOUBLE_BIT_ERRf, ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_ecc(unit, info, FALSE,
                                                 prefix_str, mem_str, block));
            break;
        case _SOC_PARITY_TYPE_HASH:
            /* PARITY_ERR_BMf, MULTIPLE_ERRf, BUCKET_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_hash(unit, info, FALSE,
                                                  prefix_str, mem_str, block));
            break;
        case _SOC_PARITY_TYPE_EDATABUF:
            /* ECC_ERR_MGRPf, ECC_ERR_2B_MGRPf, ECC_MULTI_MGRPf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_edatabuf(unit, info, FALSE,
                                                      prefix_str, mem_str));
            break;
        case _SOC_PARITY_TYPE_COUNTER:
            /* PARITY_ERRf, MULTIPLE_ERRf, COUNTER_IDXf, PORT_IDX,
               ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_counter(unit, info, FALSE,
                                                   prefix_str, mem_str, block));
            break;
        case _SOC_PARITY_TYPE_MXQPORT:
            /* One more level of report tree structure */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_mxqport(unit, id, info, block));
            break;
        default:
            break;
        } /* Handle different parity error reporting style */
    } /* Loop through each info entry in the list */

    return SOC_E_NONE;
}

STATIC int
_soc_katana_parity_process_all(int unit)
{
    const _soc_katana_parity_route_block_t *route_block;
    int route_block_index;
    soc_reg_t route_block_en_reg, route_block_reg;
    uint32 cmic_rval, route_block_rval, rval;
    uint32 addr, cmic_bit;
    char prefix_str[10];

    sal_sprintf(prefix_str, "unit %d", unit);

    /* Read CMIC parity status register */
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN
        (READ_CMIC_CMC0_IRQ_STAT2r(unit, &cmic_rval));
    if (cmic_rval == 0) {
        return SOC_E_NONE;
    }

    /* Loop through each place-and-route block entry */
    for (route_block_index = 0; ; route_block_index++) {
        route_block = &_soc_katana_parity_route_blocks[route_block_index];
        cmic_bit = route_block->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }

        /* Check status for the route block in the CMIC register */
        if (!(cmic_rval & cmic_bit)) {
            /* No interrupt bit asserted for the route block */
            continue;
        }

        /* Read per route block parity status register */
        route_block_reg = route_block->status_reg;
        route_block_en_reg = route_block->enable_reg;
        addr = soc_reg_addr(unit, route_block_reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &route_block_rval));
        if (route_block_rval == 0) {
            continue;
        }
        if (route_block->blocktype == SOC_BLK_MMU) {
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_mmu(unit, route_block_en_reg, 
                                                route_block_reg, prefix_str));
        } else {
            uint32 handled_route_block_rval = 0;
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_info(unit,
                                                  route_block->id,
                                                  route_block_reg,
                                                  route_block_rval,
                                                  &handled_route_block_rval,
                                                  route_block->info,
                                                  prefix_str,
                                                  route_block->blocktype));
            /* Get the current route block status value */
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr,
                                               &route_block_rval));
            /* Reset the status bits we just handled as a part of parity correction */
            route_block_rval &= ~handled_route_block_rval;
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, route_block_rval));
        }
    } /* Loop through each place-and-route block entry */
    /* Handle OAM interrupt */
    if (READ_IP1_INTR_STATUS_1r(unit, &rval)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error reading %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_STATUS_1r)));
        return SOC_E_INTERNAL;
    }
    /* Handle OAM interrupt */
    if (rval & KT_OAM_INTR_MASK) {
        SOC_IF_ERROR_RETURN(soc_kt_oam_interrupt_process(unit));
    }

    return SOC_E_NONE;
}

void
soc_katana_parity_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    (void)_soc_katana_parity_process_all(unit);
    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
    if (d2 != NULL) {
        (void)soc_cmicm_intr2_enable(unit, PTR_TO_INT(d2));
    }
}

void
soc_katana_oam_event_process(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    uint32 rval;
    /* Handle OAM interrupt */
    if (READ_IP1_INTR_STATUS_1r(unit, &rval) == SOC_E_NONE) {
        /* Handle OAM interrupt */
        if (rval & KT_OAM_INTR_MASK) {
            (void) soc_kt_oam_interrupt_process(unit);
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error reading %s reg !!\n"),
                              unit, SOC_REG_NAME(unit, IP1_INTR_STATUS_1r)));
    }

    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
    if (d2 != NULL) {
        (void)soc_cmicm_intr2_enable(unit, PTR_TO_INT(d2));
    }
}


STATIC int
_soc_katana_mem_nack_process_info(int unit, int reg_mem,
                                   _kt_ser_nack_reg_mem_t nack_reg_mem,
                                   soc_block_t block,
                                   const _soc_katana_parity_info_t *info_list,
                                   char *prefix_str)
{
    const _soc_katana_parity_info_t *info;
    int info_index;
    char *mem_str;

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }
        if (reg_mem == _SOC_KT_SER_MEM && info->mem != nack_reg_mem.mem) {
            continue;
        }
        if (info->mem_str) {
            mem_str = info->mem_str;
        } else {
            mem_str = SOC_MEM_NAME(unit, info->mem);
        }

        if (reg_mem == _SOC_KT_SER_REG && info->type != _SOC_PARITY_TYPE_COUNTER) {
            continue;
        }

        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_PARITY_TYPE_PARITY:
            /* PARITY_ERRf, MULTIPLE_ERRf, ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_parity(unit, info, TRUE,
                                                    prefix_str, mem_str, block));
            break;
        case _SOC_PARITY_TYPE_ECC:
            /* ECC_ERRf, MULTIPLE_ERRf, DOUBLE_BIT_ERRf, ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_ecc(unit, info, TRUE,
                                                 prefix_str, mem_str, block));
            break;
        case _SOC_PARITY_TYPE_HASH:
            /* PARITY_ERR_BMf, MULTIPLE_ERRf, BUCKET_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_hash(unit, info, TRUE,
                                                  prefix_str, mem_str, block));
            break;
        case _SOC_PARITY_TYPE_EDATABUF:
            /* ECC_ERR_MGRPf, ECC_ERR_2B_MGRPf, ECC_MULTI_MGRPf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_edatabuf(unit, info, TRUE,
                                                      prefix_str, mem_str));
            break;
        case _SOC_PARITY_TYPE_COUNTER:
            /* PARITY_ERRf, MULTIPLE_ERRf, COUNTER_IDXf, PORT_IDX,
               ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_katana_parity_process_counter(unit, info, TRUE,
                                                     prefix_str, mem_str, block));
            break;
        default:
            break;
        } /* Handle different parity error reporting style */
    } /* Loop through each info entry in the list */

    return SOC_E_NONE;
}


STATIC int
_soc_katana_mem_nack_error_process(int unit, 
                                   _kt_ser_nack_reg_mem_t nack_reg_mem, 
                                   soc_block_t block, 
                                   int reg_mem)
{
    const _soc_katana_parity_route_block_t *route_block;
    int route_block_index;
    uint32 cmic_bit;
    char prefix_str[10];

    sal_sprintf(prefix_str, "unit %d", unit);

    /* Loop through each place-and-route block entry */
    for (route_block_index = 0; ; route_block_index++) {
        route_block = &_soc_katana_parity_route_blocks[route_block_index];
        cmic_bit = route_block->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }

        if (route_block->info == NULL) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (_soc_katana_mem_nack_process_info(unit, 
                                                reg_mem, nack_reg_mem, block,
                                                route_block->info,
                                                prefix_str));
    } /* Loop through each place-and-route block entry */

    return SOC_E_NONE;
}

void
soc_katana_stat_nack(int unit, int *fixed)
{
    int rv;
    _kt_ser_nack_reg_mem_t nack_reg_mem;

    nack_reg_mem.mem = -1;
    nack_reg_mem.reg = -1;
    _stat_error_fixed[unit] = 0;

    if ((rv = _soc_katana_mem_nack_error_process(unit, nack_reg_mem, 0, 
        _SOC_KT_SER_REG)) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d STAT SCHAN NACK analysis failure.\n"), unit));
    }
    *fixed = _stat_error_fixed[unit];
}

void
soc_katana_mem_nack(void *unit_vp, void *addr_vp, void *blk_vp, 
                     void *d3, void *d4)
{
    soc_mem_t mem = INVALIDm;
    int reg_mem = PTR_TO_INT(d3);
    int rv, unit = PTR_TO_INT(unit_vp);
    uint32 address = PTR_TO_INT(addr_vp);
    uint32 block = PTR_TO_INT(blk_vp);
    uint32 offset = 0, min_addr = 0, max_addr = 0;
    soc_regaddrinfo_t ainfo;
    _kt_ser_nack_reg_mem_t nack_reg_mem;
    
    nack_reg_mem.reg = INVALIDr;
    nack_reg_mem.mem = INVALIDm;

    if (reg_mem == _SOC_KT_SER_REG) {
        if (address) {
            soc_regaddrinfo_get(unit, &ainfo, address);
            nack_reg_mem.reg = ainfo.reg;
        }
    } else {
        offset = address & ~0xC0f00000; /* Strip block ID */
        mem = soc_addr_to_mem(unit, address, &block);
        if (mem == INVALIDm) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d mem decode failed, "
                                  "SCHAN NACK analysis failure\n"), unit));
            return;
        }
        
        _soc_katana_mem_rename(&mem);
        nack_reg_mem.mem = mem;

        min_addr = max_addr = SOC_MEM_INFO(unit, mem).base;
        min_addr += SOC_MEM_INFO(unit, mem).index_min;
        max_addr += SOC_MEM_INFO(unit, mem).index_max;
    }

    if ((rv = _soc_katana_mem_nack_error_process(unit, nack_reg_mem, block,
        reg_mem)) < 0) {
        if (reg_mem == _SOC_KT_SER_REG) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d REG SCHAN NACK analysis failure\n"),
                       unit));
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s entry %d SCHAN NACK analysis failure\n"),
                       unit, SOC_MEM_NAME(unit, mem),
                       min_addr - offset));
        }
    }
    if (reg_mem == _SOC_KT_SER_REG) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d REG SCHAN NACK analysis\n"),
                       unit));
    } else {
        LOG_INFO(BSL_LS_SOC_SCHAN,
                 (BSL_META_U(unit,
                             "unit %d %s entry %d SCHAN NACK analysis\n"),
                  unit, SOC_MEM_NAME(unit, mem),
                  min_addr - offset));
    }
}

/* SER processing for TCAMs */
STATIC _soc_ser_parity_info_t _soc_kt_ser_parity_info_template[] = {
    { EFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_0r, CMIC_SER_END_ADDR_0r,
      CMIC_SER_MEM_ADDR_0r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_0_PARITY_BITSf, 430, CMIC_SER_RANGE0_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ},
    { VFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_1r, CMIC_SER_END_ADDR_1r,
      CMIC_SER_MEM_ADDR_1r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_1_PARITY_BITSf, 470, CMIC_SER_RANGE1_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ},
    { FP_UDF_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_2r, CMIC_SER_END_ADDR_2r,
      CMIC_SER_MEM_ADDR_2r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_2_PARITY_BITSf, 139, CMIC_SER_RANGE2_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ},
    { CPU_COS_MAPm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_3r, CMIC_SER_END_ADDR_3r,
      CMIC_SER_MEM_ADDR_3r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_3_PARITY_BITSf, 288, CMIC_SER_RANGE3_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ},
    { L3_TUNNELm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_4r, CMIC_SER_END_ADDR_4r,
      CMIC_SER_MEM_ADDR_4r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_4_PARITY_BITSf, 211, CMIC_SER_RANGE4_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ},
    { VLAN_SUBNETm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_5r, CMIC_SER_END_ADDR_5r,
      CMIC_SER_MEM_ADDR_5r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_5_PARITY_BITSf, 129, CMIC_SER_RANGE5_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ},
    { MY_STATION_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_6r, CMIC_SER_END_ADDR_6r,
      CMIC_SER_MEM_ADDR_6r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_6_PARITY_BITSf, 161, CMIC_SER_RANGE6_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ},
    { L3_DEFIPm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_7r, CMIC_SER_END_ADDR_7r,
      CMIC_SER_MEM_ADDR_7r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_7_PARITY_BITSf, 190, CMIC_SER_RANGE7_DATAENTRY_LENr, 0, 0, 0, 
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SIZE_VERIFY | 
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_REMAP_READ},
    { L3_DEFIP_PAIR_128m, _SOC_SER_PARITY_MODE_2BITS, 
      CMIC_SER_START_ADDR_8r, CMIC_SER_END_ADDR_8r,
      CMIC_SER_MEM_ADDR_8r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_8_PARITY_BITSf, 380, CMIC_SER_RANGE8_DATAENTRY_LENr, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SIZE_VERIFY | 
      _SOC_SER_FLAG_NO_DMA | _SOC_SER_FLAG_REMAP_READ},
    { L2_USER_ENTRYm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_9r, CMIC_SER_END_ADDR_9r,
      CMIC_SER_MEM_ADDR_9r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_9_PARITY_BITSf, 123, CMIC_SER_RANGE9_DATAENTRY_LENr, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { FP_GLOBAL_MASK_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_10r, CMIC_SER_END_ADDR_10r,
      CMIC_SER_MEM_ADDR_10r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_10_PARITY_BITSf, 81, CMIC_SER_RANGE10_DATAENTRY_LENr, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { FP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
      CMIC_SER_START_ADDR_11r, CMIC_SER_END_ADDR_11r,
      CMIC_SER_MEM_ADDR_11r, CMIC_SER_PARITY_MODE_SEL_15_0r,
      RANGE_11_PARITY_BITSf, 470, CMIC_SER_RANGE11_DATAENTRY_LENr, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { INVALIDm, _SOC_SER_PARITY_MODE_NUM},
};

static _soc_ser_parity_info_t *_soc_kt_ser_parity_info[SOC_MAX_NUM_DEVICES];

#define SOC_KT_SER_MEM_AVAILABLE       (2048 * 32)  /* bits */

STATIC int
_soc_katana_ser_init(int unit)
{
    int alloc_size;
    
    /* First, make per-unit copy of the master TCAM list */
    alloc_size = sizeof(_soc_kt_ser_parity_info_template);
    if (NULL == _soc_kt_ser_parity_info[unit]) {
        if ((_soc_kt_ser_parity_info[unit] =
             sal_alloc(alloc_size, "kt tcam list")) == NULL) {
            return SOC_E_MEMORY;
        }
    }
    /* Make a fresh copy of the TCAM template info */
    sal_memcpy(_soc_kt_ser_parity_info[unit],
               &(_soc_kt_ser_parity_info_template),
               alloc_size);
    return soc_ser_init(unit, _soc_kt_ser_parity_info[unit],
                        SOC_KT_SER_MEM_AVAILABLE);
}

int
soc_katana_ser_mem_clear(int unit, soc_mem_t mem)
{
    return soc_ser_mem_clear(unit, _soc_kt_ser_parity_info[unit], mem);
}

void
soc_katana_ser_fail(int unit)

{
    soc_process_ser_parity_error(unit, _soc_kt_ser_parity_info[unit],
                                 _SOC_PARITY_TYPE_SER);
    return;
}

int
soc_katana_pipe_mem_clear(int unit)
{
    uint32        rval;
    int           index, pipe_init_usec;
    soc_timeout_t to;
    static const soc_mem_t cam[] = {
        EFP_TCAMm,
        VFP_TCAMm,
        FP_UDF_TCAMm,
        CPU_COS_MAPm,
        L3_TUNNELm,
        VLAN_SUBNETm,
        MY_STATION_TCAMm,
        L3_DEFIPm,
        L3_DEFIP_PAIR_128m,
        L2_USER_ENTRYm,
        FP_GLOBAL_MASK_TCAMm,
        FP_TCAMm
    };

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table (L2X) */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 32768);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table (EGR_VLAN_XLATE) */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 16384);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 0);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, CMIC_REQ_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    for (index = 0; index < sizeof(cam) / sizeof(soc_mem_t); index++) {
        SOC_IF_ERROR_RETURN(soc_mem_clear(unit, cam[index], COPYNO_ALL, TRUE));
    }
    return SOC_E_NONE;
}

/*
 * cpu port (mmu port 0): 48 queues (0-47)
 * loopback port (mmu port 35): 8 queues (0-7)
 * hg ports : 24 queues
 * other ports : 8 queues, vlan levels : 64                                    
 */

int
soc_katana_num_cosq_init(int unit)
{
    soc_info_t *si;
    int port, prev_port, vlan_queue_enable = 0;
    int num_base_queues = 0;

    si = &SOC_INFO(unit);
    prev_port = 0;
    
    PBMP_ALL_ITER(unit, port) { 

        si->port_num_ext_cosq[port] = 0;
        si->port_num_cosq[port] = 0;
        /* Supports max 8 ports for vlan queues */
        vlan_queue_enable =
                   soc_property_port_get(unit, port, spn_VLAN_QUEUE_ENABLE, 0);

        if (vlan_queue_enable != 0) {
            si->port_num_ext_cosq[port] = soc_property_get(unit,
                                            spn_VLAN_QUEUE_LEVELS_MAX, 32);
            if (si->port_num_ext_cosq[port] > 64) {
                si->port_num_ext_cosq[port] = 64;
            }
        }
        if (IS_CPU_PORT(unit, port)) {
            si->port_cosq_base[port] = 0;
            si->port_uc_cosq_base[port] = 0;
            si->port_num_uc_cosq[port] = 48;
            si->port_num_ext_cosq[port] = 0;
        } else {
            si->port_cosq_base[port] = si->port_cosq_base[prev_port] +
                                       si->port_num_uc_cosq[prev_port];
            si->port_uc_cosq_base[port] = si->port_cosq_base[port];
            if (IS_HG_PORT(unit, port)) {
                si->port_num_uc_cosq[port] = 
                                          (si->port_num_ext_cosq[port] < 24) ? \
                                           24 : si->port_num_ext_cosq[port];
            } else {
                si->port_num_uc_cosq[port] = 
                                          (si->port_num_ext_cosq[port] < 8) ? \
                                           8 : si->port_num_ext_cosq[port];
            }
        }
        prev_port = port;
        num_base_queues += si->port_num_uc_cosq[port];

        if (num_base_queues > KATANA_BASE_QUEUES_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Base queue allocation failed: num_queues %d \n"),
                       num_base_queues));
            return SOC_E_PARAM;
        }

        if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "\nport num %d cosq_base %d num_cosq %d"),
                     port, si->port_cosq_base[port],
                     si->port_num_uc_cosq[port]));
        }
    }
    si->num_mmu_base_cosq = num_base_queues;

    return SOC_E_NONE;
}

STATIC int
_soc_katana_shaper_rate_info_dealloc(int unit)
{
    int rv = SOC_E_NONE;

    if (shaper_info[unit] == NULL) {
        return(rv);
    }
    if (shaper_info[unit]->basic_rate_info != NULL) {
        sal_free(shaper_info[unit]->basic_rate_info);
    }
    if (shaper_info[unit]->burst_size_info != NULL) {
        sal_free(shaper_info[unit]->burst_size_info);
    }
    sal_free(shaper_info[unit]);

    return(rv);
}

STATIC int
_soc_katana_shaper_rate_info_alloc(int unit)
{
    int          rv = SOC_E_NONE;
    uint32       exponent, mantissa, denominator = 1;
    uint64       numerator;
    uint32       uRate = 0;
    int          i, temp;

    /* allocate shaper info */
    shaper_info[unit] = sal_alloc(sizeof(katanaShaperInfo_t),
                                                            "Katana_Shaper");
    if (shaper_info[unit] == NULL) {
        return(SOC_E_MEMORY);
    }
    sal_memset(shaper_info[unit], 0, sizeof(katanaShaperInfo_t));

    /* allocate rate info */
    shaper_info[unit]->basic_rate_info = sal_alloc(
                    sizeof(katanaRateInfo_t) * (KATANA_SHAPER_RATE_MAX_EXPONENT + 1),
                                                "Katana_Rate");
    if (shaper_info[unit]->basic_rate_info == NULL) {
        rv = SOC_E_MEMORY;
        goto err;
    }

    /* allocate burst info */
    shaper_info[unit]->burst_size_info = sal_alloc(
                    sizeof(katanaRateInfo_t) * (KATANA_SHAPER_BURST_MAX_EXPONENT + 1),
                                                 "Katana_Burst");
    if (shaper_info[unit]->burst_size_info == NULL) {
        rv = SOC_E_MEMORY;
        goto err;
    }

    /* initialize rate info */
    for (i = 0, mantissa = KATANA_SHAPER_RATE_MIN_MANTISSA;
                           mantissa <= KATANA_SHAPER_RATE_MIN_MANTISSA; mantissa++) {
        for (exponent = KATANA_SHAPER_RATE_MIN_EXPONENT;
                           exponent <= KATANA_SHAPER_RATE_MAX_EXPONENT; exponent++) {
	    if (exponent == 0) {
	        /* (2^r_e+4) * (r_m/1024) / ref_cycle based on 7.8125uS(128KHz) */
                COMPILER_64_ZERO(numerator); 
            } else {
	        /* (2^(r_e+3)) * (1+(r_m / 1024)) / ref_cycle 
                   r_m = 0 => (1 << exponent + 3 + 7) */ 

                COMPILER_64_SET(numerator, 0, 1);
                temp = exponent + 10 + 10;
                COMPILER_64_SHL(numerator, temp);
            }

            denominator = 1024;

	    if (soc_esw_div64(numerator, denominator, &uRate) == -1) {
	      rv = SOC_E_INTERNAL;
	      goto err;
	    }
            /* the rate is stored in kbps */
            shaper_info[unit]->basic_rate_info[i].rate     = uRate;
            shaper_info[unit]->basic_rate_info[i].exponent = exponent;
            shaper_info[unit]->basic_rate_info[i].mantissa = mantissa;
            i++;
        }
    }

    /* initialize burst info */
    for (i = 0, mantissa = KATANA_SHAPER_BURST_MIN_MANTISSA;
                           mantissa <= KATANA_SHAPER_BURST_MIN_MANTISSA; mantissa++) {
        for (exponent = KATANA_SHAPER_BURST_MIN_EXPONENT;
                           exponent <= KATANA_SHAPER_BURST_MAX_EXPONENT; exponent++) {

	    /* rate = 2^(t_e+9) * (1+(t_m/128)) */
            shaper_info[unit]->burst_size_info[i].rate = (1 << (exponent + 9));
            shaper_info[unit]->burst_size_info[i].exponent = exponent;
            shaper_info[unit]->burst_size_info[i].mantissa = mantissa;
            i++;
        }
    }

    return(rv);

err:
    _soc_katana_shaper_rate_info_dealloc(unit);
    return(rv);
}

STATIC int
_soc_katana_get_shaper_rate(int unit, uint32 rate, int *index,
                            uint32 *rate_mantissa, uint32 *rate_exponent)
{
    int     rv = SOC_E_NONE;
    int     i;
    int     delta_rate_diff = 0;
    uint32  last_mantissa = -1, last_exponent = -1, cur_mantissa = -1, cur_exponent;
    uint32  last_rate_diff = -1, cur_rate_diff;

    if (rate < 2) {
        *index = 0;
        /* set minimum granularity of 2kbps for non zero rate */
        *rate_mantissa = (rate != 0) ? 1 : 0;
        *rate_exponent = 0;
        return rv;
    }
    /* As basic rate is stored in 32-bit kbps, diff will be in 32-bit */ 
    for (i = 0; i < (KATANA_SHAPER_RATE_MAX_EXPONENT + 1); i++) {
        cur_exponent = shaper_info[unit]->basic_rate_info[i].exponent;
        if (cur_exponent == 0) {
            cur_mantissa = (rate - shaper_info[unit]->basic_rate_info[i].rate) / 
                               (1 << (cur_exponent + 1));
        } else {
            cur_mantissa = (rate - shaper_info[unit]->basic_rate_info[i].rate) / 
                               (1 << cur_exponent);
        }
        if (cur_mantissa > KATANA_SHAPER_RATE_MAX_MANTISSA) {
            continue;
        }
        
        if (cur_exponent == 0) {
            cur_rate_diff = rate - (shaper_info[unit]->basic_rate_info[i].rate + 
                               (cur_mantissa * (1 << (cur_exponent+1))));
        } else {
            cur_rate_diff = rate - (shaper_info[unit]->basic_rate_info[i].rate + 
                               (cur_mantissa * (1 << cur_exponent)));
        }    

        if (last_mantissa == -1) {
            last_mantissa = cur_mantissa;
            last_exponent = cur_exponent;
            last_rate_diff = cur_rate_diff;
        }
        else if (cur_rate_diff < last_rate_diff) {
            last_mantissa = cur_mantissa;
            last_exponent = cur_exponent;
            last_rate_diff = cur_rate_diff;
        }

        if (cur_rate_diff == 0) {
            break;
        }
    }
    if (last_mantissa == -1) {
        rv = SOC_E_PARAM;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d Ingress Shaper Rate not found\n"), unit));
        return(rv);
    }
    else {
        /* Compute the delta rate between requested rate and hw rate. */
        if (last_exponent == 0) {
            delta_rate_diff = rate - (shaper_info[unit]->
                                  basic_rate_info[last_exponent].rate +
                                   (last_mantissa * (1 << (last_exponent+1))));
        } else {
            delta_rate_diff = rate - (shaper_info[unit]->
                                  basic_rate_info[last_exponent].rate +
                                   (last_mantissa * (1 << last_exponent)));
        }

        /* If the delta rate is +ve then only adjust the rate to next higher slot */
        if (delta_rate_diff > 0) {
            if ((last_mantissa == KATANA_SHAPER_RATE_MAX_MANTISSA) &&
                (last_exponent != KATANA_SHAPER_RATE_MAX_EXPONENT)) {
                /* Mantissa is at its max, move to next EXPONENT */
                /* provided we are not at max rate already */
                last_mantissa = 0;
                last_exponent++;
            } else if (last_mantissa != KATANA_SHAPER_RATE_MAX_MANTISSA) {
                /* Move the mantissa to next higher slot */
                last_mantissa++;
            }
        }

        (*index) = i;
        (*rate_mantissa) = last_mantissa;
        (*rate_exponent) = last_exponent;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Closest Rate: %d Kbps, mantissa: %d, exponent: %d\n"),
                     rate, last_mantissa, last_exponent));
    }

    return(rv);
}

STATIC int
_soc_katana_get_shaper_burst_size(int unit, uint32 rate,
                                  uint32 *burst_mantissa, uint32 *burst_exponent)
{
    int      rv = SOC_E_NONE;
    int      i;
    int      delta_rate_diff = 0;
    uint32 last_mantissa = -1, last_exponent = -1, cur_mantissa = -1, cur_exponent;
    uint32 last_rate_diff = -1, cur_rate_diff;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "\n\nCalculating default burst size, rate: %d\n"), (unsigned int)rate));

    if (rate == 0) {
        *burst_mantissa = 0;
        *burst_exponent = 0;
        return rv;
    }

    for (i = 0; i < (KATANA_SHAPER_BURST_MAX_EXPONENT + 1); i++) {
        cur_exponent = shaper_info[unit]->burst_size_info[i].exponent;

        if (shaper_info[unit]->burst_size_info[i].rate <= rate) {
            cur_mantissa = (rate - shaper_info[unit]->burst_size_info[i].rate) / 
                               (1 << (cur_exponent + 2));
        } else {
            break;
        }

        if (cur_mantissa > KATANA_SHAPER_BURST_MAX_MANTISSA) {
            continue;
        }
        
        cur_rate_diff = rate - (shaper_info[unit]->burst_size_info[i].rate +
                               (cur_mantissa * (1 << (cur_exponent + 2))));

        if (last_mantissa == -1) {
            last_mantissa = cur_mantissa;
            last_exponent = cur_exponent;
            last_rate_diff = cur_rate_diff;
        }
        else if (cur_rate_diff < last_rate_diff) {
            last_mantissa = cur_mantissa;
            last_exponent = cur_exponent;
            last_rate_diff = cur_rate_diff;
        }

        if (cur_rate_diff == 0) {
            break;
        }
    }
    if (last_mantissa == -1) {
        rv = SOC_E_PARAM;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d Ingress Burst Size not found\n"), unit));
        return(rv);
    }
    else {
        /* Compute the delta rate between requested rate and hw rate. */
        delta_rate_diff = rate - (shaper_info[unit]->
                                     burst_size_info[last_exponent].rate +
                                 (last_mantissa * (1 << (last_exponent + 2))));

        /* If the delta rate is +ve then only adjust the rate to next higher slot */
        if (delta_rate_diff > 0) {
            if ((last_mantissa == KATANA_SHAPER_BURST_MAX_MANTISSA) &&
                (last_exponent != KATANA_SHAPER_BURST_MAX_EXPONENT)) {
                /* Mantissa is at its max, move to next EXPONENT */
                /* provided we are not at max rate already */
                last_mantissa = 0;
                last_exponent++;
            } else if (last_mantissa != KATANA_SHAPER_BURST_MAX_MANTISSA) {
                /* Move the mantissa to next higher slot */
                last_mantissa++;
            }
        }

        (*burst_mantissa) = last_mantissa;
        (*burst_exponent) = last_exponent;

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Closest BurstSize: %d kbps burstSize: %d kbps, mantissa: %d, exponent: %d\n"),
                     rate,
                     (shaper_info[unit]->burst_size_info[i].rate  + 
                     (last_mantissa * (1 << (last_exponent + 2)))),
                     last_mantissa, last_exponent));
    }

    return(rv);
}

STATIC int
_soc_katana_perq_flex_counters_init(int unit)
{
    _soc_katana_counter_info_t *counter_info;
    uint32 rval, addr;
    int index, seg, i;

    /* Setup MMU counter pool segment start address, consider each 1K block 
       as separate segment */
    for (i = 0; i < 12; i++) {
        if ((!soc_feature(unit, soc_feature_cosq_gport_stat_ability) && 
            ((i % 4) != 0)) || 
            ((soc_feature(unit, soc_feature_counter_toggled_read) &&
            (i == 8)))) {
            /* 
             * Create 3 segments each with 4K counters 
             * Seg0(total enq drop pkts) seg_start address 0
             * Seg4(red enq drop pkts)  seg_start address 4 * 1024
             * Seg8(deq stats)) seg_start_address 8 * 1024 
             */  
            continue;
        }    
        rval = 0;
        soc_reg_field_set(unit, CTR_SEGMENT_STARTr, &rval, SEG_STARTf, (i * 1024));
        SOC_IF_ERROR_RETURN
            (soc_reg32_write(unit, soc_reg_addr(unit, CTR_SEGMENT_STARTr, 
                                                REG_PORT_ANY, i), rval));    
    }

    /* Cosq_stats - 
     * CTR_FLEX_COUNT_0 = Total ENQ discarded 
     * CTR_FLEX_COUNT_4  = Red ENQ discarded 
     * CTR_FLEX_COUNT_8  = Total DEQ
     */ 

    /* Cosq_gport_stats - 
     * CTR_FLEX_COUNT_1 = Total ENQ Accepted 
     * CTR_FLEX_COUNT_2 = Green ENQ discarded 
     * CTR_FLEX_COUNT_3 = Yellow ENQ discarded
     * CTR_FLEX_COUNT_5 = Green ENQ Accepted
     * CTR_FLEX_COUNT_6 = Yellow ENQ Accepted
     * CTR_FLEX_COUNT_7 = Red ENQ Accepted
     */
    
    counter_info = _soc_katana_counter_info;

    /* set deq stats config */
    rval = 0;
    seg = counter_info[_SOC_COUNTER_TYPE_ACCEPT_DEQ].segment;
    if (!soc_feature(unit, soc_feature_cosq_gport_stat_ability) && 
        soc_feature(unit, soc_feature_counter_toggled_read)) {
        seg = counter_info[_SOC_COUNTER_TYPE_ACCEPT_DEQ].segment - 4;
    }
    index = counter_info[_SOC_COUNTER_TYPE_ACCEPT_DEQ].index;
    addr = soc_reg_addr(unit, CTR_DEQ_STATS_CFGr, REG_PORT_ANY, index);
    soc_reg_field_set(unit, CTR_DEQ_STATS_CFGr, &rval, ACTIVE0f, 1);
    soc_reg_field_set(unit, CTR_DEQ_STATS_CFGr, &rval, SEG0f, seg); 
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));

    /* set enq stats config */
    for (i = _SOC_COUNTER_TYPE_DROP_ENQ_GREEN; 
         i < _SOC_COUNTER_TYPE_ACCEPT_DEQ; i++) {

        if (!soc_feature(unit, soc_feature_cosq_gport_stat_ability) &&
            (i >= _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN)) {
            continue;
        }
        rval = 0;
        index = counter_info[i].index;
        addr = soc_reg_addr(unit, CTR_ENQ_STATS_CFGr, REG_PORT_ANY, index);
        if (i < _SOC_COUNTER_TYPE_ACCEPT_ENQ_GREEN) {
            seg = counter_info[_SOC_COUNTER_TYPE_DROP_ENQ].segment;
        } else {
            seg = counter_info[_SOC_COUNTER_TYPE_ACCEPT_ENQ].segment;
        }
        
        if (!soc_feature(unit, soc_feature_counter_toggled_read) ||
            !soc_feature(unit, soc_feature_cosq_gport_stat_ability)) {
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, ACTIVE0f, 1);
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, SEG0f, seg);
        } else {
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, ACTIVE0f, 1);
            seg = counter_info[i].segment;
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, SEG0f, seg);
        }

        if (!soc_feature(unit, soc_feature_counter_toggled_read) && 
            (soc_feature(unit, soc_feature_cosq_gport_stat_ability) ||
            (i == _SOC_COUNTER_TYPE_DROP_ENQ_RED))) {
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, ACTIVE1f, 1);
            seg = counter_info[i].segment;
            soc_reg_field_set(unit, CTR_ENQ_STATS_CFGr, &rval, SEG1f, seg);
        } 
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, rval));
        
    }    
    
    return SOC_E_NONE;
}

int
soc_katana_get_shaper_rate_info(int unit, uint32 rate,
                                 uint32 *rate_mantissa, uint32 *rate_exponent)
{
    int rv = SOC_E_NONE;
    int index;

    if (shaper_info[unit] == NULL) {
        rv = _soc_katana_shaper_rate_info_alloc(unit);
        if (rv != SOC_E_NONE) {
            return(rv);
        }
    }

    rv = _soc_katana_get_shaper_rate(unit, rate, &index, rate_mantissa, rate_exponent);
    if (rv != SOC_E_NONE) {
        return(rv);
    }

    return(rv);
}

int
soc_katana_get_shaper_burst_info(int unit, uint32 rate,
                                 uint32 *burst_mantissa, uint32 *burst_exponent,
                                 uint32 flags)
{
    int rv = SOC_E_NONE;
    uint32 burst_bps;

    if (shaper_info[unit] == NULL) {
        rv = _soc_katana_shaper_rate_info_alloc(unit);
        if (rv != SOC_E_NONE) {
            return(rv);
        }
    }

    /* convert kbps to bps */
    burst_bps = rate * 1000;

    if (burst_bps > 0xFF0000) {
        burst_bps = 0xFF0000;
    }    

    rv = _soc_katana_get_shaper_burst_size(unit,
                        burst_bps, burst_mantissa, burst_exponent);
    if (rv != SOC_E_NONE) {
        return(rv);
    }

    if ((rate > 0) && (*burst_exponent < 5)) {
        *burst_exponent = 5;
        *burst_mantissa = 0;
    }

    return(rv);
}

int
soc_katana_compute_shaper_rate(int unit, uint32 rate_mantissa, 
                               uint32 rate_exponent, uint32 *rate)
{
    int rv = SOC_E_NONE;
    uint64 numerator;
    uint32 denominator;    
    uint32 temp;

    if (rate_exponent == 0) {
	/* (2^r_e+4) * (r_m/1024) / ref_cycle based on 7.8125uS(128KHz) */
        COMPILER_64_SET(numerator, 0, rate_mantissa);
        COMPILER_64_SHL(numerator, 11);
    } else {
	/* (2^(r_e+3)) * (1+(r_m / 1024)) / ref_cycle */
        temp = 1024 + rate_mantissa;
        COMPILER_64_SET(numerator, 0, temp);
        temp = rate_exponent + 3 + 7;
        COMPILER_64_SHL(numerator, temp);
    }

    denominator = 1024;

    if (soc_esw_div64(numerator, denominator, rate) == -1) {
        rv = SOC_E_INTERNAL;
    }

    return(rv);
}

int
soc_katana_compute_shaper_burst(int unit, uint32 burst_mantissa, 
                               uint32 burst_exponent, uint32 *rate)
{
    int rv = SOC_E_NONE;
    /* rate = 2^(t_e+9) * (1+(t_m/128)) */
    *rate = ((1 << (burst_exponent + 9)) * (128 + burst_mantissa)) / 128;

    /* convert bps to kbps */
    *rate = *rate / 1000;

    return(rv);
}

int
soc_katana_compute_refresh_rate(int unit, uint32 rate_exp, 
                                uint32 rate_mant, 
                                uint32 burst_exp, 
                                uint32 burst_mant,
                                uint32 *cycle_sel)
{
    int rv = SOC_E_NONE;
    int temp_cyc;
    int deq_max_temp, deq_max_length_shift, headroom;
    int rfh_max_temp, rfh_max_padded_threshold;
    int rfh_max_temp_threshold, rfh_max_fnl_threshold;

    if (cycle_sel == NULL) {
        return SOC_E_INTERNAL;
    }
    *cycle_sel = 0;

    rfh_max_padded_threshold = (128 + burst_mant) * (1 << (burst_exp + 9));

    rfh_max_temp_threshold = (128 + burst_mant) * (1 << burst_exp);

    for (temp_cyc = 0 ; temp_cyc < 10 ; temp_cyc++) {
        if (rate_exp < 1) {
            rfh_max_temp = rate_exp + temp_cyc + 1;
        } else {
            rfh_max_temp = rate_exp + temp_cyc;
        }

        deq_max_temp = rfh_max_temp;
        if (deq_max_temp <= 10) {
            deq_max_length_shift = 10 - deq_max_temp;
        } else {
            deq_max_length_shift = deq_max_temp - 10;
        }
 
        if (rate_exp < 10) {
            rfh_max_fnl_threshold = rfh_max_padded_threshold / (1 << rfh_max_temp);
        } else {
            rfh_max_fnl_threshold = rfh_max_temp_threshold / 2;
        }

        headroom = 0x1fffff - rfh_max_fnl_threshold;

        /* It is expected that the bucket did not go negative after subtracting the 
	 * threshold.  It is also expected that there is still enough headroom to 
	 * dequeue and account for two 16K packets */

        if ((headroom > 0) && (headroom / (1 << deq_max_length_shift)) >= 32768) {
            *cycle_sel = temp_cyc;
            break;
        }
    }   

    return(rv);
}


int
soc_katana_compute_burst_rate_check(int unit, uint32 rate_exp,
                                uint32 rate_mant,
                                uint32 burst_exp,
                                uint32 burst_mant,
                                uint32 cycle_sel)
{
    int rv = SOC_E_NONE;
    uint32 expected_rate_kbps, expected_burst;
    uint32 deq_max_temp, deq_max_length_shift, max_length;
    uint32 max_credit_pre_shift, max_credit;
    uint32 rfh_max_temp, rfh_max_padded_threshold;
    uint32 rfh_max_temp_threshold, rfh_max_fnl_threshold;
    uint32 deq_bytes = 1;
    
    /* Calculate the expected rate and burst for error checking later */
    if (!rate_exp) {
        expected_rate_kbps = (1 << 4) * 128 * rate_mant / 1024;
    } else if (rate_exp >= 10) {
        /* Avoid uint32 overflow */
        expected_rate_kbps = (1 << (rate_exp + 3)) * 128;
        expected_rate_kbps += expected_rate_kbps / 1024 * rate_mant;
    } else {
        expected_rate_kbps = (1 << (rate_exp + 3)) * 128;
        expected_rate_kbps += expected_rate_kbps * rate_mant / 1024;
    }
    
    expected_burst = (1 << (burst_exp + 9));
    expected_burst += expected_burst * burst_mant / 128;    
    
    /* Compute values that remain constant regardless of cycle_sel */
    if (!rate_exp) {
        max_credit_pre_shift = rate_mant;
    } else {
        max_credit_pre_shift = rate_mant + 1024;
    }
    
    if (rate_exp < 11) {
        max_credit = max_credit_pre_shift;
    } else {
        max_credit = max_credit_pre_shift * (1 << (rate_exp - 10));
    }
    
    rfh_max_padded_threshold = (128 + burst_mant) * (1 << (burst_exp + 9));
    
    rfh_max_temp_threshold = (128 + burst_mant) * (1 << burst_exp);
    
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Shaper inputs:\n")));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  rate_exp = %u\n"), rate_exp));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  rate_mant = %u\n"), rate_mant));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  burst_exp = %u\n"), burst_exp));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  burst_mant = %u\n"), burst_mant));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  cycle_sel = %u\n"), cycle_sel));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  expected_rate_kbps = %u\n"), expected_rate_kbps));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  expected_burst = %u\n"), expected_burst));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  max_credit_pre_shift = %u\n"), max_credit_pre_shift));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  max_credit = %u\n"), max_credit));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  rfh_max_padded_threshold = %u\n"), rfh_max_padded_threshold));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  rfh_max_temp_threshold = %u\n"), rfh_max_temp_threshold));

    /* Compute values */
    if (rate_exp) {
        deq_max_temp = rate_exp + cycle_sel;
    } else {
        deq_max_temp = rate_exp + cycle_sel + 1;
    }

    if (deq_max_temp <= 10) {
        deq_max_length_shift = 10 - deq_max_temp;
    } else {
        deq_max_length_shift = deq_max_temp - 10;
    }

    if (rate_exp < 10) {
        max_length = deq_bytes * (1 << deq_max_length_shift);
    } else {
        max_length = deq_bytes;
    }

    if (rate_exp < 1) {
        rfh_max_temp = rate_exp + cycle_sel + 1;
    } else {
        rfh_max_temp = rate_exp + cycle_sel;
    }

    if (rate_exp < 10) {
        rfh_max_fnl_threshold = rfh_max_padded_threshold / (1 << rfh_max_temp);
    } else {
        rfh_max_fnl_threshold = rfh_max_temp_threshold / 2;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "  cycle_sel = %d:\n"), cycle_sel));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    deq_max_temp = %u\n"), deq_max_temp));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    deq_max_length_shift = %u\n"), deq_max_length_shift));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    max_length = %u\n"), max_length));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    rfh_max_temp = %u\n"), rfh_max_temp));
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    rfh_max_fnl_threshold = %u\n"), rfh_max_fnl_threshold));

    /* Check to ensure that all requirements are satisfied */
    if (max_length > MAX_BUCKET_VAL) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    Failed due to DEQUEUE_ADD check\n")));
        return SOC_E_PARAM;
    }

    if (rfh_max_fnl_threshold > MAX_BUCKET_VAL) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    Failed due to THRESHOLD check\n")));
        return SOC_E_PARAM;
    }

    if (max_credit > MAX_BUCKET_VAL) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    Failed due to REFRESH check\n")));
        return SOC_E_PARAM;
    }


    if (8 * 128 / max_length * max_credit / (1 << cycle_sel) != expected_rate_kbps) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    Failed due to rate mismatch check\n")));
        return SOC_E_PARAM;
    }

    if (rfh_max_fnl_threshold / max_length * 8 != expected_burst) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "    Failed due to burst mismatch check\n")));
        return SOC_E_PARAM;
    }
    return(rv);
}

/* 56440/56445/55440 TDM sequence */
uint32 kt_tdm_0[78] = { 1,9,25,26,27,28,
                        17,2,25,26,27,28,
                        0,10,18,25,26,27,
                        28,3,11,25,26,27,
                        28,35,19,4,25,26,
                        27,28,12,20,25,26,
                        27,28,63,5,13,25,
                        26,27,28,21,6,25,
                        26,27,28,0,14,22,
                        25,26,27,28,7,15,
                        25,26,27,28,35,23,
                        8,25,26,27,28,16,
                        24,25,26,27,28,35
                      };
/* 56441 TDM sequence */
uint32 kt_tdm_1[48] = { 9,28,1,27,
                        63,28,10,27,
                        63,28,0,27,
                        11,28,63,27,
                        63,28,12,27,
                        35,28,63,27,
                        13,28,63,27,
                        63,28,14,27,
                        35,28,63,27,
                        15,28,63,27,
                        63,28,16,27,
                        35,28,0,27
                      };
/* 56442 TDM sequence */
uint32 kt_tdm_2[48] = { 9,17,1,35,
                        63,0,10,18,
                        63,63,63,63,
                        11,19,35,63,
                        63,63,12,20,
                        63,0,63,63,
                        13,21,35,63,
                        63,63,14,22,
                        63,63,63,63,
                        15,23,35,63,
                        63,63,16,24,
                        63,63,0,63
                      };
/* 56443 TDM sequence */
uint32 kt_tdm_3[78] = { 9,1,25,26,28,27,
                        63,10,25,26,29,32,
                        63,0,25,26,30,33,
                        11,63,25,26,31,34,
                        63,12,25,26,28,27,
                        35,63,25,26,29,32,
                        13,63,25,26,30,33,
                        63,14,25,26,31,34,
                        63,63,25,26,28,27,
                        15,35,25,26,29,32,
                        63,16,25,26,30,33,
                        0,63,25,26,31,34,
                        63,63,25,26,63,63
                      };
/* 56440/56445/55440 config-1 TDM sequence */
uint32 kt_tdm_4[78] = { 1,9,25,26,28,27,
                        17,2,25,26,29,32,
                        10,18,25,26,30,33,
                        3,11,25,26,31,34,
                        19,4,25,26,28,35,
                        12,20,25,26,29,27,
                        5,13,25,26,30,32,
                        21,6,25,26,31,33,
                        14,22,25,26,28,34,
                        7,15,25,26,29,27,
                        23,8,25,26,30,32,
                        16,24,25,26,31,33,
                        63,0,25,26,63,34
                      };
/* 56449 config-1 TDM sequence */
uint32 kt_tdm_5[48] = { 1,9,17,25,
                        26,27,2,10,
                        18,28,35,63,
                        3,11,19,0,
                        25,26,4,12,
                        20,27,28,35,
                        5,13,21,63,
                        63,0,6,14,
                        22,25,26,35,
                        7,15,23,27,
                        63,28,8,16,
                        24,63,0,63
                      };
/* 56243 TDM sequence */
uint32 kt_tdm_6[16] = { 27,28,63,35,
                        32,29,0,35,
                        33,30,63,35,
                        34,31,0,63
                      };

/* 240/242/245/246 TDM sequence */
uint32 kt_tdm_7[40] = { 27,35,26,25,
                        32,28,26,25,
                        33,29,26,25,
                        34,30,26,25,
                        35,31,26,25,
                        27,28,26,25,
                        32,29,26,25,
                        33,30,26,25,
                        34,31,26,25,
                        0,1,26,25
                      };

uint32 kt_tdm_8[78] = { 1,9,25,26,27,28,
                        17,2,25,26,27,29,
                        0,10,18,25,26,27,
                        30,3,11,25,26,27,
                        31,35,19,4,25,26,
                        27,28,12,20,25,26,
                        27,29,63,5,13,25,
                        26,27,30,21,6,25,
                        26,27,31,0,14,22,
                        25,26,27,28,7,15,
                        25,26,27,29,35,23,
                        8,25,26,27,30,16,
                        24,25,26,27,31,35
                      };

uint32 kt_tdm_9[78] = { 1,9,25,26,27,28,
                        17,2,25,26,32,28,
                        0,10,18,25,26,33,
                        28,3,11,25,26,34,
                        28,35,19,4,25,26,
                        27,28,12,20,25,26,
                        32,28,63,5,13,25,
                        26,33,28,21,6,25,
                        26,34,28,0,14,22,
                        25,26,27,28,7,15,
                        25,26,32,28,35,23,
                        8,25,26,33,28,16,
                        24,25,26,34,28,35
                      };

static uint32
 _soc_katana_mmu_tdm_var(uint32 *arr, uint16 dev_id, int cfg_num, int pos)
{
    uint32 retval;

    retval = arr[pos];
    
    switch (dev_id) {
        case BCM56441_DEVICE_ID:
        case BCM56446_DEVICE_ID:
            if (cfg_num == 1) {
                switch (pos) {
                    case 5: case 21: case 37: 
                        retval = 29;
                        break;
                    case 9: case 25: case 41:
                        retval = 30;
                        break;
                    case 13: case 29: case 45:
                        retval = 31;
                        break;
                    case 7: case 23: case 39:
                        retval = 32;
                        break;
                    case 11: case 27: case 43:
                        retval = 33;
                        break;
                    case 15: case 31: case 47:
                        retval = 34;
                        break;
                }
            } else if (cfg_num == 2) {
                switch (pos) {
                    case 24 :
                        retval = 9;
                        break;
                    case 30:
                        retval = 10;
                        break;
                    case 36:
                        retval = 11;
                        break;
                    case 42:
                        retval = 12;
                        break;
                    case 5: case 21: case 37:
                        retval = 29;
                        break;
                    case 9: case 25: case 41:
                        retval = 30;
                        break;
                    case 13: case 29: case 45:
                        retval = 31;
                        break;
                }                
            }
            if (dev_id == BCM56446_DEVICE_ID) {
                if ((retval >= 9) && (retval <= 16)) {
                    retval -= 8;
                } else if (retval == 1) {
                    retval = 63;
                }
            }
            break;


        case BCM56442_DEVICE_ID:
        case BCM56447_DEVICE_ID:
            if (dev_id == BCM56447_DEVICE_ID) {
                if ((retval >= 17) && (retval <= 24)) {
                    retval -= 16;
                } else if (retval == 1) {
                    retval = 63;
                }
            }
            break;

        case BCM56242_DEVICE_ID:
        case BCM56246_DEVICE_ID:
            switch (pos) {
                case 7: case 11:
                case 15: case 19:
                case 27: case 31:
                case 35: case 39:
                    retval = 63;
                    break;
                case 6: case 10:
                case 14: case 18:
                case 26: case 30:
                case 34: case 38:
                    retval = 63;
                    break;
            }
            break;

        case BCM56243_DEVICE_ID:
            switch (pos) {
                case 0: case 4:
                case 8: case 12:
                    retval = 63;
                    break;
            }
            break;

        case BCM56240_DEVICE_ID:
        case BCM56245_DEVICE_ID:
            if (cfg_num == 0) {
                switch (pos) {
                    case 4: case 8: case 12:
                    case 24: case 28: case 32:
                        retval = 27;
                        break;
                    case 9: case 13: case 17:
                    case 25: case 29: case 38:
                        retval = 28;
                        break;
                }
            }
            break;

        default:
            break;
    }

    return retval;
}

 STATIC int
_soc_katana_mmu_tdm_init(int unit)
{
    uint32 *arr;
    int i, tdm_size, cfg_num;
    iarb_tdm_table_entry_t iarb_tdm;
    lls_port_tdm_entry_t lls_tdm;
    uint32 rval, arr_ele;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    cfg_num = soc_property_get(unit, spn_BCM5644X_CONFIG, 0);

    if ((dev_id == BCM56440_DEVICE_ID) || (dev_id == BCM56445_DEVICE_ID) ||
        (dev_id == BCM55440_DEVICE_ID) || (dev_id == BCM55441_DEVICE_ID)) {
        tdm_size = 78;
        if (cfg_num == 1) {
            arr = kt_tdm_4;
        } else if (cfg_num == 3) {
            arr = kt_tdm_8;
        } else if (cfg_num == 4) {
            arr = kt_tdm_9;
        } else {
            arr = kt_tdm_0;
        }
    } else if (dev_id == BCM56448_DEVICE_ID) {
        tdm_size = 78;
        arr = kt_tdm_0;
    } else if ((dev_id == BCM56441_DEVICE_ID) || (dev_id == BCM56446_DEVICE_ID)) {
        tdm_size = 48;
        arr = kt_tdm_1;
    } else if ((dev_id == BCM56442_DEVICE_ID) || (dev_id == BCM56447_DEVICE_ID)) {
        tdm_size = 48;
        arr = kt_tdm_2;
    } else if (dev_id == BCM56443_DEVICE_ID) {
        tdm_size = 78;
        arr = kt_tdm_3;
    } else if (dev_id == BCM56243_DEVICE_ID) {
        tdm_size = 16;
        arr = kt_tdm_6;
    } else if ((dev_id == BCM56240_DEVICE_ID) || (dev_id == BCM56241_DEVICE_ID) ||
               (dev_id == BCM56242_DEVICE_ID) || (dev_id == BCM56245_DEVICE_ID) ||
               (dev_id == BCM56246_DEVICE_ID)) {
        tdm_size = 40;
        arr = kt_tdm_7;
    } else if (dev_id == BCM56449_DEVICE_ID) {
        if (cfg_num == 0) {
            tdm_size = 78;
            arr = kt_tdm_0;
        } else if (cfg_num == 1) {
            tdm_size = 48;
            arr = kt_tdm_5;
        }  else {
            return SOC_E_FAIL;
        }
    } else {
        return SOC_E_FAIL;
    }

    /* Disable IARB TDM before programming... */
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    for (i = 0; i < tdm_size; i++) {
        arr_ele = _soc_katana_mmu_tdm_var(arr, dev_id, cfg_num, i);

        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf,
                                        arr_ele);
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                  &iarb_tdm));

        if (0 == (i%2)) {
            /* Two entries per mem entry */
            sal_memset(&lls_tdm, 0, sizeof(lls_port_tdm_entry_t));
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_0f, arr_ele);
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_0_ENABLEf, 1);
        } else {
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_1f, arr_ele);
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_1_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_LLS_PORT_TDMm(unit, SOC_BLOCK_ALL, (i/2),
                                                      &lls_tdm));
        }
    }
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    
    rval = 0;
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, END_Af, tdm_size - 1);
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, END_Bf, tdm_size - 1);
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFGr(unit, rval));

    return SOC_E_NONE;
}

STATIC int
_katana_ledup_init(int unit)
{
    int ix;
    uint32 rval = 0;
    struct led_remap {
       uint32 reg_addr;
       uint32 port0;
       uint32 port1;
       uint32 port2;
       uint32 port3;
    } led0_remap[] = {
        {CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r,
                    REMAP_PORT_32f,REMAP_PORT_33f,REMAP_PORT_34f,REMAP_PORT_35f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r,
                    REMAP_PORT_36f,REMAP_PORT_37f,REMAP_PORT_38f,REMAP_PORT_39f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
                    REMAP_PORT_24f,REMAP_PORT_25f,REMAP_PORT_26f,REMAP_PORT_27f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
                    REMAP_PORT_28f,REMAP_PORT_29f,REMAP_PORT_30f,REMAP_PORT_31f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
                    REMAP_PORT_16f,REMAP_PORT_17f,REMAP_PORT_18f,REMAP_PORT_19f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
                    REMAP_PORT_20f,REMAP_PORT_21f,REMAP_PORT_22f,REMAP_PORT_23f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
                    REMAP_PORT_12f,REMAP_PORT_13f,REMAP_PORT_14f,REMAP_PORT_15f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
                    REMAP_PORT_8f,REMAP_PORT_9f,REMAP_PORT_10f,REMAP_PORT_11f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
                    REMAP_PORT_4f,REMAP_PORT_5f,REMAP_PORT_6f,REMAP_PORT_7f},
        {CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
                    REMAP_PORT_0f,REMAP_PORT_1f,REMAP_PORT_2f,REMAP_PORT_3f}
    };


    int led_data[40] = {
                     1, 2, 3, 4,
                     5, 6, 7, 8,
                     9, 10, 11, 12, 
                     13, 14, 15, 16,
                     17, 18, 19, 20, 
                     21, 22, 23, 24, 
                     38, 38, 38, 25, 
                     26, 38, 38, 26, 
                     34, 33, 32, 27, 
                     31, 30, 29, 28
                   }; 
    /* initialize the led remap register settings. 
     * port 0 entry will not be used for easy index of the physical port starting 1
     */   
    for (ix = 0; ix < sizeof(led0_remap)/sizeof(led0_remap[0]); ix++) {
        rval = 0;
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port0, led_data[ix * 4]);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port1, led_data[(ix * 4) + 1]);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port2, led_data[(ix * 4) + 2]);
        soc_reg_field_set(unit, led0_remap[ix].reg_addr, &rval, 
                         led0_remap[ix].port3, led_data[(ix * 4) + 3]);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit, 
                soc_reg_addr(unit, led0_remap[ix].reg_addr, REG_PORT_ANY, 0), 
                rval));
    }

    /* initialize the UP0 data ram */
    rval = 0;
    for (ix = 0; ix < 256; ix++) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit,ix, rval));
    }
    return SOC_E_NONE;  
}

STATIC int
_soc_katana_misc_init(int unit)
{
    static const soc_field_t port_field[4] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    soc_info_t *si;
    uint32 rval, rval1;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_pbmp_t pbmp;
    int port, blk_port, blk, pindex;
    int count;
    uint64 reg64;
    uint16 dev_id;
    uint8 rev_id;
    int i, parity_enable;
    mmu_ipmc_group_tbl0_entry_t ipmc_entry;
    mmu_rqe_queue_op_node_map_entry_t rqe_entry;
    mmu_wred_queue_op_node_map_entry_t wred_entry;
    mmu_ext_mc_group_map_entry_t mc_group_entry;
    mmu_ext_mc_queue_list0_entry_t mc_list0_entry;
    mmu_ext_mc_queue_list1_entry_t mc_list1_entry;
    mmu_ext_mc_queue_list4_entry_t mc_list4_entry;
    soc_field_t fields[3];
    uint32 values[3];
    soc_port_t it_port;
    uint32 mxq = 0;
    int last_two_mxqblock_ports[2][4] = {{27, 32, 33, 34},{28, 29, 30, 31}};
    soc_port_t any_port=0;
    soc_field_t xport_field[2] = { XPORT2_MULTI_PORTf, XPORT3_MULTI_PORTf };
    uint8   mxqport_used = 0;
    svm_policy_table_entry_t meter_action;
    svm_macroflow_index_table_entry_t macro_flow_entry;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    si = &SOC_INFO(unit);
    
    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        (void) _soc_katana_ser_init(unit);
#ifdef INCLUDE_MEM_SCAN
        soc_mem_scan_ser_list_register(unit, FALSE,
                                       _soc_kt_ser_parity_info[unit]);
#endif
        memset(&_kt_ser_functions, 0, sizeof(soc_ser_functions_t));
        _kt_ser_functions._soc_ser_stat_nack_f = &soc_katana_stat_nack;
        _kt_ser_functions._soc_ser_fail_f = &soc_katana_ser_fail;
        _kt_ser_functions._soc_ser_mem_nack_f = &soc_katana_mem_nack;
        _kt_ser_functions._soc_ser_parity_error_cmicm_intr_f = 
            &soc_katana_parity_error;
        soc_ser_function_register(unit, &_kt_ser_functions);
    } else {
        memset(&_kt_oam_event_functions, 0, sizeof(soc_oam_event_functions_t));
        _kt_oam_event_functions._soc_oam_event_intr_f = &soc_katana_oam_event_process;
        soc_oam_event_function_register (unit, &_kt_oam_event_functions);
    }


    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_katana_pipe_mem_clear(unit));
    }

    /* Some registers are implemented in memory, need to clear them in order
     * to have correct parity value */
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2r(unit, port, 0));
    }

    /* default DDR3 Configuration */
    /* 16k Col * 1k Row * 8 Banks * 16 bits = 2Gb */
#if 0 /* Katana Only supports 1k Cols and 8 banks */
    SOC_DDR3_NUM_COLUMNS(unit) = soc_property_get(unit,spn_EXT_RAM_COLUMNS, 1024);
    SOC_DDR3_NUM_BANKS(unit) = soc_property_get(unit,spn_EXT_RAM_BANKS, 8);
#else
    SOC_DDR3_NUM_COLUMNS(unit) = 1024;
    SOC_DDR3_NUM_BANKS(unit) = 8;
#endif
    SOC_DDR3_NUM_MEMORIES(unit) = soc_property_get(unit,spn_EXT_RAM_PRESENT, 3);
    SOC_DDR3_NUM_ROWS(unit) = soc_property_get(unit,spn_EXT_RAM_ROWS, 8192);
    SOC_DDR3_CLOCK_MHZ(unit) = soc_property_get(unit, spn_DDR3_CLOCK_MHZ, 800);
    SOC_DDR3_MEM_GRADE(unit) = soc_property_get(unit, spn_DDR3_MEM_GRADE, 0);

    if (soc_feature(unit, soc_feature_ddr3)) {
        /* CI Init */
        soc_ddr40_set_shmoo_dram_config(unit, ((uint32) 0xFFFFFFFF) >> (32 - SOC_DDR3_NUM_MEMORIES(unit)));
        if ((soc_ddr40_phy_pll_ctl(unit, 0, SOC_DDR3_CLOCK_MHZ(unit), 1, 0) == SOC_E_NONE) &&
            (soc_ddr40_ctlr_ctl(unit, 0, 1, 0) == SOC_E_NONE) &&
            (soc_ddr40_phy_calibrate(unit, 0, 1, 0) == SOC_E_NONE)) {

            if (soc_property_get(unit, spn_DDR3_AUTO_TUNE, FALSE)) {
                soc_ddr40_shmoo_ctl(unit, 0, 1, 1, 0, 0);
                if (SOC_DDR3_NUM_MEMORIES(unit) == 3) {
                    soc_ddr40_shmoo_ctl(unit, 2, 1, 1, 0, 0);
                }
                LOG_CLI((BSL_META_U(unit,
                                    "DDR Tune Completed\n")));
            } else {
                if ((soc_ddr40_shmoo_restorecfg(unit, 0) == SOC_E_NONE) &&
                    ((SOC_DDR3_NUM_MEMORIES(unit) != 3) ||
                     (soc_ddr40_shmoo_restorecfg(unit, 2) == SOC_E_NONE))) {
                    LOG_CLI((BSL_META_U(unit,
                                        "DDR Tune Values Restored\n")));
                }
            }
        }
    }
    /* SW WAR for avoiding continuous parity errors coming from 
       SVM_POLICY_TABLE and SVM_MACROFLOW_INDEX_TABLE entry 0 */ 
    SOC_IF_ERROR_RETURN
       (soc_mem_read(unit, SVM_POLICY_TABLEm, MEM_BLOCK_ANY, 
                                 0, &meter_action)); 
    SOC_IF_ERROR_RETURN
       (soc_mem_read(unit, SVM_MACROFLOW_INDEX_TABLEm, MEM_BLOCK_ANY, 
                                 0, &macro_flow_entry)); 
    SOC_IF_ERROR_RETURN
       (soc_mem_write(unit, SVM_POLICY_TABLEm, MEM_BLOCK_ANY, 
                                 0, &meter_action)); 
    SOC_IF_ERROR_RETURN
       (soc_mem_write(unit, SVM_MACROFLOW_INDEX_TABLEm, MEM_BLOCK_ANY, 
                                 0, &macro_flow_entry)); 
    /* Reset MXQPORT MIB counter (registers implemented in memory) */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
        blk_port = SOC_BLOCK_PORT(unit, blk);
        if (blk_port < 0) {
            continue;
        }
        rval = 0;
        soc_reg_field_set(unit, XPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf); /* All Ports */
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, blk_port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, blk_port, 0));
    }
    /* WRED Configuration */
    if ((dev_id != BCM56440_DEVICE_ID) && (dev_id != BCM56445_DEVICE_ID) && 
        (dev_id != BCM56448_DEVICE_ID) && (dev_id != BCM55440_DEVICE_ID)) { /* And all other 90MHz variants */
        SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 7);
        SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_MASKr(unit, &rval));
        soc_reg_field_set(unit, WRED_PARITY_ERROR_MASKr, &rval, UPDATE_INTRPT_MASKf, 0);
        SOC_IF_ERROR_RETURN(WRITE_WRED_PARITY_ERROR_MASKr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, REFRESH_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    }

    if (parity_enable) {
        _soc_katana_parity_enable_all(unit, TRUE);
    }

    /* Egress Logical to physical port map.
       For KT logical-port and physical-port are the same */
    PBMP_ALL_ITER(unit, port) {
        rval = 0;
        soc_reg_field_set(unit, EGR_VLAN_LOGICAL_TO_PHYSICAL_MAPPINGr, &rval,
                          PHYSICAL_PORT_NUMf, port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_VLAN_LOGICAL_TO_PHYSICAL_MAPPINGr(unit, port, rval));
    }

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

    SOC_PBMP_CLEAR(pbmp);
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port) ||
            (si->port_group[port] >= 2 && si->port_group[port] <= 3)) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        }
    }

    sal_memset(entry, 0, sizeof(egr_ing_port_entry_t));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 1);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XPORT_CONFIGr, port, HIGIG_MODEf,
                                    1));
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, si->cpu_hg_index,
                       entry));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 2);
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, LB_PORT(unit),
                       entry));

    /* MXQPORT0 - 25 */
    rval = 0;
    rval1 = 0;
    soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval1,
                      PORT0f, 1); /* Enable 1 port */
    if (SOC_PORT_VALID(unit, 25)) {
        if (si->port_speed_max[25] < 10000) {
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                PHY_PORT_MODEf, 2);
            /* No Quad Core port mode, since only one port is used */
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                CORE_PORT_MODEf, 0);
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                PORT_GMII_MII_ENABLEf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, 25, rval));
        SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, 25, rval1));
    }

    /* MXQPORT1 - 26 */
    rval = 0;
    rval1 = 0;
    soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval1,
                      PORT0f, 1); /* Enable 1 port */
    if (SOC_PORT_VALID(unit, 26)) {
        if (si->port_speed_max[26] < 10000) {
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                PHY_PORT_MODEf, 2);
            /* No Quad Core port mode, since only one port is used */
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                CORE_PORT_MODEf, 0);
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                PORT_GMII_MII_ENABLEf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, 26, rval));
        SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, 26, rval1));
    }

    /* MXQPORT2 = 27,32,33,34 */
    /* MXQPORT3 = 28,29,30,31 */
    rval = 0;
    rval1 = 0;

    for(mxq=2;mxq<=3;mxq++) {
        mxqport_used = 0;
        rval = 0;
        rval1 = 0;
        any_port = 0;

        for (pindex = 0; pindex < 4; pindex++) {
             if (SOC_PORT_VALID(unit,last_two_mxqblock_ports[mxq-2][pindex])) {
                 mxqport_used++;
                 any_port = last_two_mxqblock_ports[mxq-2][pindex];
                 if (mxqport_used == 1) {
                     SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(
                                         unit, any_port, &rval1));
                 }
                 soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval1,
                                   port_field[pindex], 1);
             }
        }
        if (any_port != 0) {
            SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, any_port, &rval));
            if (mxqport_used == 1) {
                soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                  CORE_PORT_MODEf, 0); /* Single */
            } else {  /* Dual=1 is not valid for Katana device */
                soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                  CORE_PORT_MODEf, 2); /* Quad */
            }
            if (si->port_speed_max[any_port] < 10000) { /* Actually <=2500 */
                soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                  PHY_PORT_MODEf, 2); /* Quad */
                soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                  PORT_GMII_MII_ENABLEf, 1);
            } else {
                soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                  PHY_PORT_MODEf, 0); /* Single */
            }
            SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, any_port, rval));
            SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(
                                unit, any_port, rval1));
            if (mxqport_used > 0) {
                SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
                soc_reg_field_set(unit, MISCCONFIGr, &rval,
                                  xport_field[mxq-2], 1);
                SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
            }
        }
    }


    count = 32;

    /* Setup main TDM control */
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    /* BCMSIM handles Single Cell packets */
    if (SAL_BOOT_BCMSIM) {
        soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    } else {
        soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    }
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, count);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

	_soc_katana_perq_flex_counters_init(unit);
	
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    if (0) { /* FIXME: Which Devices?   if (dev_id != BCM56440_DEVICE_ID)  */
        rval = 0;
        soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval, IFP_BYPASS_ENABLEf,
                          1);
        SOC_IF_ERROR_RETURN(WRITE_ING_BYPASS_CTRLr(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, EGR_BYPASS_CTRLr, &rval, EFP_BYPASSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_BYPASS_CTRLr(unit, rval));
    }

    /* Enable dual hash on L2, L3 and MPLS_ENTRY tables */
    fields[0] = ENABLEf;
    values[0] = 1;
    fields[1] = HASH_SELECTf;
    values[1] = FB_HASH_CRC32_LOWER;
    fields[2] = INSERT_LEAST_FULL_HALFf;
    values[2] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MPLS_ENTRY_HASH_CONTROLr, REG_PORT_ANY,
                                INSERT_LEAST_FULL_HALFf, 1));

    /*
     * IPMC init 
     */
    SOC_IF_ERROR_RETURN(READ_RQE_GLOBAL_CONFIGr(unit, &rval));
    soc_reg_field_set(unit,RQE_GLOBAL_CONFIGr, &rval, PARITY_GEN_ENf,
                      1);
    soc_reg_field_set(unit,RQE_GLOBAL_CONFIGr, &rval, PARITY_CHK_ENf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_RQE_GLOBAL_CONFIGr(unit, rval));

    sal_memset(&ipmc_entry, 0, sizeof(mmu_ipmc_group_tbl0_entry_t));
    count = soc_mem_index_count(unit, MMU_IPMC_GROUP_TBL0m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_IPMC_GROUP_TBL0m(unit,
                            MEM_BLOCK_ANY, i, &ipmc_entry));
    }
    count = soc_mem_index_count(unit, MMU_IPMC_GROUP_TBL1m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_IPMC_GROUP_TBL1m(unit,
                            MEM_BLOCK_ANY, i, &ipmc_entry));
    }
    count = soc_mem_index_count(unit, MMU_IPMC_GROUP_TBL2m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_IPMC_GROUP_TBL2m(unit,
                            MEM_BLOCK_ANY, i, &ipmc_entry));
    }
    count = soc_mem_index_count(unit, MMU_IPMC_GROUP_TBL3m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_IPMC_GROUP_TBL3m(unit,
                            MEM_BLOCK_ANY, i, &ipmc_entry));
    }
    count = soc_mem_index_count(unit, MMU_IPMC_GROUP_TBL4m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_IPMC_GROUP_TBL4m(unit,
                            MEM_BLOCK_ANY, i, &ipmc_entry));
    }

    sal_memset(&rqe_entry, 0, sizeof(mmu_rqe_queue_op_node_map_entry_t));
    count = soc_mem_index_count(unit, MMU_RQE_QUEUE_OP_NODE_MAPm);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_RQE_QUEUE_OP_NODE_MAPm(unit,
                            MEM_BLOCK_ANY, i, &rqe_entry));
    }

    sal_memset(&wred_entry, 0, sizeof(mmu_wred_queue_op_node_map_entry_t));
    count = soc_mem_index_count(unit, MMU_WRED_QUEUE_OP_NODE_MAPm);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_WRED_QUEUE_OP_NODE_MAPm(unit,
                            MEM_BLOCK_ANY, i, &wred_entry));
    }

    sal_memset(&mc_group_entry, 0, sizeof(mmu_ext_mc_group_map_entry_t));
    count = soc_mem_index_count(unit, MMU_EXT_MC_GROUP_MAPm);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_EXT_MC_GROUP_MAPm(unit,
                            MEM_BLOCK_ANY, i, &mc_group_entry));
    }

    sal_memset(&mc_list0_entry, 0, sizeof(mmu_ext_mc_queue_list0_entry_t));
    count = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST0m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit,
                            MEM_BLOCK_ANY, i, &mc_list0_entry));
    }

    sal_memset(&mc_list1_entry, 0, sizeof(mmu_ext_mc_queue_list1_entry_t));
    count = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST1m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_EXT_MC_QUEUE_LIST1m(unit,
                            MEM_BLOCK_ANY, i, &mc_list1_entry));
    }

    sal_memset(&mc_list4_entry, 0, sizeof(mmu_ext_mc_queue_list4_entry_t));
    count = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST4m);
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_EXT_MC_QUEUE_LIST4m(unit,
                            MEM_BLOCK_ANY, i, &mc_list4_entry));
    }

    /*
     * Egress Enable
     */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry));
    }

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    PBMP_E_ITER(unit, port) {
        if (IS_MXQ_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_E_ITER(unit, port) {
        if (IS_MXQ_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
    }

    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                           &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));

    /* set to allow Mirror bit in Module Header */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_CONFIG_1r, REG_PORT_ANY, 
                                RING_MODEf, 1));


    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf,
                           &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_TXBUF_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_TXBUF_CONFIGr, &rval,
                            FIRST_SERVE_BUFFERS_WITH_EOP_CELLSf, 0);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_TXBUF_CONFIGr(unit, rval));

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT)));

    /* 
     * Set the per-chip STORM_CONTROL_METER_MAPPING register for now:
     * 0th meter - BCAST 
     * 1st meter - MCAST (including UNKNOWN/KNOWN and IPMC/L2MC);
     * 2rd meter - DLF.
     * The rate APIs rely on above setting.
     */
    {
        soc_field_t  fields[] = { BCAST_METER_INDEXf, KNOWN_L2MC_METER_INDEXf,
            UNKNOWN_L2MC_METER_INDEXf, KNOWN_IPMC_METER_INDEXf, 
            UNKNOWN_IPMC_METER_INDEXf, DLFBC_METER_INDEXf };
        uint32 values[] =  { 0, 1, 1, 1, 1, 2 };


        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, STORM_CONTROL_METER_MAPPINGr,
                             REG_PORT_ANY, COUNTOF(values), fields, values));

        /* Enable vrf based l3 lookup by default. */
        SOC_IF_ERROR_RETURN
           (soc_reg_field32_modify(unit, VRF_MASKr, REG_PORT_ANY, MASKf, 0));

        /* Setup SW2_FP_DST_ACTION_CONTROL */
        fields[0] = HGTRUNK_RES_ENf;
        fields[1] = LAG_RES_ENf;
        values[0] = values[1] = 1;
        SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit,
                SW2_FP_DST_ACTION_CONTROLr, REG_PORT_ANY, 2, fields, values));
    }

    /* initialize LED UP0 */
    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_katana_ledup_init(unit));
    }

    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit));
    }

#define RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

    if (soc_feature(unit, soc_feature_ces)) {
        /*
         * Add CES TDM ports
         */
        for (i = 0;i < 16;i++) {
            int port = 39 + i;
            SOC_PBMP_PORT_ADD(si->tdm_pbm, port);
            si->tdm.port[si->tdm.num++] = port;
            if (si->tdm.min > port || si->tdm.min < 0) {
        	si->tdm.min = port;
            }
            if (si->tdm.max < port) {
        	si->tdm.max = port;
            }
            SOC_PBMP_PORT_ADD(si->tdm.bitmap, port);
        }
    } else {
        /*
         * If there is no CES then remove port 1 from the bitmaps
         */
        if (dev_id == BCM56441_DEVICE_ID ||
            dev_id == BCM56442_DEVICE_ID ||
            dev_id == BCM56443_DEVICE_ID) {
            SOC_PBMP_PORT_REMOVE(si->ge.bitmap, 1);
            SOC_PBMP_PORT_REMOVE(si->ether.bitmap, 1);
            SOC_PBMP_PORT_REMOVE(si->port.bitmap, 1);
            SOC_PBMP_PORT_REMOVE(si->all.bitmap, 1);
            RECONFIGURE_PORT_TYPE_INFO(ether);
            RECONFIGURE_PORT_TYPE_INFO(ge);
            RECONFIGURE_PORT_TYPE_INFO(port);
            RECONFIGURE_PORT_TYPE_INFO(all);
            soc_dport_map_update(unit);
        }
    }

    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
    }

    /* LED Scan operation needs all GPORT_UMAC_CONTROLr out of reset.
     * Irrespective of the block has a valid port. So, use raw addresses
     */
    {
        /* GPORT_UMAC_CONTROL.gport0 */
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 0x680049, 0xff)); 
        /* GPORT_UMAC_CONTROL.gport1 */
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 0x780049, 0xff)); 
        /* GPORT_UMAC_CONTROL.gport2 */
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 0x880049, 0xff)); 
        sal_udelay(10);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 0x680049, 0x0));
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 0x780049, 0x0));
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 0x880049, 0x0));
    }
    
    return SOC_E_NONE;
}

#define KT_MMU_EFIFO_DEPTH             10
#define KT_MMU_EFIFO_XMIT_THERESHOLD   4

/*********************************************************
 * KATANA MMU HELPER  --- START
 */

/* Based on Katana_MMU_Settings-v7.xls */

#define KA_MMU_MIN_PKT_SIZE    64    /*(bytes) c5*/
#define KA_MMU_ETHERNET_MTU_BYTES    1536 /*(bytes) c6*/
#define KA_MMU_MAX_PKT_SIZE    9216/*(bytes) c7*/
#define KA_MMU_JUMBO_FRAME     9216/*(bytes) c8*/
#define KA_MMU_INT_BUF_CELL_SIZE   192/*(bytes) c9*/
#define KA_MMU_EXT_BUF_CELL_SIZE   768/*(bytes) c10*/
#define KA_MMU_PKT_HEADER_SIZE     52/*(bytes) c11*/
#define KA_MMU_LOSSLESS_PG_NUM     1/*c15*/
#define KA_MMU_NUM_CPU_PORT    1/*c30*/
#define KA_MMU_NUM_CPU_QUEUE   48/*c31*/
#define KA_MMU_NUM_LB_PORT    1/*c33*/
#define KA_MMU_NUM_LB_QUEUE   8/*c34*/
#define KA_MMU_NUM_EGRESS_QUEUE_IN_DEVICE    4096/*c36*/
#define SB_MMU_INT_BUF_SIZE    512/*(MB)*/
#define KA_MMU_INT_BUF_SIZE    2048/*(MB) c37  "10924cells*192/1024"*/
#define KA_MMU_RESERVED_INT_BUF_SPACE_CFAP    8 /*(MB) c38 "44cells*192/1024" */
#define SB_MMU_EXT_BUF_SIZE    96/*(MB)*/
#define KA_MMU_EXT_BUF_SIZE    192/*(MB) c42*/
#define KA_MMU_RESERVED_EXT_BUF_SPACE_CFAP    1535 /*cells c43*/
#define SB_MMU_EGRESS_QUEUE_ENTRIES    128/*(K)*/
#define KA_MMU_EGRESS_QUEUE_ENTRIES    256/*(K)c44*/
#define KA_MMU_RE_WORK_QUEUE_ENTRIES    8/*(K)c46*/
#define KA_MMU_RESERVED_RE_WORK_QUEUE_ENTRIES    292 /*c47*/
#define KA_MMU_RE_WORK_QUEUES_IN_DEVICE    3/*c48*/
#define KA_MMU_EMA_QUEUES    8/*c49*/
#define KA_MMU_ING_PORT_DYN_THD_PARAM    2 /*c53*/
#define KA_MMU_ING_PG_DYN_THD_PARAM    7 /*c54*/
#define KA_MMU_EGR_QUEUE_DYN_THD_PARAM    (0.25) /*c55*/
#define KA_MMU_EGR_QUEUE_DYN_THD_PARAM_LOSSY    4 /*c56*/
#define KA_MMU_ING_CELL_BUF_REDUCTION    0/*c57*/
#define KA_MMU_ING_PKT_BUF_REDUCTION    0/*c58*/
#define SB_MMU_CFAP_THDIO_LATENCY       20

/* Intermediate Results */
#define KA_MMU_MAX_PKT_SIZE_IN_CELLS    (((KA_MMU_MAX_PKT_SIZE + \
                                          KA_MMU_PKT_HEADER_SIZE) / \
                                          KA_MMU_INT_BUF_CELL_SIZE) + 1)/*c67*/
#define KA_MMU_JUMBO_FRAME_INT_BUF    (((KA_MMU_JUMBO_FRAME + \
                                          KA_MMU_PKT_HEADER_SIZE) / \
                                          KA_MMU_INT_BUF_CELL_SIZE) + 1)/*c68*/
#define KA_MMU_JUMBO_FRAME_EXT_BUF    (((KA_MMU_JUMBO_FRAME + \
                                          KA_MMU_PKT_HEADER_SIZE) / \
                                          KA_MMU_EXT_BUF_CELL_SIZE) + 1)/*c69*/

#define KA_MMU_EXT_BUF_TO_INT_BUF_RATIO    (KA_MMU_EXT_BUF_CELL_SIZE / \
                                            KA_MMU_INT_BUF_CELL_SIZE) /*c95*/

#define KA_MMU_CELL_PER_AVG_PKT_INT_BUF    1 /*c96*/
#define KA_MMU_CELL_PER_AVG_PKT_EXT_BUF    1 /*c97*/

typedef struct _soc_ka_mmu_params {
    uint32    lossless_mode;
    uint32    extbuf_used; /*c16*/
    uint32    num_ge_ports;/*c18*/
    uint32    num_hg_ports;/*c24*/
    uint32    available_intbuf;   /*c39 portion of total internal buffer */
    uint32    available_emapool;  /*c40 portion of total internal buffer */
    uint32    num_replication_per_pkt; /*c45*/
    /* Input Port Thresholds */
    uint32    hg_pg_hdrm_intbuf;/*108*/
    uint32    hg_pg_hdrm_ema;/*109*/
    uint32    hg_pg_hdrm_extbuf;/*110*/
    uint32    hg_pg_hdrm_rewqe;/*111*/
    uint32    ge_pg_hdrm_intbuf;/*118*/
    uint32    ge_pg_hdrm_ema;/*119*/
    uint32    ge_pg_hdrm_extbuf;/*120*/
    uint32    ge_pg_hdrm_rewqe;/*121*/
    uint32    cpu_pg_hdrm_intbuf;/*128*/
    uint32    cpu_pg_hdrm_ema;/*129*/
    uint32    cpu_pg_hdrm_extbuf;/*130*/
    uint32    cpu_pg_hdrm_rewqe;/*131*/
    uint32    cpu_pg_hdrm_eqe;/*132*/
    uint32    lb_pg_hdrm_intbuf;/*133*/
    uint32    lb_pg_hdrm_ema;/*134*/
    uint32    lb_pg_hdrm_extbuf;/*135*/
    uint32    lb_pg_hdrm_rewqe;/*136*/
    uint32    lb_pg_hdrm_eqe;/*137*/
} _soc_ka_mmu_params_t;

/* Helper function for MMU settings based on
 * memory selection (internal/external/both) and mode (lossless/lossy) 
 * The memory selection is done using config variable  'pbmp_ext_mem'
 * and the mode is selected based on config variable 'lossless_mode' 
 */
STATIC int
_soc_katana_mmu_init_helper(int unit)
{
    _soc_ka_mmu_params_t    ka_mmu_params;
    uint32    mmu_egr_queue_dyn_thd_param_lossy = 0; /*c56 */
    /* Intermediate Results */
    uint32    ethernet_mtu_cells_intbuf = 0;/*c71*/
    uint32    ethernet_mtu_cells_extbuf = 0;/*c72*/
    uint32    num_ports = 0;        /*data ports+cpu+loopback, c74*/
    uint32    num_data_ports = 0;   /*data ports only, c75*/
    uint32    num_eqe_geports_intbuf_bound = 0;/*c77*/
    uint32    num_eqe_geports_extbuf_bound = 0;/*c78*/
    uint32    num_eqe_hgports_intbuf_bound = 0;/*c79*/
    uint32    num_eqe_hgports_extbuf_bound = 0;/*c80*/
    uint32    total_queue_intbuf_bound = 0;/*c83*/
    uint32    total_queue_extbuf_bound = 0;/*c84*/
    uint32    num_ema_queue = 0;/*c85*/
    uint32    total_eqe_device = 0;/*c87*/
    uint32    max_intbuf_size;/*(cells)c88*/
    uint32    intbuf_pool_size = 0;/*(cells)c90 for both ingress and egress buffer */
    uint32    emapool_size = 0; /*(cells) c91*/
    uint32    extbuf_size = 0;/*(cells) c92*/
    uint32    rewqe_entries_admission = 0; /*c93*/
    /* Input port thresholds */
    uint32    ipthd_global_hdrm_intbuf = 0;/*c103*/
    uint32    ipthd_global_hdrm_extbuf = 0;/*c105*/
    uint32    ipthd_global_hdrm_ema = 0;/*c104*/
    uint32    ipthd_global_hdrm_rewqe = 0;/*c106*/
    uint32    ipthd_global_hdrm_eqe = 0; /*c107*/

    uint32    ipthd_hg_pg_hdrm_eqe = 0;/*c112*/

    uint32    ipthd_hg_total_hdrm_intbuf = 0;/*c113*/
    uint32    ipthd_hg_total_hdrm_extbuf = 0;/*c115*/
    uint32    ipthd_hg_total_hdrm_ema = 0;/*c114*/
    uint32    ipthd_hg_total_hdrm_rewqe = 0;/*c116*/
    uint32    ipthd_hg_total_hdrm_eqe = 0;/*c117*/

    uint32    ipthd_ge_pg_hdrm_eqe = 0;/*c122*/

    uint32    ipthd_ge_total_hdrm_intbuf = 0;/*c123*/
    uint32    ipthd_ge_total_hdrm_extbuf = 0;/*c125*/
    uint32    ipthd_ge_total_hdrm_ema = 0;/*c124*/
    uint32    ipthd_ge_total_hdrm_rewqe = 0;/*c126*/
    uint32    ipthd_ge_total_hdrm_eqe = 0;/*c127*/

    uint32    ipthd_total_hdrm_intbuf = 0;/*c138*/
    uint32    ipthd_total_hdrm_extbuf = 0;/*c140*/
    uint32    ipthd_total_hdrm_ema = 0;/*c139*/
    uint32    ipthd_total_hdrm_rewqe = 0;/*c141*/
    uint32    ipthd_total_hdrm_eqe = 0;/*142*/

    uint32    ipthd_per_pg_min_intbuf_ingpool = 0;/*c144*/
    uint32    ipthd_per_pg_min_intbuf_ingpool_pg_0_6 = 0;/*c145*/
    uint32    ipthd_per_pg_min_intbuf_emapool = 0;/*c146*/
    uint32    ipthd_per_pg_min_extbuf = 0;/*c147*/
    uint32    ipthd_per_pg_min_rewqe = 0;/*c148*/
    uint32    ipthd_per_pg_min_eqe = 0;/*c149*/

    uint32    ipthd_total_pg_min_intbuf_ingpool = 0;/*c151*/
    uint32    ipthd_total_pg_min_intbuf_emapool = 0;/*c152*/
    uint32    ipthd_total_pg_min_extbuf = 0;/*c153*/
    uint32    ipthd_total_pg_min_rewqe = 0;/*c154*/
    uint32    ipthd_total_pg_min_eqe = 0;/*c155*/

    uint32    ipthd_per_port_min_intbuf_ingpool = 0;/*c157*/
    uint32    ipthd_per_port_min_intbuf_emapool = 0;/*c158*/
    uint32    ipthd_per_port_min_extbuf = 0;/*c159*/
    uint32    ipthd_per_port_min_rewqe = 0;/*c160*/
    uint32    ipthd_per_port_min_eqe = 0;/*c161*/

    uint32    ipthd_total_port_min_intbuf_ingpool = 0;/*c163*/
    uint32    ipthd_total_port_min_intbuf_emapool = 0;/*c164*/
    uint32    ipthd_total_port_min_extbuf = 0;/*c165*/
    uint32    ipthd_total_port_min_rewqe = 0;/*c166*/
    uint32    ipthd_total_port_min_eqe = 0;/*c167*/

    uint32    ipthd_total_min_intbuf_ingpool = 0;/*c169*/
    uint32    ipthd_total_min_intbuf_emapool = 0;/*c170*/
    uint32    ipthd_total_min_extbuf = 0;/*c171*/
    uint32    ipthd_total_min_rewqe = 0;/*c172*/
    uint32    ipthd_total_min_eqe = 0;/*c173*/

    uint32    ipthd_total_shared_intbuf_ingpool = 0;/*c175*/
    uint32    ipthd_total_shared_intbuf_emapool = 0;/*c176*/
    uint32    ipthd_total_shared_extbuf = 0;/*c177*/
    uint32    ipthd_total_shared_rewqe = 0;/*c178*/
    uint32    ipthd_total_shared_eqe = 0;/*c179*/

    /* output port thresholds */
    uint32    opthd_port_min_grntee_queue_intbuf = 0;/*c193*/
    uint32    opthd_port_min_grntee_queue_extbuf = 0;/*c194*/
    uint32    opthd_port_min_grntee_eqe = 0;/*c195*/
    uint32    opthd_port_min_grntee_ema_queue = 0; /*c196*/
    uint32    opthd_port_min_grntee_rewqe = 0;/*c197*/
    uint32    opthd_port_min_grntee_rewqe_intbuf_mc = 0;/*c198*/
    uint32    opthd_port_min_grntee_rewqe_intbuf_tocpu = 0;/*c199*/
    uint32    opthd_port_min_grntee_rewqe_intbuf_mirror = 0;/*c200*/
    uint32    opthd_port_min_grntee_rewqe_extbuf = 0;/*c201*/

    uint32    opthd_total_min_grntee_queue_intbuff = 0;/*c204*/
    uint32    opthd_total_min_grntee_queue_extbuff = 0;/*c205*/
    uint32    opthd_total_min_grntee_eqe = 0;/*c206*/
    uint32    opthd_total_min_grntee_ema_queue = 0;/*c207*/
    uint32    opthd_total_min_grntee_rewqe = 0;/*c208*/
    uint32    opthd_total_min_grntee_rewqe_intbuf = 0;/*c209*/
    uint32    opthd_total_min_grntee_rewqe_extbuf = 0;/*c210*/

    uint32    opthd_total_shared_queue_intbuff = 0;/*c213*/
    uint32    opthd_total_shared_queue_extbuff = 0;/*c214*/
    uint32    opthd_total_shared_queue_eqe = 0;/*c215*/
    uint32    opthd_total_shared_queue_ema_queue = 0;/*c216*/
    uint32    opthd_total_shared_queue_rewqe = 0;/*c217*/
    uint32    opthd_total_shared_queue_rewqe_intbuf = 0;/*c218*/
    uint32    opthd_total_shared_queue_rewqe_extbuf = 0;/*c219*/

    uint32    mmu_egress_queue_entries = 0;
    uint32    ext_mem_port_count = 0;
    uint32    valid_port_count = 0;
    uint32    all_int_mem_port = 0;
    uint32    all_ext_mem_port = 0;
    uint32    int_ext_mem_port = 0;
    uint32    lossless = 0;
    uint32    rval = 0, val = 0;
    uint32    i = 0, j = 0, pg_num = 0, opn_index = 0;
    uint32    q_min = 0, q_color_dyn = 0;
    uint32    entry[SOC_MAX_MEM_WORDS];
    uint64    r64val, val64;
    soc_port_t    port;
    soc_info_t    *si;
    soc_pbmp_t    temp_pbmp;
    int           saber_mmu_ext_buf_size = 0;
    uint16        dev_id = 0;
    uint8         rev_id = 0;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    mmu_egress_queue_entries = KA_MMU_EGRESS_QUEUE_ENTRIES; /*256K */
    if ((dev_id == BCM56240_DEVICE_ID) ||
         (dev_id == BCM56241_DEVICE_ID) ||
         (dev_id == BCM56242_DEVICE_ID) ||
         (dev_id == BCM56243_DEVICE_ID)) {
        saber_mmu_ext_buf_size = SB_MMU_EXT_BUF_SIZE; /* 96 MB */
        mmu_egress_queue_entries = SB_MMU_EGRESS_QUEUE_ENTRIES; /*128K */
    }

    if ((dev_id == BCM56245_DEVICE_ID) ||
        (dev_id == BCM56246_DEVICE_ID)) {
        saber_mmu_ext_buf_size = KA_MMU_EXT_BUF_SIZE; /* 192 MB */
    }

    COMPILER_64_ZERO(r64val);
    COMPILER_64_ZERO(val64);

    si = &SOC_INFO(unit);
    sal_memset(&ka_mmu_params, 0, sizeof(_soc_ka_mmu_params_t));
    ka_mmu_params.lossless_mode = (soc_property_get(unit, spn_LOSSLESS_MODE, 0) |
                                   soc_property_get(unit, spn_MMU_LOSSLESS, 0));
    lossless = ka_mmu_params.lossless_mode;
    SOC_PBMP_COUNT(PBMP_EXT_MEM (unit), ext_mem_port_count);
    SOC_PBMP_COUNT(PBMP_PORT_ALL (unit), valid_port_count);

    all_int_mem_port = (ext_mem_port_count == 0) ? 1 : 0;
    all_ext_mem_port = (ext_mem_port_count >= valid_port_count) ? 1 : 0;

    if ((all_int_mem_port == 0) && (all_ext_mem_port == 0)) {
        int_ext_mem_port = 1;
    }

    /* In lossless mode Katana is cnfigured to accept packets
     * only if sufficient processing resources are guaranteed for all ports.
     * This may decrease overall throughput, 
     * but no accepted packets will be dropped.
     * In Lossy mode Katana is configured to accept the maximum number of
     * packets per port, but may drop them if resources are oversubscribed 
     * due to activity from other ports.
     */

    ka_mmu_params.num_ge_ports = si->ge.num + si->fe.num; /* c18*/
    ka_mmu_params.num_hg_ports = si->hg.num + si->xe.num;/*c24*/

    ka_mmu_params.num_replication_per_pkt =
        ka_mmu_params.num_ge_ports + ka_mmu_params.num_hg_ports - 1; /*c45, c46-SBv7*/

    mmu_egr_queue_dyn_thd_param_lossy = KA_MMU_EGR_QUEUE_DYN_THD_PARAM_LOSSY;

    if (all_int_mem_port) {
        ka_mmu_params.extbuf_used = 0;/*c16*/
        ka_mmu_params.available_intbuf = 100;/*c39*/
        ka_mmu_params.hg_pg_hdrm_intbuf = (lossless) ? 172 : 0; /*(cells)c108 */
        ka_mmu_params.hg_pg_hdrm_ema = 0; /*c109 */
        ka_mmu_params.hg_pg_hdrm_extbuf = 0;/*c110*/
        ka_mmu_params.hg_pg_hdrm_rewqe = (lossless) ? 157: 0;/*(pkts)c111*/

        ka_mmu_params.ge_pg_hdrm_intbuf = (lossless) ? 127 : 0;/*(cells)c118*/
        ka_mmu_params.ge_pg_hdrm_ema = 0;/*c119*/
        ka_mmu_params.ge_pg_hdrm_extbuf = 0;/*c120*/
        ka_mmu_params.ge_pg_hdrm_rewqe = (lossless) ? 114 : 0;/*(pkts)c121*/
    } else if (all_ext_mem_port) {
        ka_mmu_params.extbuf_used = 1;/*c16*/

        if (soc_feature(unit, soc_feature_mmu_reduced_internal_buffer)) {
            /*Saber*/
            ka_mmu_params.available_intbuf = 50; /*c40-SBv7*/
        } else {
            /* Katana */
            ka_mmu_params.available_intbuf = (lossless) ?
                 ((si->ether.num > 8) ? 30 : 25) : 20;/*c39*/
        }
        ka_mmu_params.hg_pg_hdrm_intbuf = (lossless) ? 76 : 0; /*(cells)c108*/
        if (soc_feature(unit, soc_feature_mmu_reduced_internal_buffer)) {
            /*Saber*/
            ka_mmu_params.hg_pg_hdrm_ema = (lossless) ? 76 : 0; /*(cells)c109*/
        } else {
            ka_mmu_params.hg_pg_hdrm_ema = (lossless) ? 172 : 0; /*(cells)c109*/
        }
        ka_mmu_params.hg_pg_hdrm_extbuf = (lossless) ? 159 : 0;/*(cells)c110*/
        ka_mmu_params.hg_pg_hdrm_rewqe= (lossless) ? 157 : 0;/*(pkts)c111*/

        ka_mmu_params.ge_pg_hdrm_intbuf = (lossless) ? 32 : 0; /*(cells)c118*/
        if (soc_feature(unit, soc_feature_mmu_reduced_internal_buffer)) {
            /*Saber*/
            ka_mmu_params.ge_pg_hdrm_ema = (lossless) ? 32 : 0; /*(cells)c119*/
        } else {
            ka_mmu_params.ge_pg_hdrm_ema = (lossless) ? 127 : 0; /*(cells)c119*/
        }
        ka_mmu_params.ge_pg_hdrm_extbuf = (lossless) ? 116 : 0;/*(cells)c120*/
        ka_mmu_params.ge_pg_hdrm_rewqe= (lossless) ? 114 : 0;/*(cells)c121*/
    } else {
        ka_mmu_params.extbuf_used = 1; /*C16*/
        ka_mmu_params.available_intbuf = 70;/*c39*/
        ka_mmu_params.hg_pg_hdrm_intbuf = (lossless) ? 76 : 0;/*(cells)c108*/
        ka_mmu_params.hg_pg_hdrm_ema = (lossless) ? 76 : 0;/*(cells)c109*/
        ka_mmu_params.hg_pg_hdrm_extbuf = (lossless) ? 68 : 0;/*(cells)c110*/
        ka_mmu_params.hg_pg_hdrm_rewqe = (lossless) ? 66 : 0;/*(pkts)c111*/

        ka_mmu_params.ge_pg_hdrm_intbuf = (lossless) ? 32 : 0;/*(cells)118*/
        ka_mmu_params.ge_pg_hdrm_ema = (lossless) ? 32 : 0;/*(cells)c119*/
        ka_mmu_params.ge_pg_hdrm_extbuf = (lossless) ? 25 : 0;/*(cells)120*/
        ka_mmu_params.ge_pg_hdrm_rewqe= (lossless) ? 23 : 0;/*(pkts)120*/
    }
    ka_mmu_params.available_emapool =
        (100 - ka_mmu_params.available_intbuf); /*c40, c41-SBv7*/
    ka_mmu_params.cpu_pg_hdrm_intbuf = (lossless) ? 46 : 0;
    ka_mmu_params.cpu_pg_hdrm_ema = (lossless) ? 46 : 0;
    ka_mmu_params.cpu_pg_hdrm_extbuf = 0;
    ka_mmu_params.cpu_pg_hdrm_rewqe = (lossless) ? 40 : 0;
    ka_mmu_params.cpu_pg_hdrm_eqe = ka_mmu_params.cpu_pg_hdrm_rewqe *
                                    ka_mmu_params.num_replication_per_pkt;

    ka_mmu_params.lb_pg_hdrm_intbuf = (lossless) ? 46 : 0;
    ka_mmu_params.lb_pg_hdrm_ema = (lossless) ? 46 : 0;
    ka_mmu_params.lb_pg_hdrm_extbuf = 0;
    ka_mmu_params.lb_pg_hdrm_rewqe = (lossless) ? 40 : 0;
    ka_mmu_params.lb_pg_hdrm_eqe = ka_mmu_params.lb_pg_hdrm_rewqe *
                                   ka_mmu_params.num_replication_per_pkt;

    /*Intermediate Results calculation */
    ethernet_mtu_cells_intbuf =
                     ((KA_MMU_ETHERNET_MTU_BYTES + KA_MMU_PKT_HEADER_SIZE)/ 
                     KA_MMU_INT_BUF_CELL_SIZE) + 1; /*c71*/

    ethernet_mtu_cells_extbuf =
                     ((KA_MMU_ETHERNET_MTU_BYTES + KA_MMU_PKT_HEADER_SIZE)/ 
                     KA_MMU_EXT_BUF_CELL_SIZE) + 1; /*c72*/

    num_ports = ka_mmu_params.num_ge_ports +
                ka_mmu_params.num_hg_ports +
                KA_MMU_NUM_LB_PORT +
                KA_MMU_NUM_CPU_PORT;  /*c74 data ports+cpu+loopback*/

    num_data_ports = ka_mmu_params.num_ge_ports +
                     ka_mmu_params.num_hg_ports;   /*c75 data ports only */

    if (SOC_PBMP_MEMBER (PBMP_EXT_MEM (unit), CMIC_PORT(unit))) {
        total_queue_extbuf_bound += KA_MMU_NUM_CPU_PORT *
                                    KA_MMU_NUM_CPU_QUEUE; /*c81*/
    } else {
        total_queue_intbuf_bound += KA_MMU_NUM_CPU_PORT *
                                    KA_MMU_NUM_CPU_QUEUE; /*c81*/
    }

    if (SOC_PBMP_MEMBER (PBMP_EXT_MEM (unit), LB_PORT(unit))) {
        total_queue_extbuf_bound += KA_MMU_NUM_LB_PORT *
                                    KA_MMU_NUM_LB_QUEUE;/*c82*/
    } else {
        total_queue_intbuf_bound += KA_MMU_NUM_LB_PORT *
                                    KA_MMU_NUM_LB_QUEUE;/*c82*/
    }

    SOC_PBMP_CLEAR (temp_pbmp);
    SOC_PBMP_ASSIGN (temp_pbmp, si->ge.bitmap);
    SOC_PBMP_OR (temp_pbmp, si->fe.bitmap);
    SOC_PBMP_OR (temp_pbmp, si->xe.bitmap);
    SOC_PBMP_OR (temp_pbmp, si->hg.bitmap);
    
    SOC_PBMP_ITER (temp_pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            if (SOC_PBMP_MEMBER (PBMP_EXT_MEM (unit), port)) {
                total_queue_extbuf_bound += si->port_num_uc_cosq[port]; /*c84*/
                if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
                    num_eqe_hgports_extbuf_bound += si->port_num_uc_cosq[port];/*c80*/
                } else {
                    num_eqe_geports_extbuf_bound += si->port_num_uc_cosq[port];/*c78*/
                }
            } else {
                total_queue_intbuf_bound += si->port_num_uc_cosq[port]; /*c85*/
                if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
                    num_eqe_hgports_intbuf_bound += si->port_num_uc_cosq[port];/*c79*/
                } else {
                    num_eqe_geports_intbuf_bound += si->port_num_uc_cosq[port];/*c77*/
                }
            }
        }
    }

    num_ema_queue = KA_MMU_EMA_QUEUES; /*c85*/
    total_eqe_device = KA_MMU_NUM_EGRESS_QUEUE_IN_DEVICE; /*c87*/

    if (soc_feature(unit, soc_feature_mmu_reduced_internal_buffer)) {
        /* Saber */
        max_intbuf_size = ((SB_MMU_INT_BUF_SIZE -
                           KA_MMU_RESERVED_INT_BUF_SPACE_CFAP) * 1024) /
                          KA_MMU_INT_BUF_CELL_SIZE; /*c88*/
        intbuf_pool_size = ((ka_mmu_params.available_intbuf *
                            max_intbuf_size) / 100) -
                            SB_MMU_CFAP_THDIO_LATENCY; /*c90*/
        emapool_size = ka_mmu_params.available_emapool ?
                       (((ka_mmu_params.available_emapool *
                       max_intbuf_size) / 100) -
                       SB_MMU_CFAP_THDIO_LATENCY) : 0; /*c91*/
        extbuf_size = ka_mmu_params.extbuf_used ?
                      (1024 * 1024 * saber_mmu_ext_buf_size /
                      KA_MMU_EXT_BUF_CELL_SIZE - 1 -
                      KA_MMU_RESERVED_EXT_BUF_SPACE_CFAP -
                      SB_MMU_CFAP_THDIO_LATENCY) : 0; /*c92*/
    } else {
        /* Katana */
        max_intbuf_size = ((KA_MMU_INT_BUF_SIZE -
                           KA_MMU_RESERVED_INT_BUF_SPACE_CFAP)* 1024) /
                          KA_MMU_INT_BUF_CELL_SIZE; /*c88*/
        intbuf_pool_size =
            (ka_mmu_params.available_intbuf * max_intbuf_size) / 100; /*c90*/
        emapool_size = ka_mmu_params.available_intbuf ?
            ((ka_mmu_params.available_emapool * max_intbuf_size) / 100) : 0; /*c91*/
        extbuf_size = ka_mmu_params.extbuf_used ?
                      (1024 * 1024 * KA_MMU_EXT_BUF_SIZE /
                      KA_MMU_EXT_BUF_CELL_SIZE - 1 -
                      KA_MMU_RESERVED_EXT_BUF_SPACE_CFAP) : 0; /*c92*/
    }

    rewqe_entries_admission = KA_MMU_RE_WORK_QUEUE_ENTRIES * 1024 -
                              KA_MMU_RESERVED_RE_WORK_QUEUE_ENTRIES; /*c93 */

    /* Input Port threshold calculations */
    ipthd_global_hdrm_intbuf = (20 * 1024) /
                               KA_MMU_INT_BUF_CELL_SIZE + 1; /*c103*/
    ipthd_global_hdrm_ema = (ka_mmu_params.extbuf_used) ?
                            ipthd_global_hdrm_intbuf : 0; /*c104*/
    ipthd_global_hdrm_extbuf = (ka_mmu_params.extbuf_used) ?
                               (ipthd_global_hdrm_intbuf /
                               KA_MMU_EXT_BUF_TO_INT_BUF_RATIO + 1) : 0;/*c105*/
    ipthd_global_hdrm_rewqe = 0; /*c106*/
    ipthd_global_hdrm_eqe = 0; /* c107 v7 */

    ipthd_hg_pg_hdrm_eqe = ka_mmu_params.num_replication_per_pkt *
                           ka_mmu_params.hg_pg_hdrm_rewqe;/*c112*/

    ipthd_hg_total_hdrm_intbuf = KA_MMU_LOSSLESS_PG_NUM * 
                                ka_mmu_params.num_hg_ports *
                                ka_mmu_params.hg_pg_hdrm_intbuf;/*c113*/
    ipthd_hg_total_hdrm_ema = KA_MMU_LOSSLESS_PG_NUM * 
                              ka_mmu_params.num_hg_ports *
                              ka_mmu_params.hg_pg_hdrm_ema;/*c114*/
    ipthd_hg_total_hdrm_extbuf = KA_MMU_LOSSLESS_PG_NUM * 
                                ka_mmu_params.num_hg_ports *
                                ka_mmu_params.hg_pg_hdrm_extbuf;/*c115*/
    ipthd_hg_total_hdrm_rewqe = KA_MMU_LOSSLESS_PG_NUM * 
                                ka_mmu_params.num_hg_ports *
                                ka_mmu_params.hg_pg_hdrm_rewqe;/*c116*/
    ipthd_hg_total_hdrm_eqe = KA_MMU_LOSSLESS_PG_NUM * 
                              ka_mmu_params.num_hg_ports *
                              ipthd_hg_pg_hdrm_eqe;/*c117*/

    ipthd_ge_pg_hdrm_eqe = ka_mmu_params.num_replication_per_pkt *
                           ka_mmu_params.ge_pg_hdrm_rewqe;/*c122*/

    ipthd_ge_total_hdrm_intbuf = KA_MMU_LOSSLESS_PG_NUM * 
                                ka_mmu_params.num_ge_ports *
                                ka_mmu_params.ge_pg_hdrm_intbuf;/*c123*/
    ipthd_ge_total_hdrm_ema = KA_MMU_LOSSLESS_PG_NUM * 
                              ka_mmu_params.num_ge_ports *
                              ka_mmu_params.ge_pg_hdrm_ema;/*c124*/
    ipthd_ge_total_hdrm_extbuf = KA_MMU_LOSSLESS_PG_NUM * 
                                ka_mmu_params.num_ge_ports *
                                ka_mmu_params.ge_pg_hdrm_extbuf;/*c125*/
    ipthd_ge_total_hdrm_rewqe = KA_MMU_LOSSLESS_PG_NUM * 
                                ka_mmu_params.num_ge_ports *
                                ka_mmu_params.ge_pg_hdrm_rewqe;/*c126*/
    ipthd_ge_total_hdrm_eqe = KA_MMU_LOSSLESS_PG_NUM * 
                                  ka_mmu_params.num_ge_ports *
                                  ipthd_ge_pg_hdrm_eqe;/*c127*/

    ipthd_total_hdrm_intbuf = ipthd_global_hdrm_intbuf +
                             ipthd_hg_total_hdrm_intbuf +
                             ipthd_ge_total_hdrm_intbuf +
                             ka_mmu_params.cpu_pg_hdrm_intbuf +
                             ka_mmu_params.lb_pg_hdrm_intbuf;/*c138*/
    ipthd_total_hdrm_ema = ipthd_global_hdrm_ema +
                             ipthd_hg_total_hdrm_ema +
                             ipthd_ge_total_hdrm_ema +
                             ka_mmu_params.cpu_pg_hdrm_ema +
                             ka_mmu_params.lb_pg_hdrm_ema;/*c139*/
    ipthd_total_hdrm_extbuf = ipthd_global_hdrm_extbuf +
                             ipthd_hg_total_hdrm_extbuf +
                             ipthd_ge_total_hdrm_extbuf +
                             ka_mmu_params.cpu_pg_hdrm_extbuf +
                             ka_mmu_params.lb_pg_hdrm_extbuf;/*c140*/
    ipthd_total_hdrm_rewqe = ipthd_global_hdrm_rewqe+
                             ipthd_hg_total_hdrm_rewqe+
                             ipthd_ge_total_hdrm_rewqe +
                             ka_mmu_params.cpu_pg_hdrm_rewqe +
                             ka_mmu_params.lb_pg_hdrm_rewqe;/*c141*/
    ipthd_total_hdrm_eqe = ipthd_global_hdrm_eqe +
                             ipthd_hg_total_hdrm_eqe +
                             ipthd_ge_total_hdrm_eqe +
                             ka_mmu_params.cpu_pg_hdrm_eqe +
                             ka_mmu_params.lb_pg_hdrm_eqe;/*c142*/

    ipthd_per_pg_min_intbuf_ingpool = (all_ext_mem_port && lossless) ?
                                      ethernet_mtu_cells_intbuf :
                                      KA_MMU_JUMBO_FRAME_INT_BUF; /*c144*/

    ipthd_per_pg_min_intbuf_ingpool_pg_0_6 = 1; /*c145*/

    ipthd_per_pg_min_intbuf_emapool = ka_mmu_params.extbuf_used ?
                                      KA_MMU_JUMBO_FRAME_INT_BUF : 0;/*c146*/
    ipthd_per_pg_min_extbuf = ka_mmu_params.extbuf_used ?
                              ((KA_MMU_JUMBO_FRAME_INT_BUF /
                              KA_MMU_EXT_BUF_TO_INT_BUF_RATIO) + 1) : 0;/*c147*/

    ipthd_per_pg_min_rewqe = KA_MMU_JUMBO_FRAME_INT_BUF / 
                             KA_MMU_CELL_PER_AVG_PKT_INT_BUF;/*c148*/

    if (soc_feature(unit, soc_feature_mmu_reduced_internal_buffer) &&
        (ka_mmu_params.extbuf_used)) {
        ipthd_per_pg_min_intbuf_ingpool = ethernet_mtu_cells_intbuf;/*c144*/
        ipthd_per_pg_min_intbuf_emapool = ipthd_per_pg_min_intbuf_ingpool;
        if (!all_ext_mem_port) {
            ipthd_per_pg_min_extbuf = (ipthd_per_pg_min_intbuf_ingpool /
                                       KA_MMU_EXT_BUF_TO_INT_BUF_RATIO) + 1;
            ipthd_per_pg_min_rewqe = ipthd_per_pg_min_intbuf_ingpool;
        }
    }

    ipthd_per_pg_min_eqe = ipthd_per_pg_min_rewqe *
                           ka_mmu_params.num_replication_per_pkt;/*c149*/

    ipthd_total_pg_min_intbuf_ingpool = KA_MMU_LOSSLESS_PG_NUM * num_ports *
                                    ipthd_per_pg_min_intbuf_ingpool +
                                    ipthd_per_pg_min_intbuf_ingpool_pg_0_6 *
                                    ka_mmu_params.num_hg_ports * 7;/*c151*/
    ipthd_total_pg_min_intbuf_emapool = KA_MMU_LOSSLESS_PG_NUM * num_ports *
                                    ipthd_per_pg_min_intbuf_emapool;/*c152*/
    ipthd_total_pg_min_extbuf = KA_MMU_LOSSLESS_PG_NUM * num_ports *
                                    ipthd_per_pg_min_extbuf;/*c153*/
    ipthd_total_pg_min_rewqe = KA_MMU_LOSSLESS_PG_NUM * num_ports *
                                    ipthd_per_pg_min_rewqe;/*c154*/
    ipthd_total_pg_min_eqe = KA_MMU_LOSSLESS_PG_NUM * num_ports *
                                    ipthd_per_pg_min_eqe;/*c155*/

    ipthd_per_port_min_intbuf_ingpool = 0;/*c157*/
    ipthd_per_port_min_intbuf_emapool = 0;/*c158*/
    ipthd_per_port_min_extbuf = 0;/*c159*/
    ipthd_per_port_min_rewqe = 0;/*c160*/
    ipthd_per_port_min_eqe = 0;/*c161*/

    ipthd_total_port_min_intbuf_ingpool =
                   num_ports * ipthd_per_port_min_intbuf_ingpool;/*163*/
    ipthd_total_port_min_intbuf_emapool =
                   num_ports * ipthd_per_port_min_intbuf_emapool;/*164*/
    ipthd_total_port_min_extbuf = num_ports * ipthd_per_port_min_extbuf;/*165*/
    ipthd_total_port_min_rewqe = num_ports * ipthd_per_port_min_rewqe;/*166*/
    ipthd_total_port_min_eqe = num_ports * ipthd_per_port_min_eqe;/*167*/

    ipthd_total_min_intbuf_ingpool = ipthd_total_pg_min_intbuf_ingpool +
                                 ipthd_total_port_min_intbuf_ingpool;/*c169*/
    ipthd_total_min_intbuf_emapool = ipthd_total_pg_min_intbuf_emapool+
                                 ipthd_total_port_min_intbuf_emapool;/*c170*/
    ipthd_total_min_extbuf = ipthd_total_pg_min_extbuf +
                             ipthd_total_port_min_extbuf;/*c171*/
    ipthd_total_min_rewqe = ipthd_total_pg_min_rewqe+
                                 ipthd_total_port_min_rewqe;/*c172*/
    ipthd_total_min_eqe = ipthd_total_pg_min_eqe + ipthd_total_port_min_eqe;/*173*/

    /* Output port thresholds calculations*/
    opthd_port_min_grntee_queue_intbuf = lossless ?
                                         0 : ethernet_mtu_cells_intbuf;/*c193*/

    opthd_port_min_grntee_queue_extbuf = lossless ? 0 :
        (ka_mmu_params.extbuf_used ? ethernet_mtu_cells_extbuf : 0);/*c194*/

    opthd_port_min_grntee_eqe = lossless ? 0 :
                                ethernet_mtu_cells_intbuf /
                                KA_MMU_CELL_PER_AVG_PKT_EXT_BUF;/*c195*/

    opthd_port_min_grntee_ema_queue =  lossless ? 0 :
        (ka_mmu_params.extbuf_used ? ethernet_mtu_cells_intbuf : 0);/*c196*/

    opthd_port_min_grntee_rewqe = opthd_port_min_grntee_queue_intbuf;/*197*/
    opthd_port_min_grntee_rewqe_intbuf_mc = lossless ? 0: 150;/*198*/
    opthd_port_min_grntee_rewqe_intbuf_tocpu= lossless ? 0: 50;/*199*/
    opthd_port_min_grntee_rewqe_intbuf_mirror= lossless ? 0: 50;/*200*/
    opthd_port_min_grntee_rewqe_extbuf = lossless ? 0 :
                           (ka_mmu_params.extbuf_used ?
                           num_ports * KA_MMU_JUMBO_FRAME_EXT_BUF : 0);/*201*/

    opthd_total_min_grntee_queue_intbuff = total_queue_intbuf_bound *
                                  opthd_port_min_grntee_queue_intbuf; /*204*/
    opthd_total_min_grntee_queue_extbuff = total_eqe_device *
                                  opthd_port_min_grntee_queue_extbuf;/*205*/
    opthd_total_min_grntee_eqe = total_eqe_device *
                                 opthd_port_min_grntee_eqe; /*206*/
    opthd_total_min_grntee_ema_queue = KA_MMU_EMA_QUEUES *
                                 opthd_port_min_grntee_ema_queue;/*207*/
    opthd_total_min_grntee_rewqe = KA_MMU_RE_WORK_QUEUES_IN_DEVICE *
                                 opthd_port_min_grntee_rewqe;/*208*/
    opthd_total_min_grntee_rewqe_intbuf =
                             opthd_port_min_grntee_rewqe_intbuf_mc +
                             opthd_port_min_grntee_rewqe_intbuf_tocpu+
                             opthd_port_min_grntee_rewqe_intbuf_mirror;/*209*/
    opthd_total_min_grntee_rewqe_extbuf =
                                 KA_MMU_RE_WORK_QUEUES_IN_DEVICE *
                                 opthd_port_min_grntee_rewqe_extbuf;/*210*/

    /* Input threshold total shared calculations*/
    ipthd_total_shared_intbuf_ingpool = intbuf_pool_size -
                                    ipthd_total_hdrm_intbuf -
                                    ipthd_total_min_intbuf_ingpool -
                                    (lossless ?
                                        opthd_total_min_grntee_queue_intbuff :
                                        0);/*c175*/
    ipthd_total_shared_intbuf_emapool = ka_mmu_params.extbuf_used ?
                                    (emapool_size - ipthd_total_hdrm_ema -
                                    ipthd_total_min_intbuf_emapool -
                                    (lossless ? 
                                        opthd_total_min_grntee_ema_queue :
                                        0)) : 0;/*c176*/
    ipthd_total_shared_extbuf = extbuf_size - 
                          ipthd_total_min_extbuf -
                          ipthd_total_hdrm_extbuf -
                          (lossless ?
                              opthd_total_min_grntee_queue_extbuff : 0);/*177*/
    ipthd_total_shared_rewqe = rewqe_entries_admission -
                          ipthd_total_hdrm_rewqe - 
                          ipthd_total_min_rewqe -
                          (lossless ? opthd_total_min_grntee_rewqe : 0);/*178*/
    ipthd_total_shared_eqe = mmu_egress_queue_entries * 1024 -
                          ipthd_total_hdrm_eqe - 
                          ipthd_total_min_eqe -
                          (lossless ? opthd_total_min_grntee_eqe : 0);/*179*/

    /* Output port thresholds calculations .. continued*/
    opthd_total_shared_queue_intbuff = intbuf_pool_size -
                      opthd_total_min_grntee_queue_intbuff -
                      opthd_total_min_grntee_rewqe_intbuf;/*c213*/
    opthd_total_shared_queue_extbuff = extbuf_size - 
                          opthd_total_min_grntee_queue_extbuff -
                          opthd_total_min_grntee_rewqe_extbuf;/*c214*/
    opthd_total_shared_queue_eqe = mmu_egress_queue_entries * 
                                    1024 - opthd_total_min_grntee_eqe;/*c215*/
    opthd_total_shared_queue_ema_queue = emapool_size -
                                     opthd_total_min_grntee_ema_queue;/*c216*/
    opthd_total_shared_queue_rewqe = rewqe_entries_admission -
                                     opthd_total_min_grntee_rewqe;/*c217*/
    opthd_total_shared_queue_rewqe_intbuf = intbuf_pool_size -
                          opthd_total_min_grntee_queue_intbuff -
                          opthd_total_min_grntee_rewqe_intbuf;/*c218*/
    opthd_total_shared_queue_rewqe_extbuf = extbuf_size -
                          opthd_total_min_grntee_queue_extbuff -
                          opthd_total_min_grntee_rewqe_extbuf;/*c219*/

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "\npbmp all count =%d  pbmp ext mem count =%d"),
                 valid_port_count, ext_mem_port_count));
        LOG_CLI((BSL_META_U(unit,
                            "\n Katana MMU config:= %s : %s"), 
                 ((all_int_mem_port) ? "all internal mem" :
                 ((all_ext_mem_port) ? "all external mem" :
                 "both internal and external mem")),
                 ((lossless) ? "lossless" : "lossy")));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_ge_ports %d"),
                 ka_mmu_params.num_ge_ports));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_hg_ports %d"),
                 ka_mmu_params.num_hg_ports));
        LOG_CLI((BSL_META_U(unit,
                            "\navailable_intbuf %d"),
                 ka_mmu_params.available_intbuf));

        LOG_CLI((BSL_META_U(unit,
                            "\nmax_pkt_size %d"),
                 KA_MMU_MAX_PKT_SIZE_IN_CELLS));
        LOG_CLI((BSL_META_U(unit,
                            "\njumbo_frame_intbuf %d"),
                 KA_MMU_JUMBO_FRAME_INT_BUF));
        LOG_CLI((BSL_META_U(unit,
                            "\njumbo_frame_extbuf %d"),
                 KA_MMU_JUMBO_FRAME_EXT_BUF));
        LOG_CLI((BSL_META_U(unit,
                            "\nethernet_mtu_cells_intbuf %d"),
                 ethernet_mtu_cells_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nethernet_mtu_cells_extbuf %d"),
                 ethernet_mtu_cells_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_ports %d"), num_ports));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_data_ports %d"), num_data_ports));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_eqe_geports_intbuf_bound %d"),
                 num_eqe_geports_intbuf_bound));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_eqe_geports_extbuf_bound %d"),
                 num_eqe_geports_extbuf_bound));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_eqe_hgports_intbuf_bound %d"),
                 num_eqe_hgports_intbuf_bound));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_eqe_hgports_extbuf_bound %d"),
                 num_eqe_hgports_extbuf_bound));

        LOG_CLI((BSL_META_U(unit,
                            "\nnum_cpu_queue %d"),
                 KA_MMU_NUM_CPU_PORT * KA_MMU_NUM_CPU_QUEUE));
        LOG_CLI((BSL_META_U(unit,
                            "\ntotal_queue_intbuf_bound %d"),
                 total_queue_intbuf_bound));
        LOG_CLI((BSL_META_U(unit,
                            "\ntotal_queue_extbuf_bound %d"),
                 total_queue_extbuf_bound));
        LOG_CLI((BSL_META_U(unit,
                            "\nnum_ema_queue %d"), num_ema_queue));
        LOG_CLI((BSL_META_U(unit,
                            "\ntotal_eqe_device %d"),
                 total_eqe_device));
        LOG_CLI((BSL_META_U(unit,
                            "\nmax_intbuf_size %d"),
                 max_intbuf_size));

        LOG_CLI((BSL_META_U(unit,
                            "\nintbuf_pool_size %d"),
                 intbuf_pool_size));
        LOG_CLI((BSL_META_U(unit,
                            "\nemapool_size %d"), emapool_size));
        LOG_CLI((BSL_META_U(unit,
                            "\nextbuf_size %d"), extbuf_size));

        LOG_CLI((BSL_META_U(unit,
                            "\nglobal_hdrm_intbuf %d"),
                 ipthd_global_hdrm_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nglobal_hdrm_extbuf %d"),
                 ipthd_global_hdrm_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nglobal_hdrm_rewqe %d"),
                 ipthd_global_hdrm_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nglobal_hdrm_eqe %d"),
                 ipthd_global_hdrm_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_pg_hdrm_intbuf %d"),
                 ka_mmu_params.hg_pg_hdrm_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_pg_hdrm_extbuf %d"),
                 ka_mmu_params.hg_pg_hdrm_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_pg_hdrm_rewqe %d"),
                 ka_mmu_params.hg_pg_hdrm_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_pg_hdrm_eqe %d"),
                 ipthd_hg_pg_hdrm_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_ipthd_total_hdrm_intbuf %d"),
                 ipthd_hg_total_hdrm_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_ipthd_total_hdrm_extbuf %d"),
                 ipthd_hg_total_hdrm_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_ipthd_total_hdrm_rewqe %d"),
                 ipthd_hg_total_hdrm_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nhg_ipthd_total_hdrm_eqe %d"),
                 ipthd_hg_total_hdrm_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nge_pg_hdrm_intbuf %d"),
                 ka_mmu_params.ge_pg_hdrm_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nge_pg_hdrm_extbuf %d"),
                 ka_mmu_params.ge_pg_hdrm_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nge_pg_hdrm_rewqe %d"),
                 ka_mmu_params.ge_pg_hdrm_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_ge_pg_hdrm_eqe %d"),
                 ipthd_ge_pg_hdrm_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_ge_total_hdrm_intbuf %d"),
                 ipthd_ge_total_hdrm_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_ge_total_hdrm_extbuf %d"),
                 ipthd_ge_total_hdrm_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_ge_total_hdrm_rewqe %d"),
                 ipthd_ge_total_hdrm_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_ge_total_hdrm_eqe %d"),
                 ipthd_ge_total_hdrm_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_hdrm_intbuf %d"),
                 ipthd_total_hdrm_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_hdrm_ema %d"),
                 ipthd_total_hdrm_ema));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_hdrm_extbuf %d"),
                 ipthd_total_hdrm_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_hdrm_rewqe %d"),
                 ipthd_total_hdrm_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_hdrm_eqe %d"),
                 ipthd_total_hdrm_eqe));

        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_pg_min_intbuf_ingpool %d"),
                 ipthd_per_pg_min_intbuf_ingpool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_pg_min_intbuf_emapool %d"),
                 ipthd_per_pg_min_intbuf_emapool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_pg_min_extbuf %d"),
                 ipthd_per_pg_min_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_pg_min_rewqe %d"),
                 ipthd_per_pg_min_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_pg_min_eqe %d"),
                 ipthd_per_pg_min_eqe));

        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_pg_min_intbuf_ingpool %d"),
                 ipthd_total_pg_min_intbuf_ingpool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_pg_min_intbuf_emapool %d"),
                 ipthd_total_pg_min_intbuf_emapool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_pg_min_extbuf %d"),
                 ipthd_total_pg_min_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_pg_min_rewqe %d"),
                 ipthd_total_pg_min_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_pg_min_eqe %d"),
                 ipthd_total_pg_min_eqe));

        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_port_min_intbuf_ingpool %d"),
                 ipthd_per_port_min_intbuf_ingpool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_port_min_intbuf_emapool %d"),
                 ipthd_per_port_min_intbuf_emapool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_port_min_extbuf %d"),
                 ipthd_per_port_min_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_port_min_rewqe %d"),
                 ipthd_per_port_min_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_per_port_min_eqe %d"),
                 ipthd_per_port_min_eqe));

        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_port_min_intbuf_ingpool %d"),
                 ipthd_total_port_min_intbuf_ingpool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_port_min_intbuf_emapool %d"),
                 ipthd_total_port_min_intbuf_emapool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_port_min_extbuf %d"),
                 ipthd_total_port_min_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_port_min_rewqe %d"),
                 ipthd_total_port_min_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_port_min_eqe %d"),
                 ipthd_total_port_min_eqe));

        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_min_intbuf_ingpool %d"),
                 ipthd_total_min_intbuf_ingpool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_min_intbuf_emapool %d"),
                 ipthd_total_min_intbuf_emapool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_min_extbuf %d"),
                 ipthd_total_min_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_min_rewqe %d"),
                 ipthd_total_min_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_min_eqe %d"),
                 ipthd_total_min_eqe));

        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_shared_intbuf_ingpool %d"),
                 ipthd_total_shared_intbuf_ingpool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_shared_intbuf_emapool %d"),
                 ipthd_total_shared_intbuf_emapool));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_shared_extbuf %d"),
                 ipthd_total_shared_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_shared_rewqe %d"),
                 ipthd_total_shared_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nipthd_total_shared_eqe %d"),
                 ipthd_total_shared_eqe));

        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_queue_intbuf %d"),
                 opthd_port_min_grntee_queue_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_queue_extbuf %d"),
                 opthd_port_min_grntee_queue_extbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_eqe %d"),
                 opthd_port_min_grntee_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_ema_queue %d"),
                 opthd_port_min_grntee_ema_queue));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_rewqe %d"),
                 opthd_port_min_grntee_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_rewqe_intbuf_mc %d"),
                 opthd_port_min_grntee_rewqe_intbuf_mc));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_rewqe_intbuf_topcu %d"),
                 opthd_port_min_grntee_rewqe_intbuf_tocpu));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_rewqe_intbuf_mirror %d"),
                 opthd_port_min_grntee_rewqe_intbuf_mirror));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_port_min_grntee_rewqe_extbuf %d"),
                 opthd_port_min_grntee_rewqe_extbuf));

        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_min_grntee_queue_intbuff %d"),
                 opthd_total_min_grntee_queue_intbuff));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_min_grntee_queue_extbuff %d"),
                 opthd_total_min_grntee_queue_extbuff));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_min_grntee_eqe %d"),
                 opthd_total_min_grntee_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_min_grntee_ema_queue %d"),
                 opthd_total_min_grntee_ema_queue));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_min_grntee_rewqe %d"),
                 opthd_total_min_grntee_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_min_grntee_rewqe_intbuf %d"),
                 opthd_total_min_grntee_rewqe_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_min_grntee_rewqe_extbuf %d"),
                 opthd_total_min_grntee_rewqe_extbuf));

        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_shared_queue_intbuff %d"),
                 opthd_total_shared_queue_intbuff));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_shared_queue_extbuff %d"),
                 opthd_total_shared_queue_extbuff));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_shared_queue_eqe %d"),
                 opthd_total_shared_queue_eqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_shared_queue_ema_queue %d"),
                 opthd_total_shared_queue_ema_queue));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_shared_queue_rewqe %d"),
                 opthd_total_shared_queue_rewqe));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_shared_queue_rewqe_intbuf %d"),
                 opthd_total_shared_queue_rewqe_intbuf));
        LOG_CLI((BSL_META_U(unit,
                            "\nopthd_total_shared_queue_rewqe_extbuf %d\n"),
                 opthd_total_shared_queue_rewqe_extbuf));
    }

    /* Input port thresholds register/table setting*/
    rval = 0;
    soc_reg_field_set(unit, COLOR_AWAREr, &rval, ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_COLOR_AWAREr (unit, rval));

    /* Internal Buffer Ingress Pool */
    rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, GLOBAL_HDRM_LIMITf,
                      ipthd_global_hdrm_intbuf);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr (unit, rval));
    rval = 0;
    soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                      ipthd_total_shared_intbuf_ingpool);
    SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SPr (unit, 0, rval));
    rval = 0;
    soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                OFFSETf, (num_ports / 4 * ethernet_mtu_cells_intbuf));
    SOC_IF_ERROR_RETURN(WRITE_CELL_RESET_LIMIT_OFFSET_SPr (unit, 0, rval));

    if (!all_int_mem_port) { /* not applicable for all internal mem port */
    /* External Buffer */
        rval = 0;
        soc_reg_field_set(unit, THDIEXT_GLOBAL_HDRM_LIMITr, &rval,
                      GLOBAL_HDRM_LIMITf, ipthd_global_hdrm_extbuf);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_GLOBAL_HDRM_LIMITr (unit, rval));
        rval = 0;
        soc_reg_field_set(unit, THDIEXT_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                          ipthd_total_shared_extbuf );
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_BUFFER_CELL_LIMIT_SPr (unit, 0,rval));
        rval = 0;
        soc_reg_field_set(unit, THDIEXT_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                OFFSETf, (num_ports /4 * ethernet_mtu_cells_extbuf));
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_CELL_RESET_LIMIT_OFFSET_SPr (unit, 0, rval));

    /* EMA pool */
        rval = 0;
        soc_reg_field_set(unit, THDIEMA_GLOBAL_HDRM_LIMITr, &rval,
                          GLOBAL_HDRM_LIMITf, ipthd_global_hdrm_ema);/*c253*/
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_GLOBAL_HDRM_LIMITr (unit, rval));
        rval = 0;
        soc_reg_field_set(unit, THDIEMA_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                          ipthd_total_shared_intbuf_emapool);/*c254*/
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_BUFFER_CELL_LIMIT_SPr (unit, 0,rval));
        rval = 0;
        soc_reg_field_set(unit, THDIEMA_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                    OFFSETf, (num_ports /4 * ethernet_mtu_cells_intbuf));/*c255*/
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEMA_CELL_RESET_LIMIT_OFFSET_SPr (unit, 0, rval));
    }

    /* RE WQEs */
    rval = 0;
    soc_reg_field_set(unit, THDIRQE_GLOBAL_HDRM_LIMITr, &rval,
                      GLOBAL_HDRM_LIMITf, ipthd_global_hdrm_rewqe);/*c257*/
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_GLOBAL_HDRM_LIMITr (unit, rval));
    rval = 0;
    soc_reg_field_set(unit, THDIRQE_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                      ipthd_total_shared_rewqe);/*c258*/
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_BUFFER_CELL_LIMIT_SPr (unit, 0, rval));
    rval = 0;
    soc_reg_field_set(unit, THDIRQE_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                      OFFSETf, (num_ports / 4));/*c259*/
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_CELL_RESET_LIMIT_OFFSET_SPr(unit,0,rval));

    /* EQEs */
    rval = 0;
    soc_reg_field_set(unit, THDIQEN_GLOBAL_HDRM_LIMITr, &rval,
                      GLOBAL_HDRM_LIMITf, ipthd_global_hdrm_eqe);/*c261*/
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_GLOBAL_HDRM_LIMITr (unit, rval));
    rval = 0;
    soc_reg_field_set(unit, THDIQEN_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                      ipthd_total_shared_eqe);/*c262*/
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_BUFFER_CELL_LIMIT_SPr (unit, 0, rval));
    rval = 0;
    soc_reg_field_set(unit, THDIQEN_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
        OFFSETf, (num_ports / 4 * ka_mmu_params.num_replication_per_pkt));/*263*/
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_CELL_RESET_LIMIT_OFFSET_SPr(unit,0,rval));

    /* Ouput port thresholds setting*/

        /* Internal buffer Egress pool */
    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLIr, &rval,
              OP_BUFFER_SHARED_LIMIT_CELLIf, opthd_total_shared_queue_intbuff);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLIr (unit, rval));/*c268*/
    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLIr, &rval,
          OP_BUFFER_LIMIT_YELLOW_CELLIf, opthd_total_shared_queue_intbuff / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLIr (unit, rval));/*c270*/
    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLIr, &rval,
              OP_BUFFER_LIMIT_RED_CELLIf, opthd_total_shared_queue_intbuff / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLIr (unit, rval));/*272*/
    rval = 0;
    if ((lossless == 0) && (all_int_mem_port ||
                            (int_ext_mem_port && (si->fe.num == 0)))) {
        /*all_internal lossy and int-ext lossy  c213-448*/
        val = opthd_total_shared_queue_intbuff - 448;
    } else {
        val = opthd_total_shared_queue_intbuff - 
            (num_ports / 4 * ethernet_mtu_cells_intbuf);
    }
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr, &rval,
                      OP_BUFFER_SHARED_LIMIT_RESUME_CELLIf, val);/*c269*/
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr (unit,rval));
    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr, &rval,
                           OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIf, val / 8);/*c271*/
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLIr, &rval,
                           OP_BUFFER_LIMIT_RESUME_RED_CELLIf, val / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_CELLIr(unit, rval));/*c273*/

    if (!all_int_mem_port) { /* not applicable for all internal mem port */
    /* Ouput port thresholds */
        /* External Buffer */
        rval = 0;/*c275*/
        soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLEr, &rval,
              OP_BUFFER_SHARED_LIMIT_CELLEf, opthd_total_shared_queue_extbuff);
        SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLEr (unit, rval));
        rval = 0;/*c277*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLEr, &rval,
          OP_BUFFER_LIMIT_YELLOW_CELLEf, opthd_total_shared_queue_extbuff / 8);
        SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLEr(unit, rval));
        rval = 0;/*c279*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLEr, &rval,
              OP_BUFFER_LIMIT_RED_CELLEf, opthd_total_shared_queue_extbuff / 8);
        SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLEr(unit, rval));
        rval = 0;/*c276*/
        val = opthd_total_shared_queue_extbuff - 
                 (num_ports /4 * ethernet_mtu_cells_extbuf);
        soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLEr, &rval,
                          OP_BUFFER_SHARED_LIMIT_RESUME_CELLEf, val);
        SOC_IF_ERROR_RETURN(
                     WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLEr(unit, rval));
        rval = 0;/*c278*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEr, &rval,
                          OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEf, val / 8);
        SOC_IF_ERROR_RETURN(
                    WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEr(unit, rval));
        rval = 0;/*c280*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLEr, &rval,
                          OP_BUFFER_LIMIT_RESUME_RED_CELLEf, val / 8);
        SOC_IF_ERROR_RETURN(
                    WRITE_OP_BUFFER_LIMIT_RESUME_RED_CELLEr(unit, rval));

    /* Ouput port thresholds */
        /* EMA pool */
        rval = 0;/*c282*/
        soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDOEMAr, &rval,
                  OP_BUFFER_SHARED_LIMITf, opthd_total_shared_queue_ema_queue);
        SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDOEMAr (unit, rval));
        rval = 0;/*c284*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_THDOEMAr, &rval,
               OP_BUFFER_LIMIT_YELLOWf, opthd_total_shared_queue_ema_queue / 8);
        SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_THDOEMAr(unit, rval));
        rval = 0;/*c286*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_THDOEMAr, &rval,
               OP_BUFFER_LIMIT_REDf, opthd_total_shared_queue_ema_queue / 8);
        SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_THDOEMAr(unit, rval));
        rval = 0;/*c283*/
        val = opthd_total_shared_queue_ema_queue - 
                 (num_ports /4 * ethernet_mtu_cells_intbuf);
        soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr, &rval,
                         OP_BUFFER_SHARED_LIMIT_RESUMEf, val);
        SOC_IF_ERROR_RETURN(
                     WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr (unit, rval));
        rval = 0;/*c285*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr, &rval,
                     OP_BUFFER_LIMIT_RESUME_YELLOWf, val / 8);
        SOC_IF_ERROR_RETURN(
                    WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr(unit, rval));
        rval = 0;/*c287*/
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr, &rval,
                     OP_BUFFER_LIMIT_RESUME_REDf, val / 8);
        SOC_IF_ERROR_RETURN(
                    WRITE_OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr(unit, rval));
    }

    /* Ouput port thresholds */
        /* RE WQEs*/
    if (opthd_total_shared_queue_rewqe > 8191) {
        opthd_total_shared_queue_rewqe = 8191;
    }
    rval = 0;/*c289*/
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDORQEQr, &rval,
                      OP_BUFFER_SHARED_LIMITf, opthd_total_shared_queue_rewqe);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDORQEQr (unit, rval));
    rval = 0;/*c291*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_THDORQEQr, &rval,
                 OP_BUFFER_LIMIT_YELLOWf, opthd_total_shared_queue_rewqe / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_THDORQEQr(unit, rval));
    rval = 0;/*c293*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_THDORQEQr, &rval,
                     OP_BUFFER_LIMIT_REDf, opthd_total_shared_queue_rewqe / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_THDORQEQr(unit, rval));
    rval = 0;/*c290*/
    val = opthd_total_shared_queue_rewqe - (num_data_ports / 4);
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr, &rval,
                     OP_BUFFER_SHARED_LIMIT_RESUMEf, val);
    SOC_IF_ERROR_RETURN(
                 WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr (unit, rval));
    rval = 0;/*c292*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr, &rval,
                       OP_BUFFER_LIMIT_RESUME_YELLOWf,
                       opthd_total_shared_queue_rewqe / 8 - 1);
    SOC_IF_ERROR_RETURN(
                WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr(unit, rval));
    rval = 0;/*c294*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr, &rval,
                       OP_BUFFER_LIMIT_RESUME_REDf,
                       opthd_total_shared_queue_rewqe / 8 - 1);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr(unit, rval));

    /* Ouput port thresholds */
        /*EQEs*/
    if (opthd_total_shared_queue_eqe > 262143) {
        opthd_total_shared_queue_eqe = 262143;
    }
    rval = 0;/*c296*/
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_QENTRYr, &rval,
                  OP_BUFFER_SHARED_LIMIT_QENTRYf, opthd_total_shared_queue_eqe);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_QENTRYr (unit, rval));
    rval = 0;/*c298*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_QENTRYr, &rval,
              OP_BUFFER_LIMIT_YELLOW_QENTRYf, opthd_total_shared_queue_eqe / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_QENTRYr(unit, rval));
    rval = 0;/*c300*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_QENTRYr, &rval,
              OP_BUFFER_LIMIT_RED_QENTRYf, opthd_total_shared_queue_eqe / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_QENTRYr(unit, rval));
    rval = 0;/*c297*/
    val = opthd_total_shared_queue_eqe - 
          (num_data_ports / 4 * ka_mmu_params.num_replication_per_pkt);
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr, &rval,
                  OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYf, val);
    SOC_IF_ERROR_RETURN(
                 WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr (unit, rval));
    rval = 0;/*c299*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr, &rval,
                         OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYf, val / 8);
    SOC_IF_ERROR_RETURN(
                 WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr(unit, rval));
    rval = 0;/*c301*/
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_QENTRYr, &rval,
                         OP_BUFFER_LIMIT_RESUME_RED_QENTRYf, val / 8);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_QENTRYr(unit, rval));

    /* Per Port Registers */
    PBMP_ALL_ITER(unit, port) {
        pg_num = (port >= 25 && port <= 28) ? 7 : 0;

        /* Ports 27,32,33,34 are ports in one MXQ block.
         * If more than 2 lanes are enabled then pg_num = 0.
         * Same with ports 28,29,30,31.
         */
        if ((port == 27) && (SOC_PORT_VALID(unit, 32) ||
            SOC_PORT_VALID(unit, 33) || SOC_PORT_VALID(unit, 34))) {
            pg_num = 0;
        } else if ((port == 28) && (SOC_PORT_VALID(unit, 29) ||
            SOC_PORT_VALID(unit, 30) || SOC_PORT_VALID(unit, 31))) {
            pg_num = 0;
        }

        /* Input Port Thresholds */
            /* Internal Buffer Ingress Pool */
        rval = 0;/*c307*/
        soc_reg_field_set(unit, PORT_MIN_CELLr, &rval,
                          PORT_MINf, ipthd_per_port_min_intbuf_ingpool);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr (unit, port, rval));
        rval = 0;/*c308*/
        soc_reg_field_set(unit, PORT_MAX_SHARED_CELLr, &rval,
                          PORT_MAXf, ipthd_total_shared_intbuf_ingpool);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_SHARED_CELLr (unit, port, rval));
        rval = 0;/*c309*/
        soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval,
                          PORT_MAX_PKT_SIZEf, KA_MMU_MAX_PKT_SIZE_IN_CELLS);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr (unit, port, rval));
        rval = 0;/*c310*/
        val = ipthd_total_shared_intbuf_ingpool - 2 * ethernet_mtu_cells_intbuf;
        soc_reg_field_set(unit, PORT_RESUME_LIMIT_CELLr, &rval, CELLSf, val);
        SOC_IF_ERROR_RETURN(WRITE_PORT_RESUME_LIMIT_CELLr (unit, port, rval));
        rval = 0;/*c311*/
        soc_reg_field_set(unit, PG_MIN_CELLr, &rval, PG_MINf,
                          ipthd_per_pg_min_intbuf_ingpool);
        SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr (unit, port, pg_num, rval));
        if (pg_num) { /*other PGs for port with 8 PGs */
            rval = 0; /*c312*/
            soc_reg_field_set(unit, PG_MIN_CELLr, &rval, PG_MINf, 1);
            for (j = 0; j < pg_num; j++) {
                SOC_IF_ERROR_RETURN(
                    WRITE_PG_MIN_CELLr(unit, port, j, rval));
            }
        }
        rval = 0;/*c313, c316*/
        val = lossless ? KA_MMU_ING_PG_DYN_THD_PARAM :
                         ipthd_total_shared_intbuf_ingpool;
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_LIMITf, val);
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_DYNAMICf, lossless ? 1 : 0);
        SOC_IF_ERROR_RETURN(
            WRITE_PG_SHARED_LIMIT_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c314*/
        soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval,
                          PG_RESET_OFFSETf, 2 * ethernet_mtu_cells_intbuf);
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c315*/
        soc_reg_field_set(unit, PG_RESET_FLOOR_CELLr, &rval, PG_RESET_FLOORf,0);
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_FLOOR_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c317*/
        val = (IS_CPU_PORT (unit, port)) ? 0 :
                  ((IS_HG_PORT (unit, port) || IS_XE_PORT (unit, port)) ?
                      ka_mmu_params.hg_pg_hdrm_intbuf :
                      ka_mmu_params.ge_pg_hdrm_intbuf);
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval,
                          PG_HDRM_LIMITf, val);
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr (unit, port, pg_num, rval));

        if (!all_int_mem_port) { /* not applicable for all internal mem port */
        /* Input Port Thresholds */
            /* External Buffer */
        rval = 0;/*c319*/
        soc_reg_field_set(unit, THDIEXT_PORT_MIN_CELLr, &rval,
                          PORT_MINf, ipthd_per_port_min_extbuf);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MIN_CELLr (unit, port, rval));
        rval = 0;/*c320*/
        soc_reg_field_set(unit, THDIEXT_PORT_MAX_SHARED_CELLr, &rval,
                          PORT_MAXf, ipthd_total_shared_extbuf);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PORT_MAX_SHARED_CELLr (unit, port, rval));
        rval = 0;/*c321*/
        soc_reg_field_set(unit, THDIEXT_PORT_MAX_PKT_SIZEr, &rval,
                          PORT_MAX_PKT_SIZEf, KA_MMU_JUMBO_FRAME_EXT_BUF);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MAX_PKT_SIZEr (unit, port,rval));
        rval = 0;/*c322*/
        val = ipthd_total_shared_extbuf - 2 * ethernet_mtu_cells_extbuf;
        soc_reg_field_set(unit, THDIEXT_PORT_RESUME_LIMIT_CELLr, &rval,
                          CELLSf, val);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PORT_RESUME_LIMIT_CELLr (unit, port, rval));
        rval = 0;/*c323*/
        soc_reg_field_set(unit, THDIEXT_PG_MIN_CELLr, &rval,
                          PG_MINf, ipthd_per_pg_min_extbuf);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_MIN_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c324, c327*/
        val = lossless ? KA_MMU_ING_PG_DYN_THD_PARAM :
                         ipthd_total_shared_extbuf;
        soc_reg_field_set(unit, THDIEXT_PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_LIMITf, val);
        soc_reg_field_set(unit, THDIEXT_PG_SHARED_LIMIT_CELLr, &rval,
                       PG_SHARED_DYNAMICf, lossless ? 1 : 0);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PG_SHARED_LIMIT_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c325*/
        soc_reg_field_set(unit, THDIEXT_PG_RESET_OFFSET_CELLr, &rval,
                          PG_RESET_OFFSETf, 2 * ethernet_mtu_cells_extbuf);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PG_RESET_OFFSET_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c326*/
        soc_reg_field_set(unit, THDIEXT_PG_RESET_FLOOR_CELLr, &rval,
                          PG_RESET_FLOORf, 0);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PG_RESET_FLOOR_CELLr(unit,port, pg_num, rval));
        rval = 0;/*c328*/
        val = (IS_CPU_PORT (unit, port)) ? 0 :
                  ((IS_HG_PORT (unit, port) || IS_XE_PORT (unit, port)) ?
                      ka_mmu_params.hg_pg_hdrm_extbuf :
                      ka_mmu_params.ge_pg_hdrm_extbuf);
        soc_reg_field_set(unit, THDIEXT_PG_HDRM_LIMIT_CELLr, &rval,
                          PG_HDRM_LIMITf, val);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PG_HDRM_LIMIT_CELLr (unit, port, pg_num, rval));
                    

        /* Input Port Thresholds */
            /*EMA Pool */
        rval = 0;/*c330*/
        soc_reg_field_set(unit, THDIEMA_PORT_MIN_CELLr, &rval,
                          PORT_MINf, ipthd_per_port_min_intbuf_emapool);
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MIN_CELLr (unit, port, rval));
        rval = 0;/*c331*/
        soc_reg_field_set(unit, THDIEMA_PORT_MAX_SHARED_CELLr, &rval,
                          PORT_MAXf, ipthd_total_shared_intbuf_emapool);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEMA_PORT_MAX_SHARED_CELLr (unit, port, rval));
        rval = 0;/*c332*/
        soc_reg_field_set(unit, THDIEMA_PORT_MAX_PKT_SIZEr, &rval,
                          PORT_MAX_PKT_SIZEf, KA_MMU_JUMBO_FRAME_INT_BUF);
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MAX_PKT_SIZEr (unit, port,rval));
        rval = 0;/*c333*/
        val = ipthd_total_shared_intbuf_emapool - 2 * ethernet_mtu_cells_intbuf;
        soc_reg_field_set(unit, THDIEMA_PORT_RESUME_LIMIT_CELLr, &rval,
                          CELLSf, val);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEMA_PORT_RESUME_LIMIT_CELLr (unit, port, rval));
        rval = 0;/*c334*/
        soc_reg_field_set(unit, THDIEMA_PG_MIN_CELLr, &rval,
                          PG_MINf, ipthd_per_pg_min_intbuf_emapool);
        SOC_IF_ERROR_RETURN(
            WRITE_THDIEMA_PG_MIN_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c335, c338*/
        val = lossless ? KA_MMU_ING_PG_DYN_THD_PARAM :
                         ipthd_total_shared_intbuf_emapool;
        soc_reg_field_set(unit, THDIEMA_PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_LIMITf, val);
        soc_reg_field_set(unit, THDIEMA_PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_DYNAMICf, lossless ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_SHARED_LIMIT_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c336*/
        soc_reg_field_set(unit, THDIEMA_PG_RESET_OFFSET_CELLr, &rval,
                          PG_RESET_OFFSETf, 2 * ethernet_mtu_cells_intbuf);
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_RESET_OFFSET_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c337*/
        soc_reg_field_set(unit, THDIEMA_PG_RESET_FLOOR_CELLr, &rval,
                          PG_RESET_FLOORf, 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_RESET_FLOOR_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c339*/
        val = (IS_CPU_PORT (unit, port)) ? 0 :
                  ((IS_HG_PORT (unit, port) || IS_XE_PORT (unit, port)) ?
                      ka_mmu_params.hg_pg_hdrm_ema :
                      ka_mmu_params.ge_pg_hdrm_ema);
        soc_reg_field_set(unit, THDIEMA_PG_HDRM_LIMIT_CELLr, &rval,
                          PG_HDRM_LIMITf, val);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIEMA_PG_HDRM_LIMIT_CELLr (unit, port, pg_num, rval));
                    
        }/* if (!all_int_mem_port)*/

        /* Input Port Thresholds */
            /* RE WQEs */
        rval = 0;/*c341*/
        soc_reg_field_set(unit, THDIRQE_PORT_MIN_CELLr, &rval,
                          PORT_MINf, ipthd_per_port_min_rewqe);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_MIN_CELLr (unit, port, rval));
        rval = 0;/*c342*/
        soc_reg_field_set(unit, THDIRQE_PORT_MAX_SHARED_CELLr, &rval,
                          PORT_MAXf, ipthd_total_shared_rewqe);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIRQE_PORT_MAX_SHARED_CELLr (unit, port, rval));
        rval = 0;/*c343*/
        soc_reg_field_set(unit, THDIRQE_PORT_RESUME_LIMIT_CELLr, &rval,
                          CELLSf, ipthd_total_shared_rewqe - 2);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIRQE_PORT_RESUME_LIMIT_CELLr (unit, port, rval));
        rval = 0;/*c344*/
        soc_reg_field_set(unit, THDIRQE_PG_MIN_CELLr, &rval,
                          PG_MINf, ipthd_per_pg_min_rewqe);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_MIN_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c345, c348*/
        val = lossless ? KA_MMU_ING_PG_DYN_THD_PARAM : ipthd_total_shared_rewqe;
        soc_reg_field_set(unit, THDIRQE_PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_LIMITf, val);
        soc_reg_field_set(unit, THDIRQE_PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_DYNAMICf, lossless ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_SHARED_LIMIT_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c346*/
        soc_reg_field_set(unit, THDIRQE_PG_RESET_OFFSET_CELLr, &rval,
                          PG_RESET_OFFSETf, 2);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_RESET_OFFSET_CELLr (unit, port, pg_num, rval));
        rval = 0;/*c347*/
        soc_reg_field_set(unit, THDIRQE_PG_RESET_FLOOR_CELLr, &rval,
                          PG_RESET_FLOORf, 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_RESET_FLOOR_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c349*/
        val = (IS_CPU_PORT (unit, port)) ? 0 :
                  ((IS_HG_PORT (unit, port) || IS_XE_PORT (unit, port)) ?
                      ka_mmu_params.hg_pg_hdrm_rewqe :
                      ka_mmu_params.ge_pg_hdrm_rewqe);
        soc_reg_field_set(unit, THDIRQE_PG_HDRM_LIMIT_CELLr, &rval,
                          PG_HDRM_LIMITf, val);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_HDRM_LIMIT_CELLr (unit, port,
                            pg_num, rval));
                    

        /* Input Port Thresholds */
           /* EQEs */
        rval = 0;/*c351*/
        soc_reg_field_set(unit, THDIQEN_PORT_MIN_CELLr, &rval,
                          PORT_MINf, ipthd_per_port_min_eqe);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_MIN_CELLr (unit, port, rval));
        rval = 0;/*c352*/
        soc_reg_field_set(unit, THDIQEN_PORT_MAX_SHARED_CELLr, &rval,
                          PORT_MAXf, ipthd_total_shared_eqe);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIQEN_PORT_MAX_SHARED_CELLr (unit, port, rval));
        rval = 0;/*c353*/
        val = ipthd_total_shared_eqe - 2 *ka_mmu_params.num_replication_per_pkt;
        soc_reg_field_set(unit, THDIQEN_PORT_RESUME_LIMIT_CELLr, &rval,
                          CELLSf, val);
        SOC_IF_ERROR_RETURN(
                     WRITE_THDIQEN_PORT_RESUME_LIMIT_CELLr (unit, port, rval));
        rval = 0;/*c354*/
        soc_reg_field_set(unit, THDIQEN_PG_MIN_CELLr, &rval,
                          PG_MINf, ipthd_per_pg_min_eqe);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_MIN_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c355, c358*/
        val = lossless ? KA_MMU_ING_PG_DYN_THD_PARAM : ipthd_total_shared_eqe;
        soc_reg_field_set(unit, THDIQEN_PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_LIMITf, val);
        soc_reg_field_set(unit, THDIQEN_PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_DYNAMICf, lossless ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_SHARED_LIMIT_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c356*/
        soc_reg_field_set(unit, THDIQEN_PG_RESET_OFFSET_CELLr, &rval,
                   PG_RESET_OFFSETf, 2 * ka_mmu_params.num_replication_per_pkt);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_RESET_OFFSET_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c357*/
        soc_reg_field_set(unit, THDIQEN_PG_RESET_FLOOR_CELLr, &rval,
                          PG_RESET_FLOORf, 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_RESET_FLOOR_CELLr (unit, port,
                            pg_num, rval));
        rval = 0;/*c359*/
        val = (IS_CPU_PORT (unit, port)) ? 0 :
                  ((IS_HG_PORT (unit, port) || IS_XE_PORT (unit, port)) ?
                      ipthd_hg_pg_hdrm_eqe : ipthd_ge_pg_hdrm_eqe);
        soc_reg_field_set(unit, THDIQEN_PG_HDRM_LIMIT_CELLr, &rval,
                          PG_HDRM_LIMITf, val);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_HDRM_LIMIT_CELLr (unit, port,
                            pg_num, rval));
    }

    /* Output port Thresholds */
    opn_index = 0;
    PBMP_ALL_ITER(unit, port) {
        /* Internal Buffer Egress pool and External Buffer*/
        for (i = 0; i < (IS_CPU_PORT(unit, port) ?
                         (si->port_num_uc_cosq[port] / 8) : 1); i++) {
            SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY, opn_index + i, &entry));
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, &entry,
                            OPN_SHARED_LIMIT_CELLf,
                            (SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port) ?
                                 opthd_total_shared_queue_extbuff :
                                 opthd_total_shared_queue_intbuff));/*c363, c379*/
            val = SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port) ? 
                            (opthd_total_shared_queue_extbuff -
                                 2 * ethernet_mtu_cells_extbuf) :
                            (opthd_total_shared_queue_intbuff -
                                 2 * ethernet_mtu_cells_intbuf);
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, &entry,
                            OPN_SHARED_RESET_VALUE_CELLf, val);/*c364, c380*/
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, &entry,
                            PORT_LIMIT_ENABLE_CELLf, 0);/*c365*/
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY, opn_index + i, &entry));
        }
        opn_index += (IS_CPU_PORT(unit, port) ?
                      (si->port_num_uc_cosq[port] / 8) : 1);

        /* Base COS queues */
        for (i = si->port_cosq_base[port];
            i < si->port_cosq_base[port] + si->port_num_uc_cosq[port]; i++) {

            val = lossless ? (SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port) ? 
                      opthd_total_shared_queue_extbuff :
                      opthd_total_shared_queue_intbuff) :
                      mmu_egr_queue_dyn_thd_param_lossy;
            SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_CELLm(unit,
                        MEM_BLOCK_ANY,i,&entry));
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                        RESET_OFFSET_CELLf, 2);/*c369, 384*/
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                        LIMIT_RED_CELLf, lossless ? (val / 8) : 0);/*c375, 390*/
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                        RESET_OFFSET_YELLOW_CELLf, 2);/*c376, 391*/
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                        RESET_OFFSET_RED_CELLf, 2);/*c377, 392*/
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_CELLm(unit,
                        MEM_BLOCK_ANY,i,&entry));

            SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_CELLm(unit,
                        MEM_BLOCK_ANY,i,&entry));
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                        Q_MIN_CELLf,
                        (SOC_PBMP_MEMBER(PBMP_EXT_MEM (unit), port) ?
                                 opthd_port_min_grntee_queue_extbuf :
                                 opthd_port_min_grntee_queue_intbuf));/*c366, 382*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                        Q_SHARED_LIMIT_CELLf, val);/*c368, 383*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                        Q_LIMIT_DYNAMIC_CELLf, lossless ? 0 : 1);/*c370, 385*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                        Q_LIMIT_ENABLE_CELLf, lossless ? 0 : 1);/*c371, 386*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                        Q_COLOR_ENABLE_CELLf, 0);/*c372, 387*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                     Q_COLOR_LIMIT_DYNAMIC_CELLf, lossless ? 0 : 1);/*c373, 388*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                     LIMIT_YELLOW_CELLf, lossless ? (val / 8) : 0);/*c374, 389*/
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,
                        MEM_BLOCK_ANY,i,&entry));
        }
    }

    /* Output port Thresholds */
        /* EMA Pool */
    if (!all_int_mem_port) { /* not applicable for all internal mem port */
        for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDOEMAr); i++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDOEMAr(unit, i, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval,
                              Q_MINf, opthd_port_min_grntee_ema_queue);/*c409*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval,
                              Q_COLOR_ENABLEf, 0);/*c414*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval,
                              Q_COLOR_DYNAMICf, lossless ? 0 : 1);/*c415*/
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDOEMAr(unit, i, rval));
        }

        for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr);
            i++) {
            SOC_IF_ERROR_RETURN(
                         READ_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr, &rval,
                              Q_RESET_OFFSETf, 2);/*c411*/
            SOC_IF_ERROR_RETURN(
                         WRITE_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, i,rval));
        }
        for(i = 0; i < SOC_REG_NUMELS(unit,
                                 OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr); i++) {
            SOC_IF_ERROR_RETURN(
                     READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr(unit, i,&rval));
            soc_reg_field_set(unit,
                              OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr, &rval,
                              RESUME_OFFSET_YELLOWf, 2);/*c418*/
            SOC_IF_ERROR_RETURN(
                     WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDOEMAr(unit, i,rval));
        }
        for(i = 0; i < SOC_REG_NUMELS(unit,
                                 OP_QUEUE_RESET_OFFSET_RED_THDOEMAr); i++) {
            SOC_IF_ERROR_RETURN(
                     READ_OP_QUEUE_RESET_OFFSET_RED_THDOEMAr(unit, i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_RED_THDOEMAr, &rval,
                              RESUME_OFFSET_REDf, 2);/*c419*/
            SOC_IF_ERROR_RETURN(
                     WRITE_OP_QUEUE_RESET_OFFSET_RED_THDOEMAr(unit, i,rval));
        }

        val = lossless ? opthd_total_shared_queue_ema_queue : 
                         mmu_egr_queue_dyn_thd_param_lossy;
        for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDOEMAr); i++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDOEMAr(unit, i, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval,
                              Q_SHARED_LIMITf, val);/*c410*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval,
                              Q_LIMIT_DYNAMICf, lossless ? 0 : 1);/*c412*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval,
                              Q_LIMIT_ENABLEf, lossless ? 0 : 1);/*c413*/
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDOEMAr(unit, i, rval));
        }
        for(i = 0; i < SOC_REG_NUMELS(unit,
                                 OP_QUEUE_LIMIT_YELLOW_THDOEMAr); i++) {
            SOC_IF_ERROR_RETURN(
                         READ_OP_QUEUE_LIMIT_YELLOW_THDOEMAr(unit, i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDOEMAr, &rval,
                              Q_LIMIT_YELLOWf, lossless ? (val / 8) : 0);/*c416*/
            SOC_IF_ERROR_RETURN(
                         WRITE_OP_QUEUE_LIMIT_YELLOW_THDOEMAr(unit, i,rval));
        }
        for(i = 0; i < SOC_REG_NUMELS(unit,
                                 OP_QUEUE_LIMIT_RED_THDOEMAr); i++) {
            SOC_IF_ERROR_RETURN(
                         READ_OP_QUEUE_LIMIT_RED_THDOEMAr(unit, i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDOEMAr, &rval,
                              Q_LIMIT_REDf, lossless ? (val / 8) : 0);/*c417*/
            SOC_IF_ERROR_RETURN(
                         WRITE_OP_QUEUE_LIMIT_RED_THDOEMAr(unit, i,rval));
        }
    } /* if (!all_int_mem_port) */

    /* Output port Thresholds */
        /* RE WQs - RQEQ */
    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval,
                          Q_MINf, opthd_port_min_grntee_rewqe);/*c421*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval,
                          Q_COLOR_ENABLEf, 0);/*c426*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval,
                          Q_COLOR_DYNAMICf, lossless ? 0 : 1);/*c427*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEQr(unit, i, rval));
    }
    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit,i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr, &rval,
                          Q_RESET_OFFSETf, 1);/*c423*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit,i,rval));
    }
    for(i = 0; i < SOC_REG_NUMELS(unit,
                                 OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(
                 READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr(unit, i,&rval));
        soc_reg_field_set(unit,
                          OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr, &rval,
                          RESUME_OFFSET_YELLOWf, 1);/*c430*/
        SOC_IF_ERROR_RETURN(
                 WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEQr(unit, i,rval));
    }
    for(i = 0; i < SOC_REG_NUMELS(unit,
                             OP_QUEUE_RESET_OFFSET_RED_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(
                     READ_OP_QUEUE_RESET_OFFSET_RED_THDORQEQr(unit, i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_RED_THDORQEQr, &rval,
                          RESUME_OFFSET_REDf, 1);/*c431*/
        SOC_IF_ERROR_RETURN(
                     WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORQEQr(unit, i,rval));
    }

    val = lossless ? opthd_total_shared_queue_rewqe :
                     mmu_egr_queue_dyn_thd_param_lossy;
    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                          Q_SHARED_LIMITf, val);/*c422*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                          Q_LIMIT_DYNAMICf, lossless ? 0 : 1);/*c424*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval,
                          Q_LIMIT_ENABLEf, lossless ? 0 : 1);/*c425*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEQr(unit, i, rval));
    }
    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORQEQr(unit, i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORQEQr, &rval,
                          Q_LIMIT_REDf, lossless ? (val / 8) : 0);/*c428*/
        SOC_IF_ERROR_RETURN(
                     WRITE_OP_QUEUE_LIMIT_RED_THDORQEQr(unit, i,rval));
    }
    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORQEQr(unit,i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEQr, &rval,
                          Q_LIMIT_YELLOWf, lossless ? (val / 8) : 0);/*c429*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORQEQr(unit,i,rval));
    }

    /* RE WQs - RQEI */
    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEIr(unit, i, &rval));

        if (i < 8) { /*MC traffic queue 0 to 7*/
            q_min = opthd_port_min_grntee_rewqe_intbuf_mc;/*c432*/
            q_color_dyn = lossless ? 1 : (all_ext_mem_port ? 1 : 0);/*c446*/
        } else if ((i >= 8) && (i < 10)) { /*copy-to-cpu traffic queue 8 and 9*/
            q_min = opthd_port_min_grntee_rewqe_intbuf_tocpu;/*c433*/
            q_color_dyn = 1;/*c447*/
        } else if (i >= 10) { /* Mirror traffic queue 10 and 11*/
            q_min = opthd_port_min_grntee_rewqe_intbuf_mirror;/*c434*/
            q_color_dyn = 1;/*c448*/
        }

        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval,
                          Q_MINf, q_min);/*c432,433,434*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval,
                          Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval,
                          Q_COLOR_DYNAMICf, q_color_dyn);/*c446,447,448*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEIr(unit, i, rval));
    }

    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit,i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr, &rval,
                          Q_RESET_OFFSETf, lossless ? 2 :
                              (all_ext_mem_port ? 2 :
                                  ((i < 8) ?
                                      (448 / 8) : 2)));/*c438,439,440*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit,i,rval));
    }

    for(i = 0; i < SOC_REG_NUMELS(unit,
                                 OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(
                 READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr(unit, i,&rval));
        soc_reg_field_set(unit,
                          OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr, &rval,
                          RESUME_OFFSET_YELLOWf, lossless ? 2 :
                              (all_ext_mem_port ? 2 :
                                  ((i < 8) ?
                                      (448 / 8) : 2)));/*c455,456,457*/
        SOC_IF_ERROR_RETURN(
                 WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEIr(unit, i,rval));
    }

    for(i = 0; i < SOC_REG_NUMELS(unit,
                             OP_QUEUE_RESET_OFFSET_RED_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(
                     READ_OP_QUEUE_RESET_OFFSET_RED_THDORQEIr(unit, i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_RED_THDORQEIr, &rval,
                          RESUME_OFFSET_REDf, lossless ? 2 :
                              (all_ext_mem_port ? 2 :
                                  ((i < 8) ?
                                      (448 / 8) : 2)));/*c458,459,460*/
        SOC_IF_ERROR_RETURN(
                     WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORQEIr(unit, i,rval));
    }

    val = lossless ? opthd_total_shared_queue_rewqe_intbuf :
                     mmu_egr_queue_dyn_thd_param_lossy;
    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEIr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                          Q_SHARED_LIMITf, lossless ? 7 :
                              (all_ext_mem_port ? 7 :
                                  ((i < 8) ?
                                      896 : 7)));/*c435,436,437*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                          Q_LIMIT_DYNAMICf, lossless ? 1 :
                              (all_ext_mem_port ? 1 :
                                  ((i < 8) ?
                                      0 : 1)));/*c441,442,443*/
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval,
                          Q_LIMIT_ENABLEf, lossless ? 0 : 1);/*c444*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEIr(unit, i, rval));
    }

    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_RED_THDORQEIr(unit, i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORQEIr, &rval,
                          Q_LIMIT_REDf, lossless ? 0 :
                              (all_ext_mem_port ? 0 :
                                  ((i < 8) ?
                                      896 : 0)));/*c452,453,545*/
        SOC_IF_ERROR_RETURN(
                     WRITE_OP_QUEUE_LIMIT_RED_THDORQEIr(unit, i,rval));
    }

    for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_LIMIT_YELLOW_THDORQEIr(unit,i,&rval));
        soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEIr, &rval,
                          Q_LIMIT_YELLOWf,  lossless ? 0 :
                              (all_ext_mem_port ? 0 :
                                  ((i < 8) ?
                                      896 : 0)));/*c449,450,451*/
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_LIMIT_YELLOW_THDORQEIr(unit,i,rval));
    }

    /* RE WQs - RQEE */
    if (!all_int_mem_port) { /* not applicable for all internal mem port */
        for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEEr); i++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEEr(unit, i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval,
                              Q_MINf, opthd_port_min_grntee_rewqe_extbuf);/*c461*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval,
                              Q_COLOR_ENABLEf, 0);/*c466*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval,
                              Q_COLOR_DYNAMICf, lossless ? 0 : 1);/*c467*/
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEEr(unit, i,rval));
        }

        for(i = 0;
            i < SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr); i++) {
            SOC_IF_ERROR_RETURN(
                         READ_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, i, &rval));
            soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr, &rval,
                              Q_RESET_OFFSETf, 2);/*c463*/
            SOC_IF_ERROR_RETURN(
                         WRITE_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, i, rval));
        }

        for(i = 0; i < SOC_REG_NUMELS(unit,
                                OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr); i++) {
            SOC_IF_ERROR_RETURN(
                 READ_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr(unit, i,&rval));
            soc_reg_field_set(unit,
                              OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr, &rval,
                              RESUME_OFFSET_YELLOWf, 2);/*c470*/
            SOC_IF_ERROR_RETURN(
                 WRITE_OP_QUEUE_RESET_OFFSET_YELLOW_THDORQEEr(unit, i,rval));
        }

        for(i = 0; i < SOC_REG_NUMELS(unit,
                                 OP_QUEUE_RESET_OFFSET_RED_THDORQEEr); i++) {
            SOC_IF_ERROR_RETURN(
                     READ_OP_QUEUE_RESET_OFFSET_RED_THDORQEEr(unit, i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_RED_THDORQEEr, &rval,
                              RESUME_OFFSET_REDf, 2);/*c471*/
            SOC_IF_ERROR_RETURN(
                     WRITE_OP_QUEUE_RESET_OFFSET_RED_THDORQEEr(unit, i,rval));
        }

        val = lossless ? opthd_total_shared_queue_rewqe_extbuf :
                         mmu_egr_queue_dyn_thd_param_lossy;
        for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEEr); i++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEEr(unit, i, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval,
                              Q_SHARED_LIMITf, val);/*c462*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval,
                              Q_LIMIT_DYNAMICf, lossless ? 0 : 1);/*c464*/
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval,
                              Q_LIMIT_ENABLEf, lossless ? 0 : 1);/*c465*/
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEEr(unit, i, rval));
        }

        for(i = 0; i < SOC_REG_NUMELS(unit, OP_QUEUE_LIMIT_RED_THDORQEEr); i++) {
            SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_LIMIT_RED_THDORQEEr(unit, i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_RED_THDORQEEr, &rval,
                              Q_LIMIT_REDf, lossless ? (val / 8) : 0);/*c469*/
            SOC_IF_ERROR_RETURN(
                         WRITE_OP_QUEUE_LIMIT_RED_THDORQEEr(unit, i,rval));
        }

        for(i = 0; i < SOC_REG_NUMELS(unit,
                                    OP_QUEUE_LIMIT_YELLOW_THDORQEEr); i++) {
            SOC_IF_ERROR_RETURN(
                READ_OP_QUEUE_LIMIT_YELLOW_THDORQEEr(unit,i,&rval));
            soc_reg_field_set(unit, OP_QUEUE_LIMIT_YELLOW_THDORQEEr, &rval,
                              Q_LIMIT_YELLOWf, lossless ? (val / 8) : 0);/*c468*/
            SOC_IF_ERROR_RETURN(
                WRITE_OP_QUEUE_LIMIT_YELLOW_THDORQEEr(unit,i,rval));
        }
    } /* if (!all_int_mem_port) */

    /* Output port Thresholds */
    opn_index = 0;
    PBMP_ALL_ITER(unit, port) {
        /* EQEs */
        for (i = 0; i < (IS_CPU_PORT(unit, port) ?
                         (si->port_num_uc_cosq[port] / 8) : 1); i++) {

            SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                               MEM_BLOCK_ANY, opn_index + i, &entry));
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm, &entry,
                         OPN_SHARED_LIMIT_QENTRYf,opthd_total_shared_queue_eqe);/*c394*/
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm, &entry,
                               OPN_SHARED_RESET_VALUE_QENTRYf,
                               opthd_total_shared_queue_eqe - 2);/*c395*/
            soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm, &entry,
                               PORT_LIMIT_ENABLE_QENTRYf, 0);/*c396*/
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                               MEM_BLOCK_ANY,opn_index + i, &entry));
        }
        opn_index += (IS_CPU_PORT(unit, port) ?
                      (si->port_num_uc_cosq[port] / 8) : 1);

        for (i = si->port_cosq_base[port];
             i < si->port_cosq_base[port] + si->port_num_uc_cosq[port]; i++) {

            val = lossless ? opthd_total_shared_queue_eqe :
                  mmu_egr_queue_dyn_thd_param_lossy;

            SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_QENTRYm(unit,
                               MEM_BLOCK_ANY,i,&entry));
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                               RESET_OFFSET_QENTRYf, 1);/*c399*/
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                               LIMIT_RED_QENTRYf, lossless ? (val/8) : 0);/*c405*/
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                               RESET_OFFSET_YELLOW_QENTRYf, 1);/*c406*/
            soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                               RESET_OFFSET_RED_QENTRYf, 1);/*c407*/
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_QENTRYm(unit,
                               MEM_BLOCK_ANY,i,&entry));

            SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_QENTRYm(unit,
                               MEM_BLOCK_ANY,i,&entry));
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                               Q_MIN_QENTRYf, opthd_port_min_grntee_eqe);/*c397*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                               Q_SHARED_LIMIT_QENTRYf, val);/*c398*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                               Q_LIMIT_DYNAMIC_QENTRYf, lossless ? 0 : 1);/*c400*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                               Q_LIMIT_ENABLE_QENTRYf, lossless ? 0 : 1);/*c401*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                               Q_COLOR_ENABLE_QENTRYf, 0);/*c402*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                               Q_COLOR_LIMIT_DYNAMIC_QENTRYf, lossless ? 0 : 1);/*c403*/
            soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                               LIMIT_YELLOW_QENTRYf, lossless ? (val/8) : 0);/*c404*/
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_QENTRYm(unit,
                               MEM_BLOCK_ANY,i,&entry));
        }

        /* Enable back pressure status from MMU for lossless mode */
        if (lossless) {
            COMPILER_64_SET(val64, 0xf, 0xffffffff);
            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIEXT_PORT_PAUSE_ENABLE0_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PORT_PAUSE_ENABLE0_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIQEN_PORT_PAUSE_ENABLE0_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIQEN_PORT_PAUSE_ENABLE0_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIRQE_PORT_PAUSE_ENABLE0_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIRQE_PORT_PAUSE_ENABLE0_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIEMA_PORT_PAUSE_ENABLE0_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIEMA_PORT_PAUSE_ENABLE0_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, PORT_PAUSE_ENABLE0_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_PORT_PAUSE_ENABLE0_64r(unit, r64val));

            COMPILER_64_SET(val64, 0x1, 0xffffffff);
            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIEXT_PORT_PAUSE_ENABLE1_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIEXT_PORT_PAUSE_ENABLE1_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIQEN_PORT_PAUSE_ENABLE1_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIQEN_PORT_PAUSE_ENABLE1_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIRQE_PORT_PAUSE_ENABLE1_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIRQE_PORT_PAUSE_ENABLE1_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, THDIEMA_PORT_PAUSE_ENABLE1_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_THDIEMA_PORT_PAUSE_ENABLE1_64r(unit, r64val));

            COMPILER_64_ZERO(r64val);
            soc_reg64_field_set(unit, PORT_PAUSE_ENABLE1_64r, &r64val,
                              PORT_PAUSE_ENABLEf, val64);
            SOC_IF_ERROR_RETURN(
                     WRITE_PORT_PAUSE_ENABLE1_64r(unit, r64val));
        }
    }

    /* Extended COS Queues */
    val = lossless ? (all_ext_mem_port ?
                     opthd_total_shared_queue_extbuff :
                     opthd_total_shared_queue_intbuff) :
                         mmu_egr_queue_dyn_thd_param_lossy;
    for (j = si->num_mmu_base_cosq;
        j < soc_mem_index_max(unit,MMU_THDO_QCONFIG_CELLm); j++) {
        /* Output port threshold Internal/External Buffer*/

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_CELLm(unit,
                    MEM_BLOCK_ANY, j, &entry));
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                    Q_MIN_CELLf, all_int_mem_port ?
                    0 : opthd_port_min_grntee_queue_extbuf);/*c367, c382*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                    Q_SHARED_LIMIT_CELLf, val);/*c368, 383*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                    Q_LIMIT_DYNAMIC_CELLf, lossless ? 0 : 1);/*c370, 385*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                    Q_LIMIT_ENABLE_CELLf, lossless ? 0 : 1);/*c371, 386*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                    Q_COLOR_ENABLE_CELLf, 0);/*c372, 387*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                 Q_COLOR_LIMIT_DYNAMIC_CELLf, lossless ? 0 : 1);/*c373, 388*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                 LIMIT_YELLOW_CELLf, lossless ? (val / 8) : 0);/*c374, 389*/
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,
                    MEM_BLOCK_ANY, j, &entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_CELLm(unit,
                    MEM_BLOCK_ANY, j, &entry));
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                    RESET_OFFSET_CELLf, 2);/*c369, 384*/
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                    RESET_OFFSET_YELLOW_CELLf, 2);/*c376, 391*/
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                    RESET_OFFSET_RED_CELLf, 2);/*c377, 392*/
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                    LIMIT_RED_CELLf, lossless ? (val / 8) : 0);/*c375, 390*/
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_CELLm(unit,
                    MEM_BLOCK_ANY, j, &entry));
    }

    val = lossless ? opthd_total_shared_queue_eqe :
                     mmu_egr_queue_dyn_thd_param_lossy;
    for (j = si->num_mmu_base_cosq;
        j < soc_mem_index_max(unit,MMU_THDO_QCONFIG_QENTRYm); j++) {
        /* Output port threshold EQEs*/

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_QENTRYm(unit,
                     MEM_BLOCK_ANY, j, &entry));
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                     Q_MIN_QENTRYf, opthd_port_min_grntee_eqe);/*c397*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                     Q_SHARED_LIMIT_QENTRYf, val);/*c398*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                     Q_LIMIT_DYNAMIC_QENTRYf, lossless ? 0 : 1);/*c400*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                     Q_LIMIT_ENABLE_QENTRYf, lossless ? 0 : 1);/*c401*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                     Q_COLOR_ENABLE_QENTRYf, 0);/*c402*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                     Q_COLOR_LIMIT_DYNAMIC_QENTRYf, lossless ? 0 : 1);/*c403*/
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                     LIMIT_YELLOW_QENTRYf, lossless ? (val/8) : 0);/*c404*/
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_QENTRYm(unit,
                     MEM_BLOCK_ANY, j, &entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_QENTRYm(unit,
                     MEM_BLOCK_ANY, j, &entry));
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                     RESET_OFFSET_QENTRYf, 1);/*c399*/
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                     LIMIT_RED_QENTRYf, lossless ? (val/8) : 0);/*c405*/
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                     RESET_OFFSET_YELLOW_QENTRYf, 1);/*c406*/
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                     RESET_OFFSET_RED_QENTRYf, 1);/*c407*/
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_QENTRYm(unit,
                     MEM_BLOCK_ANY, j, &entry));

    }

    return SOC_E_NONE;
}

/*********************************************************
 * KATANA MMU HELPER  --- END
 */

STATIC int
_soc_katana_mmu_init(int unit)
{
    uint32 rval, nxtaddr;
    uint64 r64val;
    int                 port, mmu_init_usec, i;
    soc_timeout_t       to;
    uint32 entry[SOC_MAX_MEM_WORDS];
    mmu_aging_lmt_int_entry_t age_entry;
    int count, pg_num;
    uint16 dev_id;
    uint8 rev_id;
    soc_info_t *si;
    uint32 port_cfg0, port_cfg1;
    uint32 ext_mem_bw;
    lls_l2_xoff_entry_t lls_l2_xoff_state;
    int parity_enable;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    si = &SOC_INFO(unit);

    /* Init Link List Scheduler */
    rval = 0;
    soc_reg_field_set(unit, LLS_SOFT_RESETr, &rval, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_LLS_SOFT_RESETr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, LLS_INITr, &rval, INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_INITr(unit, rval));

    /* Wait for LLS init done. */
    if (SAL_BOOT_SIMULATION) {
        mmu_init_usec = 10000000; /* Simulation  .. 10sec */
    } else {
        mmu_init_usec = 50000;
    }
    soc_timeout_init(&to, mmu_init_usec, 0);
    do {
        SOC_IF_ERROR_RETURN(READ_LLS_INITr(unit, &rval));
        if (soc_reg_field_get(unit, LLS_INITr, rval, INIT_DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : LLS INIT timeout\n"), unit));
            break;
        }
    } while (TRUE);

    /* Setup TDM for MMU Arb & LLS */
    _soc_katana_mmu_tdm_init(unit);

    /* Calculate raw BW and round down to the nearest gig */
    ext_mem_bw = SOC_DDR3_CLOCK_MHZ(unit) * SOC_DDR3_NUM_MEMORIES(unit) * 2 * 16 / 1000 * 1000;
    /* Factor in refresh, bank collision, and turnaround.  Also round down */
    if (SOC_DDR3_NUM_MEMORIES(unit) == 1) {
        ext_mem_bw = (ext_mem_bw * 86 / 100) / 1000 * 1000;
    } else {
        ext_mem_bw = (ext_mem_bw * 88 / 100) / 1000 * 1000;
    }
    /* Calculate full duplex bandwidth for 492B packet (17 32B accesses) */
    ext_mem_bw = ext_mem_bw * 8 / 17;
    /* Favor DRAM writes over DRAM reads */
    ext_mem_bw = ext_mem_bw * 9 / 10;
    for(i=0; i<16; i++) {
        SOC_IF_ERROR_RETURN(READ_TOQ_EXT_MEM_BW_MAP_TABLEr(unit, i, &rval));
        
        soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                            GBL_GUARENTEE_BW_LIMITf, ext_mem_bw / 20);
        soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                            WR_PHASEf, 120);
        soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                            RD_PHASEf, 120);
        SOC_IF_ERROR_RETURN(
            WRITE_TOQ_EXT_MEM_BW_MAP_TABLEr(unit,i,rval));
    }
    SOC_IF_ERROR_RETURN(READ_TOQ_EXT_MEM_BW_TIMER_CFGr(unit, &rval));
    soc_reg_field_set(unit, TOQ_EXT_MEM_BW_TIMER_CFGr, &rval, MIDPKT_SHAPE_ENABLEf,0);
    SOC_IF_ERROR_RETURN(WRITE_TOQ_EXT_MEM_BW_TIMER_CFGr(unit, rval));

    nxtaddr = 0;
    port_cfg0 = port_cfg1 = 0;

    PBMP_ALL_ITER(unit, port) {
        /* Configure Egress Fifo */
        rval = 0;
        soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_START_ADDRESSf,
                                                        nxtaddr);
        if (IS_EXT_MEM_PORT(unit, port)) {
            if (si->port_speed_max[port] >= 10000) {
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_XMIT_THRESHOLDf, 35);
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_DEPTHf, 60);
                nxtaddr += 60;
            } else if (si->port_speed_max[port] == 2500) {
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_XMIT_THRESHOLDf, 20);
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_DEPTHf, 28);
                nxtaddr += 28;
            } else {
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_XMIT_THRESHOLDf, 8);
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_DEPTHf, 11);
                nxtaddr += 11;
            }
            if (port <= 31) { 
                port_cfg0 |= (1 << port);
            } else {
                port_cfg1 |= (1 << (port - 32));
            }
        } else {
            soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_XMIT_THRESHOLDf, 1);
            if ((dev_id == BCM56440_DEVICE_ID) || (dev_id == BCM56441_DEVICE_ID) ||
                (dev_id == BCM56445_DEVICE_ID) || (dev_id == BCM56448_DEVICE_ID) ||
                (dev_id == BCM56449_DEVICE_ID) || (dev_id == BCM55440_DEVICE_ID)) { /* 133MHz */
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_DEPTHf,
                                                  (si->port_speed_max[port] >= 10000) ? 6 : 2);
                nxtaddr += ((si->port_speed_max[port] >= 10000) ? 6 : 2);
            } else { /* 90MHz */
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, EGRESS_FIFO_DEPTHf,
                                                  (si->port_speed_max[port] >= 10000) ? 9 : 3);
                nxtaddr += ((si->port_speed_max[port] >= 10000) ? 9 : 3);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFGr(unit, port, rval));
        
        SOC_IF_ERROR_RETURN(WRITE_DEST_PORT_CFG_0r(unit, port_cfg0));
        SOC_IF_ERROR_RETURN(WRITE_DEST_PORT_CFG_1r(unit, port_cfg1));
    
        /* Port BW Ctrl */
        SOC_IF_ERROR_RETURN(READ_TOQ_PORT_BW_CTRLr(unit, port, &rval));
        if (IS_CPU_PORT(unit,port) || IS_LB_PORT(unit,port)) {
            if ((dev_id == BCM56443_DEVICE_ID) ||(dev_id == BCM56241_DEVICE_ID)) { /* And all other 2.5G variants */
                soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, PORT_BWf, 125);
            } else {
                soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, PORT_BWf, 50);
            }
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, START_THRESHOLDf, 127);
        } else if (si->port_speed_max[port] >= 10000) {
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, PORT_BWf, si->port_speed_max[port] / 20);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, START_THRESHOLDf, 34);
        } else if (si->port_speed_max[port] == 2500) {
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, PORT_BWf, 125);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, START_THRESHOLDf, 19);
        } else {
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, PORT_BWf, 50);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, START_THRESHOLDf, 7);
        }
        SOC_IF_ERROR_RETURN(WRITE_TOQ_PORT_BW_CTRLr(unit, port, rval));
    }
    rval = 0;
    soc_reg_field_set(unit, DEQ_EFIFO_CFG_COMPLETEr, &rval, EGRESS_FIFO_CONFIGURATION_COMPLETEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFG_COMPLETEr(unit, rval));

    /* Enable LLS */
    rval = 0;
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, DEQUEUE_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, ENQUEUE_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, FC_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, MIN_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, PORT_SCHEDULER_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, SHAPER_ENABLEf, 1);
    if (SOC_REG_FIELD_VALID(unit, LLS_CONFIG0r, SPRI_VECT_MODE_ENABLEf)) {
        if (soc_feature(unit, soc_feature_vector_based_spri)) {
            soc_reg_field_set(unit, LLS_CONFIG0r, &rval,
                              SPRI_VECT_MODE_ENABLEf, 1);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_LLS_CONFIG0r(unit, rval));

    /* Enable shaper background refresh */
    rval = 0;
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, L0_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, L1_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, L2_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, PORT_MAX_REFRESH_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_MAX_REFRESH_ENABLEr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval, L0_MIN_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval, L1_MIN_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval, L2_MIN_REFRESH_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_MIN_REFRESH_ENABLEr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_LLS_MIN_CAP_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, LLS_MIN_CAP_CONFIGr, &rval, SPARE1f, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_MIN_CAP_CONFIGr(unit, rval));

    /*
     * Disable per queue backpressure from CMIC and clear XOFF state. The
     * queue back pressure mechanism for Katana (and possibliy other devices)
     * causes the round robin scheduling mechanism to restart when the queues
     * are full.  In this scenario, the programmed weights are not correctly
     * applied.  The solution is to disable the hardwired backpressure
     * mechanism and let the queue credit backpressue scheme control things.
     */

    /* Step 1: Disable the hardwired queue backpressure mechanism */
    SOC_IF_ERROR_RETURN(READ_LLS_FC_CONFIGr(unit, &rval));
    /* Set the LLS_FC_CONFIGr - FC_DROP_DEQUEUEf=1 for
     * Katana B0 onwards
     */
    if ((SOC_IS_KATANA(unit)) && (rev_id >= BCM56440_B0_REV_ID)) {
        soc_reg_field_set(unit, LLS_FC_CONFIGr, &rval, FC_DROP_DEQUEUEf, 1);
    }
    soc_reg_field_set(unit, LLS_FC_CONFIGr, &rval, FC_CFG_DISABLE_L2_COSMASK_XOFFf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_FC_CONFIGr(unit, rval));

    /* Step 2: Clear the XOFF State */
    sal_memset(&lls_l2_xoff_state, 0, sizeof(lls_l2_xoff_entry_t));
    SOC_IF_ERROR_RETURN(WRITE_LLS_L2_XOFFm(unit, MEM_BLOCK_ALL, 0, &lls_l2_xoff_state));

    /* Configure the FAP sizes  */
#define RQE_WQE_HEADROOM    64
#define RQE_WQE_OFFSET      64
#define CFAPI_HEADROOM      64
#define CFAPI_OFFSET        64
#define CFAPE_HEADROOM      1535
#define CFAPE_OFFSET        96
#define QENTRY_HEADROOM     8
#define QENTRY_OFFSET       64

    /* Configure the RQE Work Queue Entry FAP */
    SOC_IF_ERROR_RETURN(READ_MMU_ENQ_FAPCONFIG_0r(unit, &rval));
    count = soc_reg_field_get(unit, MMU_ENQ_FAPCONFIG_0r, rval, FAPPOOLSIZEf);
    count -= RQE_WQE_HEADROOM;
    rval = 0;
    soc_reg_field_set(unit, MMU_ENQ_FAPFULLSETPOINT_0r, &rval, FAPFULLSETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_FAPFULLSETPOINT_0r(unit, rval));
    count -= RQE_WQE_OFFSET; 
    rval = 0;
    soc_reg_field_set(unit, MMU_ENQ_FAPFULLRESETPOINT_0r, &rval, FAPFULLRESETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_FAPFULLRESETPOINT_0r(unit, rval));

    /* Configure the internal cell FAP */
    SOC_IF_ERROR_RETURN(READ_CFAPIOTPCONFIGr(unit, &rval));
    count = soc_reg_field_get(unit, CFAPIOTPCONFIGr, rval, MAXFULLSETf);
    rval = 0;
    soc_reg_field_set(unit, CFAPICONFIGr, &rval, CFAPIPOOLSIZEf, count);
    SOC_IF_ERROR_RETURN(WRITE_CFAPICONFIGr(unit, rval));
    count -= CFAPI_HEADROOM;
    rval = 0;
    soc_reg_field_set(unit, CFAPIFULLSETPOINTr, &rval, CFAPIFULLSETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_CFAPIFULLSETPOINTr(unit, rval));
    count -= CFAPI_OFFSET; 
    rval = 0;
    soc_reg_field_set(unit, CFAPIFULLRESETPOINTr, &rval, CFAPIFULLRESETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_CFAPIFULLRESETPOINTr(unit, rval));

    /* Configure the external cell FAP */
    SOC_IF_ERROR_RETURN(READ_CFAPEOTPCONFIGr(unit, &rval));
    count = soc_reg_field_get(unit, CFAPEOTPCONFIGr, rval, MAXFULLSETf);
    rval = 0;
    soc_reg_field_set(unit, CFAPECONFIGr, &rval, CFAPEPOOLSIZEf, count);
    SOC_IF_ERROR_RETURN(WRITE_CFAPECONFIGr(unit, rval));
    count -= CFAPE_HEADROOM;
    rval = 0;
    soc_reg_field_set(unit, CFAPEFULLSETPOINTr, &rval, CFAPEFULLSETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_CFAPEFULLSETPOINTr(unit, rval)); 
    count -= CFAPE_OFFSET; 
    rval = 0;
    soc_reg_field_set(unit, CFAPEFULLRESETPOINTr, &rval, CFAPEFULLRESETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_CFAPEFULLRESETPOINTr(unit, rval));

    /* Configure the Qentry FAPs */
    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPOTPCONFIG_0r(unit, &rval));
    count = soc_reg_field_get(unit, QSTRUCT_FAPOTPCONFIG_0r, rval, MAXFULLSETf);
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPCONFIG_0r, &rval, FAPPOOLSIZEf, 4 * count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPCONFIG_0r(unit, rval));
    count -= QENTRY_HEADROOM;
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLSETPOINT_0r, &rval, FAPFULLSETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLSETPOINT_0r(unit, rval)); 
    count -= QENTRY_OFFSET; 
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLRESETPOINT_0r, &rval, FAPFULLRESETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLRESETPOINT_0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPOTPCONFIG_1r(unit, &rval));
    count = soc_reg_field_get(unit, QSTRUCT_FAPOTPCONFIG_1r, rval, MAXFULLSETf);
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPCONFIG_1r, &rval, FAPPOOLSIZEf, 4 * count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPCONFIG_1r(unit, rval));
    count -= QENTRY_HEADROOM;
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLSETPOINT_1r, &rval, FAPFULLSETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLSETPOINT_1r(unit, rval));
    count -= QENTRY_OFFSET; 
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLRESETPOINT_1r, &rval, FAPFULLRESETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLRESETPOINT_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPOTPCONFIG_2r(unit, &rval));
    count = soc_reg_field_get(unit, QSTRUCT_FAPOTPCONFIG_2r, rval, MAXFULLSETf);
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPCONFIG_2r, &rval, FAPPOOLSIZEf, 4 * count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPCONFIG_2r(unit, rval));
    count -= QENTRY_HEADROOM;
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLSETPOINT_2r, &rval, FAPFULLSETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLSETPOINT_2r(unit, rval));
    count -= QENTRY_OFFSET; 
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLRESETPOINT_2r, &rval, FAPFULLRESETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLRESETPOINT_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_QSTRUCT_FAPOTPCONFIG_3r(unit, &rval));
    count = soc_reg_field_get(unit, QSTRUCT_FAPOTPCONFIG_3r, rval, MAXFULLSETf);
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPCONFIG_3r, &rval, FAPPOOLSIZEf, 4 * count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPCONFIG_3r(unit, rval));
    count -= QENTRY_HEADROOM;
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLSETPOINT_3r, &rval, FAPFULLSETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLSETPOINT_3r(unit, rval));
    count -= QENTRY_OFFSET; 
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPFULLRESETPOINT_3r, &rval, FAPFULLRESETPOINTf, count);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPFULLRESETPOINT_3r(unit, rval));

    /* Initialize the FAPs */
    rval = 0;
    soc_reg_field_set(unit, CFAPIINITr, &rval, START_CFAPI_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CFAPIINITr(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, CFAPEINITr, &rval, START_CFAPE_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CFAPEINITr(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPINIT_0r, &rval, START_FAP_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_0r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPINIT_1r, &rval, START_FAP_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_1r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPINIT_2r, &rval, START_FAP_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_2r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, QSTRUCT_FAPINIT_3r, &rval, START_FAP_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_3r(unit, rval));

    sal_usleep(10000);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_CFAPIINITr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CFAPEINITr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_1r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_QSTRUCT_FAPINIT_3r(unit, rval));

    /* Enable all ports */
    COMPILER_64_ZERO(r64val);
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE0_64r, &r64val,
                          INPUT_PORT_RX_ENABLE_LOf, 0xffffffff);
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE0_64r, &r64val,
                          INPUT_PORT_RX_ENABLE_HIf, 0x1f); /* ports 0..36 */
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE0_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_INPUT_PORT_RX_ENABLE0_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_INPUT_PORT_RX_ENABLE0_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_INPUT_PORT_RX_ENABLE0_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_INPUT_PORT_RX_ENABLE0_64r(unit, r64val));

    COMPILER_64_ZERO(r64val);
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE1_64r, &r64val,
                          INPUT_PORT_RX_ENABLE_LOf, 0xffffffff);
    soc_reg64_field32_set(unit, INPUT_PORT_RX_ENABLE1_64r, &r64val,
                          INPUT_PORT_RX_ENABLE_HIf, 0x1); /* ports 37..69 */
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE1_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_INPUT_PORT_RX_ENABLE1_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_INPUT_PORT_RX_ENABLE1_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_INPUT_PORT_RX_ENABLE1_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_INPUT_PORT_RX_ENABLE1_64r(unit, r64val));

    SOC_IF_ERROR_RETURN(WRITE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDO_BYPASSr(unit, 0));

    if (soc_feature(unit, soc_feature_ddr3)) {
        /* Configure EMC */
        SOC_IF_ERROR_RETURN(READ_EMC_CFGr(unit, &rval));
        soc_reg_field_set(unit, EMC_CFGr, &rval, NUM_CISf, SOC_DDR3_NUM_MEMORIES(unit));
        soc_reg_field_set(unit, EMC_CFGr, &rval, DRAM_SIZEf, SOC_DDR3_NUM_ROWS(unit) / 8192 - 1);
        SOC_IF_ERROR_RETURN(WRITE_EMC_CFGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_FREE_POOL_SIZESr(unit, &rval));
        soc_reg_field_set(unit, EMC_FREE_POOL_SIZESr, &rval, WTFP_SIZEf, 240);
        soc_reg_field_set(unit, EMC_FREE_POOL_SIZESr, &rval, RSFP_SIZEf, 
                SOC_DDR3_NUM_MEMORIES(unit) == 1 ? 64 : 128);       
        SOC_IF_ERROR_RETURN(WRITE_EMC_FREE_POOL_SIZESr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_IWRB_SIZEr(unit, &rval));
        soc_reg_field_set(unit, EMC_IWRB_SIZEr, &rval, IWRB_SIZEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_EMC_IWRB_SIZEr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_IRRB_THRESHOLDSr(unit, &rval));
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_Q_XOFF_THRESHOLDf, 112);
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_UNDERRUN_Q_XON_THRESHOLDf, 112);
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_Q_XON_THRESHOLDf, 78);
        SOC_IF_ERROR_RETURN(WRITE_EMC_IRRB_THRESHOLDSr(unit, rval));
    }

    PBMP_ALL_ITER(unit, port) {
        pg_num = (port >= 25 && port <= 28) ? 7 : 0;

        /* Ports 27,32,33,34 are ports in one MXQ block.
         * If more than 2 lanes are enabled then pg_num = 0.
         * Same with ports 28,29,30,31.
         */
        if ((port == 27) && (SOC_PORT_VALID(unit, 32) ||
            SOC_PORT_VALID(unit, 33) || SOC_PORT_VALID(unit, 34))) {
            pg_num = 0;
        } else if ((port == 28) && (SOC_PORT_VALID(unit, 29) ||
            SOC_PORT_VALID(unit, 30) || SOC_PORT_VALID(unit, 31))) {
            pg_num = 0;
        }

        rval = 0;
        soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval, PORT_MAX_PKT_SIZEf,49);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MAX_PKT_SIZEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MAX_PKT_SIZEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_MAX_PKT_SIZEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_MAX_PKT_SIZEr(unit, port, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MIN_CELLr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MIN_CELLr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_CELLr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_MIN_CELLr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_MIN_CELLr(unit, port, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MAX_SHARED_CELLr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MAX_SHARED_CELLr(unit, port, rval));
        soc_reg_field_set(unit, PORT_MAX_SHARED_CELLr, &rval, PORT_MAXf,5226);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_SHARED_CELLr(unit, port, rval));
        soc_reg_field_set(unit, PORT_MAX_SHARED_CELLr, &rval, PORT_MAXf,3300);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_MAX_SHARED_CELLr(unit, port, rval));
        soc_reg_field_set(unit, PORT_MAX_SHARED_CELLr, &rval, PORT_MAXf,129628);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_MAX_SHARED_CELLr(unit, port, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_RESUME_LIMIT_CELLr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_RESUME_LIMIT_CELLr(unit, port, rval));
        soc_reg_field_set(unit, PORT_RESUME_LIMIT_CELLr, &rval, CELLSf,5208);
        SOC_IF_ERROR_RETURN(WRITE_PORT_RESUME_LIMIT_CELLr(unit, port, rval));
        soc_reg_field_set(unit, PORT_RESUME_LIMIT_CELLr, &rval, CELLSf,3298);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_RESUME_LIMIT_CELLr(unit, port, rval));
        soc_reg_field_set(unit, PORT_RESUME_LIMIT_CELLr, &rval, CELLSf,129574);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_RESUME_LIMIT_CELLr(unit, port, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_XON_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_XON_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_PRI_XON_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PRI_XON_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PRI_XON_ENABLEr(unit, port, rval));

        rval = 0;
        soc_reg_field_set(unit, PORT_SHARED_MAX_PG_ENABLEr, &rval, PG_BMPf,0xff);
        SOC_IF_ERROR_RETURN(WRITE_PORT_SHARED_MAX_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_SHARED_MAX_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_SHARED_MAX_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_SHARED_MAX_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_SHARED_MAX_PG_ENABLEr(unit, port, rval));

        rval = 0;
        soc_reg_field_set(unit, PORT_MIN_PG_ENABLEr, &rval, PG_BMPf,0xff);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MIN_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MIN_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MIN_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_MIN_PG_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_MIN_PG_ENABLEr(unit, port, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_MIN_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_MIN_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_MIN_CELLr, &rval, PG_MINf, 0x31);
        SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_MIN_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_MIN_CELLr(unit, port, pg_num, rval));

        rval = 0;
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval, PG_SHARED_DYNAMICf, 1);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval, PG_SHARED_LIMITf, 7);
        SOC_IF_ERROR_RETURN(WRITE_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));

        rval = 0;
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_GEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_HDRM_LIMITf, 127);
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_HDRM_LIMITf, 114);
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_GEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_HDRM_LIMITf, 3078);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval, PG_RESET_OFFSETf, 18);
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval, PG_RESET_OFFSETf, 2);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval, PG_RESET_OFFSETf, 54);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
    }

    /* CPU Port */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_HDRM_LIMIT_CELLr(unit, 0, 0, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_HDRM_LIMIT_CELLr(unit, 0, 0, rval));
    soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_GEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit,0,0,rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_HDRM_LIMIT_CELLr(unit, 0, 0, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_HDRM_LIMIT_CELLr(unit, 0, 0, rval));


    /* MXQ Ports */
    PBMP_MXQ_ITER(unit, port) {
        pg_num = (port >= 25 && port <= 28) ? 7 : 0;
        rval = (port >= 25 && port <= 28) ? 0xffffff : 0; /* PRIx_GRP = 0x7 */

        /* Ports 27,32,33,34 are ports in one MXQ block.
         * If more than 2 lanes are enabled then pg_num = 0.
         * Same with ports 28,29,30,31.
         */
        if ((port == 27) && (SOC_PORT_VALID(unit, 32) ||
            SOC_PORT_VALID(unit, 33) || SOC_PORT_VALID(unit, 34))) {
            pg_num = 0;
            rval = 0;
        } else if ((port == 28) && (SOC_PORT_VALID(unit, 29) ||
            SOC_PORT_VALID(unit, 30) || SOC_PORT_VALID(unit, 31))) {
            pg_num = 0;
            rval = 0;
        }
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_GRP0r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_GRP1r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_PRI_GRP0r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_PRI_GRP1r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PRI_GRP0r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PRI_GRP1r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PRI_GRP0r(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PRI_GRP1r(unit, port, rval));

        switch (port) {
            case 25:
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_25_PRI_GRP0r(unit, rval));
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_25_PRI_GRP1r(unit, rval));
                break;
            case 26:
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_26_PRI_GRP0r(unit, rval));
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_26_PRI_GRP1r(unit, rval));
                break;
            case 27:
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_27_PRI_GRP0r(unit, rval));
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_27_PRI_GRP1r(unit, rval));
                break;
            case 28:
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_28_PRI_GRP0r(unit, rval));
                SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_HIGIG_28_PRI_GRP1r(unit, rval));
                break;
            default:
                break;
        }

        rval = 0;
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval, PG_SHARED_DYNAMICf, 1);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval, PG_SHARED_LIMITf, pg_num);
        SOC_IF_ERROR_RETURN(WRITE_PG_SHARED_LIMIT_CELLr(unit,port,pg_num,rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_SHARED_LIMIT_CELLr(unit, port, pg_num, rval));

        rval = 0;
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_GEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_HDRM_LIMITf, 172);
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMIT_CELLr(unit,port,pg_num,rval));
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_HDRM_LIMITf, 157);
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_GEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_HDRM_LIMIT_CELLr, &rval, PG_HDRM_LIMITf, 4239);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_HDRM_LIMIT_CELLr(unit, port, pg_num, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval, PG_RESET_OFFSETf, 18);
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSET_CELLr(unit,port,pg_num,rval));
        soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval, PG_RESET_OFFSETf, 2);
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_RESET_OFFSET_CELLr, &rval, PG_RESET_OFFSETf, 54);
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_RESET_OFFSET_CELLr(unit, port, pg_num, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_FLOOR_CELLr(unit,port,pg_num,rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_RESET_FLOOR_CELLr(unit, port, pg_num, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PG_MIN_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PG_MIN_CELLr(unit, port, pg_num, rval));
        soc_reg_field_set(unit, PG_MIN_CELLr, &rval, PG_MINf, 0x31);
        SOC_IF_ERROR_RETURN(WRITE_PG_MIN_CELLr(unit,port,pg_num,rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PG_MIN_CELLr(unit, port, pg_num, rval));
        SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PG_MIN_CELLr(unit, port, pg_num, rval));

    }

    /* Input port shared space */
/*    rval = 0;
    soc_reg_field_set(unit, USE_SP_SHAREDr, &rval, ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_USE_SP_SHAREDr(unit, rval));
*/
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, 5226);
    SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, 3300);
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, 129628);
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_BUFFER_CELL_LIMIT_SPr(unit,0,rval));

    SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf, 7);
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf, 63);
    SOC_IF_ERROR_RETURN(WRITE_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf, 189);
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));

    /* Input port per-device global headroom */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_GLOBAL_HDRM_LIMITr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_GLOBAL_HDRM_LIMITr(unit, rval));
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, GLOBAL_HDRM_LIMITf, 107);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_GLOBAL_HDRM_LIMITr(unit, rval));
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, GLOBAL_HDRM_LIMITf, 2889);
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_GLOBAL_HDRM_LIMITr(unit, rval));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);

    SOC_IF_ERROR_RETURN(READ_THDO_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, STAT_CLEARf, 0);
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, PARITY_CHK_ENf, 
            parity_enable ? 1 : 0);
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, PARITY_GEN_ENf,
            parity_enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_THDO_MISCCONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, PARITY_STAT_CLRf, 0);
    soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, PARITY_CHK_ENf,
            parity_enable ? 1 : 0);
    soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, PARITY_GEN_ENf,
            parity_enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, EARLY_E2E_SELECTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLIr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLIr, &rval,
                             OP_BUFFER_SHARED_LIMIT_CELLIf, 10490);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLIr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLEr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLEr, &rval,
                           OP_BUFFER_SHARED_LIMIT_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLEr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_QENTRYr, &rval,
                           OP_BUFFER_SHARED_LIMIT_QENTRYf, 261712);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_QENTRYr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDORQEQr, &rval,
                         OP_BUFFER_SHARED_LIMITf, 8191);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDORQEQr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDOEMAr, &rval,
                       OP_BUFFER_SHARED_LIMITf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDOEMAr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr, &rval,
                           OP_BUFFER_SHARED_LIMIT_RESUME_CELLIf, 10427);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLIr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_CELLEr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLEr, &rval,
                         OP_BUFFER_SHARED_LIMIT_RESUME_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLEr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr, &rval,
                         OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYf, 261523);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr, &rval,
                       OP_BUFFER_SHARED_LIMIT_RESUMEf, 8185);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr, &rval,
                     OP_BUFFER_SHARED_LIMIT_RESUMEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_CELLIr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLIr, &rval,
                     OP_BUFFER_LIMIT_YELLOW_CELLIf, 1311);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLIr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_CELLEr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLEr, &rval,
                           OP_BUFFER_LIMIT_YELLOW_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_CELLEr(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_QENTRYr, &rval,
                           OP_BUFFER_LIMIT_YELLOW_QENTRYf, 32714);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_QENTRYr(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_THDORQEQr, &rval,
                         OP_BUFFER_LIMIT_YELLOWf, 1023);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_THDORQEQr(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_YELLOW_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_THDOEMAr, &rval,
                       OP_BUFFER_LIMIT_YELLOWf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_YELLOW_THDOEMAr(unit, rval));    
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr, &rval,
                           OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIf, 1303);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLIr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEr, &rval,
                         OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_CELLEr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr, &rval,
                         OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYf, 32690);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_QENTRYr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr, &rval,
                       OP_BUFFER_LIMIT_RESUME_YELLOWf, 1022);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_THDORQEQr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr, &rval,
                     OP_BUFFER_LIMIT_RESUME_YELLOWf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_YELLOW_THDOEMAr(unit, rval));    

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_CELLIr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLIr, &rval,
                     OP_BUFFER_LIMIT_RED_CELLIf, 1311);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLIr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_CELLEr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLEr, &rval,
                           OP_BUFFER_LIMIT_RED_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_CELLEr(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_QENTRYr, &rval,
                           OP_BUFFER_LIMIT_RED_QENTRYf, 32714);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_QENTRYr(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_THDORQEQr, &rval,
                         OP_BUFFER_LIMIT_REDf, 1023);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_THDORQEQr(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RED_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_THDOEMAr, &rval,
                       OP_BUFFER_LIMIT_REDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RED_THDOEMAr(unit, rval));    
    
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_CELLIr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLIr, &rval,
                           OP_BUFFER_LIMIT_RESUME_RED_CELLIf, 1303);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_CELLIr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_CELLEr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLEr, &rval,
                         OP_BUFFER_LIMIT_RESUME_RED_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_CELLEr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_QENTRYr, &rval,
                         OP_BUFFER_LIMIT_RESUME_RED_QENTRYf, 32690);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_QENTRYr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr, &rval,
                       OP_BUFFER_LIMIT_RESUME_REDf, 1022);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_THDORQEQr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr, &rval,
                     OP_BUFFER_LIMIT_RESUME_REDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_LIMIT_RESUME_RED_THDOEMAr(unit, rval));

    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEIr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval, Q_SHARED_LIMITf, 7079);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEIr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEEr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEEr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval, Q_SHARED_LIMITf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEEr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval, Q_SHARED_LIMITf, 8191);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEQr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDOEMAr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDOEMAr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval, Q_SHARED_LIMITf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDOEMAr(unit, i, rval));
    }

    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEIr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEIr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEEr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEEr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEEr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEQr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDOEMAr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDOEMAr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDOEMAr(unit, i, rval));
    }

    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr, &rval, Q_RESET_OFFSETf, 2);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr, &rval, Q_RESET_OFFSETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr, &rval, Q_RESET_OFFSETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr, &rval, Q_RESET_OFFSETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, i, rval));
    }
    for(i=0; i<272; i++) { /* 8 x 28 ports  + 48 CPU */
        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_CELLm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_SHARED_LIMIT_CELLf, 10490);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_MIN_CELLf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_LIMIT_ENABLE_CELLf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_LIMIT_DYNAMIC_CELLf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_COLOR_ENABLE_CELLf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_CELLm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                                    RESET_OFFSET_CELLf, 2);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_CELLm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_SHARED_LIMIT_QENTRYf, 261712);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_MIN_QENTRYf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_LIMIT_ENABLE_QENTRYf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_LIMIT_DYNAMIC_QENTRYf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_COLOR_ENABLE_QENTRYf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                                    RESET_OFFSET_QENTRYf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));
    }

    /* Helper function for MMU settings based on
       memory selection (internal/external/both) and mode (lossless/lossy) */
    SOC_IF_ERROR_RETURN(_soc_katana_mmu_init_helper(unit));

    /* Initialize MMU internal/external aging limit memory */
    count = soc_mem_index_count(unit, MMU_AGING_LMT_INTm); 
    sal_memset(&age_entry, 0, sizeof(mmu_aging_lmt_int_entry_t));
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_AGING_LMT_INTm(unit,
                            MEM_BLOCK_ANY, i, &age_entry));
    }

    count = soc_mem_index_count(unit, MMU_AGING_LMT_EXTm); 
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_AGING_LMT_EXTm(unit,
                            MEM_BLOCK_ANY, i, &age_entry));
    }

    SOC_IF_ERROR_RETURN(READ_THDO_DROP_CTR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, THDO_DROP_CTR_CONFIGr, &rval, OP_CNT_CFGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_THDO_DROP_CTR_CONFIGr(unit, rval));

    if (soc_feature(unit, soc_feature_post)) {
        SOC_IF_ERROR_RETURN(soc_mmu_post_init(unit));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_katana_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &rval));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_katana_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_katana_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 rval;

    rval = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, rval));

    return SOC_E_NONE;
}

/* *************** WORKAROUND ***********************************************
 * In Enduro-2 Variant, Access to LLS is inhibited by CI blocks, which share the
 * sbus ring. DDR clock is not present in this variant. So, the WAR is to use the
 * TAP Interface to propagate coreclk to flops that were on DDR clock so that
 * even if DDR clock is not running, the reset sync flops can get some clock pulses
 *   ***  DO NOT CHANGE THIS SEQUENCE UNLESS YOU KNOW WHAT YOU ARE DOING  ***
 */
#define TAP_SEQ_1(_i, _j) do { \
            WRITE_TOP_TAP_CONTROLr(unit,0xc); \
            for (i=0; i<(_i); i++) { \
                WRITE_TOP_TAP_CONTROLr(unit,0xd); \
                WRITE_TOP_TAP_CONTROLr(unit,0x9); \
            } \
            WRITE_TOP_TAP_CONTROLr(unit,0xd); \
            for (i=0; i<(_j); i++) { \
                WRITE_TOP_TAP_CONTROLr(unit,0xc); \
                WRITE_TOP_TAP_CONTROLr(unit,0x8); \
            }\
        } while(0)
    
#define TAP_SEQ_2(_i) do { \
            WRITE_TOP_TAP_CONTROLr(unit,0xe); \
            WRITE_TOP_TAP_CONTROLr(unit,0xa); \
            WRITE_TOP_TAP_CONTROLr(unit,0xe); \
            for (i=0; i<(_i); i++) { \
                WRITE_TOP_TAP_CONTROLr(unit,0xc); \
                WRITE_TOP_TAP_CONTROLr(unit,0x8); \
            } \
        } while(0)

void 
_kt_lls_workaround(int unit)
{
    int i,j;

	WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    WRITE_TOP_TAP_CONTROLr(unit,0xe); 
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0x8);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0x8);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);

    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);

    TAP_SEQ_2(3);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(6,3);
    TAP_SEQ_1(8,1);
    TAP_SEQ_1(9,0);
    WRITE_TOP_TAP_CONTROLr(unit,0x9);
    WRITE_TOP_TAP_CONTROLr(unit,0xd);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    WRITE_TOP_TAP_CONTROLr(unit,0xb);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(21);
    TAP_SEQ_1(1,341);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(2);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);

    sal_usleep(1);

    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(3);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(6,3);
    TAP_SEQ_1(8,1);
    TAP_SEQ_1(9,0);
    WRITE_TOP_TAP_CONTROLr(unit,0x9);
    WRITE_TOP_TAP_CONTROLr(unit,0xd);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    WRITE_TOP_TAP_CONTROLr(unit,0xb);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(14);
    TAP_SEQ_1(2,5);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,338);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(3);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(3,1);
    TAP_SEQ_1(2,1);
    TAP_SEQ_1(10,1);
    TAP_SEQ_1(9,0);
    WRITE_TOP_TAP_CONTROLr(unit,0x9);
    WRITE_TOP_TAP_CONTROLr(unit,0xd);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    WRITE_TOP_TAP_CONTROLr(unit,0xb);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(4);
    TAP_SEQ_1(1,703);
    TAP_SEQ_1(22,1);
    for (j=0; j<9; j++) {
        TAP_SEQ_1(32,1);;
    }
    TAP_SEQ_1(32,17);
    TAP_SEQ_1(1,3);
    TAP_SEQ_1(1,3);
    TAP_SEQ_1(3,7);
    TAP_SEQ_1(1,10);
    TAP_SEQ_1(1,5);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,2);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(3);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(6,3);
    TAP_SEQ_1(8,1);
    TAP_SEQ_1(9,0);
    WRITE_TOP_TAP_CONTROLr(unit,0x9);
    WRITE_TOP_TAP_CONTROLr(unit,0xd);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    WRITE_TOP_TAP_CONTROLr(unit,0xb);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(14);
    TAP_SEQ_1(2,2);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,338);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(3);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(6,3);
    TAP_SEQ_1(8,1);
    TAP_SEQ_1(9,0);
    WRITE_TOP_TAP_CONTROLr(unit,0x9);
    WRITE_TOP_TAP_CONTROLr(unit,0xd);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    WRITE_TOP_TAP_CONTROLr(unit,0xb);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(5);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);

    sal_usleep(4);

    WRITE_TOP_TAP_CONTROLr(unit,0x8);

    for (j=0; j<9; j++) {
        WRITE_TOP_TAP_CONTROLr(unit,0xc);
        WRITE_TOP_TAP_CONTROLr(unit,0x8);
    }
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,338);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(5);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);

    sal_usleep(4);

    WRITE_TOP_TAP_CONTROLr(unit,0x8);
    for (j=0; j<9; j++) {
        WRITE_TOP_TAP_CONTROLr(unit,0xc);
        WRITE_TOP_TAP_CONTROLr(unit,0x8);
    }
    TAP_SEQ_1(2,1);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,338);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(15);
    TAP_SEQ_1(2,1);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(2,2);
    TAP_SEQ_1(1,338);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(15);
    TAP_SEQ_1(2,3);
    TAP_SEQ_1(2,2);
    TAP_SEQ_1(1,338);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);

    sal_usleep(15);

    WRITE_TOP_TAP_CONTROLr(unit,0x8);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);

    sal_usleep(1200);

    WRITE_TOP_TAP_CONTROLr(unit,0x8);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(3);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(6,3);
    TAP_SEQ_1(8,1);
    TAP_SEQ_1(9,0);
    WRITE_TOP_TAP_CONTROLr(unit,0x9);
    WRITE_TOP_TAP_CONTROLr(unit,0xd);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    WRITE_TOP_TAP_CONTROLr(unit,0xb);
    WRITE_TOP_TAP_CONTROLr(unit,0xf);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(15);
    TAP_SEQ_1(2,1);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(2,2);
    TAP_SEQ_1(1,338);
    TAP_SEQ_1(2,13);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    TAP_SEQ_2(5);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);

    sal_usleep(4);

    WRITE_TOP_TAP_CONTROLr(unit,0x8);
    for (j=0; j<9; j++) {
        WRITE_TOP_TAP_CONTROLr(unit,0xc);
        WRITE_TOP_TAP_CONTROLr(unit,0x8);
    }
    TAP_SEQ_1(1,2);
    TAP_SEQ_1(1,1);
    TAP_SEQ_1(2,2);
    TAP_SEQ_1(1,14);
    WRITE_TOP_TAP_CONTROLr(unit,0xe);
    WRITE_TOP_TAP_CONTROLr(unit,0xa);
    TAP_SEQ_2(1);
    WRITE_TOP_TAP_CONTROLr(unit,0xc);
    for (i=0; i<6; i++) {
        WRITE_TOP_TAP_CONTROLr(unit,0x0);
    }
}


/*
 * Katana chip driver functions.
 */
soc_functions_t soc_katana_drv_funs = {
    _soc_katana_misc_init,
    _soc_katana_mmu_init,
    _soc_katana_age_timer_get,
    _soc_katana_age_timer_max_get,
    _soc_katana_age_timer_set,
};


static const soc_reg_t thermal_reg[] = {
    TOP_THERMAL_PVTMON_RESULT_0r, TOP_THERMAL_PVTMON_RESULT_1r,
    TOP_THERMAL_PVTMON_RESULT_2r, TOP_THERMAL_PVTMON_RESULT_3r
};

int 
_soc_katana_temperature_monitor_init(int unit) {
    uint32 rval;
    
    SOC_IF_ERROR_RETURN(READ_TOP_THERMAL_PVTMON_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_THERMAL_PVTMON_CTRL_2r, &rval, PVTMON_RSTBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_THERMAL_PVTMON_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_THERMAL_PVTMON_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_THERMAL_PVTMON_CTRLr, &rval, PROG_RESISTERf, 3);
    soc_reg_field_set(unit, TOP_THERMAL_PVTMON_CTRLr, &rval, BG_ADJf, 3);
    soc_reg_field_set(unit, TOP_THERMAL_PVTMON_CTRLr, &rval, MEASUREMENT_CALLIBRATIONf, 5);    
    SOC_IF_ERROR_RETURN(WRITE_TOP_THERMAL_PVTMON_CTRLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_THERMAL_PVTMON_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_THERMAL_PVTMON_CTRL_2r, &rval, PVTMON_SELECTf, 0);
    soc_reg_field_set(unit, TOP_THERMAL_PVTMON_CTRL_2r, &rval, PVTMON_RSTBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_THERMAL_PVTMON_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_THERMAL_PVTMON_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_THERMAL_PVTMON_CTRL_2r, &rval, PVTMON_RSTBf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_THERMAL_PVTMON_CTRL_2r(unit, rval));

    sal_usleep(1000);

    return SOC_E_NONE;
}    

int
soc_katana_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    uint32 rval;
    soc_reg_t reg;
    int index;
    int fval, cur, peak;
    int num_entries_out;

    *temperature_count = 0;
    if (COUNTOF(thermal_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(thermal_reg);
    }
    /* Check to see if the thermal monitor is initialized */
    SOC_IF_ERROR_RETURN(READ_TOP_THERMAL_PVTMON_CTRL_2r(unit, &rval));
    if ((soc_reg_field_get(unit, TOP_THERMAL_PVTMON_CTRL_2r, rval, PVTMON_RSTBf) == 0) ||
        (soc_reg_field_get(unit, TOP_THERMAL_PVTMON_CTRL_2r, rval, PVTMON_SELECTf) != 0)) {
        SOC_IF_ERROR_RETURN(_soc_katana_temperature_monitor_init(unit));
    }

    for (index = 0; index < num_entries_out; index++) {
        reg = thermal_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (soc_reg_field_valid(unit, reg, PVTTEMP_DATAf)) {
            fval = soc_reg_field_get(unit, reg, rval, PVTTEMP_DATAf);
        } else {
            fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        }
        cur = (4180000 - (5556 * fval)) / 1000;        
        if (soc_reg_field_valid(unit, reg, PEAK_PVTTEMP_DATAf)) {
            fval = soc_reg_field_get(unit, reg, rval, PEAK_PVTTEMP_DATAf);
        } else {
            fval = soc_reg_field_get(unit, reg, rval, PEAK_TEMP_DATAf);
        }
        peak = (4180000 - (5556 * fval)) / 1000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak    = peak;
    }
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON0_PEAK_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON1_PEAK_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON2_PEAK_DATA_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON3_PEAK_DATA_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROLr(unit, rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON0_PEAK_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON1_PEAK_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON2_PEAK_DATA_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                      THERMAL_PVTMON3_PEAK_DATA_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROLr(unit, rval));
    *temperature_count=num_entries_out;

    return SOC_E_NONE;
}

int
soc_katana_show_ring_osc(int unit)
{
    static char *names[] = {
        "IO ring 0", "IO ring 1", "Core ring 0", "Core ring 1"
    };
    uint32 rval;
    int ring, div, quo, rem, frac;
    int cmc = SOC_PCI_CMC(unit);
    int core_clk = 133 * 1024;

    for (ring = 0; ring < 4; ring++) {
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_3r, &rval, OSC_ENABLEf, 1);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_3r, &rval, OSC_SELf, ring);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_3r(unit, rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_3r, &rval, OSC_CNT_RSTBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_3r(unit, rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_3r, &rval, OSC_CNT_STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_3r(unit, rval));

        sal_usleep(1000);

        rval = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc));
        /* SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_IRQ_STAT3r(unit, &rval)); */
        div = rval & 0xffff;
        if (0 == div) {
            quo = 0;
            frac = 0;
        } else {
            quo = core_clk / div;
            rem = core_clk - quo * div;
            frac = (rem * 10000) / div;
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s: %d.%04d Mhz\n"),
                 names[ring], quo, frac));
    }

    return SOC_E_NONE;
}

STATIC int
soc_kt_oam_interrupt_process(int unit)
{
    uint32  irqStat;
    int rv = SOC_E_NONE;
    soc_field_t fault_field = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, IP1_INTR_STATUS_1r,
                        REG_PORT_ANY, 0, &irqStat));
    if ((irqStat & KT_OAM_INTR_MASK) != 0) {
        if (en_oam_handler[unit] != NULL) {
            rv =  en_oam_handler[unit](unit, fault_field);
        }
    }
    return rv;
}

int
soc_kt_lls_bmap_alloc(int unit)
{
    soc_control_t *soc;
    int rv = SOC_E_NONE;
    int port;
    int alloc_size_l0, alloc_size_l1, alloc_size_l2;

    soc = SOC_CONTROL(unit);

    /* First cleanup if any allocation was done earlier.
     * This is needed to make sure that on subsequent rc
     * there is no memory leak due to MMU LLS usage bitmap
     * allocation.
     */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (soc->port_lls_l0_bmap[port] != NULL) {
            sal_free(soc->port_lls_l0_bmap[port]);
            soc->port_lls_l0_bmap[port] = NULL;
        }
        if (soc->port_lls_l1_bmap[port] != NULL) {
            sal_free(soc->port_lls_l1_bmap[port]);
            soc->port_lls_l1_bmap[port] = NULL;
        }
        if (soc->port_lls_l2_bmap[port] != NULL) {
            sal_free(soc->port_lls_l2_bmap[port]);
            soc->port_lls_l2_bmap[port] = NULL;
        }
    }

    alloc_size_l0 =
        SHR_BITALLOCSIZE(soc_mem_index_count(unit, LLS_L0_PARENTm));
    alloc_size_l1 =
        SHR_BITALLOCSIZE(soc_mem_index_count(unit, LLS_L1_PARENTm));
    alloc_size_l2 =
        SHR_BITALLOCSIZE(soc_mem_index_count(unit, LLS_L2_PARENTm));

    PBMP_ALL_ITER(unit, port) {
        soc->port_lls_l0_bmap[port] = sal_alloc(alloc_size_l0, "LLS_L0_BMAP");
        if (soc->port_lls_l0_bmap[port] == NULL) {
            rv = SOC_E_MEMORY;
            goto clean_up;
        }
        soc->port_lls_l1_bmap[port] = sal_alloc(alloc_size_l1, "LLS_L1_BMAP");
        if (soc->port_lls_l1_bmap[port] == NULL) {
            rv = SOC_E_MEMORY;
            goto clean_up;
        }
        soc->port_lls_l2_bmap[port] = sal_alloc(alloc_size_l2, "LLS_L2_BMAP");
        if (soc->port_lls_l2_bmap[port] == NULL) {
            rv = SOC_E_MEMORY;
            goto clean_up;
        }
        sal_memset(soc->port_lls_l0_bmap[port], 0, alloc_size_l0);
        sal_memset(soc->port_lls_l1_bmap[port], 0, alloc_size_l1);
        sal_memset(soc->port_lls_l2_bmap[port], 0, alloc_size_l2);
    }

    clean_up:
    if (SOC_FAILURE(rv))  {
        PBMP_ALL_ITER(unit, port) {
	    if (soc->port_lls_l1_bmap[port] != NULL) {
		sal_free(soc->port_lls_l1_bmap[port]);
                soc->port_lls_l1_bmap[port] = NULL;
	    }
	    if (soc->port_lls_l0_bmap[port] != NULL) {
	       sal_free(soc->port_lls_l0_bmap[port]);
               soc->port_lls_l0_bmap[port] = NULL;
	    }
	    if (soc->port_lls_l2_bmap[port] != NULL) {
	       sal_free(soc->port_lls_l2_bmap[port]);
               soc->port_lls_l2_bmap[port] = NULL;
	    }
        }
    }
    return rv;
}

int 
soc_kt_port_flush_state_set(int unit, int port, int enable) 
{
    soc_pbmp_t port_flush_pbmp;

    SOC_PBMP_CLEAR(port_flush_pbmp);
    if (enable) {
        SOC_LLS_SCHEDULER_LOCK(unit);
        port_flush_pbmp = SOC_CONTROL(unit)->port_flush_pbmp;
        SOC_PBMP_PORT_ADD(port_flush_pbmp, port);
        SOC_LLS_SCHEDULER_UNLOCK(unit);
    } else {
        SOC_LLS_SCHEDULER_LOCK(unit);
        SOC_PBMP_PORT_REMOVE(port_flush_pbmp, port);
        SOC_LLS_SCHEDULER_UNLOCK(unit);
    }
    return SOC_E_NONE; 
}

int soc_kt_port_flush_state_get(int unit, int port, int *mode) 
{
    soc_pbmp_t port_flush_pbmp;

    SOC_PBMP_CLEAR(port_flush_pbmp);

    port_flush_pbmp = SOC_CONTROL(unit)->port_flush_pbmp;
    if (SOC_PBMP_MEMBER(port_flush_pbmp, port)) {
        /* Port is in flush state */
        *mode = 1;
    } else {
        /* Port is not in flush state */
        *mode = 0;
    } 
    return SOC_E_NONE;
}
  
int
soc_kt_cosq_max_bucket_set(int unit, int port,
                        int index, uint32 level)
{
    uint32 idx = 0;
    uint32 i = 0;
    lls_port_shaper_config_c_entry_t port_entry;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    soc_mem_t config_mem[2];
    static const soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    static const soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    static const soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
       C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
       C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
    };
    static const soc_field_t burst_mant_fields[] = {
       C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
       C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
       C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
       C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
    };
    static const soc_field_t cycle_sel_fields[] = {
        C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
        C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,
        C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
        C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
    };

    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXPf, C_MIN_REF_RATE_EXPf
    };
    soc_field_t rate_mant_f[] = {
        C_MAX_REF_RATE_MANTf, C_MIN_REF_RATE_MANTf
    };
    soc_field_t burst_exp_f[] = {
        C_MAX_THLD_EXPf, C_MIN_THLD_EXPf
    };
    soc_field_t burst_mant_f[] = {
        C_MAX_THLD_MANTf, C_MIN_THLD_MANTf
    };
    soc_field_t cycle_sel_f[] = {
        C_MAX_CYCLE_SELf, C_MIN_CYCLE_SELf
    };

    /* set Min/Max threshold shapers to 33Gbps */
    switch (level)  {
        case _SOC_KT_COSQ_NODE_LEVEL_ROOT:
            config_mem[0] = LLS_PORT_SHAPER_CONFIG_Cm;

            sal_memset(&port_entry, 0,
                       sizeof(lls_port_shaper_config_c_entry_t));
            SOC_IF_ERROR_RETURN
               (soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &port_entry));
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_exp_f[0], KT_QUEUE_FLUSH_RATE_EXP);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_mant_f[0], KT_QUEUE_FLUSH_RATE_MANTISSA);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_exp_f[0], KT_QUEUE_FLUSH_BURST_EXP);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_mant_f[0], KT_QUEUE_FLUSH_BURST_MANTISSA);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                cycle_sel_f[0], KT_QUEUE_FLUSH_CYCLE_SEL);
            SOC_IF_ERROR_RETURN
                   (soc_mem_write(unit, config_mem[0],
                                       MEM_BLOCK_ALL, index, &port_entry));
           break;
        case _SOC_KT_COSQ_NODE_LEVEL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                sal_memset(&l0_entry, 0,
                             sizeof(lls_l0_shaper_config_c_entry_t));
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                     index, &l0_entry));
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_exp_f[i], KT_QUEUE_FLUSH_RATE_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_mant_f[i], KT_QUEUE_FLUSH_RATE_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_exp_f[i], KT_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_mant_f[i], KT_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    cycle_sel_f[i], KT_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                         (soc_mem_write(unit, config_mem[i],
                                    MEM_BLOCK_ALL, index, &l0_entry));
            }
            break;
        case _SOC_KT_COSQ_NODE_LEVEL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                sal_memset(&l1_entry, 0,
                            sizeof(lls_l1_shaper_config_c_entry_t));
                idx = (i * 4) + (index % 4);
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                   index/4, &l1_entry));
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_exp_fields[idx], KT_QUEUE_FLUSH_RATE_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_mant_fields[idx], KT_QUEUE_FLUSH_RATE_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_exp_fields[idx], KT_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_mant_fields[idx], KT_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    cycle_sel_fields[idx], KT_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                     (soc_mem_write(unit, config_mem[i],
                                    MEM_BLOCK_ALL, index/4, &l1_entry));
            }
            break;
        case _SOC_KT_COSQ_NODE_LEVEL_L2:
            if ((index % 8) < 4) {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_LOWERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_LOWER_Cm;
            } else {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_UPPERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_UPPER_Cm;
            }

            for ( i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);

                sal_memset(&l2_entry, 0,
                           sizeof(lls_l2_shaper_config_lower_entry_t));
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                  index/8, &l2_entry));
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_exp_fields[idx], KT_QUEUE_FLUSH_RATE_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_mant_fields[idx], KT_QUEUE_FLUSH_RATE_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_exp_fields[idx], KT_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_mant_fields[idx], KT_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    cycle_sel_fields[idx], KT_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, config_mem[i],
                                   MEM_BLOCK_ALL, index/8, &l2_entry));
            }
            break;
        default:
            return SOC_E_NONE;

    }

    return SOC_E_NONE;

}

int
_soc_kt_cosq_begin_port_flush(int unit, int port, int hw_index)
{
    uint32 rval = 0;
    uint32 ext_mem = 0;
    int flush_active = 0;
    uint32 timeout_val;
    soc_timeout_t timeout;

    if (IS_EXT_MEM_PORT(unit, port)) {
        ext_mem = 1;
    }


    SOC_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval));

    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_NUMf,1);
    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r,
                      &rval, Q_FLUSH_ID0f, hw_index);
    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_EXTERNALf,
                          ext_mem);
    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ACTIVEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOQ_QUEUE_FLUSH0r(unit, rval));


    /* Wait for flush completion */
    flush_active = 1;
    timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 1600000);
    soc_timeout_init(&timeout, timeout_val, 0);

    while (flush_active) {
        if (SOC_REG_IS_VALID(unit, TOQ_QUEUE_FLUSH0r)) {
            SOC_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval));
            flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval,
                                             Q_FLUSH_ACTIVEf);
        }

        if (soc_timeout_check(&timeout)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
               "ERROR: Port %d Queue flush operation failed for queue %d \n"),
                port, hw_index));
            return (SOC_E_TIMEOUT);

        }
    }
    return SOC_E_NONE;
}

int
_soc_kt_cosq_end_port_flush(int unit, int hw_index)
{
    uint32 rval = 0, q_bmp;

    KT_COSQ_LOCK(unit);

    soc_reg_field_set(unit, THDO_QUEUE_DISABLE_CFG2r, &rval, QUEUE_NUMf,
                      (hw_index & 0xfe0));
    SOC_IF_ERROR_RETURN(WRITE_THDO_QUEUE_DISABLE_CFG2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_THDO_QUEUE_DISABLE_CFG1r(unit, &rval));
    soc_reg_field_set(unit, THDO_QUEUE_DISABLE_CFG1r, &rval, QUEUE_RDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_THDO_QUEUE_DISABLE_CFG1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_THDO_QUEUE_DISABLE_STATUSr(unit, &rval));
    q_bmp = soc_reg_field_get(unit, THDO_QUEUE_DISABLE_STATUSr, rval,
                                           QUEUE_BITMAPf);

    if (q_bmp & (1 << (hw_index % 32))) {
        SOC_IF_ERROR_RETURN(READ_THDO_QUEUE_DISABLE_CFG2r(unit, &rval));
        soc_reg_field_set(unit, THDO_QUEUE_DISABLE_CFG2r, &rval, QUEUE_NUMf,
                                       hw_index );
        SOC_IF_ERROR_RETURN(WRITE_THDO_QUEUE_DISABLE_CFG2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_THDO_QUEUE_DISABLE_CFG1r(unit, &rval));
        soc_reg_field_set(unit, THDO_QUEUE_DISABLE_CFG1r, &rval, QUEUE_WRf, 1);
        SOC_IF_ERROR_RETURN(WRITE_THDO_QUEUE_DISABLE_CFG1r(unit, rval));
    }

    KT_COSQ_UNLOCK(unit);
    return SOC_E_NONE;
}
int soc_kt_port_flush(int unit, int port, int enable)
{
    soc_mem_t mem;
    uint32 *bmap;
    int  index, word, bit, word_count, index_count, count;
#ifdef BCM_KATANA2_SUPPORT
    int egr_enable = 0, rv = 0;
    egr_enable_entry_t egr_en;
#endif
    /* for queue flush */
    bmap =  SOC_CONTROL(unit)->port_lls_l2_bmap[port];
    mem = LLS_L2_PARENTm;
    index_count = soc_mem_index_count(unit, mem);
    word_count = _SHR_BITDCLSIZE(index_count);
    count = 0;
    for (word = (word_count - 1); word >= 0; word--) {
        if (bmap[word] == 0) {
            continue;
        }
        for (bit = (SHR_BITWID - 1); bit >= 0; bit--) {
            if (!(bmap[word] & (1 << bit))) {
                continue;
            }
            index = word * SHR_BITWID + bit;
            count++;

            if (enable == 1) {
                /* Set the threshold for L2 node to max shaper rate - 33 Gbps */
#ifdef BCM_SABER2_SUPPORT
                if (SOC_IS_SABER2(unit)) {
                    SOC_IF_ERROR_RETURN
                        (soc_sb2_cosq_max_bucket_set(unit, port,
                                                     index,
                                                     _SOC_KT2_COSQ_NODE_LVL_L2));
                    /* Flush the queues */
                    SOC_IF_ERROR_RETURN(_soc_kt2_cosq_begin_port_flush(
                                unit, port, index));

                } else 
#endif
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    SOC_IF_ERROR_RETURN
                       (soc_kt2_cosq_max_bucket_set(unit, port,
                                                   index,
                                    _SOC_KT2_COSQ_NODE_LVL_L2));
                    sal_memset(&egr_en, 0, sizeof(egr_enable_entry_t));
                    SOC_IF_ERROR_RETURN(READ_EGR_ENABLEm(unit, SOC_BLOCK_ANY, port, &egr_en));
                    egr_enable = soc_EGR_ENABLEm_field32_get(unit, &egr_en, PRT_ENABLEf);
                    if (!egr_enable) {
                        soc_mem_field32_set(unit, EGR_ENABLEm, &egr_en, PRT_ENABLEf, 1);
                        SOC_IF_ERROR_RETURN
                                    (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, &egr_en));
                    }
                    /* Flush the queues */
                    rv = _soc_kt2_cosq_begin_port_flush(unit, port, index);
                    if (!egr_enable) {
                        soc_mem_field32_set(unit, EGR_ENABLEm, &egr_en, PRT_ENABLEf, 0);
                        SOC_IF_ERROR_RETURN
                                    (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, &egr_en));
                    }
                    if (rv < 0) {
                        return rv;
                    }
                } else
#endif
                {
                    SOC_IF_ERROR_RETURN
                       (soc_kt_cosq_max_bucket_set(unit, port, index,
                                 _SOC_KT_COSQ_NODE_LEVEL_L2));
                    /* Flush the queues */
                    SOC_IF_ERROR_RETURN(_soc_kt_cosq_begin_port_flush(
                                            unit, port, index));
                }
            } else {
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit) || SOC_IS_SABER2(unit)) {
                    SOC_IF_ERROR_RETURN
                       (_soc_kt2_cosq_end_port_flush(
                                    unit, index));
                } else
#endif
                {
                    /* Enable the queues */
                    SOC_IF_ERROR_RETURN(_soc_kt_cosq_end_port_flush(
                                    unit, index));
                }
            }
        }
    }

    return SOC_E_NONE;
}

int
soc_kt_sched_get_node_config(int unit, soc_port_t port, int level, int index,
                              int *pnum_spri, int *pfirst_child,
                              uint32 *pspmap)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    uint32 num_spri = 0, f1, f2;
    int first_child = -1, ii;
    int sp_vec = soc_feature(unit, soc_feature_vector_based_spri);

    *pspmap = 0;

    mem = _SOC_KT_NODE_CONFIG_MEM(level);
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    if (sp_vec) {
        f1 = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
        f2 = soc_mem_field32_get(unit, mem, &entry, P_VECT_SPRI_7_4f);
        *pspmap = f1 | (f2 << 4);
        for (ii = 0; ii < 32; ii++) {
            if ((1 << ii) & *pspmap) {
                num_spri++;
            }
        }
    } else {
        num_spri = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
    }
    first_child = soc_mem_field32_get(unit, mem, &entry, P_START_SPRIf);

    if (pnum_spri) {
        *pnum_spri = num_spri;
    }

    if (pfirst_child) {
        *pfirst_child = first_child;
    }
    return SOC_E_NONE;
}

STATIC int
soc_kt_sched_weight_get(int unit, int level, int index, int *weight)
{
    soc_mem_t mem_weight;
    uint32 entry[SOC_MAX_MEM_WORDS];

    mem_weight = _SOC_KT_NODE_WIEGHT_MEM(level);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem_weight, MEM_BLOCK_ALL, index, &entry));

    *weight = soc_mem_field32_get(unit, mem_weight, &entry, C_WEIGHTf);
    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "sched_weight_get L%d index=%d wt=%d\n"),
              level, index, *weight));
    return SOC_E_NONE;
}

int
soc_kt_cosq_get_sched_mode(int unit, soc_port_t port, int level, int index,
                              soc_kt_sched_mode_e *pmode, int *weight)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    soc_kt_sched_mode_e mode = SOC_KT_SCHED_MODE_UNKNOWN;

    SOC_IF_ERROR_RETURN(soc_kt_sched_weight_get(unit, level, index, weight));

    if (*weight == 0) {
        mode = SOC_KT_SCHED_MODE_STRICT;
    } else {
        mem = _SOC_KT_NODE_CONFIG_MEM(_SOC_KT_COSQ_NODE_LEVEL_ROOT);
        if (port > 41){
            mem = LLS_S1_CONFIGm;
         }
        index = port;
        SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
        if (soc_mem_field32_get(unit, mem, entry, PACKET_MODE_WRR_ACCOUNTING_ENABLEf)) {
            mode = SOC_KT_SCHED_MODE_WRR;
        } else {
            mode = SOC_KT_SCHED_MODE_WDRR;
        }
    }

    if (pmode) {
        *pmode = mode;
    }

    return SOC_E_NONE;
}

#define _KT_INDEX_1K_ENTRIES  1023
#define _KT_OFFSET_1K_ENTRIES 1024
#define _KT_INDEX_2K_ENTRIES  2047
#define _KT_OFFSET_2K_ENTRIES 2048
#define _KT_INDEX_3K_ENTRIES  3071
#define _KT_OFFSET_3K_ENTRIES 3072

STATIC int
_soc_katana_l3_defip_urpf_index_map(int unit, int wide, int index)
{
    int new_index = index;
    int num_tcams = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int entries_left_in_paired_tcam = 0;
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit) / 2;
    
    if (wide == 0) {
        num_tcams = soc_defip_index_remap / tcam_size;
        entries_left_in_paired_tcam = tcam_size -
                                      (soc_defip_index_remap % tcam_size);
        if (index < entries_left_in_paired_tcam) {
            new_index = index + (num_tcams * 2 * tcam_size) +
                        (soc_defip_index_remap % tcam_size);
        } else if (index < 2 * entries_left_in_paired_tcam) {
            new_index = index + (num_tcams * 2 * tcam_size) +
                        ((soc_defip_index_remap % tcam_size) * 2);
        } else if (index < 3 * entries_left_in_paired_tcam) {
            new_index = index + (num_tcams * 2 * tcam_size) +
                        ((soc_defip_index_remap % tcam_size) * 3);
        } else {
            new_index = index + (num_tcams * 2 * tcam_size) +
                        ((soc_defip_index_remap % tcam_size) * 4);
        }
        return new_index;
    }
    if (index >= soc_defip_index_remap) {
        new_index = index + tcam_size - soc_defip_index_remap;
    }
    return new_index;
}

STATIC int
_soc_katana_l3_defip_urpf_index_remap(int unit, int wide, int index)
{
    int new_index = index;
    int num_tcams = 0;
    int tcam_size = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit) / 2;
    
    if (wide == 0) {
        num_tcams = soc_defip_index_remap / tcam_size;
                             
        if (index > _KT_INDEX_3K_ENTRIES + soc_defip_index_remap % tcam_size) {
            new_index = (index - (num_tcams * 2 * tcam_size)) - 
                             (soc_defip_index_remap % tcam_size) * 4;
        } else if (index > _KT_INDEX_2K_ENTRIES + soc_defip_index_remap % tcam_size) {
            new_index = (index - (num_tcams * 2 * tcam_size)) - 
                             (soc_defip_index_remap % tcam_size) * 3;
        } else if (index > _KT_INDEX_1K_ENTRIES + soc_defip_index_remap % tcam_size) {
            new_index = (index - (num_tcams * 2 * tcam_size)) - 
                             (soc_defip_index_remap % tcam_size) * 2;
        } else {
            new_index = (index - (num_tcams * 2 * tcam_size)) - 
                             soc_defip_index_remap % tcam_size;
        }
        return new_index;
    }
    if (index >= _KT_OFFSET_1K_ENTRIES) {
        new_index = index - _KT_OFFSET_1K_ENTRIES + soc_defip_index_remap;
    }
    return new_index;
}


/* Map logical (always starts from 0 and contiguous) index to physical index 
   which can have a starting offset and/or holes.
   Input  : logical index
   Returns: physical index */
int
soc_kt_l3_defip_index_map(int unit, soc_mem_t mem, int index)
{
    int wide = 0;
    
    if (SOC_CONTROL(unit)->l3_defip_index_remap == 0) {
        return index;
    }

    if (mem == L3_DEFIP_PAIR_128m ||
        mem == L3_DEFIP_PAIR_128_ONLYm ||
        mem == L3_DEFIP_PAIR_128_DATA_ONLYm ||
        mem == L3_DEFIP_PAIR_128_HIT_ONLYm) {
        wide = 1;
    }
    if (SOC_CONTROL(unit)->l3_defip_urpf) {
        return _soc_katana_l3_defip_urpf_index_map(unit, wide, index);
    } else {
        return soc_l3_defip_index_map(unit, wide, index);
    }
}

/* Reverse map physical index to logical index.
   Input  : physical index
   Returns: logical index */
int
soc_kt_l3_defip_index_remap(int unit, soc_mem_t mem, int index)
{
    int wide = 0;
    
    if (SOC_CONTROL(unit)->l3_defip_index_remap == 0) {
        return index;
    }

    if (mem == L3_DEFIP_PAIR_128m ||
        mem == L3_DEFIP_PAIR_128_ONLYm ||
        mem == L3_DEFIP_PAIR_128_DATA_ONLYm ||
        mem == L3_DEFIP_PAIR_128_HIT_ONLYm) {
        wide = 1;
    }
    if (SOC_CONTROL(unit)->l3_defip_urpf) {
        return _soc_katana_l3_defip_urpf_index_remap(unit, wide, index);
    } else {
        return soc_l3_defip_index_remap(unit, wide, index);
    }
}

/* Given a physical index (always in terms of the narrow entry) 
   return the logical index and memory type */
int
soc_kt_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem)
{
    int soc_defip_index_remap = SOC_L3_DEFIP_INDEX_REMAP_GET(unit) / 2;
    
    *mem = L3_DEFIPm;
    if (SOC_CONTROL(unit)->l3_defip_urpf) {
        if (SOC_CONTROL(unit)->l3_defip_index_remap == 0) {
            *mem = L3_DEFIPm;
            return pindex;  
        }
        if (pindex < soc_defip_index_remap) {
            *mem = L3_DEFIP_PAIR_128m;
            return pindex;
        } else if ((pindex > _KT_INDEX_1K_ENTRIES) && 
        	         (pindex < _KT_OFFSET_1K_ENTRIES + soc_defip_index_remap)) {
            *mem = L3_DEFIP_PAIR_128m;
            return pindex - _KT_OFFSET_1K_ENTRIES;
        } else if ((pindex > _KT_INDEX_2K_ENTRIES) && 
        	         (pindex < _KT_OFFSET_2K_ENTRIES + soc_defip_index_remap)) {
        	  *mem = L3_DEFIP_PAIR_128m;
            return pindex - _KT_OFFSET_2K_ENTRIES + soc_defip_index_remap;
        } else if ((pindex > _KT_INDEX_3K_ENTRIES) && 
        	         (pindex < _KT_OFFSET_3K_ENTRIES + soc_defip_index_remap)) {
            *mem = L3_DEFIP_PAIR_128m;
            return pindex - _KT_OFFSET_3K_ENTRIES + soc_defip_index_remap;
        } else {
            *mem = L3_DEFIPm;
            return _soc_katana_l3_defip_urpf_index_remap(unit, 0, pindex);
        }
        
    } else {
        if (SOC_CONTROL(unit)->l3_defip_index_remap == 0) {
            *mem = L3_DEFIPm;
            return pindex;  
        }      
        return soc_l3_defip_index_mem_map(unit, pindex, mem);
    }
}

#define _SOC_KT_DEFIP_MAX_TCAMS   6
#define _SOC_KT_DEFIP_TCAM_DEPTH  1024
int
soc_kt_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
    soc_persist_t *sop = SOC_PERSIST(unit);
    switch (dev_id) {
    case BCM56240_DEVICE_ID:
    case BCM56241_DEVICE_ID:
    case BCM56242_DEVICE_ID:
    case BCM56243_DEVICE_ID:
    case BCM56245_DEVICE_ID:
    case BCM56246_DEVICE_ID:
        SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_KT_DEFIP_MAX_TCAMS/3;
        break;
    default:
        SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_KT_DEFIP_MAX_TCAMS;
        break;
    }
    SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_KT_DEFIP_TCAM_DEPTH;
    if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 0);

        num_ipv6_128b_entries =
            soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
                             (defip_config ? 1024 : 0));

        config_v6_entries = num_ipv6_128b_entries;

        sop->memState[L3_DEFIP_PAIR_128m].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                                          num_ipv6_128b_entries - 1;
        sop->memState[L3_DEFIPm].index_max =
                                 (SOC_CONTROL(unit)->l3_defip_max_tcams *
                                  SOC_CONTROL(unit)->l3_defip_tcam_size) -
                                 (num_ipv6_128b_entries * 2) - 1;
        sop->memState[L3_DEFIP_ONLYm].index_max =
                                      sop->memState[L3_DEFIPm].index_max;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                                      sop->memState[L3_DEFIPm].index_max;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                                      sop->memState[L3_DEFIPm].index_max;
        SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
        soc_l3_defip_indexes_init(unit, config_v6_entries);
    }
    return rv;
}


#endif /* BCM_KATANA_SUPPORT */
